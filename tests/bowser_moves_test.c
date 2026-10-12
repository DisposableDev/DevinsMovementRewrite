/* Native Bowser fire and tail decisions; flat-floor movement, spawning and RNG
 * are supplied services. This does not reproduce target collision or rendering. */
#include <stdint.h>
#include <math.h>
#define absf(x) fabsf(x)
#include "test_support.h"
typedef int8_t s8;
typedef uint8_t u8;
typedef int16_t s16;
typedef uint16_t u16;
typedef int32_t s32;
typedef uint32_t u32;
typedef float f32;
typedef float Vec3f[3];
typedef s16 Vec3s[3];
typedef s32 ModelID32;
#define NUM_OBJ_LISTS 16
#define OBJ_LIST_SURFACE 8
#define OBJ_LIST_GENACTOR 0
typedef u32 BehaviorScript;
#define TRUE 1
#define FALSE 0
#define UNUSED
#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define LEVEL_BOWSER_1 30
#define LEVEL_BOWSER_3 34
#define LEVEL_BOWSER_2 33
#define SURFACE_FLAG_DYNAMIC (1 << 0)
#define sqr(a) ((a) * (a))
static s16 gCurrLevelNum;
static const void *gCurrDemoInput;
static s32 gDebugInfo[6][10];
#define DEBUG_PAGE_EFFECTINFO 0
#define ABS(a) ((a) < 0 ? -(a) : (a))
#include "bowser_definitions.inc"
#define print_debug_bottom_up(...) ((void) 0)
#undef player_stats_add
static int statEvents[PLAYER_STAT_COUNT];
static void player_stats_add(enum PlayerStatId id, s32 count) {
    statEvents[id] += count;
}
struct Animation {
    s32 loopEnd;
};
struct Surface {
    u16 flags;
    s16 type, room;
    struct {
        f32 x, y, z;
    } normal;
    struct Object *object;
};
struct Object {
    u16 bowserThrowAirFrames, bowserArenaBooTimer, bowserArenaSpinyAge;
    u8 bowserArenaFade, bowserArenaCoinRolled, bowserArenaStage, bowserArenaFireMask,
        bowserArenaCollapse;
    u8 bowserArenaFallAllowed;
    s32 oBobombFuseLit, oBobombFuseTimer, oBobombBlinkTimer, oEnemyLakituNumSpinies;
    s32 oInteractStatus, oGroundTwirlStunTimer, oGroundTwirlContactRebound, oGroundTwirlStunDuration;
    struct Object *bowserArenaOwner;
    const BehaviorScript *curBhvCommand;
    s32 oBowserRandSplitFloor;
    f32 oBowserDistToCenter;
    s32 oBowserAngleToCenter;
    struct {
        struct {
            f32 scale[3];
            struct {
                s32 animFrame, animFrameAccelAssist;
                struct Animation *curAnim;
            } animInfo;
        } gfx;
        struct Object *next;
    } header;
    const BehaviorScript *behavior;
    struct Object *parentObj, *prevObj, *platform, *oBitsPlatformBowser, *oFlameBowser;
    s32 oBitsPlatformTimer, oBooDeathStatus, oBowserEyesShut, oBowserEyesTimer, oBowserRainbowLight,
        oSmallPiranhaFlameModel;
    f32 oSmallPiranhaFlameStartSpeed, oSmallPiranhaFlameEndSpeed;
    u32 activeFlags, oFlags, oMoveFlags, oInteractType, oBehParams;
    s32 oAction, oPrevAction, oTimer, oHeldState, oOpacity, oBowserTargetOpacity, oIntangibleTimer,
        oBowserGrabCooldown, oCapDoScaleVertically;
    s32 oFaceAngleYaw, oFaceAnglePitch, oFaceAngleRoll, oMoveAngleYaw, oAngleToMario, oMoveAnglePitch,
        oAngleVelYaw, oInteractionSubtype;
    s32 oFlameSpeedTimerOffset, oAnimState, oBehParams2ndByte, oFloorType, oRoom, oFloorRoom;
    s32 oDamageOrCoinValue, oHealth, oNumLootCoins, oSubAction, oBowserTimer, oSoundStateID,
        oBowserStatus, oBowserIsReacting, oBowserPostAttackFacingTimer, oBowserGrabbedStatus,
        oBowserHeldAngleVelYaw, oBowserHeldAnglePitch;
    f32 oPosX, oPosY, oPosZ, oVelX, oVelY, oVelZ, oForwardVel, oGravity, oBounciness, oBuoyancy,
        oDragStrength, oHomeX, oHomeY, oHomeZ, oDistanceToMario;
    struct Surface *oFloor;
    f32 oWallHitboxRadius;
    f32 oFlameScale, oFlameUnusedRand, oGraphYOffset, oFloorHeight, oBlueFlameNextScale;
    f32 hitboxRadius, hitboxHeight, hitboxDownOffset, hurtboxRadius, hurtboxHeight;
};
struct MarioState {
    u32 action, actionArg, actionState, input;
    struct Object *heldObj, *marioObj, *interactObj, *usedObj;
    s16 faceAngle[3];
    Vec3f pos, prevPos, vel;
    f32 forwardVel, floorHeight, slideVelX, slideVelZ;
};
static const BehaviorScript bhvMovingFlame[] = { 0 };
static struct Object object, marioObject, bowser, children[128], gObjectLists[NUM_OBJ_LISTS];
static struct Object *gCurrentObject = &object, *gMarioObject = &marioObject;
#define o gCurrentObject
static struct {
    s32 wedges;
} gHudDisplay;
static struct MarioState heldMario;
static struct MarioState *gMarioState = &heldMario;
#define SOUND_OBJ_BOWSER_TAIL_PICKUP 3
#define SOUND_OBJ2_BOWSER_TELEPORT 4
static void mario_drop_held_object(struct MarioState *m) {
    m->heldObj->oHeldState = HELD_DROPPED;
    m->heldObj = NULL;
}
static s32 set_mario_action(struct MarioState *m, u32 action, u32 arg) {
    m->action = action;
    m->actionArg = arg;
    return TRUE;
}
static s32 __attribute__((unused)) drop_and_set_mario_action(struct MarioState *m, u32 action,
                                                             u32 arg) {
    if (m->heldObj != NULL) {
        mario_drop_held_object(m);
    }
    return set_mario_action(m, action, arg);
}
static f32 escapeCapSpeed;
static void mario_blow_off_cap(struct MarioState *m, f32 speed) {
    (void) m;
    escapeCapSpeed = speed;
}
static s32 gGlobalTimer, gCollisionFlags, spawnedCount, tailOverlap, simulateMovement;
static f32 suppliedRandom;
static f32 queuedRandom[2];
static int queuedRandomCount, queuedRandomIndex;
static int impactCalls;
static f32 animationSpeed;
static s32 releaseMovementCalls;
static f32 releaseForwardVel, releaseVelY;
static struct Animation suppliedAnimation = { 100 };
static s16 dBowserFlamesOrientationValues[500];
#define SOUND_AIR_BOWSER_SPIT_FIRE 1
#define SOUND_OBJ_BOWSER_INHALING 2
static void *segmented_to_virtual(const void *data) {
    return (void *) data;
}
static void cur_obj_play_sound_1(s32 sound) {
}
#define SOUND_OBJ2_BOWSER_ROAR 4
#define SOUND_OBJ_BOWSER_WALK 5
static int centerJumpRoars;
static void cur_obj_play_sound_2(s32 sound) {
    if (sound == SOUND_OBJ2_BOWSER_ROAR) {
        centerJumpRoars++;
    }
}
static void cur_obj_init_animation_with_accel_and_sound(s32 animation, f32 speed) {
    o->oSoundStateID = animation;
    o->header.gfx.animInfo.curAnim = &suppliedAnimation;
    animationSpeed = speed;
}
static s32 cur_obj_check_if_near_animation_end(void) {
    return o->header.gfx.animInfo.animFrame >= suppliedAnimation.loopEnd - 2;
}
static void mario_queue_bowser_impact(struct Object *source) {
    impactCalls++;
}
static void spawn_mist_particles_variable(s32 a, s32 b, f32 scale) {
}
static s32 cur_obj_check_anim_frame(s32 frame) {
    return o->header.gfx.animInfo.animFrame == frame;
}
static void cur_obj_init_animation_with_sound(s32 animation) {
}
static void cur_obj_forward_vel_approach_upward(f32 target, f32 step) {
    o->oForwardVel = MIN(target, o->oForwardVel + step);
}
static void cur_obj_start_cam_event(struct Object *obj, s32 event) {
}
#define BEHAVIOR(name) static const BehaviorScript name[1] = { 0 }
BEHAVIOR(bhvGoomba);
BEHAVIOR(bhvBobomb);
BEHAVIOR(bhvBobombFuseSmoke);
BEHAVIOR(bhvEnemyLakitu);
BEHAVIOR(bhvMrI);
BEHAVIOR(bhvBoo);
BEHAVIOR(bhvSnufit);
BEHAVIOR(bhvCirclingAmp);
BEHAVIOR(bhvSmallBully);
BEHAVIOR(bhvHomingAmp);
BEHAVIOR(bhvFireSpitter);
BEHAVIOR(bhvFallingBowserPlatform);
BEHAVIOR(bhvSpiny);
BEHAVIOR(bhvMovingYellowCoin);
BEHAVIOR(bhvBowser);
BEHAVIOR(bhvBowserTailAnchor);
BEHAVIOR(bhvBowserShockWave);
BEHAVIOR(bhvTiltingBowserLavaPlatform);
BEHAVIOR(bhvBlackSmokeUpward);
BEHAVIOR(bhvTemporaryYellowCoin);
BEHAVIOR(bhvFlameLargeBurningOut);
BEHAVIOR(bhvFlameBowser);
BEHAVIOR(bhvBlueFlamesGroup);
BEHAVIOR(bhvFlameFloatingLanding);
BEHAVIOR(bhvFlameBouncing);
BEHAVIOR(bhvBlueBowserFlame);
BEHAVIOR(bhvFlameMovingForwardGrowing);
static f32 random_float(void) {
    if (queuedRandomIndex < queuedRandomCount) {
        return queuedRandom[queuedRandomIndex++];
    }
    return suppliedRandom;
}
static u16 random_u16(void) {
    return 0;
}
static f32 sins(s16 angle) {
    return sinf(angle * 3.14159265358979323846f / 32768.0f);
}
static f32 coss(s16 angle) {
    return cosf(angle * 3.14159265358979323846f / 32768.0f);
}
static s16 angle_to(f32 x, f32 z) {
    return (s16) (atan2f(x, z) * 32768.0f / 3.14159265358979323846f);
}
static s16 atan2s(f32 z, f32 x) {
    return angle_to(x, z);
}
static s16 obj_angle_to_object(struct Object *a, struct Object *b) {
    return angle_to(b->oPosX - a->oPosX, b->oPosZ - a->oPosZ);
}
static s16 mario_obj_angle_to_object(struct MarioState *m, struct Object *obj) {
    return angle_to(obj->oPosX - m->pos[0], obj->oPosZ - m->pos[2]);
}
static s16 abs_angle_diff(s16 a, s16 b) {
    s16 diff = b - a;
    return diff == -0x8000 ? 0x7FFF : ABS(diff);
}
#define GET_BPARAM1(params) ((params) >> 24)
static struct ObjectHitbox sYellowCoinHitbox = { 0 };
static void cur_obj_set_model(s32 model) {
}
static s32 cur_obj_has_model(s32 model) {
    return FALSE;
}
static void obj_copy_pos(struct Object *dest, struct Object *src) {
    dest->oPosX = src->oPosX;
    dest->oPosY = src->oPosY;
    dest->oPosZ = src->oPosZ;
}
static s32 obj_has_behavior(struct Object *obj, const BehaviorScript *behavior) {
    return obj->behavior == behavior;
}
static s32 cur_obj_has_behavior(const BehaviorScript *behavior) {
    return obj_has_behavior(o, behavior);
}
static void obj_mark_for_deletion(struct Object *obj) {
    obj->activeFlags = ACTIVE_FLAG_DEACTIVATED;
}
static void cur_obj_become_tangible(void) {
    o->oIntangibleTimer = 0;
}
static void cur_obj_become_intangible(void) {
    o->oIntangibleTimer = -1;
}
static void cur_obj_unrender_set_action_and_anim(s32 animation, s32 action) {
    o->oAction = action;
    cur_obj_init_animation_with_sound(animation);
}
static void cur_obj_get_thrown_or_placed(f32 forwardVel, f32 velY, s32 action) {
    o->oHeldState = HELD_FREE;
    o->oAction = action;
    o->oForwardVel = forwardVel;
    o->oVelY = velY;
}
static void cur_obj_scale(f32 scale) {
    for (int i = 0; i < 3; i++) {
        o->header.gfx.scale[i] = scale;
    }
}
static void obj_translate_xz_random(struct Object *obj, f32 distance) {
}
static struct Surface *cur_obj_update_floor_height_and_get_floor(void) {
    return o->oFloor;
}
static void cur_obj_update_floor_and_walls(void) {
}
static void cur_obj_update_floor_height(void) {
}
static void cur_obj_set_pos_via_transform(void) {
}
static void cur_obj_move_y(f32 gravity, f32 bounciness, f32 buoyancy) {
    if (simulateMovement) {
        o->oVelY += gravity;
        o->oPosY += o->oVelY;
        if (o->oPosY < o->oFloorHeight) {
            o->oPosY = o->oFloorHeight;
            o->oMoveFlags |= OBJ_MOVE_LANDED;
        }
    }
}
void cur_obj_compute_vel_xz(void);
void cur_obj_apply_drag_xz(f32 dragStrength);
static void cur_obj_move_standard(s32 slope) {
    releaseMovementCalls++;
    releaseForwardVel = o->oForwardVel;
    releaseVelY = o->oVelY;
    if (simulateMovement) {
        cur_obj_compute_vel_xz();
        cur_obj_apply_drag_xz(o->oDragStrength);
        o->oPosX += o->oVelX;
        o->oPosZ += o->oVelZ;
        cur_obj_move_y(o->oGravity, o->oBounciness, o->oBuoyancy);
        s32 negativeSpeed = o->oForwardVel < 0.0f;
        o->oForwardVel = sqrtf(sqr(o->oVelX) + sqr(o->oVelZ));
        if (negativeSpeed) {
            o->oForwardVel = -o->oForwardVel;
        }
    }
}
static s32 obj_check_if_collided_with_object(struct Object *a, struct Object *b) {
    return tailOverlap;
}
static struct Object *spawn_object(struct Object *parent, s32 model, const BehaviorScript *behavior) {
    if (spawnedCount == 128) {
        fprintf(stderr, "HARNESS ERROR: spawn capacity\n");
        exit(2);
    }
    struct Object *child = &children[spawnedCount++];
    *child =
        (struct Object){ .behavior = behavior, .parentObj = parent, .activeFlags = ACTIVE_FLAG_ACTIVE };
    child->bowserArenaOwner = parent->bowserArenaOwner;
    child->oMoveAnglePitch = parent->oMoveAnglePitch;
    child->oFaceAnglePitch = parent->oFaceAnglePitch;
    child->oFaceAngleRoll = parent->oFaceAngleRoll;
    child->header.next = gObjectLists[0].header.next;
    gObjectLists[0].header.next = child;
    return child;
}
static struct Object *spawn_object_with_scale(struct Object *parent, s32 model,
                                              const BehaviorScript *behavior, f32 scale) {
    return spawn_object(parent, model, behavior);
}
static struct Object *spawn_object_relative_with_scale(s32 param, s32 x, s32 y, s32 z, f32 scale,
                                                       struct Object *parent, s32 model,
                                                       const BehaviorScript *behavior) {
    struct Object *child = spawn_object(parent, model, behavior);
    child->oBehParams2ndByte = param;
    return child;
}
static struct Object *spawn_object_relative(s32 param, s32 x, s32 y, s32 z, struct Object *parent,
                                            s32 model, const BehaviorScript *behavior) {
    return spawn_object_relative_with_scale(param, x, y, z, 1.0f, parent, model, behavior);
}
static struct Surface teleportFloor;
static int teleportUnsafe;
static int arenaFloorTest;
static f32 find_floor(f32 x, f32 y, f32 z, struct Surface **floor) {
    (void) y;
    if (arenaFloorTest) {
        teleportFloor.type = fabsf(x) > 2500 || fabsf(z) > 2500 || (arenaFloorTest == 2 && x != 600)
                                 ? SURFACE_BURNING
                                 : SURFACE_DEFAULT;
        *floor = &teleportFloor;
        return 307;
    }
    teleportFloor.type = teleportUnsafe && (x != heldMario.pos[0] || z != heldMario.pos[2])
                             ? SURFACE_BURNING
                             : SURFACE_DEFAULT;
    *floor = teleportUnsafe == 2 ? NULL : &teleportFloor;
    return 100;
}
#define MODEL_SMOKE 0
#define PARTICLE_PRIORITY_AMBIENT 0
static const BehaviorScript bhvWhitePuffSmoke2[1] = { 0 };
static void try_spawn_particle_relative(s32 param, s32 x, s32 y, s32 z, f32 scale,
                                        struct Object *parent, s32 model,
                                        const BehaviorScript *behavior, s32 priority) {
}
static void cur_obj_extend_animation_if_at_end(void) {
}
static s32 cur_obj_init_animation_and_check_if_near_end(s32 animation) {
    return cur_obj_check_if_near_animation_end();
}
static struct Surface *sObjFloor;
static int capPositionSaves;
static s16 capSavedX, capSavedY, capSavedZ;
static s16 capCollisionFlags;
static int capQuicksandChecks, capScaleCalls;
static s16 object_step(void) {
    return capCollisionFlags;
}
static void cap_check_quicksand(void) {
    capQuicksandChecks++;
}
static void cap_scale_vertically(void) {
    capScaleCalls++;
}
static void save_file_set_cap_pos(s16 x, s16 y, s16 z) {
    capPositionSaves++;
    capSavedX = x;
    capSavedY = y;
    capSavedZ = z;
}
// Arena progression is separate from the shared move timing fixture.
static int behaviorUpdates;
static void cur_obj_update(void) {
    behaviorUpdates++;
    o->curBhvCommand = o->behavior + 1;
}
static struct Object *cur_obj_nearest_object_with_behavior(const BehaviorScript *behavior) {
    (void) behavior;
    return &object;
}
static s32 suppliedHitMine;
static s32 bowser_check_hit_mine(void) {
    return suppliedHitMine;
}
#define SOUND_AIR_BOBOMB_LIT_FUSE 0
#define MODEL_SMOKE 0
#define PARTICLE_PRIORITY_AMBIENT 0
static int bobombSteps;
static s32 is_point_within_radius_of_mario(f32 x, f32 y, f32 z, s32 radius) {
    return sqr(x - marioObject.oPosX) + sqr(y - marioObject.oPosY) + sqr(z - marioObject.oPosZ)
           < sqr(radius);
}
static void bobomb_free_loop(void) {
    bobombSteps++;
    o->oPosY -= 75.0f;
}
static void bobomb_held_loop(void) {
}
static void bobomb_thrown_loop(void) {
}
static void bobomb_dropped_loop(void) {
}
static void curr_obj_random_blink(s32 *timer) {
}
static void try_spawn_particle(struct Object *obj, s32 model, const BehaviorScript *behavior,
                               s32 priority) {
}
static const void *virtual_to_segmented(s32 segment, const void *address) {
    (void) segment;
    return address;
}
#include "bowser_sources.inc"
static void begin(void) {
    sTestGameOptions[GO_BOWSER_FIGHTS] = 0;
    gHudDisplay.wedges = 8;
    behaviorUpdates = 0;
    suppliedHitMine = FALSE;
    for (int id = 0; id < PLAYER_STAT_COUNT; id++) {
        statEvents[id] = 0;
    }
    for (int list = 0; list < NUM_OBJ_LISTS; list++) {
        gObjectLists[list].header.next = &gObjectLists[list];
    }
    queuedRandomCount = queuedRandomIndex = 0;
    centerJumpRoars = 0;
    object = (struct Object){
        .activeFlags = ACTIVE_FLAG_ACTIVE, .oPosY = 500, .oFlameScale = 3, .oOpacity = 255
    };
    marioObject = (struct Object){ .oPosX = 1000, .oPosZ = 0 };
    bowser = (struct Object){ .behavior = bhvBowser, .oOpacity = 255 };
    gCurrentObject = &object;
    spawnedCount = tailOverlap = simulateMovement = impactCalls = 0;
    suppliedRandom = 0.5f;
    cur_obj_scale(3);
}
static int airborne_fire(void) {
    void (*loops[])(void) = { bhv_flame_bowser_loop, bhv_flame_floating_landing_loop,
                              bhv_flame_moving_forward_growing_loop, bhv_flame_bouncing_loop };
    for (unsigned kind = 0; kind < sizeof(loops) / sizeof(*loops); kind++) {
        for (int cycle = 0; cycle < 3; cycle++) {
            begin();
            object.oFloorType = SURFACE_DEATH_PLANE;
            object.oIntangibleTimer = -1;
            for (int frame = 0; frame < 300; frame++) {
                object.oTimer = frame;
                loops[kind]();
                EXPECT_EQ(object.activeFlags, ACTIVE_FLAG_ACTIVE);
                EXPECT_EQ(object.oIntangibleTimer, 0);
                EXPECT_EQ(object.oInteractType, INTERACT_FLAME);
                EXPECT_EQ(object.hitboxHeight > 0, TRUE);
            }
            object.oTimer = 300;
            loops[kind]();
            EXPECT_EQ(object.activeFlags, ACTIVE_FLAG_DEACTIVATED);
        }
    }
    return TRUE;
}
static int fire_landing(void) {
    for (int param = 0; param < 3; param++) {
        begin();
        object.oBehParams2ndByte = param;
        object.oMoveFlags = OBJ_MOVE_LANDED;
        bhv_flame_floating_landing_loop();
        EXPECT_EQ(object.activeFlags, ACTIVE_FLAG_DEACTIVATED);
        EXPECT_EQ(spawnedCount, 1);
        EXPECT_EQ(children[0].behavior, param == 0 ? bhvFlameLargeBurningOut : bhvBlueFlamesGroup);
    }
    begin();
    object.oMoveFlags = OBJ_MOVE_LANDED;
    bhv_flame_bowser_loop();
    EXPECT_EQ(object.oAction, 1);
    EXPECT_EQ(object.oGravity, 0);
    EXPECT_EQ(object.activeFlags, ACTIVE_FLAG_ACTIVE);
    begin();
    object.oMoveFlags = OBJ_MOVE_LANDED;
    object.oFloorType = SURFACE_BURNING;
    EXPECT_EQ(bowser_flame_should_despawn(300), TRUE);
    return TRUE;
}
static int red_fire_homing(void) {
    for (int fightOne = 0; fightOne < 2; fightOne++) {
        begin();
        gCurrLevelNum = fightOne ? LEVEL_BOWSER_1 : LEVEL_BOWSER_3;
        object.oPosY = -1;
        object.oMoveAngleYaw = 0x2000;
        bhv_flame_moving_forward_growing_loop();
        EXPECT_EQ(spawnedCount, 1);
        EXPECT_EQ(children[0].oBehParams & BOWSER_FLAME_GROUND_BREATH, BOWSER_FLAME_GROUND_BREATH);
        s16 incomingYaw = object.oMoveAngleYaw;
        gCurrentObject = &children[0];
        bhv_flame_bowser_init();
        EXPECT_EQ(o->oAction, 1);
        EXPECT_EQ(o->oMoveAngleYaw, incomingYaw);
        EXPECT_EQ(o->oVelY, 0);
        EXPECT_EQ(o->oGravity, 0);
        EXPECT_EQ(o->oForwardVel, fightOne ? 5 : 15);
        simulateMovement = TRUE;
        bhv_flame_bowser_loop();
        EXPECT_EQ(o->oPosY, 0);
        EXPECT_EQ(o->oMoveAngleYaw, incomingYaw + (fightOne ? 0x20 : DEGREES(1)));
        EXPECT_EQ(o->oPosX > 0 && o->oPosZ > 0, TRUE);
    }
    void (*loops[])(void) = { bhv_flame_bowser_loop, bhv_flame_moving_forward_growing_loop,
                              bhv_flame_floating_landing_loop, bhv_blue_bowser_flame_loop };
    for (unsigned kind = 0; kind < sizeof(loops) / sizeof(*loops); kind++) {
        for (int side = -1; side <= 1; side += 2) {
            begin();
            object.oForwardVel = 10;
            marioObject.oPosX *= side;
            for (int frame = 0; frame < 10; frame++) {
                s16 before = object.oMoveAngleYaw;
                loops[kind]();
                EXPECT_EQ((s16) (object.oMoveAngleYaw - before), side * 0x20);
            }
        }
    }
    for (int fightOne = 0; fightOne < 2; fightOne++) {
        begin();
        gCurrLevelNum = fightOne ? LEVEL_BOWSER_1 : LEVEL_BOWSER_3;
        object.oAction = 1;
        object.oForwardVel = 0;
        object.oGravity = 0;
        object.oPosY = 0;
        simulateMovement = TRUE;
        bhv_flame_bowser_loop();
        EXPECT_EQ(object.oPosX > 0 && object.oPosZ > 0, TRUE);
        EXPECT_EQ(object.oMoveAngleYaw, fightOne ? 0x20 : DEGREES(1));
        EXPECT_EQ(object.oForwardVel, fightOne ? 5 : 15);
    }
    gCurrLevelNum = 0;
    EXPECT_EQ(sBowserFlameHitbox.radius, 12);
    EXPECT_EQ(sGrowingBowserFlameHitbox.radius, 12);
    return TRUE;
}
static int blue_fire_homing(void) {
    for (int side = -1; side <= 1; side += 2) {
        begin();
        marioObject.oPosX *= side;
        for (int frame = 0; frame < 15; frame++) {
            object.oTimer = frame;
            s16 before = object.oMoveAngleYaw;
            bhv_flame_bouncing_loop();
            EXPECT_EQ((s16) (object.oMoveAngleYaw - before), side * 0xC0);
            EXPECT_EQ(object.activeFlags, ACTIVE_FLAG_ACTIVE);
        }
        begin();
        object.oBehParams2ndByte = 1;
        marioObject.oPosX *= side;
        bhv_blue_bowser_flame_loop();
        EXPECT_EQ(object.oMoveAngleYaw, side * 0xC0);
    }
    for (int param = 1; param <= 2; param++) {
        begin();
        object.oBehParams2ndByte = param;
        bhv_flame_floating_landing_loop();
        EXPECT_EQ(object.oMoveAngleYaw, 0xC0);
    }
    begin();
    object.oBehParams2ndByte = 0;
    bhv_blue_bowser_flame_loop();
    EXPECT_EQ(object.oMoveAngleYaw, 0x20);
    // Turning across signed-angle wrap follows the short path.
    begin();
    object.oMoveAngleYaw = 0x7FC0;
    marioObject.oPosX = -1;
    marioObject.oPosZ = -1000;
    bhv_flame_bouncing_loop();
    EXPECT_EQ((s16) object.oMoveAngleYaw < 0, TRUE);
    return TRUE;
}
static int varied_fire_gravity(void) {
    void (*initializers[])(void) = { bhv_flame_bowser_init, bhv_flame_floating_landing_init,
                                     bhv_flame_large_burning_out_init };
    void (*loops[])(void) = { bhv_flame_bowser_loop, bhv_flame_floating_landing_loop,
                              bhv_flame_bowser_loop };
    for (unsigned kind = 0; kind < sizeof(initializers) / sizeof(*initializers); kind++) {
        f32 gravities[2];
        int landingFrames[2];
        for (int variant = 0; variant < 2; variant++) {
            begin();
            suppliedRandom = variant ? 0.9f : 0.1f;
            initializers[kind]();
            gravities[variant] = object.oGravity;
            EXPECT_EQ(object.oGravity >= -1.2f && object.oGravity <= -0.8f, TRUE);
            object.oVelY = 0;
            simulateMovement = TRUE;
            for (landingFrames[variant] = 0; !(object.oMoveFlags & OBJ_MOVE_LANDED);
                 landingFrames[variant]++) {
                EXPECT_EQ(landingFrames[variant] < 300, TRUE);
                object.oTimer = landingFrames[variant];
                loops[kind]();
            }
        }
        EXPECT_EQ(gravities[0] != gravities[1], TRUE);
        EXPECT_EQ(landingFrames[0] - landingFrames[1] >= 15, TRUE);
    }
    return TRUE;
}
static int blue_spit_coin_drop(void) {
    const f32 rolls[] = { 0.0f, 0.0499f, 0.05f, 0.0999f, 0.1f, 0.9999f };
    for (unsigned roll = 0; roll < sizeof(rolls) / sizeof(*rolls); roll++) {
        begin();
        suppliedRandom = rolls[roll];
        bowser_flame_despawn();
        EXPECT_EQ(object.activeFlags, ACTIVE_FLAG_DEACTIVATED);
        EXPECT_EQ(spawnedCount, 1 + (roll < 2));
        EXPECT_EQ(children[0].behavior == bhvBlackSmokeUpward, TRUE);
        if (roll < 2) {
            EXPECT_EQ(children[1].behavior == bhvTemporaryYellowCoin, TRUE);
        }
    }
    const s16 levels[] = { LEVEL_BOWSER_1, LEVEL_BOWSER_2, LEVEL_BOWSER_3 };
    const s32 lifetimes[] = { 120, 180, 240 };
    for (int fight = 0; fight < 3; fight++) {
        gCurrLevelNum = levels[fight];
        for (unsigned roll = 0; roll < sizeof(rolls) / sizeof(*rolls); roll++) {
            begin();
            suppliedRandom = rolls[roll];
            bowser_flame_despawn();
            EXPECT_EQ(spawnedCount, 1 + (roll < (fight == 2 ? 4 : 2)));
        }
        for (int grounded = 0; grounded < 2; grounded++) {
            for (unsigned roll = 0; roll < sizeof(rolls) / sizeof(*rolls); roll++) {
                begin();
                object.oBehParams = BOWSER_FLAME_NO_SPLIT;
                object.oBehParams2ndByte = 1;
                object.oAction = grounded;
                object.oTimer = lifetimes[fight] - 1;
                suppliedRandom = rolls[roll];
                bhv_blue_bowser_flame_loop();
                EXPECT_EQ(spawnedCount, 0);
                object.oTimer = lifetimes[fight];
                bhv_blue_bowser_flame_loop();
                EXPECT_EQ(object.activeFlags, ACTIVE_FLAG_DEACTIVATED);
                EXPECT_EQ(spawnedCount, roll < (fight == 2 ? 4 : 2));
                if (spawnedCount != 0) {
                    EXPECT_EQ(children[0].behavior == bhvTemporaryYellowCoin, TRUE);
                }
            }
        }
        void (*loops[])(void) = { bhv_flame_bouncing_loop, bhv_flame_floating_landing_loop };
        for (int kind = 0; kind < 2; kind++) {
            begin();
            object.oBehParams2ndByte = 1;
            object.oTimer = lifetimes[fight] - 1;
            loops[kind]();
            EXPECT_EQ(object.activeFlags, ACTIVE_FLAG_ACTIVE);
            object.oTimer = lifetimes[fight];
            loops[kind]();
            EXPECT_EQ(object.activeFlags, ACTIVE_FLAG_DEACTIVATED);
        }
        begin();
        object.oTimer = 299;
        bhv_flame_floating_landing_loop();
        EXPECT_EQ(object.activeFlags, ACTIVE_FLAG_ACTIVE);
        object.oTimer = 300;
        bhv_flame_floating_landing_loop();
        EXPECT_EQ(object.activeFlags, ACTIVE_FLAG_DEACTIVATED);
    }
    gCurrLevelNum = 0;
    return TRUE;
}

