/* Current source functions; movement, rendering, audio and AI services are
 * controlled doubles. These are transition checks, not an emulator playtest. */
#include <stdint.h>
#include <assert.h>
#define game_options_secrets_blocked() 0
#include <math.h>
#include "test_support.h"
typedef int8_t s8;
typedef uint8_t u8;
typedef int16_t s16;
typedef uint16_t u16;
typedef int32_t s32;
typedef int Bool32;
typedef uint32_t u32;
typedef uint64_t u64;
typedef float f32;
typedef f32 Vec3f[3];
typedef s16 Vec3s[3];
typedef f32 Mat4[4][4];
typedef int BehaviorScript;
static const BehaviorScript bhvTiltingBowserLavaPlatform[1] = { 301 };
#define REONUCAM
#define FLOOMBAS
#define SOUND_MOVING_ALMOST_DROWNING 100
#define SOUND_MARIO_UH_LONG_JUMP_LAND 99
#define TRUE 1
#define FALSE 0
#define UNUSED
#define COS73 0.29237169f
#define COS80 0.17364818f
#define CBUTTON_MASK 0x0f
#define U_CBUTTONS 0x08
#define D_CBUTTONS 0x04
#define L_CBUTTONS 0x02
#define R_CBUTTONS 0x01
#define R_TRIG 0x10
#define CUTSCENE_NONE 0
#define D_JPAD 0x20
#define WARP_OP_DEATH 1
#define MODEL_BOWLING_BALL 1
#define MODEL_MARIOS_CAP 2
#define MODEL_BUBBLE 3
#define MODEL_SMALL_WATER_SPLASH 4
#define MODEL_MIST 5
#define MODEL_WHITE_PARTICLE_SMALL 6
#define OBJECT_POOL_CAPACITY 240
#define PARTICLE_PRIORITY_FEEDBACK 7
#define SAVE_FLAG_CAP_ON_GROUND 1
#define SAVE_FLAG_CAP_ON_KLEPTO 2
#define SAVE_FLAG_CAP_ON_UKIKI 4
#define SAVE_FLAG_CAP_ON_MR_BLIZZARD 8
#define bzero(ptr, size) memset((ptr), 0, (size))
#define SOUND_OBJ_SNUFIT_SHOOT 1
#define SOUND_OBJ_SNUFIT_SKEETER_DEATH 2
#define SOUND_MARIO_ATTACKED 3
#define SOUND_MARIO_DROWNING 19
#define SOUND_ACTION_WATER_PLUNGE 22
#define SOUND_ACTION_FLYING_FAST 101
#define SOUND_OBJ_POUNDING_CANNON 102
#define SOUND_MOVING_AIM_CANNON 103
#define SOUND_MOVING_TERRAIN_SLIDE 26
#define SOUND_ACTION_SWIM 24
#define SOUND_ACTION_SWIM_OVER_PLUNGE 25
#define SOUND_MARIO_HAHA_WATER 23
#define SOUND_MARIO_WAAAOOOW 21
#define SOUND_MOVING_QUICKSAND_DEATH 20
#define SOUND_MARIO_OOOF2 4
#define SOUND_ACTION_TERRAIN_BODY_HIT_GROUND 5
#define SOUND_ACTION_WATER_JUMP 6
#define SOUND_MARIO_PUNCH_HOO 7
#define SOUND_ACTION_READ_SIGN 8
#define SOUND_OBJ_BOBOMB_WALK 9
#define SOUND_ACTION_THROW 10
#define SOUND_MARIO_HOOHOO 11
#define SOUND_MARIO_HAHA 12
#define SOUND_MENU_CAMERA_ZOOM_IN 0x220000
#define SOUND_MENU_CAMERA_ZOOM_OUT 0x220001
#define SOUND_MENU_CAMERA_BUZZ 0x220002
#define SOUND_ACTION_TERRAIN_JUMP 12
#define SOUND_ACTION_SPIN 13
#define SOUND_ACTION_TERRAIN_LANDING 14
#define SOUND_ACTION_TERRAIN_STEP 0x410000
#define SOUND_ACTION_METAL_STEP 0x410001
#define SOUND_ACTION_METAL_LANDING 0x410002
#define SOUND_ACTION_UNSTUCK_FROM_GROUND 0x410003
#define SOUND_ACTION_TWIRL 0x420000
#define SOUND_ACTION_BONK 15
#define SOUND_ACTION_METAL_BONK 16
#define SOUND_GENERAL_FLAME_OUT 18
#define SOUND_MOVING_LAVA_BURN 0x220000
#define MARIO_EYES_DEAD 8
#define SOUND_ACTION_HIT 17
#define SOUND_MARIO_PANTING 0x180000
#define SOUND_MARIO_COUGHING1 0x1b0000
#define SOUND_MARIO_COUGHING2 0x1b0001
#define SOUND_MARIO_COUGHING3 0x1b0002
#define SOUND_MARIO_YAWNING 0x1c0000
#define SOUND_MARIO_IMA_TIRED 0x1d0000
#define SOUND_MARIO_SNORING1 0x1e0000
#define SOUND_MARIO_SNORING2 0x1f0000
#define SOUND_MARIO_SNORING3 0x200000
#define SOUND_ACTION_PAT_BACK 0x210000
#define MARIO_EYES_HALF_CLOSED 1
#define MARIO_EYES_CLOSED 2
#define MARIO_EYES_BLINK 0
#define SOUNDARGS_MASK_PRIORITY 0x0000ff00
#define SOUNDARGS_SHIFT_PRIORITY 8
#define GRAB_POS_LIGHT_OBJ 1
#define A_BUTTON 0x8000
#define B_BUTTON 0x4000
#define L_TRIG 0x40
#define Z_TRIG 0x80
#define sqr(x) ((x) * (x))
#define DEGREES(x) ((s16) ((x) * 65536.0f / 360.0f))
#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define CLAMP(x, a, b) MIN(MAX(x, a), b)
#define FALL_THROUGH __attribute__((fallthrough))
#define FLOOR_LOWER_LIMIT -11000
#define FLOOR_LOWER_LIMIT_MISC -11000
#define MODEL_FISH 987
#include "fixes_definitions.inc"
struct Animation {
    s16 loopEnd, animYTransDivisor, startFrame;
    const u16 *index;
    const s16 *values;
};
static struct Animation sStopCrouchingAnimation = { .loopEnd = STOP_CROUCHING_ANIM_LENGTH };
static const u16 sHandstandJumpIndex[] = { 1, 0, 1, 1, 1, 0 };
static const s16 sHandstandJumpValues[] = { 0, 137 };
static struct Animation sHandstandJumpAnimation = {
    .loopEnd = 36,
    .animYTransDivisor = 189,
    .index = sHandstandJumpIndex,
    .values = sHandstandJumpValues,
};
struct AnimInfo {
    s16 animID, animFrame;
    struct Animation *curAnim;
    s16 animYTrans;
};
struct Object {
    const BehaviorScript *behavior;
    s32 model;
    struct {
        struct Object *next;
        struct {
            f32 scale[3], pos[3], cameraToObject[3];
            s16 angle[3];
            s16 areaIndex, activeAreaIndex;
            Mat4 *throwMatrix;
            struct {
                s16 flags;
            } node;
            struct AnimInfo animInfo;
        } gfx;
    } header;
    f32 oSpawnPos[3], oForwardVel;
    s32 oSpawnAngles[3];
    struct Object *parentObj;
    f32 oPosX, oPosY, oPosZ, oHomeX, oHomeY, oHomeZ, oWaveTrailSize;
    f32 oDistanceToMario, oSnufitScale, oWallHitboxRadius;
    f32 hitboxRadius, hurtboxRadius, hitboxDownOffset, hitboxHeight, oMarioPolePos;
    s32 oGoombaSize, oKoopaMovementType, oBooDeathStatus, oBooTargetOpacity;
    s32 oCannonIsActive, oCannonAngle, oCannonTimeSinceActivated;
    s32 oTimer, oSubAction, oAnimState, oOpacity, oBehParams2ndByte, oGroundTwirlStunDuration,
        oGroundTwirlStunTimer, oGroundTwirlContactRebound;
    s32 oSnufitCircularPeriod, oSnufitBodyScalePeriod, oSnufitBodyBaseScale;
    s32 oSnufitBodyScale, oSnufitRecoil, oSnufitBullets, oAction;
    s32 oMoveAngleRoll, oMoveAnglePitch, oMoveAngleYaw, oFaceAnglePitch, oFaceAngleYaw, oAngleToMario,
        oDeathSound, oBullySubtype;
    s32 oWaterObjScaleXAngle, oWaterObjScaleYAngle, oWaterObjScaleXAngleVel, oWaterObjScaleYAngleVel;
    u32 activeFlags, oActiveParticleFlags, oMoveFlags, oInteractStatus, oInteractionSubtype;
    s32 oMarioLongJumpIsSlow, oMarioWalkingPitch, oMarioSteepJumpYaw;
    s32 oMarioBurnTimer, oDamageOrCoinValue;
    s32 oMarioCannonObjectYaw, oMarioCannonInputYaw;
    s32 oCommonAnchorAction, oHeaveHoThrowState, oKingBobombPlayerGrabEscapeCooldown;
    u32 oInteractType, oFlags, metalContactCooldown, metalContactPendingDefeat;
    f32 oVelY, oGravity, oDragStrength;
    s32 oIntangibleTimer, oRoom, oHeldState;
    f32 oDrawingDistance;
    void *collisionData;
};
#define oPosVec oSpawnPos[0]
#define oHomeVec oHomeX
#define oMoveAngleVec oSpawnAngles[0]
struct Surface {
    s32 type, flags;
    struct {
        f32 x, y, z;
    } normal;
    struct Object *object;
};
static struct Surface gWaterSurfacePseudoFloor;
struct Camera {
    s32 mode, defMode, cutscene;
    s16 yaw;
};
struct TestPlayerCameraState {
    u8 cameraEvent;
    struct Object *usedObj;
};
struct Area {
    s32 terrainType;
    struct Camera *camera;
};
struct Controller {
    f32 stickMag, stickX, stickY;
    u16 buttonDown, buttonPressed;
};
struct MarioState;
void mario_set_wall_bonk_momentum(struct MarioState *m, f32 recoilSpeed, f32 minimumSpeed);
struct MarioState {
    u32 forcedCapFlags, naturalCapFlags;
    u8 landingJumpBufferedEdge;
    u8 objectRecoveryAFrames, objectRecoveryBFrames;
    u8 automaticAFrames, automaticBFrames, automaticZFrames, groundPoundInputFrames;

    u8 rolloutFrames;
    u8 waterWalkTwirl;
    u8 metalFallGroundPoundImpact;
    struct Object *metalFallGroundPoundFloorObject;
    u8 waterWalkCooldown;
    u8 waterWalkDisabled;
    u32 airTwirlSourceAction;
    s16 airTwirlStartAngle, airTwirlWallYaw, airTwirlCooldown;
    u8 airTwirlCooldownResetByCoin;
    u8 airTwirlInputFrames;
    u8 airTwirlJumpQueued;
    u8 airTwirlLPressed;
    u8 airTwirlFlightRollFrame;
    u8 airTwirlUsed, airTwirlWingResetUsed, airTwirlGravity, airTwirlContinue, airTwirlWallContact;
    u8 airTwirlWallBounceCount;
    u8 airTwirlWingControlTimer, wallTwirlSoundTimer;
    f32 airTwirlWingSpeedLimit;
    u8 wallBonkMomentum;
    const struct MarioMovementProfile *movementProfile;
    s32 backwardSlopeSlideCount, backwardButtSlideJumpCount, wallKickFromDoubleJump;
    f32 slopeJumpKickImpactSpeed;
    u32 action, prevAction, actionArg, actionState, flags, input, particleFlags;
    u32 collidedObjInteractTypes;
    u16 actionTimer;
    f32 intendedMag, forwardVel, slideVelX, slideVelZ, vel[3], pos[3], prevPos[3], floorHeight,
        ceilHeight;
    f32 peakHeight, quicksandDepth;
    s16 faceAngle[3], angleVel[3], twirlYaw, intendedYaw, floorYaw, ceilYaw, wallYaw, slideYaw, health,
        waterLevel;
    s32 wallKickChainCount, wallKickContactSoundPlayed, wallKickTimer, rolloutWallKickFrames,
        squishTimer, fastMetalFallSquish, framesSinceA, groundTakeoffThisFrame, windCatchActive;
    s32 landingJumpInputFrames, wallKickInputFrames, wallKickForwardStickFrames,
        wallKickBackwardStickFrames, burnCoinJumpPending, capPickupLockFrames;
    u32 capPickupLockStartFrame;
    s32 bullyKnockbackActive, bullyKnockbackBuffer, bullyKnockbackImmunityFrames, zCrouchBuffered,
        zBPressed;
    s32 heaveHoThrowPendingLanding, heaveHoThrowRecoveryPending, heaveHoLaunchImmunityFrames;
    u16 soakedTimer;
    u8 soakedThisUpdate, soakedWaterContact, soakedDripFrames, soakedFlameProtection;
    u8 crouchSlideNeutral, crouchSlideFrames, crouchSlideBackflip;
    u8 backwardLongJumpChain, backwardLongJumpCrouchFrames;
    s32 doubleJumpTimer, healCounter, respawnHealCounter, hurtCounter, invincTimer, breath,
        breathCounter, terrainSoundAddend;
    s32 numCoins;
    u8 yoshiCoinDeathPending;
    u8 poisonMushroomDeathPending;
    struct Object *marioObj, *heldObj, *riddenObj, *usedObj, *interactObj;
    struct TestPlayerCameraState *statusForCamera;
    struct Surface *floor, *ceil, *wall;
    f32 windGravity;
    u8 windColumnActive, windDeathGraceTimer;
    s32 zHeldFrames;
    struct TestBodyState {
        s16 headAngle[3];
        s32 grabPos, wingFlutter, punchState, eyeState, capState, handState;
        u16 modelState, capBlinkModelState;
        u8 capBlinkBlend;
        s8 capBlinkCapState, capBlinkHandState;
    } *marioBodyState;
    u8 fadeWarpOpacity;
    s32 framesSinceB, groundPoundBounceCooldown, spawnFallProtection, capTimer, waterGroundPoundImpact;
    struct Object *waterGroundPoundFloorObject;
    struct Area *area;
    struct Controller *controller;
};
static struct {
    s16 previousHealth;
    u16 drainProgress;
    u8 outOfWaterFrames;
    u8 shallowDeathPending;
} sWaterHealth;
static struct {
    u16 drainProgress, cleanAirFrames;
    u8 protected;
} sPoisonHealth;
static u16 sHealthRegenFrames;
static u16 sHealthRegenInactiveFrames;
static u8 sShallowRegenExposureFrames;
static struct {
    u16 underwaterFrames;
    u8 voiceFrames;
    u32 surfaceVoice, playingVoice;
} sSurfaceBreath;
static struct {
    f32 lastPos[3];
    s32 active;
} sSubmergedTrail;
static u16 sDialogueCameraBlockedButtons;
static s32 gDebugLevelSelect, gCurrLevelNum, gCurrAreaIndex;
static u32 gGlobalTimer;
static u8 gDebugFastWalk;
static s32 gCurrSaveFileNum = 3;
static s32 sHeThiccFile;
static s32 save_file_is_he_thicc(s32 file) {
    return file == 2 && sHeThiccFile;
}
static void *gCurrDemoInput, *gCurrCreditsEntry;
static struct Object *o, *gMarioObject;
static struct Object gObjectLists[NUM_OBJ_LISTS];
static struct {
    struct Object *next;
} gFreeObjectList;
static struct MarioState *gMarioState;
static struct Area *gCurrentArea;
static struct {
    s32 mode, defMode;
} gLakituState;
static struct {
    u16 rHoldFrames, rTapFrames;
} gReonucamState;
static s16 sSelectionFlags;
static f32 gGlobalSoundSource[3];
static u16 gCameraMovementFlags;
static s16 sCameraSoundFlags;
static s16 sHudCameraStatus;
static void game_timing_runtime_entered(void) {
}
static void hud_reveal_counters(void) {
}
static s32 sCamSelection = CAM_SELECTION_MARIO;
static struct Controller *gPlayer1Controller;
static const BehaviorScript bhvBalconyBigBoo[] = { 201 };
static const BehaviorScript bhvMerryGoRoundBigBoo[] = { 202 };
static const BehaviorScript bhvGhostHuntBigBoo[] = { 203 };
static const BehaviorScript bhvWigglerHead[] = { 204 };
static const BehaviorScript bhvKoopaShell[] = { 5 }, bhvKoopaShellUnderwater[] = { 6 };
static const BehaviorScript bhvGhostHuntBoo[] = { 7 }, bhvBoo[] = { 8 }, bhvMerryGoRoundBoo[] = { 9 };
static const BehaviorScript bhvBooWithCage[] = { 10 };
static s32 obj_has_behavior(struct Object *obj, const BehaviorScript *bhv) {
    return obj->behavior == bhv;
}

static struct {
    Vec3s startPos, startAngle;
} sSpawnInfo, *gMarioSpawnInfo = &sSpawnInfo;
static const BehaviorScript bhvNormalCap[] = { 2 };
static struct ObjectHitbox {
    int unused;
} sSnufitHitbox;
static int bhvSnufitBalls;
static int sDeleted, sStep, sAnimEnded, sDives, sWarped, sAirStepResult, sWallKickWindows, sAnimAccel;
static int sBonkSounds, sMetalBonkSounds, sLavaWallBoosts;
static int sCUpSounds, sCUpExitSounds, sCUpStores;
static int sCUpBlockedSounds;
static int sPlungeSounds, sSwimSounds, sSwimOverPlungeSounds;
static int sTrackRolloutGround, sRolloutGroundSteps, sRealSliding;
static int sThrowReleases;
static int sDabVoices, sTalkSounds, sFootsteps;
static int sBubbleSpawns, sBubbleSplashes;
static int sEffectParticleCount, sEffectSpawnLimit, sClearedParticleFlags;
static int sSoakedDripSpawns;
static f32 sSoakedDripScale;
static struct Object sSoakedDrip;
static u16 sTestRandomU16;
static struct Object sEffectParticles[16];
static struct Object sTestBubble, sTestSplash;
static const BehaviorScript bhvSmallWaterWave[] = { 7 }, bhvObjectWaterSplash[] = { 8 };
static const BehaviorScript bhvWaterDroplet[] = { 13 };
static const BehaviorScript bhvWaterDropletSplash[] = { 14 };
static const BehaviorScript bhvWhitePuffExplosion[] = { 9 };
static int sSurfaceVoices, sStoppedSurfaceVoices;
static u32 sIdleCancelAction, sIdleCancelMask;
static int sRealDabIdleCancels;
s32 ground_dive_idle_cancels(struct MarioState *m);
static int sHostileDamage, sHostileRebounds;
static int sDiveTrajectory;
static struct {
    s32 reboundHiddenTimer;
} sTwirlContact;
static const BehaviorScript bhvChuckya[] = { 1 };
static const BehaviorScript bhvKingBobomb[] = { 79 }, bhvHeaveHo[] = { 80 };
static const BehaviorScript bhvKoopa[] = { 12 };
static const BehaviorScript bhvThwomp[] = { 77 }, bhvThwomp2[] = { 78 };
static const BehaviorScript bhvGoomba[] = { 10 }, bhvFloomba[] = { 11 }, bhvPiranhaPlant[] = { 12 };
static f32 sWaterLevel;
static f32 absf(f32 x) {
    return fabsf(x);
}
static s16 abs_angle_diff(s16 a, s16 b) {
    s16 d = b - a;
    if (d == -0x8000) {
        return 0x7fff;
    }
    return d < 0 ? -d : d;
}
static f32 sins(s16 angle) {
    return sinf(angle * (6.28318530718f / 65536));
}
static f32 coss(s16 angle) {
    return cosf(angle * (6.28318530718f / 65536));
}
static s16 atan2s(f32 z, f32 x) {
    return (s16) (atan2f(x, z) * (65536 / 6.28318530718f));
}
#define vec3_zero(v) memset((v), 0, 3 * sizeof((v)[0]))
static void vec3f_set(f32 *v, f32 x, f32 y, f32 z) {
    v[0] = x;
    v[1] = y;
    v[2] = z;
}
static void vec3f_copy(f32 *dest, f32 *src) {
    memcpy(dest, src, 3 * sizeof(f32));
}
static s32 sRealMovingEntry;
static s32 sRealCutsceneEntry;
u32 real_set_mario_action_moving(struct MarioState *m, u32 a, u32 arg);
void real_mario_set_forward_vel(struct MarioState *m, f32 v);
static void mario_set_forward_vel(struct MarioState *m, f32 v) {
    if (sRealMovingEntry) {
        real_mario_set_forward_vel(m, v);
        return;
    }
    m->forwardVel = v;
    m->vel[0] = v * sins(m->faceAngle[1]);
    m->vel[2] = v * coss(m->faceAngle[1]);
}
u32 set_mario_action(struct MarioState *m, u32 action, u32 arg);
s32 act_freefall_land_stop(struct MarioState *m);
s32 check_common_stationary_cancels(struct MarioState *m);
s32 mario_is_head_underwater(struct MarioState *m);
s32 mario_is_on_floor(struct MarioState *m);
s32 mario_floor_is_slope(struct MarioState *m);
static s32 set_jumping_action(struct MarioState *m, u32 a, u32 arg) {
    return set_mario_action(m, a, arg);
}
static s32 drop_and_set_mario_action(struct MarioState *m, u32 a, u32 arg) {
    m->heldObj = NULL;
    return set_mario_action(m, a, arg);
}
#define ACTION_SETUP(name)                                                                             \
    static u32 name(struct MarioState *m, u32 a, u32 arg) {                                            \
        return a;                                                                                      \
    }
static u32 set_mario_action_moving(struct MarioState *m, u32 a, u32 arg) {
    return sRealMovingEntry ? real_set_mario_action_moving(m, a, arg) : a;
}
u32 set_mario_action_submerged(struct MarioState *m, u32 action, u32 actionArg);
u32 real_set_mario_action_cutscene(struct MarioState *m, u32 action, u32 actionArg);
static u32 set_mario_action_cutscene(struct MarioState *m, u32 action, u32 actionArg) {
    return sRealCutsceneEntry ? real_set_mario_action_cutscene(m, action, actionArg) : action;
}
static void mario_ground_twirl_reset_wall_bounces(void) {
}
static void mario_ground_twirl_reset_cooldown(void) {
}
static void mario_reset_punch_sequence(void) {
}

static f32 get_additive_y_vel_for_jumps(void) {
    return 0;
}
static void set_camera_mode(struct Camera *c, s32 a, s32 b) {
}
static void store_lakitu_cam_info_for_c_up(struct Camera *c) {
    sCUpStores++;
}
static s32 cam_select_alt_mode(s32 mode) {
    return sCamSelection;
}
static s32 set_cam_angle(s32 mode) {
    return (sSelectionFlags & CAM_MODE_MARIO_ACTIVE) ? CAM_ANGLE_MARIO : CAM_ANGLE_LAKITU;
}
static void set_hud_camera_status(s16 status) {
    sHudCameraStatus = status;
}
static s32 mario_has_dive_input(struct MarioState *m) {
    return sDives;
}
static int sShellMusicStops;
static void stop_shell_music(void) {
    sShellMusicStops++;
}
static void update_surface_breath_sound(struct MarioState *m);
static void play_sound_with_rumble(u32 sound, f32 *position, s32 strength, s32 duration) {
    sSurfaceVoices++;
}
static void stop_sound(u32 sound, f32 *position) {
    sStoppedSurfaceVoices++;
}
static void update_low_health_warning(struct MarioState *m) {
}
static void update_mario_sound_and_camera(struct MarioState *m) {
}
static void load_level_init_text(s32 arg) {
}
static s32 mario_push_off_steep_floor(struct MarioState *m, u32 a, u32 arg) {
    return set_mario_action(m, a, arg);
}
static s32 sRealSlideEligibility;
s32 real_should_begin_sliding(struct MarioState *m);
static s32 should_begin_sliding(struct MarioState *m) {
    return sRealSlideEligibility ? real_should_begin_sliding(m) : FALSE;
}
static s32 set_jump_from_landing(struct MarioState *m) {
    return set_mario_action(m, ACT_DOUBLE_JUMP, 0);
}
s32 real_set_jump_from_landing(struct MarioState *m);
s32 real_set_jumping_action(struct MarioState *m, u32 action, u32 arg);
static void set_steep_jump_action(struct MarioState *m) {
    set_mario_action(m, ACT_STEEP_JUMP, 0);
}
static s32 check_common_action_exits(struct MarioState *m) {
    return set_mario_action(m, ACT_WALKING, 0);
}
void play_sound_if_no_flag(struct MarioState *m, u32 sound, u32 flags);
static u32 sLastMarioSound;
static s32 sMarioSoundCount;
static void play_sound(u32 sound, f32 *position) {
    sLastMarioSound = sound;
    sMarioSoundCount++;
    if (sound == SOUND_ACTION_WATER_PLUNGE) {
        sPlungeSounds++;
    }
    if (sound == SOUND_ACTION_SWIM) {
        sSwimSounds++;
    }
    if (sound == SOUND_ACTION_SWIM_OVER_PLUNGE) {
        sSwimOverPlungeSounds++;
    }
    if (sound == SOUND_MENU_CAMERA_ZOOM_IN) {
        sCUpSounds++;
    }
    if (sound == SOUND_MENU_CAMERA_ZOOM_OUT) {
        sCUpExitSounds++;
    }
    if (sound == SOUND_MENU_CAMERA_BUZZ) {
        sCUpBlockedSounds++;
    }
    if (sound == SOUND_MARIO_PUNCH_HOO) {
        sDabVoices++;
    }
    if (sound == SOUND_ACTION_BONK || sound == SOUND_ACTION_METAL_BONK) {
        sBonkSounds++;
    }
    if (sound == SOUND_ACTION_METAL_BONK) {
        sMetalBonkSounds++;
    }
}
static void queue_rumble_data(s32 a, s32 b) {
}
static void mario_stop_riding_and_holding(struct MarioState *m) {
    m->heldObj = NULL;
    m->riddenObj = NULL;
}
static void level_trigger_warp(struct MarioState *m, s32 a) {
    sWarped++;
}
static s32 perform_ground_step(struct MarioState *m) {
    if (sTrackRolloutGround) {
        sRolloutGroundSteps++;
        if (sStep != GROUND_STEP_HIT_WALL) {
            m->pos[0] += m->vel[0];
            m->pos[2] += m->vel[2];
        }
    }
    return sStep;
}
static s32 stationary_ground_step(struct MarioState *m) {
    if (sTrackRolloutGround) {
        mario_set_forward_vel(m, 0.0f);
    }
    return sStep;
}
static void mario_throw_held_object(struct MarioState *m) {
    if (m->heldObj != NULL) {
        sThrowReleases++;
        m->heldObj = NULL;
    }
}
static struct {
    f32 surfaceExitVelocityY;
    u32 swimButton;
    u16 strokeCooldown, punchCooldown, bTapFrames, waterDownImpactCooldown;
    u8 waterDownRepeatFrames;
    u8 strokeQueued, strokeBuffered, entryTapFrames, entryTapPending, entryStrokeReady;
    u8 bTapPending, punchQueued, surfaceSplashPlayed;
    s16 wallSlideYaw;
    u8 wallSlideActive, velocityPrepared, rollFrame, rollCooldown, rollFloorContact;
    s32 rollStartAngle;
    s16 rollWallYaw;
    u8 rollWallContact;
    u8 rollBounceCount;
} sSwimming;
static s16 sWasAtSurface;
static s16 sBobIncrement;
static s32 sWaterSteps;
static s32 swimming_near_surface(struct MarioState *m) {
    return FALSE;
}
static void stationary_slow_down(struct MarioState *m, f32 drag, f32 gravity, f32 buoyancy) {
}
static u32 perform_water_step(struct MarioState *m) {
    sWaterSteps++;
    return WATER_STEP_NONE;
}
static u32 mario_update_moving_sand(struct MarioState *m) {
    return FALSE;
}
static u32 mario_update_windy_ground(struct MarioState *m) {
    return FALSE;
}
static void stop_and_set_height_to_floor(struct MarioState *m) {
    m->pos[1] = m->floorHeight;
}
static s16 set_mario_animation(struct MarioState *m, s32 a) {
    if (a == MARIO_ANIM_STOP_CROUCHING) {
        m->marioObj->header.gfx.animInfo.curAnim = &sStopCrouchingAnimation;
    }
    if (a == MARIO_ANIM_HANDSTAND_JUMP) {
        m->marioObj->header.gfx.animInfo.curAnim = &sHandstandJumpAnimation;
        m->marioObj->header.gfx.animInfo.animYTrans = 189;
    }
    if (m->marioObj->header.gfx.animInfo.animID != a) {
        m->marioObj->header.gfx.animInfo.animID = a;
        m->marioObj->header.gfx.animInfo.animFrame = 0;
    }
    return m->marioObj->header.gfx.animInfo.animFrame;
}
static void set_anim_to_frame(struct MarioState *m, s32 frame) {
    m->marioObj->header.gfx.animInfo.animFrame = frame;
}
static s32 check_common_idle_cancels(struct MarioState *m) {
    if (sRealDabIdleCancels) {
        return ground_dive_idle_cancels(m);
    }
    return sIdleCancelAction && (!sIdleCancelMask || (m->input & sIdleCancelMask))
               ? set_mario_action(m, sIdleCancelAction, 0)
               : FALSE;
}
static s32 apply_slope_decel(struct MarioState *m, f32 amount) {
    return FALSE;
}
static void slide_bonk(struct MarioState *m, u32 air, u32 ground) {
    set_mario_action(m, ground, 0);
}
static s16 set_mario_anim_with_accel(struct MarioState *m, s32 a, s32 b) {
    sAnimAccel = b;
    return set_mario_animation(m, a);
}
s32 update_decelerating_speed(struct MarioState *m);
s32 real_update_sliding(struct MarioState *m, f32 stopSpeed);
static s32 update_sliding(struct MarioState *m, f32 stopSpeed) {
    return sRealSliding ? real_update_sliding(m, stopSpeed) : FALSE;
}
static void update_walking_speed(struct MarioState *m) {
}
static s32 check_ledge_climb_down(struct MarioState *m, s32 allowFast) {
    return FALSE;
}
static s32 castle_secret_lobby_active(void) {
    return FALSE;
}
static void align_with_floor(struct MarioState *m) {
}
static void play_step_sound(struct MarioState *m, s16 frame1, s16 frame2) {
}
static void reset_rumble_timers_slip(void) {
}
static void reset_rumble_timers_vibrate(s32 value) {
}
static void anim_and_audio_for_heavy_walk(struct MarioState *m) {
}
static void mario_attack_breakable_box_wall(struct MarioState *m) {
}
static void common_slide_action(struct MarioState *m, u32 stop, u32 air, s32 animation) {
}
static void play_mario_landing_sound_once(struct MarioState *m, u32 sound) {
}
static s32 sRealAnimationEnds;
s32 real_is_anim_at_end(struct MarioState *m);
s32 real_is_anim_past_end(struct MarioState *m);
static s32 is_anim_at_end(struct MarioState *m) {
    return sRealAnimationEnds ? real_is_anim_at_end(m) : sAnimEnded;
}
static s32 is_anim_past_end(struct MarioState *m) {
    return sRealAnimationEnds ? real_is_anim_past_end(m) : sAnimEnded;
}
static s32 sRealBonkRecovery;
s32 real_apply_landing_accel(struct MarioState *m, f32 friction);
static void apply_landing_accel(struct MarioState *m, f32 friction) {
    if (sRealBonkRecovery) {
        real_apply_landing_accel(m, friction);
    }
}
static void play_mario_heavy_landing_sound_once(struct MarioState *m, s32 a) {
}
static void play_mario_heavy_landing_sound(struct MarioState *m, s32 a) {
}
static void lower_background_noise(s32 amount) {
}
static s32 sWakeNoiseRestores;
static void raise_background_noise(s32 amount) {
    sWakeNoiseRestores++;
}

static void play_mario_sound(struct MarioState *m, s32 a, s32 b) {
    m->flags |= MARIO_ACTION_SOUND_PLAYED;
}
static void play_mario_landing_sound(struct MarioState *m, s32 a) {
}
static s32 sBonkFallDamage;
void mario_play_wall_bonk_sound(struct MarioState *m);
void mario_play_air_twirl_sound(struct MarioState *m);
s32 check_fall_damage_or_get_stuck(struct MarioState *m, u32 action) {
    if (sBonkFallDamage) {
        set_mario_action(m, action, 1);
        return TRUE;
    }
    return FALSE;
}
static void begin_wall_kick_window(struct MarioState *m) {
    sWallKickWindows++;
}
void apply_gravity(struct MarioState *m);
static u32 sLastAirStepArg;
static s32 perform_air_step(struct MarioState *m, s32 a) {
    sLastAirStepArg = a;
    if (sAirStepResult >= 0) {
        if (sAirStepResult == AIR_STEP_GRABBED_LEDGE) {
            m->vel[1] -= 4.0f;
        }
        return sAirStepResult;
    }
    if (!sDiveTrajectory) {
        return AIR_STEP_NONE;
    }
    for (int axis = 0; axis < 3; axis++) {
        m->pos[axis] += m->vel[axis];
    }
    s32 result = AIR_STEP_NONE;
    if (m->pos[1] <= m->floorHeight) {
        m->pos[1] = m->floorHeight;
        result = AIR_STEP_LANDED;
    }
    apply_gravity(m);
    return result;
}
static s32 common_air_action_step(struct MarioState *m, u32 a, s32 anim, u32 checks) {
    return perform_air_step(m, checks);
}
static s32 sDiveGrabAge;
static s32 mario_check_object_grab(struct MarioState *m) {
    sDiveGrabAge = m->actionState;
    return FALSE;
}
static void mario_grab_used_object(struct MarioState *m) {
}
static void update_air_without_turn(struct MarioState *m) {
}
s32 should_get_stuck_in_ground(struct MarioState *m);
static s32 check_fall_damage(struct MarioState *m, u32 a) {
    return FALSE;
}
static void mario_drop_held_object(struct MarioState *m) {
    m->heldObj = NULL;
}
static s32 mario_ground_twirl_is_active(struct MarioState *m) {
    return FALSE;
}
static void finish_air_cap_pickup(struct MarioState *m) {
}
static s32 set_water_plunge_action(struct MarioState *m) {
    return set_mario_action(m, ACT_WATER_PLUNGE, 0);
}
static s32 mario_is_riding_underwater_shell(struct MarioState *m) {
    return FALSE;
}
static s32 mario_transition_underwater_shell_to_swimming(struct MarioState *m) {
    return FALSE;
}
static void water_jump_hit_wall(struct MarioState *m) {
}
static void lava_boost_on_wall(struct MarioState *m) {
    sLavaWallBoosts++;
}
static f32 find_water_level(f32 x, f32 z) {
    return sWaterLevel;
}
static void obj_copy_pos(struct Object *dst, struct Object *src) {
    dst->oPosX = src->oPosX;
    dst->oPosY = src->oPosY;
    dst->oPosZ = src->oPosZ;
}
static struct Object *try_to_spawn_object(s16 offsetY, f32 scale, struct Object *parent, s32 model,
                                          const BehaviorScript *behavior) {
    if (sEffectParticleCount >= sEffectSpawnLimit) {
        return NULL;
    }
    struct Object *particle = &sEffectParticles[sEffectParticleCount++];
    memset(particle, 0, sizeof(*particle));
    particle->model = model;
    particle->behavior = behavior;
    particle->parentObj = parent;
    obj_copy_pos(particle, parent);
    particle->oPosY += offsetY;
    particle->header.gfx.scale[0] = particle->header.gfx.scale[1] = particle->header.gfx.scale[2] =
        scale;
    return particle;
}
static struct Object *spawn_object(struct Object *parent, s32 model, const BehaviorScript *behavior);
static struct Object *try_spawn_particle(struct Object *parent, s32 model,
                                         const BehaviorScript *behavior, s32 priority) {
    if (behavior == bhvSmallWaterWave) {
        return sEffectSpawnLimit > 0 ? spawn_object(parent, model, behavior) : NULL;
    }
    return try_to_spawn_object(0, 1.0f, parent, model, behavior);
}
static void obj_scale(struct Object *object, f32 scale) {
    object->header.gfx.scale[0] = object->header.gfx.scale[1] = object->header.gfx.scale[2] = scale;
}
static void cur_obj_clear_particle_parent_flags(u32 flags) {
    sClearedParticleFlags = flags;
    o->parentObj->oActiveParticleFlags &= ~flags;
}
static void obj_mark_for_deletion(struct Object *obj) {
    sDeleted++;
}
static void cur_obj_scale(f32 scale) {
    o->header.gfx.scale[0] = o->header.gfx.scale[2] = scale;
}
static s16 approach_s16_symmetric(s16 v, s16 target, s16 step) {
    return v < target ? MIN(v + step, target) : MAX(v - step, target);
}
static void cur_obj_play_sound_2(s32 sound) {
    if (sound == SOUND_ACTION_READ_SIGN) {
        sTalkSounds++;
    }
    if (sound == SOUND_OBJ_BOBOMB_WALK) {
        sFootsteps++;
    }
}
static void spawn_object_relative(s32 a, s32 b, s32 c, s32 d, struct Object *e, s32 f, int g) {
}
static void obj_turn_pitch_toward_mario(f32 a, s32 b) {
}
static void cur_obj_rotate_yaw_toward(s32 a, s32 b) {
}
static void obj_move_pitch_approach(s32 a, s32 b) {
}
static void obj_check_attacks(struct ObjectHitbox *h, s32 a) {
}
static void *segmented_to_virtual(const void *ptr) {
    return (void *) ptr;
}
static s32 sVisibilityEnemy = TRUE;
static s32 mario_ground_twirl_is_enemy(struct Object *obj) {
    return sVisibilityEnemy;
}
static void cur_obj_enable_rendering_in_room(void) {
    o->header.gfx.node.flags |= GRAPH_RENDER_ACTIVE;
    o->activeFlags &= ~ACTIVE_FLAG_IN_DIFFERENT_ROOM;
}
static void cur_obj_disable_rendering_in_room(void) {
    o->header.gfx.node.flags &= ~GRAPH_RENDER_ACTIVE;
    o->activeFlags |= ACTIVE_FLAG_IN_DIFFERENT_ROOM;
}
static s32 mario_ground_twirl_is_immune_enemy(struct Object *obj) {
    return obj->behavior == bhvPiranhaPlant;
}
static void mario_ground_twirl_damage_capless_contact(struct MarioState *m) {
    sHostileDamage++;
}
static s32 object_facing_mario(struct MarioState *m, struct Object *obj, s16 a) {
    return FALSE;
}
static s32 check_object_grab_mario(struct MarioState *m, u32 a, struct Object *obj) {
    return FALSE;
}
static s32 mario_ground_twirl_rebounds_from_object(struct Object *obj, u32 a) {
    return TRUE;
}
static s32 mario_ground_twirl_contact_can_damage(struct Object *obj) {
    return TRUE;
}
static void mario_rebound_from_twirl_object(struct MarioState *m, struct Object *obj, f32 speed) {
    sHostileRebounds++;
}
static void mario_ground_twirl_destroy_projectile(struct Object *obj) {
}
// Air contact policy is covered by the twirl fixture; this fixture tests its callers.
static enum DefensiveContactResult
mario_handle_air_twirl_contact(struct MarioState *m, struct Object *obj, u32 interactType) {
    return DEFENSIVE_CONTACT_NORMAL;
}
static s32 mario_ground_twirl_is_oversized_enemy(struct Object *obj) {
    return FALSE;
}
static u32 determine_interaction(struct MarioState *m, struct Object *obj) {
    return INT_NONE;
}
static void mario_launch_stunned_enemy(struct MarioState *m, struct Object *obj, f32 speed, u32 id) {
    sHostileRebounds++;
    obj->oGroundTwirlStunTimer = -30;
}
static s32 mario_ground_twirl_is_boo(struct Object *obj) {
    return FALSE;
}
static s32 able_to_grab_object(struct MarioState *m, struct Object *obj) {
    return FALSE;
}
void mario_metal_stun_enemy_on_contact(struct MarioState *m, struct Object *obj);
static s32 stunned_enemy_can_interact(struct MarioState *m, struct Object *obj, u32 type);
static s32 common_landing_action(struct MarioState *m, s32 anim, u32 air) {
    return sStep;
}
static struct Surface *sObjFloor;
static s16 object_step(void) {
    return OBJ_COL_FLAGS_LANDED;
}
static void bully_backup_check(s16 flags) {
}
static void bully_play_stomping_sound(void) {
}
/* Spawn services are doubles; init_mario and both crush/death actions below
 * are production functions. No area loader or renderer runs in this harness. */
static void mario_ground_twirl_reset_rebound_invulnerability(void) {
}
static void mario_reset_knockback_source(void) {
}
static void update_swim_button_priority(struct MarioState *m) {
}
static void update_initial_a_tap(struct MarioState *m) {
    sSwimming.strokeQueued = FALSE;
}
static s32 clear_interrupted_swimming_controls(struct MarioState *m) {
    return FALSE;
}
static void mario_reset_attack_hit_record(void) {
}
static f32 sDizzyFloorBehind;
static s32 sDizzyCapBlows;
static f32 find_floor_height_relative_polar(struct MarioState *m, s16 yaw, f32 dist) {
    return sDizzyFloorBehind;
}
static void mario_blow_off_cap(struct MarioState *m, f32 speed) {
    if ((m->flags & (MARIO_CAPS | MARIO_CAP_ON_HEAD)) == (MARIO_NORMAL_CAP | MARIO_CAP_ON_HEAD)) {
        sDizzyCapBlows++;
        m->flags &= ~(MARIO_NORMAL_CAP | MARIO_CAP_ON_HEAD);
    }
}
static void mario_swimming_reset(void) {
}
static void mario_transition_pose_reset(void) {
}
static void reset_water_health_and_breath(void) {
}
static s32 sSavedCapPresent, sCapSpawnCount, sSavedCapWrites, sCapMusicStops;
static u32 sSavedCapOwnershipFlags;
static u32 save_file_get_flags(void) {
    return sSavedCapPresent ? SAVE_FLAG_CAP_ON_GROUND : 0;
}
static Vec3s sSavedCapPos = { 120, 30, -50 };
static struct Object sCapObjects[3];
static void save_file_set_cap_pos(s16 x, s16 y, s16 z) {
    sSavedCapPresent = TRUE;
    sSavedCapWrites++;
    sSavedCapPos[0] = x;
    sSavedCapPos[1] = y;
    sSavedCapPos[2] = z;
}
static void save_file_set_flags(u32 flags) {
    sSavedCapOwnershipFlags |= flags;
}
static void save_file_clear_flags(u32 flags) {
}
static void stop_cap_music(void) {
    sCapMusicStops++;
}
static s32 sCapMusicFades;
static void fadeout_cap_music(void) {
    sCapMusicFades++;
}
void mario_lose_cap_to_shallow_water_death(struct MarioState *m);
static s32 save_file_get_cap_pos(Vec3s p) {
    if (!sSavedCapPresent) {
        return FALSE;
    }
    memcpy(p, sSavedCapPos, sizeof(sSavedCapPos));
    return TRUE;
}
static void vec3s_copy(s16 *d, const s16 *s) {
    memcpy(d, s, 3 * sizeof(s16));
}
static void vec3s_to_vec3f(f32 *d, const s16 *s) {
    for (int i = 0; i < 3; i++) {
        d[i] = s[i];
    }
}
static void vec3s_to_vec3i(s32 *d, const s16 *s) {
    for (int i = 0; i < 3; i++) {
        d[i] = s[i];
    }
}
static void vec3s_set(s16 *d, s16 x, s16 y, s16 z) {
    d[0] = x;
    d[1] = y;
    d[2] = z;
}
static f32 find_floor(f32 x, f32 y, f32 z, struct Surface **f) {
    static struct Surface spawnFloor = { .normal.y = 1 };
    *f = &spawnFloor;
    return 0;
}
static void mario_reset_bodystate(struct MarioState *m) {
}
static u32 real_update_and_return_cap_flags(struct MarioState *m);
static s32 sRealCapUpdate;
static u32 update_and_return_cap_flags(struct MarioState *m) {
    return sRealCapUpdate ? real_update_and_return_cap_flags(m) : m->flags;
}
static void update_mario_info_for_cam(struct MarioState *m) {
}
static struct Object *spawn_object(struct Object *parent, s32 model, const BehaviorScript *behavior) {
    if (behavior == bhvNormalCap) {
        if (sCapSpawnCount >= 3) {
            fprintf(stderr, "Too many cap spawns\n");
            exit(2);
        }
        struct Object *cap = &sCapObjects[sCapSpawnCount++];
        memset(cap, 0, sizeof(*cap));
        cap->behavior = behavior;
        cap->activeFlags = 1;
        cap->header.gfx.activeAreaIndex = parent->header.gfx.areaIndex;
        struct Object *head = &gObjectLists[OBJ_LIST_LEVEL];
        cap->header.next = head->header.next;
        head->header.next = cap;
        return cap;
    }
    if (behavior == bhvSmallWaterWave) {
        sBubbleSpawns++;
        memset(&sTestBubble, 0, sizeof(sTestBubble));
        sTestBubble.parentObj = parent;
        obj_copy_pos(&sTestBubble, parent);
        return &sTestBubble;
    }
    if (behavior == bhvObjectWaterSplash) {
        sBubbleSplashes++;
        return &sTestSplash;
    }
    fprintf(stderr, "Unexpected cap spawn in respawn fixture\n");
    exit(2);
}
static struct Object *spawn_object_with_scale(struct Object *parent, s32 model,
                                              const BehaviorScript *behavior, f32 scale) {
    if (behavior != bhvWaterDroplet || model != MODEL_WHITE_PARTICLE_SMALL) {
        fprintf(stderr, "Unexpected soaked drip spawn\n");
        exit(2);
    }
    sSoakedDripSpawns++;
    memset(&sSoakedDrip, 0, sizeof(sSoakedDrip));
    sSoakedDrip.parentObj = parent;
    sSoakedDrip.model = model;
    sSoakedDripScale = scale;
    return &sSoakedDrip;
}
static f32 random_f32_around_zero(f32 diameter) {
    return 0.0f;
}
static f32 random_float(void) {
    return 0.0f;
}
static s32 cur_obj_has_model(s32 model) {
    return o->model == model;
}
static u16 random_u16(void) {
    return sTestRandomU16;
}
void mario_extinguish_fire(struct MarioState *m);
static void mario_he_thicc_contact_shake(struct MarioState *m) {
}
#define MarioBodyState TestBodyState
// Pole placement uses native attachment code; geometry and animation root
// translation are supplied services, with a controlled render clock.
static s32 sPoleWalls, sPoleLeaves, sPolePositionChecks;
static f32 sPoleCeiling;
static s16 sPoleRootY;
static s32 sSnufitWallAxes, sSnufitUnreportedWallPush;
static f32 sSnufitWallLimit;
static void vec3f_copy_y_off(f32 *dest, f32 *src, f32 offset) {
    vec3f_copy(dest, src);
    dest[1] += offset;
}
static s32 f32_find_wall_collision(f32 *x, f32 *y, f32 *z, f32 offset, f32 radius) {
    sPolePositionChecks++;
    if (sSnufitWallAxes != 0) {
        s32 contacts = 0;
        if ((sSnufitWallAxes & 1) && *x > sSnufitWallLimit - radius) {
            assert(*x < sSnufitWallLimit + radius);
            *x = sSnufitWallLimit - radius;
            contacts++;
        }
        if ((sSnufitWallAxes & 2) && *z > sSnufitWallLimit - radius) {
            assert(*z < sSnufitWallLimit + radius);
            *z = sSnufitWallLimit - radius;
            contacts++;
        }
        // Triangle edges can separate an object without supplying a wall hit.
        return sSnufitUnreportedWallPush ? 0 : contacts;
    }
    return sPoleWalls;
}
static f32 find_mario_ceil(f32 *pos, f32 height, struct Surface **ceil) {
    *ceil = NULL;
    return sPoleCeiling;
}
static s16 return_mario_anim_y_translation(struct MarioState *m) {
    return sPoleRootY;
}
static void add_tree_leaf_particles(struct MarioState *m) {
    sPoleLeaves++;
}
static s32 geo_update_animation_frame(struct AnimInfo *info, s32 *assist) {
    return info->animFrame;
}
// The castle window callback is covered by the rooftop encounter fixture.
static void castle_secret_window_bonk(struct MarioState *m) {
    (void) m;
}

static s32 gMarioShotFromCannon;
static void cur_obj_become_intangible(void) {
    o->oIntangibleTimer = -1;
}
static void cur_obj_become_tangible(void) {
    o->oIntangibleTimer = 0;
}
static void cur_obj_disable_rendering(void) {
    o->header.gfx.node.flags &= ~GRAPH_RENDER_ACTIVE;
}
static void cur_obj_enable_rendering(void) {
    o->header.gfx.node.flags |= GRAPH_RENDER_ACTIVE;
}

#include "fixes_sources.inc"
#define NEAR(actual, expected)                                                                         \
    do {                                                                                               \
        f32 got = (actual), want = (expected);                                                         \
        if (!(fabsf(got - want) <= 0.002f)) {                                                          \
            fprintf(stderr, "%s:%d: %s: %s expected %.5f observed %.5f\n", __FILE__, __LINE__,         \
                    sCurrentCase, #actual, want, got);                                                 \
            return 0;                                                                                  \
        }                                                                                              \
    } while (0)
static struct Object sMarioObject, sHeld, sObj;
static struct Surface sFloor;
static struct Camera sCamera;
static struct TestPlayerCameraState sPlayerCamera;
static struct Area sArea;
static struct Controller sController;
static struct MarioState m;
static void begin(u32 action) {
    sRealBonkRecovery = sBonkFallDamage = FALSE;
    sSnufitUnreportedWallPush = FALSE;
    sPoleWalls = sPoleLeaves = sPolePositionChecks = 0;
    sPoleCeiling = 10000;
    sPoleRootY = 0;
    sRealMovingEntry = FALSE;
    sRealCutsceneEntry = FALSE;
    sRealSlideEligibility = FALSE;
    memset(&m, 0, sizeof(m));
    memset(&sMarioObject, 0, sizeof(sMarioObject));
    memset(&sObj, 0, sizeof(sObj));
    sHeThiccFile = FALSE;
    mario_character_init();
    mario_character_commit(&m, FALSE);
    gCurrDemoInput = gCurrCreditsEntry = NULL;
    gDebugFastWalk = FALSE;
    sFloor = (struct Surface){ .normal.y = 1 };
    sArea = (struct Area){ .camera = &sCamera };
    memset(&sController, 0, sizeof(sController));
    sIdleCancelAction = sIdleCancelMask = 0;
    sRealDabIdleCancels = FALSE;
    sDabVoices = sTalkSounds = sFootsteps = 0;
    sPlungeSounds = sSwimSounds = sSwimOverPlungeSounds = 0;
    sBubbleSpawns = sBubbleSplashes = 0;
    gFreeObjectList.next = &sTestBubble;
    memset(&sGroundTwirl, 0, sizeof(sGroundTwirl));
    mario_spin_dizzy_reset();
    sDizzyCapBlows = 0;
    sDizzyFloorBehind = 0;
    sSavedCapPresent = sCapSpawnCount = sSavedCapWrites = sCapMusicStops = 0;
    sSavedCapPos[0] = 120;
    sSavedCapPos[1] = 30;
    sSavedCapPos[2] = -50;
    for (int list = 0; list < NUM_OBJ_LISTS; list++) {
        gObjectLists[list].header.next = &gObjectLists[list];
    }
    sEffectParticleCount = 0;
    sEffectSpawnLimit = 16;
    sClearedParticleFlags = 0;
    sSoakedDripSpawns = 0;
    sSoakedDripScale = 0;
    sTestRandomU16 = 0;
    sCamera.yaw = 0;
    sSurfaceVoices = sStoppedSurfaceVoices = 0;
    memset(&sSurfaceBreath, 0, sizeof(sSurfaceBreath));
    sAnimAccel = 0;
    memset(&sSwimming, 0, sizeof(sSwimming));
    sBobIncrement = sWaterSteps = 0;
    sDiveTrajectory = 0;
    mario_dab_reset();
    memset(&sSubmergedTrail, 0, sizeof(sSubmergedTrail));
    m.action = action;
    m.marioObj = &sMarioObject;
    m.statusForCamera = &sPlayerCamera;
    m.floor = &sFloor;
    m.area = &sArea;
    m.controller = &sController;
    m.health = 0x880;
    m.waterLevel = 0;
    m.flags = MARIO_CAP_ON_HEAD;
    m.ceilHeight = 300;
    sRealAnimationEnds = FALSE;
    sAnimEnded = sWarped = sDeleted = sDives = sWallKickWindows = 0;
    sBonkSounds = sMetalBonkSounds = sLavaWallBoosts = 0;
    sStep = GROUND_STEP_NONE;
    sAirStepResult = -1;
    sTrackRolloutGround = sRolloutGroundSteps = sRealSliding = 0;
    memset(&sPoisonHealth, 0, sizeof(sPoisonHealth));
    memset(&sWaterHealth, 0, sizeof(sWaterHealth));
    sWaterHealth.previousHealth = m.health;
    sHealthRegenFrames = 0;
    sHealthRegenInactiveFrames = 0;
    sShallowRegenExposureFrames = 0;
    gMarioObject = &sMarioObject;
    gMarioState = &m;
    o = &sObj;
    sWaterLevel = 0;
    gCurrLevelNum = LEVEL_NONE;
    gCurrAreaIndex = 1;
}
static f32 apex(f32 v) {
    f32 y = 0;
    while (v > 0) {
        y += v;
        v -= 4;
    }
    return y;
}
/* Use native floor classification, steep takeoff, kick entry and gravity.
 * Linear air travel isolates the launch rule from collision impulses. */
static int slope_jump_kick_height(void) {
    const s32 slopes[] = { 0, 29, 31, 36, 60 };
    const s32 types[] = { SURFACE_DEFAULT, SURFACE_SLIPPERY, SURFACE_VERY_SLIPPERY,
                          SURFACE_NOT_SLIPPERY };
    const f32 speeds[] = { 0.0f, 40.0f, 128.0f };
    static struct TestBodyState body;

    for (unsigned slope = 0; slope < sizeof(slopes) / sizeof(slopes[0]); slope++) {
        for (unsigned type = 0; type < sizeof(types) / sizeof(types[0]); type++) {
            for (s32 metal = 0; metal < 2; metal++) {
                for (s32 downhill = 0; downhill < 2; downhill++) {
                    for (unsigned speed = 0; speed < sizeof(speeds) / sizeof(speeds[0]); speed++) {
                        for (s32 held = 0; held < 2; held++) {
                            begin(ACT_WALKING);
                            m.marioBodyState = &body;
                            m.flags |= metal ? MARIO_METAL_CAP : 0;
                            sFloor.type = types[type];
                            sFloor.normal.y = coss(DEGREES(slopes[slope]));
                            sFloor.normal.z = -sins(DEGREES(slopes[slope]));
                            m.floorYaw = 0x8000;
                            m.faceAngle[1] = downhill ? 0x8000 : 0;
                            m.forwardVel = speeds[speed];
                            m.input = INPUT_A_PRESSED | INPUT_A_DOWN | INPUT_B_PRESSED;
                            EXPECT_EQ(real_set_jumping_action(&m, ACT_JUMP, 0), TRUE);
                            f32 launch = m.vel[1];
                            f32 forward = m.forwardVel;
                            s16 yaw = m.faceAngle[1];
                            if (slopes[slope] == 31 && type == 0 && !metal && !downhill) {
                                EXPECT_EQ(m.action, ACT_STEEP_JUMP);
                                NEAR(forward, speeds[speed] * 0.75f);
                                NEAR(launch, 42.0f + forward * 0.25f);
                            }
                            EXPECT_EQ(check_kick_or_dive_in_air(&m), TRUE);
                            EXPECT_EQ(m.action, ACT_JUMP_KICK);
                            NEAR(m.vel[1], MAX(launch * 0.75f, 20.0f));
                            NEAR(m.forwardVel, forward);
                            EXPECT_EQ(m.faceAngle[1], yaw);
                            sDiveTrajectory = TRUE;
                            f32 peak = 0.0f;
                            for (s32 frame = 0; frame < 100 && m.vel[1] > 0.0f; frame++) {
                                m.input = held || frame == 0 ? INPUT_A_DOWN : 0;
                                EXPECT_EQ(act_jump_kick(&m), FALSE);
                                peak = MAX(peak, m.pos[1]);
                            }
                            if (held) {
                                NEAR(peak, apex(MAX(launch * 0.75f, 20.0f)));
                            } else {
                                EXPECT_EQ(peak <= apex(MAX(launch * 0.75f, 20.0f)), TRUE);
                            }
                        }
                    }
                }
            }
        }
    }
    return 1;
}

static int early_jump_kick_boundaries(void) {
    const u32 jumps[] = { ACT_JUMP, ACT_HOLD_JUMP, ACT_DOUBLE_JUMP, ACT_TRIPLE_JUMP, ACT_STEEP_JUMP };
    for (unsigned jump = 0; jump < sizeof(jumps) / sizeof(jumps[0]); jump++) {
        for (s32 age = 0; age <= 4; age++) {
            begin(jumps[jump]);
            m.vel[1] = 48.0f;
            m.forwardVel = -40.0f;
            m.framesSinceA = age;
            m.input = INPUT_B_PRESSED;
            EXPECT_EQ(check_kick_or_dive_in_air(&m), TRUE);
            NEAR(m.vel[1], age <= 3 ? 36.0f : 48.0f);
            NEAR(m.forwardVel, -40.0f);
            // A fresh kick during an existing kick retains the existing minimum lift.
            for (s32 cycle = 0; cycle < 3; cycle++) {
                m.vel[1] = cycle == 0 ? -12.0f : 28.0f;
                EXPECT_EQ(check_kick_or_dive_in_air(&m), TRUE);
                NEAR(m.vel[1], cycle == 0 ? 20.0f : 28.0f);
            }
        }
    }
    begin(ACT_STEEP_JUMP);
    m.actionArg = ACT_ARG_STEEP_JUMP_BACKWARD_BUTT_SLIDE_NO_UPHILL;
    m.vel[1] = 24.0f;
    m.input = INPUT_B_PRESSED;
    EXPECT_EQ(check_kick_or_dive_in_air(&m), TRUE);
    NEAR(m.vel[1], 20.0f);
    EXPECT_EQ(m.actionArg,
              ACT_ARG_JUMP_KICK_BACKWARD_BUTT_SLIDE | ACT_ARG_JUMP_KICK_BACKWARD_BUTT_SLIDE_NO_UPHILL);
    return 1;
}

static int ground_chords(void) {
    u32 launches[] = { ACT_JUMP,      ACT_DOUBLE_JUMP, ACT_TRIPLE_JUMP, ACT_BACKFLIP,
                       ACT_LONG_JUMP, ACT_HOLD_JUMP,   ACT_SIDE_FLIP };
    for (unsigned i = 0; i < sizeof(launches) / sizeof(*launches); i++) {
        begin(ACT_IDLE);
        m.input = INPUT_A_PRESSED | INPUT_A_DOWN | INPUT_Z_PRESSED | INPUT_Z_DOWN;
        m.zCrouchBuffered = 1;
        set_mario_action(&m, launches[i], 0);
        EXPECT_EQ(m.input & INPUT_Z_PRESSED, 0);
        EXPECT_EQ(m.input & INPUT_Z_DOWN, INPUT_Z_DOWN);
        EXPECT_EQ(m.zCrouchBuffered, 0);
        m.input = INPUT_Z_PRESSED | INPUT_Z_DOWN;
        set_mario_action(&m, ACT_GROUND_POUND, 0);
        EXPECT_EQ(m.input & INPUT_Z_PRESSED, INPUT_Z_PRESSED);
    }
    return 1;
}
static int ground_dive_height(void) {
    u32 jumps[] = { ACT_JUMP, ACT_DOUBLE_JUMP, ACT_TRIPLE_JUMP };
    for (unsigned j = 0; j < 3; j++) {
        for (int speed = -60; speed <= 127; speed++) {
            begin(ACT_WALKING);
            m.forwardVel = speed;
            m.input = INPUT_A_PRESSED | INPUT_A_DOWN | INPUT_B_PRESSED;
            set_mario_action(&m, jumps[j], 0);
            f32 height = apex(MIN(m.vel[1], MARIO_GLOBAL_SPEED_CAP)), forward = m.forwardVel;
            set_mario_action(&m, ACT_DIVE, 0);
            NEAR(apex(m.vel[1]), height * .9f);
            NEAR(m.forwardVel, forward + FORWARD_DIVE_SPEED_BOOST);
            EXPECT_EQ(m.groundTakeoffThisFrame, 0);
        }
    }
    begin(ACT_JUMP);
    m.vel[1] = 40;
    m.input = INPUT_A_PRESSED | INPUT_B_PRESSED;
    set_mario_action(&m, ACT_DIVE, 0);
    NEAR(m.vel[1], 40);
    begin(ACT_WATER_JUMP);
    m.vel[1] = 50;
    m.input = INPUT_A_PRESSED | INPUT_B_PRESSED;
    set_mario_action(&m, ACT_DIVE, 0);
    NEAR(m.vel[1], 50);
    return 1;
}
static int ground_dive_sequence(void) {
    const int speeds[] = { 0, 40, 100 };
    for (unsigned speed = 0; speed < 3; speed++) {
        for (int held = 0; held < 2; held++) {
            begin(ACT_IDLE);
            sController.stickMag = 64;
            sDives = 1;
            for (int cycle = 0; cycle < 3; cycle++) {
                set_mario_action(&m, ACT_IDLE, 0);
                mario_set_forward_vel(&m, speeds[speed]);
                m.input = INPUT_A_PRESSED | INPUT_B_PRESSED | INPUT_A_DOWN | INPUT_NONZERO_ANALOG;
                EXPECT_EQ(ground_dive_idle_cancels(&m), TRUE);
                EXPECT_EQ(m.action, ACT_JUMP);
                f32 originalHeight = apex(m.vel[1]);
                EXPECT_EQ(act_jump(&m), TRUE);
                EXPECT_EQ(m.action, ACT_DIVE);
                f32 launch = m.vel[1], height = 0;
                EXPECT_EQ(launch > 0, TRUE);
                sDiveTrajectory = 1;
                for (int frame = 0; frame < 100 && m.action == ACT_DIVE; frame++) {
                    if (frame > 0) {
                        m.input = held ? INPUT_A_DOWN : 0;
                    }
                    EXPECT_EQ(act_dive(&m), FALSE);
                    height = MAX(height, m.pos[1]);
                    if (frame == 0) {
                        NEAR(m.pos[1], launch);
                    }
                }
                NEAR(height, originalHeight * .9f);
                EXPECT_EQ(m.action, ACT_DIVE_SLIDE);
                EXPECT_EQ(m.groundTakeoffThisFrame, FALSE);
            }
        }
    }
    return 1;
}
static int metal_lava_launch(void) {
    u32 args[] = { ACT_ARG_LAVA_BOOST_FLOOR, ACT_ARG_LAVA_BOOST_WALL,
                   ACT_ARG_LAVA_BOOST_METAL_GROUND_POUND };
    for (unsigned arg = 0; arg < 3; arg++) {
        for (int metal = 0; metal < 2; metal++) {
            begin(ACT_GROUND_POUND);
            if (metal) {
                m.flags |= MARIO_METAL_CAP;
            }
            m.forwardVel = 10;
            m.vel[1] = -50;
            EXPECT_EQ(set_mario_action(&m, ACT_LAVA_BOOST, args[arg]), TRUE);
            EXPECT_EQ(m.action, ACT_LAVA_BOOST);
            EXPECT_EQ(m.actionArg, args[arg]);
            NEAR(m.vel[1], 84);
            NEAR(m.forwardVel, arg == 1 ? 10 : 0);
            if (metal && arg == 2) {
                EXPECT_EQ(check_common_airborne_cancels(&m), FALSE);
                EXPECT_EQ(m.action, ACT_LAVA_BOOST);
            }
        }
    }
    return 1;
}

static int squish_dispatch(void) {
    for (int metal = 0; metal < 2; metal++) {
        for (int cycle = 0; cycle < 3; cycle++) {
            begin(ACT_SQUISHED);
            m.flags |= metal ? MARIO_METAL_CAP : 0;
            m.pos[1] = 10;
            m.ceilHeight = 100;
            m.input = INPUT_SQUISHED;
            m.vel[1] = -30;
            EXPECT_EQ(act_squished(&m), TRUE);
            EXPECT_EQ(m.action, ACT_FREEFALL);
            EXPECT_EQ(check_common_airborne_cancels(&m), FALSE);
            EXPECT_EQ(m.action, ACT_FREEFALL);
            NEAR(m.pos[1], 10);
            NEAR(m.vel[1], -30);
            m.input = INPUT_SQUISHED;
            m.pos[1] = 0;
            EXPECT_EQ(check_common_airborne_cancels(&m), TRUE);
            EXPECT_EQ(m.action, ACT_SQUISHED);
            EXPECT_EQ(act_squished(&m), FALSE);
            EXPECT_EQ(m.action, ACT_SQUISHED);
        }
    }
    return 1;
}
static int crush_respawn_visibility(void) {
    for (int timeout = 0; timeout < 2; timeout++) {
        begin(ACT_IDLE);
        gMarioState = &m;
        gCurrentArea = &sArea;
        sWaterLevel = -10000;
        sSpawnInfo.startPos[0] = 0;
        sSpawnInfo.startPos[1] = 20;
        sSpawnInfo.startPos[2] = 400;
        sSpawnInfo.startAngle[1] = (s16) 0x8000;
        static struct TestBodyState body;
        m.marioBodyState = &body;
        for (int cycle = 0; cycle < 3; cycle++) {
            m.health = 0xFF;
            m.pos[1] = m.floorHeight = 0;
            m.ceilHeight = 40;
            set_mario_action(&m, ACT_SQUISHED, timeout ? 301 : 0);
            m.actionState = ACT_STATE_SQUISHED_UNSQUISH;
            m.actionTimer = timeout ? 0 : 14;
            m.marioObj->header.gfx.node.flags = GRAPH_RENDER_ACTIVE | GRAPH_RENDER_HAS_ANIMATION;
            int warps = sWarped;
            act_squished(&m);
            EXPECT_EQ(sWarped, warps + 1);
            EXPECT_EQ(m.action, ACT_DISAPPEARED);
            for (int frame = 0; frame < 48; frame++) {
                act_disappeared(&m);
            }
            EXPECT_EQ(m.marioObj->header.gfx.node.flags & GRAPH_RENDER_ACTIVE, 0);
            m.windColumnActive = TRUE;
            m.windDeathGraceTimer = 30;
            init_mario();
            EXPECT_EQ(m.windColumnActive, FALSE);
            EXPECT_EQ(m.windDeathGraceTimer, 0);
            EXPECT_EQ(m.marioObj == &sMarioObject, TRUE);
            EXPECT_EQ(m.marioObj->header.gfx.node.flags & GRAPH_RENDER_ACTIVE, GRAPH_RENDER_ACTIVE);
            EXPECT_EQ(m.marioObj->header.gfx.node.flags & GRAPH_RENDER_HAS_ANIMATION,
                      GRAPH_RENDER_HAS_ANIMATION);
            EXPECT_EQ(m.action, ACT_IDLE);
            EXPECT_EQ(m.squishTimer, 0);
            NEAR(m.pos[0], 0);
            NEAR(m.pos[1], 20);
            NEAR(m.pos[2], 400);
        }
    }
    return 1;
}
static int normal_cap_same_area_warp(void) {
    begin(ACT_IDLE);
    gCurrentArea = &sArea;
    sWaterLevel = -10000;
    static struct TestBodyState body;
    m.marioBodyState = &body;
    sMarioObject.header.gfx.areaIndex = gCurrAreaIndex = 1;
    sSavedCapPresent = TRUE;

    init_mario();
    EXPECT_EQ(sCapSpawnCount, 1);
    EXPECT_EQ(sCapObjects[0].behavior, bhvNormalCap);
    NEAR(sCapObjects[0].oPosVec, 120);
    NEAR(sCapObjects[0].oSpawnPos[1], 30);
    NEAR(sCapObjects[0].oSpawnPos[2], -50);
    for (int warp = 0; warp < 3; warp++) {
        init_mario();
        EXPECT_EQ(sCapSpawnCount, 1);
        EXPECT_EQ(gObjectLists[OBJ_LIST_LEVEL].header.next, &sCapObjects[0]);
    }

    // A cap awaiting removal cannot suppress restoration after another load.
    sCapObjects[0].activeFlags = ACTIVE_FLAG_DEACTIVATED;
    init_mario();
    EXPECT_EQ(sCapSpawnCount, 2);
    init_mario();
    EXPECT_EQ(sCapSpawnCount, 2);

    // A clean level load clears the object list, then restores one saved cap.
    gObjectLists[OBJ_LIST_LEVEL].header.next = &gObjectLists[OBJ_LIST_LEVEL];
    init_mario();
    EXPECT_EQ(sCapSpawnCount, 3);
    init_mario();
    EXPECT_EQ(sCapSpawnCount, 3);

    // Once the cap has been collected, the cleared save state creates none.
    sSavedCapPresent = FALSE;
    init_mario();
    EXPECT_EQ(sCapSpawnCount, 3);
    return 1;
}
static int lab_chill_bully_spawn(void) {
    const int levels[] = { LEVEL_PHYSICS_LAB, LEVEL_SL, LEVEL_BOB };
    const float heights[] = { -300, 0, 1029, 1030, 1400 };
    begin(ACT_IDLE);
    sObjFloor = &sFloor;
    for (unsigned level = 0; level < 3; level++) {
        for (unsigned y = 0; y < 5; y++) {
            gCurrLevelNum = levels[level];
            o->oBullySubtype = BULLY_STYPE_CHILL;
            o->oPosY = heights[y];
            o->oAction = 0;
            sFloor.type = SURFACE_DEFAULT;
            for (int frame = 0; frame < 180; frame++) {
                bully_step();
            }
            EXPECT_EQ(o->oAction,
                      levels[level] != LEVEL_PHYSICS_LAB && heights[y] < 1030 ? OBJ_ACT_LAVA_DEATH : 0);
        }
    }
    gCurrLevelNum = LEVEL_PHYSICS_LAB;
    const int surfaces[] = { SURFACE_BURNING, SURFACE_DEATH_PLANE, SURFACE_VERTICAL_WIND };
    for (int i = 0; i < 3; i++) {
        sFloor.type = surfaces[i];
        o->oAction = 0;
        bully_step();
        EXPECT_EQ(o->oAction, i == 0 ? OBJ_ACT_LAVA_DEATH : OBJ_ACT_DEATH_PLANE_DEATH);
    }
    gCurrLevelNum = LEVEL_SL;
    o->oBullySubtype = BULLY_STYPE_GENERIC;
    o->oPosY = 0;
    o->oAction = 0;
    sFloor.type = SURFACE_DEFAULT;
    bully_step();
    EXPECT_EQ(o->oAction, 0);
    return 1;
}
static int dialogue_stop(void) {
    u32 actions[] = { ACT_WAITING_FOR_DIALOG, ACT_READING_SIGN, ACT_READING_NPC_DIALOG };
    for (unsigned i = 0; i < 3; i++) {
        begin(ACT_WALKING);
        m.forwardVel = 40;
        m.vel[0] = 20;
        m.vel[2] = 30;
        m.slideVelX = 70;
        m.slideVelZ = -45;
        m.vel[1] = 8;
        set_mario_action(&m, actions[i], 0);
        NEAR(m.forwardVel, 0);
        NEAR(m.vel[0], 0);
        NEAR(m.vel[2], 0);
        NEAR(m.slideVelX, 0);
        NEAR(m.slideVelZ, 0);
        NEAR(m.vel[1], 8);
    }
    return 1;
}
static int shoreline(void) {
    for (int held = 0; held < 2; held++) {
        for (int analog = 0; analog < 2; analog++) {
            for (int dir = -1; dir <= 1; dir += 2) {
                begin(ACT_BREASTSTROKE);
                m.heldObj = held ? &sHeld : NULL;
                m.input = analog ? INPUT_NONZERO_ANALOG : 0;
                m.faceAngle[0] = 12000;
                m.faceAngle[2] = -900;
                m.marioObj->header.gfx.angle[0] = -12000;
                m.marioObj->header.gfx.angle[2] = -900;
                m.vel[0] = 12 * dir;
                m.vel[2] = 18 * dir;
                m.forwardVel = 60;
                transition_submerged_to_walking(&m);
                NEAR(m.vel[0], 12 * dir);
                NEAR(m.vel[2], 18 * dir);
                NEAR(m.slideVelX, 12 * dir);
                NEAR(m.slideVelZ, 18 * dir);
                EXPECT_EQ(m.faceAngle[0], 0);
                EXPECT_EQ(m.faceAngle[2], 0);
                EXPECT_EQ(m.marioObj->header.gfx.angle[0], 0);
                EXPECT_EQ(m.marioObj->header.gfx.angle[2], 0);
                u32 expectedAction;
                if (held) {
                    expectedAction = analog ? ACT_HOLD_WALKING : ACT_HOLD_DECELERATING;
                } else {
                    expectedAction = analog ? ACT_WALKING : ACT_DECELERATING;
                }
                EXPECT_EQ(m.action, expectedAction);
            }
        }
    }
    return 1;
}
static int water_dive_pose(void) {
    for (int age = 0; age < 2; age++) {
        begin(ACT_WATER_JUMP);
        m.actionTimer = age ? 5 : 0;
        m.faceAngle[0] = 12000;
        m.faceAngle[2] = 900;
        m.marioObj->header.gfx.angle[0] = -12000;
        m.marioObj->header.gfx.angle[2] = 900;
        m.forwardVel = 2;
        m.vel[1] = 62;
        m.vel[2] = 2;
        m.input = INPUT_B_PRESSED;
        sDives = 1;
        EXPECT_EQ(act_water_jump(&m), TRUE);
        EXPECT_EQ(m.action, ACT_DIVE);
        EXPECT_EQ(m.faceAngle[0], 0);
        EXPECT_EQ(m.faceAngle[2], 0);
        EXPECT_EQ(m.marioObj->header.gfx.angle[0], 0);
        EXPECT_EQ(m.marioObj->header.gfx.angle[2], 0);
        NEAR(m.vel[1], 62);
        NEAR(m.forwardVel, 36);
        NEAR(m.vel[2], 36);
    }
    // A fast dive keeps its own launch speed; a backward-stick B is still a kick.
    begin(ACT_WATER_JUMP);
    m.forwardVel = 40;
    m.vel[1] = 62;
    m.input = INPUT_B_PRESSED;
    sDives = 1;
    act_water_jump(&m);
    EXPECT_EQ(m.action, ACT_DIVE);
    NEAR(m.forwardVel, 45);
    NEAR(m.vel[1], 62);
    begin(ACT_WATER_JUMP);
    m.forwardVel = 2;
    m.vel[1] = 62;
    m.input = INPUT_B_PRESSED;
    sDives = 0;
    act_water_jump(&m);
    EXPECT_EQ(m.action, ACT_JUMP_KICK);
    NEAR(m.forwardVel, 2);
    begin(ACT_BREASTSTROKE);
    m.faceAngle[0] = 12000;
    m.faceAngle[2] = 900;
    m.marioObj->header.gfx.angle[0] = -12000;
    m.marioObj->header.gfx.angle[2] = 900;
    m.vel[1] = 30;
    m.input = INPUT_A_DOWN;
    sDives = 1;
    transition_submerged_to_airborne(&m);
    EXPECT_EQ(m.action, ACT_DIVE);
    EXPECT_EQ(m.faceAngle[0], 0);
    EXPECT_EQ(m.faceAngle[2], 0);
    EXPECT_EQ(m.marioObj->header.gfx.angle[0], 0);
    EXPECT_EQ(m.marioObj->header.gfx.angle[2], 0);
    NEAR(m.vel[1], 30);
    return 1;
}
static int wing_dive_burial(void) {
    for (int cycle = 0; cycle < 3; cycle++) {
        for (int terrain = 0; terrain < 2; terrain++) {
            for (int wing = 0; wing < 2; wing++) {
                for (int held = 0; held < 2; held++) {
                    begin(ACT_DIVE);
                    m.flags = wing ? MARIO_WING_CAP : 0;
                    sArea.terrainType = terrain ? TERRAIN_SNOW : TERRAIN_SAND;
                    m.peakHeight = 1100.0f;
                    m.faceAngle[0] = -DEGREES(60);
                    m.input = held ? 0 : INPUT_A_DOWN;
                    sController.buttonDown = held ? A_BUTTON : 0;
                    sAirStepResult = AIR_STEP_LANDED;
                    act_dive(&m);
                    EXPECT_EQ(m.action, wing && held ? ACT_DIVE_SLIDE : ACT_HEAD_STUCK_IN_GROUND);
                    EXPECT_EQ(m.faceAngle[0], 0);
                }
            }
        }
    }
    return TRUE;
}
static int water_jump_ledge_voice(void) {
    const f32 incoming[] = {
        -40.0f, -24.01f, -24.0f, -23.99f, -20.0f, -16.0f, -8.0f, -0.01f, 0.0f, 2.0f
    };
    for (int cycle = 0; cycle < 3; cycle++) {
        for (unsigned i = 0; i < sizeof(incoming) / sizeof(incoming[0]); i++) {
            begin(ACT_WATER_JUMP);
            m.vel[1] = incoming[i];
            sAirStepResult = AIR_STEP_GRABBED_LEDGE;
            act_water_jump(&m);
            EXPECT_EQ(m.action, ACT_LEDGE_GRAB);
            EXPECT_EQ(m.actionArg, incoming[i] <= -24.0f ? 0 : ACT_ARG_LEDGE_GRAB_GENTLE);
            NEAR(m.vel[1], incoming[i] - 4.0f);
        }
    }
    return TRUE;
}
static int water_jump_exit_pose(void) {
    for (int exit = 0; exit < 4; exit++) {
        begin(exit >= 2 ? ACT_HOLD_WATER_JUMP : ACT_WATER_JUMP);
        if (exit >= 2) {
            m.heldObj = &sHeld;
            sHeld.oInteractionSubtype = 0;
        }
        m.faceAngle[0] = 12000;
        m.faceAngle[2] = 900;
        m.marioObj->header.gfx.angle[0] = -12000;
        m.marioObj->header.gfx.angle[2] = 900;
        m.vel[1] = 62;
        if (exit % 2) {
            m.input = INPUT_Z_PRESSED;
        } else if (exit == 2) {
            m.input = INPUT_B_PRESSED;
        } else {
            m.input = 0;
        }
        sAirStepResult = exit == 0 ? AIR_STEP_LANDED : AIR_STEP_NONE;
        if (exit >= 2) {
            act_hold_water_jump(&m);
        } else {
            act_water_jump(&m);
        }
        EXPECT_EQ(m.action, exit == 0   ? ACT_JUMP_LAND
                            : exit == 1 ? ACT_GROUND_POUND
                            : exit == 2 ? ACT_AIR_THROW
                                        : ACT_GROUND_POUND);
        EXPECT_EQ(m.faceAngle[0], 0);
        EXPECT_EQ(m.faceAngle[2], 0);
        EXPECT_EQ(m.marioObj->header.gfx.angle[0], 0);
        EXPECT_EQ(m.marioObj->header.gfx.angle[2], 0);
    }
    return 1;
}
static int water_jump_rim_assist(void) {
    for (int held = 0; held < 2; held++) {
        begin(held ? ACT_HOLD_WATER_JUMP : ACT_WATER_JUMP);
        if (held) {
            m.heldObj = &sHeld;
        }
        m.faceAngle[0] = DEGREES(60);
        sController.stickY = -64;
        m.vel[1] = 62;
        m.forwardVel = 2;
        m.vel[2] = 2;
        for (int frame = 0; frame < 11; frame++) {
            // The pool wall can block travel during the rise; keep building speed for its rim.
            sAirStepResult = frame < 5 ? AIR_STEP_HIT_WALL : AIR_STEP_NONE;
            EXPECT_EQ(held ? act_hold_water_jump(&m) : act_water_jump(&m), FALSE);
            NEAR(m.forwardVel, MIN(2.0f + 3.0f * (frame + 1), 20.0f));
            NEAR(m.vel[2], m.forwardVel);
        }
        EXPECT_EQ(m.action, held ? ACT_HOLD_WATER_JUMP : ACT_WATER_JUMP);
    }
    for (int gate = 0; gate < 5; gate++) {
        begin(ACT_WATER_JUMP);
        m.forwardVel = gate == 2 ? 20 : 2;
        m.vel[1] = gate == 3 ? -1 : 62;
        m.faceAngle[0] = gate == 1 ? DEGREES(44) : DEGREES(60);
        sController.stickY = gate == 0 ? -47 : -64;
        if (gate == 4) {
            m.actionTimer = WATER_JUMP_WALL_KICK_LOCK_FRAMES + 1;
        }
        sAirStepResult = AIR_STEP_NONE;
        act_water_jump(&m);
        NEAR(m.forwardVel, gate == 2 ? 20 : 2);
    }
    return 1;
}
static int water_jump_air_control(void) {
    for (int held = 0; held < 2; held++) {
        u32 action = held ? ACT_HOLD_WATER_JUMP : ACT_WATER_JUMP;
        f32 heights[8], verticalSpeeds[8];
        for (int steering = 0; steering < 2; steering++) {
            begin(action);
            sRealMovingEntry = sDiveTrajectory = TRUE;
            if (held) {
                m.heldObj = &sHeld;
                sHeld.oInteractionSubtype = 0;
            }
            m.vel[1] = 62.0f;
            m.input = INPUT_A_DOWN | (steering ? INPUT_NONZERO_ANALOG : 0);
            m.intendedMag = 32.0f;
            for (int frame = 0; frame < 8; frame++) {
                EXPECT_EQ(held ? act_hold_water_jump(&m) : act_water_jump(&m), FALSE);
                EXPECT_EQ(m.action, action);
                if (steering) {
                    NEAR(m.pos[1], heights[frame]);
                    NEAR(m.vel[1], verticalSpeeds[frame]);
                } else {
                    heights[frame] = m.pos[1];
                    verticalSpeeds[frame] = m.vel[1];
                }
            }
            // A stationary exit can cross a body-radius distance while still rising.
            EXPECT_EQ(m.vel[1] > 0.0f, TRUE);
            if (steering) {
                EXPECT_EQ(m.pos[2] > 48.0f, TRUE);
            } else {
                NEAR(m.pos[2], 0.0f);
            }
        }
        for (int yaw = 0; yaw < 2; yaw++) {
            for (int side = -1; side <= 1; side += 2) {
                begin(action);
                sRealMovingEntry = TRUE;
                m.heldObj = held ? &sHeld : NULL;
                m.faceAngle[1] = yaw ? 0x4000 : 0;
                m.intendedYaw = m.faceAngle[1] + side * 0x4000;
                m.intendedMag = 16.0f;
                m.input = INPUT_NONZERO_ANALOG;
                m.vel[1] = -8.0f;
                m.actionTimer = WATER_JUMP_WALL_KICK_LOCK_FRAMES + 1;
                m.vel[0] = m.slideVelX = yaw ? 0.0f : side * 3.0f;
                m.vel[2] = m.slideVelZ = yaw ? -side * 3.0f : 0.0f;
                sAirStepResult = AIR_STEP_NONE;
                held ? act_hold_water_jump(&m) : act_water_jump(&m);
                NEAR(m.vel[0], yaw ? 0.0f : side * 3.0f);
                NEAR(m.vel[2], yaw ? -side * 3.0f : 0.0f);
                NEAR(m.vel[1], -8.0f);
                EXPECT_EQ(m.faceAngle[1], yaw ? 0x4000 : 0);
                m.input = 0;
                for (int frame = 0; frame < 3; frame++) {
                    held ? act_hold_water_jump(&m) : act_water_jump(&m);
                    NEAR(m.vel[0], yaw ? 0.0f : side * 3.0f);
                    NEAR(m.vel[2], yaw ? -side * 3.0f : 0.0f);
                }
            }
        }
        begin(action);
        sRealMovingEntry = TRUE;
        m.heldObj = held ? &sHeld : NULL;
        m.input = INPUT_NONZERO_ANALOG;
        m.intendedMag = 32.0f;
        m.vel[1] = -8.0f;
        mario_set_forward_vel(&m, 60.0f);
        sAirStepResult = AIR_STEP_NONE;
        held ? act_hold_water_jump(&m) : act_water_jump(&m);
        NEAR(m.forwardVel, 60.0f);
        m.intendedYaw = -0x8000;
        held ? act_hold_water_jump(&m) : act_water_jump(&m);
        NEAR(m.forwardVel, 60.0f);
        mario_set_forward_vel(&m, 2.0f);
        for (int frame = 0; frame < 30; frame++) {
            held ? act_hold_water_jump(&m) : act_water_jump(&m);
        }
        NEAR(m.forwardVel, 2.0f);
        m.intendedYaw = 0;
        for (int frame = 0; frame < 50; frame++) {
            held ? act_hold_water_jump(&m) : act_water_jump(&m);
        }
        NEAR(m.forwardVel, 32.0f);
        m.intendedYaw = 0x2000;
        mario_set_forward_vel(&m, 0.0f);
        held ? act_hold_water_jump(&m) : act_water_jump(&m);
        NEAR(m.forwardVel, 1.5f * coss(0x2000));
        NEAR(m.vel[0], 0.0f);
        m.intendedYaw = 0;
        // Standard steering does not stack acceleration onto the steep-exit boost.
        m.faceAngle[0] = DEGREES(60);
        m.actionTimer = 0;
        m.vel[1] = 62.0f;
        sController.stickY = -64;
        mario_set_forward_vel(&m, 2.0f);
        held ? act_hold_water_jump(&m) : act_water_jump(&m);
        NEAR(m.forwardVel, 5.0f);
    }
    return TRUE;
}
static int water_jump_wall_kick_priority(void) {
    const f32 speeds[] = { 10.0f, 10.01f, 16.0f, 16.01f, 40.0f };
    for (int cycle = 0; cycle < 3; cycle++) {
        for (int timer = WATER_JUMP_WALL_KICK_LOCK_FRAMES - 1;
             timer <= WATER_JUMP_WALL_KICK_LOCK_FRAMES + 1; timer++) {
            for (int buffer = 0; buffer <= 2; buffer++) {
                for (int stick = 0; stick < 2; stick++) {
                    for (unsigned speed = 0; speed < sizeof(speeds) / sizeof(*speeds); speed++) {
                        begin(ACT_WATER_JUMP);
                        m.actionTimer = timer;
                        m.vel[1] = -1.0f;
                        m.forwardVel = speeds[speed];
                        m.wallKickInputFrames = buffer;
                        m.wallKickForwardStickFrames = stick;
                        sAirStepResult = AIR_STEP_NONE;
                        sLastAirStepArg = 0;
                        act_water_jump(&m);
                        s32 priority = timer >= WATER_JUMP_WALL_KICK_LOCK_FRAMES && buffer != 0
                                       && stick != 0 && speed != 0;
                        EXPECT_EQ(sLastAirStepArg, AIR_STEP_CHECK_LEDGE_GRAB
                                                       | (priority ? AIR_STEP_CHECK_WALL_KICK : 0));
                        EXPECT_EQ(m.action, ACT_WATER_JUMP);
                        EXPECT_EQ(m.wallKickInputFrames, buffer);
                    }
                }
            }
        }
    }
    for (unsigned speed = 0; speed < sizeof(speeds) / sizeof(speeds[0]); speed++) {
        begin(ACT_WATER_JUMP);
        m.actionTimer = WATER_JUMP_WALL_KICK_LOCK_FRAMES + 1;
        m.forwardVel = speeds[speed];
        m.wall = &sFloor;
        m.wallKickForwardStickFrames = 1;
        real_water_jump_hit_wall(&m);
        EXPECT_EQ(sWallKickWindows, speeds[speed] > 10.0f);
        EXPECT_EQ(m.action, ACT_WATER_JUMP);
    }
    return TRUE;
}
static int bully_release(void) {
    begin(ACT_SOFT_BONK);
    m.input = INPUT_A_PRESSED;
    update_bully_knockback_buffer(&m);
    sAirStepResult = AIR_STEP_LANDED;
    common_air_knockback_step(&m, ACT_FREEFALL_LAND, ACT_HARD_BACKWARD_GROUND_KB, 0, -16);
    m.input = 0;
    common_landing_cancels(&m, &sFreefallLandAction, set_jumping_action);
    EXPECT_EQ(m.action, ACT_DOUBLE_JUMP);
    for (int age = 0; age <= 6; age++) {
        begin(ACT_BACKWARD_GROUND_KB);
        m.input = INPUT_A_PRESSED | INPUT_A_DOWN | INPUT_B_PRESSED;
        update_bully_knockback_buffer(&m);
        m.input = INPUT_A_DOWN;
        m.framesSinceA = m.framesSinceB = age;
        update_bully_knockback_buffer(&m);
        EXPECT_EQ(m.bullyKnockbackBuffer, age < 6 ? INPUT_A_PRESSED | INPUT_B_PRESSED : 0);
    }
    static struct TestBodyState body;
    begin(ACT_BACKWARD_AIR_KB);
    m.bullyKnockbackActive = 1;
    m.input = INPUT_A_PRESSED | INPUT_A_DOWN;
    update_bully_knockback_buffer(&m);
    EXPECT_EQ(m.bullyKnockbackBuffer & INPUT_A_PRESSED, INPUT_A_PRESSED);
    m.input = INPUT_B_PRESSED;
    update_bully_knockback_buffer(&m);
    EXPECT_EQ(m.bullyKnockbackBuffer, INPUT_A_PRESSED | INPUT_B_PRESSED);
    m.action = ACT_BACKWARD_GROUND_KB;
    sAnimEnded = 1;
    common_ground_knockback_action(&m, 0, 10, FALSE, 0);
    EXPECT_EQ(m.action, ACT_JUMP);
    EXPECT_EQ(m.bullyKnockbackImmunityFrames, 2);
    begin(ACT_BACKWARD_GROUND_KB);
    m.bullyKnockbackActive = 1;
    m.bullyKnockbackBuffer = INPUT_A_PRESSED;
    sAnimEnded = 1;
    common_ground_knockback_action(&m, 0, 10, FALSE, 0);
    EXPECT_EQ(m.action, ACT_JUMP);
    EXPECT_EQ(m.bullyKnockbackImmunityFrames, 2);
    begin(ACT_BACKWARD_GROUND_KB);
    m.bullyKnockbackActive = 1;
    m.input = INPUT_A_DOWN | INPUT_A_PRESSED;
    update_bully_knockback_buffer(&m);
    sAnimEnded = 1;
    common_ground_knockback_action(&m, 0, 10, FALSE, 0);
    EXPECT_EQ(m.action, ACT_JUMP);
    EXPECT_EQ(m.bullyKnockbackActive, 0);
    EXPECT_EQ(m.bullyKnockbackImmunityFrames, 2);
    begin(ACT_BACKWARD_GROUND_KB);
    m.bullyKnockbackActive = 1;
    set_mario_action(&m, ACT_BACKWARD_AIR_KB, 0);
    EXPECT_EQ(m.bullyKnockbackImmunityFrames, 0);
    const u32 ground[] = { ACT_SOFT_BACKWARD_GROUND_KB,
                           ACT_SOFT_FORWARD_GROUND_KB,
                           ACT_BACKWARD_GROUND_KB,
                           ACT_FORWARD_GROUND_KB,
                           ACT_HARD_BACKWARD_GROUND_KB,
                           ACT_HARD_FORWARD_GROUND_KB,
                           ACT_GROUND_BONK };
    for (unsigned i = 0; i < sizeof(ground) / sizeof(*ground); i++) {
        for (int held = 0; held < 2; held++) {
            begin(ground[i]);
            m.input = INPUT_A_PRESSED | INPUT_A_DOWN | INPUT_B_PRESSED;
            update_bully_knockback_buffer(&m);
            m.input = held ? INPUT_A_DOWN : 0;
            update_bully_knockback_buffer(&m);
            sAnimEnded = 1;
            common_ground_knockback_action(&m, 0, 10, FALSE, 0);
            EXPECT_EQ(m.action, ACT_JUMP);
            EXPECT_EQ(m.bullyKnockbackBuffer, 0);
            EXPECT_EQ(m.bullyKnockbackImmunityFrames, 0);
        }
    }
    const u32 air[] = { ACT_BACKWARD_AIR_KB,     ACT_FORWARD_AIR_KB, ACT_HARD_BACKWARD_AIR_KB,
                        ACT_HARD_FORWARD_AIR_KB, ACT_THROWN_FORWARD, ACT_THROWN_BACKWARD };
    for (unsigned i = 0; i < sizeof(air) / sizeof(*air); i++) {
        begin(air[i]);
        m.input = INPUT_B_PRESSED;
        update_bully_knockback_buffer(&m);
        sAirStepResult = AIR_STEP_LANDED;
        common_air_knockback_step(&m, ACT_BACKWARD_GROUND_KB, ACT_HARD_BACKWARD_GROUND_KB, 0, -16);
        EXPECT_EQ(m.bullyKnockbackBuffer, INPUT_B_PRESSED);
        m.input = 0;
        sAnimEnded = 1;
        common_ground_knockback_action(&m, 0, 10, FALSE, 0);
        EXPECT_EQ(m.action, ACT_PUNCHING);
        EXPECT_EQ(m.bullyKnockbackBuffer, 0);
    }
    for (int forward = 0; forward < 2; forward++) {
        for (int held = 0; held < 2; held++) {
            begin(forward ? ACT_FORWARD_WATER_KB : ACT_BACKWARD_WATER_KB);
            m.input = INPUT_A_PRESSED | INPUT_A_DOWN | INPUT_B_PRESSED;
            update_bully_knockback_buffer(&m);
            m.input = held ? INPUT_A_DOWN : 0;
            update_bully_knockback_buffer(&m);
            sAnimEnded = 1;
            m.marioBodyState = &body;
            common_water_knockback_step(&m, 0, ACT_WATER_IDLE, 0);
            EXPECT_EQ(m.action, ACT_BREASTSTROKE);
            EXPECT_EQ(m.bullyKnockbackBuffer, 0);
        }
    }
    begin(ACT_FORWARD_GROUND_KB);
    m.input = INPUT_B_PRESSED;
    update_bully_knockback_buffer(&m);
    set_mario_action(&m, ACT_STANDING_DEATH, 0);
    EXPECT_EQ(m.bullyKnockbackBuffer, 0);
    return 1;
}
static int heave_ho_landing_grace(void) {
    for (int cycle = 0; cycle < 3; cycle++) {
        begin(ACT_THROWN_BACKWARD);
        m.heaveHoThrowPendingLanding = 1;
        sAirStepResult = AIR_STEP_NONE;
        common_air_knockback_step(&m, ACT_BACKWARD_GROUND_KB, ACT_HARD_BACKWARD_GROUND_KB,
                                  MARIO_ANIM_BACKWARD_AIR_KB, m.forwardVel);
        EXPECT_EQ(m.heaveHoThrowPendingLanding, 1);
        EXPECT_EQ(m.heaveHoThrowRecoveryPending, 0);
        EXPECT_EQ(m.heaveHoLaunchImmunityFrames, 0);
        sAirStepResult = AIR_STEP_LANDED;
        common_air_knockback_step(&m, ACT_BACKWARD_GROUND_KB, ACT_HARD_BACKWARD_GROUND_KB,
                                  MARIO_ANIM_BACKWARD_AIR_KB, m.forwardVel);
        EXPECT_EQ(m.action, ACT_BACKWARD_GROUND_KB);
        EXPECT_EQ(m.heaveHoThrowPendingLanding, 0);
        EXPECT_EQ(m.heaveHoThrowRecoveryPending, 1);
        EXPECT_EQ(m.heaveHoLaunchImmunityFrames, 0);
        common_air_knockback_step(&m, ACT_BACKWARD_GROUND_KB, ACT_HARD_BACKWARD_GROUND_KB,
                                  MARIO_ANIM_BACKWARD_AIR_KB, m.forwardVel);
        EXPECT_EQ(m.heaveHoThrowRecoveryPending, 1);
        EXPECT_EQ(m.heaveHoLaunchImmunityFrames, 0);
        begin(ACT_THROWN_FORWARD);
        m.heaveHoThrowPendingLanding = 1;
        set_mario_action(&m, ACT_FREEFALL, 0);
        EXPECT_EQ(m.heaveHoThrowPendingLanding, 0);
        EXPECT_EQ(m.heaveHoThrowRecoveryPending, 0);
    }
    return 1;
}
static int twirl_landing_cancels(void) {
    const u32 inputs[] = { INPUT_A_PRESSED, INPUT_B_PRESSED, INPUT_Z_DOWN, INPUT_NONZERO_ANALOG };
    const u32 exits[] = { ACT_JUMP, ACT_PUNCHING, ACT_START_CROUCHING, ACT_WALKING };
    for (int cycle = 0; cycle < 3; cycle++) {
        for (int frame = -1; frame <= 4; frame++) {
            for (int input = 0; input < 4; input++) {
                for (int moving = 0; moving < 2; moving++) {
                    begin(ACT_TWIRL_LAND);
                    m.prevAction = ACT_TWIRLING;
                    set_mario_animation(&m, MARIO_ANIM_TWIRL_LAND);
                    m.marioObj->header.gfx.animInfo.animFrame = frame;
                    m.faceAngle[1] = 0x1000;
                    m.twirlYaw = 0x2000;
                    m.angleVel[1] = 0x1800;
                    m.input = inputs[input];
                    sTrackRolloutGround = TRUE;
                    mario_set_forward_vel(&m, moving ? 12 : 0);
                    EXPECT_EQ(act_twirl_land(&m), TRUE);
                    u32 exit = moving && input == 1   ? ACT_MOVE_PUNCHING
                               : moving && input == 2 ? ACT_CROUCH_SLIDE
                                                      : exits[input];
                    EXPECT_EQ(m.action, exit);
                    EXPECT_EQ(m.faceAngle[1], 0x3000);
                    EXPECT_EQ(m.angleVel[1], 0);
                    EXPECT_EQ(m.twirlYaw, 0);
                    if (input == 1 || input == 3) {
                        NEAR(m.forwardVel, moving ? 12 : 0);
                    }
                }
            }
        }
    }
    // With no input, the landing keeps spinning and finishes normally.
    begin(ACT_TWIRL_LAND);
    set_mario_animation(&m, MARIO_ANIM_TWIRL_LAND);
    m.marioObj->header.gfx.animInfo.animFrame = 3;
    m.faceAngle[1] = 0x1000;
    m.twirlYaw = 0x2000;
    m.angleVel[1] = 0x800;
    EXPECT_EQ(act_twirl_land(&m), FALSE);
    EXPECT_EQ(m.action, ACT_TWIRL_LAND);
    EXPECT_EQ(m.faceAngle[1], 0x1000);
    EXPECT_EQ(m.angleVel[1], 0x400);
    EXPECT_EQ(m.twirlYaw, 0x2400);
    sAnimEnded = TRUE;
    act_twirl_land(&m);
    EXPECT_EQ(m.action, ACT_IDLE);
    EXPECT_EQ(m.faceAngle[1], 0x3400);
    // Environmental interruption keeps its priority even before frame 3.
    for (int frame = 0; frame <= 3; frame++) {
        for (int stomp = 0; stomp < 2; stomp++) {
            begin(ACT_TWIRL_LAND);
            set_mario_animation(&m, MARIO_ANIM_TWIRL_LAND);
            m.marioObj->header.gfx.animInfo.animFrame = frame;
            m.input = INPUT_A_PRESSED | (stomp ? INPUT_STOMPED : INPUT_OFF_FLOOR);
            EXPECT_EQ(act_twirl_land(&m), TRUE);
            EXPECT_EQ(m.action, stomp ? ACT_SHOCKWAVE_BOUNCE : ACT_FREEFALL);
        }
    }
    sTrackRolloutGround = FALSE;
    return TRUE;
}

static int twirl_landing_buffers(void) {
    const u32 inputs[] = { INPUT_A_PRESSED, INPUT_B_PRESSED, INPUT_Z_PRESSED };
    const u32 exits[] = { ACT_JUMP, ACT_PUNCHING, ACT_START_CROUCHING };
    for (int origin = 0; origin < 2; origin++) {
        for (int button = 0; button < 3; button++) {
            for (int age = 0; age <= 6; age++) {
                begin(origin ? ACT_TWIRL_LAND : ACT_TWIRLING);
                m.input = inputs[button];
                mario_update_move_buffers(&m);
                m.input = 0;
                for (int update = 0; update < age; update++) {
                    mario_update_move_buffers(&m);
                }
                set_mario_action(&m, ACT_TWIRL_LAND, 0);
                EXPECT_EQ(act_twirl_land(&m), age < 6);
                EXPECT_EQ(m.action, age < 6 ? exits[button] : ACT_TWIRL_LAND);
                if (age < 6) {
                    EXPECT_EQ(m.automaticAFrames | m.automaticBFrames | m.automaticZFrames, 0);
                }
            }
        }
    }
    for (int offFloor = 0; offFloor < 2; offFloor++) {
        begin(ACT_TWIRL_LAND);
        m.input = INPUT_A_PRESSED | INPUT_B_PRESSED | INPUT_Z_PRESSED;
        mario_update_move_buffers(&m);
        m.input = offFloor ? INPUT_OFF_FLOOR : INPUT_STOMPED;
        EXPECT_EQ(act_twirl_land(&m), TRUE);
        EXPECT_EQ(m.action, offFloor ? ACT_FREEFALL : ACT_SHOCKWAVE_BOUNCE);
        EXPECT_EQ(m.automaticAFrames | m.automaticBFrames | m.automaticZFrames, 0);
    }
    begin(ACT_TWIRLING);
    m.input = INPUT_B_PRESSED;
    mario_update_move_buffers(&m);
    set_mario_action(&m, ACT_GROUND_POUND, 0);
    EXPECT_EQ(m.automaticBFrames, 0);
    return TRUE;
}

static int stop_recovery_controls(void) {
    const u32 actions[] = { ACT_BRAKING_STOP, ACT_BUTT_SLIDE_STOP, ACT_SLIDE_KICK_SLIDE_STOP };
    s32 (*handlers[])(struct MarioState *) = { act_braking_stop, act_butt_slide_stop,
                                               act_slide_kick_slide_stop };
    const u32 inputs[] = { INPUT_A_PRESSED,      INPUT_B_PRESSED,    INPUT_Z_DOWN,
                           INPUT_NONZERO_ANALOG, INPUT_FIRST_PERSON, INPUT_A_PRESSED | INPUT_Z_DOWN };
    const u32 exits[] = { ACT_JUMP,    ACT_MOVE_PUNCHING, ACT_CROUCH_SLIDE,
                          ACT_WALKING, ACT_IDLE,          ACT_BACKFLIP };
    for (int cycle = 0; cycle < 3; cycle++) {
        for (unsigned state = 0; state < 3; state++) {
            for (int frame = 0; frame < 3; frame++) {
                for (unsigned input = 0; input < 6; input++) {
                    begin(actions[state]);
                    m.input = inputs[input];
                    m.actionTimer = frame * 3;
                    sMarioObject.header.gfx.animInfo.animFrame = frame * 3;
                    sAnimEnded = frame == 2;
                    sTrackRolloutGround = TRUE;
                    mario_set_forward_vel(&m, -16);
                    EXPECT_EQ(handlers[state](&m), TRUE);
                    EXPECT_EQ(m.action, input == 2 && state > 0 ? ACT_START_CROUCHING : exits[input]);
                    if (input == 0) {
                        NEAR(m.forwardVel, -16 * m.movementProfile->jumpRetention);
                    } else if (input != 5) {
                        NEAR(m.forwardVel, -16); // No stationary step before cancellation.
                    }
                    if (input == 4) {
                        EXPECT_EQ(ground_dive_idle_cancels(&m), TRUE);
                        EXPECT_EQ(m.action, ACT_FIRST_PERSON);
                    }
                }
            }
        }
    }
    for (unsigned state = 0; state < 3; state++) {
        // C-Up must survive real input filtering, not just a directly injected flag.
        begin(actions[state]);
        gCameraMovementFlags = CAM_MOVE_C_UP_MODE;
        mario_update_c_up_input(&m);
        EXPECT_EQ(m.input & INPUT_FIRST_PERSON, INPUT_FIRST_PERSON);
        EXPECT_EQ(handlers[state](&m), TRUE);
        EXPECT_EQ(ground_dive_idle_cancels(&m), TRUE);
        EXPECT_EQ(m.action, ACT_FIRST_PERSON);
        begin(actions[state]);
        gCameraMovementFlags = CAM_MOVE_C_UP_MODE;
        sController.stickMag = 1;
        mario_update_c_up_input(&m);
        EXPECT_EQ(m.input & INPUT_FIRST_PERSON, 0);
        EXPECT_EQ(gCameraMovementFlags & CAM_MOVE_C_UP_MODE, 0);
        // Raw Z+B can suppress B; the remembered chord must reach the crouch handler.
        for (int moving = 0; moving < 2; moving++) {
            for (int strong = 0; strong < 2; strong++) {
                begin(actions[state]);
                sTrackRolloutGround = TRUE;
                m.forwardVel = moving ? 16 : 0;
                m.input = INPUT_Z_DOWN;
                m.zBPressed = TRUE;
                sController.stickMag = strong ? 64 : 0;
                EXPECT_EQ(handlers[state](&m), TRUE);
                EXPECT_EQ(m.action, moving ? ACT_CROUCH_SLIDE : ACT_CROUCHING);
                EXPECT_EQ(m.zBPressed, TRUE);
                EXPECT_EQ(m.input & INPUT_B_PRESSED, INPUT_B_PRESSED);
                if (moving) {
                    EXPECT_EQ(check_ground_dive_or_punch(&m), TRUE);
                } else {
                    EXPECT_EQ(act_crouching(&m), TRUE);
                }
                EXPECT_EQ(m.action, strong   ? ACT_SLIDE_KICK
                                    : moving ? ACT_MOVE_PUNCHING
                                             : ACT_PUNCHING);
                EXPECT_EQ(m.actionArg, strong ? 0 : ACT_ARG_PUNCH_SEQUENCE_BREAKDANCE);
            }
        }
        begin(actions[state]);
        m.input = INPUT_A_PRESSED | INPUT_Z_DOWN;
        sController.stickMag = 64;
        EXPECT_EQ(handlers[state](&m), TRUE);
        EXPECT_EQ(m.action, ACT_LONG_JUMP);
        for (int environment = 0; environment < 2; environment++) {
            begin(actions[state]);
            m.input = INPUT_A_PRESSED | INPUT_B_PRESSED | INPUT_Z_DOWN | INPUT_FIRST_PERSON
                      | (environment ? INPUT_OFF_FLOOR : INPUT_STOMPED);
            EXPECT_EQ(handlers[state](&m), TRUE);
            EXPECT_EQ(m.action, environment ? ACT_FREEFALL : ACT_SHOCKWAVE_BOUNCE);
        }
        // Released controls keep the original animation and its ordinary finish action.
        begin(actions[state]);
        EXPECT_EQ(handlers[state](&m), FALSE);
        EXPECT_EQ(m.action, actions[state]);
        sAnimEnded = TRUE;
        EXPECT_EQ(handlers[state](&m), FALSE);
        EXPECT_EQ(m.action, state == 2 ? ACT_CROUCHING : ACT_IDLE);
    }
    return 1;
}
static int neutral_slide_stop_crouch(void) {
    const u32 actions[] = { ACT_BUTT_SLIDE_STOP, ACT_SLIDE_KICK_SLIDE_STOP, ACT_STOMACH_SLIDE_STOP };
    s32 (*handlers[])(struct MarioState *) = { act_butt_slide_stop, act_slide_kick_slide_stop,
                                               act_stomach_slide_stop };
    for (unsigned state = 0; state < 3; state++) {
        for (int direction = -1; direction <= 1; direction++) {
            for (int frame = 0; frame < 3; frame++) {
                begin(actions[state]);
                sTrackRolloutGround = TRUE;
                m.input = INPUT_Z_DOWN;
                sController.stickMag = 0;
                sMarioObject.header.gfx.animInfo.animFrame = frame * 3;
                mario_set_forward_vel(&m, direction * 16);
                EXPECT_EQ(handlers[state](&m), TRUE);
                EXPECT_EQ(m.action, ACT_START_CROUCHING);
                NEAR(m.forwardVel, direction * 16);
                EXPECT_EQ(sRolloutGroundSteps, 0);
                // Crouch entry keeps coasting; A backflips before it finishes.
                sAnimEnded = FALSE;
                f32 before = m.pos[2];
                EXPECT_EQ(act_start_crouching(&m), FALSE);
                EXPECT_EQ(m.action, ACT_START_CROUCHING);
                NEAR(m.pos[2] - before, m.forwardVel);
                if (direction) {
                    EXPECT_EQ(m.forwardVel < 0, direction < 0);
                    EXPECT_EQ(absf(m.forwardVel) > 0, TRUE);
                }
                m.input |= INPUT_A_PRESSED;
                EXPECT_EQ(act_start_crouching(&m), TRUE);
                EXPECT_EQ(m.action, ACT_BACKFLIP);
            }
        }
        // Holding the stick still selects the original moving crouch slide.
        begin(actions[state]);
        m.input = INPUT_Z_DOWN | INPUT_NONZERO_ANALOG;
        sController.stickMag = 64;
        mario_set_forward_vel(&m, 16);
        EXPECT_EQ(handlers[state](&m), TRUE);
        EXPECT_EQ(m.action, ACT_CROUCH_SLIDE);
        NEAR(m.forwardVel, 16);
    }
    return TRUE;
}

static int pole_grab_cancel_windows(void) {
    const int thresholds[] = { 2, 10 };
    const u32 buttons[] = { INPUT_A_PRESSED | INPUT_Z_PRESSED,
                            INPUT_A_DOWN | INPUT_Z_DOWN | INPUT_A_PRESSED | INPUT_Z_PRESSED };
    for (int kind = 0; kind < 2; kind++) {
        for (int frame = thresholds[kind] - 1; frame <= thresholds[kind] + 1; frame++) {
            for (int input = 0; input < 3; input++) {
                begin(ACT_HOLDING_POLE);
                m.marioObj->header.gfx.animInfo.animFrame = frame;
                m.input = input == 0 ? INPUT_A_PRESSED : 0;
                sController.stickY = input == 1 ? 17.0f : 0.0f;
                EXPECT_EQ(pole_grab_can_cancel(&m, buttons[kind], thresholds[kind]),
                          frame >= thresholds[kind] && input < 2);
            }
        }
    }
    return TRUE;
}

static int air_throw_landing_controls(void) {
    const u32 inputs[] = { INPUT_A_PRESSED,      INPUT_B_PRESSED,    INPUT_Z_DOWN,
                           INPUT_NONZERO_ANALOG, INPUT_FIRST_PERSON, INPUT_ABOVE_SLIDE };
    const u32 actions[] = { ACT_JUMP,    ACT_PUNCHING, ACT_START_CROUCHING,
                            ACT_WALKING, ACT_IDLE,     ACT_WALKING };
    for (int cycle = 0; cycle < 3; cycle++) {
        for (unsigned i = 0; i < sizeof(inputs) / sizeof(inputs[0]); i++) {
            // Already released, or releasing on this grounded update.
            for (int pending = 0; pending < 2; pending++) {
                begin(ACT_AIR_THROW_LAND);
                m.actionTimer = 3;
                set_mario_animation(&m, MARIO_ANIM_THROW_LIGHT_OBJECT);
                m.marioObj->header.gfx.animInfo.animFrame = 6;
                m.heldObj = pending ? &sHeld : NULL;
                m.input = inputs[i];
                sThrowReleases = 0;
                EXPECT_EQ(act_air_throw_land(&m), TRUE);
                EXPECT_EQ(m.action, actions[i]);
                EXPECT_EQ(m.heldObj == NULL, TRUE);
                EXPECT_EQ(sThrowReleases, pending);
            }
        }
        begin(ACT_AIR_THROW_LAND);
        m.heldObj = &sHeld;
        m.input = INPUT_A_PRESSED | INPUT_B_PRESSED | INPUT_Z_DOWN;
        sTrackRolloutGround = TRUE;
        mario_set_forward_vel(&m, 20.0f);
        sThrowReleases = 0;
        EXPECT_EQ(act_air_throw_land(&m), FALSE);
        EXPECT_EQ(m.action, ACT_AIR_THROW_LAND);
        EXPECT_EQ(m.heldObj == &sHeld, TRUE);
        EXPECT_EQ(sThrowReleases, 0);
        NEAR(m.forwardVel, 20.0f - GROUND_NEUTRAL_COAST_DECEL);
        for (int held = 0; held < 2; held++) {
            begin(ACT_AIR_THROW_LAND);
            m.heldObj = held ? &sHeld : NULL;
            m.input = INPUT_OFF_FLOOR | INPUT_A_PRESSED;
            EXPECT_EQ(act_air_throw_land(&m), TRUE);
            EXPECT_EQ(m.action, held ? ACT_HOLD_FREEFALL : ACT_FREEFALL);
            begin(ACT_AIR_THROW_LAND);
            m.input = INPUT_STOMPED | INPUT_A_PRESSED;
            EXPECT_EQ(act_air_throw_land(&m), TRUE);
            EXPECT_EQ(m.action, ACT_SHOCKWAVE_BOUNCE);
        }
    }
    return TRUE;
}

static int small_object_throw_speed(void) {
    const f32 nativeSpeeds[] = { 25.0f, 40.0f };
    const f32 carriedSpeeds[] = { -40.0f, 0.0f, 12.0f, 40.0f, 100.0f };
    for (int cycle = 0; cycle < 3; cycle++) {
        for (unsigned v = 0; v < sizeof(nativeSpeeds) / sizeof(nativeSpeeds[0]); v++) {
            for (unsigned mSpeed = 0; mSpeed < sizeof(carriedSpeeds) / sizeof(carriedSpeeds[0]);
                 mSpeed++) {
                sHeld.oForwardVel = carriedSpeeds[mSpeed];
                f32 expected =
                    (nativeSpeeds[v] + fmaxf(0.0f, carriedSpeeds[mSpeed]) * 0.5f) * 1.05f * 1.05f;
                NEAR(mario_small_object_throw_speed(&sHeld, nativeSpeeds[v]), expected);
                NEAR(sHeld.oForwardVel, carriedSpeeds[mSpeed]);
            }
        }
    }
    return TRUE;
}

static int air_twirl_landing_buffers(void) {
    const u32 inputs[] = { INPUT_A_PRESSED, INPUT_B_PRESSED, INPUT_Z_PRESSED };
    const u32 exits[] = { ACT_DOUBLE_JUMP, ACT_PUNCHING, ACT_START_CROUCHING };
    for (int button = 0; button < 3; button++) {
        for (int age = 0; age <= 6; age++) {
            for (int touchdownAge = 0; touchdownAge <= age; touchdownAge++) {
                begin(ACT_L_AIR_TWIRL);
                m.input = inputs[button];
                mario_update_move_buffers(&m);
                m.input = 0;
                for (int update = 0; update < touchdownAge; update++) {
                    mario_update_move_buffers(&m);
                }
                set_mario_action(&m, ACT_FREEFALL_LAND, 0);
                for (int update = touchdownAge; update < age; update++) {
                    mario_update_move_buffers(&m);
                }
                EXPECT_EQ(common_landing_cancels(&m, &sFreefallLandAction, set_jumping_action),
                          age < 6);
                EXPECT_EQ(m.action, age < 6 ? exits[button] : ACT_FREEFALL_LAND);
                if (age < 6) {
                    EXPECT_EQ(m.automaticAFrames | m.automaticBFrames | m.automaticZFrames, 0);
                }
            }
        }
    }
    for (int interruption = 0; interruption < 3; interruption++) {
        begin(ACT_L_AIR_TWIRL);
        m.input = INPUT_A_PRESSED | INPUT_B_PRESSED | INPUT_Z_PRESSED;
        mario_update_move_buffers(&m);
        if (interruption == 0) {
            set_mario_action(&m, ACT_SOFT_BONK, 0);
        } else {
            set_mario_action(&m, ACT_FREEFALL_LAND, 0);
            m.input = interruption == 1 ? INPUT_OFF_FLOOR : INPUT_STOMPED;
            EXPECT_EQ(common_landing_cancels(&m, &sFreefallLandAction, set_jumping_action), TRUE);
            EXPECT_EQ(m.action, interruption == 1 ? ACT_FREEFALL : ACT_SHOCKWAVE_BOUNCE);
        }
        EXPECT_EQ(m.automaticAFrames | m.automaticBFrames | m.automaticZFrames, 0);
    }
    // Ordinary freefall landings do not inherit air-twirl requests.
    begin(ACT_FREEFALL);
    m.input = INPUT_B_PRESSED;
    mario_update_move_buffers(&m);
    m.input = 0;
    set_mario_action(&m, ACT_FREEFALL_LAND, 0);
    EXPECT_EQ(common_landing_cancels(&m, &sFreefallLandAction, set_jumping_action), FALSE);
    return TRUE;
}

static int landing_controls(void) {
    struct LandingAction *landings[] = { &sJumpLandAction,       &sFreefallLandAction,
                                         &sSideFlipLandAction,   &sLongJumpLandAction,
                                         &sDoubleJumpLandAction, &sTripleJumpLandAction,
                                         &sBackflipLandAction };
    for (unsigned i = 0; i < 7; i++) {
        for (int final = 0; final < 2; final++) {
            for (int input = 0; input < 5; input++) {
                begin(ACT_JUMP_LAND);
                m.actionTimer = final ? landings[i]->numFrames - 1 : 0;
                m.forwardVel = 16;
                if (input == 0) {
                    m.input = INPUT_B_PRESSED;
                } else if (input == 1) {
                    m.input = INPUT_Z_DOWN;
                } else {
                    m.input = INPUT_A_PRESSED;
                }
                if (input >= 3) {
                    m.input |= INPUT_Z_DOWN;
                }
                if (input == 4) {
                    sController.stickMag = 64;
                }
                EXPECT_EQ(common_landing_cancels(&m, landings[i], set_jumping_action), TRUE);
                EXPECT_EQ(m.action, input == 0   ? ACT_MOVE_PUNCHING
                                    : input == 1 ? ACT_CROUCH_SLIDE
                                    : input == 3 ? ACT_BACKFLIP
                                    : input == 4
                                        ? ACT_LONG_JUMP
                                        : landing_jump_action(&m, landings[i]->aPressedAction));
            }
        }
    }
    for (int input = 0; input < 4; input++) {
        begin(ACT_LONG_JUMP_LAND_STOP);
        if (input == 0) {
            m.input = INPUT_B_PRESSED;
        } else if (input == 1) {
            m.input = INPUT_Z_DOWN;
        } else {
            m.input = INPUT_A_PRESSED;
        }
        if (input == 3) {
            m.input |= INPUT_Z_DOWN;
        }
        EXPECT_EQ(check_common_landing_cancels(&m, ACT_JUMP), TRUE);
        EXPECT_EQ(m.action, input == 0   ? ACT_PUNCHING
                            : input == 1 ? ACT_START_CROUCHING
                            : input == 3 ? ACT_BACKFLIP
                                         : ACT_JUMP);
    }
    begin(ACT_HOLD_JUMP_LAND);
    m.heldObj = &sHeld;
    m.input = INPUT_B_PRESSED;
    EXPECT_EQ(common_landing_cancels(&m, &sHoldJumpLandAction, set_jumping_action), TRUE);
    EXPECT_EQ(m.action, ACT_THROWING);
    begin(ACT_JUMP_LAND);
    m.input = INPUT_OFF_FLOOR | INPUT_A_PRESSED | INPUT_B_PRESSED;
    common_landing_cancels(&m, &sJumpLandAction, set_jumping_action);
    EXPECT_EQ(m.action, ACT_FREEFALL);
    return 1;
}
static int double_jump_wall_kick_followup(void) {
    const f32 speeds[] = { -11.0f, -10.0f, 0.0f, 10.0f, 11.0f };
    const u32 groundActions[] = { ACT_JUMP_LAND_STOP, ACT_WALKING, ACT_DECELERATING };

    for (int cycle = 0; cycle < 3; cycle++) {
        for (int kicks = 1; kicks <= 3; kicks++) {
            for (int route = 0; route < 5; route++) {
                for (int wing = 0; wing < 2; wing++) {
                    for (int speed = 0; speed < 5; speed++) {
                        begin(ACT_DOUBLE_JUMP);
                        for (int kick = 0; kick < kicks; kick++) {
                            set_mario_action(&m, ACT_AIR_HIT_WALL, 0);
                            EXPECT_EQ(m.wallKickFromDoubleJump, TRUE);
                            set_mario_action(&m, ACT_WALL_KICK_AIR, 0);
                        }
                        set_mario_action(&m, ACT_JUMP_LAND, route == 1 ? 2 : 0);
                        if (route >= 2) {
                            m.input = 0;
                            EXPECT_EQ(act_jump_land(&m), FALSE);
                            set_mario_action(&m, groundActions[route - 2], 0);
                        }
                        m.forwardVel = speeds[speed];
                        m.flags = wing ? MARIO_WING_CAP : 0;
                        m.input = route == 1 ? 0 : INPUT_A_PRESSED;
                        EXPECT_EQ(route >= 2 ? real_set_jump_from_landing(&m) : act_jump_land(&m),
                                  TRUE);
                        u32 expected = ACT_JUMP;
                        if (wing) {
                            expected = ACT_FLYING_TRIPLE_JUMP;
                        } else if (fabsf(speeds[speed]) > 10.0f) {
                            expected = ACT_TRIPLE_JUMP;
                        }
                        EXPECT_EQ(m.action, expected);
                        if (expected == ACT_TRIPLE_JUMP) {
                            EXPECT_EQ(m.actionArg, speeds[speed] < 0.0f ? ACT_ARG_TRIPLE_JUMP_BACKWARD
                                                                        : ACT_ARG_TRIPLE_JUMP_FORWARD);
                        }
                        EXPECT_EQ(m.wallKickFromDoubleJump, FALSE);
                    }
                }
            }
        }
    }

    const u32 ordinarySources[] = { ACT_JUMP, ACT_TRIPLE_JUMP, ACT_BACKFLIP, ACT_FREEFALL };
    for (int source = 0; source < 4; source++) {
        begin(ordinarySources[source]);
        set_mario_action(&m, ACT_AIR_HIT_WALL, 0);
        set_mario_action(&m, ACT_WALL_KICK_AIR, 0);
        set_mario_action(&m, ACT_JUMP_LAND, 0);
        m.forwardVel = 20.0f;
        m.input = INPUT_A_PRESSED;
        EXPECT_EQ(act_jump_land(&m), TRUE);
        EXPECT_EQ(m.action, ACT_DOUBLE_JUMP);
    }

    for (int strong = 0; strong < 2; strong++) {
        begin(ACT_DOUBLE_JUMP);
        set_mario_action(&m, ACT_AIR_HIT_WALL, 0);
        set_mario_action(&m, ACT_WALL_KICK_AIR, 0);
        set_mario_action(&m, ACT_JUMP_LAND, 0);
        sController.stickMag = strong ? 64.0f : 0.0f;
        m.input = INPUT_A_PRESSED | INPUT_Z_DOWN;
        EXPECT_EQ(act_jump_land(&m), TRUE);
        EXPECT_EQ(m.action, strong ? ACT_LONG_JUMP : ACT_BACKFLIP);
        EXPECT_EQ(m.wallKickFromDoubleJump, FALSE);
    }
    return 1;
}

static int double_jump_wall_kick_interruptions(void) {
    const u32 exits[] = { ACT_FREEFALL,        ACT_GROUND_POUND, ACT_JUMP_KICK,    ACT_SOFT_BONK,
                          ACT_BACKWARD_AIR_KB, ACT_LEDGE_GRAB,   ACT_WATER_PLUNGE, ACT_PUNCHING,
                          ACT_CROUCHING,       ACT_IDLE };
    const u32 stages[] = { ACT_AIR_HIT_WALL, ACT_WALL_KICK_AIR, ACT_JUMP_LAND };
    for (int stage = 0; stage < 3; stage++) {
        for (int exit = 0; exit < 10; exit++) {
            begin(ACT_DOUBLE_JUMP);
            set_mario_action(&m, ACT_AIR_HIT_WALL, 0);
            if (stage >= 1) {
                set_mario_action(&m, ACT_WALL_KICK_AIR, 0);
            }
            if (stage == 2) {
                set_mario_action(&m, ACT_JUMP_LAND, 0);
                m.doubleJumpTimer = 5;
            }
            EXPECT_EQ(m.action, stages[stage]);
            set_mario_action(&m, exits[exit], 0);
            EXPECT_EQ(m.wallKickFromDoubleJump, FALSE);
            set_mario_action(&m, ACT_AIR_HIT_WALL, 0);
            set_mario_action(&m, ACT_WALL_KICK_AIR, 0);
            set_mario_action(&m, ACT_JUMP_LAND, 0);
            m.input = INPUT_A_PRESSED;
            EXPECT_EQ(act_jump_land(&m), TRUE);
            EXPECT_EQ(m.action, ACT_DOUBLE_JUMP);
        }
    }
    // Touchdown during the contact window without a kick never earns a triple jump.
    begin(ACT_DOUBLE_JUMP);
    set_mario_action(&m, ACT_AIR_HIT_WALL, 0);
    set_mario_action(&m, ACT_FREEFALL_LAND, 0);
    EXPECT_EQ(m.wallKickFromDoubleJump, FALSE);

    for (int timer = 0; timer <= 1; timer++) {
        begin(ACT_DOUBLE_JUMP);
        set_mario_action(&m, ACT_AIR_HIT_WALL, 0);
        set_mario_action(&m, ACT_WALL_KICK_AIR, 0);
        set_mario_action(&m, ACT_JUMP_LAND, 0);
        m.doubleJumpTimer = timer;
        set_mario_action(&m, ACT_WALKING, 0);
        m.forwardVel = 20.0f;
        EXPECT_EQ(real_set_jump_from_landing(&m), TRUE);
        EXPECT_EQ(m.action, timer ? ACT_TRIPLE_JUMP : ACT_JUMP);
        EXPECT_EQ(m.wallKickFromDoubleJump, FALSE);
    }
    // Pole jumps share the kick action but have no double-jump wall contact.
    begin(ACT_DOUBLE_JUMP);
    set_mario_action(&m, ACT_TOP_OF_POLE_JUMP, 0);
    set_mario_action(&m, ACT_WALL_KICK_AIR, 0);
    EXPECT_EQ(m.wallKickFromDoubleJump, FALSE);
    return 1;
}

static int crouch_z_release_buffer(void) {
    const u32 actions[] = { ACT_IDLE, ACT_WALKING };
    for (u32 action = 0; action < sizeof(actions) / sizeof(actions[0]); action++) {
        for (int held = 0; held < 2; held++) {
            begin(actions[action]);
            for (int cycle = 0; cycle < 4; cycle++) {
                m.input = 0;
                sController.buttonDown = sController.buttonPressed = Z_TRIG;
                update_mario_button_inputs(&m);
                EXPECT_EQ(m.zCrouchBuffered, TRUE);
                EXPECT_EQ(m.zHeldFrames, 0);
                m.input = 0;
                sController.buttonPressed = 0;
                sController.buttonDown = held ? Z_TRIG : 0;
                update_mario_button_inputs(&m);
                EXPECT_EQ(m.zCrouchBuffered, held);
                EXPECT_EQ(m.zHeldFrames, held);
                m.input = 0;
                sController.buttonDown = 0;
                update_mario_button_inputs(&m);
                EXPECT_EQ(m.zCrouchBuffered, FALSE);
                EXPECT_EQ(m.zHeldFrames, 0);
            }
        }
    }
    return TRUE;
}

static int buffered_z_landing_jumps(void) {
    const u32 air[] = { ACT_TRIPLE_JUMP, ACT_SIDE_FLIP, ACT_LONG_JUMP, ACT_WALL_KICK_AIR };
    const u32 land[] = { ACT_TRIPLE_JUMP_LAND, ACT_SIDE_FLIP_LAND, ACT_LONG_JUMP_LAND, ACT_JUMP_LAND };
    struct LandingAction *tables[] = { &sTripleJumpLandAction, &sSideFlipLandAction,
                                       &sLongJumpLandAction, &sJumpLandAction };
    const f32 stickMagnitudes[] = { 0, 16, 64 };
    for (int route = 0; route < 4; route++) {
        int window = 6;
        for (int backwards = 0; backwards < 2; backwards++) {
            for (int stick = 0; stick < 3; stick++) {
                for (int age = 0; age <= window; age++) {
                    begin(air[route]);
                    sController.stickMag = stickMagnitudes[stick];
                    sController.buttonDown = A_BUTTON | Z_TRIG;
                    sController.buttonPressed = A_BUTTON;
                    update_mario_button_inputs(&m);
                    for (int frame = 0; frame < age; frame++) {
                        m.input = 0;
                        sController.buttonDown = Z_TRIG;
                        sController.buttonPressed = 0;
                        update_mario_button_inputs(&m);
                    }
                    u32 queued = m.landingJumpInputFrames;
                    EXPECT_EQ(queued, age < window ? window - age : 0);
                    set_mario_action(&m, land[route], queued);
                    m.forwardVel = backwards ? -16 : 16;
                    EXPECT_EQ(common_landing_cancels(&m, tables[route], set_jumping_action), TRUE);
                    u32 jump = stick == 2 || (route == 2 && backwards && stick != 0) ? ACT_LONG_JUMP
                                                                                     : ACT_BACKFLIP;
                    EXPECT_EQ(m.action, age < window ? jump : ACT_CROUCH_SLIDE);
                    EXPECT_EQ(m.landingJumpInputFrames, 0);
                    EXPECT_EQ(m.actionArg, 0);
                }
            }
        }
    }
    // Repeated long jumps consume each request; held Z alone does not jump again.
    begin(ACT_LONG_JUMP);
    sController.stickMag = 64;
    for (int cycle = 0; cycle < 3; cycle++) {
        m.input = 0;
        sController.buttonDown = A_BUTTON | Z_TRIG;
        sController.buttonPressed = A_BUTTON;
        update_mario_button_inputs(&m);
        m.input = 0;
        sController.buttonDown = Z_TRIG;
        sController.buttonPressed = 0;
        update_mario_button_inputs(&m);
        set_mario_action(&m, ACT_LONG_JUMP_LAND, m.landingJumpInputFrames);
        m.forwardVel = 16;
        EXPECT_EQ(common_landing_cancels(&m, &sLongJumpLandAction, set_jumping_action), TRUE);
        EXPECT_EQ(m.action, ACT_LONG_JUMP);
        EXPECT_EQ(m.landingJumpInputFrames, 0);
    }
    set_mario_action(&m, ACT_LONG_JUMP_LAND, 0);
    EXPECT_EQ(common_landing_cancels(&m, &sLongJumpLandAction, set_jumping_action), TRUE);
    EXPECT_EQ(m.action, ACT_CROUCH_SLIDE);
    // Buffered A does not bypass off-floor or stomp reactions, or reuse the chord's Z edge.
    for (int hazard = 0; hazard < 2; hazard++) {
        begin(ACT_LONG_JUMP_LAND);
        m.actionArg = 2;
        m.input = INPUT_Z_DOWN | (hazard ? INPUT_STOMPED : INPUT_OFF_FLOOR);
        EXPECT_EQ(common_landing_cancels(&m, &sLongJumpLandAction, set_jumping_action), TRUE);
        EXPECT_EQ(m.action, hazard ? ACT_SHOCKWAVE_BOUNCE : ACT_FREEFALL);
    }
    begin(ACT_LONG_JUMP_LAND);
    m.actionArg = 2;
    m.input = INPUT_Z_DOWN | INPUT_Z_PRESSED;
    EXPECT_EQ(common_landing_cancels(&m, &sLongJumpLandAction, set_jumping_action), TRUE);
    EXPECT_EQ(m.action, ACT_BACKFLIP);
    EXPECT_EQ(m.input & INPUT_Z_PRESSED, 0);
    EXPECT_EQ(m.input & INPUT_Z_DOWN, INPUT_Z_DOWN);
    return 1;
}
static int backward_long_jump_crouch_window(void) {
    for (int age = 0; age <= 5; age++) {
        for (int neutral = FALSE; neutral <= TRUE; neutral++) {
            begin(ACT_LONG_JUMP);
            m.forwardVel = -20;
            sController.stickMag = 16;
            sController.buttonDown = Z_TRIG;
            sController.buttonPressed = 0;
            update_mario_button_inputs(&m);
            set_mario_action(&m, ACT_LONG_JUMP_LAND, 0);
            set_mario_action(&m, ACT_LONG_JUMP_LAND_STOP, 0);
            set_mario_action(&m, ACT_CROUCHING, 0);
            EXPECT_EQ(m.backwardLongJumpCrouchFrames, 5);
            for (int frame = 0; frame < age; frame++) {
                m.input = INPUT_Z_DOWN | INPUT_NONZERO_ANALOG;
                EXPECT_EQ(act_crouching(&m), FALSE);
                EXPECT_EQ(m.action, ACT_CROUCHING);
                m.input = 0;
                update_mario_button_inputs(&m);
            }
            if (neutral) {
                sController.stickMag = 0;
                update_mario_button_inputs(&m);
                sController.stickMag = 16; // Re-tilting does not restore crouch eligibility.
            }
            m.input = INPUT_A_PRESSED | INPUT_Z_DOWN | INPUT_NONZERO_ANALOG;
            EXPECT_EQ(act_crouching(&m), TRUE);
            EXPECT_EQ(m.action, age < 5 && !neutral ? ACT_LONG_JUMP : ACT_BACKFLIP);
        }
    }
    begin(ACT_CROUCHING);
    sController.stickMag = 16;
    m.input = INPUT_A_PRESSED | INPUT_Z_DOWN;
    EXPECT_EQ(act_crouching(&m), TRUE);
    EXPECT_EQ(m.action, ACT_BACKFLIP);
    begin(ACT_LONG_JUMP);
    m.forwardVel = -20;
    sController.stickMag = 16;
    update_mario_button_inputs(&m);
    set_mario_action(&m, ACT_FREEFALL, 0);
    set_mario_action(&m, ACT_CROUCHING, 0);
    EXPECT_EQ(m.backwardLongJumpCrouchFrames, 0);
    return TRUE;
}

static int bowser_release_cancels(void) {
    const u16 presses[] = { INPUT_A_PRESSED, INPUT_B_PRESSED, INPUT_Z_PRESSED };
    const u16 masks[] = { INPUT_A_PRESSED, INPUT_B_PRESSED, INPUT_Z_DOWN };
    const u32 actions[] = { ACT_JUMP, ACT_PUNCHING, ACT_START_CROUCHING };
    for (int button = 0; button < 3; button++) {
        for (int age = 0; age <= 6; age++) {
            begin(ACT_RELEASING_BOWSER);
            m.actionTimer = 2;
            m.input = presses[button];
            mario_update_move_buffers(&m);
            m.input = 0;
            for (int update = 0; update < age; update++) {
                mario_update_move_buffers(&m);
            }
            m.marioObj->header.gfx.animInfo.animID = MARIO_ANIM_RELEASE_BOWSER;
            m.marioObj->header.gfx.animInfo.animFrame = 25;
            sIdleCancelMask = masks[button];
            sIdleCancelAction = actions[button];
            EXPECT_EQ(act_releasing_bowser(&m), FALSE);
            EXPECT_EQ(m.action, ACT_RELEASING_BOWSER);
            m.marioObj->header.gfx.animInfo.animFrame = 26;
            EXPECT_EQ(act_releasing_bowser(&m), age < 6);
            EXPECT_EQ(m.action, age < 6 ? actions[button] : ACT_RELEASING_BOWSER);
            if (age < 6) {
                EXPECT_EQ(m.automaticAFrames | m.automaticBFrames | m.automaticZFrames, 0);
            }
        }
    }
    begin(ACT_RELEASING_BOWSER);
    m.heldObj = &sHeld;
    act_releasing_bowser(&m);
    EXPECT_EQ(sThrowReleases, 1);
    act_releasing_bowser(&m);
    EXPECT_EQ(sThrowReleases, 1);
    return TRUE;
}

static int additional_input_buffers(void) {
    for (int age = 0; age <= 6; age++) {
        begin(ACT_BACKFLIP_LAND_STOP);
        m.framesSinceA = age;
        m.input = INPUT_Z_DOWN;
        act_backflip_land_stop(&m);
        EXPECT_EQ(m.action, age < 6 ? ACT_BACKFLIP : ACT_START_CROUCHING);
    }
    for (int age = 0; age <= 6; age++) {
        for (int held = 0; held < 2; held++) {
            for (int z = 0; z < 2; z++) {
                begin(ACT_BACKFLIP);
                sController.buttonPressed = sController.buttonDown = A_BUTTON;
                update_mario_button_inputs(&m);
                for (int frame = 0; frame < age; frame++) {
                    m.input = 0;
                    sController.buttonPressed = 0;
                    sController.buttonDown = held ? A_BUTTON : 0;
                    update_mario_button_inputs(&m);
                }
                set_mario_action(&m, ACT_BACKFLIP_LAND, m.landingJumpInputFrames);
                m.input = 0;
                sController.buttonPressed = 0;
                sController.buttonDown = (held ? A_BUTTON : 0) | (z ? Z_TRIG : 0);
                update_mario_button_inputs(&m);
                act_backflip_land(&m);
                u32 expectedAction;
                if (age < 6) {
                    expectedAction = z ? ACT_BACKFLIP : ACT_JUMP;
                } else {
                    expectedAction = z ? ACT_START_CROUCHING : ACT_BACKFLIP_LAND;
                }
                EXPECT_EQ(m.action, expectedAction);
            }
        }
    }
    const u32 air[] = { ACT_FREEFALL,   ACT_HOLD_FREEFALL, ACT_HOLD_JUMP,
                        ACT_STEEP_JUMP, ACT_WATER_JUMP,    ACT_TOP_OF_POLE_JUMP };
    for (unsigned route = 0; route < sizeof(air) / sizeof(*air); route++) {
        for (int age = 0; age <= 6; age++) {
            begin(air[route]);
            sController.buttonPressed = sController.buttonDown = A_BUTTON;
            update_mario_button_inputs(&m);
            for (int frame = 0; frame < age; frame++) {
                m.input = 0;
                sController.buttonPressed = sController.buttonDown = 0;
                update_mario_button_inputs(&m);
            }
            m.input = 0;
            set_mario_action(&m, route == 1 || route == 2 ? ACT_HOLD_FREEFALL_LAND : ACT_FREEFALL_LAND,
                             m.landingJumpInputFrames);
            EXPECT_EQ(!!(m.input & INPUT_A_PRESSED), age < 6);
            EXPECT_EQ(m.landingJumpInputFrames, 0);
            // Contact ends the air handler; the landing consumes the request next update.
            m.input = 0;
            sController.buttonPressed = sController.buttonDown = 0;
            update_mario_button_inputs(&m);
            if (age < 6) {
                common_landing_cancels(
                    &m, route == 1 || route == 2 ? &sHoldFreefallLandAction : &sFreefallLandAction,
                    set_jumping_action);
                EXPECT_EQ(m.action, route == 1 || route == 2 ? ACT_HOLD_JUMP : ACT_DOUBLE_JUMP);
            }
        }
    }
    // A Z edge during the last two locked updates fires on the next eligible update.
    begin(ACT_JUMP);
    m.groundPoundBounceCooldown = 2;
    sController.buttonPressed = sController.buttonDown = Z_TRIG;
    update_mario_button_inputs(&m);
    EXPECT_EQ(m.groundPoundInputFrames, 3);
    EXPECT_EQ(!!(m.input & INPUT_Z_PRESSED), FALSE);
    m.input = 0;
    sController.buttonPressed = sController.buttonDown = 0;
    update_mario_button_inputs(&m);
    EXPECT_EQ(!!(m.input & INPUT_Z_PRESSED), FALSE);
    m.input = 0;
    update_mario_button_inputs(&m);
    EXPECT_EQ(!!(m.input & INPUT_Z_PRESSED), TRUE);
    act_jump(&m);
    EXPECT_EQ(m.action, ACT_GROUND_POUND);
    EXPECT_EQ(m.groundPoundInputFrames, 0);
    const u32 recoveries[] = { ACT_DIVE_PICKING_UP, ACT_THROWING, ACT_HEAVY_THROW };
    for (unsigned route = 0; route < sizeof(recoveries) / sizeof(*recoveries); route++) {
        for (int age = 0; age <= 6; age++) {
            begin(recoveries[route]);
            m.input = INPUT_A_PRESSED | INPUT_B_PRESSED;
            mario_update_move_buffers(&m);
            m.input = 0;
            for (int frame = 0; frame < age; frame++) {
                mario_update_move_buffers(&m);
            }
            EXPECT_EQ(m.objectRecoveryAFrames > 0, age < 6);
            EXPECT_EQ(m.objectRecoveryBFrames > 0, age < 6);
            if (route > 0) {
                struct Animation animation = { .loopEnd = 20 };
                m.marioObj->header.gfx.animInfo.animID = MARIO_ANIM_GROUND_THROW;
                m.marioObj->header.gfx.animInfo.curAnim = &animation;
                m.marioObj->header.gfx.animInfo.animFrame = 14;
                sIdleCancelAction = ACT_JUMP;
                sIdleCancelMask = INPUT_A_PRESSED;
                EXPECT_EQ(cancel_throw_recovery(&m, MARIO_ANIM_GROUND_THROW, 5), FALSE);
                EXPECT_EQ(m.input, 0);
                m.marioObj->header.gfx.animInfo.animFrame = 15;
                EXPECT_EQ(cancel_throw_recovery(&m, MARIO_ANIM_GROUND_THROW, 5), age < 6);
                if (age < 6) {
                    EXPECT_EQ(m.action, ACT_JUMP);
                }
            }
            set_mario_action(&m, ACT_IDLE, 0);
            EXPECT_EQ(m.objectRecoveryAFrames | m.objectRecoveryBFrames, 0);
        }
    }
    const u32 attached[] = { ACT_GRAB_POLE_SLOW, ACT_GRAB_POLE_FAST, ACT_TOP_OF_POLE_TRANSITION,
                             ACT_START_HANGING, ACT_LEDGE_CLIMB_DOWN };
    for (unsigned route = 0; route < sizeof(attached) / sizeof(*attached); route++) {
        for (int age = 0; age <= 3; age++) {
            begin(attached[route]);
            m.input = INPUT_A_PRESSED | INPUT_B_PRESSED | INPUT_Z_PRESSED;
            mario_update_move_buffers(&m);
            m.input = 0;
            for (int frame = 0; frame < age; frame++) {
                mario_update_move_buffers(&m);
            }
            mario_apply_automatic_buffer(&m);
            EXPECT_EQ(!!(m.input & INPUT_Z_PRESSED), age < 3);
            EXPECT_EQ(!!(m.input & INPUT_A_PRESSED), age < 3 && route != 4);
            EXPECT_EQ(!!(m.input & INPUT_B_PRESSED), age < 3 && route == 3);
            set_mario_action(&m, ACT_FREEFALL, 0);
            EXPECT_EQ(m.automaticAFrames | m.automaticBFrames | m.automaticZFrames, 0);
        }
    }
    // Fast climb retains released jump/attack requests for six updates.
    for (int age = 0; age <= 6; age++) {
        begin(ACT_LEDGE_CLIMB_FAST);
        m.input = INPUT_A_PRESSED | INPUT_B_PRESSED | INPUT_Z_PRESSED;
        mario_update_move_buffers(&m);
        m.input = 0;
        for (int frame = 0; frame < age; frame++) {
            mario_update_move_buffers(&m);
        }
        EXPECT_EQ(m.automaticAFrames > 0, age < 6);
        EXPECT_EQ(m.automaticBFrames > 0, age < 6);
        EXPECT_EQ(m.automaticZFrames > 0, age < 3);
        set_mario_action(&m, ACT_IDLE, 0);
        EXPECT_EQ(m.automaticAFrames | m.automaticBFrames | m.automaticZFrames, 0);
    }
    const u32 unbuffered[] = { ACT_HOLDING_BOWSER, ACT_IN_CANNON, ACT_WALKING, ACT_CROUCHING };
    for (unsigned route = 0; route < sizeof(unbuffered) / sizeof(*unbuffered); route++) {
        begin(unbuffered[route]);
        m.input = INPUT_A_PRESSED | INPUT_B_PRESSED | INPUT_Z_PRESSED;
        mario_update_move_buffers(&m);
        EXPECT_EQ(m.objectRecoveryAFrames | m.objectRecoveryBFrames | m.automaticAFrames
                      | m.automaticBFrames | m.automaticZFrames | m.groundPoundInputFrames,
                  0);
    }
    return TRUE;
}

static int transition_input_windows(void) {
    const u32 airActions[] = { ACT_TRIPLE_JUMP, ACT_SIDE_FLIP, ACT_LONG_JUMP, ACT_WALL_KICK_AIR };
    const u32 landActions[] = { ACT_TRIPLE_JUMP_LAND, ACT_SIDE_FLIP_LAND, ACT_LONG_JUMP_LAND,
                                ACT_JUMP_LAND };
    struct LandingAction *landings[] = { &sTripleJumpLandAction, &sSideFlipLandAction,
                                         &sLongJumpLandAction, &sJumpLandAction };
    for (int action = 0; action < 4; action++) {
        int window = 6;
        for (int age = 0; age <= window; age++) {
            begin(airActions[action]);
            m.input = 0;
            sController.buttonDown = A_BUTTON;
            sController.buttonPressed = A_BUTTON;
            update_mario_button_inputs(&m);
            EXPECT_EQ(m.landingJumpInputFrames, window);
            for (int update = 0; update < age; update++) {
                m.input = 0;
                sController.buttonDown = sController.buttonPressed = 0;
                update_mario_button_inputs(&m);
            }
            u32 queued = m.landingJumpInputFrames;
            set_mario_action(&m, landActions[action], queued);
            m.input = 0;
            s32 jumped = common_landing_cancels(&m, landings[action], set_jumping_action);
            EXPECT_EQ(jumped, age < window);
            EXPECT_EQ(m.action == landActions[action], age == window);
            if (action == 3 && age < window) {
                EXPECT_EQ(m.action, ACT_DOUBLE_JUMP);
            }
        }
    }
    // A takeoff press and an interrupted airborne press cannot become a landing request.
    begin(ACT_IDLE);
    sController.buttonDown = A_BUTTON;
    sController.buttonPressed = A_BUTTON;
    update_mario_button_inputs(&m);
    EXPECT_EQ(m.landingJumpInputFrames, 0);
    set_mario_action(&m, ACT_TRIPLE_JUMP, 0);
    EXPECT_EQ(m.landingJumpInputFrames, 0);
    set_mario_action(&m, ACT_WALL_KICK_AIR, 0);
    EXPECT_EQ(m.landingJumpInputFrames, 0);
    m.input = 0;
    sController.buttonPressed = 0;
    update_mario_button_inputs(&m);
    EXPECT_EQ(m.landingJumpInputFrames, 0);
    m.input = 0;
    sController.buttonPressed = A_BUTTON;
    update_mario_button_inputs(&m);
    EXPECT_EQ(m.landingJumpInputFrames, 6);
    set_mario_action(&m, ACT_FREEFALL, 0);
    EXPECT_EQ(m.landingJumpInputFrames, 0);
    // A used before the wall kick, including a held initiation press, is not its landing jump.
    begin(ACT_TRIPLE_JUMP);
    sController.buttonDown = A_BUTTON;
    sController.buttonPressed = A_BUTTON;
    update_mario_button_inputs(&m);
    EXPECT_EQ(m.landingJumpInputFrames, 6);
    set_mario_action(&m, ACT_WALL_KICK_AIR, 0);
    EXPECT_EQ(m.landingJumpInputFrames, 0);
    m.input = 0;
    sController.buttonPressed = 0;
    update_mario_button_inputs(&m);
    EXPECT_EQ(m.landingJumpInputFrames, 0);
    set_mario_action(&m, ACT_JUMP_LAND, 0);
    EXPECT_EQ(common_landing_cancels(&m, &sJumpLandAction, set_jumping_action), FALSE);
    EXPECT_EQ(m.action, ACT_JUMP_LAND);
    // The airborne wall press survives exactly one following input update.
    begin(ACT_LONG_JUMP);
    sController.buttonDown = A_BUTTON;
    sController.buttonPressed = A_BUTTON;
    update_mario_button_inputs(&m);
    EXPECT_EQ(m.wallKickInputFrames, 3);
    m.input = 0;
    sController.buttonDown = sController.buttonPressed = 0;
    update_mario_button_inputs(&m);
    EXPECT_EQ(m.wallKickInputFrames, 2);
    m.input = 0;
    update_mario_button_inputs(&m);
    EXPECT_EQ(m.wallKickInputFrames, 1);
    m.input = 0;
    update_mario_button_inputs(&m);
    EXPECT_EQ(m.wallKickInputFrames, 0);
    begin(ACT_LONG_JUMP);
    sController.buttonDown = A_BUTTON;
    sController.buttonPressed = A_BUTTON;
    update_mario_button_inputs(&m);
    set_mario_action(&m, ACT_DIVE, 0);
    EXPECT_EQ(m.wallKickInputFrames, 0);
    return 1;
}
static s32 burning_test_update(void) {
    s32 redispatch;
    s32 advanced = FALSE;
    do {
        switch (m.action) {
            case ACT_BURNING_GROUND:
                redispatch = act_burning_ground(&m);
                break;
            case ACT_BURNING_JUMP:
                redispatch = act_burning_jump(&m);
                break;
            case ACT_BURNING_FALL:
                redispatch = act_burning_fall(&m);
                break;
            default:
                return advanced;
        }
        advanced = TRUE;
    } while (redispatch);
    return TRUE;
}

static int burning_episode_damage(void) {
    static struct TestBodyState body;
    for (int cycle = 0; cycle < 3; cycle++) {
        for (int capless = 0; capless < 2; capless++) {
            for (int pattern = 0; pattern < 5; pattern++) {
                begin(pattern == 1 ? ACT_BURNING_JUMP : ACT_BURNING_GROUND);
                m.marioBodyState = &body;
                m.waterLevel = FLOOR_LOWER_LIMIT;
                m.flags = capless ? 0 : MARIO_CAP_ON_HEAD;
                for (int update = 0; update < 200; update++) {
                    s32 before = sMarioObject.oMarioBurnTimer;
                    m.input = pattern >= 2 && before < 200 && update % 5 == 0 ? INPUT_A_PRESSED : 0;
                    sStep =
                        pattern == 4 && update % 7 == 0 ? GROUND_STEP_LEFT_GROUND : GROUND_STEP_NONE;
                    if (pattern == 1) {
                        sAirStepResult = before >= 220 ? AIR_STEP_LANDED : AIR_STEP_NONE;
                    } else {
                        sAirStepResult = update % 5 == 4 ? AIR_STEP_LANDED : AIR_STEP_NONE;
                    }
                    if (pattern == 3 && m.action == ACT_BURNING_JUMP && update % 5 == 2) {
                        m.action = ACT_BURNING_FALL;
                    }
                    sHealthRegenFrames = 100;
                    EXPECT_EQ(burning_test_update(), TRUE);
                    s32 progress = MIN(sMarioObject.oMarioBurnTimer, 160);
                    EXPECT_EQ(m.health, 0x880 - (progress * 3 / 160) * 0x100);
                    if (m.action == ACT_WALKING) {
                        break;
                    }
                    update_mario_health(&m);
                    if (progress * 3 / 160 > MIN(before, 160) * 3 / 160) {
                        EXPECT_EQ(sHealthRegenFrames, 0);
                    }
                }
                EXPECT_EQ(m.action, ACT_WALKING);
                EXPECT_EQ(m.health, 0x580);
                // Recovery starts a new burn with a fresh budget.
                m.action = ACT_BURNING_GROUND;
                sMarioObject.oMarioBurnTimer = 0;
                sStep = GROUND_STEP_NONE;
                m.input = 0;
                for (int update = 0; update < 81; update++) {
                    burning_test_update();
                }
                EXPECT_EQ(m.health, 0x280);
                EXPECT_EQ(m.action, ACT_WALKING);
            }
        }
    }
    // Jump redispatch and airborne touchdown each advance one damage step.
    begin(ACT_BURNING_GROUND);
    m.marioBodyState = &body;
    m.waterLevel = FLOOR_LOWER_LIMIT;
    m.input = INPUT_A_PRESSED;
    sAirStepResult = AIR_STEP_LANDED;
    burning_test_update();
    EXPECT_EQ(m.action, ACT_BURNING_GROUND);
    EXPECT_EQ(sMarioObject.oMarioBurnTimer, 3);
    EXPECT_EQ(m.health, 0x880);
    return TRUE;
}

static int burning_extinguish_and_death(void) {
    static struct TestBodyState body;
    const u32 actions[] = { ACT_BURNING_GROUND, ACT_BURNING_JUMP, ACT_BURNING_FALL };
    const s32 progress[] = { 2, 3, 80, 159, 160, 163 };
    for (int cycle = 0; cycle < 3; cycle++) {
        for (unsigned a = 0; a < sizeof(actions) / sizeof(actions[0]); a++) {
            for (unsigned p = 0; p < sizeof(progress) / sizeof(progress[0]); p++) {
                begin(actions[a]);
                m.marioBodyState = &body;
                m.waterLevel = FLOOR_LOWER_LIMIT;
                sMarioObject.oMarioBurnTimer = progress[p];
                m.health -= (MIN(progress[p], 160) * 3 / 160) * 0x100;
                s32 health = m.health;
                sObj.oDamageOrCoinValue = 1;
                m.input = INPUT_A_DOWN;
                interact_coin(&m, INTERACT_COIN, &sObj);
                EXPECT_EQ(sMarioObject.oMarioBurnTimer, 0);
                EXPECT_EQ(m.health, health);
                EXPECT_EQ(m.action, a == 0 ? ACT_WALKING : ACT_FREEFALL);
                EXPECT_EQ(m.burnCoinJumpPending, a == 0 ? 6 : 0);
                EXPECT_EQ(burning_test_update(), FALSE);
                EXPECT_EQ(m.health, health);
                // Soaked/water interruption discards the unpaid remainder too.
                m.action = actions[a];
                sMarioObject.oMarioBurnTimer = progress[p];
                mario_apply_soaked_status(&m);
                mario_update_soaked_status(&m);
                EXPECT_EQ(m.health, health);
                EXPECT_EQ(sMarioObject.oMarioBurnTimer, 0);
                EXPECT_EQ(burning_test_update(), FALSE);
            }
            begin(actions[a]);
            m.marioBodyState = &body;
            m.waterLevel = FLOOR_LOWER_LIMIT;
            m.health = 0x105;
            sMarioObject.oMarioBurnTimer = 53;
            sAirStepResult = AIR_STEP_NONE;
            burning_test_update();
            if (a == 0) {
                EXPECT_EQ(m.action, ACT_STANDING_DEATH);
            } else {
                EXPECT_EQ(m.health, 0xFF);
            }
        }
    }
    // A ground step crossing 159->161 must settle its last whole wedge.
    begin(ACT_BURNING_GROUND);
    m.marioBodyState = &body;
    m.waterLevel = FLOOR_LOWER_LIMIT;
    sMarioObject.oMarioBurnTimer = 159;
    m.health = 0x880 - (159 * 3 / 160) * 0x100;
    burning_test_update();
    EXPECT_EQ(m.health, 0x580);
    EXPECT_EQ(m.action, ACT_WALKING);
    begin(ACT_BURNING_GROUND);
    m.marioBodyState = &body;
    m.waterLevel = FLOOR_LOWER_LIMIT;
    sMarioObject.oMarioBurnTimer = 159;
    m.health = 0x102;
    burning_test_update();
    EXPECT_EQ(m.action, ACT_STANDING_DEATH);
    // The native grounded water exit also stops damage before this update's debit.
    begin(ACT_BURNING_GROUND);
    m.marioBodyState = &body;
    m.waterLevel = 51;
    sMarioObject.oMarioBurnTimer = 80;
    m.health = 0x700;
    burning_test_update();
    EXPECT_EQ(m.action, ACT_WALKING);
    EXPECT_EQ(m.health, 0x700);
    return TRUE;
}

static int burning_coin_jump(void) {
    for (int cycle = 0; cycle < 3; cycle++) {
        begin(ACT_BURNING_GROUND);
        m.input = INPUT_A_DOWN;
        sObj.oDamageOrCoinValue = 1;
        sMarioObject.oMarioBurnTimer = 100;
        m.particleFlags |= PARTICLE_FIRE;
        interact_coin(&m, INTERACT_COIN, &sObj);
        EXPECT_EQ(m.action, ACT_WALKING);
        EXPECT_EQ(m.burnCoinJumpPending, 6);
        EXPECT_EQ(m.numCoins, 1);
        EXPECT_EQ(sMarioObject.oMarioBurnTimer, 0);
        EXPECT_EQ(try_burning_coin_recovery_jump(&m), TRUE);
        EXPECT_EQ(m.action, ACT_JUMP);
        EXPECT_EQ(m.burnCoinJumpPending, 0);
        EXPECT_EQ(try_burning_coin_recovery_jump(&m), FALSE);
    }
    begin(ACT_BURNING_GROUND);
    m.input = INPUT_A_DOWN;
    sObj.oDamageOrCoinValue = 1;
    interact_coin(&m, INTERACT_COIN, &sObj);
    m.input = 0;
    EXPECT_EQ(try_burning_coin_recovery_jump(&m), TRUE);
    EXPECT_EQ(m.action, ACT_JUMP);
    begin(ACT_BURNING_GROUND);
    m.input = INPUT_A_DOWN;
    sObj.oDamageOrCoinValue = 1;
    interact_coin(&m, INTERACT_COIN, &sObj);
    set_mario_action(&m, ACT_CROUCH_SLIDE, 0);
    EXPECT_EQ(m.burnCoinJumpPending, 0);
    begin(ACT_BURNING_JUMP);
    m.input = INPUT_A_DOWN;
    sObj.oDamageOrCoinValue = 1;
    interact_coin(&m, INTERACT_COIN, &sObj);
    EXPECT_EQ(m.action, ACT_FREEFALL);
    EXPECT_EQ(m.burnCoinJumpPending, 0);
    for (int age = 0; age <= 6; age++) {
        begin(ACT_BURNING_GROUND);
        m.framesSinceA = age;
        m.input = 0;
        sObj.oDamageOrCoinValue = 1;
        interact_coin(&m, INTERACT_COIN, &sObj);
        EXPECT_EQ(m.burnCoinJumpPending, age < 6 ? 6 - age : 0);
        EXPECT_EQ(try_burning_coin_recovery_jump(&m), age < 6);
    }
    return 1;
}
static int airborne_sand(void) {
    begin(ACT_IDLE);
    sFloor.type = SURFACE_INSTANT_QUICKSAND;
    m.pos[1] = 5;
    m.quicksandDepth = 80;
    EXPECT_EQ(mario_update_quicksand(&m, .5f), FALSE);
    EXPECT_EQ(m.action, ACT_IDLE);
    NEAR(m.quicksandDepth, 0);
    m.action = ACT_FREEFALL;
    m.pos[1] = 0;
    m.quicksandDepth = 80;
    sMarioObject.header.gfx.pos[1] = 123;
    sink_mario_in_quicksand(&m);
    NEAR(sMarioObject.header.gfx.pos[1], 123);
    m.action = ACT_IDLE;
    EXPECT_EQ(mario_update_quicksand(&m, .5f), TRUE);
    EXPECT_EQ(m.action, ACT_QUICKSAND_DEATH);
    begin(ACT_IDLE);
    sFloor.type = SURFACE_DEEP_QUICKSAND;
    m.quicksandDepth = 159;
    EXPECT_EQ(mario_update_quicksand(&m, 1), TRUE);
    begin(ACT_SQUISHED);
    m.pos[1] = 10;
    EXPECT_EQ(act_squished(&m), TRUE);
    EXPECT_EQ(m.action, ACT_FREEFALL);
    NEAR(m.pos[1], 10);
    begin(ACT_SQUISHED);
    m.ceilHeight = 200;
    EXPECT_EQ(act_squished(&m), TRUE);
    EXPECT_EQ(m.action, ACT_IDLE);
    return 1;
}
static int held_landing_controls(void) {
    s32 (*landings[])(struct MarioState *) = { act_hold_jump_land_stop, act_hold_freefall_land_stop };
    for (unsigned i = 0; i < 2; i++) {
        for (int input = 0; input < 4; input++) {
            begin(i ? ACT_HOLD_FREEFALL_LAND_STOP : ACT_HOLD_JUMP_LAND_STOP);
            m.heldObj = &sHeld;
            m.input = input == 0   ? INPUT_B_PRESSED | INPUT_NONZERO_ANALOG
                      : input == 1 ? INPUT_Z_DOWN
                      : input == 2 ? INPUT_A_PRESSED
                                   : INPUT_OFF_FLOOR | INPUT_A_PRESSED;
            EXPECT_EQ(landings[i](&m), TRUE);
            EXPECT_EQ(m.action, input == 0   ? ACT_THROWING
                                : input == 1 ? ACT_START_CROUCHING
                                : input == 2 ? ACT_HOLD_JUMP
                                             : ACT_HOLD_FREEFALL);
        }
    }
    return 1;
}
static int prone_water(void) {
    u32 poses[] = { ACT_CROUCHING,        ACT_START_CROUCHING,   ACT_STOP_CROUCHING, ACT_CRAWLING,
                    ACT_START_CRAWLING,   ACT_STOP_CRAWLING,     ACT_STOMACH_SLIDE,  ACT_DIVE_SLIDE,
                    ACT_SLIDE_KICK_SLIDE, ACT_STOMACH_SLIDE_STOP };
    for (unsigned i = 0; i < sizeof(poses) / sizeof(*poses); i++) {
        begin(poses[i]);
        m.waterLevel = 80;
        for (int frame = 0; frame < 383; frame++) {
            update_mario_health(&m);
        }
        EXPECT_EQ(m.health, 0x880);
        update_mario_health(&m);
        EXPECT_EQ(m.health, 0x780);
        m.flags |= MARIO_METAL_CAP;
        for (int frame = 0; frame < 400; frame++) {
            update_mario_health(&m);
        }
        EXPECT_EQ(m.health, 0x780);
    }
    begin(ACT_IDLE);
    m.waterLevel = 80;
    for (int frame = 0; frame < 400; frame++) {
        update_mario_health(&m);
    }
    EXPECT_EQ(m.health, 0x880);
    begin(ACT_CROUCHING);
    m.waterLevel = 50;
    EXPECT_EQ(mario_is_head_underwater(&m), FALSE);
    m.waterLevel = 51;
    EXPECT_EQ(mario_is_head_underwater(&m), TRUE);
    return 1;
}
static int drowning_recovery(void) {
    begin(ACT_CRAWLING);
    m.waterLevel = 80;
    m.health = 0x180;
    sWaterHealth.previousHealth = m.health;
    for (int i = 0; i < 384; i++) {
        update_mario_health(&m);
    }
    EXPECT_EQ(m.health, 0xff);
    EXPECT_EQ(mario_should_drown_on_land(&m), TRUE);
    m.flags |= MARIO_METAL_CAP;
    EXPECT_EQ(mario_should_drown_on_land(&m), TRUE);
    m.flags |= MARIO_VANISH_CAP;
    EXPECT_EQ(mario_should_drown_on_land(&m), FALSE);
    m.flags &= ~MARIO_VANISH_CAP;
    m.flags &= ~MARIO_METAL_CAP;
    m.action = ACT_IDLE;
    EXPECT_EQ(mario_should_drown_on_land(&m), FALSE);
    begin(ACT_CRAWLING);
    m.waterLevel = 80;
    sArea.terrainType = TERRAIN_SNOW;
    for (int i = 0; i < 128; i++) {
        update_mario_health(&m);
    }
    EXPECT_EQ(m.health, 0x780);
    begin(ACT_CRAWLING);
    m.action |= ACT_FLAG_INTANGIBLE;
    m.waterLevel = 80;
    for (int i = 0; i < 400; i++) {
        update_mario_health(&m);
    }
    EXPECT_EQ(m.health, 0x880);
    return 1;
}
static int shallow_water_sink_death(void) {
    begin(ACT_CROUCHING);
    m.health = 0xff;
    EXPECT_EQ(mario_has_shallow_water_death_pending(&m), FALSE);
    m.waterLevel = 50;
    EXPECT_EQ(mario_should_drown_on_land(&m), FALSE);
    m.waterLevel = 51;
    EXPECT_EQ(mario_should_drown_on_land(&m), TRUE);
    EXPECT_EQ(mario_is_shallow_water_drowning(&m), TRUE);
    m.waterLevel = 100;
    EXPECT_EQ(mario_is_shallow_water_drowning(&m), TRUE);
    m.waterLevel = 101;
    EXPECT_EQ(mario_is_shallow_water_drowning(&m), FALSE);
    m.action = ACT_IDLE;
    m.waterLevel = 80;
    EXPECT_EQ(mario_is_shallow_water_drowning(&m), FALSE);

    for (int cycle = 0; cycle < 2; cycle++) {
        begin(ACT_QUICKSAND_DEATH);
        m.actionArg = ACT_ARG_QUICKSAND_DEATH_SHALLOW_WATER;
        m.flags = MARIO_NORMAL_CAP | MARIO_CAP_ON_HEAD | (cycle ? MARIO_WING_CAP : 0);
        m.capTimer = cycle ? 120 : 0;
        m.pos[0] = 40;
        m.pos[1] = m.floorHeight = 7;
        m.pos[2] = -80;
        for (int frame = 1; frame < 20; frame++) {
            act_quicksand_death(&m);
            EXPECT_EQ(m.actionState, ACT_STATE_QUICKSAND_DEATH_SINK);
            EXPECT_EQ(m.flags & MARIO_NORMAL_CAP, MARIO_NORMAL_CAP);
            EXPECT_EQ(sSavedCapWrites, 0);
            EXPECT_EQ(sWarped, 0);
        }
        act_quicksand_death(&m);
        NEAR(m.quicksandDepth, 100);
        EXPECT_EQ(m.flags & (MARIO_NORMAL_CAP | MARIO_CAP_ON_HEAD | MARIO_SPECIAL_CAPS), 0);
        EXPECT_EQ(m.capTimer, 0);
        EXPECT_EQ(sSavedCapWrites, 1);
        EXPECT_EQ(sSavedCapPos[0], 40);
        EXPECT_EQ(sSavedCapPos[1], 7);
        EXPECT_EQ(sSavedCapPos[2], -80);
        sMarioObject.header.gfx.pos[1] = m.pos[1];
        sink_mario_in_quicksand(&m);
        NEAR(sMarioObject.header.gfx.pos[1], -93);
        for (int frame = 20; frame < 36; frame++) {
            act_quicksand_death(&m);
        }
        EXPECT_EQ(m.actionState, ACT_STATE_QUICKSAND_DEATH_DEAD);
        EXPECT_EQ(sWarped, 1);
        EXPECT_EQ(sSavedCapWrites, 1);
        act_quicksand_death(&m);
        EXPECT_EQ(sWarped, 1);

        // A fresh load uses the saved ground-cap position and leaves Mario capless.
        static struct TestBodyState body;
        m.marioBodyState = &body;
        gCurrentArea = &sArea;
        sMarioObject.header.gfx.areaIndex = gCurrAreaIndex;
        init_mario();
        EXPECT_EQ(m.flags & MARIO_NORMAL_CAP, 0);
        EXPECT_EQ(sCapSpawnCount, 1);
        NEAR(sCapObjects[0].oSpawnPos[0], 40);
        NEAR(sCapObjects[0].oSpawnPos[1], 7);
        NEAR(sCapObjects[0].oSpawnPos[2], -80);
        init_mario();
        EXPECT_EQ(sCapSpawnCount, 1);
    }

    // Ordinary quicksand keeps its original cap and death timing.
    begin(ACT_QUICKSAND_DEATH);
    m.flags = MARIO_NORMAL_CAP | MARIO_CAP_ON_HEAD;
    for (int frame = 0; frame < 36; frame++) {
        act_quicksand_death(&m);
    }
    EXPECT_EQ(sWarped, 1);
    EXPECT_EQ(sSavedCapWrites, 0);
    EXPECT_EQ(m.flags & MARIO_NORMAL_CAP, MARIO_NORMAL_CAP);

    begin(ACT_QUICKSAND_DEATH);
    m.actionArg = ACT_ARG_QUICKSAND_DEATH_SHALLOW_WATER;
    m.flags = 0;
    for (int frame = 0; frame < 20; frame++) {
        act_quicksand_death(&m);
    }
    EXPECT_EQ(sSavedCapWrites, 0);
    return 1;
}
static int pole_handstand_z_cancel(void) {
    const s16 headings[] = { 0, 0x4000, -0x8000 };
    const int frames[] = { 0, 10, 20 };
    struct Animation handstand = { .loopEnd = 21 };
    const s16 animations[] = { MARIO_ANIM_START_HANDSTAND, MARIO_ANIM_HANDSTAND_IDLE,
                               MARIO_ANIM_RETURN_FROM_HANDSTAND };
    for (int phase = 0; phase < 3; phase++) {
        for (unsigned h = 0; h < sizeof(headings) / sizeof(headings[0]); h++) {
            for (unsigned frame = 0; frame < sizeof(frames) / sizeof(frames[0]); frame++) {
                begin(phase == 1 ? ACT_TOP_OF_POLE : ACT_TOP_OF_POLE_TRANSITION);
                m.actionArg = phase == 2;
                m.usedObj = &sObj;
                sObj.hitboxHeight = 600;
                sObj.oPosY = sObj.oSpawnPos[1] = 50;
                sObj.oSpawnPos[0] = 20;
                sObj.oSpawnPos[2] = -40;
                sObj.oMoveAnglePitch = 0x1000;
                sObj.oMoveAngleRoll = -0x1000;
                sMarioObject.oMarioPolePos = 500;
                sMarioObject.header.gfx.animInfo.curAnim = &handstand;
                sMarioObject.header.gfx.animInfo.animID = animations[phase];
                sMarioObject.header.gfx.animInfo.animFrame = frames[frame];
                m.faceAngle[1] = headings[h];
                update_pole_handstand_facing(&m);
                s16 displayedYaw = sMarioObject.header.gfx.angle[1];
                // Move the support after the preceding draw; cancel at its new position.
                sObj.oSpawnPos[0] += 15;
                sObj.oPosY = sObj.oSpawnPos[1] += 10;
                sPoleRootY = frames[frame];
                m.pos[0] = -999;
                m.pos[1] = -999;
                m.faceAngle[0] = 0x1000;
                m.faceAngle[2] = -0x1000;
                m.angleVel[1] = 0x1000;
                m.input = INPUT_Z_PRESSED;
                if (frame != 1) {
                    m.input |= INPUT_A_DOWN | INPUT_A_PRESSED;
                }
                sAnimEnded = TRUE; // Z also wins over transition completion.
                EXPECT_EQ(phase == 1 ? act_top_of_pole(&m) : act_top_of_pole_transition(&m), TRUE);
                EXPECT_EQ(m.action, ACT_SOFT_BONK);
                NEAR(m.forwardVel, -2);
                EXPECT_EQ(m.faceAngle[1], displayedYaw);
                EXPECT_EQ(sMarioObject.header.gfx.angle[1], displayedYaw);
                EXPECT_EQ(m.faceAngle[0], 0);
                EXPECT_EQ(m.faceAngle[2], 0);
                EXPECT_EQ(sMarioObject.header.gfx.angle[0], 0);
                EXPECT_EQ(sMarioObject.header.gfx.angle[2], 0);
                EXPECT_EQ(m.angleVel[1], 0);
                NEAR(m.pos[0], 35);
                NEAR(m.pos[1], 560 + sPoleRootY);
                NEAR(m.pos[2], -40);
                for (int axis = 0; axis < 3; axis++) {
                    NEAR(sMarioObject.header.gfx.pos[axis], m.pos[axis]);
                }
                EXPECT_EQ(sPolePositionChecks, 2);
                EXPECT_EQ(sPoleLeaves, 1);
                EXPECT_EQ(m.usedObj, &sObj);
                EXPECT_EQ(m.prevAction, phase == 1 ? ACT_TOP_OF_POLE : ACT_TOP_OF_POLE_TRANSITION);
            }
        }
    }
    return 1;
}

static int pole_handstand_preserved_controls(void) {
    struct Animation handstand = { .loopEnd = 21 };
    const s16 animations[] = { MARIO_ANIM_START_HANDSTAND, MARIO_ANIM_HANDSTAND_IDLE,
                               MARIO_ANIM_RETURN_FROM_HANDSTAND };
    for (int phase = 0; phase < 3; phase++) {
        begin(phase == 1 ? ACT_TOP_OF_POLE : ACT_TOP_OF_POLE_TRANSITION);
        m.usedObj = &sObj;
        sObj.hitboxHeight = 600;
        sMarioObject.oMarioPolePos = 500;
        sMarioObject.header.gfx.animInfo.curAnim = &handstand;
        sMarioObject.header.gfx.animInfo.animID = animations[phase];
        sMarioObject.header.gfx.animInfo.animFrame = 10;
        m.actionArg = phase == 2;
        m.input = INPUT_Z_DOWN; // Only a fresh press cancels, like ordinary pole hold.
        EXPECT_EQ(phase == 1 ? act_top_of_pole(&m) : act_top_of_pole_transition(&m), FALSE);
        EXPECT_EQ(m.action, phase == 1 ? ACT_TOP_OF_POLE : ACT_TOP_OF_POLE_TRANSITION);
        EXPECT_EQ(sPoleLeaves, 0);
    }
    begin(ACT_TOP_OF_POLE);
    m.usedObj = &sObj;
    sObj.hitboxHeight = 600;
    sMarioObject.oMarioPolePos = 500;
    m.input = INPUT_A_DOWN;
    m.faceAngle[1] = 0x4000;
    EXPECT_EQ(act_top_of_pole(&m), TRUE);
    EXPECT_EQ(m.action, ACT_TOP_OF_POLE_JUMP);
    EXPECT_EQ(m.faceAngle[1], (s16) 0xC000);
    NEAR(m.pos[1], 500 - 137 / 4.0f);
    NEAR(sMarioObject.header.gfx.pos[1], m.pos[1]);
    EXPECT_EQ(sPolePositionChecks, 2);
    begin(ACT_TOP_OF_POLE);
    sController.stickY = -17;
    EXPECT_EQ(act_top_of_pole(&m), TRUE);
    EXPECT_EQ(m.action, ACT_TOP_OF_POLE_TRANSITION);
    EXPECT_EQ(m.actionArg, 1);
    begin(ACT_TOP_OF_POLE_TRANSITION);
    sAnimEnded = TRUE;
    EXPECT_EQ(act_top_of_pole_transition(&m), TRUE);
    EXPECT_EQ(m.action, ACT_TOP_OF_POLE);
    // A can leave startup one update early; neutral input still completes normally.
    for (int frame = 18; frame <= 20; frame++) {
        for (int jump = 0; jump < 2; jump++) {
            begin(ACT_TOP_OF_POLE_TRANSITION);
            m.usedObj = &sObj;
            sObj.hitboxHeight = 600;
            sMarioObject.oMarioPolePos = 500;
            sMarioObject.header.gfx.animInfo.curAnim = &handstand;
            sMarioObject.header.gfx.animInfo.animID = MARIO_ANIM_START_HANDSTAND;
            sMarioObject.header.gfx.animInfo.animFrame = frame;
            sRealAnimationEnds = TRUE;
            m.input = jump ? INPUT_A_DOWN : 0;
            int ready = frame == 20 || (jump && frame == 19);
            EXPECT_EQ(act_top_of_pole_transition(&m), ready);
            EXPECT_EQ(m.action, ready ? ACT_TOP_OF_POLE : ACT_TOP_OF_POLE_TRANSITION);
            if (ready && jump) {
                EXPECT_EQ(act_top_of_pole(&m), TRUE);
                EXPECT_EQ(m.action, ACT_TOP_OF_POLE_JUMP);
            }
        }
    }
    for (int frame = 0; frame <= 2; frame++) {
        begin(ACT_TOP_OF_POLE_TRANSITION);
        m.usedObj = &sObj;
        sObj.hitboxHeight = 600;
        sMarioObject.oMarioPolePos = 500;
        m.actionArg = 1;
        sMarioObject.header.gfx.animInfo.animID = MARIO_ANIM_RETURN_FROM_HANDSTAND;
        sMarioObject.header.gfx.animInfo.animFrame = frame;
        EXPECT_EQ(act_top_of_pole_transition(&m), frame <= 1);
        EXPECT_EQ(m.action, frame <= 1 ? ACT_HOLDING_POLE : ACT_TOP_OF_POLE_TRANSITION);
    }
    // Forced floor/wall detach retains the placement routine's own response.
    for (int wall = 0; wall < 2; wall++) {
        for (int jumping = 0; jumping < 2; jumping++) {
            begin(ACT_TOP_OF_POLE);
            m.usedObj = &sObj;
            sObj.hitboxHeight = 600;
            sMarioObject.oMarioPolePos = wall ? 500 : -1;
            m.input = jumping ? INPUT_A_DOWN : INPUT_Z_PRESSED;
            sPoleWalls = wall;
            EXPECT_EQ(act_top_of_pole(&m), TRUE);
            EXPECT_EQ(m.action, wall ? ACT_SOFT_BONK : ACT_IDLE);
            EXPECT_EQ(sPoleLeaves, 0);
        }
    }
    begin(ACT_TOP_OF_POLE);
    m.usedObj = &sObj;
    sObj.hitboxHeight = 600;
    sMarioObject.oMarioPolePos = 500;
    m.health = 0xFF;
    EXPECT_EQ(act_top_of_pole(&m), TRUE);
    EXPECT_EQ(m.action, ACT_SOFT_BONK);
    return 1;
}

static int jump_kick_pole_grab(void) {
    const f32 speeds[] = { -40, 0, 24, 25, 31.99f, 32, 32.01f, 60 };
    for (int recovery = 0; recovery < 2; recovery++) {
        for (int forward = 0; forward < 2; forward++) {
            for (unsigned speed = 0; speed < sizeof(speeds) / sizeof(*speeds); speed++) {
                begin(ACT_JUMP_KICK);
                m.actionState = recovery;
                m.actionArg = ACT_ARG_JUMP_KICK_FOLLOWUP_QUEUED;
                m.flags |= MARIO_KICKING;
                m.forwardVel = speeds[speed];
                m.vel[1] = -12;
                m.faceAngle[0] = m.marioObj->header.gfx.angle[0] = 0x1000;
                m.pos[1] = 120;
                sObj.oPosY = 20;
                sDives = forward;
                EXPECT_EQ(interact_pole(&m, INTERACT_POLE, &sObj), TRUE);
                EXPECT_EQ(m.action,
                          (speeds[speed] > 32 && forward) ? ACT_GRAB_POLE_FAST : ACT_GRAB_POLE_SLOW);
                EXPECT_EQ(m.usedObj, &sObj);
                EXPECT_EQ(m.interactObj, &sObj);
                EXPECT_EQ(m.actionArg, 0);
                EXPECT_EQ(m.flags & MARIO_KICKING, 0);
                EXPECT_EQ(m.forwardVel, 0);
                EXPECT_EQ(m.vel[1], 0);
                EXPECT_EQ(m.marioObj->oMarioPolePos, 100);
                EXPECT_EQ(m.faceAngle[0], 0);
                EXPECT_EQ(m.marioObj->header.gfx.angle[0], 0);
            }
        }
    }
    // Forward dives bypass both speed and stick requirements; other approaches do not.
    const f32 diveSpeeds[] = { -40, 0, 0.5f, 24, 32, 60 };
    for (int forward = 0; forward < 2; forward++) {
        for (unsigned speed = 0; speed < sizeof(diveSpeeds) / sizeof(*diveSpeeds); speed++) {
            begin(ACT_DIVE);
            m.forwardVel = diveSpeeds[speed];
            sDives = forward;
            EXPECT_EQ(interact_pole(&m, INTERACT_POLE, &sObj), TRUE);
            EXPECT_EQ(m.action, diveSpeeds[speed] > 0 ? ACT_GRAB_POLE_FAST : ACT_GRAB_POLE_SLOW);
        }
    }
    begin(ACT_JUMP_KICK);
    m.prevAction = ACT_HOLDING_POLE;
    m.usedObj = &sObj;
    EXPECT_EQ(interact_pole(&m, INTERACT_POLE, &sObj), FALSE);
    EXPECT_EQ(m.action, ACT_JUMP_KICK);
    struct Object otherPole = { 0 };
    EXPECT_EQ(interact_pole(&m, INTERACT_POLE, &otherPole), TRUE);
    EXPECT_EQ(m.usedObj, &otherPole);
    begin(ACT_JUMP_KICK);
    m.pos[1] = -10;
    sObj.hitboxDownOffset = 20;
    EXPECT_EQ(interact_pole(&m, INTERACT_POLE, &sObj), TRUE);
    EXPECT_EQ(m.marioObj->oMarioPolePos, -20);
    begin(ACT_SLIDE_KICK);
    EXPECT_EQ(interact_pole(&m, INTERACT_POLE, &sObj), FALSE);
    EXPECT_EQ(m.action, ACT_SLIDE_KICK);
    return 1;
}

static int soaked_wind_drying(void) {
    for (int exposure = 0; exposure < 6; exposure++) {
        for (int metal = 0; metal < 2; metal++) {
            for (int cycle = 0; cycle < 3; cycle++) {
                begin(ACT_IDLE);
                m.flags = metal ? MARIO_METAL_CAP : 0;
                m.waterLevel = FLOOR_LOWER_LIMIT;
                sWaterLevel = FLOOR_LOWER_LIMIT;
                m.forwardVel = 42;
                m.vel[1] = -12;
                if (exposure < 2) {
                    m.collidedObjInteractTypes = exposure ? INTERACT_TORNADO : INTERACT_STRONG_WIND;
                } else if (exposure < 4) {
                    m.action = exposure == 2 ? ACT_GETTING_BLOWN : ACT_TORNADO_TWIRLING;
                } else {
                    sFloor.type = exposure == 4 ? SURFACE_HORIZONTAL_WIND : SURFACE_VERTICAL_WIND;
                    m.floorHeight = exposure == 4 ? 0 : -3000;
                    m.pos[1] = exposure == 4 ? 0 : -900;
                }
                u32 action = m.action;
                mario_apply_soaked_status(&m);
                m.soakedDripFrames = 9;
                mario_update_soaked_status(&m);
                EXPECT_EQ(mario_is_soaked(&m), FALSE);
                EXPECT_EQ(m.soakedTimer, 0);
                EXPECT_EQ(m.soakedDripFrames, 0);
                EXPECT_EQ(m.action, action);
                EXPECT_EQ(m.forwardVel, 42);
                EXPECT_EQ(m.vel[1], -12);
                // A fresh wet application during movement must also dry before drips/hazards.
                mario_apply_soaked_status(&m);
                mario_refresh_soaked_after_movement(&m);
                EXPECT_EQ(mario_is_soaked(&m), FALSE);
                EXPECT_EQ(m.soakedTimer, 0);
                // Leaving wind alone does not recreate the status.
                m.action = ACT_IDLE;
                m.collidedObjInteractTypes = 0;
                sFloor.type = SURFACE_DEFAULT;
                mario_update_soaked_status(&m);
                EXPECT_EQ(mario_is_soaked(&m), FALSE);
                mario_apply_soaked_status(&m);
                mario_update_soaked_status(&m);
                EXPECT_EQ(mario_is_soaked(&m), TRUE);
            }
        }
    }
    for (int boundary = 0; boundary < 3; boundary++) {
        begin(ACT_FREEFALL);
        m.waterLevel = FLOOR_LOWER_LIMIT;
        sFloor.type = boundary == 0 ? SURFACE_HORIZONTAL_WIND : SURFACE_VERTICAL_WIND;
        m.floorHeight = boundary == 0 ? 0 : -3000;
        if (boundary == 0) {
            m.pos[1] = 1;
        } else if (boundary == 1) {
            m.pos[1] = -953;
        } else {
            m.pos[1] = 500;
        }
        mario_apply_soaked_status(&m);
        mario_update_soaked_status(&m);
        EXPECT_EQ(mario_is_soaked(&m), TRUE);
    }
    // Continuous water does not undo drying; a new water entry restores Soaked.
    begin(ACT_IDLE);
    m.waterLevel = sWaterLevel = 0;
    sFloor.type = SURFACE_HORIZONTAL_WIND;
    mario_update_soaked_status(&m);
    mario_refresh_soaked_after_movement(&m);
    EXPECT_EQ(mario_is_soaked(&m), FALSE);
    EXPECT_EQ(m.soakedWaterContact, TRUE);
    sFloor.type = SURFACE_DEFAULT;
    for (int update = 0; update < 3; update++) {
        mario_update_soaked_status(&m);
        mario_refresh_soaked_after_movement(&m);
        EXPECT_EQ(mario_is_soaked(&m), FALSE);
    }
    m.waterLevel = sWaterLevel = FLOOR_LOWER_LIMIT;
    mario_update_soaked_status(&m);
    m.waterLevel = sWaterLevel = 0;
    mario_update_soaked_status(&m);
    EXPECT_EQ(mario_is_soaked(&m), TRUE);
    return TRUE;
}

static int permanent_soaked(void) {
    sTestGameOptions[GO_CHEATS_ENABLED] = TRUE;
    sTestGameOptions[GO_PERMANENT_SOAKED] = TRUE;
    for (int normalWet = 0; normalWet < 2; normalWet++) {
        sTestGameOptions[GO_SOAKED] = normalWet;
        for (int vanish = 0; vanish < 2; vanish++) {
            begin(ACT_IDLE);
            if (vanish) {
                m.flags |= MARIO_VANISH_CAP;
            }
            m.waterLevel = sWaterLevel = FLOOR_LOWER_LIMIT;
            for (int frame = 0; frame < 180; frame++) {
                mario_update_soaked_status(&m);
                EXPECT_EQ(mario_is_soaked(&m), TRUE);
                mario_consume_soaked_status(&m);
                EXPECT_EQ(mario_is_soaked(&m), TRUE);
                mario_refresh_soaked_after_movement(&m);
                mario_emit_soaked_drips(&m);
                EXPECT_EQ(m.soakedDripFrames, (frame + 1) % 10);
            }
            sFloor.type = SURFACE_HORIZONTAL_WIND;
            m.pos[1] = m.floorHeight;
            mario_dry_from_wind(&m);
            EXPECT_EQ(mario_is_soaked(&m), TRUE);
            m.action = ACT_BURNING_GROUND;
            m.marioObj->oMarioBurnTimer = 20;
            mario_update_soaked_status(&m);
            EXPECT_EQ(m.action, ACT_WALKING);
            EXPECT_EQ(mario_is_soaked(&m), TRUE);
            sTestGameOptions[GO_CHEATS_ENABLED] = FALSE;
            mario_update_soaked_status(&m);
            EXPECT_EQ(mario_is_soaked(&m), FALSE);
            EXPECT_EQ(m.soakedDripFrames, 0);
            sTestGameOptions[GO_CHEATS_ENABLED] = TRUE;
        }
    }
    sTestGameOptions[GO_SOAKED] = TRUE;
    sTestGameOptions[GO_PERMANENT_SOAKED] = FALSE;
    sTestGameOptions[GO_CHEATS_ENABLED] = FALSE;
    return TRUE;
}

static int soaked_status(void) {
    begin(ACT_IDLE);
    m.waterLevel = FLOOR_LOWER_LIMIT;
    mario_apply_soaked_status(&m);
    EXPECT_EQ(mario_is_soaked(&m), TRUE);
    EXPECT_EQ(m.soakedTimer, 150);
    m.flags = MARIO_VANISH_CAP;
    mario_apply_soaked_status(&m);
    EXPECT_EQ(m.soakedTimer, 150);
    mario_consume_soaked_status(&m);
    mario_apply_soaked_status(&m);
    EXPECT_EQ(m.soakedTimer, 0);

    begin(ACT_IDLE);
    m.waterLevel = 0;
    m.pos[1] = 0;
    mario_update_soaked_status(&m);
    EXPECT_EQ(mario_is_soaked(&m), TRUE);
    EXPECT_EQ(m.soakedTimer, 150);
    for (int frame = 0; frame < 8; frame++) {
        mario_update_soaked_status(&m);
        EXPECT_EQ(m.soakedTimer, 150);
    }
    m.waterLevel = FLOOR_LOWER_LIMIT;
    for (int dry = 1; dry <= 150; dry++) {
        mario_update_soaked_status(&m);
        EXPECT_EQ(mario_is_soaked(&m), TRUE);
        EXPECT_EQ(m.soakedTimer, 150 - dry);
    }
    mario_update_soaked_status(&m);
    EXPECT_EQ(mario_is_soaked(&m), FALSE);
    m.waterLevel = 0;
    mario_update_soaked_status(&m);
    EXPECT_EQ(mario_is_soaked(&m), TRUE);
    EXPECT_EQ(m.soakedTimer, 150);
    mario_consume_soaked_status(&m);
    EXPECT_EQ(mario_is_soaked(&m), FALSE);
    EXPECT_EQ(m.soakedTimer, 0);
    for (int frame = 0; frame < 5; frame++) {
        mario_update_soaked_status(&m);
        EXPECT_EQ(mario_is_soaked(&m), FALSE);
    }
    m.waterLevel = FLOOR_LOWER_LIMIT;
    mario_update_soaked_status(&m);
    m.waterLevel = 0;
    mario_update_soaked_status(&m);
    EXPECT_EQ(mario_is_soaked(&m), TRUE);
    m.flags |= MARIO_VANISH_CAP;
    mario_update_soaked_status(&m);
    EXPECT_EQ(mario_is_soaked(&m), FALSE);
    EXPECT_EQ(m.soakedTimer, 0);
    m.flags &= ~MARIO_VANISH_CAP;
    mario_update_soaked_status(&m);
    EXPECT_EQ(mario_is_soaked(&m), TRUE);

    begin(ACT_BURNING_GROUND);
    m.waterLevel = 0;
    m.pos[1] = 0;
    m.marioObj->oMarioBurnTimer = 20;
    mario_update_soaked_status(&m);
    EXPECT_EQ(m.action, ACT_WALKING);
    EXPECT_EQ(mario_is_soaked(&m), FALSE);
    EXPECT_EQ(m.marioObj->oMarioBurnTimer, 0);
    EXPECT_EQ(m.hurtCounter, 0);
    begin(ACT_BURNING_JUMP);
    m.waterLevel = FLOOR_LOWER_LIMIT;
    m.soakedTimer = 150;
    m.marioObj->oMarioBurnTimer = 20;
    mario_update_soaked_status(&m);
    EXPECT_EQ(m.action, ACT_FREEFALL);
    EXPECT_EQ(mario_is_soaked(&m), FALSE);
    EXPECT_EQ(m.marioObj->oMarioBurnTimer, 0);
    EXPECT_EQ(m.hurtCounter, 0);

    begin(ACT_IDLE);
    m.waterLevel = FLOOR_LOWER_LIMIT;
    m.pos[1] = 0;
    sWaterLevel = 0;
    mario_refresh_soaked_after_movement(&m);
    EXPECT_EQ(mario_is_soaked(&m), TRUE);
    EXPECT_EQ(m.soakedWaterContact, TRUE);
    mario_emit_soaked_drips(&m);
    EXPECT_EQ(sSoakedDripSpawns, 0);
    sWaterLevel = FLOOR_LOWER_LIMIT;
    mario_refresh_soaked_after_movement(&m);
    for (int frame = 0; frame < 4; frame++) {
        mario_emit_soaked_drips(&m);
    }
    EXPECT_EQ(sSoakedDripSpawns, 0);
    mario_emit_soaked_drips(&m);
    EXPECT_EQ(sSoakedDripSpawns, 1);
    NEAR(sSoakedDripScale, 1.2f);
    NEAR(sSoakedDrip.oPosX, 60);
    NEAR(sSoakedDrip.oPosZ, 0);
    for (int frame = 0; frame < 4; frame++) {
        mario_emit_soaked_drips(&m);
    }
    EXPECT_EQ(sSoakedDripSpawns, 1);
    mario_emit_soaked_drips(&m);
    EXPECT_EQ(sSoakedDripSpawns, 2);
    NEAR(sSoakedDripScale, 2.4f);
    NEAR(sSoakedDrip.oPosX, 60);
    NEAR(sSoakedDrip.oPosZ, 0);
    m.faceAngle[1] = 0;
    sCamera.yaw = 0x4000;
    sTestRandomU16 = 1;
    for (int frame = 0; frame < 4; frame++) {
        mario_emit_soaked_drips(&m);
    }
    EXPECT_EQ(sSoakedDripSpawns, 2);
    mario_emit_soaked_drips(&m);
    EXPECT_EQ(sSoakedDripSpawns, 3);
    NEAR(sSoakedDripScale, 1.2f);
    for (int frame = 0; frame < 4; frame++) {
        mario_emit_soaked_drips(&m);
    }
    EXPECT_EQ(sSoakedDripSpawns, 3);
    mario_emit_soaked_drips(&m);
    EXPECT_EQ(sSoakedDripSpawns, 4);
    NEAR(sSoakedDripScale, 2.4f);
    EXPECT_EQ(sSoakedDrip.oAction, 1);
    NEAR(sSoakedDrip.oPosY, 80);
    NEAR(sSoakedDrip.oPosX, 0);
    NEAR(sSoakedDrip.oPosZ, 60);
    mario_consume_soaked_status(&m);
    mario_emit_soaked_drips(&m);
    EXPECT_EQ(sSoakedDripSpawns, 4);
    o = &sSoakedDrip;
    sMarioObject.oPosX = 100;
    sMarioObject.oPosY = 20;
    sMarioObject.oPosZ = -50;
    sSoakedDrip.oTimer = 0;
    bhv_water_droplet_loop();
    NEAR(sSoakedDrip.oPosX, 80);
    NEAR(sSoakedDrip.oPosY, 76);
    NEAR(sSoakedDrip.oPosZ, 20);
    sMarioObject.oPosX = 180;
    sMarioObject.oPosY = 120;
    sMarioObject.oPosZ = 25;
    sSoakedDrip.oTimer = 1;
    bhv_water_droplet_loop();
    NEAR(sSoakedDrip.oPosX, 144);
    NEAR(sSoakedDrip.oPosY, 68);
    NEAR(sSoakedDrip.oPosZ, 80);
    sSoakedDrip.oTimer = 2;
    bhv_water_droplet_loop();
    NEAR(sSoakedDrip.oPosX, 144);
    NEAR(sSoakedDrip.oPosY, 56);
    NEAR(sSoakedDrip.oPosZ, 80);
    sMarioObject.oPosX = 70;
    sMarioObject.oPosY = 0;
    sMarioObject.oPosZ = -20;
    sSoakedDrip.oPosY = 80;
    sSoakedDrip.oVelY = 0;
    sEffectParticleCount = 0;
    sDeleted = 0;
    for (int frame = 0; frame < 6; frame++) {
        sSoakedDrip.oTimer = frame;
        bhv_water_droplet_loop();
    }
    EXPECT_EQ(sDeleted, 1);
    EXPECT_EQ(sEffectParticleCount, 1);
    NEAR(sEffectParticles[0].oPosX, 56);
    NEAR(sEffectParticles[0].oPosZ, 44);
    NEAR(sEffectParticles[0].oPosY, 0);
    EXPECT_EQ(sEffectParticles[0].oAction, 1);
    o = &sEffectParticles[0];
    bhv_water_droplet_splash_init();
    NEAR(o->header.gfx.scale[0], 0.9f);
    EXPECT_EQ(o->oOpacity, 216);
    for (int frame = 0; frame < 8; frame++) {
        o->oTimer = frame;
        bhv_water_droplet_splash_fade();
        EXPECT_EQ(o->oOpacity, MAX(216 - (frame + 1) * 30, 0));
    }
    o->oAction = 0;
    bhv_water_droplet_splash_init();
    NEAR(o->header.gfx.scale[0], 1.5f);
    EXPECT_EQ(o->oOpacity, 255);
    sDeleted = 0;
    o->oTimer = 0;
    bhv_water_droplet_splash_fade();
    EXPECT_EQ(o->oOpacity, 255);
    EXPECT_EQ(sDeleted, 0);
    o->oTimer = 5;
    bhv_water_droplet_splash_fade();
    EXPECT_EQ(sDeleted, 1);
    return 1;
}

static int fountain_unlock_drowning(void) {
    static struct Object bigBoo;
    for (int trial = 0; trial < 19; trial++) {
        begin(ACT_DAB);
        gCurrLevelNum = LEVEL_CASTLE_COURTYARD;
        bigBoo = (struct Object){ .behavior = bhvBooWithCage,
                                  .activeFlags = ACTIVE_FLAG_ACTIVE,
                                  .oOpacity = 255,
                                  .oBooTargetOpacity = 255 };
        bigBoo.header.next = &gObjectLists[OBJ_LIST_GENACTOR];
        gObjectLists[OBJ_LIST_GENACTOR].header.next = &bigBoo;
        m.flags = 0;
        m.waterLevel = 51;
        m.pos[0] = 0;
        m.pos[2] = -1200;
        m.health = 0x180;
        if (trial == 1) {
            gCurrLevelNum = LEVEL_PHYSICS_LAB;
        }
        if (trial == 2) {
            gCurrAreaIndex = 2;
        }
        if (trial == 3) {
            m.pos[0] = 674;
        }
        if (trial == 4) {
            m.pos[2] = -1074;
        }
        if (trial == 5) {
            m.action = ACT_CROUCHING;
        }
        if (trial == 6) {
            m.waterLevel = 120;
        }
        if (trial == 7) {
            m.flags |= MARIO_VANISH_CAP;
        }
        if (trial == 8) {
            m.flags |= MARIO_METAL_CAP;
        }
        if (trial == 9) {
            m.health = 0xff;
            m.hurtCounter = 1;
        }
        if (trial == 10) {
            sObj.behavior = bhvGhostHuntBoo;
            sObj.activeFlags = ACTIVE_FLAG_ACTIVE;
            sObj.oBooDeathStatus = BOO_DEATH_STATUS_ALIVE;
            sObj.oAction = 1;
            gObjectLists[OBJ_LIST_GENACTOR].header.next = &sObj;
            sObj.header.next = &bigBoo;
        }
        if (trial == 11) {
            gObjectLists[OBJ_LIST_GENACTOR].header.next = &gObjectLists[OBJ_LIST_GENACTOR];
        }
        if (trial == 12) {
            bigBoo.oAction = 3;
        }
        if (trial == 13) {
            bigBoo.oBooDeathStatus = BOO_DEATH_STATUS_DEAD;
        }
        if (trial == 14) {
            bigBoo.activeFlags = ACTIVE_FLAG_DEACTIVATED;
        }
        if (trial == 15) {
            m.flags = MARIO_NORMAL_CAP | MARIO_CAP_ON_HEAD;
        }
        if (trial == 16) {
            m.flags = MARIO_NORMAL_CAP | MARIO_CAP_IN_HAND;
        }
        if (trial == 17) {
            bigBoo.oOpacity = 40;
        }
        if (trial == 18) {
            bigBoo.oBooTargetOpacity = 40;
        }
        sWaterHealth.previousHealth = m.health;
        for (int frame = 0; frame < 400 && m.action != ACT_QUICKSAND_DEATH; frame++) {
            update_mario_health(&m);
        }
        EXPECT_EQ(m.action == ACT_QUICKSAND_DEATH, trial == 0 || trial == 8);
        if (trial == 0) {
            EXPECT_EQ(m.actionArg, ACT_ARG_QUICKSAND_DEATH_HE_THICC_UNLOCK);
            for (int frame = 0; frame < 36; frame++) {
                act_quicksand_death(&m);
            }
            EXPECT_EQ(sWarped, 1);
            EXPECT_EQ(sSavedCapWrites, 0);
        }
        if (trial == 15) {
            // Cap loss after the lethal damage cannot retroactively qualify that damage.
            EXPECT_EQ(m.health, 0xff);
            m.flags = 0;
            update_mario_health(&m);
            EXPECT_EQ(m.action, ACT_DAB);
            m.flags = MARIO_NORMAL_CAP | MARIO_CAP_ON_HEAD;
            m.action = ACT_QUICKSAND_DEATH;
            m.actionArg = ACT_ARG_QUICKSAND_DEATH_SHALLOW_WATER;
            for (int frame = 0; frame < 36; frame++) {
                act_quicksand_death(&m);
            }
            EXPECT_EQ(sSavedCapWrites, 1);
            EXPECT_EQ(m.actionArg, ACT_ARG_QUICKSAND_DEATH_SHALLOW_WATER);
        }
    }
    return 1;
}

static int shallow_water_damage_cough(void) {
    begin(ACT_DAB);
    m.waterLevel = 80;
    EXPECT_EQ(mario_is_low_land_pose(&m), TRUE);
    for (int frame = 0; frame < 400 && m.action == ACT_DAB; frame++) {
        update_mario_health(&m);
    }
    EXPECT_EQ(m.action, ACT_COUGHING);
    EXPECT_EQ(m.actionArg, ACT_ARG_COUGHING_SHALLOW_WATER);
    EXPECT_EQ(m.health, 0x780);
    sIdleCancelAction = ACT_JUMP;
    sIdleCancelMask = INPUT_A_PRESSED;
    m.input = INPUT_A_PRESSED | INPUT_NONZERO_ANALOG | INPUT_Z_DOWN | INPUT_B_PRESSED;
    for (int frame = 0; frame < 3; frame++) {
        EXPECT_EQ(act_coughing(&m), FALSE);
        EXPECT_EQ(m.action, ACT_COUGHING);
    }
    sAnimEnded = TRUE;
    EXPECT_EQ(act_coughing(&m), TRUE);
    EXPECT_EQ(m.action, ACT_IDLE);
    begin(ACT_CROUCHING);
    m.waterLevel = 80;
    m.health = 0x180;
    sWaterHealth.previousHealth = m.health;
    for (int frame = 0; frame < 400 && m.health >= 0x100; frame++) {
        update_mario_health(&m);
    }
    EXPECT_EQ(m.health, 0xff);
    EXPECT_EQ(m.action, ACT_CROUCHING);
    begin(ACT_COUGHING);
    m.actionArg = ACT_ARG_COUGHING_SHALLOW_WATER;
    m.input = INPUT_OFF_FLOOR;
    sIdleCancelAction = ACT_FREEFALL;
    sIdleCancelMask = INPUT_OFF_FLOOR;
    EXPECT_EQ(act_coughing(&m), TRUE);
    EXPECT_EQ(m.action, ACT_FREEFALL);
    return 1;
}

static int plunge_breaststroke_input(void) {
    for (int cycle = 0; cycle < 3; cycle++) {
        begin(ACT_WATER_PLUNGE);
        m.pos[1] = -200;
        m.vel[1] = -15;
        m.actionState = ACT_STATE_WATER_PLUNGE_FALL;
        m.input = INPUT_A_PRESSED | INPUT_A_DOWN;
        act_water_plunge(&m);
        EXPECT_EQ(m.action, ACT_BREASTSTROKE);
        EXPECT_EQ(sPlungeSounds, 1);
        EXPECT_EQ(sSwimOverPlungeSounds, 1);
        EXPECT_EQ(sSwimSounds, 0);
        play_sound_if_no_flag(&m, SOUND_ACTION_SWIM, MARIO_ACTION_SOUND_PLAYED);
        EXPECT_EQ(sSwimSounds, 0); // The following stroke update cannot replay it.
        EXPECT_EQ(sWaterSteps, 1);
        EXPECT_EQ(sMarioObject.header.gfx.animInfo.animID, MARIO_ANIM_SWIM_PART1);
        begin(ACT_WATER_PLUNGE);
        m.pos[1] = -200;
        m.vel[1] = -15;
        m.input = INPUT_A_PRESSED | INPUT_A_DOWN | INPUT_B_DOWN;
        sSwimming.swimButton = INPUT_B_DOWN;
        act_water_plunge(&m);
        EXPECT_EQ(m.action, ACT_WATER_PLUNGE);
        EXPECT_EQ(sSwimOverPlungeSounds, 0);
        begin(ACT_WATER_PLUNGE);
        m.pos[1] = -200;
        m.vel[1] = -15;
        m.input = INPUT_A_PRESSED | INPUT_A_DOWN;
        m.flags |= MARIO_METAL_CAP;
        act_water_plunge(&m);
        EXPECT_EQ(m.action, ACT_WATER_PLUNGE);
        EXPECT_EQ(sSwimOverPlungeSounds, 0);
        begin(ACT_WATER_PLUNGE);
        m.pos[1] = -200;
        m.vel[1] = -15;
        m.input = INPUT_A_PRESSED | INPUT_A_DOWN;
        m.heldObj = &sObj;
        act_water_plunge(&m);
        EXPECT_EQ(m.action, ACT_WATER_PLUNGE);
        EXPECT_EQ(sSwimOverPlungeSounds, 0);
    }
    begin(ACT_BREASTSTROKE);
    play_sound_if_no_flag(&m, SOUND_ACTION_SWIM, MARIO_ACTION_SOUND_PLAYED);
    EXPECT_EQ(sSwimSounds, 1);
    EXPECT_EQ(sSwimOverPlungeSounds, 0);
    return 1;
}
static int air_entry_stroke_buffer(void) {
    for (int cycle = 0; cycle < 3; cycle++) {
        for (int age = 1; age <= 8; age++) {
            begin(ACT_DIVE);
            m.pos[1] = -200;
            m.vel[1] = -15;
            m.input = INPUT_A_PRESSED | INPUT_A_DOWN;
            mario_swimming_update(&m);
            for (int frame = 1; frame <= age; frame++) {
                m.input = 0;
                mario_swimming_update(&m);
            }
            m.prevAction = ACT_DIVE;
            m.action = ACT_WATER_PLUNGE;
            act_water_plunge(&m);
            EXPECT_EQ(m.action,
                      age <= AIR_ENTRY_SWIM_STROKE_TAP_FRAMES ? ACT_BREASTSTROKE : ACT_WATER_PLUNGE);
            EXPECT_EQ(sPlungeSounds, 1);
            EXPECT_EQ(sSwimOverPlungeSounds, age <= AIR_ENTRY_SWIM_STROKE_TAP_FRAMES ? 1 : 0);
            EXPECT_EQ(sWaterSteps, 1);
            EXPECT_EQ(sSwimming.entryStrokeReady, FALSE);
        }
    }
    return 1;
}
static int air_entry_stroke_handoff(void) {
    begin(ACT_DIVE);
    m.pos[1] = -200;
    m.vel[1] = -15;
    m.input = INPUT_A_PRESSED | INPUT_A_DOWN;
    mario_swimming_update(&m);
    m.input = INPUT_A_DOWN;
    mario_swimming_update(&m);
    m.prevAction = ACT_DIVE;
    m.action = ACT_WATER_PLUNGE;
    act_water_plunge(&m);
    EXPECT_EQ(m.action, ACT_WATER_PLUNGE);
    EXPECT_EQ(sPlungeSounds, 1);
    EXPECT_EQ(sSwimOverPlungeSounds, 0);
    m.input = 0;
    mario_swimming_update(&m);
    act_water_plunge(&m);
    EXPECT_EQ(m.action, ACT_BREASTSTROKE);
    EXPECT_EQ(sPlungeSounds, 1);
    EXPECT_EQ(sSwimOverPlungeSounds, 1);

    begin(ACT_DIVE);
    m.pos[1] = -200;
    m.vel[1] = -15;
    m.input = INPUT_A_PRESSED | INPUT_A_DOWN;
    mario_swimming_update(&m);
    m.input = INPUT_A_DOWN;
    mario_swimming_update(&m);
    m.prevAction = ACT_DIVE;
    m.action = ACT_WATER_PLUNGE;
    act_water_plunge(&m);
    m.action = ACT_WATER_IDLE;
    m.input = 0;
    mario_swimming_update(&m);
    EXPECT_EQ(sSwimming.strokeQueued, TRUE);
    EXPECT_EQ(check_initial_swim_stroke(&m), TRUE);
    EXPECT_EQ(m.action, ACT_BREASTSTROKE);
    EXPECT_EQ(sSwimming.entryStrokeReady, FALSE);
    return 1;
}
static int air_entry_stroke_priority_and_interruption(void) {
    for (int mode = 0; mode < 4; mode++) {
        begin(ACT_DIVE);
        m.pos[1] = -200;
        m.vel[1] = -15;
        m.input = INPUT_A_PRESSED | INPUT_A_DOWN;
        mario_swimming_update(&m);
        m.input = 0;
        mario_swimming_update(&m);
        if (mode == 0) {
            m.input = INPUT_B_DOWN;
        }
        if (mode == 1) {
            m.flags |= MARIO_METAL_CAP;
        }
        if (mode == 2) {
            m.heldObj = &sObj;
        }
        if (mode == 3) {
            m.action = ACT_SQUISHED;
            mario_swimming_update(&m);
        }
        m.prevAction = ACT_DIVE;
        m.action = ACT_WATER_PLUNGE;
        mario_swimming_update(&m);
        act_water_plunge(&m);
        EXPECT_EQ(m.action, ACT_WATER_PLUNGE);
        m.input = 0;
        m.flags &= ~MARIO_METAL_CAP;
        m.heldObj = NULL;
        mario_swimming_update(&m);
        act_water_plunge(&m);
        EXPECT_EQ(m.action, ACT_WATER_PLUNGE);
    }
    return 1;
}
static void advance_health_updates(int frames) {
    for (int frame = 0; frame < frames; frame++) {
        update_mario_health(&m);
    }
}

static int water_reentry(void) {
    begin(ACT_CRAWLING);
    m.flags = 0;
    m.waterLevel = 80;
    advance_health_updates(255);
    EXPECT_EQ(m.health, 0x880);
    update_mario_health(&m);
    EXPECT_EQ(m.health, 0x780);
    advance_health_updates(100);
    m.action = ACT_IDLE;
    advance_health_updates(30);
    m.action = ACT_CRAWLING;
    advance_health_updates(255);
    EXPECT_EQ(m.health, 0x780);
    update_mario_health(&m);
    EXPECT_EQ(m.health, 0x680);
    return 1;
}
static int poison_health_timing(void) {
    // Each wedge gets a full interval, including after our own damage.
    begin(ACT_IDLE);
    m.input = INPUT_IN_POISON_GAS;
    for (int cycle = 0; cycle < 3; cycle++) {
        int health = m.health;
        advance_health_updates(63);
        EXPECT_EQ(m.health, health);
        EXPECT_EQ(sPoisonHealth.drainProgress, 252);
        update_mario_health(&m);
        EXPECT_EQ(m.health, health - 0x100);
        EXPECT_EQ(sPoisonHealth.drainProgress, 0);
    }
    // A short exit preserves progress; ten continuous seconds restart it.
    const int safeFrames[] = { 29, 30, 255, 299, 300, 450 };
    for (unsigned exit = 0; exit < sizeof(safeFrames) / sizeof(*safeFrames); exit++) {
        int safe = safeFrames[exit];
        begin(ACT_IDLE);
        m.input = INPUT_IN_POISON_GAS;
        advance_health_updates(63);
        m.input = 0;
        advance_health_updates(safe);
        EXPECT_EQ(sPoisonHealth.cleanAirFrames, safe < 300 ? safe : 300);
        EXPECT_EQ(sPoisonHealth.drainProgress, safe < 300 ? 252 : 0);
        m.input = INPUT_IN_POISON_GAS;
        update_mario_health(&m);
        EXPECT_EQ(m.health, safe < 300 ? 0x780 : 0x880);
        if (safe >= 300) {
            advance_health_updates(62);
            EXPECT_EQ(m.health, 0x880);
            update_mario_health(&m);
            EXPECT_EQ(m.health, 0x780);
        }
    }
    // Gaining Metal, intangibility or debug protection resets the poison interval.
    for (int protection = 0; protection < 3; protection++) {
        begin(ACT_IDLE);
        m.input = INPUT_IN_POISON_GAS;
        advance_health_updates(63);
        if (protection == 0) {
            m.flags |= MARIO_METAL_CAP;
        }
        if (protection == 1) {
            m.action = ACT_DISAPPEARED;
        }
        if (protection == 2) {
            gDebugLevelSelect = TRUE;
        }
        advance_health_updates(100);
        EXPECT_EQ(m.health, 0x880);
        EXPECT_EQ(sPoisonHealth.drainProgress, 0);
        EXPECT_EQ(sPoisonHealth.cleanAirFrames, 0);
        m.flags &= ~(MARIO_METAL_CAP | MARIO_VANISH_CAP);
        m.action = ACT_IDLE;
        gDebugLevelSelect = FALSE;
        advance_health_updates(63);
        EXPECT_EQ(m.health, 0x880);
        update_mario_health(&m);
        EXPECT_EQ(m.health, 0x780);
    }
    // Queued and direct health changes restart both hazard intervals.
    for (int event = 0; event < 4; event++) {
        begin(ACT_IDLE);
        m.health = 0x580;
        mario_reset_health_regeneration(&m);
        m.input = INPUT_IN_POISON_GAS;
        advance_health_updates(63);
        if (event == 0) {
            m.healCounter = 1;
        }
        if (event == 1) {
            m.hurtCounter = 1;
        }
        if (event == 2) {
            m.health += 0x100;
        }
        if (event == 3) {
            m.health -= 0x100;
        }
        update_mario_health(&m);
        int health = m.health;
        EXPECT_EQ(sPoisonHealth.drainProgress, 0);
        advance_health_updates(63);
        EXPECT_EQ(m.health, health);
        update_mario_health(&m);
        EXPECT_EQ(m.health, health - 0x100);
    }
    // Water and gas retain separate progress; gas keeps priority during overlap.
    begin(ACT_WATER_IDLE);
    m.waterLevel = 400;
    advance_health_updates(10);
    EXPECT_EQ(sWaterHealth.drainProgress, 20);
    m.input = INPUT_IN_POISON_GAS;
    advance_health_updates(10);
    EXPECT_EQ(sPoisonHealth.drainProgress, 40);
    EXPECT_EQ(sWaterHealth.drainProgress, 20);
    m.input = 0;
    update_mario_health(&m);
    EXPECT_EQ(sPoisonHealth.drainProgress, 40);
    EXPECT_EQ(sWaterHealth.drainProgress, 22);
    m.input = INPUT_IN_POISON_GAS;
    advance_health_updates(54);
    EXPECT_EQ(m.health, 0x780);
    EXPECT_EQ(sWaterHealth.drainProgress, 0);
    EXPECT_EQ(sPoisonHealth.drainProgress, 0);
    // A fatal wedge stays fatal, and dead Mario accumulates no further exposure.
    begin(ACT_IDLE);
    m.health = 0x180;
    mario_reset_health_regeneration(&m);
    m.input = INPUT_IN_POISON_GAS;
    advance_health_updates(64);
    EXPECT_EQ(m.health, 0xff);
    advance_health_updates(1000);
    EXPECT_EQ(m.health, 0xff);
    EXPECT_EQ(sPoisonHealth.drainProgress, 0);
    mario_reset_health_regeneration(&m);
    EXPECT_EQ(sPoisonHealth.drainProgress, 0);
    return 1;
}
static int poison_contact_and_reentry(void) {
    for (int cycle = 0; cycle < 3; cycle++) {
        begin(ACT_IDLE);
        // At or above the surface is clean; shallow contact still starts exposure.
        const f32 heights[] = { 101, 100, 99.5f, 50, -1 };
        for (unsigned height = 0; height < sizeof(heights) / sizeof(*heights); height++) {
            m.pos[1] = heights[height];
            m.input = 0;
            update_mario_poison_input(&m, 100);
            EXPECT_EQ((m.input & INPUT_IN_POISON_GAS) != 0, height >= 2);
        }
        m.pos[1] = 99.5f;
        for (int frame = 0; frame < 63; frame++) {
            m.input = 0;
            update_mario_poison_input(&m, frame % 2 ? 100 : 200);
            update_mario_health(&m);
        }
        EXPECT_EQ(sPoisonHealth.drainProgress, 252);
        EXPECT_EQ(m.health, 0x880);
        // Returning before ten seconds breaks the outside streak, in any gas volume.
        m.pos[1] = 300;
        for (int frame = 0; frame < 299; frame++) {
            m.input = 0;
            update_mario_poison_input(&m, 100);
            update_mario_health(&m);
        }
        m.pos[1] = 199.5f;
        m.input = 0;
        update_mario_poison_input(&m, 200);
        update_mario_health(&m);
        EXPECT_EQ(m.health, 0x780);
        EXPECT_EQ(sPoisonHealth.cleanAirFrames, 0);
        advance_health_updates(10);
        m.input = 0;
        advance_health_updates(299);
        EXPECT_EQ(sPoisonHealth.drainProgress, 40);
        m.input = INPUT_IN_POISON_GAS;
        update_mario_health(&m);
        EXPECT_EQ(sPoisonHealth.drainProgress, 44);
        EXPECT_EQ(sPoisonHealth.cleanAirFrames, 0);
        m.input = 0;
        advance_health_updates(300);
        EXPECT_EQ(sPoisonHealth.drainProgress, 0);
        EXPECT_EQ(m.health, 0x780);
    }
    return 1;
}
static int capless_poison_and_crush(void) {
    for (int capped = 0; capped < 2; capped++) {
        begin(ACT_IDLE);
        m.flags = capped ? MARIO_CAP_ON_HEAD : 0;
        m.input = INPUT_IN_POISON_GAS;
        int interval = capped ? 64 : 32;
        for (int cycle = 0; cycle < 3; cycle++) {
            int health = m.health;
            advance_health_updates(interval - 1);
            EXPECT_EQ(m.health, health);
            update_mario_health(&m);
            EXPECT_EQ(m.health, health - 0x100);
        }
        // Partial exposure is retained when the hat changes during an interval.
        begin(ACT_IDLE);
        m.input = INPUT_IN_POISON_GAS;
        advance_health_updates(32);
        m.flags = 0;
        advance_health_updates(15);
        EXPECT_EQ(m.health, 0x880);
        update_mario_health(&m);
        EXPECT_EQ(m.health, 0x780);
        for (int metal = 0; metal < 2; metal++) {
            begin(ACT_SQUISHED);
            m.flags = (capped ? MARIO_CAP_ON_HEAD : 0) | (metal ? MARIO_METAL_CAP : 0);
            m.ceilHeight = 5;
            m.floorHeight = 0;
            int expectedHurt = capped ? 12 : 24;
            if (metal) {
                expectedHurt = 0;
            }
            act_squished(&m);
            EXPECT_EQ(m.hurtCounter, expectedHurt);
            for (int frame = 0; frame < 4; frame++) {
                act_squished(&m);
            }
            EXPECT_EQ(m.hurtCounter, expectedHurt);
        }
    }
    return 1;
}
static int underwater_shell_water_exits(void) {
    for (int route = 0; route < 5; route++) {
        begin(ACT_WATER_SHELL_SWIMMING);
        sShellMusicStops = 0;
        m.heldObj = &sHeld;
        m.waterLevel = 1000;
        if (route == 0) {
            m.pos[1] = 990;
        } else if (route == 1) {
            m.pos[1] = 1000;
        } else {
            m.pos[1] = 920;
        }
        m.floorHeight = route == 0 ? 980 : 0;
        m.forwardVel = 30;
        m.vel[2] = 30;
        m.input = route == 1 || route == 2 ? INPUT_A_DOWN : 0;
        m.controller->stickMag = 64;
        sDives = route == 1;
        m.intendedYaw = m.faceAngle[1] = 0;
        if (route == 3) {
            m.flags = MARIO_WING_CAP;
            m.input = INPUT_A_DOWN;
        }
        if (route == 4) {
            m.input = INPUT_A_PRESSED;
            m.controller->stickY = -64;
        }
        sSwimming.surfaceExitVelocityY = 40;
        sSwimming.surfaceSplashPlayed = TRUE;
        s32 exited;
        if (route < 2) {
            exited = check_common_submerged_cancels(&m);
        } else if (route == 4) {
            exited = check_water_jump(&m);
        } else {
            exited = try_swimming_surface_launch(&m);
        }
        EXPECT_EQ(exited, TRUE);
        u32 actions[] = { ACT_DECELERATING, ACT_DIVE, ACT_WATER_JUMP, ACT_FLYING, ACT_WATER_JUMP };
        EXPECT_EQ(m.action, actions[route]);
        EXPECT_EQ(m.heldObj == NULL, TRUE);
        EXPECT_EQ(sHeld.oInteractStatus, INT_STATUS_STOP_RIDING);
        EXPECT_EQ(sShellMusicStops, 1);
        if (route == 2 || route == 3) {
            NEAR(m.vel[1], (40.0f + 9.0f) * 1.2f);
            EXPECT_EQ(m.flags & MARIO_ACTION_SOUND_PLAYED, MARIO_ACTION_SOUND_PLAYED);
        }
        if (route == 2) {
            // The ordinary exit action keeps its existing ledge-grab handling.
            sAirStepResult = AIR_STEP_GRABBED_LEDGE;
            m.input = 0;
            act_water_jump(&m);
            EXPECT_EQ(m.action, ACT_LEDGE_GRAB);
            EXPECT_EQ(sShellMusicStops, 1);
        }
    }
    // Failed launch requests must keep the shell and its music.
    for (int blocked = 0; blocked < 5; blocked++) {
        begin(ACT_WATER_SHELL_SWIMMING);
        sShellMusicStops = 0;
        m.heldObj = &sHeld;
        m.waterLevel = 1000;
        m.pos[1] = blocked == 0 ? 919 : 920;
        m.input = blocked == 1 ? 0 : INPUT_A_DOWN;
        m.health = blocked == 2 ? 0xff : 0x880;
        sSwimming.surfaceExitVelocityY = blocked == 3 ? 25 : 40;
        if (blocked == 4) {
            m.floor = NULL;
        }
        EXPECT_EQ(try_swimming_surface_launch(&m), FALSE);
        EXPECT_EQ(m.action, ACT_WATER_SHELL_SWIMMING);
        EXPECT_EQ(m.heldObj == &sHeld, TRUE);
        EXPECT_EQ(sShellMusicStops, 0);
    }
    return TRUE;
}

static int wing_swimming_surface_flight(void) {
    for (int wing = 0; wing < 2; wing++) {
        for (int held = 0; held < 2; held++) {
            begin(held ? ACT_HOLD_FLUTTER_KICK : ACT_FLUTTER_KICK);
            m.flags = wing ? MARIO_WING_CAP : 0;
            m.heldObj = held ? &sObj : NULL;
            m.waterLevel = 1000;
            m.pos[1] = 920;
            m.input = INPUT_A_DOWN;
            m.vel[0] = 0;
            m.vel[2] = 40;
            m.angleVel[0] = 100;
            set_mario_animation(&m, MARIO_ANIM_FLUTTERKICK);
            sSwimming.surfaceExitVelocityY = 50;
            sSwimming.surfaceSplashPlayed = TRUE;
            EXPECT_EQ(try_swimming_surface_launch(&m), TRUE);
            f32 horizontal = (40.0f + 1.2f) * 1.2f;
            f32 upward = (50.0f + 9.0f) * 1.2f;
            u32 expectedAction = ACT_WATER_JUMP;
            if (held) {
                expectedAction = ACT_HOLD_WATER_JUMP;
            } else if (wing) {
                expectedAction = ACT_FLYING;
            }
            EXPECT_EQ(m.action, expectedAction);
            NEAR(m.vel[1], upward);
            NEAR(sqrtf(sqr(m.vel[0]) + sqr(m.vel[2])), horizontal);
            EXPECT_EQ(m.angleVel[0], 0);
            EXPECT_EQ(m.flags & MARIO_ACTION_SOUND_PLAYED, MARIO_ACTION_SOUND_PLAYED);
            if (wing && !held) {
                EXPECT_EQ(m.actionState, ACT_STATE_FLYING_FLY);
                EXPECT_EQ(m.actionArg, ACT_ARG_FLYING_DEFAULT);
                EXPECT_EQ(m.marioObj->header.gfx.animInfo.animID, MARIO_ANIM_WING_CAP_FLY);
                NEAR(m.forwardVel, sqrtf(sqr(horizontal) + sqr(upward)));
                EXPECT_EQ(m.faceAngle[0], atan2s(horizontal, upward));
            } else {
                EXPECT_EQ(m.marioObj->header.gfx.animInfo.animID, MARIO_ANIM_FLUTTERKICK);
                NEAR(m.forwardVel, horizontal);
            }
        }
    }
    // Obstructions, insufficient ascent, released A, death and damage cannot start flight.
    for (int blocked = 0; blocked < 5; blocked++) {
        begin(ACT_FLUTTER_KICK);
        m.flags = MARIO_WING_CAP;
        m.waterLevel = 1000;
        m.pos[1] = blocked == 0 ? 919 : 920;
        m.input = blocked == 2 ? 0 : INPUT_A_DOWN;
        m.health = blocked == 3 ? 0xff : 0x880;
        if (blocked == 4) {
            m.action = ACT_BACKWARD_WATER_KB;
        }
        sSwimming.surfaceExitVelocityY = blocked == 1 ? 25 : 50;
        u32 action = m.action;
        EXPECT_EQ(try_swimming_surface_launch(&m), FALSE);
        EXPECT_EQ(m.action, action);
    }
    // Original retains the same safe water-jump momentum and shell cleanup,
    // without entering Wing flight directly from the swimming launch.
    sTestGameOptions[GO_WING_CAP] = 1;
    for (s32 shell = 0; shell < 2; shell++) {
        begin(shell ? ACT_WATER_SHELL_SWIMMING : ACT_FLUTTER_KICK);
        m.flags = MARIO_WING_CAP;
        m.heldObj = shell ? &sHeld : NULL;
        m.waterLevel = 1000;
        m.pos[1] = 920;
        m.input = INPUT_A_DOWN;
        m.vel[2] = 40.0f;
        sSwimming.surfaceExitVelocityY = 50.0f;
        sShellMusicStops = 0;
        EXPECT_EQ(try_swimming_surface_launch(&m), TRUE);
        EXPECT_EQ(m.action, ACT_WATER_JUMP);
        NEAR(m.vel[1], 70.8f);
        NEAR(m.forwardVel, 49.44f);
        EXPECT_EQ(m.heldObj == NULL, TRUE);
        EXPECT_EQ(sShellMusicStops, shell);
    }
    sTestGameOptions[GO_WING_CAP] = 0;
    return TRUE;
}

static int air_twirl_coin_cooldown(void) {
    const s32 values[] = { 1, 2, 5 };
    for (unsigned i = 0; i < sizeof(values) / sizeof(*values); i++) {
        begin(ACT_FREEFALL);
        m.airTwirlCooldown = 30;
        m.airTwirlUsed = TRUE;
        m.airTwirlWingResetUsed = TRUE;
        m.airTwirlGravity = TRUE;
        sObj.oDamageOrCoinValue = values[i];
        interact_coin(&m, INTERACT_COIN, &sObj);
        EXPECT_EQ(m.airTwirlCooldown, 0);
        EXPECT_EQ(m.airTwirlCooldownResetByCoin, TRUE);
        EXPECT_EQ(m.airTwirlUsed, FALSE);
        EXPECT_EQ(m.airTwirlWingResetUsed, FALSE);
        EXPECT_EQ(m.airTwirlGravity, TRUE);
        EXPECT_EQ(m.action, ACT_FREEFALL);
    }
    return TRUE;
}

static int yoshi_coin_fatal_health(void) {
    // A gift costs one wedge, and only a newly fatal gift arms the cake.
    const s32 health[] = { 0x880, 0x280, 0x200, 0x180, 0x100, 0xff };
    for (unsigned i = 0; i < sizeof(health) / sizeof(*health); i++) {
        begin(ACT_WALKING);
        m.health = health[i];
        mario_reset_health_regeneration(&m);
        sObj.oDamageOrCoinValue = 1;
        sObj.oInteractionSubtype = INT_SUBTYPE_YOSHI_GIFT_COIN;
        interact_coin(&m, INTERACT_COIN, &sObj);
        s32 fatal = health[i] >= 0x100 && health[i] < 0x200;
        EXPECT_EQ(m.yoshiCoinDeathPending, fatal);
        EXPECT_EQ(m.healCounter, 0);
        for (s32 tick = 0; tick < 4; tick++) {
            update_mario_health(&m);
        }
        EXPECT_EQ(m.health, health[i] < 0x200 ? 0xff : health[i] - 0x100);
        EXPECT_EQ(m.yoshiCoinDeathPending, fatal);
    }
    // Healing rescues Mario before the gift finishes its debit.
    begin(ACT_WALKING);
    m.health = 0x180;
    mario_reset_health_regeneration(&m);
    sObj.oDamageOrCoinValue = 1;
    sObj.oInteractionSubtype = INT_SUBTYPE_YOSHI_GIFT_COIN;
    interact_coin(&m, INTERACT_COIN, &sObj);
    EXPECT_EQ(m.yoshiCoinDeathPending, TRUE);
    sObj.oInteractionSubtype = 0;
    interact_coin(&m, INTERACT_COIN, &sObj);
    advance_health_updates(4);
    EXPECT_EQ(m.health, 0x180);
    EXPECT_EQ(m.yoshiCoinDeathPending, FALSE);
    // Damage that was already fatal before the gift is not attributed to it.
    begin(ACT_WALKING);
    m.health = 0x180;
    m.hurtCounter = 4;
    sObj.oInteractionSubtype = INT_SUBTYPE_YOSHI_GIFT_COIN;
    interact_coin(&m, INTERACT_COIN, &sObj);
    EXPECT_EQ(m.yoshiCoinDeathPending, FALSE);
    return TRUE;
}

static int zero_health_healing(void) {
    const s16 deadHealth[] = { 0, 0x80, 0xff };
    for (unsigned i = 0; i < sizeof(deadHealth) / sizeof(*deadHealth); i++) {
        begin(ACT_WALKING);
        m.health = deadHealth[i];
        sObj.oDamageOrCoinValue = 2;
        interact_coin(&m, INTERACT_COIN, &sObj);
        EXPECT_EQ(m.numCoins, 2);
        EXPECT_EQ(sObj.oInteractStatus, INT_STATUS_INTERACTED);
        EXPECT_EQ(m.healCounter, 0);
        // Air bubbles and both water-ring families share this interaction.
        for (s32 value = 2; value <= 5; value += 3) {
            sObj.oDamageOrCoinValue = value;
            sObj.oInteractStatus = 0;
            interact_water_ring(&m, 0, &sObj);
            EXPECT_EQ(m.healCounter, 0);
            EXPECT_EQ(sObj.oInteractStatus, INT_STATUS_INTERACTED);
        }
        // Old healing, including a heart update later in the frame, cannot linger.
        m.healCounter = 31;
        sHealthRegenFrames = HEALTH_REGEN_START_FRAMES - 1;
        update_mario_health(&m);
        EXPECT_EQ(m.health, deadHealth[i]);
        EXPECT_EQ(m.healCounter, 0);
        EXPECT_EQ(sHealthRegenFrames, 0);
        advance_health_updates(1800);
        EXPECT_EQ(m.health, deadHealth[i]);
    }
    // Coins can rescue pending damage while alive, but cannot rescue a fatal update.
    begin(ACT_WALKING);
    m.health = 0x100;
    m.hurtCounter = 1;
    update_mario_health(&m);
    EXPECT_EQ(m.health, 0xff);
    sObj.oDamageOrCoinValue = 1;
    interact_coin(&m, INTERACT_COIN, &sObj);
    EXPECT_EQ(m.healCounter, 0);
    // A legitimate fresh spawn/course refill can heal normally again.
    m.health = 0x480;
    m.hurtCounter = 0;
    mario_reset_health_regeneration(&m);
    sObj.oDamageOrCoinValue = 1;
    interact_coin(&m, INTERACT_COIN, &sObj);
    advance_health_updates(4);
    EXPECT_EQ(m.health, 0x580);
    for (s32 value = 2; value <= 5; value += 3) {
        m.health = 0x100;
        sObj.oDamageOrCoinValue = value;
        interact_water_ring(&m, 0, &sObj);
        EXPECT_EQ(m.healCounter, 4 * value);
        advance_health_updates(4 * value);
        EXPECT_EQ(m.health, 0x100 + 0x100 * value);
    }
    return TRUE;
}

static int whole_wedge_hit_damage(void) {
    // Every hit amount drains one complete wedge per four updates.
    for (int wedges = 1; wedges <= 7; wedges++) {
        begin(ACT_WALKING);
        m.hurtCounter = 4 * wedges;
        mario_reset_health_regeneration(&m);
        for (int tick = 1; tick <= 4 * wedges; tick++) {
            update_mario_health(&m);
            EXPECT_EQ(m.health, 0x880 - (tick / 4) * 0x100);
            EXPECT_EQ(mario_health_after_pending_damage(&m), 0x880 - wedges * 0x100);
        }
    }
    // A second contact during the wait preserves both hits and the cadence.
    begin(ACT_WALKING);
    m.hurtCounter = 4;
    advance_health_updates(2);
    m.hurtCounter += 4;
    advance_health_updates(1);
    EXPECT_EQ(m.health, 0x880);
    advance_health_updates(1);
    EXPECT_EQ(m.health, 0x780);
    advance_health_updates(4);
    EXPECT_EQ(m.health, 0x680);
    // Pending lethal damage stays lethal until the complete debit lands.
    begin(ACT_WALKING);
    m.health = 0x180;
    m.hurtCounter = 4;
    for (int tick = 0; tick < 3; tick++) {
        update_mario_health(&m);
        EXPECT_EQ(m.health, 0x180);
        // Lethal projections use the same canonical death sentinel as live health.
        EXPECT_EQ(mario_health_after_pending_damage(&m), 0xFF);
    }
    update_mario_health(&m);
    EXPECT_EQ(m.health, 0xff);
    EXPECT_EQ(m.healCounter, 0);
    return TRUE;
}

static int whole_wedge_healing_preservation(void) {
    const s16 startingHealth[] = { 0x480, 0x830, 0x840, 0x870, 0x880 };
    for (unsigned h = 0; h < sizeof(startingHealth) / sizeof(*startingHealth); h++) {
        for (int damage = 1; damage <= 3; damage++) {
            for (int healing = 1; healing <= 5; healing++) {
                begin(ACT_WALKING);
                m.health = startingHealth[h];
                m.hurtCounter = damage * 4;
                m.healCounter = healing * 4;
                mario_reset_health_regeneration(&m);
                for (int tick = 0; tick < 40; tick++) {
                    update_mario_health(&m);
                    EXPECT_EQ(m.health <= 0x880, TRUE);
                    EXPECT_EQ(m.health >= 0x100, TRUE);
                }
                EXPECT_EQ(m.hurtCounter, 0);
                EXPECT_EQ(m.healCounter, 0);
                EXPECT_EQ(m.health, MIN(0x880, startingHealth[h] + (healing - damage) * 0x100));
            }
        }
    }
    // A healing tick that would partially overflow is retained until damage makes room.
    begin(ACT_WALKING);
    m.health = 0x870;
    m.hurtCounter = 4;
    m.healCounter = 4;
    advance_health_updates(3);
    EXPECT_EQ(m.health, 0x870);
    EXPECT_EQ(m.healCounter, 4);
    advance_health_updates(4);
    EXPECT_EQ(m.health, 0x870);
    EXPECT_EQ(m.healCounter, 0);
    // Healing still precedes the debit when the final wedge would otherwise be fatal.
    begin(ACT_WALKING);
    m.health = 0x180;
    m.hurtCounter = 4;
    m.healCounter = 4;
    advance_health_updates(4);
    EXPECT_EQ(m.health, 0x180);
    EXPECT_EQ(m.healCounter, 0);
    return TRUE;
}

static int burning_blocks_automatic_healing(void) {
    const u32 actions[] = { ACT_BURNING_GROUND, ACT_BURNING_JUMP, ACT_BURNING_FALL };
    const s32 progress[] = { 0, 53, 100, 160, 250 };
    for (unsigned a = 0; a < sizeof(actions) / sizeof(*actions); a++) {
        for (unsigned p = 0; p < sizeof(progress) / sizeof(*progress); p++) {
            begin(actions[a]);
            m.health = 0x480;
            mario_reset_health_regeneration(&m);
            sMarioObject.oMarioBurnTimer = progress[p];
            sHealthRegenFrames = 1349;
            update_mario_health(&m);
            EXPECT_EQ(m.health, 0x480);
            EXPECT_EQ(sHealthRegenFrames, 0);
            advance_health_updates(100);
            EXPECT_EQ(m.health, 0x480);
            EXPECT_EQ(sHealthRegenFrames, 0);
            m.action = ACT_WALKING;
            advance_health_updates(1349);
            EXPECT_EQ(m.health, 0x480);
            update_mario_health(&m);
            EXPECT_EQ(m.health, 0x580);
        }
    }
    // Exercise actual ignition before its first whole-wedge debit.
    begin(ACT_BURNING_GROUND);
    m.health = 0x480;
    mario_reset_health_regeneration(&m);
    sHealthRegenFrames = 1349;
    m.marioBodyState = &(struct TestBodyState){ 0 };
    m.waterLevel = FLOOR_LOWER_LIMIT;
    burning_test_update();
    update_mario_health(&m);
    EXPECT_EQ(m.health, 0x480);
    EXPECT_EQ(sHealthRegenFrames, 0);
    return TRUE;
}

static int health_regeneration_timing(void) {
    begin(ACT_WALKING);
    m.health = 0x480;
    mario_reset_health_regeneration(&m);
    advance_health_updates(1349);
    EXPECT_EQ(m.health, 0x480);
    update_mario_health(&m);
    EXPECT_EQ(m.health, 0x580);
    advance_health_updates(89);
    EXPECT_EQ(m.health, 0x580);
    update_mario_health(&m);
    EXPECT_EQ(m.health, 0x680);

    m.hurtCounter = 1;
    update_mario_health(&m);
    EXPECT_EQ(m.health, 0x580);
    advance_health_updates(1349);
    EXPECT_EQ(m.health, 0x580);
    update_mario_health(&m);
    EXPECT_EQ(m.health, 0x680);

    begin(ACT_WALKING);
    m.health = 0x480;
    mario_reset_health_regeneration(&m);
    advance_health_updates(1349);
    m.health = 0x380;
    m.healCounter = 1;
    update_mario_health(&m);
    EXPECT_EQ(m.health, 0x3c0);
    advance_health_updates(1349);
    EXPECT_EQ(m.health, 0x3c0);
    update_mario_health(&m);
    EXPECT_EQ(m.health, 0x4c0);

    begin(ACT_WALKING);
    m.health = 0xff;
    mario_reset_health_regeneration(&m);
    advance_health_updates(3000);
    EXPECT_EQ(m.health, 0xff);
    return 1;
}
static int health_regeneration_idle_reset(void) {
    const u32 waitingActions[] = { ACT_START_SLEEPING, ACT_SLEEPING };
    for (unsigned action = 0; action < sizeof(waitingActions) / sizeof(*waitingActions); action++) {
        for (int cycle = 0; cycle < 3; cycle++) {
            begin(ACT_WALKING);
            m.health = 0x480;
            mario_reset_health_regeneration(&m);
            // Vanish prevents environmental eligibility from masking the sleep reset.
            m.flags |= MARIO_VANISH_CAP;
            advance_health_updates(1349);
            EXPECT_EQ(sHealthRegenFrames, 1349);
            EXPECT_EQ(m.health, 0x480);
            m.action = waitingActions[action];
            update_mario_health(&m);
            EXPECT_EQ(sHealthRegenFrames, 0);
            EXPECT_EQ(m.health, 0x480);
            advance_health_updates(1900);
            EXPECT_EQ(sHealthRegenFrames, 0);
            EXPECT_EQ(m.health, 0x480);
            m.action = ACT_WALKING;
            advance_health_updates(1349);
            EXPECT_EQ(m.health, 0x480);
            update_mario_health(&m);
            EXPECT_EQ(m.health, 0x580);
            advance_health_updates(89);
            // Falling asleep and sleep reset the interval after healing starts as well.
            m.action = waitingActions[action];
            update_mario_health(&m);
            EXPECT_EQ(sHealthRegenFrames, 0);
            EXPECT_EQ(m.health, 0x580);
        }
    }
    // Ordinary idle, including idle camera timing, does not reset auto-heal.
    const u32 ordinaryIdle[] = { ACT_IDLE,
                                 ACT_PANTING,
                                 ACT_STANDING_AGAINST_WALL,
                                 ACT_COUGHING,
                                 ACT_SHIVERING,
                                 ACT_HOLD_IDLE,
                                 ACT_HOLD_HEAVY_IDLE,
                                 ACT_WATER_IDLE,
                                 ACT_HOLD_WATER_IDLE,
                                 ACT_METAL_WATER_STANDING,
                                 ACT_HOLD_METAL_WATER_STANDING };
    for (unsigned action = 0; action < sizeof(ordinaryIdle) / sizeof(*ordinaryIdle); action++) {
        begin(ordinaryIdle[action]);
        m.flags |= MARIO_VANISH_CAP;
        m.health = 0x480;
        mario_reset_health_regeneration(&m);
        advance_health_updates(1349);
        EXPECT_EQ(sHealthRegenFrames, 1349);
        EXPECT_EQ(m.health, 0x480);
        update_mario_health(&m);
        EXPECT_EQ(m.health, 0x580);
    }
    // Ordinary coin/heart healing still works while idle and asleep.
    begin(ACT_SLEEPING);
    m.health = 0x480;
    m.healCounter = 4;
    mario_reset_health_regeneration(&m);
    advance_health_updates(4);
    EXPECT_EQ(m.health, 0x580);
    EXPECT_EQ(sHealthRegenFrames, 0);
    // The sleep reset must not erase poison progress on each stationary update.
    begin(ACT_START_SLEEPING);
    m.input = INPUT_IN_POISON_GAS;
    mario_reset_health_regeneration(&m);
    advance_health_updates(63);
    EXPECT_EQ(sPoisonHealth.drainProgress, 252);
    update_mario_health(&m);
    EXPECT_EQ(m.health, 0x780);
    EXPECT_EQ(sHealthRegenFrames, 0);
    // Repeated wedges remain exactly 90 updates apart and cap partial final health.
    begin(ACT_WALKING);
    m.health = 0x480;
    mario_reset_health_regeneration(&m);
    advance_health_updates(1350);
    for (int wedge = 0; wedge < 3; wedge++) {
        int health = m.health;
        advance_health_updates(89);
        EXPECT_EQ(m.health, health);
        update_mario_health(&m);
        EXPECT_EQ(m.health, health + 0x100);
    }
    m.health = 0x800;
    mario_reset_health_regeneration(&m);
    advance_health_updates(1350);
    EXPECT_EQ(m.health, 0x880);
    return 1;
}
static int health_regeneration_equivalent_sleep(void) {
    const u32 restActions[] = { ACT_IDLE,
                                ACT_HOLD_IDLE,
                                ACT_HOLD_HEAVY_IDLE,
                                ACT_CROUCHING,
                                ACT_WATER_IDLE,
                                ACT_HOLD_WATER_IDLE,
                                ACT_METAL_WATER_STANDING,
                                ACT_HOLD_METAL_WATER_STANDING,
                                ACT_HOLDING_POLE,
                                ACT_TOP_OF_POLE,
                                ACT_HANGING,
                                ACT_LEDGE_GRAB,
                                ACT_FIRST_PERSON };
    for (unsigned action = 0; action < sizeof(restActions) / sizeof(*restActions); action++) {
        for (int capped = 0; capped < 2; capped++) {
            begin(restActions[action]);
            m.health = 0x480;
            m.flags = capped ? (MARIO_CAP_ON_HEAD | MARIO_NORMAL_CAP) : 0;
            // Surface swimming is protected from environmental reset so the inactivity
            // helper is tested directly; other cases exercise its health integration.
            int delay = capped ? 2700 : 900;
            if (restActions[action] == ACT_CROUCHING) {
                sController.buttonDown = Z_TRIG;
            }
            for (int frame = 0; frame < delay - 1; frame++) {
                EXPECT_EQ(mario_health_regeneration_sleep_due(&m), FALSE);
            }
            EXPECT_EQ(sHealthRegenInactiveFrames, delay - 1);
            sHealthRegenFrames = 1349;
            update_mario_health(&m);
            EXPECT_EQ(sHealthRegenInactiveFrames, delay);
            EXPECT_EQ(sHealthRegenFrames, 0);
            EXPECT_EQ(m.health, 0x480);
            advance_health_updates(1350);
            EXPECT_EQ(m.health, 0x480);
            EXPECT_EQ(sHealthRegenFrames, 0);
            // A fresh button press starts a new inactivity window, including at rest.
            sController.buttonPressed = Z_TRIG;
            update_mario_health(&m);
            EXPECT_EQ(sHealthRegenInactiveFrames, 0);
            sController.buttonPressed = 0;
        }
    }
    // Resting-state changes alone cannot evade the shared inactivity limit.
    begin(ACT_CROUCHING);
    m.flags = 0;
    for (int cycle = 0; cycle < 3; cycle++) {
        for (int frame = 0; frame < 899; frame++) {
            m.action = restActions[frame % (sizeof(restActions) / sizeof(*restActions))];
            EXPECT_EQ(mario_health_regeneration_sleep_due(&m), FALSE);
        }
        EXPECT_EQ(mario_health_regeneration_sleep_due(&m), TRUE);
        m.input = INPUT_NONZERO_ANALOG;
        EXPECT_EQ(mario_health_regeneration_sleep_due(&m), FALSE);
        m.input = 0;
    }
    // Activity restarts the inactivity clock and allows the full auto-heal wait.
    begin(ACT_CROUCHING);
    m.health = 0x480;
    mario_reset_health_regeneration(&m);
    m.flags |= MARIO_NORMAL_CAP;
    for (int cycle = 0; cycle < 3; cycle++) {
        for (int frame = 0; frame < 2699; frame++) {
            mario_health_regeneration_sleep_due(&m);
        }
        m.input = INPUT_NONZERO_ANALOG;
        update_mario_health(&m);
        EXPECT_EQ(sHealthRegenInactiveFrames, 0);
        m.input = 0;
    }
    m.action = ACT_WALKING;
    m.forwardVel = 10;
    advance_health_updates(1346);
    EXPECT_EQ(m.health, 0x480);
    update_mario_health(&m);
    EXPECT_EQ(m.health, 0x580);
    EXPECT_EQ(sHealthRegenInactiveFrames, 0);
    // Active attacks, forced/intangible actions and moving resting states do not
    // accumulate an equivalent sleep clock. Spawn/refill clears it too.
    const u32 activeActions[] = { ACT_PUNCHING, ACT_JUMP, ACT_READING_NPC_DIALOG };
    for (unsigned action = 0; action < sizeof(activeActions) / sizeof(*activeActions); action++) {
        m.action = activeActions[action];
        sHealthRegenInactiveFrames = 800;
        EXPECT_EQ(mario_health_regeneration_sleep_due(&m), FALSE);
        EXPECT_EQ(sHealthRegenInactiveFrames, 0);
    }
    m.action = ACT_HOLD_IDLE;
    m.forwardVel = 1;
    sHealthRegenInactiveFrames = 800;
    EXPECT_EQ(mario_health_regeneration_sleep_due(&m), FALSE);
    EXPECT_EQ(sHealthRegenInactiveFrames, 0);
    sHealthRegenInactiveFrames = 800;
    mario_reset_health_regeneration(&m);
    EXPECT_EQ(sHealthRegenInactiveFrames, 0);
    return 1;
}
static int health_regeneration_exposure_reset(void) {
    begin(ACT_WALKING);
    m.health = 0x480;
    mario_reset_health_regeneration(&m);
    advance_health_updates(1349);
    m.action = ACT_CRAWLING;
    m.waterLevel = 80;
    advance_health_updates(89);
    EXPECT_EQ(m.health, 0x480);
    EXPECT_EQ(sShallowRegenExposureFrames, 89);
    m.action = ACT_WALKING;
    m.waterLevel = 0;
    update_mario_health(&m);
    EXPECT_EQ(m.health, 0x580);

    begin(ACT_WALKING);
    m.health = 0x480;
    mario_reset_health_regeneration(&m);
    advance_health_updates(1348);
    m.action = ACT_CRAWLING;
    m.waterLevel = 80;
    advance_health_updates(45);
    m.action = ACT_WALKING;
    m.waterLevel = 0;
    update_mario_health(&m);
    m.action = ACT_CRAWLING;
    m.waterLevel = 80;
    advance_health_updates(45);
    EXPECT_EQ(sShallowRegenExposureFrames, 45);
    m.action = ACT_WALKING;
    m.waterLevel = 0;
    update_mario_health(&m);
    EXPECT_EQ(m.health, 0x580);

    begin(ACT_WALKING);
    m.health = 0x480;
    mario_reset_health_regeneration(&m);
    advance_health_updates(1349);
    m.action = ACT_CRAWLING;
    m.waterLevel = 80;
    advance_health_updates(90);
    EXPECT_EQ(m.health, 0x480);
    EXPECT_EQ(sHealthRegenFrames, 0);
    m.action = ACT_WALKING;
    m.waterLevel = 0;
    advance_health_updates(1349);
    EXPECT_EQ(m.health, 0x480);
    update_mario_health(&m);
    EXPECT_EQ(m.health, 0x580);

    begin(ACT_WALKING);
    m.health = 0x480;
    mario_reset_health_regeneration(&m);
    advance_health_updates(1349);
    m.action = ACT_FLUTTER_KICK;
    m.waterLevel = 400;
    update_mario_health(&m);
    EXPECT_EQ(m.health, 0x480);
    EXPECT_EQ(sHealthRegenFrames, 0);
    m.action = ACT_WALKING;
    m.waterLevel = 0;
    advance_health_updates(1349);
    EXPECT_EQ(m.health, 0x480);
    update_mario_health(&m);
    EXPECT_EQ(m.health, 0x580);

    begin(ACT_WALKING);
    m.health = 0x480;
    mario_reset_health_regeneration(&m);
    advance_health_updates(1349);
    m.action = ACT_FLUTTER_KICK;
    m.waterLevel = 100;
    update_mario_health(&m);
    EXPECT_EQ(m.health, 0x480);
    EXPECT_EQ(sHealthRegenFrames, 0);

    begin(ACT_WALKING);
    m.health = 0x480;
    mario_reset_health_regeneration(&m);
    advance_health_updates(1349);
    m.input = INPUT_IN_POISON_GAS;
    m.healCounter = 1;
    update_mario_health(&m);
    EXPECT_EQ(m.health, 0x4c0);
    EXPECT_EQ(sHealthRegenFrames, 0);
    m.input = 0;
    advance_health_updates(1349);
    EXPECT_EQ(m.health, 0x4c0);
    update_mario_health(&m);
    EXPECT_EQ(m.health, 0x5c0);
    return 1;
}
static int breakdance_poison_windows(void) {
    struct Animation spin = { .loopEnd = 24 };
    const u32 actions[] = { ACT_PUNCHING, ACT_MOVE_PUNCHING };
    for (unsigned action = 0; action < sizeof(actions) / sizeof(*actions); action++) {
        for (int frame = 0; frame < 24; frame++) {
            begin(ACT_IDLE);
            m.input = INPUT_IN_POISON_GAS;
            advance_health_updates(63);
            EXPECT_EQ(m.health, 0x880);
            m.action = actions[action];
            m.actionArg = ACT_ARG_PUNCH_SEQUENCE_BREAKDANCE;
            m.actionTimer = 1;
            sMarioObject.header.gfx.animInfo.animID = MARIO_ANIM_BREAKDANCE;
            sMarioObject.header.gfx.animInfo.animFrame = frame;
            sMarioObject.header.gfx.animInfo.curAnim = &spin;
            update_mario_health(&m);
            EXPECT_EQ(m.health, frame == 0 || frame == 12 ? 0x780 : 0x880);
        }
    }
    return TRUE;
}

static int cap_environment_vulnerabilities(void) {
    // All power combinations, ordinary and cold water, standing and low poses.
    for (int powers = 0; powers < 8; powers++) {
        for (int cold = 0; cold < 2; cold++) {
            for (int low = 0; low < 2; low++) {
                begin(low ? ACT_CRAWLING : ACT_IDLE);
                m.waterLevel = 400;
                u32 flags = (powers & 1 ? MARIO_WING_CAP : 0) | (powers & 2 ? MARIO_METAL_CAP : 0)
                            | (powers & 4 ? MARIO_VANISH_CAP : 0);
                m.flags |= flags;
                sArea.terrainType = cold ? TERRAIN_SNOW : TERRAIN_GRASS;
                if (!(flags & (MARIO_METAL_CAP | MARIO_VANISH_CAP)) && !low) {
                    m.action = ACT_WATER_IDLE;
                }
                int interval = cold ? 128 : 384;
                advance_health_updates(interval - 1);
                EXPECT_EQ(m.health, 0x880);
                update_mario_health(&m);
                EXPECT_EQ(m.health, flags & MARIO_VANISH_CAP ? 0x880 : 0x780);
                EXPECT_EQ(sHealthRegenFrames, 0);
                m.health = 0xff;
                m.action = low ? ACT_CRAWLING : ACT_IDLE;
                EXPECT_EQ(mario_should_drown_on_land(&m),
                          !(flags & MARIO_VANISH_CAP) && (low || (flags & MARIO_METAL_CAP)));
                begin(ACT_IDLE);
                m.flags |= flags;
                m.input = INPUT_IN_POISON_GAS;
                advance_health_updates(63);
                EXPECT_EQ(m.health, 0x880);
                update_mario_health(&m);
                EXPECT_EQ(m.health, flags & MARIO_METAL_CAP ? 0x880 : 0x780);
            }
        }
    }
    // Metal drowning repeats the ordinary wedge interval and cannot regenerate underwater.
    begin(ACT_WALKING);
    m.flags |= MARIO_METAL_CAP;
    m.waterLevel = 400;
    for (int cycle = 0; cycle < 3; cycle++) {
        int health = m.health;
        advance_health_updates(384);
        EXPECT_EQ(m.health, health - 0x100);
        EXPECT_EQ(sHealthRegenFrames, 0);
    }
    // Vanish pauses a Metal water interval; gaining Metal resets a Vanish poison interval.
    begin(ACT_IDLE);
    m.flags |= MARIO_METAL_CAP;
    m.waterLevel = 400;
    advance_health_updates(383);
    m.flags |= MARIO_VANISH_CAP;
    advance_health_updates(50);
    EXPECT_EQ(m.health, 0x880);
    m.flags &= ~MARIO_VANISH_CAP;
    update_mario_health(&m);
    EXPECT_EQ(m.health, 0x780);
    begin(ACT_IDLE);
    m.flags |= MARIO_VANISH_CAP;
    m.input = INPUT_IN_POISON_GAS;
    advance_health_updates(63);
    m.flags |= MARIO_METAL_CAP;
    advance_health_updates(50);
    EXPECT_EQ(m.health, 0x880);
    m.flags &= ~MARIO_METAL_CAP;
    advance_health_updates(63);
    EXPECT_EQ(m.health, 0x880);
    update_mario_health(&m);
    EXPECT_EQ(m.health, 0x780);
    return 1;
}
static int powered_cap_theft_and_wind(void) {
    for (int powers = 0; powers < 8; powers++) {
        for (int enemy = 1; enemy <= 2; enemy++) {
            begin(ACT_IDLE);
            m.flags = MARIO_NORMAL_CAP | MARIO_CAP_ON_HEAD | (powers & 1 ? MARIO_WING_CAP : 0)
                      | (powers & 2 ? MARIO_METAL_CAP : 0) | (powers & 4 ? MARIO_VANISH_CAP : 0);
            m.capTimer = 100;
            sCapMusicStops = 0;
            sSavedCapOwnershipFlags = 0;
            EXPECT_EQ(mario_can_lose_cap_to_enemy(&m), TRUE);
            EXPECT_EQ(mario_lose_cap_to_enemy(enemy), TRUE);
            EXPECT_EQ(m.flags & MARIO_CAPS, 0);
            EXPECT_EQ(m.capTimer, powers ? 0 : 100);
            EXPECT_EQ(sCapMusicStops, powers ? 1 : 0);
            EXPECT_EQ(sSavedCapOwnershipFlags,
                      enemy == 1 ? SAVE_FLAG_CAP_ON_KLEPTO : SAVE_FLAG_CAP_ON_UKIKI);
            EXPECT_EQ(mario_lose_cap_to_enemy(enemy), FALSE);
            EXPECT_EQ(sCapMusicStops, powers ? 1 : 0);
            EXPECT_EQ(mario_can_lose_cap_to_enemy(&m), FALSE);
        }
    }
    begin(ACT_IDLE);
    m.flags = MARIO_NORMAL_CAP | MARIO_CAP_IN_HAND | MARIO_VANISH_CAP;
    EXPECT_EQ(mario_lose_cap_to_enemy(2), FALSE);
    for (int protection = 0; protection < 3; protection++) {
        begin(ACT_GETTING_BLOWN);
        m.flags = MARIO_NORMAL_CAP | MARIO_CAP_ON_HEAD | MARIO_VANISH_CAP
                  | (protection == 1   ? MARIO_METAL_CAP
                     : protection == 2 ? MARIO_WING_CAP
                                       : 0);
        m.capTimer = 100;
        sCapMusicStops = 0;
        sDizzyCapBlows = 0;
        mario_blow_off_cap_from_wind(&m, 50);
        EXPECT_EQ(m.flags & MARIO_NORMAL_CAP, protection ? MARIO_NORMAL_CAP : 0);
        EXPECT_EQ(m.flags & MARIO_VANISH_CAP, protection ? MARIO_VANISH_CAP : 0);
        EXPECT_EQ(sDizzyCapBlows, protection ? 0 : 1);
        EXPECT_EQ(sCapMusicStops, protection ? 0 : 1);
        if (!protection) {
            EXPECT_EQ(m.capTimer, 0);
        }
    }
    return 1;
}
static int vanish_environment(void) {
    u32 actions[] = { ACT_IDLE,          ACT_WALKING,  ACT_CROUCHING, ACT_CRAWLING,
                      ACT_STOMACH_SLIDE, ACT_FREEFALL, ACT_WATER_IDLE };
    for (unsigned action = 0; action < sizeof(actions) / sizeof(*actions); action++) {
        for (int cold = 0; cold < 2; cold++) {
            begin(actions[action]);
            m.flags |= MARIO_VANISH_CAP;
            m.waterLevel = 400;
            sArea.terrainType = cold ? TERRAIN_SNOW : TERRAIN_GRASS;
            for (int frame = 0; frame < 2000; frame++) {
                update_mario_health(&m);
            }
            EXPECT_EQ(m.health, 0x880);
            EXPECT_EQ(sWaterHealth.drainProgress, 0);
            m.health = 0xff;
            EXPECT_EQ(mario_should_drown_on_land(&m), FALSE);
        }
    }
    // Immunity pauses a partially filled drain interval; expiry restores normal timing.
    for (int cycle = 0; cycle < 3; cycle++) {
        begin(ACT_CRAWLING);
        m.waterLevel = 80;
        for (int frame = 0; frame < 200; frame++) {
            update_mario_health(&m);
        }
        EXPECT_EQ(sWaterHealth.drainProgress, 400);
        m.flags |= MARIO_VANISH_CAP;
        for (int frame = 0; frame < 2000; frame++) {
            update_mario_health(&m);
        }
        EXPECT_EQ(m.health, 0x880);
        EXPECT_EQ(sWaterHealth.drainProgress, 400);
        m.flags &= ~MARIO_VANISH_CAP;
        for (int frame = 0; frame < 183; frame++) {
            update_mario_health(&m);
        }
        EXPECT_EQ(m.health, 0x880);
        update_mario_health(&m);
        EXPECT_EQ(m.health, 0x780);
    }
    begin(ACT_IDLE);
    m.input = INPUT_IN_POISON_GAS;
    m.flags |= MARIO_VANISH_CAP;
    for (int frame = 0; frame < 63; frame++) {
        update_mario_health(&m);
    }
    EXPECT_EQ(m.health, 0x880);
    update_mario_health(&m);
    EXPECT_EQ(m.health, 0x780);
    begin(ACT_IDLE);
    m.input = INPUT_IN_POISON_GAS;
    m.flags |= MARIO_METAL_CAP;
    for (int frame = 0; frame < 2000; frame++) {
        update_mario_health(&m);
    }
    EXPECT_EQ(m.health, 0x880);
    // Ordinary queued damage/healing still applies while protected from the environment.
    begin(ACT_CRAWLING);
    m.waterLevel = 400;
    m.input = INPUT_IN_POISON_GAS;
    m.flags |= MARIO_VANISH_CAP;
    m.hurtCounter = 1;
    update_mario_health(&m);
    EXPECT_EQ(m.health, 0x780);
    EXPECT_EQ(m.hurtCounter, 0);
    m.healCounter = 1;
    update_mario_health(&m);
    EXPECT_EQ(m.health, 0x7c0);
    EXPECT_EQ(m.healCounter, 0);
    for (int vanish = 0; vanish < 2; vanish++) {
        begin(ACT_WATER_IDLE);
        m.waterLevel = 400;
        m.health = 0xff;
        if (vanish) {
            m.flags |= MARIO_VANISH_CAP;
        }
        EXPECT_EQ(check_common_submerged_cancels(&m), FALSE);
        EXPECT_EQ(m.action, vanish ? ACT_WATER_DEATH : ACT_DROWNING);
    }
    return 1;
}
static int vanish_bubbles(void) {
    u32 actions[] = { ACT_IDLE,     ACT_WALKING,      ACT_CRAWLING,   ACT_STOMACH_SLIDE,
                      ACT_FREEFALL, ACT_GROUND_POUND, ACT_WATER_IDLE, ACT_METAL_WATER_FALLING };
    for (unsigned action = 0; action < sizeof(actions) / sizeof(*actions); action++) {
        for (int metal = 0; metal < 2; metal++) {
            for (int cycle = 0; cycle < 3; cycle++) {
                begin(actions[action]);
                m.flags |= MARIO_VANISH_CAP | (metal ? MARIO_METAL_CAP : 0);
                m.waterLevel = 400;
                update_mario_submerged_effects(&m);
                EXPECT_EQ(m.particleFlags & PARTICLE_BUBBLE, PARTICLE_BUBBLE);
                m.particleFlags = 0;
                m.waterLevel = -1000;
                m.faceAngle[0] = 0;
                update_mario_submerged_effects(&m);
                EXPECT_EQ(m.particleFlags & PARTICLE_BUBBLE, 0);
                m.waterLevel = 400;
                m.action |= ACT_FLAG_INTANGIBLE;
                update_mario_submerged_effects(&m);
                EXPECT_EQ(m.particleFlags & PARTICLE_BUBBLE, 0);
            }
        }
    }
    for (int low = 0; low < 2; low++) {
        begin(low ? ACT_CRAWLING : ACT_IDLE);
        m.flags |= MARIO_VANISH_CAP;
        m.waterLevel = low ? 50 : 140;
        update_mario_submerged_effects(&m);
        EXPECT_EQ(m.particleFlags & PARTICLE_BUBBLE, 0);
        m.waterLevel++;
        update_mario_submerged_effects(&m);
        EXPECT_EQ(m.particleFlags & PARTICLE_BUBBLE, PARTICLE_BUBBLE);
    }
    begin(ACT_WALKING);
    m.flags |= MARIO_VANISH_CAP;
    m.waterLevel = 400;
    update_mario_submerged_effects(&m);
    m.particleFlags = 0;
    m.flags &= ~MARIO_VANISH_CAP;
    update_mario_submerged_effects(&m);
    EXPECT_EQ(m.particleFlags & PARTICLE_BUBBLE, 0);
    begin(ACT_WATER_IDLE);
    m.waterLevel = 200;
    update_mario_submerged_effects(&m);
    EXPECT_EQ(m.particleFlags & PARTICLE_BUBBLE, PARTICLE_BUBBLE);
    begin(ACT_WATER_IDLE);
    m.waterLevel = 100;
    update_mario_submerged_effects(&m);
    EXPECT_EQ(m.particleFlags & PARTICLE_BUBBLE, 0);
    m.faceAngle[0] = -0x1000;
    update_mario_submerged_effects(&m);
    EXPECT_EQ(m.particleFlags & PARTICLE_BUBBLE, PARTICLE_BUBBLE);
    begin(ACT_METAL_WATER_FALLING);
    m.flags |= MARIO_METAL_CAP;
    m.waterLevel = 400;
    update_mario_submerged_effects(&m);
    EXPECT_EQ(m.particleFlags & PARTICLE_BUBBLE, 0);
    return 1;
}
static int vanish_surface_effects(void) {
    const u32 trail = PARTICLE_IDLE_WATER_WAVE | PARTICLE_WAVE_TRAIL;
    const u32 splash =
        PARTICLE_SHALLOW_WATER_WAVE | PARTICLE_SHALLOW_WATER_SPLASH | PARTICLE_WATER_SPLASH;
    for (int low = 0; low < 2; low++) {
        for (int metal = 0; metal < 2; metal++) {
            begin(low ? ACT_CRAWLING : ACT_WALKING);
            m.flags |= MARIO_VANISH_CAP | (metal ? MARIO_METAL_CAP : 0);
            m.waterLevel = low ? 51 : 141;
            m.particleFlags = trail | splash;
            update_mario_submerged_effects(&m);
            EXPECT_EQ(m.particleFlags & trail, trail);
            EXPECT_EQ(m.particleFlags & splash, 0);
            EXPECT_EQ(m.particleFlags & PARTICLE_BUBBLE, PARTICLE_BUBBLE);
            m.flags &= ~MARIO_VANISH_CAP;
            m.particleFlags = trail | splash;
            update_mario_submerged_effects(&m);
            EXPECT_EQ(m.particleFlags & (trail | splash), metal ? trail : trail | splash);
            EXPECT_EQ(m.particleFlags & PARTICLE_BUBBLE, 0);
        }
    }
    begin(ACT_WALKING);
    m.flags |= MARIO_VANISH_CAP;
    m.waterLevel = 140;
    m.particleFlags = trail | splash;
    update_mario_submerged_effects(&m);
    EXPECT_EQ(m.particleFlags & (trail | splash), trail);
    begin(ACT_BREASTSTROKE);
    m.flags |= MARIO_VANISH_CAP;
    m.waterLevel = 400;
    m.particleFlags = trail | splash;
    update_mario_submerged_effects(&m);
    EXPECT_EQ(m.particleFlags & trail, trail);
    EXPECT_EQ(m.particleFlags & splash, 0);
    begin(ACT_JUMP);
    m.flags |= MARIO_VANISH_CAP;
    m.waterLevel = 400;
    update_mario_submerged_effects(&m);
    EXPECT_EQ(m.particleFlags & PARTICLE_IDLE_WATER_WAVE, PARTICLE_IDLE_WATER_WAVE);
    m.pos[0] = 100;
    m.particleFlags = 0;
    update_mario_submerged_effects(&m);
    EXPECT_EQ(m.particleFlags & PARTICLE_WAVE_TRAIL, PARTICLE_WAVE_TRAIL);
    return 1;
}
static int wing_water_effects_and_friction(void) {
    for (s32 metal = 0; metal < 2; metal++) {
        begin(ACT_WALKING);
        m.flags |= MARIO_WING_CAP | MARIO_VANISH_CAP | (metal ? MARIO_METAL_CAP : 0);
        m.floor = &gWaterSurfacePseudoFloor;
        m.waterLevel = m.floorHeight = m.pos[1] = 0.0f;
        m.terrainSoundAddend = mario_get_terrain_sound_addend(&m);
        u32 terrainSound = m.terrainSoundAddend;
        play_sound_and_spawn_particles(&m, metal ? SOUND_ACTION_METAL_STEP : SOUND_ACTION_TERRAIN_STEP,
                                       0);
        EXPECT_EQ(m.particleFlags & PARTICLE_SHALLOW_WATER_WAVE, PARTICLE_SHALLOW_WATER_WAVE);
        EXPECT_EQ(sLastMarioSound,
                  metal ? SOUND_ACTION_METAL_STEP : SOUND_ACTION_TERRAIN_STEP + terrainSound);
        real_play_mario_landing_sound(&m, SOUND_ACTION_TERRAIN_LANDING);
        EXPECT_EQ(m.particleFlags & PARTICLE_SHALLOW_WATER_SPLASH, PARTICLE_SHALLOW_WATER_SPLASH);
        EXPECT_EQ(sLastMarioSound,
                  metal ? SOUND_ACTION_METAL_LANDING : SOUND_ACTION_TERRAIN_LANDING + terrainSound);
        update_mario_submerged_effects(&m);
        EXPECT_EQ(m.particleFlags & PARTICLE_SHALLOW_WATER_WAVE, PARTICLE_SHALLOW_WATER_WAVE);
        EXPECT_EQ(m.particleFlags & PARTICLE_SHALLOW_WATER_SPLASH, PARTICLE_SHALLOW_WATER_SPLASH);
    }
    begin(ACT_WALKING);
    m.flags |= MARIO_WING_CAP;
    m.floor = &gWaterSurfacePseudoFloor;
    gWaterSurfacePseudoFloor = (struct Surface){ .type = SURFACE_VERY_SLIPPERY, .normal.y = 1.0f };
    m.waterLevel = m.floorHeight = m.pos[1] = 0.0f;
    m.terrainSoundAddend = mario_get_terrain_sound_addend(&m);
    EXPECT_EQ(m.terrainSoundAddend, SOUND_TERRAIN_WATER << 16);
    play_sound_and_spawn_particles(&m, SOUND_ACTION_TERRAIN_STEP, 0);
    EXPECT_EQ(sLastMarioSound, SOUND_ACTION_TERRAIN_STEP + (SOUND_TERRAIN_WATER << 16));
    EXPECT_EQ(m.particleFlags & PARTICLE_SHALLOW_WATER_WAVE, PARTICLE_SHALLOW_WATER_WAVE);
    real_play_mario_landing_sound(&m, SOUND_ACTION_TERRAIN_LANDING);
    EXPECT_EQ(sLastMarioSound, SOUND_ACTION_TERRAIN_LANDING + (SOUND_TERRAIN_WATER << 16));
    EXPECT_EQ(m.particleFlags & PARTICLE_SHALLOW_WATER_SPLASH, PARTICLE_SHALLOW_WATER_SPLASH);
    update_mario_submerged_effects(&m);
    EXPECT_EQ(m.particleFlags & PARTICLE_SHALLOW_WATER_SPLASH, PARTICLE_SHALLOW_WATER_SPLASH);
    const u32 actions[] = { ACT_BUTT_SLIDE, ACT_HOLD_BUTT_SLIDE, ACT_CROUCH_SLIDE };
    for (unsigned action = 0; action < sizeof(actions) / sizeof(*actions); action++) {
        begin(actions[action]);
        m.flags |= MARIO_WING_CAP;
        m.input = INPUT_NONE;
        m.intendedMag = 0.0f;
        mario_set_forward_vel(&m, 48.0f);
        struct MarioState water = m;
        water.floor = &gWaterSurfacePseudoFloor;
        for (int frame = 0; frame < 100; frame++) {
            s32 landStopped = real_update_sliding(&m, 4.0f);
            s32 waterStopped = real_update_sliding(&water, 4.0f);
            EXPECT_EQ(waterStopped, landStopped);
            NEAR(water.forwardVel, m.forwardVel);
            NEAR(water.slideVelX, m.slideVelX);
            NEAR(water.slideVelZ, m.slideVelZ);
            if (landStopped) {
                break;
            }
        }
        EXPECT_EQ(m.forwardVel, 0.0f);
        EXPECT_EQ(water.forwardVel, 0.0f);
    }
    begin(ACT_WALKING);
    struct Object tiltingArena = { .behavior = bhvTiltingBowserLavaPlatform, .oAction = 1 };
    struct Surface arenaFloor = { .type = SURFACE_DEFAULT, .object = &tiltingArena };
    m.floor = &arenaFloor;
    EXPECT_EQ(mario_get_floor_class(&m), SURFACE_CLASS_VERY_SLIPPERY);
    tiltingArena.oAction = 0;
    EXPECT_EQ(mario_get_floor_class(&m), SURFACE_CLASS_DEFAULT);
    tiltingArena.oAction = 1;
    tiltingArena.behavior = NULL;
    EXPECT_EQ(mario_get_floor_class(&m), SURFACE_CLASS_DEFAULT);
    m.floor = &gWaterSurfacePseudoFloor;
    m.action = ACT_RIDING_SHELL_GROUND;
    EXPECT_EQ(mario_get_floor_class(&m), SURFACE_CLASS_VERY_SLIPPERY);
    return TRUE;
}

static int vanish_shallow_water_effects(void) {
    const u32 trail = PARTICLE_IDLE_WATER_WAVE | PARTICLE_WAVE_TRAIL;
    const u32 splashes =
        PARTICLE_SHALLOW_WATER_WAVE | PARTICLE_SHALLOW_WATER_SPLASH | PARTICLE_WATER_SPLASH;
    const s32 terrains[] = { TERRAIN_GRASS, TERRAIN_SAND, TERRAIN_SNOW };
    for (int cycle = 0; cycle < 3; cycle++) {
        for (unsigned terrain = 0; terrain < sizeof(terrains) / sizeof(*terrains); terrain++) {
            for (int metal = 0; metal < 2; metal++) {
                begin(ACT_WALKING);
                m.flags |= metal ? MARIO_METAL_CAP : 0;
                sArea.terrainType = terrains[terrain];
                m.waterLevel = -1000;
                u32 dryMaterial = mario_get_terrain_sound_addend(&m);
                m.waterLevel = 40;
                m.terrainSoundAddend = mario_get_terrain_sound_addend(&m);
                EXPECT_EQ(m.terrainSoundAddend, SOUND_TERRAIN_WATER << 16);
                play_sound_and_spawn_particles(&m, SOUND_ACTION_TERRAIN_STEP, 0);
                EXPECT_EQ(m.particleFlags & PARTICLE_SHALLOW_WATER_WAVE, PARTICLE_SHALLOW_WATER_WAVE);
                m.flags |= MARIO_VANISH_CAP;
                m.terrainSoundAddend = mario_get_terrain_sound_addend(&m);
                EXPECT_EQ(m.terrainSoundAddend, dryMaterial);
                m.particleFlags = trail;
                sMarioSoundCount = 0;
                play_sound_and_spawn_particles(
                    &m, metal ? SOUND_ACTION_METAL_STEP : SOUND_ACTION_TERRAIN_STEP, 0);
                EXPECT_EQ(sLastMarioSound,
                          metal ? SOUND_ACTION_METAL_STEP : SOUND_ACTION_TERRAIN_STEP + dryMaterial);
                real_play_mario_landing_sound(&m, SOUND_ACTION_TERRAIN_LANDING);
                EXPECT_EQ(sLastMarioSound, metal ? SOUND_ACTION_METAL_LANDING
                                                 : SOUND_ACTION_TERRAIN_LANDING + dryMaterial);
                EXPECT_EQ(sMarioSoundCount, 2);
                // Filter also catches splashes queued by falls and air/ground transitions.
                m.particleFlags |= splashes;
                update_mario_submerged_effects(&m);
                EXPECT_EQ(m.particleFlags & splashes, 0);
                EXPECT_EQ(m.particleFlags & trail, trail);
                m.flags &= ~MARIO_VANISH_CAP;
                m.terrainSoundAddend = mario_get_terrain_sound_addend(&m);
                EXPECT_EQ(m.terrainSoundAddend, SOUND_TERRAIN_WATER << 16);
                m.particleFlags = 0;
                play_sound_and_spawn_particles(&m, SOUND_ACTION_TERRAIN_STEP, 1);
                EXPECT_EQ(m.particleFlags & PARTICLE_SHALLOW_WATER_SPLASH,
                          PARTICLE_SHALLOW_WATER_SPLASH);
            }
        }
    }
    // Low-pose takeoffs in shallow water cannot emit the submerged splash-fleck burst.
    begin(ACT_CRAWLING);
    m.flags |= MARIO_VANISH_CAP;
    m.waterLevel = 51;
    set_mario_action(&m, ACT_JUMP, 0);
    EXPECT_EQ(m.particleFlags & PARTICLE_VANISH_WATER_RING, 0);
    return 1;
}

static int vanish_water_ring(void) {
    const u32 jumps[] = { ACT_JUMP,     ACT_DOUBLE_JUMP, ACT_TRIPLE_JUMP, ACT_HOLD_JUMP,
                          ACT_BACKFLIP, ACT_LONG_JUMP,   ACT_SIDE_FLIP,   ACT_STEEP_JUMP };
    for (unsigned action = 0; action < sizeof(jumps) / sizeof(*jumps); action++) {
        for (int metal = 0; metal < 2; metal++) {
            begin(ACT_IDLE);
            m.flags |= MARIO_VANISH_CAP | (metal ? MARIO_METAL_CAP : 0);
            m.waterLevel = 141;
            m.input = INPUT_A_PRESSED;
            set_mario_action(&m, jumps[action], 0);
            EXPECT_EQ(m.particleFlags & PARTICLE_VANISH_WATER_RING, PARTICLE_VANISH_WATER_RING);
        }
    }
    for (int cap = 0; cap < 2; cap++) {
        begin(ACT_IDLE);
        m.flags |= cap ? MARIO_METAL_CAP : 0;
        m.waterLevel = 400;
        set_mario_action(&m, ACT_JUMP, 0);
        EXPECT_EQ(m.particleFlags & PARTICLE_VANISH_WATER_RING, 0);
    }
    begin(ACT_IDLE);
    m.flags |= MARIO_VANISH_CAP;
    m.waterLevel = 140;
    set_mario_action(&m, ACT_JUMP, 0);
    EXPECT_EQ(m.particleFlags & PARTICLE_VANISH_WATER_RING, 0);
    begin(ACT_FREEFALL);
    m.flags |= MARIO_VANISH_CAP;
    m.waterLevel = 400;
    set_mario_action(&m, ACT_JUMP, 0);
    EXPECT_EQ(m.particleFlags & PARTICLE_VANISH_WATER_RING, 0);
    begin(ACT_IDLE);
    m.flags |= MARIO_VANISH_CAP;
    m.waterLevel = 400;
    set_mario_action(&m, ACT_WALKING, 0);
    EXPECT_EQ(m.particleFlags & PARTICLE_VANISH_WATER_RING, 0);
    for (int metal = 0; metal < 2; metal++) {
        begin(ACT_FREEFALL);
        m.flags |= MARIO_VANISH_CAP | (metal ? MARIO_METAL_CAP : 0);
        m.waterLevel = 400;
        set_mario_action(&m, ACT_GROUND_POUND, 0);
        EXPECT_EQ(m.particleFlags & PARTICLE_VANISH_WATER_RING, PARTICLE_VANISH_WATER_RING);
    }
    begin(ACT_FREEFALL);
    m.flags |= MARIO_VANISH_CAP;
    m.waterLevel = 140;
    set_mario_action(&m, ACT_GROUND_POUND, 0);
    EXPECT_EQ(m.particleFlags & PARTICLE_VANISH_WATER_RING, 0);
    return 1;
}
static int vanish_ring_particles(void) {
    for (int jump = 0; jump < 2; jump++) {
        begin(jump ? ACT_JUMP : ACT_WATER_DOWN_BURST);
        o = &sObj;
        sObj.parentObj = gMarioObject;
        sObj.oBehParams2ndByte = jump;
        sObj.oPosX = 100;
        sObj.oPosY = 200;
        sObj.oPosZ = -300;
        sMarioObject.oActiveParticleFlags =
            ACTIVE_PARTICLE_WATER_DOWN_MIST | ACTIVE_PARTICLE_VANISH_WATER_RING;
        bhv_water_down_mist_init();
        EXPECT_EQ(sClearedParticleFlags,
                  jump ? ACTIVE_PARTICLE_VANISH_WATER_RING : ACTIVE_PARTICLE_WATER_DOWN_MIST);
        EXPECT_EQ(sMarioObject.oActiveParticleFlags & sClearedParticleFlags, 0);
        EXPECT_EQ(sEffectParticleCount, jump ? 11 : 6);
        for (int i = 0; i < sEffectParticleCount; i++) {
            struct Object *p = &sEffectParticles[i];
            EXPECT_EQ(p->behavior == bhvWhitePuffExplosion, TRUE);
            EXPECT_EQ(p->model, i < 8 ? MODEL_MIST : MODEL_WHITE_PARTICLE_SMALL);
            NEAR(p->oPosY, i < 8 ? 220 : 275);
            NEAR(sqrtf(sqr(p->oPosX - 100) + sqr(p->oPosZ + 300)), i < 8 ? 35 : 12);
            NEAR(p->header.gfx.scale[0], i < 8 ? 0.75f : 1.5f);
            NEAR(p->oForwardVel, i < 8 ? 6.0f : 5.0f);
            NEAR(p->oVelY, i < 8 ? 0.0f : 8.0f);
        }
    }
    begin(ACT_JUMP);
    o = &sObj;
    sObj.parentObj = gMarioObject;
    sObj.oBehParams2ndByte = 1;
    sEffectSpawnLimit = 0;
    bhv_water_down_mist_init();
    EXPECT_EQ(sEffectParticleCount, 0);
    EXPECT_EQ(sClearedParticleFlags, ACTIVE_PARTICLE_VANISH_WATER_RING);
    begin(ACT_JUMP);
    o = &sObj;
    sObj.parentObj = gMarioObject;
    sObj.oBehParams2ndByte = 1;
    sEffectSpawnLimit = 9;
    bhv_water_down_mist_init();
    EXPECT_EQ(sEffectParticleCount, 9);
    return 1;
}
static int vanish_bubble_origin(void) {
    for (int metal = 0; metal < 2; metal++) {
        begin(ACT_WALKING);
        m.flags |= MARIO_VANISH_CAP | (metal ? MARIO_METAL_CAP : 0);
        m.waterLevel = 400;
        sMarioObject.oPosX = 300;
        sMarioObject.oPosY = 40;
        sMarioObject.oPosZ = -150;
        sObj.oPosX = -100;
        sObj.oPosY = 0;
        sObj.oPosZ = 20;
        bhv_spawn_mario_breath_bubble();
        EXPECT_EQ(sBubbleSpawns, 1);
        EXPECT_EQ(sTestBubble.oBehParams2ndByte, TRUE);
        NEAR(sTestBubble.oPosX, 300);
        NEAR(sTestBubble.oPosY, 40);
        NEAR(sTestBubble.oPosZ, -150);
        o = &sTestBubble;
        sTestBubble.oPosY = 401;
        sWaterLevel = 400;
        bhv_small_water_wave_loop();
        EXPECT_EQ(sBubbleSplashes, 0);
        o = &sObj;
        m.flags &= ~MARIO_VANISH_CAP;
        sObj.oPosX = -100;
        bhv_spawn_mario_breath_bubble();
        NEAR(sTestBubble.oPosX, -100);
        EXPECT_EQ(sTestBubble.oBehParams2ndByte, FALSE);
        o = &sTestBubble;
        sTestBubble.oPosY = 401;
        bhv_small_water_wave_loop();
        EXPECT_EQ(sBubbleSplashes, 1);
        m.flags |= MARIO_VANISH_CAP;
        o = &sTestBubble;
        sTestBubble.oPosY = 401;
        bhv_small_water_wave_loop();
        EXPECT_EQ(sBubbleSplashes, 1);
    }
    begin(ACT_WALKING);
    m.flags |= MARIO_VANISH_CAP;
    m.waterLevel = 140;
    sObj.oPosX = -100;
    sMarioObject.oPosX = 300;
    bhv_spawn_mario_breath_bubble();
    NEAR(sTestBubble.oPosX, -100);
    EXPECT_EQ(sTestBubble.oBehParams2ndByte, FALSE);
    return 1;
}
static int cap_surface_voices(void) {
    for (int cap = 0; cap < 3; cap++) {
        for (int cough = 0; cough < 2; cough++) {
            begin(ACT_WATER_IDLE);
            m.waterLevel = 400;
            m.pos[1] = 0;
            m.health = cough ? 0x200 : 0x880;
            for (int frame = 0; frame < 151; frame++) {
                update_surface_breath_sound(&m);
            }
            EXPECT_EQ(sSurfaceVoices, 0);
            m.flags |= cap == 0 ? MARIO_VANISH_CAP
                                : (cap == 1 ? MARIO_METAL_CAP : MARIO_VANISH_CAP | MARIO_METAL_CAP);
            m.pos[1] = 300;
            update_surface_breath_sound(&m);
            EXPECT_EQ(sSurfaceVoices, cap == 1 ? 1 : 0);
            m.flags = 0;
            update_surface_breath_sound(&m);
            EXPECT_EQ(sSurfaceVoices, cap == 1 ? 1 : 0);
        }
    }
    for (int cap = 0; cap < 2; cap++) {
        begin(ACT_WATER_IDLE);
        m.waterLevel = 400;
        m.pos[1] = 0;
        for (int frame = 0; frame < 151; frame++) {
            update_surface_breath_sound(&m);
        }
        m.pos[1] = 300;
        update_surface_breath_sound(&m);
        EXPECT_EQ(sSurfaceVoices, 1);
        m.flags |= cap ? MARIO_METAL_CAP : MARIO_VANISH_CAP;
        update_surface_breath_sound(&m);
        EXPECT_EQ(sStoppedSurfaceVoices, cap ? 0 : 1);
        for (int frame = 0; frame < 70; frame++) {
            update_surface_breath_sound(&m);
        }
        EXPECT_EQ(sSurfaceVoices, cap ? 3 : 1);
        EXPECT_EQ(sStoppedSurfaceVoices, cap ? 0 : 1);
    }
    begin(ACT_WATER_IDLE);
    m.waterLevel = 400;
    m.pos[1] = 0;
    for (int frame = 0; frame < 151; frame++) {
        update_surface_breath_sound(&m);
    }
    m.pos[1] = 300;
    update_surface_breath_sound(&m);
    EXPECT_EQ(sSurfaceVoices, 1);
    return 1;
}
static int grippy_roof_traction(void) {
    for (int ordinary = 0; ordinary < 2; ordinary++) {
        begin(ACT_IDLE);
        sFloor.type = ordinary ? SURFACE_DEFAULT : SURFACE_MARIO_ONLY_GRIPPY;
        sFloor.normal.y = 0.2f;
        EXPECT_EQ(ground_dive_idle_cancels(&m), ordinary);
        EXPECT_EQ(m.action, ordinary ? ACT_FREEFALL : ACT_IDLE);
        begin(ACT_JUMP_LAND);
        sFloor.type = ordinary ? SURFACE_DEFAULT : SURFACE_MARIO_ONLY_GRIPPY;
        sFloor.normal.y = 0.2f;
        EXPECT_EQ(common_landing_cancels(&m, &sJumpLandAction, set_jumping_action), ordinary);
        EXPECT_EQ(m.action, ordinary ? sJumpLandAction.verySteepAction : ACT_JUMP_LAND);
        begin(ACT_CRAWLING);
        m.input = INPUT_Z_DOWN;
        sFloor.type = ordinary ? SURFACE_DEFAULT : SURFACE_MARIO_ONLY_GRIPPY;
        sFloor.normal.y = 0.2f;
        act_crawling(&m);
        EXPECT_EQ(m.action, ordinary ? ACT_FREEFALL : ACT_CRAWLING);
    }
    begin(ACT_WALKING);
    m.faceAngle[1] = 0x4000;
    m.floorYaw = 0;
    for (int metal = 0; metal < 2; metal++) {
        m.flags = metal ? MARIO_METAL_CAP : 0;
        for (int angle = 20; angle <= 80; angle += 10) {
            sFloor.normal.y = cosf(angle * 3.14159265f / 180.0f);
            sFloor.type = angle % 20 ? SURFACE_MARIO_ONLY_GRIPPY : SURFACE_GRIPPY;
            EXPECT_EQ(mario_get_floor_class(&m), SURFACE_CLASS_NOT_SLIPPERY);
            EXPECT_EQ(mario_floor_is_slippery(&m), FALSE);
            EXPECT_EQ(mario_floor_is_slope(&m), FALSE);
            EXPECT_EQ(mario_floor_is_steep(&m), FALSE);
        }
        sFloor.type = SURFACE_DEFAULT;
        EXPECT_EQ(mario_floor_is_slippery(&m), TRUE);
        EXPECT_EQ(mario_floor_is_slope(&m), TRUE);
        EXPECT_EQ(mario_floor_is_steep(&m), TRUE);
    }
    return 1;
}
static int metal_slope_traction(void) {
    begin(ACT_WALKING);
    m.faceAngle[1] = 0x4000;
    m.floorYaw = 0;
    sFloor.normal.y = (COS30 + COS35) * 0.5f;
    EXPECT_EQ(mario_floor_is_steep(&m), TRUE);
    m.flags |= MARIO_METAL_CAP;
    EXPECT_EQ(mario_floor_is_steep(&m), FALSE);
    m.flags &= ~MARIO_METAL_CAP;
    sFloor.normal.y = (COS38 + COS43) * 0.5f;
    EXPECT_EQ(mario_floor_is_slippery(&m), TRUE);
    m.flags |= MARIO_METAL_CAP;
    EXPECT_EQ(mario_floor_is_slippery(&m), FALSE);
    for (int kind = 0; kind < 2; kind++) {
        sFloor.type = kind ? SURFACE_VERY_SLIPPERY : SURFACE_SLIPPERY;
        sFloor.normal.y = kind ? (COS10 + COS15) * 0.5f : (COS20 + COS25) * 0.5f;
        m.flags = 0;
        EXPECT_EQ(mario_floor_is_slippery(&m), TRUE);
        m.flags = MARIO_METAL_CAP;
        EXPECT_EQ(mario_floor_is_slippery(&m), FALSE);
    }
    sFloor.type = SURFACE_DEFAULT;
    sArea.terrainType = TERRAIN_SLIDE;
    sFloor.normal.y = (COS15 + COS20) * 0.5f;
    EXPECT_EQ(mario_floor_is_slippery(&m), TRUE);
    EXPECT_EQ(mario_floor_is_steep(&m), TRUE);
    sArea.terrainType = TERRAIN_GRASS;
    sFloor.normal.y = COS20;
    sFloor.normal.x = sqrtf(1.0f - COS20 * COS20);
    m.flags = 0;
    m.forwardVel = 10;
    apply_slope_accel(&m);
    f32 ordinaryLoss = 10.0f - m.forwardVel;
    m.flags = MARIO_METAL_CAP;
    m.forwardVel = 10;
    apply_slope_accel(&m);
    NEAR(10.0f - m.forwardVel, ordinaryLoss * 0.5f);
    m.action = ACT_BUTT_SLIDE;
    m.forwardVel = 10;
    apply_slope_accel(&m);
    NEAR(10.0f - m.forwardVel, ordinaryLoss);
    m.action = ACT_WALKING;
    m.faceAngle[1] = 0;
    m.flags = 0;
    m.forwardVel = 10;
    apply_slope_accel(&m);
    f32 ordinaryGain = m.forwardVel - 10.0f;
    m.flags = MARIO_METAL_CAP;
    m.forwardVel = 10;
    apply_slope_accel(&m);
    NEAR(m.forwardVel - 10.0f, ordinaryGain);
    return 1;
}
static int vanish_breath(void) {
    for (int gas = 0; gas < 2; gas++) {
        begin(ACT_CRAWLING);
        m.flags |= MARIO_VANISH_CAP;
        m.waterLevel = 80;
        m.breath = 0x300;
        m.input = gas ? INPUT_IN_POISON_GAS : 0;
        for (int frame = 0; frame < 2000; frame++) {
            update_mario_breath(&m);
        }
        EXPECT_EQ(m.health, 0x880);
        EXPECT_EQ(m.breath, gas ? 0x300 : 0x880);
        m.flags &= ~MARIO_VANISH_CAP;
        m.breath = 0x300;
        update_mario_breath(&m);
        EXPECT_EQ(m.breath, 0x2ff);
    }
    return 1;
}

static int lab_cold_water(void) {
    // Surface breathing drains only inside this pool. Crossing out stops drain;
    // returning before the ordinary reset retains progress. Snow courses still
    // use their existing behavior, independent of these world coordinates.
    begin(ACT_WATER_IDLE);
    gCurrLevelNum = LEVEL_PHYSICS_LAB;
    m.waterLevel = 600;
    m.pos[0] = 4400;
    m.pos[1] = 520;
    m.pos[2] = 12300;
    for (int i = 0; i < 127; i++) {
        update_mario_health(&m);
    }
    EXPECT_EQ(m.health, 0x880);
    m.pos[0] = 3200;
    update_mario_health(&m);
    EXPECT_EQ(m.health, 0x880);
    m.pos[0] = 4400;
    update_mario_health(&m);
    EXPECT_EQ(m.health, 0x780);
    for (int area = 1; area <= 2; area++) {
        for (int level = 0; level <= 1; level++) {
            begin(ACT_WATER_IDLE);
            gCurrLevelNum = level ? LEVEL_PHYSICS_LAB : LEVEL_BOB;
            gCurrAreaIndex = area;
            m.waterLevel = 600;
            m.pos[0] = 4400;
            m.pos[1] = 520;
            m.pos[2] = 12300;
            for (int i = 0; i < 128; i++) {
                update_mario_health(&m);
            }
            EXPECT_EQ(m.health, (level && area == 1) ? 0x780 : 0x880);
        }
    }
    begin(ACT_WATER_IDLE);
    gCurrLevelNum = LEVEL_PHYSICS_LAB;
    m.flags |= MARIO_METAL_CAP;
    m.waterLevel = 600;
    m.pos[0] = 4400;
    m.pos[1] = 520;
    m.pos[2] = 12300;
    for (int i = 0; i < 127; i++) {
        update_mario_health(&m);
    }
    EXPECT_EQ(m.health, 0x880);
    update_mario_health(&m);
    EXPECT_EQ(m.health, 0x780);
    return 1;
}
static int vanish_contacts(void) {
    begin(ACT_GROUND_TWIRL);
    m.flags |= MARIO_VANISH_CAP;
    EXPECT_EQ(mario_vanish_ignores_hostile_contact(&m, &sObj, INTERACT_BULLY), TRUE);
    EXPECT_EQ(mario_vanish_ignores_hostile_contact(&m, &sObj, INTERACT_HIT_FROM_BELOW), TRUE);
    EXPECT_EQ(mario_vanish_ignores_hostile_contact(&m, &sObj, INTERACT_DAMAGE), TRUE);
    EXPECT_EQ(mario_vanish_ignores_hostile_contact(&m, &sObj, INTERACT_FLAME), FALSE);
    EXPECT_EQ(mario_vanish_ignores_hostile_contact(&m, &sObj, INTERACT_COIN), FALSE);
    EXPECT_EQ(mario_vanish_ignores_hostile_contact(&m, &sObj, INTERACT_STAR_OR_KEY), FALSE);
    EXPECT_EQ(mario_vanish_ignores_hostile_contact(&m, &sObj, INTERACT_DOOR), FALSE);
    sObj.oInteractionSubtype = INT_SUBTYPE_EATS_MARIO;
    EXPECT_EQ(mario_vanish_ignores_hostile_contact(&m, &sObj, INTERACT_CLAM_OR_BUBBA), FALSE);
    sHostileDamage = sHostileRebounds = 0;
    EXPECT_EQ(mario_handle_defensive_contact(&m, &sObj, INTERACT_HIT_FROM_BELOW, TRUE),
              DEFENSIVE_CONTACT_NORMAL);
    EXPECT_EQ(sHostileDamage, 0);
    EXPECT_EQ(sHostileRebounds, 0);
    m.action = ACT_IDLE;
    sObj.oGroundTwirlStunTimer = 1;
    EXPECT_EQ(mario_handle_defensive_contact(&m, &sObj, INTERACT_HIT_FROM_BELOW, FALSE),
              DEFENSIVE_CONTACT_NORMAL);
    m.action = ACT_GROUND_TWIRL;
    m.flags &= ~MARIO_VANISH_CAP;
    sObj.oGroundTwirlStunTimer = 0;
    EXPECT_EQ(mario_handle_defensive_contact(&m, &sObj, INTERACT_HIT_FROM_BELOW, TRUE),
              DEFENSIVE_CONTACT_STOP);
    EXPECT_EQ(sHostileRebounds, 1);
    return 1;
}
static int giant_goomba_twirl_contact(void) {
    const BehaviorScript *behaviors[] = { bhvGoomba, bhvFloomba };
    for (unsigned enemy = 0; enemy < 2; enemy++) {
        for (int cycle = 0; cycle < 3; cycle++) {
            begin(ACT_GROUND_TWIRL);
            sHostileDamage = sHostileRebounds = 0;
            m.pos[0] = 100;
            m.pos[2] = -100;
            m.marioObj->hitboxRadius = 37;
            struct Object giant = { .behavior = behaviors[enemy],
                                    .oGoombaSize = GOOMBA_SIZE_HUGE,
                                    .hitboxRadius = 252,
                                    .hurtboxRadius = 147,
                                    .oPosX = 300,
                                    .oPosZ = -100 };
            // Already in the outer overlap (289 units), but outside body contact (184).
            EXPECT_EQ(mario_handle_defensive_contact(&m, &giant, INTERACT_BOUNCE_TOP, TRUE),
                      DEFENSIVE_CONTACT_SKIP);
            EXPECT_EQ(sHostileRebounds, 0);
            EXPECT_EQ(sHostileDamage, 0);
            giant.oPosX = 284;
            EXPECT_EQ(mario_handle_defensive_contact(&m, &giant, INTERACT_BOUNCE_TOP, TRUE),
                      DEFENSIVE_CONTACT_SKIP);
            giant.oPosX = 283.5f;
            EXPECT_EQ(mario_handle_defensive_contact(&m, &giant, INTERACT_BOUNCE_TOP, TRUE),
                      DEFENSIVE_CONTACT_STOP);
            EXPECT_EQ(sHostileRebounds, 1);
            EXPECT_EQ(sHostileDamage, 1);
            giant.oPosX = 240;
            giant.oPosZ = 40; // Diagonal distance must also be checked.
            EXPECT_EQ(mario_handle_defensive_contact(&m, &giant, INTERACT_BOUNCE_TOP, TRUE),
                      DEFENSIVE_CONTACT_SKIP);
            EXPECT_EQ(sHostileRebounds, 1);
            giant.oPosX = 100;
            giant.oPosZ = 83.5f;
            EXPECT_EQ(mario_handle_defensive_contact(&m, &giant, INTERACT_BOUNCE_TOP, TRUE),
                      DEFENSIVE_CONTACT_STOP);
            EXPECT_EQ(sHostileRebounds, 2);
            giant.oPosZ = 100;
            EXPECT_EQ(mario_handle_defensive_contact(&m, &giant, INTERACT_BOUNCE_TOP, FALSE),
                      DEFENSIVE_CONTACT_NORMAL);
            m.action = ACT_IDLE;
            EXPECT_EQ(mario_handle_defensive_contact(&m, &giant, INTERACT_BOUNCE_TOP, FALSE),
                      DEFENSIVE_CONTACT_NORMAL);
            m.action = ACT_GROUND_TWIRL;
            m.flags |= MARIO_VANISH_CAP;
            EXPECT_EQ(mario_handle_defensive_contact(&m, &giant, INTERACT_BOUNCE_TOP, TRUE),
                      DEFENSIVE_CONTACT_NORMAL);
            m.flags &= ~MARIO_VANISH_CAP;
            m.flags |= MARIO_METAL_CAP;
            EXPECT_EQ(mario_handle_defensive_contact(&m, &giant, INTERACT_BOUNCE_TOP, TRUE),
                      DEFENSIVE_CONTACT_SKIP);
            m.flags &= ~MARIO_METAL_CAP;
            // Missing hurtboxes retain native contact; smaller sizes also use body reach.
            giant.hurtboxRadius = 0;
            EXPECT_EQ(mario_handle_defensive_contact(&m, &giant, INTERACT_BOUNCE_TOP, TRUE),
                      DEFENSIVE_CONTACT_STOP);
            giant.hurtboxRadius = 147;
            giant.oGoombaSize = GOOMBA_SIZE_REGULAR;
            EXPECT_EQ(mario_ground_twirl_outside_enemy_body(&m, &giant), TRUE);
            giant.oGoombaSize = GOOMBA_SIZE_TINY;
            EXPECT_EQ(mario_ground_twirl_outside_enemy_body(&m, &giant), TRUE);
            giant.oGoombaSize = GOOMBA_SIZE_HUGE;
            giant.behavior = bhvChuckya;
            giant.oPosZ = -100;
            EXPECT_EQ(mario_handle_defensive_contact(&m, &giant, INTERACT_BOUNCE_TOP, TRUE),
                      DEFENSIVE_CONTACT_STOP);
        }
    }
    return 1;
}
static int piranha_twirl_contact(void) {
    for (int cycle = 0; cycle < 3; cycle++) {
        begin(ACT_GROUND_TWIRL);
        m.marioObj->hitboxRadius = 37;
        struct Object plant = {
            .behavior = bhvPiranhaPlant, .hitboxRadius = 250, .hurtboxRadius = 150, .oPosX = 200
        };
        EXPECT_EQ(mario_handle_defensive_contact(&m, &plant, INTERACT_BOUNCE_TOP, TRUE),
                  DEFENSIVE_CONTACT_SKIP);
        plant.oPosX = 140;
        EXPECT_EQ(mario_handle_defensive_contact(&m, &plant, INTERACT_BOUNCE_TOP, TRUE),
                  DEFENSIVE_CONTACT_SKIP);
        plant.oPosX = 136.5f;
        EXPECT_EQ(mario_handle_defensive_contact(&m, &plant, INTERACT_BOUNCE_TOP, TRUE),
                  DEFENSIVE_CONTACT_STOP);
        plant.hitboxRadius = plant.hurtboxRadius = 150;
        plant.oPosX = 140;
        EXPECT_EQ(mario_handle_defensive_contact(&m, &plant, INTERACT_BOUNCE_TOP, TRUE),
                  DEFENSIVE_CONTACT_SKIP);
    }
    return 1;
}
static int mobile_enemy_twirl_contact(void) {
    for (int cycle = 0; cycle < 3; cycle++) {
        begin(ACT_GROUND_TWIRL);
        m.marioObj->hitboxRadius = 37;
        struct Object enemy = {
            .behavior = bhvChuckya, .hitboxRadius = 160, .hurtboxRadius = 80, .oPosX = 140
        };
        EXPECT_EQ(mario_handle_defensive_contact(&m, &enemy, INTERACT_BOUNCE_TOP, TRUE),
                  DEFENSIVE_CONTACT_SKIP);
        enemy.oPosX = 116.5f;
        EXPECT_EQ(mario_handle_defensive_contact(&m, &enemy, INTERACT_BOUNCE_TOP, TRUE),
                  DEFENSIVE_CONTACT_STOP);
        m.action = ACT_IDLE;
        enemy.oPosX = 140;
        EXPECT_EQ(mario_handle_defensive_contact(&m, &enemy, INTERACT_BOUNCE_TOP, FALSE),
                  DEFENSIVE_CONTACT_NORMAL);
    }
    return 1;
}
static int jump_press_buffer(void) {
    for (int jump = 0; jump < 2; jump++) {
        begin(jump ? ACT_DOUBLE_JUMP : ACT_JUMP);
        m.input = INPUT_A_PRESSED;
        EXPECT_EQ(jump ? act_double_jump(&m) : act_jump(&m), FALSE);
        EXPECT_EQ(m.actionState, 1);
        m.input = 0;
        EXPECT_EQ(jump ? act_double_jump(&m) : act_jump(&m), FALSE);
        EXPECT_EQ(m.actionState, 1);
        m.input = INPUT_A_PRESSED;
        EXPECT_EQ(jump ? act_double_jump(&m) : act_jump(&m), FALSE);
        EXPECT_EQ(m.actionState, 2);
    }
    return 1;
}
static int jump_kick_grounded_chain(void) {
    for (int cycle = 0; cycle < 3; cycle++) {
        begin(ACT_JUMP_KICK);
        m.actionState = ACT_STATE_JUMP_KICK_KICKING;
        m.marioObj->header.gfx.animInfo.animFrame = 1;
        m.pos[1] = 1;
        m.vel[1] = -20;
        sDiveTrajectory = 1;
        EXPECT_EQ(act_jump_kick(&m), FALSE);
        NEAR(m.pos[1], m.floorHeight);
        EXPECT_EQ(m.action, ACT_FREEFALL_LAND);
        EXPECT_EQ(m.marioObj->header.gfx.animInfo.animID, MARIO_ANIM_GENERAL_LAND);
        m.input = INPUT_A_PRESSED | INPUT_A_DOWN;
        EXPECT_EQ(common_landing_cancels(&m, &sFreefallLandAction, set_jumping_action), TRUE);
        EXPECT_EQ(m.action, ACT_DOUBLE_JUMP);
        EXPECT_EQ(m.vel[1] > 0, TRUE);
    }
    begin(ACT_JUMP_KICK);
    m.actionState = ACT_STATE_JUMP_KICK_KICKING;
    m.marioObj->header.gfx.animInfo.animFrame = 1;
    m.pos[1] = 100;
    m.vel[1] = -20;
    sDiveTrajectory = 1;
    m.input = INPUT_A_PRESSED;
    EXPECT_EQ(act_jump_kick(&m), FALSE);
    EXPECT_EQ(m.action, ACT_JUMP_KICK);
    EXPECT_EQ(m.marioObj->header.gfx.animInfo.animID != MARIO_ANIM_GENERAL_LAND, TRUE);
    return 1;
}
static int rollout_followups(void) {
    for (int age = 0; age <= 6; age++) {
        begin(ACT_FORWARD_ROLLOUT);
        m.actionArg = ACT_ARG_FORWARD_ROLLOUT_FROM_SLIDE_KICK;
        sController.stickMag = 64;
        sAirStepResult = AIR_STEP_NONE;
        act_forward_rollout(&m);
        m.zBPressed = TRUE;
        if (age == 0) {
            sAirStepResult = AIR_STEP_LANDED;
        }
        act_forward_rollout(&m);
        m.zBPressed = FALSE;
        for (int frame = 1; frame < age; frame++) {
            act_forward_rollout(&m);
        }
        if (age > 0) {
            sAirStepResult = AIR_STEP_LANDED;
            act_forward_rollout(&m);
        }
        EXPECT_EQ(m.action == ACT_SLIDE_KICK, age < 6);
    }

    for (int cycle = 0; cycle < 3; cycle++) {
        begin(ACT_FORWARD_ROLLOUT);
        m.actionArg = ACT_ARG_FORWARD_ROLLOUT_FROM_SLIDE_KICK;
        m.zBPressed = TRUE;
        sController.stickMag = 64;
        sAirStepResult = AIR_STEP_NONE;
        act_forward_rollout(&m);
        EXPECT_EQ(m.actionTimer, 0);
        act_forward_rollout(&m);
        EXPECT_EQ(m.actionTimer, 6);
        m.zBPressed = FALSE;
        act_forward_rollout(&m);
        EXPECT_EQ(m.actionTimer, 5);
        sAirStepResult = AIR_STEP_LANDED;
        act_forward_rollout(&m);
        EXPECT_EQ(m.action, ACT_SLIDE_KICK);
        EXPECT_EQ(m.actionArg, 0);

        begin(ACT_FORWARD_ROLLOUT);
        m.actionArg = ACT_ARG_FORWARD_ROLLOUT_FROM_SLIDE_KICK;
        sController.stickMag = 64;
        sAirStepResult = AIR_STEP_NONE;
        act_forward_rollout(&m);
        m.zBPressed = TRUE;
        act_forward_rollout(&m);
        m.zBPressed = FALSE;
        for (int frame = 0; frame < 6; frame++) {
            act_forward_rollout(&m);
        }
        sAirStepResult = AIR_STEP_LANDED;
        act_forward_rollout(&m);
        EXPECT_EQ(m.actionArg, 0);

        begin(ACT_FORWARD_ROLLOUT);
        sController.stickMag = 64;
        sAirStepResult = AIR_STEP_NONE;
        act_forward_rollout(&m);
        m.zBPressed = TRUE;
        act_forward_rollout(&m);
        sAirStepResult = AIR_STEP_LANDED;
        act_forward_rollout(&m);
        EXPECT_EQ(m.actionArg, 0);

        for (int origin = 0; origin < 2; origin++) {
            for (int contact = 1; contact <= 4; contact++) {
                begin(ACT_FORWARD_ROLLOUT);
                m.actionArg = origin ? ACT_ARG_FORWARD_ROLLOUT_FROM_SLIDE_KICK : 0;
                for (int frame = 1; frame < contact; frame++) {
                    sAirStepResult = AIR_STEP_NONE;
                    act_forward_rollout(&m);
                }
                m.forwardVel = 20;
                m.wall = &sFloor;
                m.wallYaw = 0x8000;
                m.input = INPUT_A_PRESSED | INPUT_NONZERO_ANALOG;
                sController.stickMag = 64;
                m.wallKickForwardStickFrames =
                    1; // Qualifying stick was sampled before this wall contact.
                m.vel[1] = 30;
                sAirStepResult = AIR_STEP_HIT_WALL;
                act_forward_rollout(&m);
                EXPECT_EQ(sWallKickWindows, contact == 4);
                EXPECT_EQ(sBonkSounds, contact <= 3);
                if (contact <= 3) {
                    EXPECT_EQ(m.action, ACT_BACKWARD_AIR_KB);
                    EXPECT_EQ(m.prevAction, ACT_FORWARD_ROLLOUT);
                    EXPECT_EQ(m.forwardVel, -16);
                    EXPECT_EQ(m.vel[1], 0);
                    EXPECT_EQ(m.faceAngle[1], 0);
                    EXPECT_EQ(!!(m.particleFlags & PARTICLE_VERTICAL_STAR), TRUE);
                } else {
                    EXPECT_EQ(m.action, ACT_FORWARD_ROLLOUT);
                }
            }
        }
        const int bonkSpeeds[] = { 17, 37, 38, 100 };
        for (int metal = 0; metal < 2; metal++) {
            for (unsigned i = 0; i < sizeof(bonkSpeeds) / sizeof(*bonkSpeeds); i++) {
                int speed = bonkSpeeds[i];
                begin(ACT_FORWARD_ROLLOUT);
                m.wall = &sFloor;
                m.wallYaw = 0x8000;
                m.forwardVel = speed;
                m.vel[1] = 30;
                m.flags |= metal ? MARIO_METAL_CAP : 0;
                sAirStepResult = AIR_STEP_HIT_WALL;
                act_forward_rollout(&m);
                EXPECT_EQ(sWallKickWindows, 0);
                EXPECT_EQ(sBonkSounds, 1);
                EXPECT_EQ(sMetalBonkSounds, metal);
                EXPECT_EQ(m.action, ACT_BACKWARD_AIR_KB);
                NEAR(m.forwardVel, -MAX(speed * 0.5f, 16.0f));
                EXPECT_EQ(!!(m.particleFlags & PARTICLE_VERTICAL_STAR), TRUE);
                EXPECT_EQ(m.vel[1], 0);
                EXPECT_EQ(m.faceAngle[1], 0);
            }
        }
        begin(ACT_FORWARD_ROLLOUT);
        m.forwardVel = 16;
        m.wall = &sFloor;
        for (int frame = 0; frame < 3; frame++) {
            sAirStepResult = AIR_STEP_NONE;
            act_forward_rollout(&m);
        }
        sAirStepResult = AIR_STEP_HIT_WALL;
        act_forward_rollout(&m);
        EXPECT_EQ(sWallKickWindows, 0);
        EXPECT_EQ(sBonkSounds, 0);
        EXPECT_EQ(m.action, ACT_FORWARD_ROLLOUT);
        EXPECT_EQ(m.forwardVel, 0);
        begin(ACT_FORWARD_ROLLOUT);
        m.forwardVel = 40;
        m.wall = NULL;
        sAirStepResult = AIR_STEP_HIT_WALL;
        act_forward_rollout(&m);
        EXPECT_EQ(sBonkSounds, 0);
        EXPECT_EQ(m.action, ACT_FORWARD_ROLLOUT);
        EXPECT_EQ(m.forwardVel, 0);
    }
    return 1;
}

static int dive_startup_wall_grace(void) {
    const f32 speeds[] = { 16.0f, 100.0f };
    for (unsigned speed = 0; speed < sizeof(speeds) / sizeof(*speeds); speed++) {
        for (int contact = 0; contact < 5; contact++) {
            begin(ACT_DIVE);
            struct Surface wall = { 0 };
            wall.normal.z = -1.0f;
            wall.object = &sObj;
            sObj.oInteractType = INTERACT_BREAKABLE;
            m.wall = &wall;
            m.wallYaw = 0x8000;
            m.heldObj = &sObj;
            sAirStepResult = AIR_STEP_NONE;
            for (int update = 0; update < contact; update++) {
                act_dive(&m);
                EXPECT_EQ(sDiveGrabAge, update == 0 ? 1 : MIN(update, 3));
            }
            mario_set_forward_vel(&m, speeds[speed]);
            m.vel[0] = 10.0f;
            m.vel[1] = 20.0f;
            m.faceAngle[0] = -0x1000;
            sAirStepResult = AIR_STEP_HIT_WALL;
            act_dive(&m);
            EXPECT_EQ(sObj.oInteractStatus,
                      INT_STATUS_INTERACTED | INT_STATUS_WAS_ATTACKED | ATTACK_FAST_ATTACK);
            if (contact < 3) {
                EXPECT_EQ(m.action, ACT_DIVE);
                EXPECT_EQ(m.heldObj == &sObj, TRUE);
                EXPECT_EQ(sBonkSounds, 0);
                EXPECT_EQ(m.particleFlags & PARTICLE_VERTICAL_STAR, 0);
                EXPECT_EQ(m.faceAngle[0], -0x1000);
                NEAR(m.vel[0], 10.0f);
                NEAR(m.vel[1], 20.0f);
                NEAR(m.vel[2], 0.0f);
            } else {
                EXPECT_EQ(m.action, ACT_BACKWARD_AIR_KB);
                EXPECT_EQ(m.heldObj == NULL, TRUE);
                EXPECT_EQ(sBonkSounds, 1);
                EXPECT_EQ(m.particleFlags & PARTICLE_VERTICAL_STAR, PARTICLE_VERTICAL_STAR);
                NEAR(m.vel[1], 0.0f);
            }
        }
    }
    for (int update = 0; update < 3; update++) {
        begin(ACT_DIVE);
        m.actionState = update;
        sAirStepResult = AIR_STEP_HIT_LAVA_WALL;
        act_dive(&m);
        EXPECT_EQ(sLavaWallBoosts, 1);
    }
    // Glancing contact removes inward travel without erasing the wall tangent.
    begin(ACT_DIVE);
    struct Surface glancingWall = { 0 };
    glancingWall.normal.x = -0.6f;
    glancingWall.normal.z = -0.8f;
    m.wall = &glancingWall;
    m.vel[0] = 100.0f;
    m.vel[2] = 0.0f;
    sAirStepResult = AIR_STEP_HIT_WALL;
    act_dive(&m);
    NEAR(m.vel[0], 64.0f);
    NEAR(m.vel[2], -48.0f);
    EXPECT_EQ(m.action, ACT_DIVE);
    // Continued pressure must not restart the grace period.
    begin(ACT_DIVE);
    struct Surface wall = { 0 };
    wall.normal.z = -1.0f;
    m.wall = &wall;
    m.wallYaw = 0x8000;
    sAirStepResult = AIR_STEP_HIT_WALL;
    for (int update = 0; update < 4; update++) {
        mario_set_forward_vel(&m, 100.0f);
        act_dive(&m);
        EXPECT_EQ(m.action, update < 3 ? ACT_DIVE : ACT_BACKWARD_AIR_KB);
    }
    // A fresh dive gets its own window after an earlier dive expired.
    set_mario_action(&m, ACT_DIVE, 0);
    mario_set_forward_vel(&m, 100.0f);
    act_dive(&m);
    EXPECT_EQ(m.action, ACT_DIVE);
    return 1;
}

static int wall_bonk_momentum(void) {
    const f32 speeds[] = { 0, 8, 16, 31.9f, 32, 32.1f, 38, 100 };
    const s16 angles[] = { 0, 0x1000, -0x1000 };
    for (int cycle = 0; cycle < 3; cycle++) {
        for (unsigned speed = 0; speed < sizeof(speeds) / sizeof(*speeds); speed++) {
            for (unsigned angle = 0; angle < sizeof(angles) / sizeof(*angles); angle++) {
                for (int backward = 0; backward < 2; backward++) {
                    begin(ACT_DIVE);
                    m.actionState = 3; // Exercise the ordinary bonk after startup.
                    m.wall = &sFloor;
                    m.wallYaw = 0x8000;
                    m.faceAngle[1] = angles[angle];
                    m.heldObj = &sObj;
                    f32 incoming = backward ? -speeds[speed] : speeds[speed];
                    mario_set_forward_vel(&m, incoming);
                    sAirStepResult = AIR_STEP_HIT_WALL;
                    act_dive(&m);
                    f32 recoil = MAX(speeds[speed] * 0.5f, 16.0f);
                    EXPECT_EQ(m.action, ACT_BACKWARD_AIR_KB);
                    EXPECT_EQ(m.wallBonkMomentum, TRUE);
                    EXPECT_EQ(m.heldObj == NULL, TRUE);
                    NEAR(m.forwardVel, incoming < 0.0f ? recoil : -recoil);
                    // A Z-facing wall reverses normal travel and preserves its tangent.
                    NEAR(m.vel[0],
                         (backward && incoming != 0.0f ? -1 : 1) * recoil * sins(angles[angle]));
                    NEAR(m.vel[2],
                         (backward && incoming != 0.0f ? 1 : -1) * recoil * coss(angles[angle]));
                    sAirStepResult = AIR_STEP_NONE;
                    common_air_knockback_step(&m, ACT_BACKWARD_GROUND_KB, ACT_HARD_BACKWARD_GROUND_KB,
                                              MARIO_ANIM_BACKWARD_AIR_KB, -16.0f);
                    NEAR(absf(m.forwardVel), recoil);
                }
            }
        }
    }
    for (unsigned speed = 0; speed < sizeof(speeds) / sizeof(*speeds); speed++) {
        begin(ACT_STOMACH_SLIDE);
        m.wall = &sFloor;
        mario_set_forward_vel(&m, speeds[speed]);
        real_slide_bonk(&m, ACT_GROUND_BONK, ACT_STOMACH_SLIDE_STOP);
        if (speeds[speed] > 16.0f) {
            EXPECT_EQ(m.action, ACT_GROUND_BONK);
            NEAR(m.forwardVel, -speeds[speed] * 0.5f);
            EXPECT_EQ(m.wallBonkMomentum, TRUE);
        } else {
            EXPECT_EQ(m.action, ACT_STOMACH_SLIDE_STOP);
            NEAR(m.forwardVel, 0.0f);
            EXPECT_EQ(m.wallBonkMomentum, FALSE);
        }
    }
    // Soft recovery retains an eight-unit floor; neither minimum grows on repeat impact.
    for (int minimum = 8; minimum <= 16; minimum += 8) {
        begin(ACT_SOFT_BONK);
        mario_set_wall_bonk_momentum(&m, -4, minimum);
        for (int impact = 0; impact < 4; impact++) {
            sAirStepResult = AIR_STEP_HIT_WALL;
            common_air_knockback_step(&m, ACT_FREEFALL_LAND, ACT_HARD_BACKWARD_GROUND_KB,
                                      MARIO_ANIM_BACKWARD_AIR_KB, m.forwardVel);
            NEAR(absf(m.forwardVel), minimum);
        }
    }
    return 1;
}

static int bonk_recovery_boundary(void) {
    const struct {
        u32 action;
        s32 animation, length, checkFrame, wallMomentum, twirl;
    } cases[] = {
        { ACT_GROUND_BONK, MARIO_ANIM_GROUND_BONK, GROUND_BONK_ANIM_LENGTH, 32, FALSE, FALSE },
        { ACT_GROUND_BONK, MARIO_ANIM_GROUND_BONK, GROUND_BONK_ANIM_LENGTH, 32, TRUE, FALSE },
        { ACT_BACKWARD_GROUND_KB, MARIO_ANIM_BACKWARD_KB, BACKWARD_BONK_ANIM_LENGTH, 22, TRUE, FALSE },
        { ACT_FORWARD_GROUND_KB, MARIO_ANIM_FORWARD_KB, FORWARD_BONK_ANIM_LENGTH, 20, TRUE, FALSE },
        { ACT_BACKWARD_GROUND_KB, MARIO_ANIM_BACKWARD_KB, BACKWARD_BONK_ANIM_LENGTH, 22, FALSE, FALSE },
        { ACT_FORWARD_GROUND_KB, MARIO_ANIM_FORWARD_KB, FORWARD_BONK_ANIM_LENGTH, 20, FALSE, FALSE },
        { ACT_SOFT_BACKWARD_GROUND_KB, MARIO_ANIM_SOFT_BACK_KB, 30, 100, FALSE, TRUE },
        { ACT_SOFT_FORWARD_GROUND_KB, MARIO_ANIM_SOFT_FRONT_KB, 30, 100, FALSE, TRUE },
    };
    for (int cycle = 0; cycle < 3; cycle++) {
        for (unsigned i = 0; i < sizeof(cases) / sizeof(*cases); i++) {
            for (int alive = 0; alive < 2; alive++) {
                begin(cases[i].action);
                sRealAnimationEnds = sRealBonkRecovery = TRUE;
                struct Animation anim = { .loopEnd = cases[i].length };
                m.marioObj->header.gfx.animInfo.curAnim = &anim;
                m.marioObj->header.gfx.animInfo.animID = cases[i].animation;
                m.wallBonkMomentum = cases[i].wallMomentum;
                m.actionState = cases[i].twirl ? GROUND_TWIRL_IMMUNE_REBOUND_STATE : 0;
                m.health = alive ? 0x880 : 0xFF;
                m.input = cycle == 0 ? 0 : INPUT_A_DOWN | INPUT_B_PRESSED | INPUT_Z_PRESSED;
                mario_set_forward_vel(&m, -40);
                s32 early = cases[i].twirl
                            || (alive && (cases[i].wallMomentum || cases[i].action == ACT_GROUND_BONK));
                s32 recoveryFrame = cases[i].length - (early ? 2 : 1);
                s32 damage = !cases[i].wallMomentum && cases[i].action != ACT_GROUND_BONK;
                for (int frame = cases[i].length - 3; frame <= recoveryFrame; frame++) {
                    m.marioObj->header.gfx.animInfo.animFrame = frame;
                    common_ground_knockback_action(&m, cases[i].animation, cases[i].checkFrame, FALSE,
                                                   damage);
                    u32 expectedAction = cases[i].action;
                    if (frame == recoveryFrame) {
                        expectedAction = alive ? ACT_IDLE : ACT_STANDING_DEATH;
                    }
                    EXPECT_EQ(m.action, expectedAction);
                }
                EXPECT_EQ(m.wallBonkMomentum, FALSE);
                EXPECT_EQ(m.invincTimer, alive && damage ? 30 : 0);
            }
        }
    }
    // Losing support on the newly available frame still enters airborne recovery.
    begin(ACT_BACKWARD_GROUND_KB);
    sRealAnimationEnds = sRealBonkRecovery = TRUE;
    struct Animation anim = { .loopEnd = BACKWARD_BONK_ANIM_LENGTH };
    m.marioObj->header.gfx.animInfo.curAnim = &anim;
    m.marioObj->header.gfx.animInfo.animID = MARIO_ANIM_BACKWARD_KB;
    m.marioObj->header.gfx.animInfo.animFrame = anim.loopEnd - 2;
    m.wallBonkMomentum = TRUE;
    mario_set_forward_vel(&m, -40);
    sStep = GROUND_STEP_LEFT_GROUND;
    common_ground_knockback_action(&m, MARIO_ANIM_BACKWARD_KB, 22, FALSE, 0);
    EXPECT_EQ(m.action, ACT_BACKWARD_AIR_KB);
    EXPECT_EQ(m.wallBonkMomentum, TRUE);
    NEAR(m.forwardVel, -36);
    return TRUE;
}

static int wall_bonk_recovery(void) {
    for (int cycle = 0; cycle < 3; cycle++) {
        begin(ACT_BACKWARD_AIR_KB);
        mario_set_wall_bonk_momentum(&m, -100, 16);
        sRealBonkRecovery = TRUE;
        sAirStepResult = AIR_STEP_LANDED;
        common_air_knockback_step(&m, ACT_BACKWARD_GROUND_KB, ACT_HARD_BACKWARD_GROUND_KB,
                                  MARIO_ANIM_BACKWARD_AIR_KB, -16);
        EXPECT_EQ(m.action, ACT_BACKWARD_GROUND_KB);
        EXPECT_EQ(m.wallBonkMomentum, TRUE);
        NEAR(m.forwardVel, -50);
        common_ground_knockback_action(&m, MARIO_ANIM_BACKWARD_KB, 22, FALSE, 0);
        NEAR(m.forwardVel, -45);
        // Late recovery keeps decaying instead of snapping to 0.1.
        m.marioObj->header.gfx.animInfo.animFrame = 23;
        common_ground_knockback_action(&m, MARIO_ANIM_BACKWARD_KB, 22, FALSE, 0);
        NEAR(m.forwardVel, -40.5f);
        sStep = GROUND_STEP_LEFT_GROUND;
        common_ground_knockback_action(&m, MARIO_ANIM_BACKWARD_KB, 22, FALSE, 0);
        EXPECT_EQ(m.action, ACT_BACKWARD_AIR_KB);
        EXPECT_EQ(m.wallBonkMomentum, TRUE);
        NEAR(m.forwardVel, -36.45f);
        sAirStepResult = AIR_STEP_NONE;
        common_air_knockback_step(&m, ACT_BACKWARD_GROUND_KB, ACT_HARD_BACKWARD_GROUND_KB,
                                  MARIO_ANIM_BACKWARD_AIR_KB, -16);
        NEAR(m.forwardVel, -36.45f);
        sAirStepResult = AIR_STEP_LANDED;
        common_air_knockback_step(&m, ACT_BACKWARD_GROUND_KB, ACT_HARD_BACKWARD_GROUND_KB,
                                  MARIO_ANIM_BACKWARD_AIR_KB, -16);
        sStep = GROUND_STEP_NONE;
        sAnimEnded = TRUE;
        common_ground_knockback_action(&m, MARIO_ANIM_BACKWARD_KB, 22, FALSE, 0);
        EXPECT_EQ(m.action, ACT_IDLE);
        EXPECT_EQ(m.wallBonkMomentum, FALSE);
    }
    const u32 interruptions[] = { ACT_BACKWARD_AIR_KB, ACT_HARD_BACKWARD_AIR_KB, ACT_FREEFALL,
                                  ACT_WATER_PLUNGE, ACT_IDLE };
    for (unsigned i = 0; i < sizeof(interruptions) / sizeof(*interruptions); i++) {
        begin(ACT_BACKWARD_AIR_KB);
        mario_set_wall_bonk_momentum(&m, -100, 16);
        set_mario_action(&m, interruptions[i], 1);
        EXPECT_EQ(m.wallBonkMomentum, FALSE);
    }
    begin(ACT_BACKWARD_AIR_KB);
    mario_set_wall_bonk_momentum(&m, -100, 16);
    sBonkFallDamage = TRUE;
    sAirStepResult = AIR_STEP_LANDED;
    common_air_knockback_step(&m, ACT_BACKWARD_GROUND_KB, ACT_HARD_BACKWARD_GROUND_KB,
                              MARIO_ANIM_BACKWARD_AIR_KB, -16);
    EXPECT_EQ(m.action, ACT_HARD_BACKWARD_GROUND_KB);
    EXPECT_EQ(m.wallBonkMomentum, FALSE);
    // Soft bonks finish ownership when landing in an ordinary landing action.
    begin(ACT_SOFT_BONK);
    mario_set_wall_bonk_momentum(&m, -38, 8);
    sAirStepResult = AIR_STEP_LANDED;
    common_air_knockback_step(&m, ACT_FREEFALL_LAND, ACT_HARD_BACKWARD_GROUND_KB,
                              MARIO_ANIM_BACKWARD_AIR_KB, m.forwardVel);
    EXPECT_EQ(m.wallBonkMomentum, FALSE);
    // Ordinary damage keeps its original air speed and ground limits.
    begin(ACT_BACKWARD_AIR_KB);
    m.forwardVel = -100;
    sAirStepResult = AIR_STEP_NONE;
    common_air_knockback_step(&m, ACT_BACKWARD_GROUND_KB, ACT_HARD_BACKWARD_GROUND_KB,
                              MARIO_ANIM_BACKWARD_AIR_KB, -16);
    NEAR(m.forwardVel, -16);
    begin(ACT_BACKWARD_GROUND_KB);
    m.forwardVel = -100;
    sRealBonkRecovery = TRUE;
    common_ground_knockback_action(&m, MARIO_ANIM_BACKWARD_KB, 22, FALSE, 1);
    NEAR(m.forwardVel, -28.8f);
    m.marioObj->header.gfx.animInfo.animFrame = 23;
    common_ground_knockback_action(&m, MARIO_ANIM_BACKWARD_KB, 22, FALSE, 1);
    NEAR(m.forwardVel, -0.1f);
    return 1;
}

static int thicc_submerged_gravity(void) {
    const u32 actions[] = { ACT_FREEFALL, ACT_HOLD_FREEFALL, ACT_LONG_JUMP, ACT_SLIDE_KICK,
                            ACT_TWIRLING };
    const f32 gravity[] = { 4, 4, 2, 2.4f, 4 };
    const u32 caps[] = { MARIO_METAL_CAP, MARIO_VANISH_CAP, MARIO_METAL_CAP | MARIO_VANISH_CAP };
    for (int choice = 0; choice < 4; choice++) {
        for (unsigned cap = 0; cap < sizeof(caps) / sizeof(*caps); cap++) {
            for (unsigned action = 0; action < sizeof(actions) / sizeof(*actions); action++) {
                begin(actions[action]);
                sHeThiccFile = choice & 1;
                EXPECT_EQ(mario_character_request(choice), TRUE);
                mario_character_commit(&m, FALSE);
                m.flags = caps[cap];
                m.waterLevel = 1000;
                m.pos[1] = 0;
                m.vel[1] = -10;
                s32 metalResistance = (caps[cap] & MARIO_METAL_CAP) && !(caps[cap] & MARIO_VANISH_CAP);
                f32 timeScale = metalResistance ? METAL_WATER_TIME_SCALE : 1.0f;
                apply_gravity(&m);
                NEAR(m.vel[1], -10 - gravity[action] * (choice & 1 ? 1.25f : 1.0f) * timeScale);
                for (int frame = 0; frame < 500; frame++) {
                    apply_gravity(&m);
                }
                // Stored velocity is per movement interval; travel uses the slower water interval.
                f32 limit = metalResistance
                                ? METAL_WATER_MAX_FALL_SPEED / timeScale
                                : (choice & 1 ? HE_THICC_MAX_FALL_SPEED : MARIO_MAX_FALL_SPEED);
                // Twirling retains its own lower terminal speed before water integration.
                if (actions[action] == ACT_TWIRLING) {
                    limit = MIN(limit, choice & 1 ? HE_THICC_MAX_FALL_SPEED : MARIO_MAX_FALL_SPEED);
                }
                NEAR(m.vel[1], -limit);
            }
        }
    }
    begin(ACT_FREEFALL);
    sHeThiccFile = TRUE;
    mario_character_request(1);
    mario_character_commit(&m, FALSE);
    m.flags = MARIO_VANISH_CAP;
    m.waterLevel = 1000;
    m.pos[1] = 920;
    apply_gravity(&m);
    NEAR(m.vel[1], -4); // No added land gravity until physically submerged.
    m.pos[1] = 0;
    m.action = ACT_BACKWARD_AIR_KB;
    m.vel[1] = 0;
    apply_gravity(&m);
    NEAR(m.vel[1], -4); // Keep invulnerable damage reactions unchanged.
    return TRUE;
}

static int character_fall_speed(void) {
    for (int choice = 0; choice < 4; choice++) {
        begin(ACT_FALLING_DEATH_EXIT);
        sHeThiccFile = choice & 1;
        mario_character_request(choice);
        mario_character_commit(&m, FALSE);
        m.waterLevel = FLOOR_LOWER_LIMIT;
        m.vel[1] = -74.0f;
        for (int frame = 0; frame < 40; frame++) {
            apply_gravity(&m);
            EXPECT_EQ(m.vel[1], -75);
        }
    }
    const u32 actions[] = { ACT_FREEFALL,     ACT_JUMP,       ACT_HOLD_FREEFALL,
                            ACT_LONG_JUMP,    ACT_SLIDE_KICK, ACT_SHOT_FROM_CANNON,
                            ACT_GETTING_BLOWN };
    for (int cycle = 0; cycle < 3; cycle++) {
        for (int choice = 0; choice < 4; choice++) {
            for (unsigned i = 0; i < sizeof(actions) / sizeof(*actions); i++) {
                begin(actions[i]);
                sHeThiccFile = choice & 1;
                EXPECT_EQ(mario_character_request(choice), TRUE);
                mario_character_commit(&m, FALSE);
                m.waterLevel = FLOOR_LOWER_LIMIT;
                m.windGravity = 4;
                f32 limit = choice & 1 ? 127 : 100;
                for (int tick = 0; tick < 500; tick++) {
                    f32 before = m.vel[1];
                    apply_gravity(&m);
                    EXPECT_EQ(m.vel[1] <= before, TRUE);
                    EXPECT_EQ(m.vel[1] >= -limit, TRUE);
                }
                NEAR(m.vel[1], -limit);
                m.vel[1] = -200;
                apply_gravity(&m);
                NEAR(m.vel[1], -limit);
            }
        }
    }
    for (int choice = 0; choice < 4; choice++) {
        begin(ACT_TWIRLING);
        sHeThiccFile = choice & 1;
        mario_character_request(choice);
        mario_character_commit(&m, FALSE);
        m.waterLevel = FLOOR_LOWER_LIMIT;
        m.angleVel[1] = 2048;
        m.vel[1] = -200;
        apply_gravity(&m);
        NEAR(m.vel[1], -(choice & 1 ? 127 : 100) * 0.5f);
        // Wing flutter and explicit slow action limits keep their established behavior.
        m.action = ACT_FREEFALL;
        m.flags |= MARIO_WING_CAP;
        m.input = INPUT_A_DOWN;
        m.vel[1] = -37.5f;
        static struct TestBodyState body;
        m.marioBodyState = &body;
        apply_gravity(&m);
        NEAR(m.vel[1], -37.5f);
        m.flags = 0;
        m.input = 0;
        m.action = ACT_LAVA_BOOST;
        m.vel[1] = -200;
        apply_gravity(&m);
        NEAR(m.vel[1], -65);
        // Fully submerged Metal still uses its underwater descent limit.
        m.action = ACT_FREEFALL;
        m.flags = MARIO_METAL_CAP;
        m.waterLevel = 1000;
        m.pos[1] = 0;
        m.vel[1] = -200;
        apply_gravity(&m);
        NEAR(m.vel[1] * mario_get_movement_time_scale(&m), -METAL_WATER_MAX_FALL_SPEED);
    }
    return TRUE;
}

static int neutral_recovery_momentum(void) {
    // Release while carrying a heavy object or crawling, in both travel directions.
    for (int cycle = 0; cycle < 3; cycle++) {
        for (int heavy = 0; heavy < 2; heavy++) {
            for (int backwards = 0; backwards < 2; backwards++) {
                begin(heavy ? ACT_HOLD_HEAVY_WALKING : ACT_CRAWLING);
                sTrackRolloutGround = TRUE;
                sRealMovingEntry = TRUE;
                mario_set_forward_vel(&m, backwards ? -8 : 8);
                m.heldObj = heavy ? &sObj : NULL;
                m.input = INPUT_IDLE | (heavy ? 0 : INPUT_Z_DOWN);
                EXPECT_EQ(heavy ? act_hold_heavy_walking(&m) : act_crawling(&m), TRUE);
                EXPECT_EQ(m.action, heavy ? ACT_HOLD_HEAVY_IDLE : ACT_STOP_CRAWLING);
                NEAR(m.forwardVel, backwards ? -8 : 8);
                EXPECT_EQ(sRolloutGroundSteps, 0);
                f32 distance = 0;
                for (int tick = 0; tick < 16; tick++) {
                    f32 before = m.pos[2], speed = absf(m.forwardVel);
                    // Finish the crawl pose before stopping to exercise the crouching handoff.
                    sAnimEnded = tick >= 2;
                    if (heavy) {
                        act_hold_heavy_idle(&m);
                    } else if (m.action == ACT_STOP_CRAWLING) {
                        act_stop_crawling(&m);
                    } else {
                        EXPECT_EQ(m.action, ACT_CROUCHING);
                        act_crouching(&m);
                    }
                    EXPECT_EQ(m.heldObj, heavy ? &sObj : NULL);
                    NEAR(absf(m.forwardVel), fmaxf(0, speed - GROUND_NEUTRAL_COAST_DECEL));
                    NEAR(m.pos[2] - before, m.forwardVel);
                    distance += absf(m.pos[2] - before);
                }
                EXPECT_EQ(m.forwardVel, 0);
                EXPECT_EQ(distance > 20, TRUE);
                EXPECT_EQ(m.action, heavy ? ACT_HOLD_HEAVY_IDLE : ACT_CROUCHING);
            }
        }
    }
    sRealMovingEntry = FALSE;
    // Belly/dive entry keeps the threshold and its slow signed travel.
    for (int dive = 0; dive < 2; dive++) {
        for (int backwards = 0; backwards < 2; backwards++) {
            begin(dive ? ACT_DIVE_SLIDE : ACT_STOMACH_SLIDE);
            sRealMovingEntry = TRUE;
            sRealSliding = TRUE;
            sTrackRolloutGround = TRUE;
            sAnimEnded = TRUE;
            mario_set_forward_vel(&m, backwards ? -3 : 3);
            if (dive) {
                act_dive_slide(&m);
            } else {
                stomach_slide_action(&m, ACT_STOMACH_SLIDE_STOP, ACT_FREEFALL, MARIO_ANIM_SLIDE_DIVE);
            }
            EXPECT_EQ(m.action, ACT_STOMACH_SLIDE_STOP);
            EXPECT_EQ(absf(m.forwardVel) > 2, TRUE);
            EXPECT_EQ(m.forwardVel < 0, backwards);
            f32 before = m.pos[2];
            sAnimEnded = FALSE;
            act_stomach_slide_stop(&m);
            EXPECT_EQ(m.action, ACT_STOMACH_SLIDE_STOP);
            EXPECT_EQ(absf(m.pos[2] - before) > 1, TRUE);
            sAnimEnded = TRUE;
            act_stomach_slide_stop(&m);
            EXPECT_EQ(m.action, ACT_DECELERATING);
            EXPECT_EQ(absf(m.forwardVel) > 0, TRUE);
        }
    }
    sRealMovingEntry = FALSE;
    // An ended backward slide-kick animation must continue sliding at real speed.
    begin(ACT_SLIDE_KICK_SLIDE);
    sRealMovingEntry = TRUE;
    sRealSliding = TRUE;
    sTrackRolloutGround = TRUE;
    mario_set_forward_vel(&m, -12);
    sAnimEnded = TRUE;
    for (int tick = 0; tick < 5; tick++) {
        f32 before = m.pos[2];
        act_slide_kick_slide(&m);
        EXPECT_EQ(m.action, ACT_SLIDE_KICK_SLIDE);
        EXPECT_EQ(m.forwardVel < -1, TRUE);
        EXPECT_EQ(m.pos[2] < before, TRUE);
    }
    sRealMovingEntry = FALSE;
    // Coasting into an edge falls; a wall stops travel and stored velocity.
    for (int heavy = 0; heavy < 2; heavy++) {
        for (int wall = 0; wall < 2; wall++) {
            begin(heavy ? ACT_HOLD_HEAVY_IDLE : ACT_STOP_CRAWLING);
            sTrackRolloutGround = TRUE;
            mario_set_forward_vel(&m, -8);
            m.heldObj = heavy ? &sObj : NULL;
            sStep = wall ? GROUND_STEP_HIT_WALL : GROUND_STEP_LEFT_GROUND;
            if (heavy) {
                act_hold_heavy_idle(&m);
            } else {
                act_stop_crawling(&m);
            }
            if (wall) {
                EXPECT_EQ(m.forwardVel, 0);
                NEAR(m.vel[0], 0);
                NEAR(m.vel[2], 0);
            } else {
                EXPECT_EQ(m.action, ACT_FREEFALL);
                EXPECT_EQ(m.heldObj, NULL);
            }
        }
    }
    // Releasing Z through the recovery must not erase speed when standing completes.
    begin(ACT_STOP_CRAWLING);
    sTrackRolloutGround = TRUE;
    mario_set_forward_vel(&m, 8);
    sAnimEnded = TRUE;
    act_stop_crawling(&m);
    EXPECT_EQ(m.action, ACT_CROUCHING);
    EXPECT_EQ(act_crouching(&m), TRUE);
    EXPECT_EQ(m.action, ACT_STOP_CROUCHING);
    EXPECT_EQ(act_stop_crouching(&m), TRUE);
    EXPECT_EQ(m.action, ACT_DECELERATING);
    EXPECT_EQ(m.forwardVel > 0, TRUE);
    return TRUE;
}

static int belly_slide_input_chain(void) {
    for (int cycle = 0; cycle < 3; cycle++) {
        for (int button = 0; button < 2; button++) {
            begin(ACT_FORWARD_ROLLOUT);
            m.actionState = 1;
            mario_set_forward_vel(&m, 24);
            m.input = INPUT_B_PRESSED | INPUT_NONZERO_ANALOG;
            sController.stickMag = 64;
            sAirStepResult = AIR_STEP_LANDED;
            act_forward_rollout(&m);
            EXPECT_EQ(m.action, ACT_DIVE);
            EXPECT_EQ((m.actionArg & ACT_ARG_DIVE_FROM_ROLLOUT_CONTACT) != 0, TRUE);
            // The following update's edge must survive even though this dive has not run yet.
            m.input = button ? INPUT_B_PRESSED : INPUT_A_PRESSED;
            sAirStepResult = AIR_STEP_NONE;
            act_dive(&m);
            EXPECT_EQ(m.actionTimer, 6);
            EXPECT_EQ(m.actionArg & ACT_ARG_DIVE_FROM_ROLLOUT_CONTACT, 0);
            m.input = 0;
            sAirStepResult = AIR_STEP_LANDED;
            act_dive(&m);
            EXPECT_EQ(m.action, ACT_DIVE_SLIDE);
            EXPECT_EQ(m.actionArg, 5);
            EXPECT_EQ(act_dive_slide(&m), TRUE);
            EXPECT_EQ(m.action, ACT_FORWARD_ROLLOUT);
        }
    }
    for (int timer = 0; timer < 8; timer++) {
        for (int button = 0; button < 2; button++) {
            for (int backward = 0; backward < 2; backward++) {
                begin(ACT_STOMACH_SLIDE);
                m.actionTimer = timer;
                mario_set_forward_vel(&m, backward ? -24 : 24);
                m.input = button ? INPUT_B_PRESSED : INPUT_A_PRESSED;
                EXPECT_EQ(stomach_slide_action(&m, ACT_STOMACH_SLIDE_STOP, ACT_FREEFALL,
                                               MARIO_ANIM_SLIDE_DIVE),
                          TRUE);
                EXPECT_EQ(m.action, backward ? ACT_BACKWARD_ROLLOUT : ACT_FORWARD_ROLLOUT);
            }
        }
    }
    for (int input = 0; input < 5; input++) {
        begin(ACT_STOMACH_SLIDE_STOP);
        m.input = input == 0   ? INPUT_A_PRESSED
                  : input == 1 ? INPUT_B_PRESSED
                  : input == 2 ? INPUT_Z_DOWN
                  : input == 3 ? INPUT_NONZERO_ANALOG
                               : INPUT_A_PRESSED | INPUT_STOMPED;
        EXPECT_EQ(act_stomach_slide_stop(&m), TRUE);
        EXPECT_EQ(m.action, input == 0   ? ACT_DOUBLE_JUMP
                            : input == 1 ? ACT_PUNCHING
                            : input == 2 ? ACT_START_CROUCHING
                            : input == 3 ? ACT_WALKING
                                         : ACT_SHOCKWAVE_BOUNCE);
    }
    // Ordinary dive entry still excludes its initiating B and expires old requests.
    begin(ACT_DIVE);
    m.input = INPUT_B_PRESSED;
    sAirStepResult = AIR_STEP_NONE;
    act_dive(&m);
    EXPECT_EQ(m.actionTimer, 0);
    act_dive(&m);
    EXPECT_EQ(m.actionTimer, 6);
    m.input = 0;
    for (int frame = 0; frame < 6; frame++) {
        act_dive(&m);
    }
    sAirStepResult = AIR_STEP_LANDED;
    act_dive(&m);
    EXPECT_EQ(m.actionArg, 0);
    act_dive_slide(&m);
    EXPECT_EQ(m.action, ACT_DIVE_SLIDE);
    begin(ACT_STOMACH_SLIDE);
    m.input = INPUT_ABOVE_SLIDE | INPUT_A_PRESSED;
    sFloor.normal.y = 0.8f;
    EXPECT_EQ(stomach_slide_action(&m, ACT_STOMACH_SLIDE_STOP, ACT_FREEFALL, MARIO_ANIM_SLIDE_DIVE),
              FALSE);
    EXPECT_EQ(m.action, ACT_STOMACH_SLIDE);
    return TRUE;
}

static int belly_slide_forward_hold(void) {
    const struct {
        f32 magnitude;
        s16 yaw;
        s32 held;
    } sticks[] = { { 0, 0, FALSE },       { 4, 0, FALSE },
                   { 8, 0, TRUE },        { 64, 0, TRUE },
                   { 64, 0x2000, TRUE },  { 64, 0x4000, FALSE },
                   { 64, 0x6000, FALSE }, { 64, (s16) 0x8000, FALSE } };
    const u32 actions[] = { ACT_DIVE_SLIDE, ACT_STOMACH_SLIDE, ACT_HOLD_STOMACH_SLIDE };
    for (int cycle = 0; cycle < 3; cycle++) {
        for (int variant = 0; variant < 3; variant++) {
            for (unsigned stick = 0; stick < sizeof(sticks) / sizeof(*sticks); stick++) {
                for (int backward = FALSE; backward <= TRUE; backward++) {
                    u32 action = actions[variant];
                    u32 stop = variant == 2 ? ACT_DIVE_PICKING_UP : ACT_STOMACH_SLIDE_STOP;
                    begin(action);
                    sFloor.normal.y = 1.0f;
                    m.faceAngle[1] = 0x2000;
                    m.intendedYaw = (s16) (m.faceAngle[1] + sticks[stick].yaw);
                    m.input = sticks[stick].magnitude ? INPUT_NONZERO_ANALOG : 0;
                    sController.stickMag = sticks[stick].magnitude;
                    m.forwardVel = backward ? -3.0f : 3.0f;
                    sAnimEnded = TRUE;
                    if (variant == 2) {
                        m.heldObj = &sObj;
                    }
                    if (variant == 0) {
                        act_dive_slide(&m);
                    } else {
                        stomach_slide_action(&m, stop, variant == 2 ? ACT_HOLD_FREEFALL : ACT_FREEFALL,
                                             MARIO_ANIM_SLIDE_DIVE);
                    }
                    EXPECT_EQ(m.action, sticks[stick].held ? action : stop);
                    EXPECT_EQ(m.forwardVel, backward ? -3.0f : 3.0f);
                    if (sticks[stick].held) {
                        m.input = 0;
                        sController.stickMag = 0;
                        if (variant == 0) {
                            act_dive_slide(&m);
                        } else {
                            stomach_slide_action(&m, stop, ACT_FREEFALL, MARIO_ANIM_SLIDE_DIVE);
                        }
                        EXPECT_EQ(m.action, stop);
                    }
                    if (variant == 2) {
                        EXPECT_EQ(m.heldObj == &sObj, TRUE);
                    }
                }
            }
        }
    }
    return TRUE;
}

static int belly_slide_slope_rollout(void) {
    const struct {
        f32 x, z, speed, nx, nz;
        s16 facing;
        s32 allowed;
    } paths[] = {
        { 0, 24, 24, 0, 0.6f, 0, TRUE },              // Forward downhill.
        { 24, 0, 24, 0.6f, 0, 0x4000, TRUE },         // Rotated floor.
        { 14, 14, 20, 0.4f, 0.4f, 0x2000, TRUE },     // Diagonal downhill.
        { 0, 0.0001f, 0.0001f, 0, 0.6f, 0, TRUE },    // Positive low speed.
        { 0, 24, 24, 0, 0.95f, 0, TRUE },             // Steep slide.
        { 0, -24, 24, 0, 0.6f, (s16) 0x8000, FALSE }, // Forward uphill.
        { 24, -1, 24, 0, 0.6f, DEGREES(70), FALSE },  // Travel uphill, launch downhill.
        { 0, 24, -24, 0, 0.6f, (s16) 0x8000, FALSE }, // Backward downhill.
        { -24, 1, 24, 0, 0.6f, DEGREES(70), FALSE },  // Actual travel opposes facing.
        { 24, 1, 24, 0, 0.6f, DEGREES(110), FALSE },  // Slide downhill, launch uphill.
        { 24, 0, 24, 0, 0.6f, 0x4000, FALSE },        // Cross-slope.
        { 24, 0, 24, 0, 0.6f, DEGREES(70), FALSE },   // Cross-slope while facing downhill.
        { 0, 0, 0, 0, 0.6f, 0, FALSE },               // Stopped.
        { 0, 24, 0, 0, 0.6f, 0, FALSE },              // No signed rollout speed.
    };
    for (int cycle = 0; cycle < 3; cycle++) {
        for (int variant = 0; variant < 3; variant++) {
            for (unsigned path = 0; path < sizeof(paths) / sizeof(paths[0]); path++) {
                for (int request = 0; request < 3; request++) {
                    if (request == 2 && variant != 2) {
                        continue;
                    }
                    u32 action = variant == 2 ? ACT_DIVE_SLIDE
                                 : variant    ? ACT_HOLD_STOMACH_SLIDE
                                              : ACT_STOMACH_SLIDE;
                    begin(action);
                    m.forwardVel = paths[path].speed;
                    m.slideVelX = paths[path].x;
                    m.slideVelZ = paths[path].z;
                    m.faceAngle[1] = paths[path].facing;
                    sFloor.normal.x = paths[path].nx;
                    sFloor.normal.z = paths[path].nz;
                    sFloor.normal.y = sqrtf(1.0f - sqr(paths[path].nx) - sqr(paths[path].nz));
                    if (variant == 1) {
                        m.heldObj = &sHeld;
                    }
                    m.input = INPUT_ABOVE_SLIDE
                              | (request == 0   ? INPUT_A_PRESSED
                                 : request == 1 ? INPUT_B_PRESSED
                                                : 0);
                    if (request == 2) {
                        m.actionArg = 6;
                    }
                    s32 rolled =
                        variant == 2
                            ? act_dive_slide(&m)
                            : stomach_slide_action(
                                  &m, variant ? ACT_DIVE_PICKING_UP : ACT_STOMACH_SLIDE_STOP,
                                  variant ? ACT_HOLD_FREEFALL : ACT_FREEFALL, MARIO_ANIM_SLIDE_DIVE);
                    EXPECT_EQ(rolled, paths[path].allowed);
                    EXPECT_EQ(m.action, paths[path].allowed ? ACT_FORWARD_ROLLOUT : action);
                    NEAR(m.forwardVel, paths[path].speed);
                    if (variant == 1) {
                        EXPECT_EQ(m.heldObj == NULL, paths[path].allowed);
                    }
                    if (variant == 2) {
                        EXPECT_EQ(m.actionArg, paths[path].allowed ? 0 : 5);
                    }
                }
            }
        }
    }
    // An uphill queue expires; it cannot wait indefinitely for another rollout.
    begin(ACT_DIVE_SLIDE);
    m.forwardVel = 24;
    m.slideVelZ = -24;
    m.faceAngle[1] = 0x8000;
    sFloor.normal.y = 0.8f;
    sFloor.normal.z = 0.6f;
    m.actionArg = 6;
    for (int frame = 0; frame < 8; frame++) {
        m.input = INPUT_ABOVE_SLIDE;
        EXPECT_EQ(act_dive_slide(&m), FALSE);
        EXPECT_EQ(m.action, ACT_DIVE_SLIDE);
        EXPECT_EQ(m.actionArg, MAX(0, 5 - frame));
        NEAR(m.forwardVel, 24);
    }
    m.faceAngle[1] = 0;
    m.slideVelZ = 24;
    EXPECT_EQ(act_dive_slide(&m), FALSE); // Expired, even after turning downhill.
    m.input |= INPUT_B_PRESSED;
    EXPECT_EQ(act_dive_slide(&m), TRUE);
    EXPECT_EQ(m.action, ACT_FORWARD_ROLLOUT);
    // A still-live queue may roll out when the slide reaches flat ground.
    begin(ACT_DIVE_SLIDE);
    m.forwardVel = -24;
    m.slideVelZ = -24;
    m.actionArg = 2;
    m.input = INPUT_ABOVE_SLIDE;
    sFloor.normal.y = 0.8f;
    sFloor.normal.z = 0.6f;
    EXPECT_EQ(act_dive_slide(&m), FALSE);
    EXPECT_EQ(m.actionArg, 1);
    m.input = 0;
    EXPECT_EQ(act_dive_slide(&m), TRUE);
    EXPECT_EQ(m.action, ACT_BACKWARD_ROLLOUT);
    // Held buttons alone cannot start or renew the request.
    for (int dive = 0; dive < 2; dive++) {
        begin(dive ? ACT_DIVE_SLIDE : ACT_STOMACH_SLIDE);
        m.input = INPUT_ABOVE_SLIDE | INPUT_A_DOWN | INPUT_B_DOWN;
        m.forwardVel = 24;
        m.slideVelZ = 24;
        sFloor.normal.y = 0.8f;
        sFloor.normal.z = 0.6f;
        EXPECT_EQ(dive ? act_dive_slide(&m)
                       : stomach_slide_action(&m, ACT_STOMACH_SLIDE_STOP, ACT_FREEFALL,
                                              MARIO_ANIM_SLIDE_DIVE),
                  FALSE);
        EXPECT_EQ(m.action, dive ? ACT_DIVE_SLIDE : ACT_STOMACH_SLIDE);
        EXPECT_EQ(m.actionArg, 0);
    }
    // Flat-ground direction boundaries retain their established differences.
    for (int dive = 0; dive < 2; dive++) {
        for (int direction = -1; direction <= 1; direction++) {
            begin(dive ? ACT_DIVE_SLIDE : ACT_STOMACH_SLIDE);
            m.forwardVel = direction * 24;
            m.input = INPUT_A_PRESSED;
            if (dive) {
                act_dive_slide(&m);
            } else {
                stomach_slide_action(&m, ACT_STOMACH_SLIDE_STOP, ACT_FREEFALL, MARIO_ANIM_SLIDE_DIVE);
            }
            s32 forwardRollout = dive ? direction > 0 : direction >= 0;
            EXPECT_EQ(m.action, forwardRollout ? ACT_FORWARD_ROLLOUT : ACT_BACKWARD_ROLLOUT);
        }
    }
    begin(ACT_STOMACH_SLIDE);
    m.input = INPUT_ABOVE_SLIDE | INPUT_A_PRESSED;
    m.floor = NULL;
    m.forwardVel = 24;
    m.slideVelZ = 24;
    // Missing support is an eligibility check, not a valid sliding-physics fixture.
    EXPECT_EQ(can_rollout_from_belly_slide(&m), FALSE);
    return TRUE;
}

static int water_twirl_entry(void) {
    for (int supported = 0; supported < 2; supported++) {
        begin(ACT_WALKING);
        m.flags |= MARIO_WING_CAP;
        m.floor = supported ? &gWaterSurfacePseudoFloor : &sFloor;
        set_mario_action(&m, ACT_GROUND_TWIRL, 0);
        m.forwardVel = 0.0f;
        m.vel[1] = 0.0f;
        sController.buttonDown = L_TRIG;
        EXPECT_EQ(m.waterWalkTwirl, supported);
        for (int frame = 0; frame < 1800; frame++) {
            mario_update_water_walking_state(&m, 0.0f, 10000.0f);
            EXPECT_EQ(mario_can_walk_on_water(&m, 0.0f, 10000.0f), supported);
        }
        if (supported) {
            sController.buttonDown = 0;
            EXPECT_EQ(mario_can_walk_on_water(&m, 0.0f, 10000.0f), FALSE);
            sController.buttonDown = L_TRIG;
            m.input = INPUT_NONZERO_ANALOG;
            EXPECT_EQ(mario_can_walk_on_water(&m, 0.0f, 10000.0f), FALSE);
            m.input = 0;
            m.flags &= ~MARIO_WING_CAP;
            EXPECT_EQ(mario_can_walk_on_water(&m, 0.0f, 10000.0f), FALSE);
        }
        set_mario_action(&m, ACT_IDLE, 0);
        EXPECT_EQ(m.waterWalkTwirl, FALSE);
    }
    return TRUE;
}

static int rollout_water_walk_timing(void) {
    const u32 actions[] = { ACT_FORWARD_ROLLOUT, ACT_BACKWARD_ROLLOUT };
    for (unsigned i = 0; i < sizeof(actions) / sizeof(*actions); i++) {
        begin(actions[i]);
        m.flags |= MARIO_WING_CAP;
        m.forwardVel = 48.0f;
        m.waterWalkCooldown = 5; // A preceding dive's cooldown expires during entry.
        sAirStepResult = AIR_STEP_NONE;
        for (int frame = 0; frame < 12; frame++) {
            mario_tick_water_walking_cooldown(&m);
            mario_update_water_walking_state(&m, 0.0f, 10000.0f);
            EXPECT_EQ(mario_can_walk_on_water(&m, 0.0f, 10000.0f), frame >= 8);
            if (i == 0) {
                // This timer is a follow-up queue, not rollout age.
                m.actionTimer = 3;
                act_forward_rollout(&m);
            } else {
                act_backward_rollout(&m);
            }
            EXPECT_EQ(m.rolloutFrames, MIN(frame + 1, 8));
        }
        m.forwardVel = 29.0f;
        EXPECT_EQ(mario_can_walk_on_water(&m, 0.0f, 10000.0f), FALSE);
        set_mario_action(&m, ACT_WALKING, 0);
        set_mario_action(&m, actions[i], 0);
        m.forwardVel = 48.0f;
        EXPECT_EQ(m.rolloutFrames, 0);
        EXPECT_EQ(mario_can_walk_on_water(&m, 0.0f, 10000.0f), FALSE);
    }
    return TRUE;
}

static int rollout_recovery_inputs(void) {
    const s16 attackAngles[] = { 0, 0x4000, 0x471C, 0x471D, (s16) 0x8000, -0x471D };
    for (unsigned angle = 0; angle < 6; angle++) {
        for (int strong = 0; strong < 2; strong++) {
            begin(ACT_WALKING);
            m.faceAngle[1] = 0x7000;
            m.intendedYaw = (s16) (m.faceAngle[1] + attackAngles[angle]);
            m.input = INPUT_B_PRESSED | INPUT_NONZERO_ANALOG;
            sController.stickMag = strong ? 64 : 20;
            EXPECT_EQ(check_ground_dive_or_punch(&m), TRUE);
            EXPECT_EQ(m.action, strong && angle <= 2 ? ACT_DIVE : ACT_MOVE_PUNCHING);
            if (m.action == ACT_MOVE_PUNCHING) {
                EXPECT_EQ(m.actionArg, 0);
            }
        }
    }

    const u16 buttons[] = { A_BUTTON, B_BUTTON, Z_TRIG, A_BUTTON | Z_TRIG, B_BUTTON | Z_TRIG };
    const float sticks[] = { 0, 20, 64, 64 };
    for (int cycle = 0; cycle < 3; cycle++) {
        for (int backward = 0; backward < 2; backward++) {
            for (int input = 0; input < 5; input++) {
                for (int stick = 0; stick < 4; stick++) {
                    begin(backward ? ACT_BACKWARD_ROLLOUT : ACT_FORWARD_ROLLOUT);
                    mario_set_forward_vel(&m, backward ? -24.0f : 24.0f);
                    sAirStepResult = AIR_STEP_NONE;
                    if (backward) {
                        act_backward_rollout(&m);
                    } else {
                        act_forward_rollout(&m);
                    }
                    sController.buttonPressed = buttons[input];
                    sController.buttonDown = buttons[input];
                    sController.stickMag = sticks[stick];
                    m.intendedYaw = stick == 3 ? 0x7fff : 0;
                    m.input = INPUT_OFF_FLOOR | INPUT_ABOVE_SLIDE | (stick ? INPUT_NONZERO_ANALOG : 0);
                    update_mario_button_inputs(&m);
                    sAirStepResult = AIR_STEP_LANDED;
                    sTrackRolloutGround = TRUE;
                    EXPECT_EQ(backward ? act_backward_rollout(&m) : act_forward_rollout(&m), FALSE);
                    u32 expected;
                    switch (input) {
                        case 0:
                            expected = ACT_DOUBLE_JUMP;
                            break;
                        case 1:
                            expected = stick == 2 ? ACT_DIVE : ACT_MOVE_PUNCHING;
                            break;
                        case 2:
                            expected = ACT_CROUCH_SLIDE;
                            break;
                        case 3:
                            expected = stick >= 2 ? ACT_LONG_JUMP : ACT_BACKFLIP;
                            break;
                        default:
                            expected = stick == 2 ? ACT_SLIDE_KICK : ACT_MOVE_PUNCHING;
                            break;
                    }
                    EXPECT_EQ(m.action, expected);
                    EXPECT_EQ(sRolloutGroundSteps, 0); // Contact already spent the movement step.
                    EXPECT_EQ(m.input & INPUT_OFF_FLOOR, 0);
                    if (input == 3) {
                        EXPECT_EQ(m.input & INPUT_Z_PRESSED, 0);
                    }
                    if (input == 1 && stick == 2) {
                        NEAR(m.forwardVel, 29.0f);
                        NEAR(m.vel[1], 25.0f);
                    }
                    if (input == 4 && stick != 2) {
                        EXPECT_EQ(m.actionArg, ACT_ARG_PUNCH_SEQUENCE_BREAKDANCE);
                    }
                }
            }
        }
    }
    // The initiating press cannot be reused on an immediate landing.
    for (int backward = 0; backward < 2; backward++) {
        for (int input = 0; input < 2; input++) {
            begin(backward ? ACT_BACKWARD_ROLLOUT : ACT_FORWARD_ROLLOUT);
            m.input = (input ? INPUT_B_PRESSED : INPUT_A_PRESSED) | INPUT_NONZERO_ANALOG;
            sController.stickMag = 64;
            sAirStepResult = AIR_STEP_LANDED;
            if (backward) {
                act_backward_rollout(&m);
            } else {
                act_forward_rollout(&m);
            }
            EXPECT_EQ(m.action, ACT_WALKING);
        }
    }
    // Queued slide kicks keep A and environmental priority at touchdown.
    for (int mode = 0; mode < 3; mode++) {
        begin(ACT_FORWARD_ROLLOUT);
        m.actionState = 1;
        m.actionArg = ACT_ARG_FORWARD_ROLLOUT_FROM_SLIDE_KICK;
        m.actionTimer = 3;
        if (mode == 0) {
            m.input = INPUT_A_PRESSED;
        } else if (mode == 1) {
            m.input = INPUT_STOMPED;
        } else {
            m.input = INPUT_FIRST_PERSON;
        }
        sAirStepResult = AIR_STEP_LANDED;
        act_forward_rollout(&m);
        u32 expectedAction;
        if (mode == 0) {
            expectedAction = ACT_DOUBLE_JUMP;
        } else if (mode == 1) {
            expectedAction = ACT_SHOCKWAVE_BOUNCE;
        } else {
            expectedAction = ACT_IDLE;
        }
        EXPECT_EQ(m.action, expectedAction);
    }
    for (int backward = 0; backward < 2; backward++) {
        for (int mode = 0; mode < 3; mode++) {
            begin(backward ? ACT_BACKWARD_ROLLOUT : ACT_FORWARD_ROLLOUT);
            m.actionState = 1;
            m.input = INPUT_A_PRESSED | INPUT_B_PRESSED | INPUT_NONZERO_ANALOG;
            if (mode == 0) {
                m.health = 0xff;
            }
            if (mode == 1) {
                m.input |= INPUT_SQUISHED;
            }
            if (mode == 2) {
                m.waterLevel = 200;
            }
            sAirStepResult = AIR_STEP_LANDED;
            EXPECT_EQ(backward ? act_backward_rollout(&m) : act_forward_rollout(&m), FALSE);
            EXPECT_EQ(m.action, mode == 0   ? ACT_STANDING_DEATH
                                : mode == 1 ? ACT_SQUISHED
                                            : ACT_WATER_PLUNGE);
        }
    }
    return 1;
}
static int rollout_recovery_momentum(void) {
    for (int cycle = 0; cycle < 3; cycle++) {
        for (int backward = 0; backward < 2; backward++) {
            for (int stick = 0; stick < 2; stick++) {
                begin(backward ? ACT_BACKWARD_ROLLOUT : ACT_FORWARD_ROLLOUT);
                mario_set_forward_vel(&m, backward ? -24.0f : 24.0f);
                m.input = stick ? INPUT_NONZERO_ANALOG : 0;
                sController.stickMag = stick ? 64 : 0;
                sAirStepResult = AIR_STEP_LANDED;
                sTrackRolloutGround = TRUE;
                EXPECT_EQ(backward ? act_backward_rollout(&m) : act_forward_rollout(&m), FALSE);
                EXPECT_EQ(m.action, stick ? ACT_WALKING : ACT_DECELERATING);
                EXPECT_EQ(m.actionArg, ACT_ARG_GROUND_SPEED_FROM_ROLLOUT);
                NEAR(m.forwardVel, backward ? -24.0f : 24.0f);
                NEAR(m.vel[2], m.forwardVel);
                NEAR(m.vel[1], 0);
                NEAR(m.pos[2], 0);
                EXPECT_EQ(sRolloutGroundSteps, 0);
                if (stick) {
                    m.input = INPUT_IDLE;
                    EXPECT_EQ(begin_braking_action(&m), TRUE);
                    EXPECT_EQ(m.action, backward ? ACT_DECELERATING : ACT_BRAKING);
                    EXPECT_EQ(m.actionArg, ACT_ARG_GROUND_SPEED_FROM_ROLLOUT);
                }
                if (m.action == ACT_DECELERATING) {
                    update_decelerating_speed(&m);
                    NEAR(m.forwardVel, backward ? -22.5f : 22.5f);
                }
                // The next fresh B uses ordinary ground attack handling immediately.
                m.input = INPUT_B_PRESSED | INPUT_NONZERO_ANALOG;
                sController.stickMag = 64;
                EXPECT_EQ(check_ground_dive_or_punch(&m), TRUE);
                EXPECT_EQ(m.action, ACT_DIVE);
            }
        }
    }
    begin(ACT_FORWARD_ROLLOUT);
    sAirStepResult = AIR_STEP_LANDED;
    act_forward_rollout(&m);
    EXPECT_EQ(m.action, ACT_IDLE);
    // An unrelated freefall landing still uses its original recovery.
    begin(ACT_FREEFALL_LAND_STOP);
    m.prevAction = ACT_FREEFALL;
    mario_set_forward_vel(&m, 20.0f);
    sTrackRolloutGround = 1;
    EXPECT_EQ(act_freefall_land_stop(&m), FALSE);
    EXPECT_EQ(m.action, ACT_FREEFALL_LAND_STOP);
    NEAR(m.forwardVel, 0.0f);
    begin(ACT_JUMP_LAND);
    m.actionTimer = sJumpLandAction.numFrames - 1;
    m.forwardVel = 12.0f;
    EXPECT_EQ(common_landing_cancels(&m, &sJumpLandAction, set_jumping_action), TRUE);
    EXPECT_EQ(m.action, ACT_DECELERATING);
    EXPECT_EQ(m.actionArg, 0);
    update_decelerating_speed(&m);
    NEAR(m.forwardVel, 11.0f);
    return 1;
}
static int dialogue_camera(void) {
    sDialogueCameraBlockedButtons = 0;
    u32 actions[] = { ACT_WAITING_FOR_DIALOG, ACT_READING_SIGN, ACT_READING_NPC_DIALOG,
                      ACT_READING_AUTOMATIC_DIALOG };
    for (unsigned i = 0; i < 4; i++) {
        EXPECT_EQ(dialogue_camera_input_mask(actions[i], R_TRIG | CBUTTON_MASK),
                  R_TRIG | CBUTTON_MASK | D_JPAD);
        EXPECT_EQ(dialogue_camera_input_mask(ACT_IDLE, R_TRIG | CBUTTON_MASK), R_TRIG | CBUTTON_MASK);
        EXPECT_EQ(dialogue_camera_input_mask(ACT_IDLE, 0), 0);
        EXPECT_EQ(dialogue_camera_input_mask(ACT_IDLE, R_TRIG | CBUTTON_MASK), 0);
    }
    EXPECT_EQ(dialogue_camera_input_mask(ACT_READING_SIGN, R_TRIG), R_TRIG | CBUTTON_MASK | D_JPAD);
    EXPECT_EQ(dialogue_camera_input_mask(ACT_READING_SIGN, 0), R_TRIG | CBUTTON_MASK | D_JPAD);
    EXPECT_EQ(dialogue_camera_input_mask(ACT_IDLE, R_TRIG), 0);
    return 1;
}
static int camera_hud_arrow(void) {
    begin(ACT_WALKING);
    gPlayer1Controller = &sController;
    sCamera.mode = CAMERA_MODE_8_DIRECTIONS;
    sCamera.cutscene = CUTSCENE_NONE;
    sSelectionFlags = 0;
    sCamSelection = CAM_SELECTION_MARIO;
    gCameraMovementFlags = 0;
    EXPECT_EQ(update_camera_hud_status(&sCamera), CAM_STATUS_LAKITU);

    // C-Up is requested during a run, but the camera remains in its normal mode.
    gCameraMovementFlags = CAM_MOVE_C_UP_MODE;
    EXPECT_EQ(update_camera_hud_status(&sCamera) & CAM_STATUS_C_MODE_GROUP, 0);
    // C-Down during that wait must display down, never both arrows or neither.
    gCameraMovementFlags |= CAM_MOVE_ZOOMED_OUT;
    EXPECT_EQ(update_camera_hud_status(&sCamera) & CAM_STATUS_C_MODE_GROUP, CAM_STATUS_C_DOWN);
    sCamera.mode = CAMERA_MODE_C_UP;
    EXPECT_EQ(update_camera_hud_status(&sCamera) & CAM_STATUS_C_MODE_GROUP, CAM_STATUS_C_UP);
    EXPECT_EQ(sHudCameraStatus & CAM_STATUS_C_MODE_GROUP, CAM_STATUS_C_UP);
    // Return from first person while the C-Down setting remains active.
    sCamera.mode = CAMERA_MODE_8_DIRECTIONS;
    EXPECT_EQ(update_camera_hud_status(&sCamera) & CAM_STATUS_C_MODE_GROUP, CAM_STATUS_C_DOWN);
    gCameraMovementFlags = 0;
    EXPECT_EQ(update_camera_hud_status(&sCamera) & CAM_STATUS_C_MODE_GROUP, 0);

    sCamera.mode = CAMERA_MODE_C_UP;
    EXPECT_EQ(update_camera_hud_status(&sCamera) & CAM_STATUS_C_MODE_GROUP, CAM_STATUS_C_UP);
    sCamera.cutscene = 1;
    EXPECT_EQ(update_camera_hud_status(&sCamera) & CAM_STATUS_C_MODE_GROUP, 0);
    EXPECT_EQ(sHudCameraStatus & CAM_STATUS_MODE_GROUP, CAM_STATUS_FIXED);
    sCamera.cutscene = CUTSCENE_NONE;
    sCamera.mode = CAMERA_MODE_8_DIRECTIONS;
    sSelectionFlags = CAM_MODE_MARIO_ACTIVE;
    EXPECT_EQ(update_camera_hud_status(&sCamera) & CAM_STATUS_MODE_GROUP, CAM_STATUS_MARIO);
    sController.buttonDown = R_TRIG;
    sCamSelection = CAM_SELECTION_FIXED;
    EXPECT_EQ(update_camera_hud_status(&sCamera) & CAM_STATUS_MODE_GROUP, CAM_STATUS_FIXED);
    return 1;
}
static int c_up_neutral_entry(void) {
    begin(ACT_IDLE);
    gPlayer1Controller = &sController;
    gCameraMovementFlags = sCameraSoundFlags = 0;
    sCUpSounds = sCUpExitSounds = sCUpStores = 0;
    // A press while steering is rejected, even if the stick later centers.
    sController.stickMag = 32;
    set_mode_c_up(&sCamera);
    EXPECT_EQ(gCameraMovementFlags & CAM_MOVE_C_UP_MODE, 0);
    EXPECT_EQ(sCUpStores, 0);
    sController.stickMag = 0;
    m.input = 0;
    mario_update_c_up_input(&m);
    EXPECT_EQ(m.input & INPUT_FIRST_PERSON, 0);

    set_mode_c_up(&sCamera);
    EXPECT_EQ(sCUpStores, 1);
    mario_update_c_up_input(&m);
    EXPECT_EQ(m.input & INPUT_FIRST_PERSON, INPUT_FIRST_PERSON);
    EXPECT_EQ(sCUpSounds, 0); // The request itself is silent.
    play_c_up_exit_sound_once();
    EXPECT_EQ(sCUpExitSounds, 0);
    play_c_up_entry_sound_if_zooming(800, 800);
    play_c_up_entry_sound_if_zooming(800, 900);
    EXPECT_EQ(sCUpSounds, 0);
    play_c_up_entry_sound_if_zooming(800, 780);
    play_c_up_entry_sound_if_zooming(800, 760);
    EXPECT_EQ(sCUpSounds, 1);
    play_c_up_exit_sound_once();
    play_c_up_exit_sound_once();
    EXPECT_EQ(sCUpExitSounds, 1);

    // Steering cancels an accepted request before first-person entry.
    m.input = 0;
    sController.stickMag = 32;
    mario_update_c_up_input(&m);
    EXPECT_EQ(gCameraMovementFlags & CAM_MOVE_C_UP_MODE, 0);
    EXPECT_EQ(m.input & INPUT_FIRST_PERSON, 0);
    sController.stickMag = 0;
    mario_update_c_up_input(&m);
    EXPECT_EQ(m.input & INPUT_FIRST_PERSON, 0);
    set_mode_c_up(&sCamera);
    EXPECT_EQ(sCUpStores, 2);
    EXPECT_EQ(sCUpSounds, 1);
    play_c_up_entry_sound_if_zooming(800, 799);
    EXPECT_EQ(sCUpSounds, 2);
    play_c_up_exit_sound_once();
    EXPECT_EQ(sCUpExitSounds, 2);
    play_sound_cbutton_up();
    EXPECT_EQ(sCUpSounds, 2); // Request-side helper stays silent.

    begin(ACT_JUMP);
    gCameraMovementFlags = CAM_MOVE_C_UP_MODE;
    mario_update_c_up_input(&m);
    EXPECT_EQ(gCameraMovementFlags & CAM_MOVE_C_UP_MODE, 0);
    const f32 lookStick[][2] = {
        { 32, 0 }, { -32, 0 }, { 0, 32 }, { 0, -32 }, { 32, 32 }, { -32, -32 }
    };
    for (int cycle = 0; cycle < 3; cycle++) {
        begin(ACT_FIRST_PERSON);
        gCameraMovementFlags = CAM_MOVE_C_UP_MODE;
        for (unsigned direction = 0; direction < sizeof(lookStick) / sizeof(lookStick[0]);
             direction++) {
            sController.stickMag = 32;
            sController.stickX = lookStick[direction][0];
            sController.stickY = lookStick[direction][1];
            for (int update = 0; update < 20; update++) {
                m.input = INPUT_NONZERO_ANALOG;
                mario_update_c_up_input(&m);
                EXPECT_EQ(gCameraMovementFlags & CAM_MOVE_C_UP_MODE, CAM_MOVE_C_UP_MODE);
                EXPECT_EQ(m.input & INPUT_FIRST_PERSON, INPUT_FIRST_PERSON);
                EXPECT_EQ(m.input & INPUT_NONZERO_ANALOG, INPUT_NONZERO_ANALOG);
                NEAR(sController.stickX, lookStick[direction][0]);
                NEAR(sController.stickY, lookStick[direction][1]);
            }
        }
        // Releasing the stick keeps the view; clearing the camera latch still exits.
        m.input = 0;
        sController.stickMag = 0;
        mario_update_c_up_input(&m);
        EXPECT_EQ(m.input & INPUT_FIRST_PERSON, INPUT_FIRST_PERSON);
        m.input = 0;
        gCameraMovementFlags = 0;
        mario_update_c_up_input(&m);
        EXPECT_EQ(m.input & INPUT_FIRST_PERSON, 0);
    }
    return 1;
}
static int c_up_rejection_feedback(void) {
    const u32 blockedActions[] = { ACT_JUMP,        ACT_FREEFALL,
                                   ACT_JUMP_KICK,   ACT_TOP_OF_POLE_JUMP,
                                   ACT_WATER_IDLE,  ACT_READING_NPC_DIALOG,
                                   ACT_GROUND_POUND };
    const u16 otherButtons[] = { D_CBUTTONS, L_CBUTTONS,          R_CBUTTONS,
                                 R_TRIG,     D_CBUTTONS | R_TRIG, L_CBUTTONS | R_CBUTTONS };
    for (int cycle = 0; cycle < 3; cycle++) {
        for (unsigned state = 0; state < sizeof(blockedActions) / sizeof(blockedActions[0]); state++) {
            begin(blockedActions[state]);
            gPlayer1Controller = &sController;
            sCamera.mode = CAMERA_MODE_FREE_ROAM;
            sCamera.cutscene = CUTSCENE_NONE;
            gCameraMovementFlags = 0;
            sController.buttonDown = sController.buttonPressed = U_CBUTTONS;
            sCUpBlockedSounds = 0;
            play_c_up_rejection_sound(&sCamera, CAMERA_MODE_FREE_ROAM, 0);
            EXPECT_EQ(sCUpBlockedSounds, 1);
            EXPECT_EQ(sLastMarioSound, SOUND_MENU_CAMERA_BUZZ);
            EXPECT_EQ(m.action, blockedActions[state]);
            EXPECT_EQ(gCameraMovementFlags, 0);
            sController.buttonPressed = 0;
            for (int held = 0; held < 20; held++) {
                play_c_up_rejection_sound(&sCamera, CAMERA_MODE_FREE_ROAM, 0);
            }
            EXPECT_EQ(sCUpBlockedSounds, 1);
            sController.buttonPressed = U_CBUTTONS;
            play_c_up_rejection_sound(&sCamera, CAMERA_MODE_FREE_ROAM, 0);
            EXPECT_EQ(sCUpBlockedSounds, 2);
            for (unsigned button = 0; button < sizeof(otherButtons) / sizeof(otherButtons[0]);
                 button++) {
                sController.buttonDown = U_CBUTTONS | otherButtons[button];
                play_c_up_rejection_sound(&sCamera, CAMERA_MODE_FREE_ROAM, 0);
                EXPECT_EQ(sCUpBlockedSounds, 2);
            }
        }
        begin(ACT_IDLE);
        gPlayer1Controller = &sController;
        sCamera.mode = CAMERA_MODE_FREE_ROAM;
        sCamera.cutscene = CUTSCENE_NONE;
        sController.buttonDown = sController.buttonPressed = U_CBUTTONS;
        gCameraMovementFlags = CAM_MOVE_C_UP_MODE;
        sCUpBlockedSounds = 0;
        play_c_up_rejection_sound(&sCamera, CAMERA_MODE_FREE_ROAM, 0);
        EXPECT_EQ(sCUpBlockedSounds, 0); // Neutral, eligible pending entry.
        sController.stickMag = 32;
        play_c_up_rejection_sound(&sCamera, CAMERA_MODE_FREE_ROAM, 0);
        EXPECT_EQ(sCUpBlockedSounds, 1);
        sController.stickMag = 0;
        sCamera.cutscene = 1;
        play_c_up_rejection_sound(&sCamera, CAMERA_MODE_FREE_ROAM, 0);
        EXPECT_EQ(sCUpBlockedSounds, 2);
        sCamera.cutscene = CUTSCENE_NONE;
        m.action = ACT_FIRST_PERSON;
        sCamera.mode = CAMERA_MODE_C_UP;
        sController.stickMag = 32;
        play_c_up_rejection_sound(&sCamera, CAMERA_MODE_C_UP, 0);
        EXPECT_EQ(sCUpBlockedSounds, 2); // Looking remains valid in first person.
        m.action = ACT_FREEFALL;
        sCamera.mode = CAMERA_MODE_FREE_ROAM;
        play_c_up_rejection_sound(&sCamera, CAMERA_MODE_C_UP, 0);
        EXPECT_EQ(sCUpBlockedSounds, 2); // An exit must not also report failed entry.
        sController.stickMag = 0;
        gCameraMovementFlags = 0;
        play_c_up_rejection_sound(&sCamera, CAMERA_MODE_FREE_ROAM, CAM_MOVE_ZOOMED_OUT);
        EXPECT_EQ(sCUpBlockedSounds, 2); // C-Up successfully returned from wide zoom.
        gCameraMovementFlags = CAM_MOVE_ZOOMED_OUT;
        play_c_up_rejection_sound(&sCamera, CAMERA_MODE_FREE_ROAM, CAM_MOVE_ZOOMED_OUT);
        EXPECT_EQ(sCUpBlockedSounds, 3); // A wide-zoom flag alone is not a successful response.
    }
    return TRUE;
}

static int enemy_draw_distance_hysteresis(void) {
    const u32 flags = OBJ_FLAG_COMPUTE_DIST_TO_MARIO;
    for (int room = -1; room <= 0; room++) {
        begin(ACT_IDLE);
        o->oRoom = room;
        o->oDrawingDistance = 1000.0f;
        o->oHeldState = HELD_FREE;
        o->oFlags = flags;
        for (int cycle = 0; cycle < 3; cycle++) {
            cur_obj_update_distance_visibility(990.0f, flags, MARIO_INSIDE_ROOM);
            EXPECT_EQ(o->header.gfx.node.flags & GRAPH_RENDER_ACTIVE, GRAPH_RENDER_ACTIVE);
            for (int jitter = 0; jitter < 8; jitter++) {
                f32 distance = jitter & 1 ? 1050.0f : 990.0f;
                cur_obj_update_distance_visibility(distance, flags, MARIO_INSIDE_ROOM);
                EXPECT_EQ(o->header.gfx.node.flags & GRAPH_RENDER_ACTIVE,
                          distance < 1000.0f ? GRAPH_RENDER_ACTIVE : 0);
                EXPECT_EQ((o->activeFlags & ACTIVE_FLAG_FAR_AWAY) != 0, distance > 1000.0f);
            }
            cur_obj_update_distance_visibility(1000.0f, flags, MARIO_INSIDE_ROOM);
            EXPECT_EQ(o->header.gfx.node.flags & GRAPH_RENDER_ACTIVE,
                      room == -1 ? GRAPH_RENDER_ACTIVE : 0);
            cur_obj_update_distance_visibility(1101.0f, flags, MARIO_INSIDE_ROOM);
            EXPECT_EQ(o->header.gfx.node.flags & GRAPH_RENDER_ACTIVE, 0);
            cur_obj_update_distance_visibility(1050.0f, flags, MARIO_INSIDE_ROOM);
            EXPECT_EQ(o->header.gfx.node.flags & GRAPH_RENDER_ACTIVE, 0);
        }
    }
    begin(ACT_IDLE);
    o->oRoom = 0;
    o->oDrawingDistance = 1000.0f;
    o->oFlags = flags | OBJ_FLAG_ACTIVE_FROM_AFAR;
    o->header.gfx.node.flags |= GRAPH_RENDER_ACTIVE;
    NEAR(cur_obj_drawing_distance(1050.0f), 1250.0f);
    cur_obj_update_distance_visibility(1050.0f, o->oFlags, MARIO_INSIDE_ROOM);
    EXPECT_EQ(o->header.gfx.node.flags & GRAPH_RENDER_ACTIVE, GRAPH_RENDER_ACTIVE);
    EXPECT_EQ(o->activeFlags & ACTIVE_FLAG_FAR_AWAY, 0);
    cur_obj_update_distance_visibility(1050.0f, o->oFlags, MARIO_OUTSIDE_ROOM);
    EXPECT_EQ(o->header.gfx.node.flags & GRAPH_RENDER_ACTIVE, 0);
    begin(ACT_IDLE);
    o->oRoom = -1;
    o->oDrawingDistance = 1000.0f;
    sVisibilityEnemy = FALSE;
    cur_obj_update_distance_visibility(990.0f, flags, MARIO_INSIDE_ROOM);
    cur_obj_update_distance_visibility(1050.0f, flags, MARIO_INSIDE_ROOM);
    EXPECT_EQ(o->header.gfx.node.flags & GRAPH_RENDER_ACTIVE, 0);
    sVisibilityEnemy = TRUE;
    o->oRoom = 0;
    cur_obj_update_distance_visibility(990.0f, flags, MARIO_INSIDE_ROOM);
    cur_obj_update_distance_visibility(1050.0f, flags, MARIO_OUTSIDE_ROOM);
    EXPECT_EQ(o->header.gfx.node.flags & GRAPH_RENDER_ACTIVE, 0);
    o->oRoom = -1;
    o->header.gfx.node.flags = 0;
    o->oHeldState = HELD_HELD;
    cur_obj_update_distance_visibility(990.0f, flags, MARIO_INSIDE_ROOM);
    EXPECT_EQ(o->header.gfx.node.flags & GRAPH_RENDER_ACTIVE, 0);
    o->oHeldState = HELD_FREE;
    o->collisionData = o;
    o->header.gfx.node.flags |= GRAPH_RENDER_ACTIVE;
    NEAR(cur_obj_drawing_distance(1050.0f), 1250.0f);
    o->header.gfx.node.flags = 0;
    NEAR(cur_obj_drawing_distance(1050.0f), 1000.0f);
    cur_obj_update_distance_visibility(990.0f, flags, MARIO_INSIDE_ROOM);
    EXPECT_EQ(o->header.gfx.node.flags & GRAPH_RENDER_ACTIVE, 0);
    o->collisionData = NULL;
    cur_obj_update_distance_visibility(2000.0f, flags | OBJ_FLAG_ACTIVE_FROM_AFAR, MARIO_INSIDE_ROOM);
    EXPECT_EQ(o->header.gfx.node.flags & GRAPH_RENDER_ACTIVE, 0);
    return 1;
}
static int camera_object_fade(void) {
    const float depths[] = { 0, 256, 320, 496, 672, 848, 1024, 1200 };
    const int opacity[] = { 0, 0, 0, 40, 128, 215, 255, 255 };
    begin(ACT_IDLE);
    for (unsigned i = 0; i < sizeof(depths) / sizeof(*depths); i++) {
        sObj.header.gfx.cameraToObject[2] = -depths[i];
        obj_set_opacity_from_cam_dist(&sObj);
        EXPECT_EQ(sObj.oOpacity, opacity[i]);
    }
    int previous = 0;
    for (int depth = 320; depth <= 1024; depth++) {
        sObj.header.gfx.cameraToObject[2] = -depth;
        obj_set_opacity_from_cam_dist(&sObj);
        EXPECT_EQ(sObj.oOpacity >= previous, TRUE);
        previous = sObj.oOpacity;
    }
    return 1;
}
static int wave_trails(void) {
    for (int parity = 0; parity < 2; parity++) {
        begin(ACT_BREASTSTROKE);
        o->parentObj = gMarioObject;
        o->oPosY = -200;
        gGlobalTimer = parity;
        bhv_wave_trail_shrink();
        EXPECT_EQ(sDeleted, 0);
        EXPECT_EQ(o->oSubAction, 1);
        NEAR(o->header.gfx.scale[0], 1.15f);
        o->oTimer = 1;
        bhv_wave_trail_shrink();
        NEAR(o->header.gfx.scale[0], 1.075f);
    }
    begin(ACT_WALKING);
    o->parentObj = gMarioObject;
    o->oPosY = 0;
    gGlobalTimer = 1;
    bhv_wave_trail_shrink();
    EXPECT_EQ(sDeleted, 1);
    begin(ACT_WALKING);
    m.flags |= MARIO_VANISH_CAP;
    m.waterLevel = 400;
    o->parentObj = gMarioObject;
    o->oPosY = -200;
    bhv_wave_trail_shrink();
    EXPECT_EQ(sDeleted, 0);
    NEAR(o->oPosY, 5);
    return 1;
}
static int snufit_walls(void) {
    for (s32 unreported = 0; unreported <= 1; unreported++) {
        for (s32 axes = 0; axes <= 3; axes++) {
            begin(ACT_IDLE);
            sPoleWalls = 0;
            sSnufitUnreportedWallPush = unreported;
            sSnufitWallAxes = axes;
            sSnufitWallLimit = 90.0f;
            o->oWallHitboxRadius = 30.0f;
            o->oDistanceToMario = 1000.0f;
            o->oHomeY = 400.0f;
            for (s32 frame = 0; frame < 400; frame++) {
                gGlobalTimer = frame;
                bhv_snufit_loop();
                if (axes & 1) {
                    EXPECT_EQ(o->oPosX <= 60.001f, TRUE);
                }
                if (axes & 2) {
                    EXPECT_EQ(o->oPosZ <= 60.001f, TRUE);
                }
                NEAR(o->oPosY, 400.0f + 8.0f * coss(4000 * gGlobalTimer));
                NEAR(o->oPosX, o->oHomeX + 100.0f * coss(o->oSnufitCircularPeriod));
                NEAR(o->oPosZ, o->oHomeZ + 100.0f * sins(o->oSnufitCircularPeriod));
                if (axes == 0) {
                    NEAR(o->oHomeX, 0.0f);
                    NEAR(o->oHomeZ, 0.0f);
                }
            }
            s32 queries = sPolePositionChecks;
            f32 x = o->oPosX;
            f32 z = o->oPosZ;
            o->activeFlags |= ACTIVE_FLAG_IN_DIFFERENT_ROOM;
            bhv_snufit_loop();
            EXPECT_EQ(sPolePositionChecks, queries);
            NEAR(o->oPosX, x);
            NEAR(o->oPosZ, z);
        }
    }
    begin(ACT_IDLE);
    sSnufitWallAxes = 1;
    o->oWallHitboxRadius = 30.0f;
    o->oDistanceToMario = 1000.0f;
    o->oPosX = 65.0f;
    o->oPosY = 400.0f;
    o->oGroundTwirlStunDuration = 8;
    bhv_snufit_loop();
    NEAR(o->oPosX, 60.0f);
    EXPECT_EQ(o->oGroundTwirlStunDuration, 0);
    bhv_snufit_loop();
    EXPECT_EQ(o->oPosX <= 60.001f, TRUE);
    sSnufitWallAxes = 0;
    return 1;
}

static int snufit_resume(void) {
    for (int cycle = 0; cycle < 3; cycle++) {
        begin(ACT_IDLE);
        o->oPosX = 1000 + cycle * 10;
        o->oPosY = 400;
        o->oPosZ = -300;
        o->oDistanceToMario = 1000;
        o->oSnufitCircularPeriod = cycle * 17000;
        o->oGroundTwirlStunDuration = 8;
        gGlobalTimer = cycle * 100;
        bhv_snufit_loop();
        NEAR(o->oPosX, 1000 + cycle * 10);
        NEAR(o->oPosY, 400);
        NEAR(o->oPosZ, -300);
        EXPECT_EQ(o->oGroundTwirlStunDuration, 0);
        f32 home = o->oHomeX;
        bhv_snufit_loop();
        NEAR(o->oHomeX, home);
    }
    begin(ACT_IDLE);
    o->oGroundTwirlStunDuration = 8;
    o->oPosX = 1000;
    o->oInteractStatus = INT_STATUS_WAS_ATTACKED;
    bhv_snufit_loop();
    NEAR(o->oHomeX, 0);
    EXPECT_EQ(o->oGroundTwirlStunDuration, 0);
    return 1;
}
static int wave_spacing(void) {
    begin(ACT_BREASTSTROKE);
    m.pos[1] = -200;
    update_mario_submerged_effects(&m);
    EXPECT_EQ(m.particleFlags & PARTICLE_WAVE_TRAIL, 0);
    for (int trail = 0; trail < 3; trail++) {
        m.particleFlags = 0;
        m.pos[0] = 100 * trail + 99;
        update_mario_submerged_effects(&m);
        EXPECT_EQ(m.particleFlags & PARTICLE_WAVE_TRAIL, 0);
        EXPECT_EQ(m.particleFlags & PARTICLE_IDLE_WATER_WAVE, PARTICLE_IDLE_WATER_WAVE);
        m.particleFlags = 0;
        m.pos[0] = 100 * (trail + 1);
        update_mario_submerged_effects(&m);
        EXPECT_EQ(m.particleFlags & PARTICLE_WAVE_TRAIL, PARTICLE_WAVE_TRAIL);
    }
    m.action = ACT_WALKING;
    update_mario_submerged_effects(&m);
    m.action = ACT_BREASTSTROKE;
    m.pos[0] = 1000;
    m.particleFlags = 0;
    update_mario_submerged_effects(&m);
    EXPECT_EQ(m.particleFlags & PARTICLE_WAVE_TRAIL, 0);
    // Depth cutoff preserves the static ripple for swimmers and Vanish land actions.
    for (s32 vanish = FALSE; vanish <= TRUE; vanish++) {
        for (s32 depth = 499; depth <= 501; depth++) {
            begin(vanish ? ACT_WALKING : ACT_BREASTSTROKE);
            if (vanish) {
                m.flags |= MARIO_VANISH_CAP;
            }
            m.waterLevel = 1000;
            m.pos[1] = m.waterLevel - depth;
            update_mario_submerged_effects(&m);
            for (s32 travel = 1; travel <= 3; travel++) {
                m.pos[0] += 100.0f;
                m.particleFlags = 0;
                update_mario_submerged_effects(&m);
                EXPECT_EQ((m.particleFlags & PARTICLE_WAVE_TRAIL) != 0, depth < 500);
                EXPECT_EQ(m.particleFlags & PARTICLE_IDLE_WATER_WAVE, PARTICLE_IDLE_WATER_WAVE);
            }
            if (depth >= 500) {
                m.pos[1] = m.waterLevel - 499;
                m.particleFlags = 0;
                update_mario_submerged_effects(&m);
                EXPECT_EQ(m.particleFlags & PARTICLE_WAVE_TRAIL, 0);
                m.pos[0] += 99.0f;
                m.particleFlags = 0;
                update_mario_submerged_effects(&m);
                EXPECT_EQ(m.particleFlags & PARTICLE_WAVE_TRAIL, 0);
                m.pos[0] += 1.0f;
                m.particleFlags = 0;
                update_mario_submerged_effects(&m);
                EXPECT_EQ(m.particleFlags & PARTICLE_WAVE_TRAIL, PARTICLE_WAVE_TRAIL);
            }
        }
    }
    return 1;
}
static int buddy_talk_sound(void) {
    s16 angles[] = { 0, 0x4000, -0x8000 };
    for (unsigned angle = 0; angle < 3; angle++) {
        begin(ACT_IDLE);
        for (int conversation = 0; conversation < 2; conversation++) {
            o->oAction = BOBOMB_BUDDY_ACT_TURN_TO_TALK;
            o->oMoveAngleYaw = angles[angle];
            o->oAngleToMario = 0;
            o->oTimer = 0;
            while (o->oAction == BOBOMB_BUDDY_ACT_TURN_TO_TALK) {
                bobomb_buddy_act_turn_to_talk();
                o->oTimer++;
                if (o->oTimer > 8) {
                    fprintf(stderr, "Buddy turn did not finish\n");
                    return 0;
                }
            }
            EXPECT_EQ(sTalkSounds, conversation + 1);
        }
    }
    begin(ACT_IDLE);
    o->oMoveAngleYaw = -0x8000;
    o->oAngleToMario = 0;
    o->header.gfx.animInfo.animFrame = 5;
    bobomb_buddy_act_turn_to_talk();
    o->oTimer++;
    o->header.gfx.animInfo.animFrame = 16;
    bobomb_buddy_act_turn_to_talk();
    EXPECT_EQ(sTalkSounds, 1);
    EXPECT_EQ(sFootsteps, 2);
    // An interrupted turn may start a later conversation with a fresh entry timer.
    o->oAction = BOBOMB_BUDDY_ACT_TURN_TO_TALK;
    o->oTimer = 0;
    bobomb_buddy_act_turn_to_talk();
    EXPECT_EQ(sTalkSounds, 2);
    return 1;
}
static int dab_buttons(u16 pressed, u16 down) {
    sController.buttonPressed = pressed;
    sController.buttonDown = down;
    m.input = (down & Z_TRIG ? INPUT_Z_DOWN : 0) | (pressed & Z_TRIG ? INPUT_Z_PRESSED : 0);
    return mario_dab_update_shortcut(&m);
}
static int dab_input_sound(void) {
    for (int backward = 0; backward < 2; backward++) {
        begin(ACT_FREEFALL_LAND_STOP);
        m.prevAction = backward ? ACT_BACKWARD_ROLLOUT : ACT_FORWARD_ROLLOUT;
        EXPECT_EQ(dab_buttons(L_TRIG, L_TRIG), FALSE);
        EXPECT_EQ(dab_buttons(Z_TRIG, L_TRIG | Z_TRIG), TRUE);
        EXPECT_EQ(m.action, ACT_DAB);
    }
    begin(ACT_FREEFALL_LAND_STOP);
    m.prevAction = ACT_FREEFALL;
    EXPECT_EQ(dab_buttons(L_TRIG, L_TRIG), FALSE);
    EXPECT_EQ(dab_buttons(Z_TRIG, L_TRIG | Z_TRIG), FALSE);

    begin(ACT_IDLE);
    m.flags |= MARIO_MARIO_SOUND_PLAYED;
    EXPECT_EQ(dab_buttons(L_TRIG, L_TRIG), FALSE);
    EXPECT_EQ(sDabVoices, 0);
    EXPECT_EQ(dab_buttons(Z_TRIG, L_TRIG | Z_TRIG), TRUE);
    EXPECT_EQ(m.action, ACT_DAB);
    EXPECT_EQ(sDabVoices, 1);
    EXPECT_EQ(sMarioObject.header.gfx.animInfo.animID, -1);
    sController.buttonPressed = 0;
    for (int frame = 0; frame <= 7; frame++) {
        mario_dab_action(&m);
        EXPECT_EQ(sDabVoices, 1);
        sMarioObject.header.gfx.animInfo.animFrame = frame + 1;
    }
    EXPECT_EQ(m.actionState, DAB_STATE_ANALOG);
    for (int tick = 0; tick < 10; tick++) {
        EXPECT_EQ(mario_dab_update_shortcut(&m), FALSE);
        mario_dab_action(&m);
        EXPECT_EQ(sDabVoices, 1);
    }
    return 1;
}
static int dab_inverse_analog(void) {
    begin(ACT_IDLE);
    dab_buttons(L_TRIG, L_TRIG);
    dab_buttons(Z_TRIG, L_TRIG | Z_TRIG);
    sController.buttonPressed = 0;
    for (int frame = 0; frame < 7; frame++) {
        sMarioObject.header.gfx.animInfo.animFrame = frame;
        mario_dab_action(&m);
        EXPECT_EQ(m.actionState, DAB_STATE_ENTER);
    }
    sMarioObject.header.gfx.animInfo.animFrame = 7;
    mario_dab_action(&m);
    EXPECT_EQ(m.actionState, DAB_STATE_ANALOG);
    EXPECT_EQ(sMarioObject.header.gfx.animInfo.animFrame, DAB_ANALOG_START_FRAME);
    sIdleCancelAction = ACT_WALKING;
    sIdleCancelMask = INPUT_NONZERO_ANALOG;
    const int magnitudes[] = { 0, 8, 16, 32, 48, 64, 48, 32, 16, 0 };
    const int samples[] = { 0, 8, 16, 32, 47, 63, 47, 32, 16, 0 };
    const s16 headings[] = { 0, 0x4000, -0x8000 };
    for (int cycle = 0; cycle < 3; cycle++) {
        m.faceAngle[1] = m.intendedYaw = headings[cycle];
        for (unsigned i = 0; i < sizeof(magnitudes) / sizeof(magnitudes[0]); i++) {
            sController.stickMag = magnitudes[i];
            m.input = INPUT_Z_DOWN | (magnitudes[i] ? INPUT_NONZERO_ANALOG : 0);
            u16 saved = m.input;
            EXPECT_EQ(mario_dab_action(&m), FALSE);
            EXPECT_EQ(m.action, ACT_DAB);
            EXPECT_EQ(m.input, saved);
            EXPECT_EQ(m.faceAngle[1], headings[cycle]);
            EXPECT_EQ(sMarioObject.header.gfx.animInfo.animFrame, DAB_ANALOG_START_FRAME + samples[i]);
            EXPECT_EQ(sDabVoices, 1);
            EXPECT_EQ(sDab.count, 1);
        }
    }
    m.faceAngle[1] = 0;
    m.intendedYaw = 0x2000;
    sController.stickMag = 64;
    m.input = INPUT_NONZERO_ANALOG | INPUT_Z_DOWN;
    mario_dab_action(&m);
    EXPECT_EQ(sMarioObject.header.gfx.animInfo.animFrame, DAB_ANALOG_START_FRAME + 45);
    // Releasing Z with a neutral stick settles to the existing held breathing loop.
    m.intendedYaw = 0;
    mario_dab_action(&m);
    EXPECT_EQ(sDab.analogFrame, 63);
    sController.buttonDown = L_TRIG;
    m.input = 0;
    for (int tick = 0; tick < 4; tick++) {
        mario_dab_action(&m);
        EXPECT_EQ(m.actionState, DAB_STATE_ANALOG_SETTLE);
        EXPECT_EQ(sDab.analogFrame, MAX(0, 63 - 16 * (tick + 1)));
    }
    mario_dab_action(&m);
    EXPECT_EQ(m.actionState, DAB_STATE_HOLD);
    EXPECT_EQ(sMarioObject.header.gfx.animInfo.animFrame, DAB_HOLD_START_FRAME);
    // L alone cannot consume movement input.
    m.input = INPUT_NONZERO_ANALOG;
    EXPECT_EQ(mario_dab_action(&m), TRUE);
    EXPECT_EQ(m.action, ACT_WALKING);
    // Nonforward input keeps the full dab instead of canceling into movement.
    const s16 nonforward[] = { -0x8000, -0x4000 };
    for (unsigned i = 0; i < 2; i++) {
        begin(ACT_DAB);
        m.actionState = DAB_STATE_HOLD;
        m.actionArg = ACT_IDLE;
        sController.buttonDown = L_TRIG | Z_TRIG;
        sController.stickMag = 64;
        m.intendedYaw = nonforward[i];
        m.input = INPUT_NONZERO_ANALOG | INPUT_Z_DOWN;
        sIdleCancelAction = ACT_WALKING;
        sIdleCancelMask = INPUT_NONZERO_ANALOG;
        EXPECT_EQ(mario_dab_action(&m), FALSE);
        EXPECT_EQ(m.action, ACT_DAB);
        EXPECT_EQ(sMarioObject.header.gfx.animInfo.animFrame, DAB_ANALOG_START_FRAME);
    }
    // Release both buttons from the relaxed pose, then recover normally.
    begin(ACT_DAB);
    m.actionState = DAB_STATE_HOLD;
    m.actionArg = ACT_IDLE;
    sController.buttonDown = L_TRIG | Z_TRIG;
    sController.stickMag = 64;
    m.input = INPUT_NONZERO_ANALOG;
    mario_dab_action(&m);
    sController.buttonDown = 0;
    m.input = 0;
    for (int tick = 0; tick < 5; tick++) {
        mario_dab_action(&m);
    }
    EXPECT_EQ(m.actionState, DAB_STATE_RECOVER);
    EXPECT_EQ(sMarioObject.header.gfx.animInfo.animFrame, DAB_RECOVERY_FRAME);
    sMarioObject.header.gfx.animInfo.animFrame = DAB_RECOVERY_END_FRAME;
    EXPECT_EQ(mario_dab_action(&m), TRUE);
    EXPECT_EQ(m.action, ACT_IDLE);
    // Stomp and losing the floor keep their ordinary exit priority.
    const u16 exits[] = { INPUT_STOMPED, INPUT_OFF_FLOOR };
    for (unsigned i = 0; i < 2; i++) {
        begin(ACT_DAB);
        m.actionState = DAB_STATE_HOLD;
        m.actionArg = ACT_IDLE;
        sController.buttonDown = L_TRIG | Z_TRIG;
        sController.stickMag = 64;
        m.input = INPUT_NONZERO_ANALOG | INPUT_Z_DOWN | exits[i];
        sIdleCancelAction = ACT_JUMP;
        sIdleCancelMask = exits[i];
        EXPECT_EQ(mario_dab_action(&m), TRUE);
        EXPECT_EQ(m.action, ACT_JUMP);
        EXPECT_EQ(m.input, INPUT_NONZERO_ANALOG | INPUT_Z_DOWN | exits[i]);
    }
    begin(ACT_DAB);
    m.actionState = DAB_STATE_HOLD;
    m.actionArg = ACT_IDLE;
    sController.buttonDown = L_TRIG | Z_TRIG;
    sStep = GROUND_STEP_LEFT_GROUND;
    EXPECT_EQ(mario_dab_action(&m), TRUE);
    EXPECT_EQ(m.action, ACT_FREEFALL);
    // Neutral entry keeps the coasting state even once the stick controls the pose.
    begin(ACT_DECELERATING);
    m.forwardVel = 8;
    dab_buttons(L_TRIG, L_TRIG);
    EXPECT_EQ(dab_buttons(Z_TRIG, L_TRIG | Z_TRIG), TRUE);
    sMarioObject.header.gfx.animInfo.animFrame = 7;
    sController.buttonPressed = 0;
    sController.stickMag = 64;
    m.input = INPUT_NONZERO_ANALOG | INPUT_Z_DOWN;
    mario_dab_action(&m);
    NEAR(m.forwardVel, 8 - GROUND_NEUTRAL_COAST_DECEL);
    EXPECT_EQ(m.actionArg, ACT_DECELERATING);
    EXPECT_EQ(sDabVoices, 1);
    // Fresh Z presses repeat normally from control and from release settling.
    const u16 repeatStates[] = { DAB_STATE_ANALOG, DAB_STATE_ANALOG_SETTLE };
    for (unsigned i = 0; i < 2; i++) {
        m.actionState = repeatStates[i];
        dab_buttons(0, L_TRIG);
        EXPECT_EQ(dab_buttons(Z_TRIG, L_TRIG | Z_TRIG), TRUE);
        EXPECT_EQ(m.actionState, DAB_STATE_ENTER);
        EXPECT_EQ(sDab.count, i + 2);
        EXPECT_EQ(sDabVoices, i + 2);
        EXPECT_EQ(sMarioObject.header.gfx.animInfo.animFrame, DAB_REPEAT_START_FRAME);
        EXPECT_EQ(m.actionArg, ACT_DECELERATING);
    }
    // Letting go of L while Z stays held restores its ordinary crouch input.
    m.actionState = DAB_STATE_ANALOG;
    sController.buttonDown = Z_TRIG;
    m.input = INPUT_Z_DOWN;
    sIdleCancelAction = ACT_START_CROUCHING;
    sIdleCancelMask = INPUT_Z_DOWN;
    EXPECT_EQ(mario_dab_action(&m), TRUE);
    EXPECT_EQ(m.action, ACT_START_CROUCHING);
    return 1;
}
static int dab_analog_cancel_lock(void) {
    const u16 phases[] = { DAB_STATE_ENTER, DAB_STATE_HOLD, DAB_STATE_ANALOG };
    const u16 buttons[] = { 0, INPUT_A_PRESSED, INPUT_B_PRESSED, INPUT_FIRST_PERSON,
                            INPUT_A_PRESSED | INPUT_B_PRESSED | INPUT_FIRST_PERSON };
    const s16 directions[] = { 0, 0x2000, 0x4000, 0x6000, -0x8000, -0x6000, -0x4000, -0x2000 };
    const int samples[] = { 63, 45, 0, 0, 0, 0, 0, 45 };
    for (int cycle = 0; cycle < 3; cycle++) {
        for (unsigned phase = 0; phase < sizeof(phases) / sizeof(*phases); phase++) {
            for (unsigned direction = 0; direction < sizeof(directions) / sizeof(*directions);
                 direction++) {
                for (unsigned button = 0; button < sizeof(buttons) / sizeof(*buttons); button++) {
                    begin(ACT_IDLE);
                    dab_buttons(L_TRIG, L_TRIG);
                    EXPECT_EQ(dab_buttons(Z_TRIG, L_TRIG | Z_TRIG), TRUE);
                    sRealDabIdleCancels = TRUE;
                    sController.buttonPressed = 0;
                    sController.stickMag = 64;
                    m.actionState = phases[phase];
                    set_mario_animation(&m, MARIO_ANIM_DAB);
                    sMarioObject.header.gfx.animInfo.animFrame = phase ? DAB_POSE_FRAME : 3;
                    m.faceAngle[1] = cycle * 0x3000;
                    m.intendedYaw = (s16) (m.faceAngle[1] + directions[direction]);
                    m.input = INPUT_Z_DOWN | INPUT_NONZERO_ANALOG | buttons[button];
                    u16 input = m.input;
                    EXPECT_EQ(mario_dab_action(&m), FALSE);
                    EXPECT_EQ(m.action, ACT_DAB);
                    EXPECT_EQ(m.input, input);
                    EXPECT_EQ(m.faceAngle[1], cycle * 0x3000);
                    NEAR(m.forwardVel, 0);
                    EXPECT_EQ(sDabVoices, 1);
                    EXPECT_EQ(sDab.count, 1);
                    EXPECT_EQ(sMarioObject.header.gfx.animInfo.animFrame,
                              phase ? DAB_ANALOG_START_FRAME + samples[direction] : 3);
                }
            }
        }
    }
    // Letting go of either button restores native voluntary controls immediately.
    const u16 released[] = { L_TRIG, Z_TRIG, 0 };
    const u16 inputs[] = { INPUT_A_PRESSED, INPUT_B_PRESSED, INPUT_FIRST_PERSON, INPUT_NONZERO_ANALOG };
    const u32 actions[] = { ACT_JUMP, ACT_PUNCHING, ACT_FIRST_PERSON, ACT_WALKING };
    for (unsigned release = 0; release < sizeof(released) / sizeof(*released); release++) {
        for (unsigned input = 0; input < sizeof(inputs) / sizeof(*inputs); input++) {
            begin(ACT_DAB);
            sRealDabIdleCancels = TRUE;
            m.actionState = DAB_STATE_ANALOG;
            m.actionArg = ACT_IDLE;
            m.zHeldFrames = 2;
            sController.buttonDown = released[release];
            sController.stickMag = 64;
            m.input = inputs[input] | ((released[release] & Z_TRIG) ? INPUT_Z_DOWN : 0);
            u16 saved = m.input;
            EXPECT_EQ(mario_dab_action(&m), TRUE);
            EXPECT_EQ(m.action, actions[input]);
            EXPECT_EQ(m.input, saved);
        }
    }
    // Native forced exits retain priority even with every voluntary cancel held.
    const u16 forced[] = { INPUT_STOMPED, INPUT_OFF_FLOOR, INPUT_ABOVE_SLIDE };
    const u32 forcedActions[] = { ACT_SHOCKWAVE_BOUNCE, ACT_FREEFALL, ACT_BEGIN_SLIDING };
    for (unsigned input = 0; input < sizeof(forced) / sizeof(*forced); input++) {
        begin(ACT_DAB);
        sRealDabIdleCancels = TRUE;
        m.actionState = DAB_STATE_ANALOG;
        sController.buttonDown = L_TRIG | Z_TRIG;
        m.input = forced[input] | INPUT_A_PRESSED | INPUT_B_PRESSED | INPUT_FIRST_PERSON
                  | INPUT_NONZERO_ANALOG | INPUT_Z_DOWN;
        u16 saved = m.input;
        EXPECT_EQ(mario_dab_action(&m), TRUE);
        EXPECT_EQ(m.action, forcedActions[input]);
        EXPECT_EQ(m.input, saved);
    }
    return 1;
}
static int dab_repeat_sound(void) {
    begin(ACT_IDLE);
    dab_buttons(L_TRIG, L_TRIG);
    dab_buttons(Z_TRIG, L_TRIG | Z_TRIG);
    u32 states[] = { DAB_STATE_ENTER, DAB_STATE_HOLD, DAB_STATE_SETTLE, DAB_STATE_RECOVER };
    for (unsigned repeat = 0; repeat < 4; repeat++) {
        m.actionState = states[repeat];
        m.actionArg = ACT_DECELERATING;
        EXPECT_EQ(dab_buttons(0, L_TRIG), FALSE);
        EXPECT_EQ(dab_buttons(Z_TRIG, L_TRIG | Z_TRIG), TRUE);
        EXPECT_EQ(sDabVoices, repeat + 2);
        EXPECT_EQ(m.actionArg, ACT_DECELERATING);
        EXPECT_EQ(sMarioObject.header.gfx.animInfo.animFrame, DAB_REPEAT_START_FRAME);
        sController.buttonPressed = 0;
        mario_dab_action(&m);
        EXPECT_EQ(sDabVoices, repeat + 2);
    }
    sIdleCancelAction = ACT_WALKING;
    EXPECT_EQ(mario_dab_action(&m), TRUE);
    EXPECT_EQ(m.action, ACT_WALKING);
    EXPECT_EQ(sDabVoices, 5);
    sIdleCancelAction = 0;
    dab_buttons(L_TRIG, L_TRIG);
    EXPECT_EQ(dab_buttons(Z_TRIG, L_TRIG | Z_TRIG), TRUE);
    EXPECT_EQ(sDabVoices, 6);
    return 1;
}
static int dab_rejected_sound(void) {
    begin(ACT_IDLE);
    EXPECT_EQ(dab_buttons(L_TRIG | Z_TRIG, L_TRIG | Z_TRIG), FALSE);
    EXPECT_EQ(sDabVoices, 0);
    begin(ACT_IDLE);
    dab_buttons(L_TRIG, L_TRIG);
    dab_buttons(0, L_TRIG);
    EXPECT_EQ(dab_buttons(Z_TRIG, L_TRIG | Z_TRIG), FALSE);
    EXPECT_EQ(sDabVoices, 0);
    begin(ACT_IDLE);
    dab_buttons(L_TRIG, L_TRIG);
    sController.buttonPressed = Z_TRIG;
    sController.buttonDown = L_TRIG | Z_TRIG;
    m.input = INPUT_NONZERO_ANALOG;
    EXPECT_EQ(mario_dab_update_shortcut(&m), FALSE);
    EXPECT_EQ(sDabVoices, 0);
    begin(ACT_FREEFALL);
    EXPECT_EQ(dab_buttons(L_TRIG, L_TRIG), FALSE);
    EXPECT_EQ(dab_buttons(Z_TRIG, L_TRIG | Z_TRIG), FALSE);
    EXPECT_EQ(sDabVoices, 0);
    return 1;
}
static int cap_pose_five_frame_windows(void) {
    begin(ACT_DAB);
    m.flags = MARIO_NORMAL_CAP | MARIO_CAP_ON_HEAD;
    sController.buttonDown = L_TRIG;
    for (int completed = 0; completed < 5; completed++) {
        EXPECT_EQ(mario_dab_loses_cap_on_damage(&m), completed >= 5);
        mario_dab_action(&m);
    }
    EXPECT_EQ(mario_dab_loses_cap_on_damage(&m), TRUE);
    sController.buttonDown = 0;
    EXPECT_EQ(mario_dab_loses_cap_on_damage(&m), FALSE);
    mario_dab_reset();
    EXPECT_EQ(mario_dab_loses_cap_on_damage(&m), FALSE);

    begin(ACT_GROUND_TWIRL);
    m.actionTimer = GROUND_TWIRL_FRAMES;
    sGroundTwirl.lHeldThroughout = TRUE;
    sController.buttonDown = L_TRIG;
    for (int completed = 0; completed < 5; completed++) {
        sGroundTwirl.heldPoseFrames = completed;
        EXPECT_EQ(mario_ground_twirl_loses_cap_on_damage(&m), FALSE);
    }
    sGroundTwirl.heldPoseFrames = 5;
    EXPECT_EQ(mario_ground_twirl_loses_cap_on_damage(&m), TRUE);
    m.input = INPUT_NONZERO_ANALOG;
    EXPECT_EQ(mario_ground_twirl_loses_cap_on_damage(&m), FALSE);
    m.input = 0;
    sController.buttonDown = 0;
    EXPECT_EQ(mario_ground_twirl_loses_cap_on_damage(&m), FALSE);
    return 1;
}
static int spin_dizzy_cap_loss(void) {
    const int fights[] = { LEVEL_BOWSER_1, LEVEL_BOWSER_2, LEVEL_BOWSER_3 };
    for (int fight = 0; fight < 3; fight++) {
        begin(ACT_WALKING);
        gCurrLevelNum = fights[fight];
        m.flags = MARIO_NORMAL_CAP | MARIO_CAP_ON_HEAD;
        sController.stickMag = 60;
        m.input = INPUT_NONZERO_ANALOG;
        mario_spin_dizzy_update(&m);
        for (int step = 0; step < 24; step++) {
            m.faceAngle[1] += 0x2000;
            mario_spin_dizzy_update(&m);
        }
        sController.stickMag = 0;
        m.input = 0;
        mario_spin_dizzy_update(&m);
        EXPECT_EQ(m.action, ACT_WALKING);
        EXPECT_EQ(sDizzyCapBlows, 0);
        EXPECT_EQ(m.flags & MARIO_NORMAL_CAP, MARIO_NORMAL_CAP);
    }

    for (int direction = -1; direction <= 1; direction += 2) {
        begin(ACT_WALKING);
        m.flags = MARIO_NORMAL_CAP | MARIO_CAP_ON_HEAD;
        sController.stickMag = 60;
        m.input = INPUT_NONZERO_ANALOG;
        mario_spin_dizzy_update(&m);
        for (int step = 1; step <= 24; step++) {
            m.faceAngle[1] += (s16) (direction * 0x2000);
            mario_spin_dizzy_update(&m);
            EXPECT_EQ(m.action, ACT_WALKING);
            EXPECT_EQ(sDizzyCapBlows, 0);
        }
        sController.stickMag = 0;
        m.input = 0;
        mario_spin_dizzy_update(&m);
        EXPECT_EQ(m.action, ACT_SPIN_DIZZY);
        EXPECT_EQ(sDizzyCapBlows, 1);
        EXPECT_EQ(m.flags & MARIO_NORMAL_CAP, 0);
        mario_spin_dizzy_update(&m);
        EXPECT_EQ(sDizzyCapBlows, 1);
    }

    // A completed spin without safe ground behind Mario does not topple him.
    begin(ACT_WALKING);
    m.flags = MARIO_NORMAL_CAP | MARIO_CAP_ON_HEAD;
    sController.stickMag = 60;
    m.input = INPUT_NONZERO_ANALOG;
    mario_spin_dizzy_update(&m);
    for (int step = 0; step < 24; step++) {
        m.faceAngle[1] += 0x2000;
        mario_spin_dizzy_update(&m);
    }
    sDizzyFloorBehind = 100;
    m.input = 0;
    sController.stickMag = 0;
    mario_spin_dizzy_update(&m);
    EXPECT_EQ(m.action, ACT_WALKING);
    EXPECT_EQ(sDizzyCapBlows, 0);
    EXPECT_EQ(m.flags & MARIO_NORMAL_CAP, MARIO_NORMAL_CAP);

    // Leaving locomotion early cancels the rotation history.
    begin(ACT_WALKING);
    m.flags = MARIO_NORMAL_CAP | MARIO_CAP_ON_HEAD;
    sController.stickMag = 60;
    m.input = INPUT_NONZERO_ANALOG;
    mario_spin_dizzy_update(&m);
    for (int step = 0; step < 12; step++) {
        m.faceAngle[1] += 0x2000;
        mario_spin_dizzy_update(&m);
    }
    m.action = ACT_JUMP;
    mario_spin_dizzy_update(&m);
    m.action = ACT_WALKING;
    m.input = 0;
    sController.stickMag = 0;
    mario_spin_dizzy_update(&m);
    EXPECT_EQ(m.action, ACT_WALKING);
    EXPECT_EQ(sDizzyCapBlows, 0);
    return 1;
}
static int dab_sleep_input_lock(void) {
    static struct TestBodyState body;
    EXPECT_EQ(DAB_SLEEP_INPUT_LOCK_FRAMES, 110);
    const u32 voluntary[] = { INPUT_A_PRESSED, INPUT_B_PRESSED, INPUT_Z_DOWN, INPUT_NONZERO_ANALOG,
                              INPUT_FIRST_PERSON };
    for (int capless = 0; capless < 2; capless++) {
        begin(ACT_IDLE);
        if (capless) {
            m.flags &= ~MARIO_CAP_ON_HEAD;
        }
        EXPECT_EQ(dab_buttons(L_TRIG, L_TRIG), FALSE);
        EXPECT_EQ(dab_buttons(Z_TRIG, L_TRIG | Z_TRIG), TRUE);
        sDab.count = (capless ? DAB_SLEEP_CAPLESS_COUNT : DAB_SLEEP_WITH_CAP_COUNT) - 1;
        EXPECT_EQ(dab_buttons(0, L_TRIG), FALSE);
        EXPECT_EQ(dab_buttons(Z_TRIG, L_TRIG | Z_TRIG), TRUE);
        m.actionState = DAB_STATE_ENTER;
        sMarioObject.header.gfx.animInfo.animFrame = DAB_POSE_FRAME;
        m.input = 0;
        EXPECT_EQ(mario_dab_action(&m), TRUE);
        EXPECT_EQ(m.action, ACT_START_SLEEPING);
        EXPECT_EQ(m.actionArg, DAB_SLEEP_INPUT_LOCK_FRAMES);
    }
    for (int cycle = 0; cycle < 3; cycle++) {
        for (unsigned i = 0; i < sizeof(voluntary) / sizeof(*voluntary); i++) {
            begin(ACT_START_SLEEPING);
            m.actionArg = DAB_SLEEP_INPUT_LOCK_FRAMES;
            m.marioBodyState = &body;
            m.input = voluntary[i];
            m.zCrouchBuffered = TRUE;
            sIdleCancelAction = ACT_JUMP;
            sIdleCancelMask = voluntary[i];
            for (int frame = 0; frame < DAB_SLEEP_INPUT_LOCK_FRAMES; frame++) {
                EXPECT_EQ(act_start_sleeping(&m), FALSE);
                EXPECT_EQ(m.action, ACT_START_SLEEPING);
                EXPECT_EQ(m.actionArg, DAB_SLEEP_INPUT_LOCK_FRAMES - frame - 1);
                EXPECT_EQ(m.zCrouchBuffered, FALSE);
            }
            EXPECT_EQ(act_start_sleeping(&m), TRUE);
            EXPECT_EQ(m.action, ACT_JUMP);
        }
    }
    begin(ACT_START_SLEEPING);
    m.actionArg = DAB_SLEEP_INPUT_LOCK_FRAMES;
    m.marioBodyState = &body;
    m.input = INPUT_OFF_FLOOR;
    sIdleCancelAction = ACT_FREEFALL;
    sIdleCancelMask = INPUT_OFF_FLOOR;
    EXPECT_EQ(act_start_sleeping(&m), TRUE);
    EXPECT_EQ(m.action, ACT_FREEFALL);
    begin(ACT_START_SLEEPING);
    m.actionArg = 0;
    m.marioBodyState = &body;
    m.input = INPUT_A_PRESSED;
    sIdleCancelAction = ACT_JUMP;
    sIdleCancelMask = INPUT_A_PRESSED;
    EXPECT_EQ(act_start_sleeping(&m), TRUE);
    EXPECT_EQ(m.action, ACT_JUMP);

    // If the sleep-start animation ends early, the unused lock carries forward.
    begin(ACT_START_SLEEPING);
    m.actionArg = DAB_SLEEP_INPUT_LOCK_FRAMES;
    m.marioBodyState = &body;
    m.actionState = ACT_STATE_START_SLEEPING_SLEEP;
    m.input = INPUT_A_PRESSED;
    sIdleCancelAction = ACT_JUMP;
    sIdleCancelMask = INPUT_A_PRESSED;
    EXPECT_EQ(act_start_sleeping(&m), TRUE);
    EXPECT_EQ(m.action, ACT_SLEEPING);
    EXPECT_EQ(m.actionArg, DAB_SLEEP_INPUT_LOCK_FRAMES);
    EXPECT_EQ(act_sleeping(&m), FALSE);
    EXPECT_EQ(m.actionArg, DAB_SLEEP_INPUT_LOCK_FRAMES - 1);
    for (int frame = 1; frame < DAB_SLEEP_INPUT_LOCK_FRAMES; frame++) {
        EXPECT_EQ(act_sleeping(&m), FALSE);
        EXPECT_EQ(m.action, ACT_SLEEPING);
    }
    EXPECT_EQ(m.actionArg, 0);
    EXPECT_EQ(act_sleeping(&m), TRUE);
    EXPECT_EQ(m.action, ACT_WAKING_UP);
    return 1;
}
static int capless_wake_playback(void) {
    const u32 caps[] = { 0, MARIO_NORMAL_CAP, MARIO_WING_CAP, MARIO_METAL_CAP, MARIO_VANISH_CAP };
    for (int cycle = 0; cycle < 3; cycle++) {
        for (int lying = 0; lying < 2; lying++) {
            for (unsigned cap = 0; cap < sizeof(caps) / sizeof(caps[0]); cap++) {
                begin(ACT_WAKING_UP);
                m.flags = caps[cap];
                m.actionArg = lying ? ACT_SLEEPING_STATE_LYING : ACT_SLEEPING_STATE_IDLE;
                m.input = INPUT_A_PRESSED | INPUT_B_PRESSED | INPUT_NONZERO_ANALOG;
                sWakeNoiseRestores = 0;
                int updates = cap == 0 ? 40 : 20;
                for (int frame = 0; frame < updates; frame++) {
                    if (frame == 5) {
                        m.flags = cap == 0 ? MARIO_NORMAL_CAP : 0;
                    }
                    sAnimAccel = -1;
                    EXPECT_EQ(act_waking_up(&m), FALSE);
                    EXPECT_EQ(m.action, ACT_WAKING_UP);
                    EXPECT_EQ(sMarioObject.header.gfx.animInfo.animID,
                              lying ? MARIO_ANIM_WAKE_FROM_LYING : MARIO_ANIM_WAKE_FROM_SLEEP);
                    EXPECT_EQ(sAnimAccel, cap == 0 ? 0x8000 : -1);
                    EXPECT_EQ(sStoppedSurfaceVoices, 3);
                    EXPECT_EQ(sWakeNoiseRestores, 1);
                }
                EXPECT_EQ(act_waking_up(&m), TRUE);
                EXPECT_EQ(m.action, ACT_IDLE);
                EXPECT_EQ(sStoppedSurfaceVoices, 3);
                EXPECT_EQ(sWakeNoiseRestores, 1);
            }
        }
    }
    const u32 inputs[] = { INPUT_STOMPED, INPUT_OFF_FLOOR, INPUT_ABOVE_SLIDE };
    const u32 actions[] = { ACT_SHOCKWAVE_BOUNCE, ACT_FREEFALL, ACT_BEGIN_SLIDING };
    for (int capless = 0; capless < 2; capless++) {
        for (int late = 0; late < 2; late++) {
            for (unsigned input = 0; input < sizeof(inputs) / sizeof(inputs[0]); input++) {
                begin(ACT_WAKING_UP);
                m.flags = capless ? 0 : MARIO_NORMAL_CAP;
                sWakeNoiseRestores = 0;
                if (late) {
                    EXPECT_EQ(act_waking_up(&m), FALSE);
                }
                m.input = inputs[input];
                EXPECT_EQ(act_waking_up(&m), TRUE);
                EXPECT_EQ(m.action, actions[input]);
                EXPECT_EQ(sStoppedSurfaceVoices, 3);
                EXPECT_EQ(sWakeNoiseRestores, 1);
            }
        }
    }
    return TRUE;
}

static int cap_pickup_lock(void) {
    for (int cycle = 0; cycle < 3; cycle++) {
        begin(ACT_WALKING);
        m.flags = MARIO_NORMAL_CAP | MARIO_CAP_ON_HEAD;
        m.health = 0x880;
        gGlobalTimer = 100 + cycle * 100;
        if (cycle == 1) {
            mario_tick_cap_pickup_lock(&m); // Mario updated before the loss.
        }
        mario_blow_off_cap_in_direction(&m, 15.0f, 0);
        EXPECT_EQ(m.flags & (MARIO_NORMAL_CAP | MARIO_CAP_ON_HEAD), 0);
        EXPECT_EQ(m.capPickupLockFrames, MARIO_CAP_PICKUP_LOCK_FRAMES);
        EXPECT_EQ(m.capPickupLockStartFrame, gGlobalTimer);
        mario_tick_cap_pickup_lock(&m); // If Mario updates after the loss, this frame does not count.
        EXPECT_EQ(m.capPickupLockFrames, MARIO_CAP_PICKUP_LOCK_FRAMES);
        for (int frame = 1; frame < MARIO_CAP_PICKUP_LOCK_FRAMES; frame++) {
            gGlobalTimer++;
            mario_tick_cap_pickup_lock(&m);
            EXPECT_EQ(mario_cap_pickup_is_blocked(&m, MARIO_NORMAL_CAP), TRUE);
            EXPECT_EQ(mario_cap_pickup_is_blocked(&m, MARIO_METAL_CAP), TRUE);
            EXPECT_EQ(mario_retrieve_cap(), FALSE);
            EXPECT_EQ(m.flags & MARIO_NORMAL_CAP, 0);
        }
        gGlobalTimer++;
        mario_tick_cap_pickup_lock(&m);
        EXPECT_EQ(m.capPickupLockFrames, 0);
        EXPECT_EQ(mario_cap_pickup_is_blocked(&m, MARIO_NORMAL_CAP), FALSE);
        EXPECT_EQ(mario_cap_pickup_is_blocked(&m, MARIO_METAL_CAP), FALSE);
        EXPECT_EQ(mario_retrieve_cap(), TRUE);
        EXPECT_EQ(m.flags & MARIO_NORMAL_CAP, MARIO_NORMAL_CAP);
    }
    // A second knock-off request while capless does not restart the wait.
    begin(ACT_WALKING);
    m.flags = MARIO_NORMAL_CAP | MARIO_CAP_ON_HEAD;
    m.health = 0x880;
    mario_blow_off_cap_in_direction(&m, 15.0f, 0);
    m.capPickupLockFrames = 5;
    u32 lossFrame = m.capPickupLockStartFrame;
    mario_blow_off_cap_in_direction(&m, 15.0f, 0);
    EXPECT_EQ(m.capPickupLockFrames, 5);
    EXPECT_EQ(m.capPickupLockStartFrame, lossFrame);
    for (int enemy = 1; enemy <= 2; enemy++) {
        begin(ACT_WALKING);
        m.flags = MARIO_NORMAL_CAP | MARIO_CAP_ON_HEAD;
        gGlobalTimer = 500 + enemy;
        sSavedCapOwnershipFlags = 0;
        EXPECT_EQ(mario_lose_cap_to_enemy(enemy), TRUE);
        EXPECT_EQ(m.capPickupLockFrames, MARIO_CAP_PICKUP_LOCK_FRAMES);
        EXPECT_EQ(m.capPickupLockStartFrame, gGlobalTimer);
        EXPECT_EQ(sSavedCapOwnershipFlags,
                  enemy == 1 ? SAVE_FLAG_CAP_ON_KLEPTO : SAVE_FLAG_CAP_ON_UKIKI);
        EXPECT_EQ(mario_lose_cap_to_enemy(enemy), FALSE);
        EXPECT_EQ(mario_retrieve_cap(), FALSE);
    }
    begin(ACT_DROWNING);
    m.flags = MARIO_NORMAL_CAP | MARIO_CAP_ON_HEAD;
    gGlobalTimer = 600;
    mario_lose_cap_to_shallow_water_death(&m);
    EXPECT_EQ(m.flags & MARIO_NORMAL_CAP, 0);
    EXPECT_EQ(m.capPickupLockFrames, MARIO_CAP_PICKUP_LOCK_FRAMES);
    EXPECT_EQ(m.capPickupLockStartFrame, gGlobalTimer);
    begin(ACT_DROWNING);
    m.flags = 0;
    mario_lose_cap_to_shallow_water_death(&m);
    EXPECT_EQ(m.capPickupLockFrames, 0);
    begin(ACT_WALKING);
    m.flags = 0;
    m.health = 0x880;
    EXPECT_EQ(mario_cap_pickup_is_blocked(&m, MARIO_NORMAL_CAP), FALSE);
    const u32 knockbacks[] = { ACT_SOFT_BACKWARD_GROUND_KB, ACT_SOFT_FORWARD_GROUND_KB,
                               ACT_BACKWARD_WATER_KB,       ACT_FORWARD_WATER_KB,
                               ACT_BACKWARD_GROUND_KB,      ACT_FORWARD_GROUND_KB,
                               ACT_HARD_BACKWARD_GROUND_KB, ACT_HARD_FORWARD_GROUND_KB,
                               ACT_BACKWARD_AIR_KB,         ACT_FORWARD_AIR_KB,
                               ACT_HARD_BACKWARD_AIR_KB,    ACT_HARD_FORWARD_AIR_KB };
    for (unsigned i = 0; i < sizeof(knockbacks) / sizeof(*knockbacks); i++) {
        begin(knockbacks[i]);
        m.flags = 0;
        m.health = 0x880;
        EXPECT_EQ(mario_cap_pickup_is_blocked(&m, MARIO_NORMAL_CAP), TRUE);
    }
    return 1;
}
static int blj_controls(void) {
    for (int cycle = 0; cycle < 3; cycle++) {
        for (int mag = 0; mag < 4; mag++) {
            begin(ACT_LONG_JUMP_LAND);
            m.forwardVel = -50;
            m.input = INPUT_Z_DOWN | INPUT_A_PRESSED;
            sController.stickMag = (f32[]){ 0, 1, 20, 64 }[mag];
            EXPECT_EQ(act_long_jump_land(&m), TRUE);
            EXPECT_EQ(m.action, mag ? ACT_LONG_JUMP : ACT_BACKFLIP);
            if (mag) {
                NEAR(m.forwardVel, -60);
            }
        }
    }
    begin(ACT_LONG_JUMP_LAND);
    m.forwardVel = -50;
    m.input = 0;
    act_long_jump_land(&m);
    NEAR(m.forwardVel, -50);
    begin(ACT_JUMP_LAND);
    m.forwardVel = -50;
    m.input = INPUT_Z_DOWN;
    sController.stickMag = 20;
    EXPECT_EQ(landing_jump_action(&m, ACT_DOUBLE_JUMP), ACT_BACKFLIP);
    return 1;
}
static int death_camera_mode(void) {
    for (int cycle = 0; cycle < 3; cycle++) {
        for (int held = 0; held < 2; held++) {
            begin(ACT_IDLE);
            sCamera.mode = CAMERA_MODE_BEHIND_MARIO;
            sCamera.defMode = CAMERA_MODE_8_DIRECTIONS;
            gLakituState.mode = CAMERA_MODE_BEHIND_MARIO;
            sSelectionFlags = CAM_MODE_MARIO_SELECTED | CAM_MODE_MARIO_ACTIVE;
            gPlayer1Controller = &sController;
            sController.buttonDown = held ? R_TRIG : 0;
            gReonucamState.rTapFrames = 1;
            gReonucamState.rHoldFrames = 1;
            reset_camera_mode(&sCamera);
            EXPECT_EQ(sCamera.mode, CAMERA_MODE_8_DIRECTIONS);
            EXPECT_EQ(gLakituState.mode, sCamera.mode);
            EXPECT_EQ(gLakituState.defMode, sCamera.defMode);
            EXPECT_EQ(sSelectionFlags, CAM_MODE_MARIO_SELECTED);
            EXPECT_EQ(gReonucamState.rTapFrames, REONUCAM_R_DOUBLE_TAP_FRAMES + 1);
            EXPECT_EQ(gReonucamState.rHoldFrames, held ? REONUCAM_R_TAP_FRAMES + 1 : 0);
        }
    }
    return 1;
}
static int shell_silhouette(void) {
    begin(ACT_IDLE);
    gMarioState = &m;
    sObj.oFlags = OBJ_FLAG_SILHOUETTE;
    const BehaviorScript *shells[] = { bhvKoopaShell, bhvKoopaShellUnderwater };
    for (int shell = 0; shell < 2; shell++) {
        for (int cycle = 0; cycle < 3; cycle++) {
            sObj.behavior = shells[shell];
            m.riddenObj = m.heldObj = NULL;
            EXPECT_EQ(object_has_silhouette(&sObj, sObj.oFlags), FALSE);
            m.riddenObj = &sObj;
            EXPECT_EQ(object_has_silhouette(&sObj, sObj.oFlags), TRUE);
            m.riddenObj = NULL;
            m.heldObj = &sObj;
            EXPECT_EQ(object_has_silhouette(&sObj, sObj.oFlags), TRUE);
            m.heldObj = &sHeld;
            EXPECT_EQ(object_has_silhouette(&sObj, sObj.oFlags), FALSE);
            m.heldObj = NULL;
        }
    }
    sObj.behavior = bhvChuckya;
    EXPECT_EQ(object_has_silhouette(&sObj, sObj.oFlags), TRUE);
    sObj.oFlags = 0;
    EXPECT_EQ(object_has_silhouette(&sObj, sObj.oFlags), FALSE);
    return 1;
}
static int metal_stunned_contact(void) {
    const u32 types[] = { INTERACT_BOUNCE_TOP, INTERACT_SPINY_WALKING, INTERACT_GRABBABLE };
    for (int type = 0; type < 3; type++) {
        for (int cycle = 0; cycle < 3; cycle++) {
            for (int phase = 0; phase < 6; phase++) {
                begin(ACT_WALKING);
                m.flags |= MARIO_METAL_CAP;
                sObj.oInteractType = types[type];
                sObj.oPosX = 100;
                m.vel[0] = 20;
                sObj.oGroundTwirlStunTimer = phase == 0 ? -20 : 20;
                sObj.oMoveFlags = phase >= 2 ? OBJ_MOVE_ON_GROUND : OBJ_MOVE_IN_AIR;
                if (phase == 2) {
                    sObj.metalContactCooldown = 1;
                }
                if (phase == 3) {
                    sObj.oGroundTwirlContactRebound = 20;
                }
                if (phase == 5) {
                    sObj.oMoveFlags = OBJ_MOVE_UNDERWATER_ON_GROUND;
                }
                EXPECT_EQ(stunned_enemy_can_interact(&m, &sObj, types[type]), TRUE);
                mario_metal_stun_enemy_on_contact(&m, &sObj);
                EXPECT_EQ(sObj.metalContactPendingDefeat, phase >= 4);
                EXPECT_EQ(sObj.oInteractStatus, 0);
                mario_metal_stun_enemy_on_contact(&m, &sObj);
                EXPECT_EQ(sObj.metalContactPendingDefeat, phase >= 4);
            }
        }
    }
    begin(ACT_WALKING);
    m.flags |= MARIO_METAL_CAP;
    sObj.oPosX = 100;
    m.vel[0] = 20;
    mario_metal_stun_enemy_on_contact(&m, &sObj);
    EXPECT_EQ(sObj.oInteractStatus, 0);
    EXPECT_EQ(sObj.oGroundTwirlStunTimer, -30);
    EXPECT_EQ(sObj.metalContactCooldown, 15);
    // Holding contact through every protected update does not attack/relaunch.
    for (int update = 0; update < 15; update++) {
        mario_metal_stun_enemy_on_contact(&m, &sObj);
        EXPECT_EQ(sObj.oInteractStatus, 0);
        sObj.metalContactCooldown--;
    }
    mario_metal_stun_enemy_on_contact(&m, &sObj);
    EXPECT_EQ(sObj.oInteractStatus, 0);
    sObj.oGroundTwirlStunTimer = 6;
    sObj.oMoveFlags = OBJ_MOVE_LANDED;
    sObj.oVelY = 0;
    mario_metal_stun_enemy_on_contact(&m, &sObj);
    EXPECT_EQ(sObj.metalContactPendingDefeat, TRUE);
    EXPECT_EQ(sObj.oInteractStatus, 0);
    return 1;
}

static int jump_kick_impact_slide_sequence(void) {
    const f32 speeds[] = { -15.5f, -16, -16.01f, -25.5f, -45.5f };
    for (int cycle = 0; cycle < 3; cycle++) {
        for (int held = 0; held < 2; held++) {
            for (unsigned i = 0; i < sizeof(speeds) / sizeof(speeds[0]); i++) {
                begin(ACT_JUMP_KICK);
                sRealMovingEntry = TRUE;
                sFloor.normal.y = 0.6f;
                sFloor.normal.z = 0.8f;
                m.faceAngle[1] = m.floorYaw = 0;
                for (int attempt = 0; attempt < 4; attempt++) {
                    if (attempt) {
                        set_mario_action(&m, held ? ACT_HOLD_JUMP : ACT_JUMP, 0);
                        set_mario_action(&m, ACT_JUMP_KICK, 0);
                    }
                    mario_set_forward_vel(&m, speeds[i]);
                    // Actual contact response followed by the real action-setter handoff.
                    EXPECT_EQ(mario_restrict_air_uphill(&m, &sFloor), TRUE);
                    NEAR(m.forwardVel, MIN(speeds[i] + 20, 0));
                    set_mario_action(&m, ACT_FREEFALL_LAND, 0);
                    NEAR(m.slopeJumpKickImpactSpeed, speeds[i]);
                    set_mario_action(&m, held ? ACT_HOLD_BEGIN_SLIDING : ACT_BEGIN_SLIDING, 0);
                    EXPECT_EQ(m.action, held ? ACT_HOLD_BUTT_SLIDE : ACT_BUTT_SLIDE);
                    f32 expected;
                    if (speeds[i] >= -16) {
                        expected = MIN(speeds[i] + 20, 0);
                    } else if (attempt == 0) {
                        expected = 0;
                    } else if (attempt == 1) {
                        expected = 10;
                    } else {
                        expected = MAX(-speeds[i], 20);
                    }
                    NEAR(m.forwardVel, expected);
                    NEAR(m.slideVelZ, expected);
                    NEAR(m.vel[2], expected);
                    NEAR(m.slopeJumpKickImpactSpeed, 0);
                }
            }
        }
    }
    // An interrupted landing cannot restore old impact momentum later.
    const u32 exits[] = { ACT_JUMP, ACT_WALKING, ACT_STOMACH_SLIDE, ACT_HARD_BACKWARD_GROUND_KB };
    for (unsigned i = 0; i < sizeof(exits) / sizeof(exits[0]); i++) {
        begin(ACT_JUMP_KICK);
        sRealMovingEntry = TRUE;
        sFloor.normal.y = 0.6f;
        sFloor.normal.z = 0.8f;
        mario_set_forward_vel(&m, -25.5f);
        mario_restrict_air_uphill(&m, &sFloor);
        set_mario_action(&m, ACT_FREEFALL_LAND, 0);
        set_mario_action(&m, exits[i], 0);
        NEAR(m.slopeJumpKickImpactSpeed, 0);
        mario_set_forward_vel(&m, -5);
        set_mario_action(&m, ACT_BUTT_SLIDE, 0);
        NEAR(m.forwardVel, -5);
    }
    sRealMovingEntry = FALSE;
    return TRUE;
}

static int slope_attempt_reset(void) {
    const u32 exits[] = { ACT_WALKING, ACT_IDLE, ACT_STOMACH_SLIDE, ACT_WATER_IDLE,
                          ACT_HARD_BACKWARD_GROUND_KB };
    for (unsigned i = 0; i < sizeof(exits) / sizeof(exits[0]); i++) {
        begin(ACT_FREEFALL_LAND);
        sRealMovingEntry = TRUE;
        sFloor.normal.y = 0.6f;
        mario_set_forward_vel(&m, -30);
        set_mario_action(&m, ACT_BUTT_SLIDE, 0);
        NEAR(m.forwardVel, 0);
        EXPECT_EQ(m.backwardSlopeSlideCount, 1);
        m.backwardButtSlideJumpCount = 2;
        set_mario_action(&m, exits[i], 0);
        EXPECT_EQ(m.backwardSlopeSlideCount, 0);
        EXPECT_EQ(m.backwardButtSlideJumpCount, 0);
        mario_set_forward_vel(&m, -30);
        set_mario_action(&m, ACT_BUTT_SLIDE, 0);
        NEAR(m.forwardVel, 0);
    }
    begin(ACT_FREEFALL_LAND);
    sRealMovingEntry = TRUE;
    sFloor.normal.y = 0.6f;
    mario_set_forward_vel(&m, -30);
    set_mario_action(&m, ACT_BUTT_SLIDE, 0);
    set_mario_action(&m, ACT_JUMP, 0);
    sFloor.normal.y = 1;
    set_mario_action(&m, ACT_JUMP_LAND, 0);
    EXPECT_EQ(m.backwardSlopeSlideCount, 0);
    sFloor.normal.y = 0.6f;
    mario_set_forward_vel(&m, -30);
    set_mario_action(&m, ACT_BUTT_SLIDE, 0);
    NEAR(m.forwardVel, 0);
    sRealMovingEntry = FALSE;
    return TRUE;
}

static int slope_butt_slide_entry(void) {
    const f32 speeds[] = { -40, -16.01f, -16, -15.99f, 0, 24 };
    const u32 origins[] = { ACT_JUMP_KICK, ACT_FREEFALL_LAND, ACT_HOLD_JUMP_LAND, ACT_WALKING };
    for (int cycle = 0; cycle < 3; cycle++) {
        for (int held = 0; held < 2; held++) {
            for (int indirect = 0; indirect < 2; indirect++) {
                for (int slope = 0; slope < 2; slope++) {
                    for (unsigned origin = 0; origin < sizeof(origins) / sizeof(origins[0]); origin++) {
                        for (unsigned speed = 0; speed < sizeof(speeds) / sizeof(speeds[0]); speed++) {
                            begin(origins[origin]);
                            sRealMovingEntry = TRUE;
                            sFloor.normal.y = slope ? 0.8f : 1;
                            sFloor.normal.z = slope ? 0.6f : 0;
                            m.faceAngle[1] = m.floorYaw = 0x2000;
                            m.vel[1] = -12;
                            mario_set_forward_vel(&m, speeds[speed]);
                            u32 slide = held ? ACT_HOLD_BUTT_SLIDE : ACT_BUTT_SLIDE;
                            u32 entry = slide;
                            if (indirect) {
                                entry = held ? ACT_HOLD_BEGIN_SLIDING : ACT_BEGIN_SLIDING;
                            }
                            set_mario_action(&m, entry, 0);
                            EXPECT_EQ(m.action, slide);
                            f32 expected = slope && speeds[speed] < -16 ? 0 : speeds[speed];
                            NEAR(m.forwardVel, expected);
                            NEAR(m.vel[0], expected * sins(m.faceAngle[1]));
                            NEAR(m.vel[2], expected * coss(m.faceAngle[1]));
                            NEAR(m.slideVelX, m.vel[0]);
                            NEAR(m.slideVelZ, m.vel[2]);
                            NEAR(m.vel[1], -12);
                            // Once sliding, gravity may build speed without the entry rule repeating.
                            mario_set_forward_vel(&m, -32);
                            set_mario_action(&m, slide, 0);
                            NEAR(m.forwardVel, -32);
                        }
                    }
                }
            }
        }
    }
    for (int held = 0; held < 2; held++) {
        begin(ACT_FREEFALL_LAND);
        sRealMovingEntry = TRUE;
        sFloor.normal.y = 0.8f;
        m.floorYaw = 0;
        m.faceAngle[1] = 0x8000;
        mario_set_forward_vel(&m, -24);
        set_mario_action(&m, held ? ACT_HOLD_BEGIN_SLIDING : ACT_BEGIN_SLIDING, 0);
        EXPECT_EQ(m.action, held ? ACT_HOLD_STOMACH_SLIDE : ACT_STOMACH_SLIDE);
        NEAR(m.forwardVel, -24);
    }
    sRealMovingEntry = FALSE;
    return TRUE;
}

static int character_jump_profiles(void) {
    const u32 actions[] = { ACT_JUMP,      ACT_DOUBLE_JUMP,      ACT_TRIPLE_JUMP,
                            ACT_HOLD_JUMP, ACT_BACKFLIP,         ACT_FLYING_TRIPLE_JUMP,
                            ACT_SIDE_FLIP, ACT_METAL_WATER_JUMP, ACT_HOLD_METAL_WATER_JUMP,
                            ACT_LONG_JUMP, ACT_WALL_KICK_AIR,    ACT_WATER_JUMP };
    const f32 standardY[] = {
        50.82f, 61.32f, 72.45f, 52.5f, 65.1f, 86.1f, 62, 50.82f, 52.5f, 30, 62, 42
    };
    for (int choice = 0; choice < 4; choice++) {
        for (unsigned i = 0; i < sizeof(actions) / sizeof(*actions); i++) {
            begin(ACT_IDLE);
            sHeThiccFile = choice & 1;
            mario_character_request(choice);
            mario_character_commit(&m, FALSE);
            m.forwardVel = 32;
            m.vel[0] = 0;
            m.vel[2] = 32;
            set_mario_action(&m, actions[i], 0);
            NEAR(m.vel[1], standardY[i]);
            if (i < 4) {
                NEAR(m.forwardVel, 28.8f);
            }
        }
    }
    return 1;
}
static int character_spawn_lifecycle(void) {
    begin(ACT_IDLE);
    gCurrentArea = &sArea;
    sWaterLevel = -10000;
    static struct TestBodyState body;
    m.marioBodyState = &body;
    EXPECT_EQ(mario_character_request(-1), FALSE);
    EXPECT_EQ(mario_character_request(4), FALSE);
    for (int choice = 0; choice < 4; choice++) {
        int previous = mario_character_active_index();
        gDebugFastWalk = TRUE;
        sHeThiccFile = choice & 1;
        mario_character_request(choice);
        EXPECT_EQ(mario_character_active_index(), previous);
        init_mario();
        EXPECT_EQ(mario_character_active_index(), choice);
        EXPECT_EQ(m.movementProfile == &gMarioMovementProfiles[0], TRUE);
        EXPECT_EQ(gDebugFastWalk, choice == previous);
        gDebugFastWalk = TRUE;
        init_mario();
        EXPECT_EQ(gDebugFastWalk, TRUE);
        gCurrDemoInput = &m;
        init_mario();
        EXPECT_EQ(mario_character_active_index(), 0);
        EXPECT_EQ(mario_character_requested_index(), choice);
        EXPECT_EQ(gDebugFastWalk, TRUE);
        gCurrDemoInput = NULL;
        gCurrCreditsEntry = &m;
        init_mario();
        EXPECT_EQ(mario_character_active_index(), 0);
        gCurrCreditsEntry = NULL;
        init_mario();
        EXPECT_EQ(mario_character_active_index(), choice);
        EXPECT_EQ(gDebugFastWalk, TRUE);
    }
    return 1;
}

static int debug_cap_hand_cleanup(void) {
    const u32 actions[] = { ACT_IDLE,
                            ACT_WALKING,
                            ACT_HOLDING_BOWSER,
                            ACT_PUTTING_ON_CAP,
                            ACT_PUTTING_ON_CAP_AIR,
                            ACT_STAR_DANCE_EXIT,
                            ACT_DEBUG_FREE_MOVE };
    for (int debug = 0; debug < 2; debug++) {
        for (unsigned i = 0; i < sizeof(actions) / sizeof(actions[0]); i++) {
            begin(actions[i]);
            gDebugLevelSelect = debug;
            m.flags = MARIO_NORMAL_CAP | MARIO_CAP_IN_HAND;
            u32 rendered = real_update_and_return_cap_flags(&m);
            int legitimate = (actions[i] & ACT_GROUP_MASK) == ACT_GROUP_CUTSCENE
                             || actions[i] == ACT_PUTTING_ON_CAP_AIR;
            int cleanup = actions[i] == ACT_DEBUG_FREE_MOVE || (debug && !legitimate);
            EXPECT_EQ(!!(rendered & MARIO_CAP_IN_HAND), !cleanup);
            EXPECT_EQ(rendered, m.flags);
            EXPECT_EQ(!!(m.flags & MARIO_CAP_ON_HEAD), cleanup);
        }
    }
    // Entry must repair the cap before a same-update flight exit can skip rendering.
    const u32 capFlags[] = { MARIO_NORMAL_CAP, MARIO_NORMAL_CAP | MARIO_WING_CAP, MARIO_METAL_CAP,
                             MARIO_VANISH_CAP };
    for (int debug = 0; debug < 2; debug++) {
        for (unsigned cap = 0; cap < sizeof(capFlags) / sizeof(*capFlags); cap++) {
            begin(ACT_PUTTING_ON_CAP_AIR);
            gDebugLevelSelect = debug;
            m.flags = capFlags[cap] | MARIO_CAP_IN_HAND;
            m.capTimer = 400;
            EXPECT_EQ(real_set_mario_action_cutscene(&m, ACT_DEBUG_FREE_MOVE, 0), ACT_DEBUG_FREE_MOVE);
            EXPECT_EQ(m.flags, capFlags[cap] | MARIO_CAP_ON_HEAD);
            EXPECT_EQ(m.capTimer, 400);
            m.action = ACT_FREEFALL;
            EXPECT_EQ(real_update_and_return_cap_flags(&m) & MARIO_CAP_IN_HAND, 0);
        }
    }
    begin(ACT_IDLE);
    m.flags = 0;
    real_set_mario_action_cutscene(&m, ACT_DEBUG_FREE_MOVE, 0);
    EXPECT_EQ(m.flags, 0); // Flight cannot conjure a missing cap.
    gDebugLevelSelect = FALSE;
    for (int wing = 0; wing < 2; wing++) {
        begin(ACT_HOLDING_BOWSER);
        struct MarioBodyState body = { .handState = MARIO_HAND_FISTS };
        struct Object heldObject = { 0 };
        m.marioBodyState = &body;
        m.heldObj = &heldObject;
        m.flags = MARIO_NORMAL_CAP | MARIO_CAP_IN_HAND | (wing ? MARIO_WING_CAP : 0);
        mario_set_cap_render_state(&m, m.flags);
        // Both ordinary hand branches contain the carried-object graph node.
        EXPECT_EQ(body.handState, MARIO_HAND_FISTS);
        m.heldObj = NULL;
        mario_set_cap_render_state(&m, m.flags);
        EXPECT_EQ(body.handState, wing ? MARIO_HAND_HOLDING_WING_CAP : MARIO_HAND_HOLDING_CAP);
    }
    return TRUE;
}

static int cap_warning_timing(void) {
    const u32 powers[] = { MARIO_WING_CAP, MARIO_METAL_CAP, MARIO_VANISH_CAP, MARIO_SPECIAL_CAPS };
    for (int cycle = 0; cycle < 3; cycle++) {
        for (unsigned cap = 0; cap < sizeof(powers) / sizeof(*powers); cap++) {
            for (int normalHat = FALSE; normalHat <= TRUE; normalHat++) {
                begin(ACT_IDLE);
                u32 initialFlags = powers[cap] | MARIO_CAP_ON_HEAD | (normalHat ? MARIO_NORMAL_CAP : 0);
                m.flags = initialFlags;
                m.capTimer = 151;
                sCapMusicFades = 0;
                int slowTransitions = 0;
                int fastTransitions = 0;
                int previousDim = FALSE;
                for (int remaining = 150; remaining >= 0; remaining--) {
                    u32 rendered = real_update_and_return_cap_flags(&m);
                    int dim = !(rendered & MARIO_SPECIAL_CAPS);
                    EXPECT_EQ(m.capTimer, remaining);
                    if (remaining >= 150) {
                        EXPECT_EQ(rendered, initialFlags);
                    } else if (remaining == 149) {
                        EXPECT_EQ(dim, TRUE);
                    }
                    if (remaining > 0) {
                        EXPECT_EQ(m.flags, initialFlags);
                        EXPECT_EQ(sCapMusicStops, 0);
                    }
                    EXPECT_EQ(!!(rendered & MARIO_NORMAL_CAP), normalHat);
                    EXPECT_EQ(!!(rendered & MARIO_CAP_ON_HEAD), normalHat || !dim);
                    if (remaining >= 100 && remaining < 150) {
                        slowTransitions += dim != previousDim;
                    } else if (remaining < 50) {
                        fastTransitions += dim != previousDim;
                    }
                    previousDim = dim;
                    EXPECT_EQ(sCapMusicFades, remaining <= 60);
                }
                EXPECT_EQ(fastTransitions > slowTransitions, TRUE);
                EXPECT_EQ(m.flags & MARIO_SPECIAL_CAPS, 0);
                EXPECT_EQ(sCapMusicStops, 1);
                EXPECT_EQ(real_update_and_return_cap_flags(&m), m.flags);
                EXPECT_EQ(sCapMusicStops, 1);
                EXPECT_EQ(sCapMusicFades, 1);
            }
        }
    }
    // Existing dialogue/cannon pauses still freeze above the final 60 updates.
    const u32 pausedActions[] = { ACT_READING_AUTOMATIC_DIALOG, ACT_READING_NPC_DIALOG,
                                  ACT_READING_SIGN, ACT_IN_CANNON };
    for (unsigned action = 0; action < sizeof(pausedActions) / sizeof(*pausedActions); action++) {
        begin(pausedActions[action]);
        m.flags = MARIO_WING_CAP | MARIO_CAP_ON_HEAD;
        m.capTimer = 149;
        u32 rendered = real_update_and_return_cap_flags(&m);
        for (int update = 0; update < 10; update++) {
            EXPECT_EQ(real_update_and_return_cap_flags(&m), rendered);
            EXPECT_EQ(m.capTimer, 149);
            EXPECT_EQ(m.flags, MARIO_WING_CAP | MARIO_CAP_ON_HEAD);
        }
        m.capTimer = 60;
        real_update_and_return_cap_flags(&m);
        EXPECT_EQ(m.capTimer, 59);
    }
    return 1;
}

static int cap_warning_blend(void) {
    static struct TestBodyState body;
    // All combinations, with and without the ordinary hat; both directions of every blink.
    for (int powers = 1; powers < 8; powers++) {
        for (int hat = 0; hat < 2; hat++) {
            for (int dark = 0; dark < 4; dark++) {
                begin(ACT_IDLE);
                m.marioBodyState = &body;
                u32 caps = (powers & 1 ? MARIO_WING_CAP : 0) | (powers & 2 ? MARIO_METAL_CAP : 0)
                           | (powers & 4 ? MARIO_VANISH_CAP : 0);
                u32 initial = caps | MARIO_CAP_ON_HEAD | (hat ? MARIO_NORMAL_CAP : 0);
                for (int cycle = 0; cycle < 3; cycle++) {
                    m.flags = initial | (dark >= 2 ? MARIO_TELEPORTING : 0);
                    m.fadeWarpOpacity = dark == 3 ? 150 : 25;
                    m.invincTimer = dark == 1 ? 3 : 0;
                    gGlobalTimer = 3;
                    m.capTimer = 152;
                    for (int timer = 151; timer >= 0; timer--) {
                        int oldDim =
                            timer + 1 < 150 && ((sCapFlickerFrames >> ((timer + 1) * 63 / 150)) & 1);
                        int dim = timer < 150 && ((sCapFlickerFrames >> (timer * 63 / 150)) & 1);
                        int edge = timer > 0 && oldDim != dim;
                        real_mario_reset_bodystate(&m);
                        sRealCapUpdate = TRUE;
                        mario_update_hitbox_and_cap_model(&m);
                        sRealCapUpdate = FALSE;
                        int geometryChanges = (caps & (MARIO_METAL_CAP | MARIO_WING_CAP)) || !hat;
                        int expectedBlend;
                        if (!edge) {
                            expectedBlend = MARIO_CAP_BLINK_NONE;
                        } else {
                            expectedBlend =
                                geometryChanges ? MARIO_CAP_BLINK_GEOMETRY : MARIO_CAP_BLINK_OPACITY;
                        }
                        EXPECT_EQ(body.capBlinkBlend, expectedBlend);
                        int alpha = (body.modelState & MODEL_STATE_ALPHA)
                                        ? body.modelState & MODEL_STATE_MASK
                                        : 255;
                        int ordinaryAlpha;
                        if (dark == 2) {
                            ordinaryAlpha = 25;
                        } else if (dark == 1 || (caps & MARIO_WING_CAP)) {
                            ordinaryAlpha = 64;
                        } else {
                            ordinaryAlpha = dark == 3 ? 150 : 255;
                        }
                        int poweredAlpha;
                        if (dark == 2) {
                            poweredAlpha = 25;
                        } else if (dark == 1) {
                            poweredAlpha = 64;
                        } else if (dark == 3) {
                            poweredAlpha = 150;
                        } else {
                            poweredAlpha = caps & MARIO_VANISH_CAP ? 128 : 255;
                        }
                        int expiredAlpha;
                        if (dark == 2) {
                            expiredAlpha = 25;
                        } else if (dark == 1) {
                            expiredAlpha = 64;
                        } else {
                            expiredAlpha = dark == 3 ? 150 : 255;
                        }
                        int expectedAlpha;
                        if (timer == 0) {
                            expectedAlpha = expiredAlpha;
                        } else if (edge) {
                            expectedAlpha = poweredAlpha;
                        } else {
                            expectedAlpha = dim ? ordinaryAlpha : poweredAlpha;
                        }
                        EXPECT_EQ(alpha, expectedAlpha);
                        EXPECT_EQ(m.capTimer, timer);
                        EXPECT_EQ(m.flags & MARIO_SPECIAL_CAPS, timer > 0 ? caps : 0);
                        EXPECT_EQ(sMarioObject.hitboxHeight, 160);
                        if (body.capBlinkBlend) {
                            int blendBaseAlpha = (body.capBlinkModelState & MODEL_STATE_ALPHA)
                                                     ? body.capBlinkModelState & MODEL_STATE_MASK
                                                     : 255;
                            EXPECT_EQ(blendBaseAlpha, ordinaryAlpha);
                            EXPECT_EQ(body.capBlinkModelState & MODEL_STATE_METAL, 0);
                            EXPECT_EQ(body.capBlinkCapState,
                                      hat ? MARIO_HAS_DEFAULT_CAP_ON : MARIO_HAS_DEFAULT_CAP_OFF);
                            EXPECT_EQ(body.capBlinkHandState, MARIO_HAND_FISTS);
                            EXPECT_EQ(body.modelState & MODEL_STATE_METAL,
                                      caps & MARIO_METAL_CAP ? MODEL_STATE_METAL : 0);
                        }
                    }
                }
            }
        }
    }
    // A Wing cap held during a cap action blends the hand geometry too.
    begin(ACT_IDLE);
    m.marioBodyState = &body;
    m.flags = MARIO_WING_CAP | MARIO_NORMAL_CAP | MARIO_CAP_IN_HAND;
    m.capTimer = 150;
    real_mario_reset_bodystate(&m);
    sRealCapUpdate = TRUE;
    mario_update_hitbox_and_cap_model(&m);
    sRealCapUpdate = FALSE;
    EXPECT_EQ(body.capBlinkBlend, TRUE);
    EXPECT_EQ(body.capBlinkHandState, MARIO_HAND_HOLDING_CAP);
    EXPECT_EQ(body.handState, MARIO_HAND_HOLDING_WING_CAP);
    EXPECT_EQ(body.capState, MARIO_HAS_DEFAULT_CAP_OFF);
    // A frozen warning, a renewal, and expiry have no lingering blend state.
    begin(ACT_READING_SIGN);
    m.marioBodyState = &body;
    m.flags = MARIO_METAL_CAP | MARIO_NORMAL_CAP | MARIO_CAP_ON_HEAD;
    m.capTimer = 149;
    sRealCapUpdate = TRUE;
    for (int update = 0; update < 6; update++) {
        real_mario_reset_bodystate(&m);
        mario_update_hitbox_and_cap_model(&m);
        EXPECT_EQ(body.capBlinkBlend, FALSE);
        EXPECT_EQ(m.capTimer, 149);
    }
    m.action = ACT_IDLE;
    m.capTimer = 600;
    real_mario_reset_bodystate(&m);
    mario_update_hitbox_and_cap_model(&m);
    EXPECT_EQ(body.capBlinkBlend, FALSE);
    EXPECT_EQ(body.modelState & MODEL_STATE_METAL, MODEL_STATE_METAL);
    m.flags = MARIO_NORMAL_CAP | MARIO_CAP_ON_HEAD;
    m.capTimer = 0;
    real_mario_reset_bodystate(&m);
    mario_update_hitbox_and_cap_model(&m);
    EXPECT_EQ(body.capBlinkBlend, FALSE);
    EXPECT_EQ(body.modelState, 0);
    sRealCapUpdate = FALSE;
    return TRUE;
}

static int invincibility_opacity(void) {
    static struct TestBodyState body;
    const u32 caps[] = { MARIO_CAP_ON_HEAD, MARIO_CAP_ON_HEAD | MARIO_METAL_CAP,
                         MARIO_CAP_ON_HEAD | MARIO_VANISH_CAP,
                         MARIO_CAP_ON_HEAD | MARIO_VANISH_CAP | MARIO_METAL_CAP };
    for (int choice = 0; choice < 4; choice++) {
        for (unsigned cap = 0; cap < sizeof(caps) / sizeof(*caps); cap++) {
            begin(ACT_IDLE);
            sHeThiccFile = choice & 1;
            mario_character_request(choice);
            mario_character_commit(&m, FALSE);
            m.marioBodyState = &body;
            m.flags = caps[cap];
            for (int cycle = 0; cycle < 3; cycle++) {
                for (int timer = 6; timer >= 0; timer--) {
                    for (int phase = 0; phase < 10; phase++) {
                        m.invincTimer = timer;
                        gGlobalTimer = phase;
                        sMarioObject.header.gfx.node.flags = GRAPH_RENDER_ACTIVE;
                        real_mario_reset_bodystate(&m);
                        mario_update_hitbox_and_cap_model(&m);
                        s32 alpha = (body.modelState & MODEL_STATE_ALPHA)
                                        ? (body.modelState & MODEL_STATE_MASK)
                                        : 255;
                        EXPECT_EQ(alpha, timer >= 3 && phase % 5 >= 3     ? 64
                                         : (caps[cap] & MARIO_VANISH_CAP) ? 128
                                                                          : 255);
                        EXPECT_EQ(sMarioObject.header.gfx.node.flags, GRAPH_RENDER_ACTIVE);
                        EXPECT_EQ(m.invincTimer, timer);
                        EXPECT_EQ(m.flags, caps[cap]);
                        EXPECT_EQ((body.modelState & MODEL_STATE_METAL) != 0,
                                  (caps[cap] & MARIO_METAL_CAP) != 0);
                        EXPECT_EQ(sMarioObject.hitboxHeight, 160);
                        EXPECT_EQ(body.capState, MARIO_HAS_DEFAULT_CAP_ON);
                    }
                }
            }
        }
    }
    const u8 fades[] = { 0, 25, 51, 63, 64, 65, 255 };
    for (unsigned i = 0; i < sizeof(fades) / sizeof(*fades); i++) {
        for (int phase = 0; phase < 10; phase++) {
            begin(ACT_CROUCHING);
            m.marioBodyState = &body;
            m.flags = MARIO_METAL_CAP | MARIO_TELEPORTING;
            m.fadeWarpOpacity = fades[i];
            m.invincTimer = 3;
            gGlobalTimer = phase;
            // An action's deliberate disappearance remains hidden, independently of the blink.
            sMarioObject.header.gfx.node.flags = GRAPH_RENDER_ACTIVE | GRAPH_RENDER_INVISIBLE;
            real_mario_reset_bodystate(&m);
            mario_update_hitbox_and_cap_model(&m);
            s32 alpha =
                (body.modelState & MODEL_STATE_ALPHA) ? (body.modelState & MODEL_STATE_MASK) : 255;
            EXPECT_EQ(alpha, phase % 5 >= 3 && fades[i] > 64 ? 64 : fades[i]);
            EXPECT_EQ(sMarioObject.header.gfx.node.flags, GRAPH_RENDER_ACTIVE | GRAPH_RENDER_INVISIBLE);
            EXPECT_EQ(body.modelState & MODEL_STATE_METAL, MODEL_STATE_METAL);
            EXPECT_EQ(sMarioObject.hitboxHeight, 100);
        }
    }
    return TRUE;
}
static int cap_reduced_flashing(void) {
    static struct TestBodyState body;
    sReducedFlashing = TRUE;
    for (int powers = 1; powers < 8; powers++) {
        for (int hat = 0; hat < 2; hat++) {
            begin(ACT_IDLE);
            sCapMusicFades = 0;
            m.marioBodyState = &body;
            u32 caps = (powers & 1 ? MARIO_WING_CAP : 0) | (powers & 2 ? MARIO_METAL_CAP : 0)
                       | (powers & 4 ? MARIO_VANISH_CAP : 0);
            m.flags = caps | MARIO_CAP_ON_HEAD | (hat ? MARIO_NORMAL_CAP : 0);
            m.capTimer = 152;
            sRealCapUpdate = TRUE;
            for (int timer = 151; timer >= 0; timer--) {
                gGlobalTimer++;
                real_mario_reset_bodystate(&m);
                mario_update_hitbox_and_cap_model(&m);
                EXPECT_EQ(m.capTimer, timer);
                EXPECT_EQ(m.flags & MARIO_SPECIAL_CAPS, timer ? caps : 0);
                EXPECT_EQ(body.capBlinkBlend, MARIO_CAP_BLINK_NONE);
                EXPECT_EQ(body.modelState & MODEL_STATE_METAL,
                          timer && (caps & MARIO_METAL_CAP) ? MODEL_STATE_METAL : 0);
                EXPECT_EQ(body.capState, timer && (caps & MARIO_WING_CAP) ? MARIO_HAS_WING_CAP_ON
                                         : timer || hat                   ? MARIO_HAS_DEFAULT_CAP_ON
                                                                          : MARIO_HAS_DEFAULT_CAP_OFF);
                s32 alpha =
                    body.modelState & MODEL_STATE_ALPHA ? body.modelState & MODEL_STATE_MASK : 255;
                EXPECT_EQ(alpha, timer && (timer < 150 || (caps & MARIO_VANISH_CAP)) ? 128 : 255);
                EXPECT_EQ(sCapMusicFades, timer <= 60);
                EXPECT_EQ(sCapMusicStops, timer == 0);
                EXPECT_EQ(sMarioObject.hitboxHeight, 160);
            }
            // A renewed cap immediately restores ordinary full-cap rendering.
            m.flags = caps | MARIO_CAP_ON_HEAD;
            m.capTimer = 600;
            real_mario_reset_bodystate(&m);
            mario_update_hitbox_and_cap_model(&m);
            EXPECT_EQ(body.capBlinkBlend, MARIO_CAP_BLINK_NONE);
            EXPECT_EQ(body.modelState & MODEL_STATE_ALPHA,
                      caps & MARIO_VANISH_CAP ? MODEL_STATE_ALPHA : 0);
            sRealCapUpdate = FALSE;
        }
    }
    // Protection, darker teleport fades and crouching hitboxes do not change.
    const u8 fades[] = { 0, 25, 64, 128, 255 };
    for (unsigned i = 0; i < sizeof(fades); i++) {
        for (int phase = 0; phase < 10; phase++) {
            begin(ACT_CROUCHING);
            m.marioBodyState = &body;
            m.flags = MARIO_METAL_CAP | MARIO_TELEPORTING;
            m.fadeWarpOpacity = fades[i];
            m.invincTimer = 3;
            gGlobalTimer = phase;
            real_mario_reset_bodystate(&m);
            mario_update_hitbox_and_cap_model(&m);
            EXPECT_EQ(body.modelState & MODEL_STATE_MASK, MIN(fades[i], 64));
            EXPECT_EQ(m.invincTimer, 3);
            EXPECT_EQ(m.flags, MARIO_METAL_CAP | MARIO_TELEPORTING);
            EXPECT_EQ(sMarioObject.hitboxHeight, 100);
        }
    }
    sReducedFlashing = FALSE;
    return TRUE;
}

static int slide_kick_slope_recovery(void) {
    const f32 speeds[] = { 0.5f, 24.0f, -12.0f };
    for (int cycle = 0; cycle < 3; cycle++) {
        for (int policy = 0; policy < 5; policy++) {
            for (unsigned speed = 0; speed < sizeof(speeds) / sizeof(*speeds); speed++) {
                begin(ACT_SLIDE_KICK_SLIDE);
                sRealMovingEntry = sRealSlideEligibility = sTrackRolloutGround = TRUE;
                m.input = INPUT_A_DOWN | INPUT_B_DOWN;
                if (policy != 4) {
                    m.input |= INPUT_ABOVE_SLIDE;
                    sFloor.normal.y = 0.7f;
                }
                m.faceAngle[1] = policy == 1 ? 0x7000 : 0x1000;
                if (policy == 2) {
                    sArea.terrainType = TERRAIN_SLIDE;
                    m.faceAngle[1] = 0x7000;
                }
                if (policy == 3) {
                    sFloor.type = SURFACE_SUPER_SLIPPERY;
                    m.faceAngle[1] = 0x7000;
                }
                m.forwardVel = speeds[speed];
                m.slideYaw = 0x2400;
                m.vel[0] = m.slideVelX = 11.0f;
                m.vel[2] = m.slideVelZ = speeds[speed];
                s16 facing = m.faceAngle[1];
                sAnimEnded = TRUE;
                s32 eligible = policy != 4 && (policy != 1 || speeds[speed] < 0.0f);
                s32 stopped = !eligible && absf(speeds[speed]) < 1.0f;
                u32 expectedAction = ACT_SLIDE_KICK_SLIDE;
                if (eligible) {
                    expectedAction = ACT_BUTT_SLIDE;
                } else if (stopped) {
                    expectedAction = ACT_SLIDE_KICK_SLIDE_STOP;
                }
                EXPECT_EQ(act_slide_kick_slide(&m), eligible || stopped);
                EXPECT_EQ(m.action, expectedAction);
                if (eligible) {
                    NEAR(m.forwardVel, speeds[speed]);
                    NEAR(m.slideVelX, 11.0f);
                    NEAR(m.slideVelZ, speeds[speed]);
                    NEAR(m.vel[0], 11.0f);
                    NEAR(m.vel[2], speeds[speed]);
                    EXPECT_EQ(m.slideYaw, 0x2400);
                    EXPECT_EQ(m.faceAngle[1], facing);
                    EXPECT_EQ(sRolloutGroundSteps, 0);
                }
            }
        }
        for (int request = 0; request < 3; request++) {
            begin(ACT_SLIDE_KICK_SLIDE);
            sRealMovingEntry = sRealSlideEligibility = sTrackRolloutGround = TRUE;
            sFloor.normal.y = 0.7f;
            m.input = INPUT_ABOVE_SLIDE;
            m.forwardVel = 24.0f;
            if (request == 0) {
                m.actionArg = 3;
            } else {
                m.input |= request == 1 ? INPUT_A_PRESSED : INPUT_B_PRESSED;
            }
            EXPECT_EQ(act_slide_kick_slide(&m), TRUE);
            EXPECT_EQ(m.action, ACT_FORWARD_ROLLOUT);
            EXPECT_EQ(m.actionArg, ACT_ARG_FORWARD_ROLLOUT_FROM_SLIDE_KICK);
            EXPECT_EQ(sRolloutGroundSteps, 0);
        }
    }
    // Fast backward entries retain the accepted butt-slide chain response.
    const f32 backwardEntrySpeeds[] = { 0.0f, 10.0f, 24.0f };
    for (int attempt = 0; attempt < 3; attempt++) {
        begin(ACT_SLIDE_KICK_SLIDE);
        sRealMovingEntry = sRealSlideEligibility = TRUE;
        sFloor.normal.y = 0.7f;
        m.input = INPUT_ABOVE_SLIDE;
        m.backwardSlopeSlideCount = attempt;
        mario_set_forward_vel(&m, -24.0f);
        EXPECT_EQ(act_slide_kick_slide(&m), TRUE);
        EXPECT_EQ(m.action, ACT_BUTT_SLIDE);
        NEAR(m.forwardVel, backwardEntrySpeeds[attempt]);
        EXPECT_EQ(m.backwardSlopeSlideCount, attempt + 1);
    }
    return TRUE;
}

static int bowser_mine_flight(void) {
    for (int cycle = 0; cycle < 3; cycle++) {
        begin(ACT_HARD_BACKWARD_AIR_KB);
        m.actionArg = 7 | BOWSER_MINE_LAUNCH_ARG;
        m.forwardVel = -140;
        sAirStepResult = AIR_STEP_NONE;
        for (int frame = 0; frame < 10; frame++) {
            common_air_knockback_step(&m, ACT_HARD_BACKWARD_GROUND_KB, ACT_HARD_BACKWARD_GROUND_KB,
                                      MARIO_ANIM_BACKWARD_AIR_KB, -16);
            NEAR(m.forwardVel, -140);
            EXPECT_EQ(m.actionArg, 7 | BOWSER_MINE_LAUNCH_ARG);
        }
        sAirStepResult = AIR_STEP_LANDED;
        common_air_knockback_step(&m, ACT_HARD_BACKWARD_GROUND_KB, ACT_HARD_BACKWARD_GROUND_KB,
                                  MARIO_ANIM_BACKWARD_AIR_KB, -16);
        EXPECT_EQ(m.action, ACT_HARD_BACKWARD_GROUND_KB);
        EXPECT_EQ(m.actionArg, 7);
        begin(ACT_HARD_BACKWARD_AIR_KB);
        m.actionArg = 7;
        m.forwardVel = -140;
        sAirStepResult = AIR_STEP_NONE;
        common_air_knockback_step(&m, ACT_HARD_BACKWARD_GROUND_KB, ACT_HARD_BACKWARD_GROUND_KB,
                                  MARIO_ANIM_BACKWARD_AIR_KB, -16);
        NEAR(m.forwardVel, -16);
    }
    return TRUE;
}
static int metal_fall_landing_sound(void) {
    const u32 impactSound = 0x1234;
    for (int once = 0; once < 2; once++) {
        begin(ACT_JUMP);
        m.flags |= MARIO_METAL_CAP;
        m.metalFallGroundPoundImpact = TRUE;
        sLastMarioSound = impactSound;
        if (once) {
            real_play_mario_landing_sound_once(&m, SOUND_ACTION_TERRAIN_LANDING);
        } else {
            real_play_mario_landing_sound(&m, SOUND_ACTION_TERRAIN_LANDING);
        }
        EXPECT_EQ(sLastMarioSound, impactSound);
        EXPECT_EQ(m.flags & MARIO_ACTION_SOUND_PLAYED, MARIO_ACTION_SOUND_PLAYED);
        m.metalFallGroundPoundImpact = FALSE;
        m.flags &= ~MARIO_ACTION_SOUND_PLAYED;
        real_play_mario_landing_sound(&m, SOUND_ACTION_TERRAIN_LANDING);
        EXPECT_EQ(sLastMarioSound, SOUND_ACTION_METAL_LANDING);
    }
    return TRUE;
}

static int jump_kick_ground_pound_cancel(void) {
    static struct TestBodyState body;
    for (int pressedAt = 13; pressedAt <= 16; pressedAt++) {
        begin(ACT_JUMP_KICK);
        m.marioBodyState = &body;
        m.actionState = ACT_STATE_JUMP_KICK_KICKING;
        for (int frame = 13; frame <= 16; frame++) {
            m.actionTimer = frame - 1;
            m.input = 0;
            sController.buttonPressed = frame == pressedAt ? Z_TRIG : 0;
            sController.buttonDown = 0;
            update_mario_button_inputs(&m);
            act_jump_kick(&m);
            EXPECT_EQ(m.action, frame == 16 && pressedAt >= 14 ? ACT_GROUND_POUND : ACT_JUMP_KICK);
        }
    }
    return TRUE;
}

static int gameplay_option_transitions(void) {
    begin(ACT_WALKING);
    struct MarioState original = m;
    mario_update_gameplay_options(&m);
    EXPECT_EQ(memcmp(&original, &m, sizeof(m)), 0);
    const u32 actions[] = { ACT_GROUND_TWIRL, ACT_TWIRL_LAND, ACT_L_AIR_TWIRL, ACT_TWIRLING,
                            ACT_FLYING };
    sTestGameOptions[GO_TWIRLS] = FALSE;
    for (int moving = 0; moving < 2; moving++) {
        for (unsigned a = 0; a < sizeof(actions) / sizeof(*actions); a++) {
            begin(actions[a]);
            m.input = moving ? INPUT_NONZERO_ANALOG : 0;
            m.forwardVel = 33;
            m.vel[1] = -72;
            m.peakHeight = 3000;
            m.airTwirlInputFrames = 6;
            m.airTwirlJumpQueued = m.airTwirlContinue = m.airTwirlGravity = TRUE;
            m.airTwirlWingControlTimer = 30;
            m.airTwirlFlightRollFrame = actions[a] == ACT_FLYING ? 8 : 0;
            sGroundTwirl.id = 42;
            sGroundTwirl.inputQueued = sGroundTwirl.bufferedInputFrames = 6;
            sGroundTwirl.bufferedInput = INPUT_A_PRESSED;
            mario_update_gameplay_options(&m);
            u32 expected = actions[a] == ACT_FLYING ? ACT_FLYING
                           : a < 2                  ? (moving ? ACT_WALKING : ACT_DECELERATING)
                                                    : ACT_FREEFALL;
            EXPECT_EQ(m.action, expected);
            NEAR(m.forwardVel, 33);
            NEAR(m.vel[1], -72);
            NEAR(m.peakHeight, 3000);
            EXPECT_EQ(m.airTwirlInputFrames | m.airTwirlJumpQueued | m.airTwirlContinue
                          | m.airTwirlGravity | m.airTwirlWingControlTimer | m.airTwirlFlightRollFrame,
                      0);
            EXPECT_EQ(sGroundTwirl.inputQueued | sGroundTwirl.bufferedInputFrames
                          | sGroundTwirl.bufferedInput,
                      0);
            EXPECT_EQ(sGroundTwirl.id, 42);
            EXPECT_EQ(m.marioObj->header.gfx.angle[1], m.faceAngle[1]);
            EXPECT_EQ(m.marioObj->header.gfx.angle[2], m.faceAngle[2]);
        }
    }
    // Enemy/tornado release rewards retain their incoming bounce velocity without twirling.
    begin(ACT_FREEFALL);
    m.vel[1] = 80;
    set_mario_action(&m, ACT_TWIRLING, 0);
    EXPECT_EQ(m.action, ACT_FREEFALL);
    NEAR(m.vel[1], 80);
    set_mario_action(&m, ACT_TWIRL_LAND, 0);
    EXPECT_EQ(m.action, ACT_FREEFALL_LAND);
    begin(ACT_WATER_IDLE);
    sSwimming.rollFrame = 8;
    sSwimming.rollCooldown = 30;
    sSwimming.rollWallContact = sSwimming.rollFloorContact = TRUE;
    EXPECT_EQ(mario_swimming_roll_is_active(), FALSE);
    update_swimming_roll(&m);
    EXPECT_EQ(sSwimming.rollFrame | sSwimming.rollCooldown | sSwimming.rollWallContact
                  | sSwimming.rollFloorContact,
              0);
    sTestGameOptions[GO_TWIRLS] = TRUE;
    begin(ACT_GROUND_TWIRL);
    m.actionTimer = GROUND_TWIRL_FRAMES;
    sGroundTwirl.lHeldThroughout = TRUE;
    EXPECT_EQ(mario_ground_twirl_is_held_pose(&m), TRUE);
    sTestGameOptions[GO_HELD_TWIRL] = FALSE;
    EXPECT_EQ(mario_ground_twirl_is_held_pose(&m), FALSE);
    mario_update_gameplay_options(&m);
    EXPECT_EQ(m.action, ACT_GROUND_TWIRL); // The underlying spin/recovery remains available.
    sTestGameOptions[GO_HELD_TWIRL] = TRUE;
    const u32 poses[] = { ACT_DAB, ACT_SPIN_DIZZY };
    const enum GameOptionId options[] = { GO_DAB, GO_DIZZINESS };
    for (int pose = 0; pose < 2; pose++) {
        begin(poses[pose]);
        m.input = INPUT_NONZERO_ANALOG;
        m.forwardVel = 18;
        sDab.shortcutFrames = 1;
        sSpinDizzy.rotation = 3 * 0x10000;
        sSpinDizzy.releaseFrames = 30;
        sTestGameOptions[options[pose]] = FALSE;
        mario_update_gameplay_options(&m);
        EXPECT_EQ(m.action, ACT_WALKING);
        NEAR(m.forwardVel, 18);
        if (pose == 0) {
            EXPECT_EQ(sDab.shortcutFrames, 0);
            EXPECT_EQ(mario_dab_update_shortcut(&m), FALSE);
        } else {
            EXPECT_EQ(sSpinDizzy.rotation | sSpinDizzy.releaseFrames, 0);
            mario_spin_dizzy_update(&m);
            EXPECT_EQ(m.action, ACT_WALKING);
        }
        sTestGameOptions[options[pose]] = TRUE;
    }
    return TRUE;
}

static int cap_presets_health(void) {
    for (int metalOriginal = 0; metalOriginal < 2; metalOriginal++) {
        for (int vanishOriginal = 0; vanishOriginal < 2; vanishOriginal++) {
            sTestGameOptions[GO_METAL_CAP] = metalOriginal;
            sTestGameOptions[GO_VANISH_CAP] = vanishOriginal;
            for (int powers = 0; powers < 8; powers++) {
                for (int cold = 0; cold < 2; cold++) {
                    for (int low = 0; low < 2; low++) {
                        begin(low ? ACT_CRAWLING : ACT_IDLE);
                        m.flags |= (powers & 1 ? MARIO_WING_CAP : 0)
                                   | (powers & 2 ? MARIO_METAL_CAP : 0)
                                   | (powers & 4 ? MARIO_VANISH_CAP : 0);
                        m.waterLevel = 400;
                        if (!low && !mario_has_land_movement_underwater(&m)) {
                            m.action = powers & 2 ? ACT_METAL_WATER_STANDING : ACT_WATER_IDLE;
                        }
                        sArea.terrainType = cold ? TERRAIN_SNOW : TERRAIN_GRASS;
                        int protectedWater =
                            ((powers & 2) && metalOriginal) || ((powers & 4) && !vanishOriginal);
                        advance_health_updates(cold ? 128 : 384);
                        EXPECT_EQ(m.health, protectedWater ? 0x880 : 0x780);
                        m.health = 0xff;
                        m.action = low ? ACT_CRAWLING : ACT_IDLE;
                        EXPECT_EQ(mario_should_drown_on_land(&m),
                                  !protectedWater && (low || mario_has_land_movement_underwater(&m)));
                        m.metalFallGroundPoundImpact = TRUE;
                        m.metalFallGroundPoundFloorObject = &sObj;
                        m.fastMetalFallSquish = TRUE;
                        m.peakHeight = 4000;
                        mario_update_gameplay_options(&m);
                        int keepMetalImpact = !metalOriginal;
                        EXPECT_EQ(m.metalFallGroundPoundImpact, !!keepMetalImpact);
                        EXPECT_EQ(m.fastMetalFallSquish, !!keepMetalImpact);
                        EXPECT_EQ(m.peakHeight, 4000);
                    }
                }
            }
        }
    }
    for (int vanishOriginal = 0; vanishOriginal < 2; vanishOriginal++) {
        sTestGameOptions[GO_VANISH_CAP] = vanishOriginal;
        begin(ACT_WATER_IDLE);
        m.flags |= MARIO_VANISH_CAP;
        m.waterLevel = 400;
        m.health = 0xff;
        check_common_submerged_cancels(&m);
        EXPECT_EQ(m.action, vanishOriginal ? ACT_DROWNING : ACT_WATER_DEATH);
    }
    sTestGameOptions[GO_METAL_CAP] = sTestGameOptions[GO_VANISH_CAP] = 0;
    return TRUE;
}

static int gameplay_status_gates(void) {
    for (int power = 0; power < 8; power++) {
        begin(ACT_IDLE);
        m.flags = MARIO_NORMAL_CAP | MARIO_CAP_ON_HEAD | (power & 1 ? MARIO_WING_CAP : 0)
                  | (power & 2 ? MARIO_METAL_CAP : 0) | (power & 4 ? MARIO_VANISH_CAP : 0);
        m.capTimer = 100;
        u32 flags = m.flags;
        sTestGameOptions[GO_CAP_LOSS] = FALSE;
        for (int enemy = 1; enemy <= 2; enemy++) {
            EXPECT_EQ(mario_lose_cap_to_enemy(enemy), FALSE);
        }
        mario_blow_off_cap_in_direction(&m, 20, 0x4000);
        mario_blow_off_cap_from_wind(&m, 50);
        mario_lose_cap_to_shallow_water_death(&m);
        m.health = 0x100;
        m.hurtCounter = 16;
        mario_lose_cap_on_lethal_damage_with_speed(&m, 0x4000, 2);
        EXPECT_EQ(m.flags, flags);
        EXPECT_EQ(m.capTimer, 100);
        EXPECT_EQ(sSavedCapOwnershipFlags | sCapSpawnCount | sSavedCapWrites | sCapMusicStops, 0);
        sTestGameOptions[GO_CAP_LOSS] = TRUE;
    }
    begin(ACT_IDLE);
    m.waterLevel = sWaterLevel = 0;
    mario_update_soaked_status(&m);
    EXPECT_EQ(mario_is_soaked(&m), TRUE);
    m.soakedFlameProtection = TRUE;
    sTestGameOptions[GO_SOAKED] = FALSE;
    EXPECT_EQ(mario_is_soaked(&m), FALSE);
    mario_update_soaked_status(&m);
    mario_apply_soaked_status(&m);
    mario_refresh_soaked_after_movement(&m);
    EXPECT_EQ(m.soakedTimer | m.soakedThisUpdate | m.soakedWaterContact | m.soakedFlameProtection
                  | m.soakedDripFrames,
              0);
    sTestGameOptions[GO_SOAKED] = TRUE;
    mario_update_soaked_status(&m);
    EXPECT_EQ(mario_is_soaked(&m), TRUE); // A current water contact can wet Mario again.

    begin(ACT_WALKING);
    m.health = 0x480;
    mario_reset_health_regeneration(&m);
    sHealthRegenFrames = HEALTH_REGEN_START_FRAMES - 1;
    sTestGameOptions[GO_PASSIVE_HEAL] = FALSE;
    update_mario_health(&m);
    EXPECT_EQ(m.health, 0x480);
    EXPECT_EQ(sHealthRegenFrames, 0);
    m.healCounter = 4;
    advance_health_updates(4);
    EXPECT_EQ(m.health, 0x580); // Coins/hearts and respawn healing still run.
    m.hurtCounter = 4;
    advance_health_updates(4);
    EXPECT_EQ(m.health, 0x480);
    m.input = INPUT_IN_POISON_GAS;
    advance_health_updates(64);
    EXPECT_EQ(m.health, 0x380); // Environmental drain remains active.
    m.input = 0;
    sTestGameOptions[GO_PASSIVE_HEAL] = TRUE;
    mario_reset_health_regeneration(&m);
    advance_health_updates(HEALTH_REGEN_START_FRAMES - 1);
    EXPECT_EQ(m.health, 0x380);
    update_mario_health(&m);
    EXPECT_EQ(m.health, 0x480);
    return TRUE;
}

// Run the cannon's native release/reset/idle bodies with engine-style timer resets.
static void cannon_release_updates(void) {
    for (s32 frame = 0; frame < 12; frame++) {
        s32 before = o->oAction;
        switch (before) {
            case OPENED_CANNON_ACT_IDLE:
                opened_cannon_act_idle();
                break;
            case OPENED_CANNON_ACT_READY:
                opened_cannon_act_ready();
                break;
            case OPENED_CANNON_ACT_SHOOT:
                opened_cannon_act_shoot();
                break;
            case OPENED_CANNON_ACT_RESETTING:
                opened_cannon_act_resetting();
                break;
        }
        o->oTimer = before == o->oAction ? o->oTimer + 1 : 0;
    }
}

static int cannon_debug_interruption(void) {
    const s32 states[] = { ACT_STATE_IN_CANNON_READY, ACT_STATE_IN_CANNON_INIT,
                           ACT_STATE_IN_CANNON_WAIT_FOR_CANNON };
    for (unsigned i = 0; i < sizeof(states) / sizeof(states[0]); i++) {
        begin(ACT_IN_CANNON);
        sRealCutsceneEntry = TRUE;
        m.usedObj = &sObj;
        m.actionState = states[i];
        sObj.oAction =
            states[i] == ACT_STATE_IN_CANNON_READY ? OPENED_CANNON_ACT_READY : OPENED_CANNON_ACT_RISING;
        sObj.oDistanceToMario = 400;
        sCamera.mode = CAMERA_MODE_8_DIRECTIONS;
        sCamera.cutscene = CUTSCENE_ENTER_CANNON;
        sPlayerCamera.usedObj = &sObj;
        sPlayerCamera.cameraEvent = CAM_EVENT_CANNON;
        m.flags = MARIO_CAP_IN_HAND;

        // The authorized U-pad caller installs this mode, then uses the shared setter.
        set_mario_action(&m, ACT_DEBUG_FREE_MOVE, 0);
        cannon_release_updates();
        EXPECT_EQ(m.action, ACT_DEBUG_FREE_MOVE);
        EXPECT_EQ(sObj.oAction, OPENED_CANNON_ACT_IDLE);
        EXPECT_EQ((sObj.header.gfx.node.flags & GRAPH_RENDER_ACTIVE) != 0, TRUE);
        EXPECT_EQ(sObj.oIntangibleTimer, 0);
        EXPECT_EQ(m.usedObj == NULL, TRUE);
        EXPECT_EQ(sPlayerCamera.usedObj == NULL, TRUE);
        EXPECT_EQ(sPlayerCamera.cameraEvent, CAM_EVENT_NONE);
        EXPECT_EQ(sCamera.cutscene, CUTSCENE_NONE);
        EXPECT_EQ(sCamera.mode, CAMERA_MODE_8_DIRECTIONS);
        EXPECT_EQ((m.flags & MARIO_CAP_ON_HEAD) != 0, TRUE);
    }

    // Another source action keeps its unrelated cached object and camera ownership.
    begin(ACT_WALKING);
    sRealCutsceneEntry = TRUE;
    m.usedObj = sPlayerCamera.usedObj = &sObj;
    sObj.oAction = 7;
    sCamera.cutscene = CUTSCENE_ENTER_CANNON;
    sPlayerCamera.cameraEvent = CAM_EVENT_CANNON;
    set_mario_action(&m, ACT_DEBUG_FREE_MOVE, 0);
    EXPECT_EQ(m.usedObj == &sObj, TRUE);
    EXPECT_EQ(sPlayerCamera.usedObj == &sObj, TRUE);
    EXPECT_EQ(sObj.oAction, 7);
    EXPECT_EQ(sCamera.cutscene, CUTSCENE_ENTER_CANNON);
    EXPECT_EQ(sPlayerCamera.cameraEvent, CAM_EVENT_CANNON);
    return TRUE;
}

static int cannon_angles(void) {
    for (s32 original = 0; original < 2; original++) {
        sTestGameOptions[GO_CANNON_ANGLES] = original;
        for (s32 direction = -1; direction <= 1; direction += 2) {
            begin(ACT_IN_CANNON);
            m.usedObj = &sObj;
            m.actionState = ACT_STATE_IN_CANNON_READY;
            sController.stickX = 64 * direction;
            sController.stickY = -64;
            for (s32 frame = 0; frame < 100; frame++) {
                EXPECT_EQ(act_in_cannon(&m), FALSE);
                EXPECT_EQ(m.action, ACT_IN_CANNON);
                s32 expected = original ? CLAMP(-640 * direction * (frame + 1), -0x4000, 0x4000)
                                        : (s16) (-640 * direction * (frame + 1));
                EXPECT_EQ(sMarioObject.oMarioCannonInputYaw, expected);
            }
            EXPECT_EQ(m.faceAngle[0], DEGREES(80));
            sController.stickY = 64;
            for (s32 frame = 0; frame < 100; frame++) {
                act_in_cannon(&m);
            }
            EXPECT_EQ(m.faceAngle[0], 0);
            sController.stickX = sController.stickY = 0;
            m.input = INPUT_A_PRESSED;
            act_in_cannon(&m);
            EXPECT_EQ(m.action, ACT_SHOT_FROM_CANNON);
            EXPECT_EQ(sObj.oAction, OPENED_CANNON_ACT_SHOOT);
            EXPECT_EQ(m.forwardVel, 100);
            EXPECT_EQ(m.vel[1], 0);
            EXPECT_EQ((sMarioObject.header.gfx.node.flags & GRAPH_RENDER_ACTIVE) != 0, TRUE);
        }
    }
    // Switching to Original while aimed beyond its limits clamps without wrapping.
    begin(ACT_IN_CANNON);
    m.usedObj = &sObj;
    m.actionState = ACT_STATE_IN_CANNON_READY;
    sMarioObject.oMarioCannonObjectYaw = 0x7000;
    sMarioObject.oMarioCannonInputYaw = 32760;
    sController.stickX = -64;
    act_in_cannon(&m);
    EXPECT_EQ(sMarioObject.oMarioCannonInputYaw, 0x4000);
    EXPECT_EQ(m.faceAngle[1], (s16) (0x7000 + 0x4000));
    sTestGameOptions[GO_CANNON_ANGLES] = 0;
    return TRUE;
}

static int disable_knockback_cleanup(void) {
    const u32 actions[] = {
        ACT_BACKWARD_AIR_KB,  ACT_HARD_FORWARD_AIR_KB, ACT_BACKWARD_GROUND_KB, ACT_FORWARD_WATER_KB,
        ACT_SHOCKED,          ACT_WATER_SHOCKED,       ACT_GETTING_BLOWN,      ACT_TORNADO_TWIRLING,
        ACT_SHOCKWAVE_BOUNCE, ACT_THROWN_FORWARD,      ACT_THROWN_BACKWARD,    ACT_BURNING_GROUND,
        ACT_BURNING_JUMP,     ACT_BURNING_FALL,        ACT_LAVA_BOOST,         ACT_LAVA_BOOST_LAND
    };
    sTestGameOptions[GO_CHEATS_ENABLED] = TRUE;
    sTestGameOptions[GO_KNOCKBACK] = TRUE;
    for (unsigned i = 0; i < sizeof(actions) / sizeof(*actions); i++) {
        begin(actions[i]);
        m.forwardVel = m.vel[0] = 40;
        m.vel[1] = -20;
        m.slideVelX = 40;
        m.hurtCounter = 4;
        m.peakHeight = 4000;
        mario_update_environment_cheats(&m);
        EXPECT_EQ(mario_is_knocked_back(m.action), FALSE);
        EXPECT_EQ(m.forwardVel, 0);
        EXPECT_EQ(m.vel[0] + m.vel[1] + m.vel[2] + m.slideVelX + m.slideVelZ, 0);
        EXPECT_EQ(m.hurtCounter, 4);
        EXPECT_EQ(m.invincTimer, 30);
        if (m.action & ACT_FLAG_AIR) {
            EXPECT_EQ(m.peakHeight, 4000);
        }
    }
    begin(ACT_DEATH_EXIT);
    mario_update_environment_cheats(&m);
    EXPECT_EQ(m.action, ACT_DEATH_EXIT);
    begin(ACT_LAVA_BOOST);
    m.actionArg = ACT_ARG_LAVA_BOOST_METAL_GROUND_POUND;
    mario_update_environment_cheats(&m);
    EXPECT_EQ(m.action, ACT_LAVA_BOOST);
    const BehaviorScript *const owners[] = { bhvChuckya, bhvKingBobomb, bhvHeaveHo };
    for (unsigned i = 0; i < sizeof(owners) / sizeof(*owners); i++) {
        begin(ACT_GRABBED);
        sObj.behavior = owners[i];
        sObj.oInteractStatus = INT_STATUS_GRABBED_MARIO;
        sObj.oCommonAnchorAction = sObj.oHeaveHoThrowState = 2;
        m.usedObj = &sObj;
        // Captor anchors move the model while grabbed physics stays at entry.
        vec3f_set(m.pos, -100, 0, -300);
        vec3f_set(m.marioObj->header.gfx.pos, 1000, 200, 3000);
        m.peakHeight = 4000;
        m.hurtCounter = 4;
        m.marioObj->oInteractStatus =
            INT_STATUS_MARIO_THROWN_BY_OBJ | INT_STATUS_MARIO_DROPPED_BY_OBJ | INT_STATUS_MARIO_STUNNED;
        mario_update_environment_cheats(&m);
        EXPECT_EQ(m.action, ACT_FREEFALL);
        EXPECT_EQ(m.pos[0], 1000);
        EXPECT_EQ(m.pos[1], 200);
        EXPECT_EQ(m.pos[2], 3000);
        EXPECT_EQ(m.peakHeight, 4000);
        EXPECT_EQ(m.hurtCounter, 4);
        EXPECT_EQ(m.usedObj == NULL, TRUE);
        EXPECT_EQ(sObj.oInteractStatus & INT_STATUS_GRABBED_MARIO, 0);
        EXPECT_EQ(m.marioObj->oInteractStatus, INT_STATUS_MARIO_STUNNED);
        EXPECT_EQ(i == 2 ? sObj.oHeaveHoThrowState : sObj.oCommonAnchorAction, 0);
    }
    begin(ACT_BACKWARD_AIR_KB);
    m.forwardVel = 40;
    sTestGameOptions[GO_CHEATS_ENABLED] = FALSE;
    mario_update_environment_cheats(&m);
    EXPECT_EQ(m.action, ACT_BACKWARD_AIR_KB);
    EXPECT_EQ(m.forwardVel, 40);
    sTestGameOptions[GO_KNOCKBACK] = FALSE;
    return TRUE;
}

static int infinite_health(void) {
    sTestGameOptions[GO_CHEATS_ENABLED] = TRUE;
    sTestGameOptions[GO_INFINITE_HEALTH] = TRUE;
    const u32 actions[] = { ACT_IDLE, ACT_WATER_IDLE, ACT_COUGHING, ACT_BURNING_GROUND,
                            ACT_BACKWARD_AIR_KB };
    for (unsigned i = 0; i < sizeof(actions) / sizeof(*actions); i++) {
        begin(actions[i]);
        m.health = 0xff;
        m.hurtCounter = 28;
        m.healCounter = m.respawnHealCounter = 4;
        m.yoshiCoinDeathPending = TRUE;
        EXPECT_EQ(mario_health_after_pending_damage(&m), 0x880);
        EXPECT_EQ(mario_update_infinite_health(&m), TRUE);
        EXPECT_EQ(m.invincTimer, 30);
        m.invincTimer = 50;
        EXPECT_EQ(mario_update_infinite_health(&m), TRUE);
        EXPECT_EQ(m.invincTimer, 50);
        EXPECT_EQ(m.health, 0x880);
        EXPECT_EQ(m.hurtCounter | m.healCounter | m.respawnHealCounter, 0);
        EXPECT_EQ(m.yoshiCoinDeathPending, FALSE);
        EXPECT_EQ(m.action, actions[i]);
        m.input = INPUT_IN_POISON_GAS;
        for (int frame = 0; frame < 400; frame++) {
            update_mario_health(&m);
        }
        EXPECT_EQ(m.health, 0x880);
        EXPECT_EQ(sHealthRegenFrames, 0);
        EXPECT_EQ(sWaterHealth.drainProgress | sPoisonHealth.drainProgress, 0);
        m.marioObj->oMarioBurnTimer = MARIO_BURN_TIMER_LIMIT;
        mario_apply_burning_damage(&m, MARIO_BURN_TIMER_LIMIT);
        EXPECT_EQ(m.health, 0x880);
    }
    begin(ACT_WATER_IDLE);
    EXPECT_EQ(mario_update_infinite_health(&m), TRUE);
    EXPECT_EQ(m.invincTimer, 0);
    m.pos[1] = 0;
    m.waterLevel = sWaterLevel = 1000;
    for (int frame = 0; frame < 600; frame++) {
        update_mario_health(&m);
    }
    EXPECT_EQ(m.health, 0x880);
    m.breath = 0xff;
    m.breathCounter = 4;
    update_mario_breath(&m);
    EXPECT_EQ(m.breath, 0x880);
    EXPECT_EQ(m.breathCounter, 0);
    begin(ACT_IDLE);
    m.hurtCounter = 28;
    update_mario_health(&m);
    sTestGameOptions[GO_CHEATS_ENABLED] = FALSE;
    for (int frame = 0; frame < 40; frame++) {
        update_mario_health(&m);
    }
    EXPECT_EQ(m.health, 0x880); // Suppressed hits do not become queued damage.
    m.hurtCounter = 4;
    for (int frame = 0; frame < 4; frame++) {
        update_mario_health(&m);
    }
    EXPECT_EQ(m.health, 0x780);
    sTestGameOptions[GO_INFINITE_HEALTH] = FALSE;
    return TRUE;
}

static int environment_cleanup(void) {
    sTestGameOptions[GO_CHEATS_ENABLED] = TRUE;
    sTestGameOptions[GO_IGNORE_SAND] = TRUE;
    sTestGameOptions[GO_IGNORE_WIND] = TRUE;
    sTestGameOptions[GO_IGNORE_CURRENTS] = TRUE;
    begin(ACT_HEAD_STUCK_IN_GROUND);
    sFloor.type = SURFACE_INSTANT_QUICKSAND;
    EXPECT_EQ(check_for_instant_quicksand(&m), FALSE);
    sTestGameOptions[GO_CHEATS_ENABLED] = FALSE;
    EXPECT_EQ(check_for_instant_quicksand(&m), TRUE);
    EXPECT_EQ(m.action, ACT_QUICKSAND_DEATH);
    sTestGameOptions[GO_CHEATS_ENABLED] = TRUE;
    for (int arg = 0; arg < 3; arg++) {
        begin(ACT_QUICKSAND_DEATH);
        m.actionArg = arg;
        m.quicksandDepth = 159;
        m.pos[1] = 100;
        m.peakHeight = 4000;
        mario_update_environment_cheats(&m);
        EXPECT_EQ(m.action, arg ? ACT_QUICKSAND_DEATH : ACT_FREEFALL);
        EXPECT_EQ(m.quicksandDepth, 0);
        EXPECT_EQ(m.peakHeight, arg ? 4000 : 100);
    }
    begin(ACT_QUICKSAND_DEATH);
    m.health = 0xff;
    mario_update_environment_cheats(&m);
    EXPECT_EQ(m.action, ACT_QUICKSAND_DEATH);
    const u32 buried[] = { ACT_HEAD_STUCK_IN_GROUND, ACT_BUTT_STUCK_IN_GROUND,
                           ACT_FEET_STUCK_IN_GROUND };
    for (unsigned i = 0; i < sizeof(buried) / sizeof(*buried); i++) {
        for (int sand = 0; sand < 2; sand++) {
            begin(buried[i]);
            sArea.terrainType = sand ? TERRAIN_SAND : TERRAIN_SNOW;
            mario_update_environment_cheats(&m);
            EXPECT_EQ(m.action, sand ? ACT_IDLE : buried[i]);
        }
    }
    const u32 wind[] = { ACT_VERTICAL_WIND, ACT_GETTING_BLOWN, ACT_TORNADO_TWIRLING };
    for (unsigned i = 0; i < sizeof(wind) / sizeof(*wind); i++) {
        begin(wind[i]);
        m.usedObj = &sObj;
        m.vel[1] = -12;
        m.windCatchActive = m.windColumnActive = TRUE;
        m.windDeathGraceTimer = 20;
        mario_update_environment_cheats(&m);
        EXPECT_EQ(m.action, ACT_FREEFALL);
        EXPECT_EQ(m.usedObj, NULL);
        EXPECT_EQ(m.vel[1], -12);
        EXPECT_EQ(m.windCatchActive | m.windColumnActive | m.windDeathGraceTimer, 0);
    }
    for (int metal = 0; metal < 2; metal++) {
        begin(ACT_CAUGHT_IN_WHIRLPOOL);
        m.flags = metal ? MARIO_METAL_CAP : 0;
        m.usedObj = &sObj;
        m.vel[1] = -50;
        mario_update_environment_cheats(&m);
        EXPECT_EQ(m.action, metal ? ACT_FREEFALL : ACT_WATER_IDLE);
        EXPECT_EQ(m.usedObj, NULL);
        EXPECT_EQ(m.vel[1], 0);
    }
    begin(ACT_IDLE);
    m.collidedObjInteractTypes = INTERACT_STRONG_WIND;
    m.soakedTimer = 400;
    mario_dry_from_wind(&m);
    EXPECT_EQ(m.soakedTimer, 400);
    sTestGameOptions[GO_CHEATS_ENABLED] = FALSE;
    mario_dry_from_wind(&m);
    EXPECT_EQ(m.soakedTimer, 0);
    begin(ACT_CAUGHT_IN_WHIRLPOOL);
    m.usedObj = &sObj;
    m.vel[1] = -50;
    mario_update_environment_cheats(&m);
    EXPECT_EQ(m.action, ACT_CAUGHT_IN_WHIRLPOOL);
    EXPECT_EQ(m.usedObj, &sObj);
    EXPECT_EQ(m.vel[1], -50);
    sTestGameOptions[GO_IGNORE_SAND] = FALSE;
    sTestGameOptions[GO_IGNORE_WIND] = FALSE;
    sTestGameOptions[GO_IGNORE_CURRENTS] = FALSE;
    return TRUE;
}

static int environment_slippery(void) {
    const s16 surfaces[] = { SURFACE_SLIPPERY,
                             SURFACE_NOISE_SLIPPERY,
                             SURFACE_HARD_SLIPPERY,
                             SURFACE_NO_CAM_COL_SLIPPERY,
                             SURFACE_SUPER_SLIPPERY,
                             SURFACE_VERY_SLIPPERY,
                             SURFACE_ICE,
                             SURFACE_HARD_VERY_SLIPPERY,
                             SURFACE_NOISE_VERY_SLIPPERY_73,
                             SURFACE_NOISE_VERY_SLIPPERY_74,
                             SURFACE_NOISE_VERY_SLIPPERY,
                             SURFACE_NO_CAM_COL_VERY_SLIPPERY };
    sTestGameOptions[GO_CHEATS_ENABLED] = TRUE;
    sTestGameOptions[GO_IGNORE_SLIPPERY] = TRUE;
    for (unsigned i = 0; i < sizeof(surfaces) / sizeof(*surfaces); i++) {
        begin(ACT_WALKING);
        m.faceAngle[1] = 0x4000;
        sFloor.type = surfaces[i];
        sFloor.normal.y = COS25;
        EXPECT_EQ(mario_get_floor_class(&m), SURFACE_CLASS_DEFAULT);
        EXPECT_EQ(mario_floor_is_slippery(&m), FALSE);
        sFloor.normal.y = 0.2f;
        EXPECT_EQ(mario_floor_is_slippery(&m), TRUE);
        EXPECT_EQ(mario_floor_is_steep(&m), TRUE);
        sFloor.type = SURFACE_GRIPPY;
        EXPECT_EQ(mario_get_floor_class(&m), SURFACE_CLASS_NOT_SLIPPERY);
        EXPECT_EQ(mario_floor_is_slippery(&m), FALSE);
    }
    begin(ACT_WALKING);
    sArea.terrainType = TERRAIN_SLIDE;
    sFloor.normal.y = COS15;
    EXPECT_EQ(mario_floor_is_slippery(&m), FALSE);
    struct Object tiltingArena = { .behavior = bhvTiltingBowserLavaPlatform, .oAction = 1 };
    sFloor.object = &tiltingArena;
    EXPECT_EQ(mario_get_floor_class(&m), SURFACE_CLASS_DEFAULT);
    sFloor.object = NULL;
    sTestGameOptions[GO_CHEATS_ENABLED] = FALSE;
    EXPECT_EQ(mario_floor_is_slippery(&m), TRUE);
    sTestGameOptions[GO_IGNORE_SLIPPERY] = FALSE;
    return TRUE;
}

int main(int argc, char **argv) {
    test_init(argc, argv);
    struct {
        const char *name;
        int (*run)(void);
    } cases[] = { { "fixes.cannon_debug_interruption", cannon_debug_interruption },
                  { "fixes.cannon_angles", cannon_angles },
                  { "fixes.disable_knockback_cleanup", disable_knockback_cleanup },
                  { "fixes.infinite_health", infinite_health },
                  { "fixes.environment_cleanup", environment_cleanup },
                  { "fixes.environment_slippery", environment_slippery },
                  { "fixes.gameplay_option_transitions", gameplay_option_transitions },
                  { "fixes.gameplay_status_gates", gameplay_status_gates },
                  { "fixes.cap_presets_health", cap_presets_health },
                  { "fixes.slide_kick_slope_recovery", slide_kick_slope_recovery },
                  { "fixes.invincibility_opacity", invincibility_opacity },
                  { "fixes.debug_cap_hand_cleanup", debug_cap_hand_cleanup },
                  { "fixes.cap_warning_timing", cap_warning_timing },
                  { "fixes.cap_warning_blend", cap_warning_blend },
                  { "fixes.cap_reduced_flashing", cap_reduced_flashing },
                  { "fixes.character_jump_profiles", character_jump_profiles },
                  { "fixes.character_spawn_lifecycle", character_spawn_lifecycle },
                  { "fixes.slope_jump_kick_height", slope_jump_kick_height },
                  { "fixes.early_jump_kick_boundaries", early_jump_kick_boundaries },
                  { "fixes.jump_kick_impact_slide_sequence", jump_kick_impact_slide_sequence },
                  { "fixes.slope_attempt_reset", slope_attempt_reset },
                  { "fixes.slope_butt_slide_entry", slope_butt_slide_entry },
                  { "fixes.blj_controls", blj_controls },
                  { "fixes.death_camera_mode", death_camera_mode },
                  { "fixes.shell_silhouette", shell_silhouette },
                  { "fixes.metal_stunned_contact", metal_stunned_contact },
                  { "fixes.enemy_draw_distance_hysteresis", enemy_draw_distance_hysteresis },
                  { "fixes.camera_object_fade", camera_object_fade },
                  { "fixes.ground_chords", ground_chords },
                  { "fixes.ground_dive_height", ground_dive_height },
                  { "fixes.ground_dive_sequence", ground_dive_sequence },
                  { "fixes.metal_lava_launch", metal_lava_launch },
                  { "fixes.squish_dispatch", squish_dispatch },
                  { "fixes.crush_respawn_visibility", crush_respawn_visibility },
                  { "fixes.normal_cap_same_area_warp", normal_cap_same_area_warp },
                  { "fixes.lab_chill_bully_spawn", lab_chill_bully_spawn },
                  { "fixes.dialogue_stop", dialogue_stop },
                  { "fixes.shoreline", shoreline },
                  { "fixes.water_dive_pose", water_dive_pose },
                  { "fixes.wing_dive_burial", wing_dive_burial },
                  { "fixes.water_jump_ledge_voice", water_jump_ledge_voice },
                  { "fixes.water_jump_exit_pose", water_jump_exit_pose },
                  { "fixes.water_jump_rim_assist", water_jump_rim_assist },
                  { "fixes.water_jump_air_control", water_jump_air_control },
                  { "fixes.water_jump_wall_kick_priority", water_jump_wall_kick_priority },
                  { "fixes.bully_release", bully_release },
                  { "fixes.heave_ho_landing_grace", heave_ho_landing_grace },
                  { "fixes.twirl_landing_cancels", twirl_landing_cancels },
                  { "fixes.twirl_landing_buffers", twirl_landing_buffers },
                  { "fixes.stop_recovery_controls", stop_recovery_controls },
                  { "fixes.neutral_slide_stop_crouch", neutral_slide_stop_crouch },
                  { "fixes.landing_controls", landing_controls },
                  { "fixes.air_twirl_landing_buffers", air_twirl_landing_buffers },
                  { "fixes.air_throw_landing_controls", air_throw_landing_controls },
                  { "fixes.small_object_throw_speed", small_object_throw_speed },
                  { "fixes.transition_input_windows", transition_input_windows },
                  { "fixes.additional_input_buffers", additional_input_buffers },
                  { "fixes.bowser_release_cancels", bowser_release_cancels },
                  { "fixes.crouch_z_release_buffer", crouch_z_release_buffer },
                  { "fixes.buffered_z_landing_jumps", buffered_z_landing_jumps },
                  { "fixes.backward_long_jump_crouch_window", backward_long_jump_crouch_window },
                  { "fixes.double_jump_wall_kick_followup", double_jump_wall_kick_followup },
                  { "fixes.double_jump_wall_kick_interruptions", double_jump_wall_kick_interruptions },
                  { "fixes.airborne_sand", airborne_sand },
                  { "fixes.burning_coin_jump", burning_coin_jump },
                  { "fixes.burning_episode_damage", burning_episode_damage },
                  { "fixes.burning_extinguish_and_death", burning_extinguish_and_death },
                  { "fixes.held_landing_controls", held_landing_controls },
                  { "fixes.prone_water", prone_water },
                  { "fixes.water_reentry", water_reentry },
                  { "fixes.poison_health_timing", poison_health_timing },
                  { "fixes.poison_contact_and_reentry", poison_contact_and_reentry },
                  { "fixes.capless_poison_and_crush", capless_poison_and_crush },
                  { "fixes.yoshi_coin_fatal_health", yoshi_coin_fatal_health },
                  { "fixes.air_twirl_coin_cooldown", air_twirl_coin_cooldown },
                  { "fixes.underwater_shell_water_exits", underwater_shell_water_exits },
                  { "fixes.wing_swimming_surface_flight", wing_swimming_surface_flight },
                  { "fixes.whole_wedge_hit_damage", whole_wedge_hit_damage },
                  { "fixes.whole_wedge_healing_preservation", whole_wedge_healing_preservation },
                  { "fixes.burning_blocks_automatic_healing", burning_blocks_automatic_healing },
                  { "fixes.health_regeneration_timing", health_regeneration_timing },
                  { "fixes.zero_health_healing", zero_health_healing },
                  { "fixes.vanish_contacts", vanish_contacts },
                  { "fixes.health_regeneration_exposure_reset", health_regeneration_exposure_reset },
                  { "fixes.health_regeneration_idle_reset", health_regeneration_idle_reset },
                  { "fixes.health_regeneration_equivalent_sleep",
                    health_regeneration_equivalent_sleep },
                  { "fixes.giant_goomba_twirl_contact", giant_goomba_twirl_contact },
                  { "fixes.piranha_twirl_contact", piranha_twirl_contact },
                  { "fixes.mobile_enemy_twirl_contact", mobile_enemy_twirl_contact },
                  { "fixes.jump_press_buffer", jump_press_buffer },
                  { "fixes.jump_kick_grounded_chain", jump_kick_grounded_chain },
                  { "fixes.jump_kick_ground_pound_cancel", jump_kick_ground_pound_cancel },
                  { "fixes.rollout_followups", rollout_followups },
                  { "fixes.dive_startup_wall_grace", dive_startup_wall_grace },
                  { "fixes.wall_bonk_momentum", wall_bonk_momentum },
                  { "fixes.wall_bonk_recovery", wall_bonk_recovery },
                  { "fixes.bowser_mine_flight", bowser_mine_flight },
                  { "fixes.bonk_recovery_boundary", bonk_recovery_boundary },
                  { "fixes.character_fall_speed", character_fall_speed },
                  { "fixes.thicc_submerged_gravity", thicc_submerged_gravity },
                  { "fixes.neutral_recovery_momentum", neutral_recovery_momentum },
                  { "fixes.belly_slide_input_chain", belly_slide_input_chain },
                  { "fixes.belly_slide_slope_rollout", belly_slide_slope_rollout },
                  { "fixes.belly_slide_forward_hold", belly_slide_forward_hold },
                  { "fixes.rollout_recovery_inputs", rollout_recovery_inputs },
                  { "fixes.rollout_water_walk_timing", rollout_water_walk_timing },
                  { "fixes.water_twirl_entry", water_twirl_entry },
                  { "fixes.rollout_recovery_momentum", rollout_recovery_momentum },
                  { "fixes.drowning_recovery", drowning_recovery },
                  { "fixes.shallow_water_sink_death", shallow_water_sink_death },
                  { "fixes.soaked_status", soaked_status },
                  { "fixes.permanent_soaked", permanent_soaked },
                  { "fixes.soaked_wind_drying", soaked_wind_drying },
                  { "fixes.pole_handstand_z_cancel", pole_handstand_z_cancel },
                  { "fixes.pole_grab_cancel_windows", pole_grab_cancel_windows },
                  { "fixes.pole_handstand_preserved_controls", pole_handstand_preserved_controls },
                  { "fixes.jump_kick_pole_grab", jump_kick_pole_grab },
                  { "fixes.shallow_water_damage_cough", shallow_water_damage_cough },
                  { "fixes.fountain_unlock_drowning", fountain_unlock_drowning },
                  { "fixes.plunge_breaststroke_input", plunge_breaststroke_input },
                  { "fixes.air_entry_stroke_buffer", air_entry_stroke_buffer },
                  { "fixes.air_entry_stroke_handoff", air_entry_stroke_handoff },
                  { "fixes.air_entry_stroke_priority_and_interruption",
                    air_entry_stroke_priority_and_interruption },
                  { "fixes.vanish_environment", vanish_environment },
                  { "fixes.cap_environment_vulnerabilities", cap_environment_vulnerabilities },
                  { "fixes.breakdance_poison_windows", breakdance_poison_windows },
                  { "fixes.powered_cap_theft_and_wind", powered_cap_theft_and_wind },
                  { "fixes.vanish_bubbles", vanish_bubbles },
                  { "fixes.vanish_breath", vanish_breath },
                  { "fixes.lab_cold_water", lab_cold_water },
                  { "fixes.vanish_surface_effects", vanish_surface_effects },
                  { "fixes.cap_surface_voices", cap_surface_voices },
                  { "fixes.wing_water_effects_and_friction", wing_water_effects_and_friction },
                  { "fixes.metal_fall_landing_sound", metal_fall_landing_sound },
                  { "fixes.vanish_shallow_water_effects", vanish_shallow_water_effects },
                  { "fixes.vanish_water_ring", vanish_water_ring },
                  { "fixes.vanish_ring_particles", vanish_ring_particles },
                  { "fixes.vanish_bubble_origin", vanish_bubble_origin },
                  { "fixes.grippy_roof_traction", grippy_roof_traction },
                  { "fixes.metal_slope_traction", metal_slope_traction },
                  { "fixes.dialogue_camera", dialogue_camera },
                  { "fixes.camera_hud_arrow", camera_hud_arrow },
                  { "fixes.c_up_neutral_entry", c_up_neutral_entry },
                  { "fixes.c_up_rejection_feedback", c_up_rejection_feedback },
                  { "fixes.wave_trails", wave_trails },
                  { "fixes.snufit_resume", snufit_resume },
                  { "fixes.snufit_walls", snufit_walls },
                  { "fixes.wave_spacing", wave_spacing },
                  { "fixes.buddy_talk_sound", buddy_talk_sound },
                  { "fixes.dab_inverse_analog", dab_inverse_analog },
                  { "fixes.dab_analog_cancel_lock", dab_analog_cancel_lock },
                  { "fixes.dab_input_sound", dab_input_sound },
                  { "fixes.dab_repeat_sound", dab_repeat_sound },
                  { "fixes.dab_rejected_sound", dab_rejected_sound },
                  { "fixes.dab_sleep_input_lock", dab_sleep_input_lock },
                  { "fixes.capless_wake_playback", capless_wake_playback },
                  { "fixes.cap_pickup_lock", cap_pickup_lock },
                  { "fixes.cap_pose_five_frame_windows", cap_pose_five_frame_windows },
                  { "fixes.spin_dizzy_cap_loss", spin_dizzy_cap_loss } };
    for (unsigned i = 0; i < sizeof(cases) / sizeof(*cases); i++) {
        if (test_selected(cases[i].name)) {
            test_report(cases[i].name, cases[i].run());
        }
    }
    return test_finish();
}