static int blue_fire_gravity(void) {
    void (*initializers[])(void) = { bhv_flame_floating_landing_init, bhv_blue_bowser_flame_init,
                                     bhv_flame_bouncing_init };
    for (unsigned kind = 0; kind < sizeof(initializers) / sizeof(*initializers); kind++) {
        for (int variant = 0; variant < 3; variant++) {
            begin();
            object.oBehParams2ndByte = variant == 2 ? 2 : 1;
            suppliedRandom = variant * 0.45f;
            initializers[kind]();
            EXPECT_EQ(object.oGravity, -1.0f);
        }
    }
    return TRUE;
}
static int landing_impacts(void) {
    for (int arena = BOWSER_BP_BITDW; arena <= BOWSER_BP_BITS; arena++) {
        for (int cycle = 0; cycle < 3; cycle++) {
            begin();
            object.oBehParams2ndByte = arena;
            EXPECT_EQ(bowser_land(), FALSE);
            EXPECT_EQ(impactCalls, 0);
            object.oMoveFlags = OBJ_MOVE_LANDED;
            object.oForwardVel = 50;
            object.oVelY = -20;
            EXPECT_EQ(bowser_land(), TRUE);
            EXPECT_EQ(impactCalls, 1);
            EXPECT_EQ(object.oForwardVel, 0);
            EXPECT_EQ(object.oVelY, 0);
            object.oMoveFlags = OBJ_MOVE_ON_GROUND;
            EXPECT_EQ(bowser_land(), FALSE);
            EXPECT_EQ(impactCalls, 1);
        }
    }
    return TRUE;
}
static int staggered_fire_attacks(void) {
    for (int sky = 0; sky < 2; sky++) {
        for (int cycle = 0; cycle < 3; cycle++) {
            begin();
            object.oAction = sky ? BOWSER_ACT_SPIT_FIRE_INTO_SKY : BOWSER_ACT_BREATH_FIRE;
            struct Object emitter = { .parentObj = &object };
            // Fresh native breath entry starts at animation frame -1; asset length is 92.
            suppliedAnimation.loopEnd = sky ? 60 : 92;
            f32 frame = -1;
            int firstSpawn = -1;
            int lastSpawn = -1;
            int completed = FALSE;
            for (int tick = 0; tick < 700; tick++) {
                object.oTimer = tick;
                object.header.gfx.animInfo.animFrame = (s32) frame;
                int before = spawnedCount;
                if (sky) {
                    bowser_act_spit_fire_into_sky();
                } else {
                    bowser_act_breath_fire();
                    gCurrentObject = &emitter;
                    bhv_bowser_flame_spawn_loop();
                    gCurrentObject = &object;
                }
                if (spawnedCount != before) {
                    EXPECT_EQ(spawnedCount - before, 1);
                    if (lastSpawn >= 0) {
                        EXPECT_EQ(tick - lastSpawn, sky ? 8 : 2);
                    }
                    if (firstSpawn < 0) {
                        firstSpawn = tick;
                    }
                    lastSpawn = tick;
                }
                f32 expectedSpeed =
                    sky && tick != 0 && (s32) frame > 24 && (s32) frame < 36 ? 0.125f : 1.0f;
                EXPECT_EQ(animationSpeed == expectedSpeed, TRUE);
                frame += animationSpeed;
                if (object.oAction == BOWSER_ACT_DEFAULT) {
                    completed = TRUE;
                    if (sky) {
                        EXPECT_EQ(firstSpawn, 26);
                        EXPECT_EQ(lastSpawn, 106);
                        EXPECT_EQ(tick, 136);
                    } else {
                        EXPECT_EQ(firstSpawn, 46);
                        EXPECT_EQ(lastSpawn, 84);
                        EXPECT_EQ(tick, 91);
                    }
                    break;
                }
            }
            EXPECT_EQ(completed, TRUE);
            EXPECT_EQ(spawnedCount, sky ? 11 : 20);
            for (int i = 0; i < spawnedCount; i++) {
                EXPECT_EQ(children[i].oBehParams2ndByte, sky && i == 10);
            }
            // An interrupted breath stops its child, then a new breath can emit again.
            if (!sky) {
                object.oSoundStateID = BOWSER_ANIM_GRABBED;
                object.header.gfx.animInfo.animFrame = 45;
                gCurrentObject = &emitter;
                bhv_bowser_flame_spawn_loop();
                EXPECT_EQ(spawnedCount, 20);
                object.oSoundStateID = BOWSER_ANIM_BREATH;
                bhv_bowser_flame_spawn_loop();
                EXPECT_EQ(spawnedCount, 21);
                gCurrentObject = &object;
            }
        }
    }
    return TRUE;
}
static int staggered_blue_groups(void) {
    for (int cycle = 0; cycle < 3; cycle++) {
        begin();
        if (cycle == 2) {
            gCurrLevelNum = LEVEL_BOWSER_3;
        } else if (cycle == 1) {
            gCurrLevelNum = LEVEL_BOWSER_2;
        } else {
            gCurrLevelNum = LEVEL_BOWSER_1;
        }
        int count = cycle == 2 ? 12 : 24;
        int lastSpawn = -1;
        int birthTicks[24];
        int peakActive = 0;
        for (int tick = 0; tick <= 314; tick++) {
            object.oTimer = tick;
            int before = spawnedCount;
            if (object.activeFlags != ACTIVE_FLAG_DEACTIVATED) {
                bhv_blue_flames_group_loop();
            }
            if (spawnedCount != before) {
                EXPECT_EQ(spawnedCount - before, 3);
                if (lastSpawn >= 0) {
                    EXPECT_EQ(tick - lastSpawn, 2);
                }
                lastSpawn = tick;
                for (int index = before; index < spawnedCount; index++) {
                    birthTicks[index] = tick;
                    EXPECT_EQ(children[index].header.gfx.scale[0], 5.0f - (index / 3) * 0.5f);
                    EXPECT_EQ(children[index].oMoveAngleYaw, (index % 3) * DEGREES(120));
                }
            }
            EXPECT_EQ(object.activeFlags,
                      tick < count / 3 * 2 ? ACTIVE_FLAG_ACTIVE : ACTIVE_FLAG_DEACTIVATED);
            int active = 0;
            for (int index = 0; index < spawnedCount; index++) {
                if (children[index].activeFlags != ACTIVE_FLAG_DEACTIVATED) {
                    gCurrentObject = &children[index];
                    o->oTimer = tick - birthTicks[index];
                    bhv_flame_bouncing_loop();
                    active += o->activeFlags != ACTIVE_FLAG_DEACTIVATED;
                }
            }
            gCurrentObject = &object;
            if (active > peakActive) {
                peakActive = active;
            }
            if (tick == 314) {
                EXPECT_EQ(active, 0);
            }
        }
        EXPECT_EQ(spawnedCount, count);
        EXPECT_EQ(peakActive, count);
    }
    return TRUE;
}
static int post_attack_center_jump(void) {
    begin();
    object.oHealth = 3;
    object.oBowserGrabCooldown = 300;
    object.oFloor = &teleportFloor;
    object.oPosY = object.oFloorHeight = 0;
    object.oPosX = 2000;
    object.oGravity = -4;
    object.oVelY = 2.8f;
    object.oMoveFlags = OBJ_MOVE_ON_GROUND;
    object.oBowserPostAttackFacingTimer = -1;
    object.oAction = BOWSER_ACT_WALK_TO_MARIO;
    for (int update = 0; update < 200; update++) {
        bowser_update_post_attack_facing(BOWSER_ACT_WALK_TO_MARIO, 0);
        EXPECT_EQ(object.oBowserPostAttackFacingTimer, -1);
    }
    object.oAction = BOWSER_ACT_DEFAULT;
    bowser_update_post_attack_facing(BOWSER_ACT_BREATH_FIRE, 0);
    EXPECT_EQ(object.oBowserPostAttackFacingTimer, 0);
    EXPECT_EQ(object.oBowserGrabCooldown, 0);
    object.oAction = BOWSER_ACT_WALK_TO_MARIO;
    for (int update = 1; update < 150; update++) {
        bowser_update_post_attack_facing(BOWSER_ACT_WALK_TO_MARIO, 0);
        EXPECT_EQ(object.oAction, BOWSER_ACT_WALK_TO_MARIO);
        EXPECT_EQ(object.oBowserPostAttackFacingTimer, update);
    }
    bowser_update_post_attack_facing(BOWSER_ACT_WALK_TO_MARIO, 0);
    EXPECT_EQ(object.oAction, BOWSER_ACT_BIG_JUMP);
    EXPECT_EQ(object.oSubAction, 3);
    EXPECT_EQ(object.oVelY, 80);
    EXPECT_EQ(centerJumpRoars, 1);
    EXPECT_EQ(object.oForwardVel > 0, TRUE);
    EXPECT_EQ(object.oBowserPostAttackFacingTimer, -1);
    object.oAction = BOWSER_ACT_WALK_TO_MARIO;
    object.oBowserPostAttackFacingTimer = 149;
    object.oBowserStatus = BOWSER_STATUS_ANGLE_MARIO;
    bowser_update_post_attack_facing(BOWSER_ACT_WALK_TO_MARIO, 0);
    EXPECT_EQ(object.oBowserPostAttackFacingTimer, 0);
    object.oBowserStatus = 0;
    object.oVelY = 0;
    object.oBowserPostAttackFacingTimer = 149;
    object.oMoveFlags = OBJ_MOVE_IN_AIR;
    bowser_update_post_attack_facing(BOWSER_ACT_WALK_TO_MARIO, 0);
    EXPECT_EQ(object.oAction, BOWSER_ACT_WALK_TO_MARIO);
    EXPECT_EQ(object.oBowserPostAttackFacingTimer, 150);
    object.oMoveFlags = OBJ_MOVE_ON_GROUND;
    bowser_update_post_attack_facing(BOWSER_ACT_WALK_TO_MARIO, 0);
    EXPECT_EQ(object.oAction, BOWSER_ACT_BIG_JUMP);
    const s32 interrupts[] = { BOWSER_ACT_HIT_MINE, BOWSER_ACT_WAIT, BOWSER_ACT_DEAD,
                               BOWSER_ACT_BREATH_FIRE };
    for (unsigned i = 0; i < sizeof(interrupts) / sizeof(*interrupts); i++) {
        object.oBowserPostAttackFacingTimer = 149;
        object.oAction = interrupts[i];
        bowser_update_post_attack_facing(BOWSER_ACT_WALK_TO_MARIO, 0);
        EXPECT_EQ(object.oBowserPostAttackFacingTimer, -1);
    }
    object.oAction = BOWSER_ACT_DEFAULT;
    object.oBowserGrabCooldown = 300;
    bowser_update_post_attack_facing(BOWSER_ACT_TELEPORT, BOWSER_SUB_ACT_TELEPORT_START);
    EXPECT_EQ(object.oBowserGrabCooldown, 300);
    object.oOpacity = 100;
    bowser_update_post_attack_facing(BOWSER_ACT_TELEPORT, BOWSER_SUB_ACT_TELEPORT_STOP);
    EXPECT_EQ(object.oBowserGrabCooldown, 300);
    object.oOpacity = 255;
    bowser_update_post_attack_facing(BOWSER_ACT_TELEPORT, BOWSER_SUB_ACT_TELEPORT_STOP);
    EXPECT_EQ(object.oBowserGrabCooldown, 0);
    object.oAction = BOWSER_ACT_DEFAULT;
    object.oHeldState = HELD_HELD;
    object.oBowserPostAttackFacingTimer = 149;
    bowser_update_post_attack_facing(BOWSER_ACT_BREATH_FIRE, 0);
    EXPECT_EQ(object.oBowserPostAttackFacingTimer, -1);
    for (int fight = BOWSER_BP_BITDW; fight <= BOWSER_BP_BITS; fight++) {
        begin();
        int timeout = fight == BOWSER_BP_BITS ? 120 : 150;
        object.oBehParams2ndByte = fight;
        object.oHealth = 3;
        object.oFloor = &teleportFloor;
        object.oMoveFlags = OBJ_MOVE_ON_GROUND;
        object.oPosY = object.oFloorHeight = 0;
        object.oAction = BOWSER_ACT_WALK_TO_MARIO;
        object.oBowserPostAttackFacingTimer = 0;
        for (int frame = 1; frame < timeout; frame++) {
            bowser_update_post_attack_facing(BOWSER_ACT_WALK_TO_MARIO, 0);
            EXPECT_EQ(object.oAction, BOWSER_ACT_WALK_TO_MARIO);
        }
        bowser_update_post_attack_facing(BOWSER_ACT_WALK_TO_MARIO, 0);
        EXPECT_EQ(object.oAction, BOWSER_ACT_BIG_JUMP);
    }
    return TRUE;
}
static int floor_spit_burst(void) {
    const s32 fights[] = { BOWSER_BP_BITDW, BOWSER_BP_BITFS, BOWSER_BP_BITS };
    for (unsigned fight = 0; fight < sizeof(fights) / sizeof(*fights); fight++) {
        int cycleFrames = fight == 2 ? 10 : 15;
        int shotFrame = fight == 2 ? 2 : 4;
        for (int count = 3; count <= 9; count++) {
            begin();
            object.oAction = object.oPrevAction = BOWSER_ACT_SPIT_FIRE_ONTO_FLOOR;
            object.oBehParams2ndByte = fights[fight];
            object.oDistanceToMario = 500;
            object.oAngleToMario = 0x4000;
            suppliedRandom = (count - 3 + .5f) / 7.0f;
            for (int update = 1; update <= count * cycleFrames; update++) {
                object.oTimer = update - 1;
                bowser_act_spit_fire_onto_floor();
                // Production behavior scripts reset entry clocks after an action change.
                if (object.oAction != object.oPrevAction) {
                    object.oTimer = object.oSubAction = 0;
                    object.oPrevAction = object.oAction;
                }
                int emitted = (update + cycleFrames - shotFrame) / cycleFrames;
                EXPECT_EQ(spawnedCount, emitted);
                EXPECT_EQ(object.oBowserRandSplitFloor, count);
                if (update == shotFrame - 1) {
                    EXPECT_EQ(object.oMoveAngleYaw, 0);
                } else if (update == shotFrame) {
                    EXPECT_EQ(object.oMoveAngleYaw, fight == 2 ? DEGREES(10) : 0x1000);
                }
                if (update < count * cycleFrames) {
                    EXPECT_EQ(object.oAction, BOWSER_ACT_SPIT_FIRE_ONTO_FLOOR);
                }
                // New RNG must not reroll the established burst length.
                suppliedRandom = .99f;
            }
            EXPECT_EQ(object.oAction, fight == 2   ? BOWSER_ACT_SPIT_FIRE_INTO_SKY
                                      : fight == 1 ? BOWSER_ACT_SPIT_FIRE_ONTO_FLOOR
                                                   : BOWSER_ACT_QUICK_JUMP);
            EXPECT_EQ(object.oBowserIsReacting, TRUE);
            EXPECT_EQ(object.oMoveAngleYaw, object.oAngleToMario);
            EXPECT_EQ(object.oSubAction, 0);
            for (int flame = 0; flame < count; flame++) {
                EXPECT_EQ(children[flame].behavior == bhvBlueBowserFlame, TRUE);
                EXPECT_EQ(children[flame].oBehParams2ndByte, 1);
                EXPECT_EQ((children[flame].oBehParams & BOWSER_FLAME_NO_SPLIT) != 0, TRUE);
            }
        }
    }
    // Picking another spit attack must restart and reroll its entry state.
    begin();
    object.oAction = object.oPrevAction = BOWSER_ACT_SPIT_FIRE_ONTO_FLOOR;
    object.oBehParams2ndByte = BOWSER_BP_BITS;
    object.oDistanceToMario = 500;
    suppliedRandom = 0;
    for (int update = 0; update < 30; update++) {
        object.oTimer = update;
        if (update == 29) {
            suppliedRandom = .875f;
        }
        bowser_act_spit_fire_onto_floor();
    }
    EXPECT_EQ(object.oAction, BOWSER_ACT_SPIT_FIRE_ONTO_FLOOR);
    EXPECT_EQ(object.oPrevAction, -1);
    object.oTimer = object.oSubAction = 0;
    object.oPrevAction = object.oAction;
    suppliedRandom = .99f;
    bowser_act_spit_fire_onto_floor();
    EXPECT_EQ(object.oBowserRandSplitFloor, 9);
    EXPECT_EQ(object.oBowserTimer, 1);
    EXPECT_EQ(spawnedCount, 3);
    return TRUE;
}

static int charge_timing(void) {
    const s32 fights[] = { BOWSER_BP_BITDW, BOWSER_BP_BITFS, BOWSER_BP_BITS };
    for (unsigned fight = 0; fight < sizeof(fights) / sizeof(*fights); fight++) {
        int startup = fight == 2 ? 9 : 14;
        int recovery = fight == 2 ? 15 : 35;
        f32 braking = fight == 2 ? 3.5f : 2.5f;
        begin();
        object.oAction = BOWSER_ACT_CHARGE_MARIO;
        object.oBehParams2ndByte = fights[fight];
        object.oSubAction = BOWSER_SUB_ACT_CHARGE_START;
        object.oBowserTimer = 99;
        object.oForwardVel = 200;
        object.header.gfx.animInfo.animFrame = -1;
        for (int update = 0; update < startup; update++) {
            object.oTimer = update;
            bowser_act_charge_mario();
            EXPECT_EQ(object.oSubAction,
                      update < startup - 1 ? BOWSER_SUB_ACT_CHARGE_START : BOWSER_SUB_ACT_CHARGE_RUN);
            EXPECT_EQ(object.oForwardVel, 0);
        }
        EXPECT_EQ(object.oBowserTimer, 0);
        EXPECT_EQ(fabsf(animationSpeed - 23.0f / startup) < .0001f, TRUE);
        for (int runningFrame = 0; runningFrame < 20; runningFrame++) {
            object.oTimer = startup + runningFrame;
            object.oForwardVel = -99; // Movement may alter speed between AI updates.
            object.oMoveAngleYaw = 0;
            object.oAngleToMario = DEGREES(10);
            bowser_act_charge_mario();
            EXPECT_EQ(object.oMoveAngleYaw, fight == 2 ? (s16) DEGREES(3.2f) : 0x200);
            EXPECT_EQ(fabsf(object.oForwardVel
                            - ((fight == 2 ? 65.0f : 60.0f) + runningFrame * (fight == 2 ? .4f : .2f)))
                          < .0001f,
                      TRUE);
        }
        EXPECT_EQ(object.oSubAction, BOWSER_SUB_ACT_CHARGE_RUN);
        object.header.gfx.animInfo.animFrame = suppliedAnimation.loopEnd - 2;
        for (int cycle = 0; cycle < 6; cycle++) {
            object.oTimer++;
            bowser_act_charge_mario();
            EXPECT_EQ(object.oSubAction,
                      cycle < 5 ? BOWSER_SUB_ACT_CHARGE_RUN : BOWSER_SUB_ACT_CHARGE_SLIP);
        }
        int brakingFrames = (int) (object.oForwardVel / braking) + 1;
        for (int update = 0; update < brakingFrames; update++) {
            f32 previousSpeed = object.oForwardVel;
            bowser_act_charge_mario();
            EXPECT_EQ(object.oForwardVel, MAX(previousSpeed - braking, 0.0f));
            EXPECT_EQ(object.oSubAction, update < brakingFrames - 1 ? BOWSER_SUB_ACT_CHARGE_SLIP
                                                                    : BOWSER_SUB_ACT_CHARGE_END);
        }
        EXPECT_EQ(object.oBowserTimer, 0);
        for (int update = 1; update <= recovery; update++) {
            bowser_act_charge_mario();
            EXPECT_EQ(object.oAction, update < recovery ? BOWSER_ACT_CHARGE_MARIO : BOWSER_ACT_DEFAULT);
            EXPECT_EQ(object.oForwardVel, 0);
        }
    }
    return TRUE;
}

static int quick_jump_timing(void) {
    for (int fight = 0; fight < 3; fight++) {
        begin();
        object.oBehParams2ndByte = fight;
        int startup, airborne, verticalSpeed;
        if (fight == BOWSER_BP_BITFS) {
            startup = 8;
            airborne = 22;
            verticalSpeed = 44;
        } else if (fight == BOWSER_BP_BITS) {
            startup = 6;
            airborne = 19;
            verticalSpeed = 38;
        } else {
            startup = 10;
            airborne = 25;
            verticalSpeed = 50;
        }
        object.oAction = BOWSER_ACT_QUICK_JUMP;
        object.oPosY = object.oFloorHeight = 0;
        object.oGravity = -4;
        for (int update = 0; update < startup; update++) {
            object.oTimer = update;
            bowser_act_quick_jump();
            EXPECT_EQ(object.oSubAction, update < startup - 1 ? 0 : 1);
            EXPECT_EQ(fabsf(animationSpeed - 12.5f / startup) < 0.0001f, TRUE);
        }
        EXPECT_EQ(object.oVelY, verticalSpeed);
        EXPECT_EQ(object.oForwardVel, 50);
        simulateMovement = TRUE;
        for (int update = 1; update <= airborne; update++) {
            cur_obj_move_standard(-78);
            EXPECT_EQ((object.oMoveFlags & OBJ_MOVE_LANDED) != 0, update == airborne);
        }
        bowser_act_quick_jump();
        EXPECT_EQ(object.oSubAction, 2);
        EXPECT_EQ(spawnedCount, fight == BOWSER_BP_BITS ? 2 : 1);
        EXPECT_EQ(children[0].behavior == bhvBowserShockWave, TRUE);
        EXPECT_EQ(children[0].oBehParams2ndByte, BOWSER_SHOCKWAVE_LIGHT);
        EXPECT_EQ(children[0].oPosY, object.oFloorHeight);
        EXPECT_EQ(object.oForwardVel, 0);
        EXPECT_EQ(impactCalls, 1);
        EXPECT_EQ(fabsf(animationSpeed - 1.2f) < 0.0001f, TRUE);
        if (fight == BOWSER_BP_BITS) {
            EXPECT_EQ(children[1].oBehParams2ndByte, BOWSER_SHOCKWAVE_DELAYED);
        }
        int recovery = 15;
        for (int update = 1; update <= recovery; update++) {
            bowser_act_quick_jump();
            EXPECT_EQ(object.oAction, update < recovery ? BOWSER_ACT_QUICK_JUMP : BOWSER_ACT_DEFAULT);
            EXPECT_EQ(impactCalls, 1);
            EXPECT_EQ(spawnedCount, fight == BOWSER_BP_BITS ? 2 : 1);
        }
    }
    return TRUE;
}
static int fight_two_move_properties(void) {
    const f32 speeds[] = { 0, 24.9f, 25, -25, -24.9f };
    for (unsigned speed = 0; speed < sizeof(speeds) / sizeof(*speeds); speed++) {
        int on = 0, front = 0, behind = 0;
        for (int sample = 0; sample < 1000; sample++) {
            begin();
            object.oBehParams2ndByte = BOWSER_BP_BITFS;
            heldMario.pos[1] = 100;
            heldMario.forwardVel = speeds[speed];
            suppliedRandom = (sample + .5f) / 1000;
            EXPECT_EQ(bowser_teleport_to_mario(), TRUE);
            on += object.oPosZ == 0;
            front += object.oPosZ == 400;
            behind += object.oPosZ == -400;
        }
        int slow = ABS(speeds[speed]) < 25;
        EXPECT_EQ(on, slow ? 600 : 0);
        EXPECT_EQ(front, slow ? 300 : 600);
        EXPECT_EQ(behind, slow ? 100 : 400);
    }
    begin();
    object.oBehParams2ndByte = BOWSER_BP_BITFS;
    for (int fadeIn = 0; fadeIn < 2; fadeIn++) {
        object.oAction = BOWSER_ACT_TELEPORT;
        object.oOpacity = fadeIn ? 0 : 255;
        object.oBowserTargetOpacity = fadeIn ? 255 : 0;
        for (int tick = 1; tick <= 8; tick++) {
            bowser_update_opacity();
            EXPECT_EQ(object.oOpacity == object.oBowserTargetOpacity, tick == 8);
        }
    }
    object.oSubAction = BOWSER_SUB_ACT_TELEPORT_START;
    object.oFloor = &teleportFloor;
    object.oMoveFlags = OBJ_MOVE_ON_GROUND;
    object.oPosY = object.oFloorHeight = 0;
    object.oOpacity = 0;
    bowser_act_teleport();
    for (int tick = 1; tick <= 14; tick++) {
        bowser_act_teleport();
        EXPECT_EQ(object.oSubAction,
                  tick < 14 ? BOWSER_SUB_ACT_TELEPORT_MOVE : BOWSER_SUB_ACT_TELEPORT_STOP);
    }
    for (int fight = 0; fight < 3; fight++) {
        begin();
        object.oBehParams2ndByte = fight;
        bowser_spawn_center_return_flames();
        EXPECT_EQ(spawnedCount, fight == BOWSER_BP_BITFS ? 8 : 0);
        for (int flame = 0; flame < spawnedCount; flame++) {
            EXPECT_EQ(children[flame].behavior == bhvFlameFloatingLanding, TRUE);
            EXPECT_EQ(children[flame].oBehParams2ndByte, 0);
            EXPECT_EQ(children[flame].oBehParams, BOWSER_FLAME_DOUBLE_LIFETIME);
        }
    }
    begin();
    object.oTimer = 300;
    EXPECT_EQ(bowser_flame_should_despawn(300), TRUE);
    object.oBehParams = BOWSER_FLAME_DOUBLE_LIFETIME;
    EXPECT_EQ(bowser_flame_should_despawn(300), FALSE);
    object.oTimer = 600;
    EXPECT_EQ(bowser_flame_should_despawn(300), TRUE);
    begin();
    object.oTimer = 21;
    object.oBehParams2ndByte = 1;
    object.oBehParams = BOWSER_FLAME_NO_SPLIT;
    bhv_blue_bowser_flame_loop();
    EXPECT_EQ(spawnedCount, 0);
    EXPECT_EQ(object.activeFlags, ACTIVE_FLAG_ACTIVE);
    EXPECT_EQ(object.oMoveAngleYaw, BOWSER_BLUE_FLAME_TURN_SPEED);
    EXPECT_EQ(object.oForwardVel, BOWSER_SPIT_FLAME_SPEED);
    object.oMoveFlags = OBJ_MOVE_LANDED;
    bhv_blue_bowser_flame_loop();
    EXPECT_EQ(object.oAction, 1);
    EXPECT_EQ(object.oForwardVel, BOWSER_SPIT_FLAME_LANDING_SPEED);
    EXPECT_EQ(object.oVelY, 0);
    EXPECT_EQ(object.oGravity, -1);
    EXPECT_EQ(object.oBounciness, 0);
    EXPECT_EQ(spawnedCount, 0);
    object.oMoveFlags = OBJ_MOVE_ON_GROUND;
    for (int frame = 1; frame <= 12; frame++) {
        object.oTimer = frame - 1;
        object.oForwardVel = 100;
        bhv_blue_bowser_flame_loop();
        EXPECT_EQ(object.oForwardVel, MAX(15, 20 - frame));
    }
    object.oTimer = 299;
    object.oMoveAngleYaw = 0;
    bhv_blue_bowser_flame_loop();
    EXPECT_EQ(object.oMoveAngleYaw, BOWSER_SPIT_FLAME_GROUND_TURN_SPEED);
    EXPECT_EQ(object.oForwardVel, BOWSER_SPIT_FLAME_GROUND_SPEED);
    EXPECT_EQ(object.activeFlags, ACTIVE_FLAG_ACTIVE);
    EXPECT_EQ(spawnedCount, 0);
    object.oTimer = 300;
    bhv_blue_bowser_flame_loop();
    EXPECT_EQ(object.activeFlags, ACTIVE_FLAG_DEACTIVATED);
    object.oBehParams = 0;
    bhv_blue_bowser_flame_loop();
    EXPECT_EQ(spawnedCount, 2);
    begin();
    object.oBehParams = BOWSER_FLAME_NO_SPLIT;
    object.oBehParams2ndByte = 1;
    object.oAction = 1;
    marioObject.oPosX = object.oPosX;
    marioObject.oPosZ = object.oPosZ - 1000;
    for (int frame = 1; frame <= 24; frame++) {
        s16 previousYaw = object.oMoveAngleYaw;
        object.oTimer = frame - 1;
        bhv_blue_bowser_flame_loop();
        EXPECT_EQ(abs_angle_diff(previousYaw, object.oMoveAngleYaw)
                      <= BOWSER_SPIT_FLAME_GROUND_TURN_SPEED,
                  TRUE);
        EXPECT_EQ((s16) object.oMoveAngleYaw == (s16) 0x8000, frame == 24);
        EXPECT_EQ(object.oForwardVel, MAX(15, 20 - frame));
        EXPECT_EQ(spawnedCount, 0);
    }
    begin();
    object.header.gfx.animInfo.animFrame = 9;
    EXPECT_EQ(bowser_set_anim_jump(), FALSE);
    object.header.gfx.animInfo.animFrame = 10;
    EXPECT_EQ(bowser_set_anim_jump(), TRUE);
    object.header.gfx.animInfo.animFrame = 11;
    EXPECT_EQ(bowser_set_anim_jump(), FALSE);
    int lifetimes[2];
    for (int doubled = 0; doubled < 2; doubled++) {
        begin();
        object.oBehParams = doubled ? BOWSER_FLAME_DOUBLE_LIFETIME : 0;
        object.oAction = 1;
        object.oFlameScale = 8;
        object.activeFlags = ACTIVE_FLAG_ACTIVE;
        int tick;
        for (tick = 0; tick < 1000 && object.activeFlags != ACTIVE_FLAG_DEACTIVATED; tick++) {
            object.oTimer = tick;
            bhv_flame_bowser_loop();
        }
        lifetimes[doubled] = tick;
    }
    EXPECT_EQ(abs(lifetimes[1] - lifetimes[0] * 2) <= 2, TRUE);
    begin();
    object.oBehParams = BOWSER_FLAME_DOUBLE_LIFETIME;
    object.oMoveFlags = OBJ_MOVE_LANDED;
    bhv_flame_floating_landing_loop();
    EXPECT_EQ(spawnedCount, 1);
    EXPECT_EQ(children[0].behavior == bhvFlameLargeBurningOut, TRUE);
    EXPECT_EQ(children[0].oBehParams, BOWSER_FLAME_DOUBLE_LIFETIME);
    begin();
    object.oBehParams2ndByte = BOWSER_BP_BITFS;
    object.oAction = BOWSER_ACT_BREATH_FIRE;
    struct Object emitter = { .parentObj = &object };
    for (int frame = 1; frame <= 72; frame++) {
        object.oTimer = frame - 1;
        bowser_act_breath_fire();
        gCurrentObject = &emitter;
        bhv_bowser_flame_spawn_loop();
        gCurrentObject = &object;
        int expectedShots;
        if (frame <= 28) {
            expectedShots = 0;
        } else if (frame < 67) {
            expectedShots = (frame - 29) / 2 + 1;
        } else {
            expectedShots = 20;
        }
        EXPECT_EQ(spawnedCount, expectedShots);
        EXPECT_EQ(object.oAction, frame < 72 ? BOWSER_ACT_BREATH_FIRE : BOWSER_ACT_DEFAULT);
    }
    return TRUE;
}
static int accidental_jump_recovery(void) {
    begin();
    object.oBehParams2ndByte = BOWSER_BP_BITS;
    object.oHomeY = 307;
    object.oAction = BOWSER_ACT_QUICK_JUMP;
    object.oPosY = 7;
    EXPECT_EQ(bowser_check_fallen_off_stage(), FALSE);
    object.oPosY = 6;
    EXPECT_EQ(bowser_check_fallen_off_stage(), TRUE);
    object.oAction = BOWSER_ACT_BIG_JUMP;
    EXPECT_EQ(bowser_check_fallen_off_stage(), TRUE);
    object.oAction = BOWSER_ACT_JUMP_ONTO_STAGE;
    object.oSubAction = BOWSER_SUB_ACT_JUMP_ON_STAGE_IDLE;
    EXPECT_EQ(bowser_check_fallen_off_stage(), FALSE);
    object.oSubAction = BOWSER_SUB_ACT_JUMP_ON_STAGE_START;
    EXPECT_EQ(bowser_check_fallen_off_stage(), FALSE);
    object.oSubAction = BOWSER_SUB_ACT_JUMP_ON_STAGE_LAND;
    EXPECT_EQ(bowser_check_fallen_off_stage(), TRUE);
    object.oVelY = 150;
    EXPECT_EQ(bowser_check_fallen_off_stage(), FALSE);
    object.oVelY = 0;
    object.oPosY = 307;
    EXPECT_EQ(bowser_check_fallen_off_stage(), FALSE);
    object.oAction = BOWSER_ACT_QUICK_JUMP;
    object.oMoveFlags = OBJ_MOVE_ON_GROUND;
    object.oFloorType = SURFACE_DEATH_PLANE;
    EXPECT_EQ(bowser_check_fallen_off_stage(), TRUE);
    object.oAction = BOWSER_ACT_DEAD;
    object.oPosY = -6000;
    EXPECT_EQ(bowser_check_fallen_off_stage(), FALSE);
    object.oAction = BOWSER_ACT_BIG_JUMP;
    object.oMoveFlags = 0;
    object.oFloorType = 0;
    object.oPosY = 6;
    object.oBehParams2ndByte = BOWSER_BP_BITFS;
    EXPECT_EQ(bowser_check_fallen_off_stage(), FALSE);
    return TRUE;
}
static int return_jump_platform_break(void) {
    begin();
    object.oAction = BOWSER_BITS_PLAT_ACT_CHECK;
    object.oBitsPlatformBowser = &bowser;
    bowser.oBehParams2ndByte = BOWSER_BP_BITS;
    bowser.platform = &object;
    bowser.oAction = BOWSER_ACT_JUMP_ONTO_STAGE;
    bowser.oBowserStatus = BOWSER_STATUS_BIG_JUMP;
    falling_bowser_plat_act_check();
    EXPECT_EQ(object.oAction, BOWSER_BITS_PLAT_ACT_CHECK);
    EXPECT_EQ(spawnedCount, 0);
    bowser.oAction = BOWSER_ACT_BIG_JUMP;
    falling_bowser_plat_act_check();
    EXPECT_EQ(object.oAction, BOWSER_BITS_PLAT_ACT_FALL);
    EXPECT_EQ(spawnedCount, 1);
    EXPECT_EQ(children[0].oBehParams2ndByte, BOWSER_SHOCKWAVE_LIGHT);
    EXPECT_EQ(impactCalls, 1);
    begin();
    object.oAction = BOWSER_BITS_PLAT_ACT_CHECK;
    object.oBitsPlatformBowser = &bowser;
    bowser.platform = NULL;
    bowser.oAction = BOWSER_ACT_BIG_JUMP;
    bowser.oBowserStatus = BOWSER_STATUS_BIG_JUMP;
    falling_bowser_plat_act_check();
    EXPECT_EQ(object.oAction, BOWSER_BITS_PLAT_ACT_CHECK);
    EXPECT_EQ(spawnedCount, 0);
    return TRUE;
}
static int walking_interrupt(void) {
    for (int facing = 0; facing < 2; facing++) {
        for (int fire = 0; fire < 2; fire++) {
            begin();
            object.oBehParams2ndByte = BOWSER_BP_BITS;
            object.oHealth = 5;
            object.oAction = BOWSER_ACT_WALK_TO_MARIO;
            object.oBowserStatus = facing ? BOWSER_STATUS_ANGLE_MARIO : 0;
            queuedRandom[0] = 0;
            queuedRandom[1] = fire ? 0.9f : 0.899f;
            queuedRandomCount = 2;
            bowser_act_walk_to_mario();
            EXPECT_EQ(object.oAction, facing ? BOWSER_ACT_WALK_TO_MARIO
                                      : fire ? BOWSER_ACT_SPIT_FIRE_ONTO_FLOOR
                                             : BOWSER_ACT_TELEPORT);
            EXPECT_EQ(queuedRandomIndex, facing ? 0 : 2);
        }
    }
    begin();
    object.oBehParams2ndByte = BOWSER_BP_BITS;
    object.oHealth = 5;
    object.oAction = BOWSER_ACT_WALK_TO_MARIO;
    suppliedRandom = 1.0f / 600.0f;
    bowser_act_walk_to_mario();
    EXPECT_EQ(object.oAction, BOWSER_ACT_WALK_TO_MARIO);
    begin();
    object.oBehParams2ndByte = BOWSER_BP_BITS;
    object.oHealth = 5;
    object.oAction = BOWSER_ACT_WALK_TO_MARIO;
    queuedRandom[0] = 1.0f / 600.0f - 0.000001f;
    queuedRandom[1] = 0;
    queuedRandomCount = 2;
    bowser_act_walk_to_mario();
    EXPECT_EQ(object.oAction, BOWSER_ACT_TELEPORT);
    for (int fight = BOWSER_BP_BITDW; fight <= BOWSER_BP_BITFS; fight++) {
        begin();
        object.oBehParams2ndByte = fight;
        object.oHealth = 3;
        object.oAction = BOWSER_ACT_WALK_TO_MARIO;
        object.oBowserStatus = BOWSER_STATUS_ANGLE_MARIO;
        suppliedRandom = 0;
        bowser_act_walk_to_mario();
        EXPECT_EQ(object.oAction,
                  fight == BOWSER_BP_BITFS ? BOWSER_ACT_TELEPORT : BOWSER_ACT_WALK_TO_MARIO);
    }
    return TRUE;
}
static int turn_speed_and_edge(void) {
    const f32 bases[] = { 5.625f, 6.0f, 6.4f };
    const f32 gains[] = { 0.1f, 0.1f, 0.1f };
    for (int fight = 0; fight < 3; fight++) {
        begin();
        object.oBehParams2ndByte = fight;
        for (int hits = 0; hits < 3 + fight; hits++) {
            object.oHealth = 3 + fight - hits;
            EXPECT_EQ(bowser_walk_turn_speed(), (s16) DEGREES(bases[fight] + hits * gains[fight]));
        }
    }
    begin();
    object.oBehParams2ndByte = BOWSER_BP_BITS;
    object.oAction = BOWSER_ACT_HIT_EDGE;
    for (int frame = 1; frame <= 20; frame++) {
        object.oTimer = frame - 1;
        bowser_act_hit_edge();
        EXPECT_EQ(object.oAction, frame < 20 ? BOWSER_ACT_HIT_EDGE : BOWSER_ACT_TURN_FROM_EDGE);
    }
    object.oHealth = 4;
    object.oMoveAngleYaw = 0;
    object.oAngleToMario = DEGREES(90);
    bowser_act_turn_from_edge();
    EXPECT_EQ(object.oMoveAngleYaw, (s16) DEGREES(6.5f));
    EXPECT_EQ(object.oAction, BOWSER_ACT_TURN_FROM_EDGE);
    for (int frame = 0; frame < 20; frame++) {
        bowser_act_turn_from_edge();
    }
    EXPECT_EQ(object.oMoveAngleYaw, object.oAngleToMario);
    EXPECT_EQ(object.oAction, BOWSER_ACT_DEFAULT);
    return TRUE;
}
static int arena_fire_spitter_waves(void) {
    begin();
    object.oBehParams2ndByte = BOWSER_BP_BITS;
    gCurrLevelNum = LEVEL_BOWSER_3;
    for (int point = 0; point < 5; point++) {
        bowser_arena_fire(&object, point);
    }
    struct Object bomb = { .oPosX = -2122, .oPosZ = -2912 };
    for (int event = 0; event < 2; event++) {
        int first = spawnedCount;
        bowser_arena_bomb_destroyed(&bomb);
        EXPECT_EQ(spawnedCount - first, 5);
        int seen[5] = { 0 };
        for (int child = first; child < spawnedCount; child++) {
            EXPECT_EQ(children[child].behavior == bhvBowserShockWave, TRUE);
            EXPECT_EQ(children[child].oBehParams2ndByte, BOWSER_SHOCKWAVE_LIGHT);
            EXPECT_EQ(children[child].oPosY, 307);
            for (int point = 0; point < 5; point++) {
                seen[point] += children[child].oPosX == sArenaPoints[point][0]
                               && children[child].oPosZ == sArenaPoints[point][2];
            }
        }
        for (int point = 0; point < 5; point++) {
            EXPECT_EQ(seen[point], 1);
        }
    }
    gCurrLevelNum = 0;
    return TRUE;
}

static int arena_enemy_survival(void) {
    const BehaviorScript *enemies[] = { bhvGoomba, bhvBobomb, bhvSmallBully, bhvHomingAmp, bhvSpiny };
    arenaFloorTest = 1;
    for (unsigned kind = 0; kind < sizeof(enemies) / sizeof(*enemies); kind++) {
        begin();
        object.behavior = enemies[kind];
        object.bowserArenaOwner = &bowser;
        object.oHomeX = 1200;
        object.oPosX = 3200;
        object.oForwardVel = 20;
        object.oMoveAngleYaw = object.oFaceAngleYaw = 0x4000;
        object.oPosY = 108;
        bowser_arena_recover_enemy(&object);
        EXPECT_EQ(object.oPosX, 3200);
        EXPECT_EQ(object.oPosY, 108);
        object.oPosY = 107;
        object.oAction = OBJ_ACT_LAVA_DEATH;
        bowser_arena_recover_enemy(&object);
        EXPECT_EQ(object.oPosX, 1200);
        EXPECT_EQ(object.oPosY, 307 + (kind == 3 ? 100 : 0));
        EXPECT_EQ(object.oForwardVel, 0);
        EXPECT_EQ((s16) object.oMoveAngleYaw, atan2s(-object.oPosZ, -object.oPosX));
        EXPECT_EQ(object.oFaceAngleYaw, object.oMoveAngleYaw);
        EXPECT_EQ(object.oAction, 0);
        EXPECT_EQ(object.activeFlags, ACTIVE_FLAG_ACTIVE);
        object.oPosX = 300;
        object.oPosY = 500;
        object.oForwardVel = 10;
        bowser_arena_recover_enemy(&object);
        EXPECT_EQ(object.oPosX, 300);
        EXPECT_EQ(object.oPosY, 500);
        EXPECT_EQ(object.oForwardVel, 10);
    }
    for (int kind = 0; kind < 7; kind++) {
        begin();
        object.behavior = bhvGoomba;
        object.bowserArenaOwner = &bowser;
        object.oHomeX = 1200;
        object.oPosX = 3200;
        if (kind == 0) {
            object.oGroundTwirlStunTimer = 10;
        } else if (kind == 1) {
            object.oGroundTwirlStunTimer = -1;
        } else {
            object.oGroundTwirlStunTimer = 0;
        }
        object.oGroundTwirlContactRebound = kind == 2;
        object.oGroundTwirlStunDuration = kind == 3 ? 30 : 0;
        object.oInteractStatus = kind == 4 ? INT_STATUS_WAS_ATTACKED : 0;
        object.oAction = 0;
        if (kind == 5) {
            object.behavior = bhvSmallBully;
            object.oAction = BULLY_ACT_KNOCKBACK;
        }
        if (kind == 6) {
            object.behavior = bhvBobomb;
            object.oAction = BOBOMB_ACT_LAUNCHED;
        }
        object.oPosX = 300;
        object.oPosY = 307;
        object.oMoveFlags = OBJ_MOVE_ON_GROUND;
        bowser_arena_recover_enemy(&object);
        EXPECT_EQ(object.bowserArenaFallAllowed, TRUE);
        object.oPosX = 3200;
        object.oPosY = 108;
        bowser_arena_recover_enemy(&object);
        EXPECT_EQ(object.oPosX, 3200);
        EXPECT_EQ(object.bowserArenaFallAllowed, TRUE);
        object.oGroundTwirlStunTimer = object.oGroundTwirlContactRebound =
            object.oGroundTwirlStunDuration = 0;
        object.oInteractStatus = 0;
        object.oAction = OBJ_ACT_DEATH_PLANE_DEATH;
        bowser_arena_recover_enemy(&object);
        EXPECT_EQ(object.oPosX, 3200);
        EXPECT_EQ(object.oAction, OBJ_ACT_DEATH_PLANE_DEATH);
        object.oPosX = 300;
        object.oPosY = 307;
        object.oAction = 0;
        bowser_arena_recover_enemy(&object);
        EXPECT_EQ(object.bowserArenaFallAllowed, FALSE);
        object.oPosX = 3200;
        object.oPosY = 107;
        bowser_arena_recover_enemy(&object);
        EXPECT_EQ(object.oPosX, 1200);
    }
    begin();
    object.behavior = bhvSmallBully;
    object.bowserArenaOwner = &bowser;
    object.oPosX = 3200;
    object.oPosY = 107;
    object.oAction = BULLY_ACT_KNOCKBACK;
    bowser_arena_recover_enemy(&object);
    EXPECT_EQ(object.activeFlags, ACTIVE_FLAG_DEACTIVATED);
    EXPECT_EQ(object.oPosX, 3200);
    begin();
    object.behavior = bhvEnemyLakitu;
    object.bowserArenaOwner = &bowser;
    object.oPosX = 3200;
    object.oPosY = 757;
    object.oForwardVel = 10;
    bowser_arena_recover_enemy(&object);
    EXPECT_EQ(object.oPosX, 3200);
    EXPECT_EQ(object.oPosY, 757);
    EXPECT_EQ(object.oForwardVel, 10);
    object.behavior = bhvSpiny;
    object.oPosY = 107;
    arenaFloorTest = 2;
    object.oPosX = 3200;
    bowser_arena_recover_enemy(&object);
    EXPECT_EQ(object.oPosX, 600);
    EXPECT_EQ(object.oHomeX, 600);
    object.oPosX = 3200;
    object.bowserArenaOwner = NULL;
    bowser_arena_recover_enemy(&object);
    EXPECT_EQ(object.oPosX, 3200);
    object.bowserArenaOwner = &bowser;
    object.oHeldState = HELD_HELD;
    bowser_arena_recover_enemy(&object);
    EXPECT_EQ(object.oPosX, 3200);
    object.oHeldState = HELD_FREE;
    object.activeFlags = ACTIVE_FLAG_DEACTIVATED;
    bowser_arena_recover_enemy(&object);
    EXPECT_EQ(object.activeFlags, ACTIVE_FLAG_DEACTIVATED);
    arenaFloorTest = 0;
    begin();
    object.behavior = bhvSpiny;
    object.bowserArenaOwner = &bowser;
    for (int update = 1; update < 900; update++) {
        object.oAction = update % 3;
        object.oTimer = 0;
        EXPECT_EQ(bowser_arena_phase_in(&object), FALSE);
        EXPECT_EQ(object.activeFlags, ACTIVE_FLAG_ACTIVE);
    }
    EXPECT_EQ(bowser_arena_phase_in(&object), TRUE);
    EXPECT_EQ(object.activeFlags, ACTIVE_FLAG_DEACTIVATED);
    EXPECT_EQ(spawnedCount, 0);
    begin();
    object.behavior = bhvSpiny;
    for (int update = 0; update < 901; update++) {
        EXPECT_EQ(bowser_arena_phase_in(&object), FALSE);
    }
    EXPECT_EQ(object.activeFlags, ACTIVE_FLAG_ACTIVE);
    return TRUE;
}

static int arena_enemy_distance_recovery(void) {
    begin();
    arenaFloorTest = 1;
    object.behavior = bhvBobomb;
    object.bowserArenaOwner = &bowser;
    object.oHomeX = 1200;
    object.oPosX = 3200;
    object.oPosY = 308;
    marioObject.oPosX = -3200;
    bobombSteps = 0;
    for (int frame = 0; frame < 3; frame++) {
        bhv_bobomb_loop();
        bowser_arena_recover_enemy(&object);
    }
    EXPECT_EQ(bobombSteps, 3);
    EXPECT_EQ(object.oPosX, 1200);
    EXPECT_EQ(object.oPosY, 307);
    object.bowserArenaOwner = NULL;
    object.oPosX = 3200;
    bhv_bobomb_loop();
    EXPECT_EQ(bobombSteps, 3);
    begin();
    object.behavior = bhvSpiny;
    object.parentObj = &bowser;
    bowser.behavior = bhvEnemyLakitu;
    bowser.oEnemyLakituNumSpinies = 1;
    object.oDistanceToMario = 6000;
    object.bowserArenaOwner = &bowser;
    EXPECT_EQ(spiny_check_active(), TRUE);
    EXPECT_EQ(object.activeFlags, ACTIVE_FLAG_ACTIVE);
    object.oHomeX = 1200;
    object.oPosX = 3200;
    object.oPosY = 107;
    bowser_arena_recover_enemy(&object);
    EXPECT_EQ(object.oPosX, 1200);
    object.bowserArenaOwner = NULL;
    EXPECT_EQ(spiny_check_active(), FALSE);
    EXPECT_EQ(object.activeFlags, ACTIVE_FLAG_DEACTIVATED);
    EXPECT_EQ(bowser.oEnemyLakituNumSpinies, 0);
    arenaFloorTest = 0;
    return TRUE;
}

static int arena_recurring_boo(void) {
    begin();
    object.oBehParams2ndByte = BOWSER_BP_BITS;
    object.bowserArenaStage = 2;
    for (int frame = 0; frame < 1800; frame++) {
        bowser_arena_update(&object);
    }
    EXPECT_EQ(spawnedCount, 0);
    object.oMoveAnglePitch = object.oFaceAnglePitch = 0x8000;
    object.oFaceAngleRoll = 0x4000;
    object.oHealth = 2;
    bowser_arena_landed(&object);
    EXPECT_EQ(children[5].behavior == bhvBoo, TRUE);
    EXPECT_EQ(children[5].oPosX, 0);
    EXPECT_EQ(children[5].oPosY, 507);
    EXPECT_EQ(children[5].oBehParams >> 24, COIN_INSIDE_BOO_BP_YELLOW_COIN);
    EXPECT_EQ(children[5].oMoveAnglePitch, 0);
    EXPECT_EQ(children[5].oFaceAnglePitch, 0);
    EXPECT_EQ(children[5].oFaceAngleRoll, 0);
    int first = spawnedCount;
    for (int frame = 0; frame < 3600; frame++) {
        bowser_arena_update(&object);
    }
    EXPECT_EQ(spawnedCount, first);
    EXPECT_EQ(object.bowserArenaBooTimer, 0);
    children[5].activeFlags = ACTIVE_FLAG_DEACTIVATED;
    for (int frame = 0; frame < 1799; frame++) {
        bowser_arena_update(&object);
    }
    EXPECT_EQ(spawnedCount, first);
    bowser_arena_update(&object);
    EXPECT_EQ(spawnedCount, first + 1);
    EXPECT_EQ(children[first].behavior == bhvBoo, TRUE);
    EXPECT_EQ(children[first].oBehParams >> 24, COIN_INSIDE_BOO_BP_YELLOW_COIN);
    EXPECT_EQ(children[first].oFaceAnglePitch, 0);
    EXPECT_EQ(children[first].oFaceAngleRoll, 0);
    for (int frame = 0; frame < 2000; frame++) {
        bowser_arena_update(&object);
    }
    EXPECT_EQ(spawnedCount, first + 1);
    EXPECT_EQ(object.bowserArenaBooTimer, 0);
    children[first].activeFlags = ACTIVE_FLAG_DEACTIVATED;
    for (int frame = 0; frame < 100; frame++) {
        bowser_arena_update(&object);
    }
    object.oHealth = 1;
    bowser_arena_landed(&object);
    EXPECT_EQ(object.bowserArenaBooTimer, 100);
    first = spawnedCount;
    for (int frame = 0; frame < 799; frame++) {
        bowser_arena_update(&object);
    }
    EXPECT_EQ(spawnedCount, first);
    bowser_arena_update(&object);
    EXPECT_EQ(spawnedCount, first + 1);
    EXPECT_EQ(children[first].behavior == bhvBoo, TRUE);
    object.oHealth = 0;
    bowser_arena_landed(&object);
    for (int child = 0; child < spawnedCount; child++) {
        if (children[child].bowserArenaOwner == &object) {
            EXPECT_EQ(children[child].activeFlags, ACTIVE_FLAG_DEACTIVATED);
        }
    }
    first = spawnedCount;
    for (int frame = 0; frame < 1800; frame++) {
        bowser_arena_update(&object);
    }
    EXPECT_EQ(spawnedCount, first);
    return TRUE;
}

static int arena_enemy_fade(void) {
    const BehaviorScript *behaviors[] = { bhvGoomba, bhvBobomb, bhvSmallBully,  bhvMrI,
                                          bhvBoo,    bhvSnufit, bhvCirclingAmp, bhvFireSpitter };
    for (unsigned i = 0; i < sizeof(behaviors) / sizeof(behaviors[0]); i++) {
        begin();
        struct Object *enemy = bowser_arena_spawn(&object, MODEL_NONE, behaviors[i], 10, 307, 20);
        EXPECT_EQ(enemy != NULL, TRUE);
        EXPECT_EQ(enemy->bowserArenaFade, BOWSER_ARENA_FADE_FRAMES);
        if (behaviors[i] == bhvBoo) {
            EXPECT_EQ(enemy->oOpacity, 255);
        }
        enemy->curBhvCommand = enemy->behavior;
        // Native sizes and native opacity must survive the spawn effect.
        enemy->header.gfx.scale[0] = 2.0f;
        enemy->header.gfx.scale[1] = 1.5f;
        enemy->header.gfx.scale[2] = 0.5f;
        enemy->oOpacity = 180;
        gCurrentObject = enemy;
        for (int frame = 1; frame <= BOWSER_ARENA_FADE_FRAMES; frame++) {
            EXPECT_EQ(bowser_arena_phase_in(enemy), TRUE);
            EXPECT_EQ(enemy->bowserArenaFade, BOWSER_ARENA_FADE_FRAMES - frame);
            EXPECT_EQ(enemy->oIntangibleTimer, frame == BOWSER_ARENA_FADE_FRAMES ? 0 : -1);
            EXPECT_EQ(enemy->oOpacity, 180);
            EXPECT_EQ(enemy->header.gfx.scale[0], 2.0f);
            EXPECT_EQ(enemy->header.gfx.scale[1], 1.5f);
            EXPECT_EQ(enemy->header.gfx.scale[2], 0.5f);
        }
        EXPECT_EQ(bowser_arena_phase_in(enemy), FALSE);
        EXPECT_EQ(behaviorUpdates, 1);
    }
    return TRUE;
}

static int quick_jump_offstage_return(void) {
    for (int fight = BOWSER_BP_BITDW; fight <= BOWSER_BP_BITS; fight++) {
        begin();
        object.oBehParams2ndByte = fight;
        object.oAction = BOWSER_ACT_QUICK_JUMP;
        for (int frame = 0; frame < 10 && object.oSubAction == 0; frame++) {
            object.oTimer = frame;
            bowser_act_quick_jump();
        }
        EXPECT_EQ(object.oSubAction, 1);
        object.oHomeY = 307;
        object.oPosY = -694;
        object.oVelY = -50;
        EXPECT_EQ(bowser_check_fallen_off_stage(), TRUE);
        cur_obj_change_action(BOWSER_ACT_JUMP_ONTO_STAGE);
        object.oSubAction = BOWSER_SUB_ACT_JUMP_ON_STAGE_IDLE;
        object.oHomeY = 307;
        object.oPosY = -693;
        object.oOpacity = 0;
        for (int frame = 0; frame < 32; frame++) {
            object.oTimer = frame;
            bowser_act_jump_onto_stage();
        }
        EXPECT_EQ(object.oSubAction, BOWSER_SUB_ACT_JUMP_ON_STAGE_START);
        // The shared jump-start animation has already passed the old launch frame.
        object.header.gfx.animInfo.animFrame = 17;
        for (int frame = 1; frame <= 12; frame++) {
            bowser_act_jump_onto_stage();
            EXPECT_EQ(object.oSubAction, frame == 12 ? BOWSER_SUB_ACT_JUMP_ON_STAGE_LAND
                                                     : BOWSER_SUB_ACT_JUMP_ON_STAGE_START);
        }
        EXPECT_EQ(object.oVelY, 150);
        EXPECT_EQ(object.oBowserTargetOpacity, 255);
        bowser_update_opacity();
        EXPECT_EQ(object.oOpacity > 0, TRUE);
        // A failed return arc still uses the existing center recovery.
        object.oVelY = -1;
        object.oPosX = 4000;
        bowser_act_jump_onto_stage();
        EXPECT_EQ(object.oPosX, 0);
        EXPECT_EQ(object.oPosY, object.oHomeY + 2000);
        object.oPosY = object.oHomeY;
        object.oMoveFlags = OBJ_MOVE_LANDED;
        bowser_act_jump_onto_stage();
        EXPECT_EQ(object.oSubAction, BOWSER_SUB_ACT_JUMP_ON_STAGE_STOP);
        object.header.gfx.animInfo.animFrame = suppliedAnimation.loopEnd - 1;
        bowser_act_jump_onto_stage();
        EXPECT_EQ(object.oAction, BOWSER_ACT_DEFAULT);
    }
    return TRUE;
}

static int defeated_offstage_recovery(void) {
    for (int fight = BOWSER_BP_BITDW; fight <= BOWSER_BP_BITS; fight++) {
        for (int waiting = 0; waiting < 2; waiting++) {
            begin();
            object.oBehParams2ndByte = fight;
            object.oAction = BOWSER_ACT_DEAD;
            object.oHealth = 0;
            object.bowserArenaStage = waiting ? 5 : 0;
            object.oHomeY = 307;
            object.oPosX = 4000;
            object.oPosY = -10000;
            object.oVelY = -78;
            object.oForwardVel = -200;
            object.oMoveFlags = OBJ_MOVE_ON_GROUND;
            object.oFloorType = SURFACE_DEATH_PLANE;
            object.oSubAction = waiting ? BOWSER_SUB_ACT_DEAD_WAIT : BOWSER_SUB_ACT_DEAD_BOUNCE;
            EXPECT_EQ(bowser_check_fallen_off_stage(), FALSE);
            bowser_recover_defeated();
            EXPECT_EQ(object.oAction, BOWSER_ACT_DEAD);
            EXPECT_EQ(object.oHealth, 0);
            EXPECT_EQ(object.oSubAction, BOWSER_SUB_ACT_DEAD_BOUNCE);
            EXPECT_EQ(object.oPosX, fight == BOWSER_BP_BITFS ? -800 : 0);
            EXPECT_EQ(object.oPosZ, 0);
            EXPECT_EQ(object.oPosY, object.oFloorHeight + 2000);
            EXPECT_EQ(object.oForwardVel, 0);
            EXPECT_EQ(object.oVelY, 0);
            EXPECT_EQ(object.oMoveFlags, OBJ_MOVE_IN_AIR);
            object.oPosY = object.oFloorHeight;
            object.oMoveFlags = OBJ_MOVE_LANDED;
            bowser_dead_bounce();
            if (fight == BOWSER_BP_BITS) {
                EXPECT_EQ(object.bowserArenaStage, 5);
                EXPECT_EQ(impactCalls, !waiting);
            }
            object.oMoveFlags = OBJ_MOVE_ON_GROUND;
            bowser_dead_bounce();
            EXPECT_EQ(object.oSubAction, BOWSER_SUB_ACT_DEAD_WAIT);
            object.oDistanceToMario = 400;
            object.oAngleToMario = 0x8000;
            object.header.gfx.animInfo.animFrame = suppliedAnimation.loopEnd - 1;
            EXPECT_EQ(bowser_dead_wait_for_mario(), TRUE);
            EXPECT_EQ(object.oAction, BOWSER_ACT_DEAD);
        }
    }
    // A lethal landing is recovered even when it is not far below home.
    begin();
    object.oAction = BOWSER_ACT_DEAD;
    object.oSubAction = BOWSER_SUB_ACT_DEAD_BOUNCE;
    object.oHomeY = object.oPosY;
    object.oFloorType = SURFACE_BURNING;
    object.oMoveFlags = OBJ_MOVE_LANDED;
    bowser_recover_defeated();
    EXPECT_EQ(object.oPosY, object.oFloorHeight + 2000);
    // Safe defeat landings and the reward/fade sequence retain their positions.
    for (int ending = 0; ending < 2; ending++) {
        begin();
        object.oAction = BOWSER_ACT_DEAD;
        object.oHomeY = object.oPosY;
        object.oSubAction = ending ? BOWSER_SUB_ACT_DEAD_FINAL_END : BOWSER_SUB_ACT_DEAD_BOUNCE;
        if (ending) {
            object.oPosY = -10000;
        }
        f32 originalY = object.oPosY;
        bowser_recover_defeated();
        EXPECT_EQ(object.oPosY, originalY);
    }
    begin();
    object.oAction = BOWSER_ACT_DEAD;
    object.oSubAction = BOWSER_SUB_ACT_DEAD_BOUNCE;
    object.oPosY = -10000;
    teleportUnsafe = 2;
    bowser_recover_defeated();
    EXPECT_EQ(object.oPosY, -10000);
    EXPECT_EQ(object.oAction, BOWSER_ACT_DEAD);
    teleportUnsafe = 0;
    return TRUE;
}

static int mine_recovery_wave(void) {
    for (int stage = 1; stage <= 4; stage++) {
        begin();
        object.oBehParams2ndByte = BOWSER_BP_BITS;
        object.bowserArenaStage = stage - 1;
        object.oHealth = 6 - stage;
        object.oAction = BOWSER_ACT_THROWN;
        object.oHomeY = 307;
        object.oPosY = 0;
        object.oVelY = -1;
        suppliedHitMine = TRUE;
        bowser_act_thrown();
        EXPECT_EQ(object.oHealth, 5 - stage);
        EXPECT_EQ(bowser_check_fallen_off_stage(), TRUE);
        cur_obj_change_action(BOWSER_ACT_JUMP_ONTO_STAGE);
        object.oSubAction = BOWSER_SUB_ACT_JUMP_ON_STAGE_LAND;
        object.oPosY = object.oHomeY;
        object.oMoveFlags = OBJ_MOVE_LANDED;
        bowser_act_jump_onto_stage();
        EXPECT_EQ(object.bowserArenaStage, stage);
        const BehaviorScript *enemy[] = { bhvGoomba, bhvBobomb, bhvMrI, bhvSmallBully };
        int found = 0;
        for (int i = 0; i < spawnedCount; i++) {
            found += children[i].behavior == enemy[stage - 1];
        }
        EXPECT_EQ(found, 5);
        int firstLandingSpawns = spawnedCount;
        bowser_arena_landed(&object);
        EXPECT_EQ(spawnedCount, firstLandingSpawns);
    }
    return TRUE;
}

static int interrupted_jump_drag(void) {
    for (int fight = BOWSER_BP_BITDW; fight <= BOWSER_BP_BITS; fight++) {
        for (int dropped = 0; dropped < 2; dropped++) {
            begin();
            object.oBehParams2ndByte = fight;
            object.oHealth = 3;
            object.oPosY = object.oHomeY = 307;
            bowser_start_center_jump();
            EXPECT_EQ(object.oDragStrength, 0);
            object.prevObj = &bowser;
            object.oHeldState = HELD_HELD;
            object.oBowserGrabbedStatus = BOWSER_GRAB_STATUS_NONE;
            heldMario.heldObj = &object;
            bowser_held_update();
            EXPECT_EQ(object.oDragStrength, 10);
            // Release also establishes normal physics if the pickup update was skipped.
            object.oDragStrength = 0;
            object.oHeldState = dropped ? HELD_DROPPED : HELD_THROWN;
            object.oBowserHeldAnglePitch = 0;
            object.oBowserHeldAngleVelYaw = 3000;
            bowser_thrown_dropped_update();
            EXPECT_EQ(object.oDragStrength, 10);
            EXPECT_EQ(object.oForwardVel, 70);
        }
    }
    return TRUE;
}

static int boo_reward_cleanup(void) {
    begin();
    object.oBehParams2ndByte = BOWSER_BP_BITS;
    struct Object *boo = spawn_object(&object, MODEL_BOO, bhvBoo);
    boo->bowserArenaOwner = &object;
    boo->oBehParams = (u32) COIN_INSIDE_BOO_BP_YELLOW_COIN << 24;
    struct Object *coin = spawn_object(boo, MODEL_YELLOW_COIN, bhvMovingYellowCoin);
    gCurrentObject = coin;
    coin_inside_boo_act_carried();
    EXPECT_EQ(coin->bowserArenaOwner == &object, TRUE);
    boo->oBooDeathStatus = BOO_DEATH_STATUS_DYING;
    coin_inside_boo_act_carried();
    EXPECT_EQ(coin->oAction, COIN_INSIDE_BOO_ACT_DROPPED);
    EXPECT_EQ(coin->bowserArenaOwner == NULL, TRUE);
    gCurrentObject = &object;
    struct Object *liveBoo = spawn_object(&object, MODEL_BOO, bhvBoo);
    liveBoo->bowserArenaOwner = &object;
    struct Object *carriedCoin = spawn_object(liveBoo, MODEL_YELLOW_COIN, bhvMovingYellowCoin);
    object.oHealth = 0;
    bowser_arena_landed(&object);
    EXPECT_EQ(coin->activeFlags, ACTIVE_FLAG_ACTIVE);
    EXPECT_EQ(liveBoo->activeFlags, ACTIVE_FLAG_DEACTIVATED);
    EXPECT_EQ(carriedCoin->activeFlags, ACTIVE_FLAG_DEACTIVATED);
    return TRUE;
}

static int missed_throw_stat(void) {
    for (int hit = 0; hit < 2; hit++) {
        begin();
        object.prevObj = &bowser;
        object.oHeldState = HELD_THROWN;
        // A weak throw still counts as a miss when it settles.
        object.oBowserHeldAngleVelYaw = hit ? 2000 : 100;
        bowser_thrown_dropped_update();
        object.oMoveFlags = OBJ_MOVE_IN_AIR;
        bowser_act_thrown();
        EXPECT_EQ(statEvents[PS_MISSED_BOWSER_THROWS], 0);
        suppliedHitMine = hit;
        object.oMoveFlags = OBJ_MOVE_ON_GROUND;
        bowser_act_thrown();
        EXPECT_EQ(statEvents[PS_MISSED_BOWSER_THROWS], !hit);
        EXPECT_EQ(statEvents[PS_BOWSER_BOMB_HITS], hit);
        suppliedHitMine = FALSE;
        bowser_act_thrown();
        EXPECT_EQ(statEvents[PS_MISSED_BOWSER_THROWS], !hit);
    }
    begin();
    object.prevObj = &bowser;
    object.oHeldState = HELD_THROWN;
    bowser_thrown_dropped_update();
    // Falling off the arena resolves once, before returning to the stage.
    bowser_finish_throw(FALSE);
    bowser_finish_throw(FALSE);
    EXPECT_EQ(statEvents[PS_MISSED_BOWSER_THROWS], 1);
    for (int escape = 0; escape < 2; escape++) {
        begin();
        object.prevObj = &bowser;
        object.oHeldState = HELD_DROPPED;
        object.oBowserGrabbedStatus = escape ? BOWSER_GRAB_STATUS_BREAKING_FREE : 0;
        bowser_thrown_dropped_update();
        bowser_finish_throw(FALSE);
        EXPECT_EQ(statEvents[PS_MISSED_BOWSER_THROWS], 0);
    }
    return TRUE;
}

static int final_arena_cleanup_releases_held_enemy(void) {
    for (s32 airborne = FALSE; airborne <= TRUE; airborne++) {
        begin();
        object.oBehParams2ndByte = BOWSER_BP_BITS;
        object.oHealth = 0;
        object.bowserArenaStage = 4;
        struct Object *enemy = bowser_arena_spawn(&object, MODEL_BLACK_BOBOMB, bhvBobomb, 0, 307, 0);
        enemy->oHeldState = HELD_HELD;
        heldMario = (struct MarioState){ .heldObj = enemy,
                                         .action = airborne ? ACT_HOLD_FREEFALL : ACT_HOLD_IDLE,
                                         .pos = { 10, 500, 20 },
                                         .vel = { 4, -12, 7 },
                                         .forwardVel = 25 };
        bowser_arena_landed(&object);
        EXPECT_EQ(heldMario.heldObj == NULL, TRUE);
        EXPECT_EQ(heldMario.action, airborne ? ACT_FREEFALL : ACT_IDLE);
        EXPECT_EQ(enemy->oHeldState, HELD_DROPPED);
        EXPECT_EQ(enemy->activeFlags, ACTIVE_FLAG_DEACTIVATED);
        EXPECT_EQ(heldMario.pos[0], 10);
        EXPECT_EQ(heldMario.pos[1], 500);
        EXPECT_EQ(heldMario.vel[0], 4);
        EXPECT_EQ(heldMario.vel[1], -12);
        EXPECT_EQ(heldMario.vel[2], 7);
        EXPECT_EQ(heldMario.forwardVel, 25);
    }
    begin();
    object.oBehParams2ndByte = BOWSER_BP_BITS;
    object.oHealth = 0;
    object.bowserArenaStage = 4;
    struct Object *enemy = bowser_arena_spawn(&object, MODEL_BLACK_BOBOMB, bhvBobomb, 0, 307, 0);
    enemy->oHeldState = HELD_HELD;
    heldMario = (struct MarioState){ .heldObj = enemy,
                                     .usedObj = &marioObject,
                                     .action = ACT_READING_NPC_DIALOG,
                                     .actionArg = 15,
                                     .actionState = 2,
                                     .pos = { 10, 500, 20 },
                                     .vel = { 4, -12, 7 },
                                     .forwardVel = 25 };
    bowser_arena_landed(&object);
    EXPECT_EQ(heldMario.heldObj == NULL, TRUE);
    EXPECT_EQ(heldMario.action, ACT_READING_NPC_DIALOG);
    EXPECT_EQ(heldMario.actionArg, 15);
    EXPECT_EQ(heldMario.actionState, 2);
    EXPECT_EQ(heldMario.usedObj == &marioObject, TRUE);
    EXPECT_EQ(enemy->oHeldState, HELD_DROPPED);
    EXPECT_EQ(enemy->activeFlags, ACTIVE_FLAG_DEACTIVATED);
    EXPECT_EQ(heldMario.pos[0], 10);
    EXPECT_EQ(heldMario.pos[1], 500);
    EXPECT_EQ(heldMario.vel[0], 4);
    EXPECT_EQ(heldMario.vel[1], -12);
    EXPECT_EQ(heldMario.vel[2], 7);
    EXPECT_EQ(heldMario.forwardVel, 25);
    for (s32 preservation = 0; preservation < 3; preservation++) {
        begin();
        object.oBehParams2ndByte = BOWSER_BP_BITS;
        object.oHealth = preservation == 1 ? 1 : 0;
        object.bowserArenaStage = preservation == 1 ? 3 : 4;
        struct Object *enemy = bowser_arena_spawn(&object, MODEL_BLACK_BOBOMB, bhvBobomb, 0, 307, 0);
        if (preservation == 0) {
            enemy->bowserArenaOwner = &bowser; // A different encounter's actor is unrelated.
        }
        if (preservation == 2) {
            sTestGameOptions[GO_BOWSER_FIGHTS] = 1;
        }
        enemy->oHeldState = HELD_HELD;
        heldMario = (struct MarioState){ .heldObj = enemy, .action = ACT_HOLD_IDLE };
        bowser_arena_landed(&object);
        EXPECT_EQ(heldMario.heldObj == enemy, TRUE);
        EXPECT_EQ(heldMario.action, ACT_HOLD_IDLE);
        EXPECT_EQ(enemy->oHeldState, HELD_HELD);
        EXPECT_EQ(enemy->activeFlags != ACTIVE_FLAG_DEACTIVATED, TRUE);
    }
    heldMario = (struct MarioState){ 0 };
    sTestGameOptions[GO_BOWSER_FIGHTS] = 0;
    return TRUE;
}

static int arena_encounter(void) {
    begin();
    object.oBehParams2ndByte = BOWSER_BP_BITS;
    const int counts[] = { 5, 10, 17, 33 };
    const BehaviorScript *waves[] = { bhvGoomba, bhvBobomb, bhvMrI, bhvSmallBully };
    for (int stage = 1; stage <= 4; stage++) {
        object.oHealth = 5 - stage;
        int first = spawnedCount;
        bowser_arena_landed(&object);
        EXPECT_EQ(spawnedCount, counts[stage - 1]);
        for (int point = 0; point < 5; point++) {
            const s16 *pos = stage == 3 ? sArenaInnerPoints[point] : sArenaPoints[point];
            f32 inset = stage == 4 ? 0.75f : 1.0f;
            struct Object *enemy = &children[first + point];
            EXPECT_EQ(enemy->behavior == waves[stage - 1], TRUE);
            EXPECT_EQ(enemy->oPosX, pos[0] * inset);
            EXPECT_EQ(enemy->oPosY, 307);
            EXPECT_EQ(enemy->oPosZ, pos[2] * inset);
            EXPECT_EQ(enemy->bowserArenaOwner == &object, TRUE);
        }
        if (stage == 4) {
            for (int point = 0; point < 5; point++) {
                struct Object *snufit = &children[first + 5 + point * 2];
                const s16 *a = sArenaInnerPoints[point];
                const s16 *b = sArenaInnerPoints[(point + 1) % 5];
                EXPECT_EQ(snufit->behavior == bhvSnufit, TRUE);
                EXPECT_EQ(snufit->oPosX, (a[0] + b[0]) * 0.5f);
                EXPECT_EQ(snufit->oPosZ, (a[2] + b[2]) * 0.5f);
                EXPECT_EQ(snufit->oPosY, 757);
            }
            EXPECT_EQ(children[first + 15].behavior == bhvCirclingAmp, TRUE);
            EXPECT_EQ(children[first + 15].oHomeX, 0);
            EXPECT_EQ(children[first + 15].oHomeZ, 0);
            EXPECT_EQ(children[first + 15].oBehParams2ndByte, AMP_BP_ROT_RADIUS_400);
        }
        bowser_arena_landed(&object);
        EXPECT_EQ(spawnedCount, counts[stage - 1]);
    }
    for (int child = 0; child < spawnedCount; child++) {
        EXPECT_EQ(children[child].behavior == bhvEnemyLakitu, FALSE);
        EXPECT_EQ(children[child].behavior == bhvHomingAmp, FALSE);
    }
    return TRUE;
}

static int arena_coin_policy(void) {
    begin();
    object.behavior = bhvGoomba;
    EXPECT_EQ(bowser_arena_try_drop_coin(&object), FALSE);
    object.bowserArenaOwner = &bowser;
    suppliedRandom = 0.1999f;
    EXPECT_EQ(bowser_arena_try_drop_coin(&object), TRUE);
    EXPECT_EQ(spawnedCount, 1);
    EXPECT_EQ(children[0].bowserArenaOwner == NULL, TRUE);
    bowser_arena_try_drop_coin(&object);
    EXPECT_EQ(spawnedCount, 1);
    object.bowserArenaCoinRolled = 0;
    suppliedRandom = 0.2f;
    bowser_arena_try_drop_coin(&object);
    EXPECT_EQ(spawnedCount, 1);
    object.bowserArenaCoinRolled = 0;
    object.behavior = bhvSpiny;
    suppliedRandom = 0;
    bowser_arena_try_drop_coin(&object);
    EXPECT_EQ(spawnedCount, 1);
    object.behavior = bhvMrI;
    object.bowserArenaCoinRolled = 0;
    suppliedRandom = 0.99f;
    bowser_arena_try_drop_coin(&object);
    EXPECT_EQ(spawnedCount, 2);
    EXPECT_EQ(children[1].behavior == bhvMovingYellowCoin, TRUE);
    EXPECT_EQ(children[1].bowserArenaOwner == NULL, TRUE);
    bowser_arena_try_drop_coin(&object);
    EXPECT_EQ(spawnedCount, 2);
    object.bowserArenaOwner = NULL;
    EXPECT_EQ(bowser_arena_try_drop_coin(&object), FALSE);
    return TRUE;
}
static int missed_throw_landing(void) {
    for (int fight = 0; fight < 3; fight++) {
        for (int airtime = 30; airtime <= 31; airtime++) {
            begin();
            object.oBehParams2ndByte = fight;
            object.oAction = BOWSER_ACT_THROWN;
            object.oTimer = 5;
            object.oMoveFlags = OBJ_MOVE_IN_AIR;
            for (int frame = 0; frame < airtime; frame++) {
                bowser_act_thrown();
                EXPECT_EQ(spawnedCount, 0);
            }
            object.oMoveFlags = OBJ_MOVE_LANDED;
            bowser_act_thrown();
            int expected = fight == BOWSER_BP_BITS && airtime > 30;
            EXPECT_EQ(spawnedCount, expected);
            EXPECT_EQ(impactCalls, expected);
            if (expected) {
                EXPECT_EQ(children[0].oBehParams2ndByte, BOWSER_SHOCKWAVE_LIGHT);
            }
            bowser_act_thrown();
            EXPECT_EQ(spawnedCount, expected);
            EXPECT_EQ(impactCalls, expected);
        }
    }
    return TRUE;
}
static int bomb_landing_light_wave(void) {
    const s32 actions[] = { BOWSER_ACT_HIT_MINE, BOWSER_ACT_DEAD, BOWSER_ACT_THROWN,
                            BOWSER_ACT_QUICK_JUMP };
    for (int fight = 0; fight < 3; fight++) {
        for (unsigned action = 0; action < sizeof(actions) / sizeof(*actions); action++) {
            begin();
            object.oBehParams2ndByte = fight;
            object.oAction = actions[action];
            object.oFloorHeight = 1200;
            int bounces = 0;
            bowser_bounce_effects(&bounces);
            EXPECT_EQ(spawnedCount, 0);
            object.oMoveFlags = OBJ_MOVE_LANDED;
            bowser_bounce_effects(&bounces);
            int expected = 0;
            if (action < 2) {
                expected = fight == BOWSER_BP_BITS ? 2 : fight == BOWSER_BP_BITFS;
            }
            EXPECT_EQ(spawnedCount, expected);
            EXPECT_EQ(impactCalls, fight == BOWSER_BP_BITS && action < 2);
            for (int wave = 0; wave < expected; wave++) {
                EXPECT_EQ(children[wave].behavior == bhvBowserShockWave, TRUE);
                EXPECT_EQ(children[wave].oBehParams2ndByte,
                          wave == 0 ? BOWSER_SHOCKWAVE_LIGHT : BOWSER_SHOCKWAVE_DELAYED);
                EXPECT_EQ(children[wave].oPosY, 1200);
            }
            bowser_bounce_effects(&bounces);
            EXPECT_EQ(spawnedCount, expected);
            EXPECT_EQ(impactCalls, fight == BOWSER_BP_BITS && action < 2);
        }
    }
    return TRUE;
}
static int cap_arena_recovery(void) {
    struct Object arena = { .behavior = bhvTiltingBowserLavaPlatform };
    struct Surface unsafe = { .type = SURFACE_BURNING };
    for (int pillar = 0; pillar < 2; pillar++) {
        begin();
        gCurrLevelNum = LEVEL_BOWSER_2;
        unsafe.type = pillar ? SURFACE_DEFAULT : SURFACE_BURNING;
        sObjFloor = &unsafe;
        teleportFloor.object = &arena;
        teleportUnsafe = 0;
        object.oPosX = 3584;
        object.oForwardVel = 25;
        object.oVelY = 10;
        capPositionSaves = 0;
        EXPECT_EQ(normal_cap_return_to_bowser_arena(), TRUE);
        EXPECT_EQ(object.oPosX, 800);
        EXPECT_EQ(object.oPosZ, 0);
        EXPECT_EQ(object.oPosY, 100);
        EXPECT_EQ(object.oForwardVel, 0);
        EXPECT_EQ(object.oVelY, 0);
        EXPECT_EQ(capPositionSaves, 1);
        EXPECT_EQ(normal_cap_return_to_bowser_arena(), FALSE);
        EXPECT_EQ(capPositionSaves, 1);
    }
    // Falling normal caps recover through the production movement dispatcher.
    const s16 fallLevels[] = { LEVEL_BOWSER_1, LEVEL_BOWSER_2 };
    for (unsigned level = 0; level < sizeof(fallLevels) / sizeof(*fallLevels); level++) {
        for (int deathFloor = 0; deathFloor < 2; deathFloor++) {
            begin();
            gCurrLevelNum = fallLevels[level];
            teleportFloor.object = level ? &arena : NULL;
            teleportFloor.normal.y = 1;
            teleportUnsafe = 0;
            unsafe.type = SURFACE_DEATH_PLANE;
            sObjFloor = deathFloor ? &unsafe : NULL;
            object.oPosX = 4000;
            object.oPosY = -1;
            object.oForwardVel = 25;
            object.oVelX = 4;
            object.oVelY = -10;
            object.oVelZ = 7;
            object.oCapDoScaleVertically = TRUE;
            capCollisionFlags = OBJ_COL_FLAGS_NONE;
            capQuicksandChecks = capScaleCalls = capPositionSaves = 0;
            normal_cap_act_0();
            EXPECT_EQ(object.oPosX, 800);
            EXPECT_EQ(object.oPosY, 100);
            EXPECT_EQ(object.oPosZ, 0);
            EXPECT_EQ(object.oForwardVel, 0);
            EXPECT_EQ(object.oVelX, 0);
            EXPECT_EQ(object.oVelY, 0);
            EXPECT_EQ(object.oVelZ, 0);
            EXPECT_EQ(object.oFaceAnglePitch, 0);
            EXPECT_EQ(object.oFloor, &teleportFloor);
            EXPECT_EQ(capPositionSaves, 1);
            EXPECT_EQ(capSavedX, 800);
            EXPECT_EQ(capSavedY, 100);
            EXPECT_EQ(capSavedZ, 0);
            EXPECT_EQ(object.activeFlags, ACTIVE_FLAG_ACTIVE);
            EXPECT_EQ(capQuicksandChecks, 0);
            EXPECT_EQ(capScaleCalls, 0);
            EXPECT_EQ(normal_cap_return_to_bowser_arena(), FALSE);
        }
    }
    // Do not teleport airborne caps before they fall below the arena cutoff.
    for (unsigned level = 0; level < sizeof(fallLevels) / sizeof(*fallLevels); level++) {
        begin();
        gCurrLevelNum = fallLevels[level];
        sObjFloor = NULL;
        object.oPosX = 4000;
        object.oPosY = 0;
        capCollisionFlags = OBJ_COL_FLAGS_NONE;
        capPositionSaves = 0;
        normal_cap_act_0();
        EXPECT_EQ(object.oPosX, 4000);
        EXPECT_EQ(capPositionSaves, 0);
    }
    // Fight 1's outer static arena remains safe, without a central-bounds shortcut.
    begin();
    gCurrLevelNum = LEVEL_BOWSER_1;
    teleportFloor.object = NULL;
    teleportFloor.type = SURFACE_DEFAULT;
    teleportFloor.normal.y = 1;
    sObjFloor = &teleportFloor;
    object.oPosX = 2800;
    object.oForwardVel = 25;
    capCollisionFlags = OBJ_COL_FLAG_GROUNDED;
    capQuicksandChecks = capPositionSaves = 0;
    normal_cap_act_0();
    EXPECT_EQ(object.oPosX, 2800);
    EXPECT_EQ(object.oForwardVel, 25);
    EXPECT_EQ(capPositionSaves, 0);
    EXPECT_EQ(capQuicksandChecks, 1);
    // Missing, hazardous or wrong-owner target floors must never consume a recovery.
    for (unsigned level = 0; level < sizeof(fallLevels) / sizeof(*fallLevels); level++) {
        for (int invalid = 0; invalid < (level ? 3 : 4); invalid++) {
            begin();
            gCurrLevelNum = fallLevels[level];
            sObjFloor = NULL;
            object.oPosX = 4000;
            object.oPosY = -1;
            object.oForwardVel = 25;
            teleportFloor.object = level ? &arena : NULL;
            teleportFloor.normal.y = 1;
            teleportUnsafe = invalid < 2 ? invalid + 1 : 0;
            heldMario.pos[0] = 0;
            if (invalid == 2) {
                teleportFloor.object = level ? NULL : &arena;
            } else if (invalid == 3) {
                teleportFloor.normal.y = 0.5f;
            }
            capPositionSaves = 0;
            EXPECT_EQ(normal_cap_return_to_bowser_arena(), FALSE);
            EXPECT_EQ(object.oPosX, 4000);
            EXPECT_EQ(object.oForwardVel, 25);
            EXPECT_EQ(capPositionSaves, 0);
        }
    }
    // Caps outside Bowser arenas retain the existing ordinary movement path.
    begin();
    gCurrLevelNum = 0;
    sObjFloor = NULL;
    object.oPosX = 4000;
    object.oPosY = -1;
    object.oForwardVel = 25;
    capCollisionFlags = OBJ_COL_FLAGS_NONE;
    capPositionSaves = 0;
    normal_cap_act_0();
    EXPECT_EQ(object.oPosX, 4000);
    EXPECT_EQ(object.oForwardVel, 25);
    EXPECT_EQ(capPositionSaves, 0);
    sObjFloor = &unsafe;
    gCurrLevelNum = LEVEL_BOWSER_2;
    teleportUnsafe = 2;
    EXPECT_EQ(normal_cap_return_to_bowser_arena(), FALSE);
    teleportUnsafe = 0;
    teleportFloor.object = NULL;
    gCurrLevelNum = LEVEL_BOWSER_3;
    teleportFloor.normal.y = 1;
    begin();
    arena.behavior = bhvFallingBowserPlatform;
    arena.oAction = BOWSER_BITS_PLAT_ACT_CHECK;
    unsafe.type = SURFACE_DEFAULT;
    unsafe.object = &arena;
    sObjFloor = &unsafe;
    object.oPosX = 3584;
    EXPECT_EQ(normal_cap_return_to_bowser_arena(), FALSE);
    EXPECT_EQ(object.oPosX, 3584);
    arena.bowserArenaCollapse = TRUE;
    EXPECT_EQ(normal_cap_return_to_bowser_arena(), TRUE);
    object.oPosX = 3584;
    sObjFloor = &unsafe;
    arena.bowserArenaCollapse = FALSE;
    arena.oAction = BOWSER_BITS_PLAT_ACT_FALL;
    EXPECT_EQ(normal_cap_return_to_bowser_arena(), TRUE);
    unsafe.object = NULL;
    const s16 unsafeTypes[] = { SURFACE_BURNING, SURFACE_MARIO_ONLY_GRIPPY, SURFACE_DEFAULT };
    for (unsigned kind = 0; kind < 3; kind++) {
        begin();
        object.oPosX = 3584;
        unsafe.type = unsafeTypes[kind];
        sObjFloor = &unsafe;
        EXPECT_EQ(normal_cap_return_to_bowser_arena(), TRUE);
        EXPECT_EQ(object.oPosX, 800);
        EXPECT_EQ(object.oPosZ, 0);
        EXPECT_EQ(normal_cap_return_to_bowser_arena(), FALSE);
    }
    object.oPosY = -1;
    sObjFloor = NULL;
    EXPECT_EQ(normal_cap_return_to_bowser_arena(), TRUE);
    gCurrLevelNum = 0;
    return TRUE;
}
static int teleport_choices(void) {
    const f32 speeds[] = { 0, 19.9f, 20, 40, -20, -19.9f };
    for (unsigned speed = 0; speed < sizeof(speeds) / sizeof(*speeds); speed++) {
        int on = 0, front = 0, behind = 0;
        for (int sample = 0; sample < 1000; sample++) {
            begin();
            heldMario = (struct MarioState){ .pos = { 100, 100, 200 }, .forwardVel = speeds[speed] };
            suppliedRandom = (sample + 0.5f) / 1000.0f;
            bowser_teleport_to_mario();
            EXPECT_EQ(object.oPosX, 100);
            EXPECT_EQ(object.oPosY, 100);
            on += object.oPosZ == 200;
            front += object.oPosZ == 500;
            behind += object.oPosZ == -100;
        }
        int slow = ABS(speeds[speed]) < 20;
        EXPECT_EQ(on, slow ? 500 : 0);
        EXPECT_EQ(front, slow ? 250 : 500);
        EXPECT_EQ(behind, slow ? 250 : 500);
    }
    begin();
    heldMario = (struct MarioState){ .pos = { 100, 100, 200 },
                                     .forwardVel = 30,
                                     .faceAngle = { 0, 0x4000, 0 } };
    suppliedRandom = 0;
    bowser_teleport_to_mario();
    EXPECT_EQ(object.oPosX, 400);
    EXPECT_EQ(fabsf(object.oPosZ - 200) < 0.1f, TRUE);
    teleportUnsafe = 1;
    bowser_teleport_to_mario();
    EXPECT_EQ(object.oPosX, 100);
    EXPECT_EQ(object.oPosZ, 200);
    teleportUnsafe = 2;
    object.oPosX = 300;
    bowser_teleport_to_mario();
    EXPECT_EQ(object.oPosX, 300);
    teleportUnsafe = 0;
    heldMario.pos[1] = 2000;
    EXPECT_EQ(bowser_teleport_to_mario(), TRUE);
    EXPECT_EQ(object.oPosY, 100);
    EXPECT_EQ(object.oFloorHeight, 100);
    EXPECT_EQ(object.oVelY, 0);
    for (int fadeIn = 0; fadeIn < 2; fadeIn++) {
        object.oAction = BOWSER_ACT_TELEPORT;
        object.oOpacity = fadeIn ? 0 : 255;
        object.oBowserTargetOpacity = fadeIn ? 255 : 0;
        for (int update = 1; update <= 10; update++) {
            bowser_update_opacity();
            EXPECT_EQ(object.oOpacity == object.oBowserTargetOpacity, update == 10);
        }
    }
    object.oAction = BOWSER_ACT_TELEPORT;
    object.oSubAction = BOWSER_SUB_ACT_TELEPORT_START;
    object.oOpacity = 255;
    object.oVelY = 2.8f; // Native gravity/bounciness leaves this positive while grounded.
    bowser_act_teleport();
    EXPECT_EQ(object.oAction, BOWSER_ACT_TELEPORT);
    EXPECT_EQ(object.oBowserTargetOpacity, 0);
    object.oOpacity = 0;
    bowser_act_teleport();
    EXPECT_EQ(object.oSubAction, BOWSER_SUB_ACT_TELEPORT_MOVE);
    for (int update = 1; update <= 15; update++) {
        object.oVelY = 2.8f;
        bowser_act_teleport();
        EXPECT_EQ(object.oForwardVel, 0);
        EXPECT_EQ(object.oSubAction,
                  update < 15 ? BOWSER_SUB_ACT_TELEPORT_MOVE : BOWSER_SUB_ACT_TELEPORT_STOP);
    }
    object.oOpacity = 255;
    bowser_act_teleport();
    EXPECT_EQ(object.oAction, BOWSER_ACT_DEFAULT);
    EXPECT_EQ(object.oIntangibleTimer, 0);
    object.oAction = BOWSER_ACT_TELEPORT;
    object.oSubAction = BOWSER_SUB_ACT_TELEPORT_START;
    object.oPosY = 500;
    bowser_act_teleport();
    EXPECT_EQ(object.oAction, BOWSER_ACT_DEFAULT);
    EXPECT_EQ(object.oPosY, 500);
    EXPECT_EQ(object.oBowserTargetOpacity, 255);
    object.oPosY = object.oFloorHeight;
    object.oVelY = 50;
    object.oMoveFlags = OBJ_MOVE_ON_GROUND;
    object.oAction = BOWSER_ACT_TELEPORT;
    bowser_act_teleport();
    EXPECT_EQ(object.oAction, BOWSER_ACT_DEFAULT);
    object.oVelY = 0;
    object.oMoveFlags = OBJ_MOVE_IN_AIR;
    object.oAction = BOWSER_ACT_TELEPORT;
    bowser_act_teleport();
    EXPECT_EQ(object.oAction, BOWSER_ACT_DEFAULT);
    return TRUE;
}
static int fight_one_decisions(void) {
    const int expected[][5] = { { 0, 0, 0, 700, 300 }, { 0, 0, 300, 0, 700 }, { 50, 550, 400, 0, 0 } };
    const s32 actions[] = { BOWSER_ACT_DANCE, BOWSER_ACT_WALK_TO_MARIO, BOWSER_ACT_TELEPORT,
                            BOWSER_ACT_BREATH_FIRE, BOWSER_ACT_QUICK_JUMP };
    for (int scenario = 0; scenario < 3; scenario++) {
        int counts[5] = { 0 };
        for (int sample = 0; sample < 1000; sample++) {
            begin();
            suppliedRandom = (sample + 0.5f) / 1000.0f;
            object.oBowserStatus = BOWSER_STATUS_ANGLE_MARIO;
            object.oBowserIsReacting = scenario == 2;
            object.oDistanceToMario = scenario == 0 ? 1499 : 1500;
            bowser_bitdw_actions();
            EXPECT_EQ(object.oBowserIsReacting, scenario != 2);
            for (int choice = 0; choice < 5; choice++) {
                counts[choice] += object.oAction == actions[choice];
            }
        }
        for (int choice = 0; choice < 5; choice++) {
            EXPECT_EQ(counts[choice], expected[scenario][choice]);
        }
    }
    begin();
    bowser_bitdw_actions();
    EXPECT_EQ(object.oAction, BOWSER_ACT_WALK_TO_MARIO);
    for (int sample = 0; sample < 1000; sample++) {
        object.oBowserIsReacting = TRUE;
        gCurrDemoInput = &object;
        suppliedRandom = (sample + 0.5f) / 1000.0f;
        bowser_bitdw_actions();
        EXPECT_EQ(object.oAction, BOWSER_ACT_WALK_TO_MARIO);
    }
    gCurrDemoInput = NULL;
    return TRUE;
}
static int fight_two_decisions(void) {
    const s32 actions[] = { BOWSER_ACT_TELEPORT,     BOWSER_ACT_BREATH_FIRE,
                            BOWSER_ACT_QUICK_JUMP,   BOWSER_ACT_BIG_JUMP,
                            BOWSER_ACT_CHARGE_MARIO, BOWSER_ACT_WALK_TO_MARIO,
                            BOWSER_ACT_DANCE,        BOWSER_ACT_SPIT_FIRE_ONTO_FLOOR };
    const int expected[][8] = {
        { 400, 200, 300, 0, 0, 0, 0, 100 }, { 100, 0, 100, 400, 200, 0, 0, 200 },
        { 550, 0, 0, 0, 0, 400, 50, 0 },    { 0, 0, 0, 150, 600, 0, 0, 250 },
        { 0, 0, 0, 150, 600, 0, 0, 250 },   { 0, 0, 0, 0, 0, 1000, 0, 0 }
    };
    for (int scenario = 0; scenario < 6; scenario++) {
        int counts[8] = { 0 };
        for (int sample = 0; sample < 1000; sample++) {
            begin();
            suppliedRandom = (sample + 0.5f) / 1000.0f;
            object.oBowserStatus = scenario == 5 ? 0 : BOWSER_STATUS_ANGLE_MARIO;
            object.oBowserIsReacting = scenario == 2;
            object.oDistanceToMario = scenario == 0 ? 1299 : 1300;
            if (scenario == 3) {
                object.oBowserDistToCenter = 500;
            } else if (scenario == 4) {
                object.oBowserDistToCenter = 1500;
            } else {
                object.oBowserDistToCenter = 1000;
            }
            bowser_bitfs_actions();
            EXPECT_EQ(object.oBowserIsReacting, scenario != 2);
            for (int choice = 0; choice < 8; choice++) {
                counts[choice] += object.oAction == actions[choice];
            }
        }
        for (int choice = 0; choice < 8; choice++) {
            EXPECT_EQ(counts[choice], expected[scenario][choice]);
        }
    }
    return TRUE;
}

static int fight_three_sky_fire(void) {
    for (int level = LEVEL_BOWSER_1; level <= LEVEL_BOWSER_3; level++) {
        for (int blue = 0; blue < 2; blue++) {
            begin();
            gCurrLevelNum = level;
            object.oBehParams2ndByte = blue;
            object.oTimer = 21;
            bhv_blue_bowser_flame_loop();
            int expected;
            if (blue) {
                expected = level == LEVEL_BOWSER_3 ? 1 : 2;
            } else {
                expected = level == LEVEL_BOWSER_3 ? 6 : 3;
            }
            EXPECT_EQ(spawnedCount, expected);
            EXPECT_EQ(object.activeFlags, ACTIVE_FLAG_DEACTIVATED);
            for (int flame = 0; flame < expected; flame++) {
                EXPECT_EQ(children[flame].behavior == bhvFlameFloatingLanding, TRUE);
                EXPECT_EQ(children[flame].oBehParams2ndByte, blue ? flame + 1 : 0);
            }
        }
    }
    for (int fight = BOWSER_BP_BITFS; fight <= BOWSER_BP_BITS; fight++) {
        for (int cycle = 0; cycle < 3; cycle++) {
            begin();
            object.oBehParams2ndByte = fight;
            object.oAction = BOWSER_ACT_SPIT_FIRE_INTO_SKY;
            suppliedAnimation.loopEnd = 60;
            f32 nativeFrame = -1;
            int total = fight == BOWSER_BP_BITS ? 79 : 137;
            int first = fight == BOWSER_BP_BITS ? 15 : 27;
            int interval = fight == BOWSER_BP_BITS ? 6 : 8;
            int count = fight == BOWSER_BP_BITS ? 10 : 11;
            for (int frame = 1; frame <= total; frame++) {
                object.oTimer = frame - 1;
                if (fight == BOWSER_BP_BITFS) {
                    object.header.gfx.animInfo.animFrame = (s32) nativeFrame;
                }
                bowser_act_spit_fire_into_sky();
                nativeFrame += animationSpeed;
                int expected = frame < first ? 0 : MIN(count, (frame - first) / interval + 1);
                EXPECT_EQ(spawnedCount, expected);
                EXPECT_EQ(object.oAction,
                          frame < total ? BOWSER_ACT_SPIT_FIRE_INTO_SKY : BOWSER_ACT_DEFAULT);
            }
            for (int shot = 0; shot < count; shot++) {
                EXPECT_EQ(children[shot].oBehParams2ndByte, shot == count - 1);
                EXPECT_EQ(children[shot].behavior == bhvBlueBowserFlame, TRUE);
            }
            EXPECT_EQ((object.oBowserStatus & BOWSER_STATUS_FIRE_SKY) != 0, TRUE);
        }
    }
    return TRUE;
}

static int fight_three_move_properties(void) {
    for (int fight = BOWSER_BP_BITFS; fight <= BOWSER_BP_BITS; fight++) {
        int startup = fight == BOWSER_BP_BITS ? 15 : 28;
        int active = fight == BOWSER_BP_BITS ? 30 : 39;
        int total = fight == BOWSER_BP_BITS ? 48 : 72;
        begin();
        object.oBehParams2ndByte = fight;
        object.oAction = BOWSER_ACT_BREATH_FIRE;
        struct Object emitter = { .parentObj = &object };
        for (int frame = 1; frame <= total; frame++) {
            object.oTimer = frame - 1;
            bowser_act_breath_fire();
            gCurrentObject = &emitter;
            bhv_bowser_flame_spawn_loop();
            bhv_bowser_flame_spawn_loop(); // Redispatch cannot duplicate a shot.
            gCurrentObject = &object;
            int elapsed = MAX(0, MIN(active, frame - startup));
            int expected = fight == BOWSER_BP_BITS ? elapsed : (elapsed + 1) / 2;
            EXPECT_EQ(spawnedCount, expected);
            EXPECT_EQ(object.oAction, frame < total ? BOWSER_ACT_BREATH_FIRE : BOWSER_ACT_DEFAULT);
        }
        for (int flame = 0; flame < spawnedCount; flame++) {
            int blue = fight == BOWSER_BP_BITS && (flame + 1) % 4 == 0;
            EXPECT_EQ(children[flame].behavior == bhvBlueBowserFlame, blue);
            if (blue) {
                EXPECT_EQ(children[flame].oBehParams2ndByte, 1);
                EXPECT_EQ((children[flame].oBehParams & BOWSER_FLAME_NO_SPLIT) != 0, TRUE);
            }
        }
        begin();
        object.oBehParams2ndByte = fight;
        int fade = fight == BOWSER_BP_BITS ? 6 : 8;
        for (int fadeIn = 0; fadeIn < 2; fadeIn++) {
            object.oAction = BOWSER_ACT_TELEPORT;
            object.oOpacity = fadeIn ? 0 : 255;
            object.oBowserTargetOpacity = fadeIn ? 255 : 0;
            for (int frame = 1; frame <= fade; frame++) {
                bowser_update_opacity();
                EXPECT_EQ(object.oOpacity == object.oBowserTargetOpacity, frame == fade);
            }
        }
        object.oSubAction = BOWSER_SUB_ACT_TELEPORT_START;
        object.oOpacity = 0;
        object.oFloor = &teleportFloor;
        object.oMoveFlags = OBJ_MOVE_ON_GROUND;
        object.oPosY = object.oFloorHeight = 0;
        object.oVelY = 0;
        bowser_act_teleport();
        int hidden = fight == BOWSER_BP_BITS ? 13 : 14;
        for (int frame = 1; frame <= hidden; frame++) {
            bowser_act_teleport();
            EXPECT_EQ(object.oSubAction,
                      frame < hidden ? BOWSER_SUB_ACT_TELEPORT_MOVE : BOWSER_SUB_ACT_TELEPORT_STOP);
        }
        int jumpFrame = fight == BOWSER_BP_BITS ? 8 : 10;
        for (int frame = jumpFrame - 1; frame <= jumpFrame + 1; frame++) {
            object.header.gfx.animInfo.animFrame = frame;
            EXPECT_EQ(bowser_set_anim_jump(), frame == jumpFrame);
        }
        if (fight == BOWSER_BP_BITS) {
            begin();
            object.oPosY = object.oFloorHeight = 0;
            object.oVelY = 60;
            object.oGravity = -4;
            simulateMovement = TRUE;
            for (int frame = 1; frame <= 30; frame++) {
                cur_obj_move_standard(-78);
                EXPECT_EQ((object.oMoveFlags & OBJ_MOVE_LANDED) != 0, frame == 30);
            }
        }
    }
    const f32 speeds[] = { 0, 10, 29.9f, 30, 40, 80, -30, -40, -29.9f };
    const f32 offsets[] = { 200, 200, 478.4f, 480, 640, 1280, 480, 640, 478.4f };
    for (unsigned speed = 0; speed < sizeof(speeds) / sizeof(*speeds); speed++) {
        int beneath = 0, ahead = 0, behind = 0;
        for (int sample = 0; sample < 1000; sample++) {
            begin();
            object.oBehParams2ndByte = BOWSER_BP_BITS;
            heldMario.forwardVel = speeds[speed];
            suppliedRandom = (sample + .5f) / 1000.0f;
            EXPECT_EQ(bowser_teleport_to_mario(), TRUE);
            beneath += object.oPosZ == 0;
            ahead += fabsf(object.oPosZ - offsets[speed]) < 0.01f;
            behind += fabsf(object.oPosZ + offsets[speed]) < 0.01f;
        }
        int slow = fabsf(speeds[speed]) < 30;
        EXPECT_EQ(beneath, slow ? 600 : 200);
        EXPECT_EQ(ahead, slow ? 300 : 500);
        EXPECT_EQ(behind, slow ? 100 : 300);
    }
    begin();
    object.oBehParams2ndByte = BOWSER_BP_BITS;
    object.oAction = BOWSER_ACT_BIG_JUMP;
    EXPECT_EQ(bowser_spawn_shockwave(), TRUE);
    EXPECT_EQ(spawnedCount, 3);
    for (int wave = 0; wave < 3; wave++) {
        EXPECT_EQ(children[wave].oBehParams2ndByte,
                  wave < 2 ? BOWSER_SHOCKWAVE_LIGHT : BOWSER_SHOCKWAVE_HEAVY);
        EXPECT_EQ(children[wave].oBowserTimer, wave * BOWSER_SHOCKWAVE_DELAY_FRAMES);
    }
    begin();
    object.oBehParams2ndByte = BOWSER_BP_BITS;
    object.oAction = BOWSER_ACT_BIG_JUMP;
    object.oBowserStatus = BOWSER_STATUS_BIG_JUMP;
    EXPECT_EQ(bowser_spawn_shockwave(), TRUE);
    EXPECT_EQ(spawnedCount, 3);
    for (int wave = 0; wave < 3; wave++) {
        EXPECT_EQ(children[wave].oBehParams2ndByte,
                  wave == 0 ? BOWSER_SHOCKWAVE_LIGHT : BOWSER_SHOCKWAVE_HEAVY);
        EXPECT_EQ(children[wave].oBowserTimer, wave * BOWSER_SHOCKWAVE_DELAY_FRAMES);
    }

    for (int status = 0; status < 2; status++) {
        begin();
        object.oBehParams2ndByte = BOWSER_BP_BITS;
        object.oAction = BOWSER_ACT_JUMP_ONTO_STAGE;
        object.oBowserStatus = status ? BOWSER_STATUS_BIG_JUMP : 0;
        object.oFloorHeight = 307;
        EXPECT_EQ(bowser_spawn_shockwave(), TRUE);
        EXPECT_EQ(spawnedCount, 2);
        for (int wave = 0; wave < 2; wave++) {
            EXPECT_EQ(children[wave].oBehParams2ndByte,
                      wave == 0 ? BOWSER_SHOCKWAVE_LIGHT : BOWSER_SHOCKWAVE_HEAVY);
            EXPECT_EQ(children[wave].oBowserTimer, wave * 20);
            EXPECT_EQ(children[wave].oPosY, 307);
        }
    }
    return TRUE;
}

static int fight_three_decisions(void) {
    const s32 actions[] = { BOWSER_ACT_TELEPORT,           BOWSER_ACT_BREATH_FIRE,
                            BOWSER_ACT_QUICK_JUMP,         BOWSER_ACT_SPIT_FIRE_ONTO_FLOOR,
                            BOWSER_ACT_SPIT_FIRE_INTO_SKY, BOWSER_ACT_CHARGE_MARIO,
                            BOWSER_ACT_BIG_JUMP,           BOWSER_ACT_WALK_TO_MARIO };
    const int expected[][8] = { { 600, 50, 200, 50, 100, 0, 0, 0 },
                                { 350, 0, 50, 0, 0, 400, 200, 0 },
                                { 0, 0, 0, 0, 0, 0, 0, 1000 } };
    for (int scenario = 0; scenario < 3; scenario++) {
        int counts[8] = { 0 };
        for (int sample = 0; sample < 1000; sample++) {
            begin();
            suppliedRandom = (sample + 0.5f) / 1000.0f;
            object.oBowserStatus = scenario == 2 ? 0 : BOWSER_STATUS_ANGLE_MARIO;
            object.oDistanceToMario = scenario == 0 ? 999 : 1000;
            bowser_bits_action_list();
            for (int choice = 0; choice < 8; choice++) {
                counts[choice] += object.oAction == actions[choice];
            }
        }
        for (int choice = 0; choice < 8; choice++) {
            EXPECT_EQ(counts[choice], expected[scenario][choice]);
        }
    }
    return TRUE;
}

static int escape_jump_center(void) {
    const f32 starts[][3] = { { 2000, 0, 0 }, { -3000, 100, 1000 }, { 0, 500, -2500 }, { 0, 0, 0 } };
    for (int fight = BOWSER_BP_BITDW; fight <= BOWSER_BP_BITS; fight++) {
        for (unsigned i = 0; i < sizeof(starts) / sizeof(*starts); i++) {
            begin();
            object.oBehParams2ndByte = fight;
            object.prevObj = &children[0];
            object.oPosX = starts[i][0];
            object.oPosY = starts[i][1];
            object.oPosZ = starts[i][2];
            object.oGravity = -4;
            object.oHeldState = HELD_DROPPED;
            object.oBowserGrabbedStatus = BOWSER_GRAB_STATUS_BREAKING_FREE;
            bowser_thrown_dropped_update();
            EXPECT_EQ(object.oPosX, starts[i][0]);
            EXPECT_EQ(object.oPosZ, starts[i][2]);
            EXPECT_EQ(object.oSubAction, 3);
            EXPECT_EQ(centerJumpRoars, 1);
            simulateMovement = TRUE;
            for (int frame = 0; frame < 80 && !(object.oMoveFlags & OBJ_MOVE_LANDED); frame++) {
                bowser_center_jump_update();
                cur_obj_move_standard(-78);
            }
            EXPECT_EQ((object.oMoveFlags & OBJ_MOVE_LANDED) != 0, TRUE);
            EXPECT_EQ(centerJumpRoars, 1);
            f32 targetX = fight == BOWSER_BP_BITFS ? -800.0f : 0.0f;
            EXPECT_EQ(sqrtf(sqr(object.oPosX - targetX) + sqr(object.oPosZ)) < 2.0f, TRUE);
            EXPECT_EQ(object.oBowserStatus & BOWSER_STATUS_VALID_THROW, 0);
        }
    }
    return TRUE;
}
static int hold_timeout(void) {
    for (int fight = BOWSER_BP_BITDW; fight <= BOWSER_BP_BITS; fight++) {
        int timeout = fight == BOWSER_BP_BITS ? 150 : 180;
        for (int spinning = 0; spinning < 2; spinning++) {
            begin();
            object.oBehParams2ndByte = fight;
            object.prevObj = &children[0];
            object.oHeldState = HELD_HELD;
            heldMario = (struct MarioState){ .action = ACT_HOLDING_BOWSER, .heldObj = &object };
            marioObject.oAngleVelYaw = spinning ? 0x1000 : 0;
            for (int frame = 0; frame < timeout; frame++) {
                bowser_held_update();
                EXPECT_EQ(heldMario.heldObj == &object, TRUE);
                EXPECT_EQ(object.oHeldState, HELD_HELD);
            }
            bowser_held_update();
            EXPECT_EQ(heldMario.heldObj == NULL, TRUE);
            EXPECT_EQ(heldMario.action, ACT_RELEASING_BOWSER);
            EXPECT_EQ(object.oHeldState, HELD_DROPPED);
            EXPECT_EQ(object.oBowserGrabbedStatus, BOWSER_GRAB_STATUS_BREAKING_FREE);
            bowser_thrown_dropped_update();
            EXPECT_EQ(object.oHeldState, HELD_FREE);
            EXPECT_EQ(object.oAction, BOWSER_ACT_BIG_JUMP);
            EXPECT_EQ(object.oSubAction, 3);
            EXPECT_EQ(escapeCapSpeed, 25);
            EXPECT_EQ(object.oPrevAction,
                      BOWSER_ACT_BIG_JUMP); // Script dispatch preserves airborne phase.
            EXPECT_EQ(object.oVelY, 80);
            EXPECT_EQ(object.oForwardVel > 0, fight == BOWSER_BP_BITFS);
            EXPECT_EQ(object.oBowserGrabCooldown, 300);
            object.oMoveFlags = OBJ_MOVE_LANDED;
            EXPECT_EQ(bowser_land(), TRUE);
            EXPECT_EQ(object.oVelY, 0);
            EXPECT_EQ(impactCalls, 1);
            // A new grab gets its own complete six-second allowance.
            object.oBowserGrabbedStatus = BOWSER_GRAB_STATUS_NONE;
            object.oHeldState = HELD_HELD;
            heldMario.heldObj = &object;
            bowser_held_update();
            EXPECT_EQ(object.oBowserTimer, 1);
            EXPECT_EQ(heldMario.heldObj == &object, TRUE);
        }
    }
    return TRUE;
}
static int punch_tail_grabs(void) {
    struct MarioState mario = { .action = ACT_PUNCHING,
                                .actionArg = ACT_ARG_PUNCH_SEQUENCE_YAH,
                                .pos = { 0, 0, -300 } };
    begin();
    EXPECT_EQ(able_to_grab_object(&mario, &bowser), TRUE);
    const u32 rejected[] = { ACT_WALKING, ACT_DIVE_SLIDE, ACT_IDLE };
    for (unsigned i = 0; i < sizeof(rejected) / sizeof(*rejected); i++) {
        mario.action = rejected[i];
        EXPECT_EQ(able_to_grab_object(&mario, &bowser), FALSE);
    }
    mario.action = ACT_PUNCHING;
    mario.pos[2] = 300;
    EXPECT_EQ(able_to_grab_object(&mario, &bowser), FALSE);
    mario.faceAngle[1] = (s16) 0x8000;
    EXPECT_EQ(able_to_grab_object(&mario, &bowser), FALSE);
    mario.pos[2] = -300;
    EXPECT_EQ(able_to_grab_object(&mario, &bowser), FALSE);
    mario.faceAngle[1] = 0;
    const s16 rearAngles[] = { -0x2AAB, -0x2AAA, 0x2AAA, 0x2AAB };
    for (unsigned i = 0; i < sizeof(rearAngles) / sizeof(*rearAngles); i++) {
        bowser.oMoveAngleYaw = rearAngles[i];
        EXPECT_EQ(able_to_grab_object(&mario, &bowser), i == 1 || i == 2);
    }
    bowser.oMoveAngleYaw = 0;
    const s16 facingAngles[] = { -0x638F, -0x638E, 0x638E, 0x638F };
    for (unsigned i = 0; i < sizeof(facingAngles) / sizeof(*facingAngles); i++) {
        mario.faceAngle[1] = facingAngles[i];
        EXPECT_EQ(able_to_grab_object(&mario, &bowser), i == 1 || i == 2);
    }
    mario.faceAngle[1] = 0;
    bowser.oBowserGrabCooldown = 1;
    EXPECT_EQ(able_to_grab_object(&mario, &bowser), FALSE);
    bowser.oBowserGrabCooldown = 0;
    bowser.oHeldState = HELD_HELD;
    EXPECT_EQ(able_to_grab_object(&mario, &bowser), FALSE);
    bowser.oHeldState = HELD_FREE;
    object.parentObj = &bowser;
    tailOverlap = TRUE;
    bowser.oAction = BOWSER_ACT_TELEPORT;
    const s32 opacities[] = { 255, 128, 0, 128, 255 };
    for (unsigned fade = 0; fade < sizeof(opacities) / sizeof(*opacities); fade++) {
        bowser.oOpacity = opacities[fade];
        EXPECT_EQ(able_to_grab_object(&mario, &bowser), FALSE);
        bowser_tail_anchor_act_default();
        EXPECT_EQ(bowser.oIntangibleTimer, -1);
    }
    bowser.oAction = BOWSER_ACT_DEFAULT;
    EXPECT_EQ(able_to_grab_object(&mario, &bowser), TRUE);
    bowser_tail_anchor_act_default();
    EXPECT_EQ(bowser.oIntangibleTimer, 0);
    return TRUE;
}
static int dive_tail_grabs(void) {
    for (int frame = 0; frame < 5; frame++) {
        begin();
        bowser.activeFlags = ACTIVE_FLAG_ACTIVE;
        bowser.hitboxRadius = 400;
        bowser.hitboxHeight = 400;
        bowser.oIntangibleTimer = -1;
        bowser.oOpacity = 128;
        bowser.oBowserTargetOpacity = 0;
        bowser.prevObj = &object;
        bowser.header.next = &gObjectLists[OBJ_LIST_GENACTOR];
        gObjectLists[OBJ_LIST_GENACTOR].header.next = &bowser;
        object.parentObj = &bowser;
        object.behavior = bhvBowserTailAnchor;
        object.oPosY = 0;
        object.oPosZ = -250;
        object.hitboxRadius = 150;
        object.hitboxHeight = 50;
        object.hitboxDownOffset = -50;
        marioObject.hitboxRadius = 37;
        marioObject.hitboxHeight = 160;
        struct MarioState mario = {
            .action = ACT_DIVE, .actionState = frame, .marioObj = &marioObject, .pos = { 0, 0, -300 }
        };
        EXPECT_EQ(mario_check_object_grab(&mario), frame < 3);
        EXPECT_EQ(mario.action, frame < 3 ? ACT_PICKING_UP_BOWSER : ACT_DIVE);
        if (frame < 3) {
            EXPECT_EQ(mario.usedObj == &bowser, TRUE);
            EXPECT_EQ(bowser.oOpacity, 255);
            EXPECT_EQ(bowser.oBowserTargetOpacity, 255);
        }
    }
    return TRUE;
}

static int held_visibility(void) {
    for (int fight = 0; fight < 3; fight++) {
        for (int opacity = 0; opacity <= 255; opacity += 85) {
            begin();
            object.oBehParams2ndByte = fight;
            object.oHeldState = HELD_HELD;
            object.oOpacity = opacity;
            object.oBowserTargetOpacity = 0;
            heldMario.heldObj = &object;
            for (int frame = 0; frame < 20; frame++) {
                bowser_held_update();
                bowser_update_opacity();
                EXPECT_EQ(object.oOpacity, 255);
                EXPECT_EQ(object.oBowserTargetOpacity, 255);
            }
        }
    }
    return TRUE;
}

static int straight_walk_speed(void) {
    const f32 speeds[] = { 3.0f, 6.0f, 9.0f };
    for (int fight = 0; fight < 3; fight++) {
        for (int turning = 0; turning < 2; turning++) {
            begin();
            object.oBehParams2ndByte = fight;
            object.oHealth = 3 + fight;
            object.oAction = BOWSER_ACT_WALK_TO_MARIO;
            object.oSubAction = 1;
            object.oBowserStatus = BOWSER_STATUS_ANGLE_MARIO;
            for (int frame = 0; frame < 20; frame++) {
                object.oMoveAngleYaw = 0;
                object.oAngleToMario = turning ? DEGREES(45) : 0;
                bowser_act_walk_to_mario();
                EXPECT_EQ(object.oForwardVel, turning ? 3.0f : speeds[fight]);
            }
            object.oSubAction = 0;
            object.oForwardVel = 0;
            object.header.gfx.animInfo.animFrame = 0;
            bowser_act_walk_to_mario();
            EXPECT_EQ(object.oForwardVel, 0);
            object.oSubAction = 2;
            object.header.gfx.animInfo.animFrame = 20;
            bowser_act_walk_to_mario();
            EXPECT_EQ(object.oForwardVel, 0);
        }
    }
    return TRUE;
}

static int tail_recovery(void) {
    for (int cycle = 0; cycle < 3; cycle++) {
        begin();
        object.parentObj = &bowser;
        tailOverlap = TRUE;
        bowser_tail_anchor_act_default();
        EXPECT_EQ(bowser.oIntangibleTimer, 0);
        tailOverlap = FALSE;
        bowser_tail_anchor_act_default();
        EXPECT_EQ(bowser.oIntangibleTimer, -1);
        tailOverlap = TRUE;
        bowser.oBowserGrabCooldown = 300;
        bowser_tail_anchor_act_default();
        EXPECT_EQ(bowser.oIntangibleTimer, -1);
        gCurrentObject = &bowser;
        bowser.oAction = BOWSER_ACT_THROWN;
        bowser.oMoveFlags = OBJ_MOVE_ON_GROUND;
        for (int frame = 0; frame < 100; frame++) {
            bowser_update_grab_cooldown();
        }
        EXPECT_EQ(bowser.oBowserGrabCooldown, 300);
        bowser.oAction = BOWSER_ACT_DEFAULT;
        bowser.oAngleToMario = (s16) 0x8000;
        for (int frame = 0; frame < 299; frame++) {
            bowser_update_grab_cooldown();
        }
        EXPECT_EQ(bowser.oBowserGrabCooldown, 1);
        for (int frame = 0; frame < 30; frame++) {
            bowser_update_grab_cooldown();
        }
        EXPECT_EQ(bowser.oBowserGrabCooldown, 1);
        bowser.oMoveAngleYaw = bowser.oAngleToMario;
        bowser_update_grab_cooldown();
        EXPECT_EQ(bowser.oBowserGrabCooldown, 0);
        gCurrentObject = &object;
        bowser_tail_anchor_thrown();
        EXPECT_EQ(object.oAction, BOWSER_ACT_TAIL_DEFAULT);
        bowser_tail_anchor_act_default();
        EXPECT_EQ(bowser.oIntangibleTimer, 0);
    }
    return TRUE;
}
static int throw_release(void) {
    for (int valid = 0; valid < 2; valid++) {
        begin();
        object.prevObj = &bowser;
        object.oHeldState = HELD_THROWN;
        object.oBowserStatus = BOWSER_STATUS_VALID_THROW;
        object.oBowserHeldAngleVelYaw = valid ? 1100 : 1000;
        bowser_thrown_dropped_update();
        EXPECT_EQ((object.oBowserStatus & BOWSER_STATUS_VALID_THROW) != 0, valid);
    }
    for (int dropped = 0; dropped < 2; dropped++) {
        for (int direction = -1; direction <= 1; direction += 2) {
            for (int pitch = -0x1000; pitch <= 0x1000; pitch += 0x1000) {
                begin();
                object.prevObj = &bowser;
                object.oHeldState = dropped ? HELD_DROPPED : HELD_THROWN;
                object.oBowserHeldAngleVelYaw = direction * 0x1000;
                object.oBowserHeldAnglePitch = pitch;
                object.oMoveAngleYaw = 0x2000;
                simulateMovement = TRUE;
                releaseMovementCalls = 0;
                f32 swingSpeed = (4096.0f / 3000.0f * 70.0f) * 2.5f;
                f32 forwardVel = coss(pitch) * swingSpeed;
                f32 velY = -sins(pitch) * swingSpeed;
                bowser_thrown_dropped_update();
                EXPECT_EQ(object.oHeldState, HELD_FREE);
                EXPECT_EQ(object.oAction, BOWSER_ACT_THROWN);
                EXPECT_EQ((object.oBowserStatus & BOWSER_STATUS_VALID_THROW) != 0, TRUE);
                EXPECT_EQ(object.oBowserGrabCooldown, BOWSER_GRAB_COOLDOWN_FRAMES);
                EXPECT_EQ(bowser.oAction, BOWSER_ACT_TAIL_THROWN);
                EXPECT_EQ(releaseMovementCalls, !dropped);
                if (!dropped) {
                    EXPECT_EQ(fabsf(releaseForwardVel - (forwardVel)) <= 0.001f, TRUE);
                    EXPECT_EQ(fabsf(releaseVelY - (velY)) <= 0.001f, TRUE);
                    // Release movement includes Bowser's normal drag of 10.
                    f32 vx = sins(0x2000) * forwardVel;
                    f32 vz = coss(0x2000) * forwardVel;
                    EXPECT_EQ(fabsf(object.oPosX - (vx - vx * vx * 0.001f)) <= 0.001f, TRUE);
                    EXPECT_EQ(fabsf(object.oPosZ - (vz - vz * vz * 0.001f)) <= 0.001f, TRUE);
                } else {
                    EXPECT_EQ(fabsf(object.oPosX - (0.0f)) <= 0.001f, TRUE);
                    EXPECT_EQ(fabsf(object.oPosZ - (0.0f)) <= 0.001f, TRUE);
                }
            }
        }
    }
    return TRUE;
}

static int moving_fire_fixed_floor(void) {
    struct Surface floor = { .normal.y = 1.0f, .type = SURFACE_DEFAULT };
    const f32 speeds[] = { -35.0f, 0.0f, 15.0f, 70.0f };
    const f32 drags[] = { 0.0f, 10.0f };
    for (u32 speed = 0; speed < sizeof(speeds) / sizeof(*speeds); speed++) {
        for (u32 drag = 0; drag < sizeof(drags) / sizeof(*drags); drag++) {
            begin();
            gCurrLevelNum = LEVEL_BOWSER_3;
            gCollisionFlags = 0;
            simulateMovement = TRUE;
            object.oRoom = -1;
            object.oFloor = &floor;
            object.oFloorHeight = 307.0f;
            object.oPosX = 400.0f;
            object.oPosZ = -200.0f;
            object.oMoveAngleYaw = 0x2000;
            object.oForwardVel = speeds[speed];
            object.oDragStrength = drags[drag];
            object.oVelY = -8.0f;
            object.oGravity = -1.0f;
            object.oMoveFlags = OBJ_MOVE_HIT_EDGE;
            struct Object original = object;
            object.oMoveFlags &= ~OBJ_MOVE_HIT_EDGE;
            cur_obj_move_standard(78);
            struct Object native = object;
            object = original;
            EXPECT_EQ(bowser_flame_move_on_fixed_arena_floor(), TRUE);
            EXPECT_EQ(object.oPosX, native.oPosX);
            EXPECT_EQ(object.oPosY, native.oPosY);
            EXPECT_EQ(object.oPosZ, native.oPosZ);
            EXPECT_EQ(object.oVelX, native.oVelX);
            EXPECT_EQ(object.oVelY, native.oVelY);
            EXPECT_EQ(object.oVelZ, native.oVelZ);
            EXPECT_EQ(object.oForwardVel, native.oForwardVel);
            EXPECT_EQ(object.oMoveFlags, native.oMoveFlags);
        }
    }
    object.oPosX = 999.0f;
    object.oPosZ = 0.0f;
    object.oForwardVel = 15.0f;
    object.oMoveAngleYaw = 0x4000;
    EXPECT_EQ(bowser_flame_move_on_fixed_arena_floor(), FALSE);
    EXPECT_EQ(object.oPosX, 999.0f);
    object.oPosX = 0.0f;
    object.oPosY = 306.99f;
    EXPECT_EQ(bowser_flame_has_fixed_arena_floor(), FALSE);
    object.oPosY = 307.0f;
    EXPECT_EQ(bowser_flame_has_fixed_arena_floor(), TRUE);
    object.oPosX = 1000.0f;
    EXPECT_EQ(bowser_flame_has_fixed_arena_floor(), FALSE);
    object.oPosX = 0.0f;
    floor.flags = SURFACE_FLAG_DYNAMIC;
    EXPECT_EQ(bowser_flame_has_fixed_arena_floor(), FALSE);
    floor.flags = 0;
    object.oMoveFlags = OBJ_MOVE_ENTERED_WATER;
    EXPECT_EQ(bowser_flame_has_fixed_arena_floor(), FALSE);
    object.oMoveFlags = 0;
    object.oWallHitboxRadius = 1.0f;
    EXPECT_EQ(bowser_flame_has_fixed_arena_floor(), FALSE);
    object.oWallHitboxRadius = 0;
    gCollisionFlags = 1;
    EXPECT_EQ(bowser_flame_has_fixed_arena_floor(), FALSE);
    gCollisionFlags = 0;
    object.activeFlags |= ACTIVE_FLAG_FAR_AWAY;
    EXPECT_EQ(bowser_flame_has_fixed_arena_floor(), FALSE);
    object.activeFlags = ACTIVE_FLAG_ACTIVE;
    gCurrLevelNum = 0;
    EXPECT_EQ(bowser_flame_has_fixed_arena_floor(), FALSE);
    return TRUE;
}

static int fight_three_flame_tuning(void) {
    const s32 levels[] = { LEVEL_BOWSER_1, LEVEL_BOWSER_2, LEVEL_BOWSER_3 };
    for (unsigned fight = 0; fight < sizeof(levels) / sizeof(*levels); fight++) {
        s32 fightThree = fight == 2;
        s16 blueTurn = fightThree ? DEGREES(3) : 0xC0;
        void (*blueLoops[])(void) = { bhv_blue_bowser_flame_loop, bhv_flame_floating_landing_loop,
                                      bhv_flame_bouncing_loop };
        for (unsigned kind = 0; kind < sizeof(blueLoops) / sizeof(*blueLoops); kind++) {
            begin();
            gCurrLevelNum = levels[fight];
            object.oBehParams2ndByte = 1;
            blueLoops[kind]();
            EXPECT_EQ(object.oMoveAngleYaw, blueTurn);
            if (kind == 2) {
                EXPECT_EQ(object.oForwardVel, fightThree ? 20 : 15);
            }
        }
        begin();
        gCurrLevelNum = levels[fight];
        object.oBehParams2ndByte = 1;
        object.oBehParams = BOWSER_FLAME_NO_SPLIT;
        bhv_blue_bowser_flame_init();
        EXPECT_EQ(object.oForwardVel, 30);
        object.oMoveFlags = OBJ_MOVE_LANDED;
        bhv_blue_bowser_flame_loop();
        EXPECT_EQ(object.oMoveAngleYaw, blueTurn);
        EXPECT_EQ(object.oForwardVel, fightThree ? 30 : 20);
        EXPECT_EQ(object.oAction, 1);
        object.oMoveFlags = OBJ_MOVE_ON_GROUND;
        for (int frame = 1; frame <= 12; frame++) {
            object.oTimer = frame - 1;
            object.oMoveAngleYaw = 0;
            bhv_blue_bowser_flame_loop();
            EXPECT_EQ(object.oForwardVel,
                      fightThree ? MAX(25.0f, 30.0f - frame * 0.5f) : MAX(15, 20 - frame));
            EXPECT_EQ(object.oMoveAngleYaw,
                      fightThree ? DEGREES(3) : BOWSER_SPIT_FLAME_GROUND_TURN_SPEED);
            EXPECT_EQ(spawnedCount, 0);
        }
        begin();
        gCurrLevelNum = levels[fight];
        object.oAction = 1;
        bhv_flame_bowser_loop();
        EXPECT_EQ(object.oMoveAngleYaw, fightThree ? DEGREES(1) : 0x20);
        int expectedRedSpeed;
        if (fightThree) {
            expectedRedSpeed = 15;
        } else {
            expectedRedSpeed = fight == 0 ? 5 : 10;
        }
        EXPECT_EQ(object.oForwardVel, expectedRedSpeed);
        begin();
        gCurrLevelNum = levels[fight];
        object.oForwardVel = 10;
        bhv_flame_bowser_loop();
        EXPECT_EQ(object.oMoveAngleYaw, 0x20);
    }
    gCurrLevelNum = 0;
    return TRUE;
}

static int original_preset_initialization(void) {
    const int levels[] = { LEVEL_BOWSER_1, LEVEL_BOWSER_2, LEVEL_BOWSER_3 };
    const int health[][3] = { { 3, 4, 5 }, { 1, 1, 3 } };
    for (int preset = 0; preset < 2; preset++) {
        for (int arena = 0; arena < 3; arena++) {
            begin();
            sTestGameOptions[GO_BOWSER_FIGHTS] = preset;
            gCurrLevelNum = levels[arena];
            bhv_bowser_init();
            EXPECT_EQ(object.oHealth, health[preset][arena]);
            EXPECT_EQ(object.oBehParams2ndByte, arena);
            EXPECT_EQ(object.oBowserPostAttackFacingTimer, -1);
            EXPECT_EQ(object.oOpacity, 255);
            EXPECT_EQ(object.oAction, BOWSER_ACT_WAIT);
        }
    }
    return TRUE;
}

static int original_preset_decisions(void) {
    const float samples[] = { 0, 0.09f, 0.39f, 0.49f, 0.5f, 0.79f, 0.8f, 0.99f };
    for (unsigned index = 0; index < sizeof(samples) / sizeof(*samples); index++) {
        begin();
        sTestGameOptions[GO_BOWSER_FIGHTS] = 1;
        object.oBowserStatus = BOWSER_STATUS_ANGLE_MARIO;
        object.oDistanceToMario = 500;
        suppliedRandom = samples[index];
        bowser_bitdw_actions();
        EXPECT_EQ(object.oAction, BOWSER_ACT_BREATH_FIRE);
        object.oBowserIsReacting = FALSE;
        object.oDistanceToMario = 1800;
        bowser_bitdw_actions();
        EXPECT_EQ(object.oAction, BOWSER_ACT_QUICK_JUMP);
        object.oBowserIsReacting = FALSE;
        object.oDistanceToMario = 500;
        bowser_bitfs_actions();
        EXPECT_EQ(object.oAction,
                  samples[index] < 0.5f ? BOWSER_ACT_TELEPORT : BOWSER_ACT_SPIT_FIRE_ONTO_FLOOR);
        object.oBowserIsReacting = FALSE;
        object.oDistanceToMario = 1800;
        object.oBowserDistToCenter = 1000;
        bowser_bitfs_actions();
        EXPECT_EQ(object.oAction,
                  samples[index] < 0.5f ? BOWSER_ACT_BIG_JUMP : BOWSER_ACT_CHARGE_MARIO);
        object.oDistanceToMario = 500;
        bowser_bits_action_list();
        EXPECT_EQ(object.oAction, samples[index] < 0.4f   ? BOWSER_ACT_SPIT_FIRE_ONTO_FLOOR
                                  : samples[index] < 0.8f ? BOWSER_ACT_SPIT_FIRE_INTO_SKY
                                                          : BOWSER_ACT_BREATH_FIRE);
        object.oDistanceToMario = 1800;
        bowser_bits_action_list();
        EXPECT_EQ(object.oAction,
                  samples[index] < 0.5f ? BOWSER_ACT_BIG_JUMP : BOWSER_ACT_CHARGE_MARIO);
    }
    return TRUE;
}

static int original_preset_fire(void) {
    begin();
    sTestGameOptions[GO_BOWSER_FIGHTS] = 1;
    gCurrLevelNum = LEVEL_BOWSER_3;
    // An orphaned flame controller must retire safely before reading its parent.
    object.parentObj = NULL;
    bhv_bowser_flame_spawn_loop();
    EXPECT_EQ(spawnedCount, 0);
    object.oInteractionSubtype = INT_SUBTYPE_BOWSER_ATTACK;
    bhv_flame_bowser_init();
    EXPECT_EQ(absf(object.oGravity - (-1)) < 0.001f, TRUE);
    EXPECT_EQ(object.oInteractionSubtype & INT_SUBTYPE_BOWSER_ATTACK, 0);
    bhv_flame_floating_landing_init();
    EXPECT_EQ(absf(object.oGravity - (-1)) < 0.001f, TRUE);
    bhv_blue_bowser_flame_init();
    EXPECT_EQ(absf(object.oGravity - (1)) < 0.001f, TRUE);
    EXPECT_EQ(object.oForwardVel, 35);
    object.oTimer = 21;
    object.oBehParams2ndByte = 0;
    bhv_blue_bowser_flame_loop();
    EXPECT_EQ(spawnedCount, 3);
    EXPECT_EQ(object.activeFlags, ACTIVE_FLAG_DEACTIVATED);
    EXPECT_EQ(children[0].behavior, bhvFlameFloatingLanding);
    begin();
    sTestGameOptions[GO_BOWSER_FIGHTS] = 1;
    gCurrLevelNum = LEVEL_BOWSER_3;
    object.header.gfx.animInfo.animFrame = 5;
    bowser_act_spit_fire_onto_floor();
    EXPECT_EQ(spawnedCount, 1);
    EXPECT_EQ(children[0].behavior, bhvMovingFlame);
    EXPECT_EQ(children[0].oSmallPiranhaFlameStartSpeed, 30);
    EXPECT_EQ(children[0].oSmallPiranhaFlameEndSpeed, 10);
    // A Bowser preset does not change unrelated flames in other courses.
    begin();
    sTestGameOptions[GO_BOWSER_FIGHTS] = 1;
    gCurrLevelNum = 99;
    bhv_flame_floating_landing_init();
    EXPECT_EQ(absf(object.oGravity - (-1)) < 0.001f, TRUE); // Current at the supplied midpoint.
    suppliedRandom = 0;
    bhv_flame_floating_landing_init();
    EXPECT_EQ(absf(object.oGravity - (-0.8f)) < 0.001f, TRUE);
    gCurrLevelNum = LEVEL_BOWSER_1;
    return TRUE;
}

static int original_preset_timing_and_arena(void) {
    begin();
    sTestGameOptions[GO_BOWSER_FIGHTS] = 1;
    object.oBehParams2ndByte = BOWSER_BP_BITS;
    object.header.gfx.animInfo.animFrame = 8;
    EXPECT_EQ(bowser_set_anim_jump(), FALSE);
    object.header.gfx.animInfo.animFrame = 11;
    EXPECT_EQ(bowser_set_anim_jump(), TRUE);
    object.oAction = BOWSER_ACT_TELEPORT;
    object.oBowserTargetOpacity = 0;
    bowser_update_opacity();
    EXPECT_EQ(object.oOpacity, 235);
    object.oOpacity = 0;
    object.oSubAction = BOWSER_SUB_ACT_TELEPORT_START;
    bowser_act_teleport();
    EXPECT_EQ(object.oBowserTimer, 30);
    EXPECT_EQ(object.oSubAction, BOWSER_SUB_ACT_TELEPORT_MOVE);
    bowser_act_teleport();
    EXPECT_EQ(object.oForwardVel, 100);
    EXPECT_EQ(object.oBowserTimer, 29);
    object.oAction = BOWSER_ACT_QUICK_JUMP;
    object.oSubAction = 1;
    object.oMoveFlags = OBJ_MOVE_LANDED;
    bowser_act_quick_jump();
    EXPECT_EQ(object.oSubAction, 2);
    EXPECT_EQ(spawnedCount, 0);
    object.oAction = BOWSER_ACT_BIG_JUMP;
    object.oSubAction = 1;
    bowser_act_big_jump();
    EXPECT_EQ(object.oSubAction, 2);
    EXPECT_EQ(spawnedCount, 1);
    EXPECT_EQ(children[0].oBehParams2ndByte, 0);
    spawnedCount = 0;
    object.oHealth = 2;
    gCurrLevelNum = LEVEL_BOWSER_3;
    bowser_arena_landed(&object);
    bowser_arena_update(&object);
    bowser_arena_bomb_destroyed(&object);
    bowser_arena_platform_destroyed(&object);
    EXPECT_EQ(spawnedCount, 0);
    EXPECT_EQ(object.bowserArenaStage, 0);
    gCurrLevelNum = LEVEL_BOWSER_1;
    return TRUE;
}

static int original_preset_hold(void) {
    begin();
    sTestGameOptions[GO_BOWSER_FIGHTS] = 1;
    object.oHeldState = HELD_HELD;
    heldMario.heldObj = &object;
    heldMario.marioObj = &marioObject;
    for (int frame = 0; frame < 240; frame++) {
        bowser_held_update();
        EXPECT_EQ(heldMario.heldObj, &object);
        EXPECT_EQ(object.oHeldState, HELD_HELD);
    }
    object.prevObj = &bowser;
    object.oHeldState = HELD_THROWN;
    object.oBowserHeldAngleVelYaw = 1000;
    bowser_thrown_dropped_update();
    EXPECT_EQ(object.oHeldState, HELD_FREE);
    EXPECT_EQ(object.oBowserGrabCooldown, 0);
    EXPECT_EQ(object.oAction, BOWSER_ACT_THROWN);
    return TRUE;
}

int main(int argc, char **argv) {
    test_init(argc, argv);
    struct {
        const char *name;
        int (*run)(void);
    } cases[] = { { "bowser.original_preset_initialization", original_preset_initialization },
                  { "bowser.original_preset_decisions", original_preset_decisions },
                  { "bowser.original_preset_fire", original_preset_fire },
                  { "bowser.original_preset_timing_and_arena", original_preset_timing_and_arena },
                  { "bowser.original_preset_hold", original_preset_hold },
                  { "bowser.airborne_fire", airborne_fire },
                  { "bowser.fire_landing", fire_landing },
                  { "bowser.moving_fire_fixed_floor", moving_fire_fixed_floor },
                  { "bowser.blue_fire_homing", blue_fire_homing },
                  { "bowser.fight_three_flame_tuning", fight_three_flame_tuning },
                  { "bowser.blue_spit_coin_drop", blue_spit_coin_drop },
                  { "bowser.red_fire_homing", red_fire_homing },
                  { "bowser.varied_fire_gravity", varied_fire_gravity },
                  { "bowser.blue_fire_gravity", blue_fire_gravity },
                  { "bowser.landing_impacts", landing_impacts },
                  { "bowser.staggered_fire_attacks", staggered_fire_attacks },
                  { "bowser.staggered_blue_groups", staggered_blue_groups },
                  { "bowser.punch_tail_grabs", punch_tail_grabs },
                  { "bowser.tail_recovery", tail_recovery },
                  { "bowser.dive_tail_grabs", dive_tail_grabs },
                  { "bowser.held_visibility", held_visibility },
                  { "bowser.straight_walk_speed", straight_walk_speed },
                  { "bowser.hold_timeout", hold_timeout },
                  { "bowser.escape_jump_center", escape_jump_center },
                  { "bowser.fight_one_decisions", fight_one_decisions },
                  { "bowser.fight_two_decisions", fight_two_decisions },
                  { "bowser.fight_three_decisions", fight_three_decisions },
                  { "bowser.fight_three_move_properties", fight_three_move_properties },
                  { "bowser.fight_three_sky_fire", fight_three_sky_fire },
                  { "bowser.bomb_landing_light_wave", bomb_landing_light_wave },
                  { "bowser.missed_throw_landing", missed_throw_landing },
                  { "bowser.missed_throw_stat", missed_throw_stat },
                  { "bowser.mine_recovery_wave", mine_recovery_wave },
                  { "bowser.defeated_offstage_recovery", defeated_offstage_recovery },
                  { "bowser.interrupted_jump_drag", interrupted_jump_drag },
                  { "bowser.boo_reward_cleanup", boo_reward_cleanup },
                  { "bowser.quick_jump_offstage_return", quick_jump_offstage_return },
                  { "bowser.arena_enemy_fade", arena_enemy_fade },
                  { "bowser.arena_encounter", arena_encounter },
                  { "bowser.final_cleanup_held_enemy", final_arena_cleanup_releases_held_enemy },
                  { "bowser.arena_recurring_boo", arena_recurring_boo },
                  { "bowser.arena_enemy_survival", arena_enemy_survival },
                  { "bowser.arena_enemy_distance_recovery", arena_enemy_distance_recovery },
                  { "bowser.arena_fire_spitter_waves", arena_fire_spitter_waves },
                  { "bowser.turn_speed_and_edge", turn_speed_and_edge },
                  { "bowser.walking_interrupt", walking_interrupt },
                  { "bowser.accidental_jump_recovery", accidental_jump_recovery },
                  { "bowser.return_jump_platform_break", return_jump_platform_break },
                  { "bowser.arena_coin_policy", arena_coin_policy },
                  { "bowser.cap_arena_recovery", cap_arena_recovery },
                  { "bowser.fight_two_move_properties", fight_two_move_properties },
                  { "bowser.teleport_choices", teleport_choices },
                  { "bowser.quick_jump_timing", quick_jump_timing },
                  { "bowser.charge_timing", charge_timing },
                  { "bowser.floor_spit_burst", floor_spit_burst },
                  { "bowser.post_attack_center_jump", post_attack_center_jump },
                  { "bowser.throw_release", throw_release } };
    for (unsigned i = 0; i < sizeof(cases) / sizeof(*cases); i++) {
        if (test_selected(cases[i].name)) {
            int ok = cases[i].run();
            test_report(cases[i].name, ok);
            if (!ok) {
                return 1;
            }
        }
    }
    return test_finish();
}
