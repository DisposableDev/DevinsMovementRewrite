#include <assert.h>
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "object_constants.h"
#include "activity_segment_counts.inc"
typedef float f32;
typedef int32_t s32;
typedef int16_t s16;
typedef uint32_t u32;
typedef uint16_t u16;
typedef uint8_t u8;
typedef uintptr_t BehaviorScript;
#define TRUE 1
#define FALSE 0
#define LEVEL_PHYSICS_LAB 32
#define GRAPH_RENDER_ACTIVE 1
#define GRAPH_RENDER_HAS_ANIMATION 32
#define INT_STATUS_NONE 0
#include "activity_time_stop.inc"
#include "activity_floor_limit.inc"
#define BITMASK(n) ((1u << (n)) - 1u)
#define ARRAY_COUNT(a) (sizeof(a) / sizeof(*(a)))
#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define absf fabsf
#define COND_BIT(c, f, b)                                                                              \
    do {                                                                                               \
        if (c)                                                                                         \
            (f) |= (b);                                                                                \
        else                                                                                           \
            (f) &= ~(b);                                                                               \
    } while (0)
#define segmented_to_virtual(p) (p)
struct Surface {
    void *object;
    struct {
        f32 y;
    } normal;
};
struct Object {
    struct {
        struct {
            struct {
                u32 flags;
            } node;
            f32 scale[3];
            struct {
                s16 animFrame;
            } animInfo;
        } gfx;
    } header;
    struct Object *parentObj, *platform, *prevObj, *oMrBlizzardHeldObj;
    void *collisionData;
    const BehaviorScript *behavior, *curBhvCommand;
    s32 activeFlags, numCollidedObjs, oHeldState, oInteractStatus;
    s32 metalContactCooldown, metalContactPendingDefeat;
    s32 oGroundTwirlStunTimer, oGroundTwirlContactRebound, oGroundTwirlStunDuration;
    s32 oAction, oPrevAction, oTimer, oSubAction, oFlags, oMoveFlags, oRoom;
    s32 faceAngles[3], moveAngles[3], oFaceAngleYaw, oMoveAngleYaw, oAngleToMario;
    s32 oPiranhaPlantSleepMusicState;
    s32 oKoopaMovementType, oBobombFuseLit, oBobombFuseTimer, oScuttlebugIsAtttacking;
    s32 oFirePiranhaPlantActive, oFirePiranhaPlantDeathSpinTimer;
    f32 oFirePiranhaPlantScale, oForwardVel;
    f32 position[3], oHomeX, oHomeZ, oHomeY, oVelY, oFloorHeight;
    f32 oCollisionDistance, oDrawingDistance, oDistanceToMario;
    s32 oAngleVelPitch;
    s32 oAnimState;
    s32 oBehParams;
    s32 oBehParams2ndByte;
    s32 oBooDeathStatus;
    s32 oBooTargetOpacity;
    f32 oBubbaAirVelY;
    f32 oBubbaJumpHeight;
    s32 oBubbaLungeTimer;
    s32 oChainChompReleaseStatus;
    f32 oChainChompSignedMaxDistBetweenChainParts;
    s32 oClamShakeTimer;
    s32 oCommonAnchorAction;
    s32 oHauntedChairSpinTimer;
    s32 oHeaveHoThrowState;
    f32 oMrBlizzardGraphYOffset;
    s32 oMrIBlinking;
    s32 oOpacity;
    f32 oPokeyBottomBodyPartSize;
    s32 oPokeyHeadWasKilled;
    s32 oPokeyNumAliveBodyParts;
    s32 oSpindelMoveTimer;
    s32 oUkikiHasCap;
    s32 oUkikiTextState;
    s32 oUnagiHasStar;
    f32 oVelZ;
    s32 oWaterBombSpawnerBombActive;
    s32 oEnemyLakituNumSpinies;
    s32 oCannonIsActive, oBreakableBoxSmallReleased, oToadMessageState, oHootAvailability;
    s32 oToadMessageRecentlyTalked;
    struct Surface *oFloor;
};
#define oPosVec position[0]
#define oFaceAngleVec faceAngles[0]
#define oMoveAngleVec moveAngles[0]
#define oPosX position[0]
#define oPosY position[1]
#define oPosZ position[2]
struct MarioState {
    f32 pos[3], vel[3], forwardVel;
} mario;
static struct MarioState *gMarioState = &mario;
static struct Object player, *gMarioObject = &player, *o;
static u16 gAreaUpdateCounter;
static s32 gCurrLevelNum = LEVEL_PHYSICS_LAB, gTimeStopState;
struct PhysicsLabRenderSection {
    s16 min[3], max[3];
    void *displayList;
};
static int inView, viewCalls, nativeCalls, angleCalls, distanceCalls, gfxCalls, emitted, scales;
static struct PhysicsLabRenderSection lastBounds;
static s32 physics_lab_activity_in_view(const struct PhysicsLabRenderSection *bounds) {
    viewCalls++;
    lastBounds = *bounds;
    return inView;
}
static s32 physics_lab_enemy_in_view(const struct PhysicsLabRenderSection *bounds) {
    return physics_lab_activity_in_view(bounds);
}
static s32 object_water_effect_in_view(const struct PhysicsLabRenderSection *bounds) {
    return physics_lab_activity_in_view(bounds);
}
#include "activity_behaviors.inc"
static int needsCollision;
static s32 object_platform_motion_needs_collision(struct Object *obj) {
    (void) obj;
    return needsCollision;
}
static int platformMotionActive;
static s32 object_platform_motion_is_active(struct Object *obj) {
    (void) obj;
    return platformMotionActive;
}
static struct Object *sMontyMoleHoleList;
#include "activity_mole_wake.inc"
static const BehaviorScript bhvSkeeterWave[] = { 80 };
static void obj_mark_for_deletion(struct Object *obj) {
    obj->activeFlags = ACTIVE_FLAG_DEACTIVATED;
}
static f32 waterLevel;
static f32 find_water_level(f32 x, f32 z) {
    (void) x;
    (void) z;
    return waterLevel;
}
s32 enemy_should_sleep(struct Object *obj);
#include "activity_activity.inc"

typedef s32 (*BhvCommandProc)(void);
#define BHV_PROC_CONTINUE 0
#define MARIO_OUTSIDE_ROOM 0
#define MARIO_INSIDE_ROOM 1
static const BehaviorScript *gCurBhvCommand;
static const BehaviorScript command[] = { 0 };
static void (*native)(void);
static s32 behavior_command(void) {
    nativeCalls++;
    if (native) {
        native();
    }
    gCurBhvCommand = command;
    return 1;
}
static BhvCommandProc BehaviorCmdTable[] = { behavior_command };
static s32 marioRoom = MARIO_INSIDE_ROOM;
static s32 cur_obj_is_mario_in_room(void) {
    return marioRoom;
}
static void cur_obj_disable_rendering_in_room(void) {
    o->header.gfx.node.flags &= ~GRAPH_RENDER_ACTIVE;
    o->activeFlags |= ACTIVE_FLAG_IN_DIFFERENT_ROOM;
}
static void cur_obj_enable_rendering_in_room(void) {
    o->header.gfx.node.flags |= GRAPH_RENDER_ACTIVE;
    o->activeFlags &= ~ACTIVE_FLAG_IN_DIFFERENT_ROOM;
}
static f32 dist_between_objects(struct Object *a, struct Object *b) {
    distanceCalls++;
    f32 dx = a->oPosX - b->oPosX, dy = a->oPosY - b->oPosY, dz = a->oPosZ - b->oPosZ;
    return sqrtf(dx * dx + dy * dy + dz * dz);
}
static s32 obj_angle_to_object(struct Object *a, struct Object *b) {
    (void) a;
    (void) b;
    angleCalls++;
    return 123;
}
static void vec3i_copy(s32 *a, const s32 *b) {
    memcpy(a, b, 3 * sizeof(s32));
}
static void cur_obj_move_xz_using_fvel_and_yaw(void) {
    assert(0);
}
static void cur_obj_move_y_with_terminal_vel(void) {
    assert(0);
}
static void obj_build_transform_relative_to_parent(struct Object *obj) {
    (void) obj;
    assert(0);
}
static void obj_set_throw_matrix_from_transform(struct Object *obj) {
    (void) obj;
    assert(0);
}
static void obj_update_gfx_pos_and_angle(struct Object *obj) {
    (void) obj;
    gfxCalls++;
}
// Cosmetic splash emission is outside the activity and update bookkeeping fixture.
static void obj_update_thrown_water_splash(struct Object *obj, s32 afterMovement) {
    (void) obj;
    (void) afterMovement;
}
static f32 cur_obj_drawing_distance(f32 distance) {
    (void) distance;
    return o->oDrawingDistance;
}
#include "activity_update.inc"
#define gCurrentObject o
static u8 updateVisited;
static int platformApplies, platformFinishes, awakeCount, sleepCount;
static u8 *object_platform_motion_start_update(struct Object *obj, s32 *rider) {
    (void) obj;
    *rider = TRUE;
    return updateVisited ? NULL : &updateVisited;
}
static struct Object *object_platform_motion_provider(struct Object *obj) {
    (void) obj;
    return NULL;
}
static void object_platform_motion_apply(struct Object *obj) {
    (void) obj;
    platformApplies++;
}
static void object_platform_motion_finish(struct Object *obj) {
    (void) obj;
    platformFinishes++;
}
static void update_object_with_ground_twirl_stun(void) {
    cur_obj_update();
}
static void profiler_count_object(s32 sleeping) {
    if (sleeping) {
        sleepCount++;
    } else {
        awakeCount++;
    }
}
static int collisionPublications, collisionRevealsRendering;
static void load_object_collision_model(void) {
    collisionPublications++;
    if (collisionRevealsRendering) {
        o->header.gfx.node.flags |= GRAPH_RENDER_ACTIVE;
    }
}
static s32 bbh_merry_go_round_update_dormancy(struct Object *obj) {
    (void) obj;
    return FALSE;
}
#include "activity_object_wrapper.inc"

#define MODEL_IDLE_WATER_WAVE 1
static struct {
    s16 relPosX, relPosZ;
} sSkeeterRelPositions[] = { { -130, -190 }, { 130, -190 }, { -180, 130 }, { 180, 130 } };
static void spawn_object_relative_with_scale(int a, int x, int y, int z, float scale,
                                             struct Object *obj, int model,
                                             const BehaviorScript *behavior) {
    (void) a;
    (void) x;
    (void) y;
    (void) z;
    (void) obj;
    assert(scale == 0.8f && model == MODEL_IDLE_WATER_WAVE && behavior == bhvSkeeterWave);
    emitted++;
}
#include "activity_waves.inc"
static int sExclamationBoxActions[6];
static void cur_obj_scale(f32 s) {
    scales++;
    for (int i = 0; i < 3; i++) {
        o->header.gfx.scale[i] = s;
    }
}
#include "activity_box_respawn.inc"
static void cur_obj_call_action_function(int *actions) {
    assert(actions == sExclamationBoxActions);
    // Collision loading is observed separately, not simulated here.
    if (o->oAction == EXCLAMATION_BOX_ACT_WAIT_FOR_RESPAWN) {
        exclamation_box_act_wait_for_respawn();
    }
}
#include "activity_box.inc"
static struct Surface floorSurface;
static void setup(struct Object *obj, const BehaviorScript *behavior) {
    memset(obj, 0, sizeof(*obj));
    obj->behavior = behavior;
    obj->parentObj = obj;
    obj->activeFlags = ACTIVE_FLAG_ACTIVE;
    obj->oFloor = &floorSurface;
    obj->oPosX = obj->oHomeX = 12000;
    obj->oPosZ = obj->oHomeZ = 12000;
    obj->oTimer = 2;
    obj->oMoveFlags = OBJ_MOVE_ON_GROUND;
    obj->oRoom = -1;
    obj->oFlags = OBJ_FLAG_COMPUTE_DIST_TO_MARIO | OBJ_FLAG_COMPUTE_ANGLE_TO_MARIO
                  | OBJ_FLAG_UPDATE_GFX_POS_AND_ANGLE;
    obj->curBhvCommand = command;
    obj->oDrawingDistance = 20000;
    for (int a = 0; a < 3; a++) {
        obj->header.gfx.scale[a] = 1;
    }
    memset(&mario, 0, sizeof(mario));
    memset(&player, 0, sizeof(player));
    gCurrLevelNum = LEVEL_PHYSICS_LAB;
    gTimeStopState = 0;
    waterLevel = 0;
    inView = 0;
    marioRoom = MARIO_INSIDE_ROOM;
    native = NULL;
    nativeCalls = angleCalls = distanceCalls = gfxCalls = emitted = scales = viewCalls = 0;
    platformMotionActive = needsCollision = collisionPublications = collisionRevealsRendering = 0;
    floorSurface.normal.y = 1.0f;
    platformApplies = platformFinishes = awakeCount = sleepCount = 0;
    gAreaUpdateCounter++;
    o = obj;
}
static void tick(void) {
    gAreaUpdateCounter++;
    updateVisited = FALSE;
    update_object_with_platform_motion();
}
static void sleep_initialization(void) {
    struct Object obj;
    for (int reuse = 0; reuse < 2; reuse++) {
        setup(&obj, bhvGoomba);
        obj.oTimer = 0;
        obj.oDrawingDistance = 4000;
        obj.curBhvCommand = obj.behavior;
        assert(!(obj.oFlags & OBJ_FLAG_ALLOW_ENEMY_SLEEP));
        tick();
        assert(nativeCalls == 1 && obj.oTimer == 1);
        assert(obj.oFlags & OBJ_FLAG_ALLOW_ENEMY_SLEEP);
        assert(obj.curBhvCommand == command);
        obj.oTimer = 31;
        tick();
        assert(nativeCalls == 1 && obj.oTimer == 31);
    }
    puts("initialization: first script executes before sleep, including a reused object slot");
}

static void sleep_updates(void) {
    struct Object obj;
    const BehaviorScript *behaviors[] = {
        bhvGoomba,         bhvSmallBully,   bhvSmallChillBully, bhvSpindrift,
        bhvSpiny,          bhvPiranhaPlant, bhvSwoop,           bhvSnufit,
        bhvFlyGuy,         bhvHomingAmp,    bhvCirclingAmp,
#ifdef FLOOMBAS
        bhvFloomba,
#endif
        bhvKoopa,          bhvBobomb,       bhvScuttlebug,      bhvFirePiranhaPlant,
        bhvMoneybagHidden, bhvMadPiano
    };
    for (u32 i = 0; i < ARRAY_COUNT(behaviors); i++) {
        setup(&obj, behaviors[i]);
        enemy_sleep_init(&obj);
        assert(obj.oFlags & OBJ_FLAG_ALLOW_ENEMY_SLEEP);
        obj.oTimer = 31;
        obj.oDrawingDistance = 4000;
        if (behaviors[i] == bhvPiranhaPlant) {
            obj.oAction = obj.oPrevAction = PIRANHA_PLANT_ACT_SLEEPING;
            obj.oPiranhaPlantSleepMusicState = 1;
        } else if (behaviors[i] == bhvCirclingAmp) {
            obj.oAction = obj.oPrevAction = AMP_ACT_IDLE;
        } else if (behaviors[i] == bhvKoopa) {
            obj.oKoopaMovementType = KOOPA_BP_NORMAL;
            obj.oAction = obj.oPrevAction = KOOPA_SHELLED_ACT_WALK;
        } else if (behaviors[i] == bhvScuttlebug) {
            obj.oSubAction = 1;
        }
        for (int cycle = 0; cycle < 3; cycle++) {
            for (int j = 0; j < 240; j++) {
                tick();
            }
            assert(nativeCalls == cycle * 2 && distanceCalls == (cycle + 1) * 240 + cycle * 2
                   && gfxCalls == cycle * 2);
            assert(obj.oTimer == 31 + cycle * 2);
            assert(platformApplies == cycle * 2 && platformFinishes == cycle * 2);
            assert(sleepCount == (cycle + 1) * 240 && awakeCount == cycle * 2);
            assert(!(obj.header.gfx.node.flags & GRAPH_RENDER_HAS_ANIMATION));
            player.oPosX = obj.oPosX;
            player.oPosZ = obj.oPosZ;
            inView = 1;
            tick();
            assert(nativeCalls == cycle * 2 + 1 && obj.oTimer == 32 + cycle * 2);
            assert(obj.header.gfx.node.flags & GRAPH_RENDER_HAS_ANIMATION);
            player.oPosX = player.oPosZ = 0;
            tick(); // One ordinary update hides the enemy again.
            inView = 0;
        }
        // A changed script cannot inherit eligibility from its old flags.
        obj.behavior = command;
        tick();
        assert(nativeCalls == 7 && obj.oTimer == 38);
    }
    puts("sleep: eighteen idle families skip 720 distant updates each; three immediate wake/resleep "
         "cycles");
}

static void sleep_guards(void) {
    struct Object obj;
    setup(&obj, bhvGoomba);
    obj.oFlags |= OBJ_FLAG_ALLOW_ENEMY_SLEEP;
    obj.oTimer = 31;
    obj.oDrawingDistance = 4000;
    assert(enemy_should_sleep(&obj));
    // Both Mario's actual position and the home activation volume wake now.
    obj.oPosX = 2000;
    obj.oPosZ = 0;
    tick();
    assert(nativeCalls == 1 && distanceCalls == 1);
    obj.oPosX = 2001;
    tick();
    assert(nativeCalls == 1);
    obj.oHomeX = obj.oHomeZ = 0;
    tick();
    assert(nativeCalls == 2);
    obj.oHomeX = 12000;
    obj.oHomeZ = 12000;
    obj.oDrawingDistance = 10000;
    tick();
    assert(nativeCalls == 2); // Drawing range no longer extends the sleep radius.
    obj.oDrawingDistance = 4000;
#define CHECK_WAKE(field, value)                                                                       \
    do {                                                                                               \
        obj.field = (value);                                                                           \
        assert(!enemy_should_sleep(&obj));                                                             \
        obj.field = 0;                                                                                 \
    } while (0)
    CHECK_WAKE(oInteractStatus, 1);
    CHECK_WAKE(numCollidedObjs, 1);
    CHECK_WAKE(metalContactCooldown, 1);
    CHECK_WAKE(metalContactPendingDefeat, 1);
    CHECK_WAKE(oGroundTwirlStunTimer, 1);
    CHECK_WAKE(oGroundTwirlContactRebound, 1);
    CHECK_WAKE(oGroundTwirlStunDuration, 1);
    CHECK_WAKE(oHeldState, HELD_HELD);
    CHECK_WAKE(oVelY, 10);
    CHECK_WAKE(platform, &player);
    CHECK_WAKE(collisionData, &player);
#undef CHECK_WAKE
    obj.oAction = GOOMBA_ACT_JUMP;
    obj.oPrevAction = obj.oAction;
    assert(!enemy_should_sleep(&obj));
    obj.oAction = obj.oPrevAction = GOOMBA_ACT_WALK;
    obj.oPrevAction = GOOMBA_ACT_JUMP;
    assert(!enemy_should_sleep(&obj));
    obj.oPrevAction = obj.oAction;
    for (int flags = 0; flags < 2; flags++) {
        obj.oMoveFlags = flags ? OBJ_MOVE_ABOVE_LAVA : OBJ_MOVE_IN_AIR;
        assert(!enemy_should_sleep(&obj));
    }
    obj.oMoveFlags = OBJ_MOVE_ON_GROUND;
    obj.parentObj = &player;
    assert(!enemy_should_sleep(&obj));
    obj.parentObj = &obj;
    floorSurface.object = &player;
    assert(!enemy_should_sleep(&obj));
    floorSurface.object = NULL;
    platformMotionActive = 1;
    assert(!enemy_should_sleep(&obj));
    platformMotionActive = needsCollision = collisionPublications = collisionRevealsRendering = 0;
    floorSurface.normal.y = 1.0f;
    obj.header.gfx.node.flags |= GRAPH_RENDER_ACTIVE;
    inView = 1;
    assert(!enemy_should_sleep(&obj));
    inView = 0;
    assert(enemy_should_sleep(&obj));
    assert(lastBounds.min[0] == obj.oPosX - 600);
    assert(lastBounds.max[0] == obj.oPosX + 600);
    obj.header.gfx.node.flags &= ~GRAPH_RENDER_ACTIVE;
    obj.oTimer = 30;
    assert(!enemy_should_sleep(&obj));
    obj.oTimer = 31;
    obj.oFlags |= OBJ_FLAG_ACTIVE_FROM_AFAR;
    assert(!enemy_should_sleep(&obj));
    obj.oFlags &= ~OBJ_FLAG_ACTIVE_FROM_AFAR;
    gTimeStopState = TIME_STOP_ACTIVE;
    assert(!enemy_should_sleep(&obj));
    gTimeStopState = 0;
    gCurrLevelNum = 1;
    obj.oFlags &= ~OBJ_FLAG_ALLOW_ENEMY_SLEEP;
    enemy_sleep_init(&obj);
    assert(obj.oFlags & OBJ_FLAG_ALLOW_ENEMY_SLEEP);
    obj.oPosX = obj.oHomeX = 2500;
    obj.oPosZ = obj.oHomeZ = 0;
    tick();
    assert(nativeCalls == 3);
    gCurrLevelNum = LEVEL_PHYSICS_LAB;
    obj.oFlags &= ~OBJ_FLAG_ALLOW_ENEMY_SLEEP;
    tick();
    assert(nativeCalls == 4);
    obj.oFlags |= OBJ_FLAG_ALLOW_ENEMY_SLEEP;
    gMarioObject = NULL;
    assert(!enemy_should_sleep(&obj));
    gMarioObject = &player;
    setup(&obj, bhvFlyGuy);
    obj.oFlags |= OBJ_FLAG_ALLOW_ENEMY_SLEEP;
    obj.oTimer = 0;
    obj.oDrawingDistance = 4000;
    obj.header.gfx.scale[0] = 1.5f;
    assert(enemy_should_sleep(&obj));
    obj.header.gfx.scale[0] = 1.0f;
    assert(!enemy_should_sleep(&obj));
    puts("wake guards: exact distance, home, drawing range, combat, state transitions, support, "
         "motion, time stop and other levels");
}

static void expanded_sleep_guards(void) {
    struct Object obj;
    setup(&obj, bhvKoopa);
    enemy_sleep_init(&obj);
    obj.oTimer = 31;
    obj.oKoopaMovementType = KOOPA_BP_UNSHELLED;
    obj.oAction = obj.oPrevAction = KOOPA_UNSHELLED_ACT_RUN;
    assert(enemy_should_sleep(&obj));
    obj.oKoopaMovementType = KOOPA_BP_KOOPA_THE_QUICK_BOB;
    assert(!enemy_should_sleep(&obj));
    obj.oKoopaMovementType = KOOPA_BP_NORMAL;
    obj.oAction = obj.oPrevAction = KOOPA_SHELLED_ACT_STOPPED;
    assert(enemy_should_sleep(&obj));
    obj.oAction = obj.oPrevAction = KOOPA_SHELLED_ACT_RUN_FROM_MARIO;
    assert(!enemy_should_sleep(&obj));

    setup(&obj, bhvBobomb);
    enemy_sleep_init(&obj);
    obj.oTimer = 31;
    obj.oMoveFlags = 0; // object_step stores support without setting move flags.
    assert(enemy_should_sleep(&obj));
    obj.oBobombFuseLit = 1;
    assert(!enemy_should_sleep(&obj));
    obj.oBobombFuseLit = 0;
    obj.oBobombFuseTimer = 1;
    assert(!enemy_should_sleep(&obj));
    obj.oBobombFuseTimer = 0;
    obj.oPosY = 10;
    assert(!enemy_should_sleep(&obj));
    obj.oPosY = 0;
    obj.oAction = obj.oPrevAction = BOBOMB_ACT_EXPLODE;
    assert(!enemy_should_sleep(&obj));

    setup(&obj, bhvScuttlebug);
    enemy_sleep_init(&obj);
    obj.oTimer = 31;
    obj.oSubAction = 1;
    assert(enemy_should_sleep(&obj));
    obj.oScuttlebugIsAtttacking = 1;
    assert(!enemy_should_sleep(&obj));
    obj.oScuttlebugIsAtttacking = 0;
    obj.oSubAction = 4;
    assert(!enemy_should_sleep(&obj));

    setup(&obj, bhvFirePiranhaPlant);
    enemy_sleep_init(&obj);
    obj.oTimer = 31;
    assert(enemy_should_sleep(&obj));
    obj.oFirePiranhaPlantActive = 1;
    assert(!enemy_should_sleep(&obj));
    obj.oFirePiranhaPlantActive = 0;
    obj.oFirePiranhaPlantScale = 0.5f;
    assert(!enemy_should_sleep(&obj));
    obj.oFirePiranhaPlantScale = 0;
    obj.oFirePiranhaPlantDeathSpinTimer = 1;
    assert(!enemy_should_sleep(&obj));

    setup(&obj, bhvMoneybagHidden);
    enemy_sleep_init(&obj);
    obj.oTimer = 31;
    assert(enemy_should_sleep(&obj));
    obj.oAction = obj.oPrevAction = FAKE_MONEYBAG_COIN_ACT_TRANSFORM;
    assert(!enemy_should_sleep(&obj));

    setup(&obj, bhvMadPiano);
    enemy_sleep_init(&obj);
    obj.oTimer = 0;
    assert(enemy_should_sleep(&obj));
    obj.curBhvCommand = obj.behavior;
    assert(!enemy_should_sleep(&obj));
    obj.curBhvCommand = command;
    obj.oAction = obj.oPrevAction = MAD_PIANO_ACT_ATTACK;
    assert(!enemy_should_sleep(&obj));

    setup(&obj, bhvGoomba);
    enemy_sleep_init(&obj);
    obj.oTimer = 31;
    obj.oDrawingDistance = 10000;
    obj.oPosX = obj.oHomeX = 2100;
    obj.oPosZ = obj.oHomeZ = 0;
    obj.header.gfx.node.flags |= GRAPH_RENDER_ACTIVE;
    tick();
    assert(nativeCalls == 0 && sleepCount == 1);
    inView = 1; // A camera turn wakes on this update while Mario stays distant.
    tick();
    assert(nativeCalls == 1 && awakeCount == 1);
    inView = 0;
    tick();
    assert(nativeCalls == 1 && sleepCount == 2);
    obj.oPosX = obj.oHomeX = 31000;
    assert(!enemy_should_sleep(&obj));
    puts("expanded states: races, fuses, attacks, recovery and initialization stay awake; camera turns "
         "wake immediately");
}

static void angles_and_effects(void) {
    struct Object obj;
    setup(&obj, bhvHomingAmp);
    inView = 1;
    tick();
    assert(angleCalls == 0 && nativeCalls == 1 && obj.oTimer == 3);
    obj.oAction = HOMING_AMP_ACT_CHASE;
    tick();
    assert(angleCalls == 1 && obj.oAngleToMario == 123);
    obj.oAction = HOMING_AMP_ACT_GIVE_UP;
    tick();
    assert(angleCalls == 1);
    setup(&obj, bhvSwoop);
    inView = 1;
    tick();
    assert(!angleCalls);
    mario.pos[0] = player.oPosX = obj.oPosX;
    mario.pos[2] = player.oPosZ = obj.oPosZ;
    tick();
    assert(angleCalls == 1);
    setup(&obj, command);
    gAreaUpdateCounter = 0;
    skeeter_spawn_waves();
    assert(emitted == 0);
    inView = 1;
    for (int i = 0; i < 120; i++) {
        gAreaUpdateCounter++;
        skeeter_spawn_waves();
    }
    assert(emitted == 120);  // Four waves on 30 of 120 distant updates.
    assert(viewCalls == 31); // No view checks on discarded cadence updates.
    inView = 0;
    mario.pos[0] = obj.oPosX;
    mario.pos[2] = obj.oPosZ;
    for (int i = 0; i < 120; i++) {
        gAreaUpdateCounter++;
        skeeter_spawn_waves();
    }
    assert(emitted == 360 && viewCalls == 31); // Nearby: 60 sets, no view tests.
    mario.pos[0] = mario.pos[2] = 0;
    gCurrLevelNum = 1;
    for (int i = 0; i < 120; i++) {
        gAreaUpdateCounter++;
        skeeter_spawn_waves();
    }
    assert(emitted == 360 && viewCalls == 151); // Ordinary offscreen: no allocations.
    inView = 1;
    for (int i = 0; i < 120; i++) {
        gAreaUpdateCounter++;
        skeeter_spawn_waves();
    }
    assert(emitted == 840);
    inView = 0;
    mario.pos[0] = obj.oPosX;
    mario.pos[2] = obj.oPosZ;
    for (int i = 0; i < 120; i++) {
        gAreaUpdateCounter++;
        skeeter_spawn_waves();
    }
    assert(emitted == 1320 && viewCalls == 271); // Nearby ordinary waves keep full cadence.
    mario.pos[0] = mario.pos[2] = 0;
    obj.oPosX = 2999;
    obj.oPosZ = 0;
    assert(physics_lab_should_emit_water_effect(&obj) && viewCalls == 271);
    obj.oPosX = 3000;
    assert(!physics_lab_should_emit_water_effect(&obj) && viewCalls == 272);
    inView = 1;
    assert(physics_lab_should_emit_water_effect(&obj) && viewCalls == 273);
    obj.oPosX = 30001;
    inView = 0;
    assert(physics_lab_should_emit_water_effect(&obj) && viewCalls == 273);
    puts("idle/effects: Lab cadence, ordinary near/visible retention and distant offscreen rejection");
}
static void boxes(void) {
    struct Object obj;
    setup(&obj, command);
    obj.oFlags = OBJ_FLAG_DONT_CALC_COLL_DIST;
    obj.oAction = obj.oPrevAction = EXCLAMATION_BOX_ACT_ACTIVE;
    obj.oCollisionDistance = 300;
    obj.oDistanceToMario = 19000;
    obj.oDrawingDistance = 4000;
    obj.oPosX = 4000;
    obj.oPosZ = 0;
    for (int a = 0; a < 3; a++) {
        obj.header.gfx.scale[a] = 2;
    }
    native = bhv_exclamation_box_loop;
    tick();
    assert(scales == 0 && !(obj.header.gfx.node.flags & GRAPH_RENDER_ACTIVE));
    obj.oPosX = 3999;
    tick();
    assert(scales == 0 && (obj.header.gfx.node.flags & GRAPH_RENDER_ACTIVE));
    obj.oPosX = 299;
    tick();
    assert(scales == 1);
    obj.oPosX = 4000;
    obj.oInteractStatus = 1;
    tick();
    assert(scales == 2);
    obj.oInteractStatus = 0;
    obj.oAction = EXCLAMATION_BOX_ACT_WAIT_FOR_RESPAWN;
    for (int i = 0; i < 302; i++) {
        tick();
    }
    assert(obj.oAction == EXCLAMATION_BOX_ACT_ACTIVE && obj.oTimer == 0);
    int before = scales;
    tick();
    assert(scales == before + 1 && obj.oTimer == 1);
    tick();
    assert(scales == before + 1 && obj.oTimer == 2);
#ifdef AUTO_COLLISION_DISTANCE
    obj.oFlags = 0;
    tick();
    assert(scales == before + 2);
#endif
    puts("boxes: exact range boundaries, attacks, near collision, complete respawn and first active "
         "update");
}

#include "activity_afar_scripts.inc"
static void enroll_settled(struct Object *obj, const BehaviorScript *behavior, s32 action) {
    setup(obj, behavior);
    enemy_sleep_init(obj);
    if (native_afar_script(behavior)) {
        obj->oFlags |= OBJ_FLAG_ACTIVE_FROM_AFAR;
    }
    obj->oAction = obj->oPrevAction = action;
    obj->oTimer = 31;
}

static void skeeter_water_sleeping(void) {
    struct Object obj;
    enroll_settled(&obj, bhvSkeeter, SKEETER_ACT_LUNGE);
    gCurrLevelNum = 9;
    obj.oMoveFlags = OBJ_MOVE_AT_WATER_SURFACE;
    obj.oPosY = waterLevel = 100;
    obj.oForwardVel = 80;
    assert(enemy_should_sleep(&obj) == SKEETER_WATER_SLEEP_TRIAL);
    if (!SKEETER_WATER_SLEEP_TRIAL) {
        return;
    }
    for (int frame = 0; frame < 90; frame++) {
        tick();
    }
    assert(nativeCalls == 0 && obj.oTimer == 31 && obj.oForwardVel == 80);
#if SKEETER_WATER_SLEEP_TRIAL
    gSkeeterWaterSleepTrial = 0;
    assert(!enemy_should_sleep(&obj));
    gSkeeterWaterSleepTrial = 1;
#endif
    waterLevel = 101;
    tick();
    assert(nativeCalls == 1); // Rising water restores native movement immediately.
    waterLevel = 99;
    assert(!enemy_should_sleep(&obj));
    waterLevel = FLOOR_LOWER_LIMIT;
    assert(!enemy_should_sleep(&obj));
    waterLevel = 100;
    assert(enemy_should_sleep(&obj));
    player.oPosX = obj.oPosX;
    player.oPosZ = obj.oPosZ;
    tick();
    assert(nativeCalls == 2);
    player.oPosX = player.oPosZ = 0;
    const s32 transientFlags[] = { OBJ_MOVE_ENTERED_WATER, OBJ_MOVE_LEAVING_WATER, OBJ_MOVE_IN_AIR,
                                   OBJ_MOVE_HIT_WALL, OBJ_MOVE_ON_GROUND };
    for (u32 i = 0; i < ARRAY_COUNT(transientFlags); i++) {
        obj.oMoveFlags = OBJ_MOVE_AT_WATER_SURFACE | transientFlags[i];
        assert(!enemy_should_sleep(&obj));
    }
    obj.oMoveFlags = OBJ_MOVE_AT_WATER_SURFACE;
    obj.oVelY = -1;
    assert(!enemy_should_sleep(&obj));
    obj.oVelY = 0;
    obj.oInteractStatus = 1;
    assert(!enemy_should_sleep(&obj));
    obj.oInteractStatus = 0;
    obj.oAction = obj.oPrevAction = SKEETER_ACT_WALK;
    assert(!enemy_should_sleep(&obj));
    obj.oAction = obj.oPrevAction = SKEETER_ACT_IDLE;
    obj.oForwardVel = 0;
    assert(enemy_should_sleep(&obj));
    waterLevel = 101;
    assert(!enemy_should_sleep(&obj));
    puts("Skeeter water trial: frozen skating, proximity wake, live water and transient guards");
}

static void grounded_patrol_sleeping(const BehaviorScript *behavior, s32 action) {
    struct Object obj;
    enroll_settled(&obj, behavior, action);
    gCurrLevelNum = 9;
    obj.oForwardVel = 10;
    obj.oVelY = 1.3333334f;
    assert(enemy_should_sleep(&obj));
#if ENEMY_PATROL_SLEEP_TRIAL
    if (behavior == bhvSkeeter) {
        gEnemyPatrolSleepTrial = 0;
        assert(!enemy_should_sleep(&obj));
        gEnemyPatrolSleepTrial = ENEMY_PATROL_SLEEP_TRIAL;
    }
#endif
    for (int frame = 0; frame < 60; frame++) {
        tick();
    }
    assert(nativeCalls == 0 && obj.oTimer == 31);
    player.oPosX = obj.oPosX;
    player.oPosZ = obj.oPosZ;
    tick();
    assert(nativeCalls == 1);
    player.oPosX = player.oPosZ = 0;
    obj.oInteractStatus = 1;
    assert(!enemy_should_sleep(&obj));
    obj.oInteractStatus = 0;
    obj.oMoveFlags = OBJ_MOVE_IN_AIR;
    assert(!enemy_should_sleep(&obj));
    obj.oMoveFlags = OBJ_MOVE_ON_GROUND;
    obj.oVelY = 5.01f;
    assert(!enemy_should_sleep(&obj));
    obj.oVelY = 1.3333334f;
    obj.oPosY = 1.0f;
    assert(!enemy_should_sleep(&obj));
    obj.oPosY = 0;
    obj.oMoveFlags |= OBJ_MOVE_BOUNCE;
    assert(!enemy_should_sleep(&obj));
    obj.oMoveFlags = OBJ_MOVE_ON_GROUND;
    if (behavior == bhvSmallWhomp) {
        obj.oAction = obj.oPrevAction = 7;
        assert(enemy_should_sleep(&obj));
        obj.oBehParams2ndByte = 1;
        assert(!enemy_should_sleep(&obj));
        obj.oBehParams2ndByte = 0;
        for (int state = 3; state <= 6; state++) {
            obj.oAction = obj.oPrevAction = state;
            assert(!enemy_should_sleep(&obj));
        }
    } else if (behavior == bhvSkeeter) {
        obj.oMoveFlags |= OBJ_MOVE_ENTERED_WATER;
        assert(!enemy_should_sleep(&obj));
    } else {
        obj.oSubAction = 2;
        assert(enemy_should_sleep(&obj));
        obj.oSubAction = 1;
        assert(!enemy_should_sleep(&obj));
        obj.oSubAction = 0;
        obj.oCommonAnchorAction = 1;
        assert(!enemy_should_sleep(&obj));
    }
    puts("Grounded patrols: frozen patrol, immediate wake, contact, native exclusions");
}

static void broader_patrol_states(void) {
    struct Object obj, pivot;
    enroll_settled(&obj, bhvChuckya, 0);
    gCurrLevelNum = 9;
    obj.oSubAction = 3;
    obj.oPosX += 1000;
    obj.oForwardVel = 10;
    assert(enemy_should_sleep(&obj));
    for (int frame = 0; frame < 90; frame++) {
        tick();
    }
    assert(nativeCalls == 0 && obj.oTimer == 31 && obj.oForwardVel == 10);
    player.oPosX = obj.oHomeX;
    player.oPosZ = obj.oHomeZ;
    tick();
    assert(nativeCalls == 1 && obj.oTimer == 32); // Home proximity wakes the moving patrol.
    player.oPosX = player.oPosZ = 0;
    obj.oCommonAnchorAction = 1;
    assert(!enemy_should_sleep(&obj));
    obj.oCommonAnchorAction = 0;
    obj.oSubAction = 1; // Pursuit is not return-home patrol.
    assert(!enemy_should_sleep(&obj));

    for (int action = 1; action <= 2; action++) {
        enroll_settled(&obj, bhvHeaveHo, action);
        gCurrLevelNum = 9;
        obj.oForwardVel = action == 2 ? 10 : 0;
        assert(enemy_should_sleep(&obj));
        tick();
        assert(nativeCalls == 0 && obj.oTimer == 31);
        obj.oHeaveHoThrowState = 2;
        assert(!enemy_should_sleep(&obj));
        obj.oHeaveHoThrowState = 0;
        obj.oAction = obj.oPrevAction = 3;
        assert(!enemy_should_sleep(&obj));
    }

    const int actions[] = { UKIKI_ACT_IDLE, UKIKI_ACT_RUN, UKIKI_ACT_TURN_TO_MARIO };
    for (unsigned i = 0; i < ARRAY_COUNT(actions); i++) {
        enroll_settled(&obj, bhvUkiki, actions[i]);
        gCurrLevelNum = 9;
        obj.oBehParams2ndByte = UKIKI_BP_CAGE;
        obj.oPosX += 500;
        obj.oForwardVel = 10;
        assert(enemy_should_sleep(&obj));
        tick();
        assert(nativeCalls == 0 && obj.oTimer == 31);
        obj.oBehParams2ndByte = UKIKI_BP_CAP;
        assert(!enemy_should_sleep(&obj)); // Let the cap monkey reset home natively.
        obj.oBehParams2ndByte = UKIKI_BP_CAGE;
        obj.oUkikiTextState = UKIKI_TEXT_GO_TO_CAGE;
        assert(!enemy_should_sleep(&obj));
        obj.oUkikiTextState = UKIKI_TEXT_DEFAULT;
        obj.oUkikiHasCap = 1;
        assert(!enemy_should_sleep(&obj));
        obj.oUkikiHasCap = 0;
        obj.oAction = obj.oPrevAction = UKIKI_ACT_WAIT_TO_RESPAWN;
        assert(!enemy_should_sleep(&obj));
    }

    enroll_settled(&obj, bhvChainChomp, CHAIN_CHOMP_ACT_MOVE);
    gCurrLevelNum = 9;
    obj.oPosX = obj.oHomeX = 4300;
    obj.oPosZ = obj.oHomeZ = 0;
    enroll_settled(&pivot, bhvChainChompChainPart, 0);
    gCurrLevelNum = 9;
    obj.parentObj = &pivot;
    pivot.parentObj = &obj;
    obj.oSubAction = CHAIN_CHOMP_SUB_ACT_TURN;
    obj.oMoveFlags = OBJ_MOVE_IN_AIR;
    obj.oForwardVel = 10;
    obj.oVelY = 20;
    obj.oTimer = 0; // Native hopping resets this clock while retaining the move action.
    o = &obj;
    assert(enemy_should_sleep(&obj) && enemy_should_sleep(&pivot));
    tick();
    assert(nativeCalls == 0 && obj.oTimer == 0 && obj.oVelY == 20);
    obj.oFloor = NULL;
    assert(!enemy_should_sleep(&obj));
    obj.oFloor = &floorSurface;
    obj.oSubAction = CHAIN_CHOMP_SUB_ACT_LUNGE;
    assert(!enemy_should_sleep(&obj) && !enemy_should_sleep(&pivot));
    obj.oSubAction = CHAIN_CHOMP_SUB_ACT_TURN;
    obj.oChainChompReleaseStatus = CHAIN_CHOMP_RELEASED_TRIGGER_CUTSCENE;
    assert(!enemy_should_sleep(&obj));
    obj.oChainChompReleaseStatus = CHAIN_CHOMP_NOT_RELEASED;
    obj.oPosX = 5000;
    assert(!enemy_should_sleep(&obj)); // Preserve native chain unloading.
    puts("broader patrols: moving home, walking/turning, tethered hops, wake and lifecycle exclusions");
}

static void additional_enemy_states(void) {
    struct Object obj, ball, hole, spawner;
    const BehaviorScript *waiting[] = { bhvBulletBill,
                                        bhvEnemyLakitu,
                                        bhvHauntedChair,
                                        bhvBoo,
                                        bhvClamShell,
                                        bhvTweester,
                                        bhvCloud,
                                        bhvFlamethrower,
                                        bhvWaterBombSpawner,
                                        bhvBookendSpawn,
                                        bhvBubSpawner,
                                        bhvChuckya,
                                        bhvHeaveHo,
                                        bhvMrI,
                                        bhvMrBlizzard,
                                        bhvPokey,
                                        bhvChainChomp,
                                        bhvMontyMole,
                                        bhvUkiki,
                                        bhvBubba,
                                        bhvSkeeter,
                                        bhvSushiShark,
                                        bhvUnagi,
                                        bhvKlepto,
                                        bhvBigBoulder,
                                        bhvBouncingFireball,
                                        bhvBub };
    for (u32 i = 0; i < ARRAY_COUNT(waiting); i++) {
        enroll_settled(&obj, waiting[i], 0);
        if (waiting[i] == bhvBulletBill) {
            obj.oAction = obj.oPrevAction = 1;
        }
        if (waiting[i] == bhvHauntedChair) {
            obj.oHauntedChairSpinTimer = 1;
        }
        if (waiting[i] == bhvClamShell) {
            obj.header.gfx.animInfo.animFrame = 30;
            obj.oDrawingDistance = 4000;
        }
        if (waiting[i] == bhvTweester) {
            obj.header.gfx.scale[0] = 0;
        }
        if (waiting[i] == bhvCloud) {
            obj.oBehParams2ndByte = CLOUD_BP_FWOOSH;
            obj.oAction = obj.oPrevAction = CLOUD_ACT_FWOOSH_HIDDEN;
        }
        if (waiting[i] == bhvBookendSpawn) {
            obj.oTimer = 41;
        }
        if (waiting[i] == bhvChuckya) {
            obj.oSubAction = 3;
        }
        if (waiting[i] == bhvMrBlizzard) {
            memset(&ball, 0, sizeof(ball));
            ball.activeFlags = ACTIVE_FLAG_ACTIVE;
            ball.oAction = ball.oPrevAction = MR_BLIZZARD_SNOWBALL_ACT_LAUNCH;
            ball.behavior = bhvMrBlizzardSnowball;
            ball.curBhvCommand = command;
            obj.oAction = obj.oPrevAction = MR_BLIZZARD_ACT_HIDE_UNHIDE;
            obj.oMrBlizzardGraphYOffset = -200;
            obj.prevObj = obj.oMrBlizzardHeldObj = &ball;
        }
        if (waiting[i] == bhvPokey) {
            obj.oDrawingDistance = 4000;
        }
        if (waiting[i] == bhvMontyMole) {
            memset(&hole, 0, sizeof(hole));
            hole.oPosX = 12000;
            sMontyMoleHoleList = &hole;
        }
        if (waiting[i] == bhvBubba) {
            obj.oMoveFlags = OBJ_MOVE_UNDERWATER_ON_GROUND;
        }
        if (waiting[i] == bhvUnagi) {
            obj.oAction = obj.oPrevAction = UNAGI_ACT_SHIP_PATH;
        }
        if (waiting[i] == bhvKlepto) {
            obj.oAction = obj.oPrevAction = KLEPTO_ACT_WAIT_FOR_MARIO;
        }
        if (waiting[i] == bhvBigBoulder) {
            obj.oAction = obj.oPrevAction = 1;
        }
        if (waiting[i] == bhvBub) {
            memset(&spawner, 0, sizeof(spawner));
            spawner.behavior = bhvBubSpawner;
            spawner.activeFlags = ACTIVE_FLAG_ACTIVE;
            spawner.oAction = BUB_SPAWNER_ACT_IDLE;
            obj.parentObj = &spawner;
            obj.oAction = obj.oPrevAction = BUB_ACT_SWIMMING_TOWARDS_MARIO;
        }
        if (!enemy_should_sleep(&obj)) {
            fprintf(stderr, "waiting family %u failed\n", i);
        }
        assert(enemy_should_sleep(&obj));
        // Three complete sleep/wake cycles, preserving local clocks when asleep.
        for (int cycle = 0; cycle < 3; cycle++) {
            int before = nativeCalls;
            int timer = obj.oTimer;
            for (int frame = 0; frame < 90; frame++) {
                tick();
            }
            assert(nativeCalls == before && obj.oTimer == timer);
            player.oPosX = obj.oPosX;
            player.oPosZ = obj.oPosZ;
            tick();
            assert(nativeCalls == before + 1);
            player.oPosX = player.oPosZ = 0;
        }
        obj.oInteractStatus = 1;
        assert(!enemy_should_sleep(&obj));
    }
    // Aquatic exceptions do not leak into ground enemies.
    enroll_settled(&obj, bhvGoomba, GOOMBA_ACT_WALK);
    obj.oMoveFlags |= OBJ_MOVE_UNDERWATER_ON_GROUND;
    assert(!enemy_should_sleep(&obj));
    puts("expanded families: 27 additional states preserve clocks and wake over three cycles");
}

static void group_and_cleanup_guards(void) {
    struct Object root, child;
    enroll_settled(&root, bhvEnemyLakitu, ENEMY_LAKITU_ACT_MAIN);
    root.oSubAction = ENEMY_LAKITU_SUB_ACT_NO_SPINY;
    enroll_settled(&child, bhvCloud, CLOUD_ACT_MAIN);
    child.parentObj = &root;
    assert(enemy_should_sleep(&child));
    root.oSubAction = ENEMY_LAKITU_SUB_ACT_HOLD_SPINY;
    assert(!enemy_should_sleep(&root) && !enemy_should_sleep(&child));
    root.oSubAction = ENEMY_LAKITU_SUB_ACT_NO_SPINY;
    root.activeFlags = ACTIVE_FLAG_DEACTIVATED;
    assert(!enemy_should_sleep(&child)); // Cloud must run its unload branch.

    enroll_settled(&root, bhvMrI, MR_I_BODY_ACT_FAR_AWAY);
    enroll_settled(&child, bhvMrIIris, 0);
    child.parentObj = &root;
    assert(enemy_should_sleep(&child));
    root.oMrIBlinking = 1;
    assert(!enemy_should_sleep(&child));
    root.oMrIBlinking = 0;
    inView = 1;
    assert(
        !enemy_should_sleep(&root)); // Group bounds protect visible children without a root draw flag.
    inView = 0;
    root.header.gfx.node.flags |= GRAPH_RENDER_ACTIVE;
    inView = 1;
    assert(!enemy_should_sleep(&child));

    enroll_settled(&root, bhvMrBlizzard, MR_BLIZZARD_ACT_HIDE_UNHIDE);
    enroll_settled(&child, bhvMrBlizzardSnowball, MR_BLIZZARD_SNOWBALL_ACT_LAUNCH);
    root.oMrBlizzardGraphYOffset = -200;
    root.prevObj = root.oMrBlizzardHeldObj = &child;
    child.parentObj = &root;
    assert(enemy_should_sleep(&root) && enemy_should_sleep(&child));
    root.prevObj = NULL;
    assert(!enemy_should_sleep(&root) && !enemy_should_sleep(&child));
    child.oAction = child.oPrevAction = MR_BLIZZARD_SNOWBALL_ACT_COLLISION;
    assert(!enemy_should_sleep(&child));

    enroll_settled(&root, bhvPokey, POKEY_ACT_WANDER);
    enroll_settled(&child, bhvPokeyBodyPart, 0);
    child.parentObj = &root;
    root.oPokeyNumAliveBodyParts = POKEY_NUM_SEGMENTS;
    root.oPokeyBottomBodyPartSize = 1;
    root.oDrawingDistance = 20000;
    root.oTimer = 0; // Healthy stacks reset this timer every native update.
    assert(enemy_should_sleep(&root) && enemy_should_sleep(&child));
    root.oPokeyNumAliveBodyParts--;
    assert(!enemy_should_sleep(&child));
    root.oPokeyNumAliveBodyParts++;
    root.oDrawingDistance = 10000;
    assert(!enemy_should_sleep(&root)); // Native far-unload must run first.

    enroll_settled(&root, bhvChainChomp, CHAIN_CHOMP_ACT_MOVE);
    root.oPosX = root.oHomeX = 4300;
    root.oPosZ = root.oHomeZ = 0;
    enroll_settled(&child, bhvChainChompChainPart, 0);
    root.parentObj = &child;
    child.parentObj = &root; // Native pivot ownership cycle.
    assert(enemy_should_sleep(&root) && enemy_should_sleep(&child));
    root.oChainChompReleaseStatus = CHAIN_CHOMP_RELEASED_TRIGGER_CUTSCENE;
    assert(!enemy_should_sleep(&root) && !enemy_should_sleep(&child));
    root.oChainChompReleaseStatus = CHAIN_CHOMP_NOT_RELEASED;
    root.oPosX = 5000;
    assert(!enemy_should_sleep(&root));

    const BehaviorScript *owners[] = { bhvChuckya, bhvHeaveHo, bhvBoo };
    const BehaviorScript *attachments[] = { bhvChuckyaAnchorMario, bhvHeaveHoThrowMario,
                                            bhvCoinInsideBoo };
    for (u32 i = 0; i < ARRAY_COUNT(owners); i++) {
        enroll_settled(&root, owners[i], 0);
        if (owners[i] == bhvChuckya) {
            root.oSubAction = 3;
        }
        enroll_settled(&child, attachments[i], 0);
        child.parentObj = &root;
        assert(enemy_should_sleep(&child));
        if (owners[i] == bhvChuckya) {
            root.oCommonAnchorAction = 2;
        }
        if (owners[i] == bhvHeaveHo) {
            root.oHeaveHoThrowState = 2;
        }
        if (owners[i] == bhvBoo) {
            root.oBooDeathStatus = BOO_DEATH_STATUS_DYING;
        }
        assert(!enemy_should_sleep(&child));
    }
    struct Object cloud;
    enroll_settled(&root, bhvEnemyLakitu, ENEMY_LAKITU_ACT_MAIN);
    enroll_settled(&cloud, bhvCloud, CLOUD_ACT_MAIN);
    cloud.parentObj = &root;
    enroll_settled(&child, bhvCloudPart, 0);
    child.parentObj = &cloud;
    assert(enemy_should_sleep(&child));
    root.oSubAction = ENEMY_LAKITU_SUB_ACT_THROW_SPINY;
    assert(!enemy_should_sleep(&child));

    enroll_settled(&root, bhvEnemyLakitu, ENEMY_LAKITU_ACT_MAIN);
    enroll_settled(&child, bhvSpiny, SPINY_ACT_WALK);
    child.parentObj = &root;
    child.oPosX = child.oHomeX = 2400;
    child.oPosZ = child.oHomeZ = 0;
    assert(enemy_should_sleep(&child));
    child.oPosX = 2501;
    assert(!enemy_should_sleep(&child)); // Despawn and Lakitu count decrement.

    enroll_settled(&root, bhvBubSpawner, BUB_SPAWNER_ACT_IDLE);
    enroll_settled(&child, bhvBub, BUB_ACT_SWIMMING_TOWARDS_MARIO);
    child.parentObj = &root;
    assert(enemy_should_sleep(&child));
    root.oAction = BUB_SPAWNER_ACT_REMOVE_BUBS;
    assert(!enemy_should_sleep(&child));
    puts("groups: ownership cycles, visibility, blinking, throwing, regeneration and native despawn "
         "remain live");
}

static void hazard_and_activation_guards(void) {
    struct Object obj, hole, secondHole;
    const BehaviorScript *hazards[] = { bhvSmallWhomp, bhvThwomp, bhvGrindel, bhvToxBox, bhvSpindel };
    for (u32 i = 0; i < ARRAY_COUNT(hazards); i++) {
        enroll_settled(&obj, hazards[i], 0);
        obj.collisionData = &obj;
        if (hazards[i] == bhvThwomp || hazards[i] == bhvGrindel) {
            obj.oAction = obj.oPrevAction = GRINDEL_THWOMP_ACT_ON_GROUND;
        }
        if (hazards[i] == bhvToxBox) {
            obj.oAction = obj.oPrevAction = 1;
        }
        if (hazards[i] == bhvSpindel) {
            obj.oSpindelMoveTimer = -1;
        }
        // Native rests are shorter than the old 31-update settle rule.
        obj.oTimer = 2;
        assert(enemy_should_sleep(&obj));
        tick();
        assert(collisionPublications == 1 && sleepCount == 1);
        needsCollision = 1;
        assert(!enemy_should_sleep(&obj));
        needsCollision = 0;
        obj.header.gfx.node.flags |= GRAPH_RENDER_ACTIVE;
        inView = 1;
        assert(!enemy_should_sleep(&obj));
    }
    enroll_settled(&obj, bhvThwomp, GRINDEL_THWOMP_ACT_FALLING);
    assert(!enemy_should_sleep(&obj));
    enroll_settled(&obj, bhvToxBox, 4);
    assert(!enemy_should_sleep(&obj));
    enroll_settled(&obj, bhvSpindel, 0);
    obj.oSpindelMoveTimer = -1;
    obj.oVelZ = 5;
    assert(!enemy_should_sleep(&obj));

    enroll_settled(&obj, bhvMontyMole, MONTY_MOLE_ACT_SELECT_HOLE);
    memset(&hole, 0, sizeof(hole));
    memset(&secondHole, 0, sizeof(secondHole));
    hole.oPosX = 12000;
    hole.parentObj = &secondHole;
    secondHole.oPosX = 1500;
    sMontyMoleHoleList = &hole;
    assert(!enemy_should_sleep(&obj));
    secondHole.oPosX = 1501;
    assert(enemy_should_sleep(&obj));

    enroll_settled(&obj, bhvWaterBombSpawner, 0);
    obj.oBehParams = 100 << 16;
    obj.oPosX = obj.oHomeX = 3000;
    obj.oPosZ = obj.oHomeZ = 0;
    assert(!enemy_should_sleep(&obj)); // Parameterized horizontal range, not 3D distance.
    obj.oBehParams = 0;
    obj.oWaterBombSpawnerBombActive = 1;
    assert(!enemy_should_sleep(&obj));
    enroll_settled(&obj, bhvUkiki, UKIKI_ACT_IDLE);
    obj.oUkikiTextState = UKIKI_TEXT_HAS_CAP;
    assert(!enemy_should_sleep(&obj));
    enroll_settled(&obj, bhvUnagi, UNAGI_ACT_SHIP_PATH);
    obj.oUnagiHasStar = 1;
    assert(!enemy_should_sleep(&obj)); // Hitbox subobjects need native teardown.
    enroll_settled(&obj, bhvBubba, BUBBA_ACT_IDLE);
    obj.oMoveFlags = OBJ_MOVE_UNDERWATER_ON_GROUND;
    obj.oBubbaAirVelY = 4;
    assert(!enemy_should_sleep(&obj));
    puts("hazards and activation: collision publication, support wake, all mole holes, water range, "
         "caps and recovery");
}
static void aggressive_sleep_and_cleanup(void) {
    struct Object obj, parent;
    const BehaviorScript *persistent[] = {
        bhvPhysicsLabMover,
        bhvPhysicsLabTiltingPlatform,
        bhvCheckerboardElevatorGroup,
        bhvCheckerboardPlatformSub,
        bhvGoomba,
        bhvBobomb,
        bhvThwomp,
        bhvChainChomp,
        bhvFirePiranhaPlant,
    };
    for (u32 i = 0; i < ARRAY_COUNT(persistent); i++) {
        setup(&obj, persistent[i]);
        enemy_sleep_init(&obj);
        obj.oAction = obj.oPrevAction = 123; // Active states may sleep.
        obj.oTimer = 1;
        obj.oVelY = 20;
        inView = 1;
        for (int cycle = 0; cycle < 3; cycle++) {
            tick();
            assert(nativeCalls == cycle && obj.oTimer == cycle + 1);
            player.oPosX = obj.oPosX;
            player.oPosZ = obj.oPosZ;
            tick();
            assert(nativeCalls == cycle + 1 && obj.oTimer == cycle + 2);
            player.oPosX = player.oPosZ = 0;
        }
        assert(viewCalls == 0);
    }
    setup(&obj, bhvBobombBuddy);
    enemy_sleep_init(&obj);
    obj.oHomeX = obj.oHomeY = obj.oHomeZ = 0;
    assert(enemy_should_sleep(&obj)); // Spawned supplies often have no home.
    obj.oPosX = 2500;
    obj.oPosZ = 0;
    assert(!enemy_should_sleep(&obj));
    obj.oPosX = 2501;
    assert(enemy_should_sleep(&obj));
    obj.oPosX = 0;
    obj.oPosY = 1000;
    assert(!enemy_should_sleep(&obj));
    obj.oPosY = 1001;
    assert(enemy_should_sleep(&obj));
    obj.oPosY = -1001;
    assert(enemy_should_sleep(&obj));
    obj.oPosX = obj.oPosZ = 1500;
    obj.oPosY = 999;
    assert(!enemy_should_sleep(&obj)); // Inside both limits, despite 3D distance.
    obj.oPosX = obj.oPosZ = 2500;
    assert(enemy_should_sleep(&obj)); // Horizontal distance includes X and Z.

    setup(&obj, bhvPhysicsLabMover);
    enemy_sleep_init(&obj);
    obj.collisionData = &obj;
    obj.oCollisionDistance = 2400;
    obj.oPosX = obj.oHomeX = 3899;
    obj.oPosZ = obj.oHomeZ = 0;
    assert(!enemy_should_sleep(&obj));
    obj.oPosX = obj.oHomeX = 3901;
    assert(enemy_should_sleep(&obj));
    tick();
    assert(collisionPublications == 1 && platformFinishes == 0 && updateVisited);
    obj.oPosX -= 600;
    assert(enemy_should_sleep(&obj)); // The pair uses its shared home bound.
    obj.oPosX += 1200;
    assert(enemy_should_sleep(&obj));
    player.platform = &obj;
    assert(!enemy_should_sleep(&obj));
    player.platform = NULL;
    obj.oHeldState = HELD_HELD;
    assert(!enemy_should_sleep(&obj));
    obj.oHeldState = HELD_FREE;
    obj.curBhvCommand = obj.behavior;
    assert(!enemy_should_sleep(&obj));
    obj.curBhvCommand = command;
    gTimeStopState = TIME_STOP_ACTIVE;
    assert(!enemy_should_sleep(&obj));
    gTimeStopState = 0;
    gCurrLevelNum = 1;
    assert(!enemy_should_sleep(&obj));

    for (u32 i = 0; i < ARRAY_COUNT(sLabDisposableBehaviors); i++) {
        setup(&obj, sLabDisposableBehaviors[i]);
        obj.oPosX = 4001;
        obj.oPosZ = 0;
        tick();
        assert(obj.activeFlags == ACTIVE_FLAG_DEACTIVATED && nativeCalls == 0 && updateVisited);
        update_object_with_platform_motion(); // A repeated visit cannot repeat disposal.
        assert(nativeCalls == 0);
        setup(&obj, sLabDisposableBehaviors[i]);
        obj.oPosX = 4000;
        obj.oPosZ = 0;
        assert(!physics_lab_discard_distant_effect(&obj));
    }
    setup(&obj, command);
    obj.activeFlags |= ACTIVE_FLAG_UNIMPORTANT;
    obj.curBhvCommand = bhvGoomba;
    obj.oPosX = 5000;
    obj.oPosZ = 0;
    assert(physics_lab_discard_distant_effect(&obj));

    setup(&parent, bhvEnemyLakitu);
    parent.oEnemyLakituNumSpinies = 2;
    setup(&obj, bhvSpiny);
    obj.parentObj = &parent;
    obj.oPosX = 5000;
    obj.oPosZ = 0;
    obj.oAction = SPINY_ACT_HELD_BY_LAKITU;
    assert(!physics_lab_discard_distant_effect(&obj));
    obj.oAction = SPINY_ACT_WALK;
    assert(physics_lab_discard_distant_effect(&obj));
    assert(parent.oEnemyLakituNumSpinies == 1);
    assert(!physics_lab_discard_distant_effect(&obj));
    assert(parent.oEnemyLakituNumSpinies == 1);

    setup(&parent, bhvWaterBombSpawner);
    parent.oWaterBombSpawnerBombActive = TRUE;
    setup(&obj, bhvWaterBomb);
    obj.parentObj = &parent;
    obj.oPosX = 5000;
    obj.oPosZ = 0;
    assert(physics_lab_discard_distant_effect(&obj));
    assert(!parent.oWaterBombSpawnerBombActive && obj.oAction == WATER_BOMB_ACT_EXPLODE);

    setup(&parent, bhvMrBlizzard);
    setup(&obj, bhvMrBlizzardSnowball);
    obj.parentObj = &parent;
    parent.prevObj = &obj;
    obj.oPosX = 5000;
    obj.oPosZ = 0;
    assert(!physics_lab_discard_distant_effect(&obj));
    parent.prevObj = NULL;
    assert(physics_lab_discard_distant_effect(&obj));
    puts("aggressive Lab sleep: active/visible objects, fixtures, wake cycles and effect cleanup");
}

static void sleeping_visibility(void) {
    struct Object obj;
    const BehaviorScript *behaviors[] = { bhvGoomba, bhvTweester };
    for (u32 i = 0; i < ARRAY_COUNT(behaviors); i++) {
        setup(&obj, behaviors[i]);
        enemy_sleep_init(&obj);
        obj.oTimer = 31;
        obj.oPosX = obj.oHomeX = 12000;
        obj.oPosZ = obj.oHomeZ = 0;
        obj.oDrawingDistance = 4000;
        if (i == 1) {
            obj.oFlags |= OBJ_FLAG_ACTIVE_FROM_AFAR;
            obj.oSubAction = TWEESTER_SUB_ACT_WAIT;
            obj.header.gfx.scale[0] = 0;
        }
        obj.header.gfx.node.flags |= GRAPH_RENDER_ACTIVE;
        for (int cycle = 0; cycle < 3; cycle++) {
            player.oPosX = 0;
            tick();
            assert(!(obj.header.gfx.node.flags & GRAPH_RENDER_ACTIVE));
            assert(nativeCalls == 0 && obj.oTimer == 31);
            assert(obj.oDistanceToMario == 12000);
            player.oPosX = 8500; // Within drawing range, still asleep and hidden.
            tick();
            assert(!(obj.header.gfx.node.flags & GRAPH_RENDER_ACTIVE));
            assert(!(obj.header.gfx.node.flags & GRAPH_RENDER_HAS_ANIMATION));
            assert(nativeCalls == 0 && obj.oTimer == 31 && obj.oDistanceToMario == 3500);
            if (i == 1) {
                assert(obj.oFlags & OBJ_FLAG_ACTIVE_FROM_AFAR);
                assert(!(obj.activeFlags & ACTIVE_FLAG_FAR_AWAY));
            }
            update_object_with_platform_motion(); // Revisited nodes do no extra work.
            assert(obj.oTimer == 31 && nativeCalls == 0);
        }
        obj.oRoom = 1;
        marioRoom = MARIO_OUTSIDE_ROOM;
        tick();
        assert(!(obj.header.gfx.node.flags & GRAPH_RENDER_ACTIVE));
        assert(obj.activeFlags & ACTIVE_FLAG_IN_DIFFERENT_ROOM);
        marioRoom = MARIO_INSIDE_ROOM;
        tick();
        assert(!(obj.header.gfx.node.flags & GRAPH_RENDER_ACTIVE));
        assert(!(obj.activeFlags & ACTIVE_FLAG_IN_DIFFERENT_ROOM));
        assert(nativeCalls == 0 && obj.oTimer == 31);
        for (int cycle = 0; cycle < 3; cycle++) {
            player.oPosX = obj.oPosX;
            tick();
            assert(obj.header.gfx.node.flags & GRAPH_RENDER_ACTIVE);
            assert(obj.header.gfx.node.flags & GRAPH_RENDER_HAS_ANIMATION);
            assert(nativeCalls == cycle + 1 && obj.oTimer == 32 + cycle);
            player.oPosX = 8500;
            tick();
            assert(!(obj.header.gfx.node.flags & GRAPH_RENDER_ACTIVE));
            assert(nativeCalls == cycle + 1 && obj.oTimer == 32 + cycle);
        }
    }
    enroll_settled(&obj, bhvThwomp, GRINDEL_THWOMP_ACT_ON_GROUND);
    obj.collisionData = &obj;
    collisionRevealsRendering = 1;
    tick();
    assert(collisionPublications == 1 && nativeCalls == 0);
    assert(!(obj.header.gfx.node.flags & GRAPH_RENDER_ACTIVE));
    player.oPosX = obj.oPosX;
    player.oPosZ = obj.oPosZ;
    tick();
    assert(nativeCalls == 1 && (obj.header.gfx.node.flags & GRAPH_RENDER_ACTIVE));
#if PHYSICS_LAB_AGGRESSIVE_SLEEP
    setup(&obj, bhvPhysicsLabTiltingPlatform);
    enemy_sleep_init(&obj);
    obj.collisionData = &obj;
    collisionRevealsRendering = 1;
    tick();
    assert(nativeCalls == 0 && collisionPublications == 1);
    assert(obj.header.gfx.node.flags & GRAPH_RENDER_ACTIVE);
    assert(!(obj.oFlags & OBJ_FLAG_HIDE_WHEN_SLEEPING));
#endif
    puts("sleep visibility: hidden dormant enemies, immediate wake reveal and frozen clocks");
}

static void awake_exclusions(void) {
    struct Object obj;
    const BehaviorScript *behaviors[] = {
        bhvCannon,
        bhvCannonBarrel,
        bhvExclamationBox,
        bhvBreakableBox,
        bhvKoopaShellUnderwater,
        bhvToadMessage,
        bhvHoot,
        bhvButterfly,
        bhvPhysicsLabChests,
        bhvTreasureChestBottom,
        bhvTreasureChestTop,
        bhvMontyMoleHole,
        bhvTree,
        bhvPoleGrabbing,
        bhvYellowCoin,
        bhvTemporaryYellowCoin,
        bhvMovingYellowCoin,
        bhvMovingBlueCoin,
        bhvBlueCoinSliding,
        bhvBlueCoinJumping,
        bhvMrIBlueCoin,
        bhvNormalCap,
        bhvWingCap,
        bhvMetalCap,
        bhvVanishCap,
        bhvReleasedPowerUpCap,
        bhv1upWalking,
        bhv1upRunningAway,
        bhv1upSliding,
        bhv1Up,
        bhv1upJumpOnApproach,
    };
    for (u32 i = 0; i < ARRAY_COUNT(behaviors); i++) {
        setup(&obj, behaviors[i]);
        obj.oFlags |= OBJ_FLAG_ALLOW_ENEMY_SLEEP; // Clear stale enrollment on initialization.
        enemy_sleep_init(&obj);
        assert(!(obj.oFlags & OBJ_FLAG_ALLOW_ENEMY_SLEEP));
        obj.oTimer = 2;
        for (int frame = 0; frame < 100; frame++) {
            tick();
        }
        assert(nativeCalls == 100 && obj.oTimer == 102 && sleepCount == 0);
    }
    if (PHYSICS_LAB_AGGRESSIVE_SLEEP) {
        struct Object owner;
        setup(&owner, bhvBoo);
        enemy_sleep_init(&owner);
        setup(&obj, bhvCoinInsideBoo);
        obj.parentObj = &owner;
        enemy_sleep_init(&obj);
        obj.oAction = obj.oPrevAction = COIN_INSIDE_BOO_ACT_CARRIED;
        assert(enemy_should_sleep(&obj));
        obj.oAction = obj.oPrevAction = COIN_INSIDE_BOO_ACT_DROPPED;
        assert(!enemy_should_sleep(&obj));
        tick();
        assert(nativeCalls == 1);
    }
    puts("awake exclusions: chests, mole holes, scenery and loose collectibles keep updating");
}

static void vanilla_sleeping(void) {
    struct Object obj, owner;
    const f32 positions[][3] = {
        { 2500, 0, 0 },  { 2501, 0, 0 },    { -2501, 0, 0 },     { 0, 1000, 0 }, { 0, 1001, 0 },
        { 0, -1001, 0 }, { 2000, 0, 2000 }, { 1500, 999, 1500 }, { 0, 0, 2501 },
    };
    const s32 expected[] = { FALSE, TRUE, TRUE, FALSE, TRUE, TRUE, TRUE, FALSE, TRUE };
    for (u32 i = 0; i < ARRAY_COUNT(positions); i++) {
        setup(&obj, bhvGoomba);
        gCurrLevelNum = 1;
        enemy_sleep_init(&obj);
        obj.oTimer = 31;
        obj.oPosX = obj.oHomeX = positions[i][0];
        obj.oPosY = obj.oHomeY = positions[i][1];
        obj.oPosZ = obj.oHomeZ = positions[i][2];
        assert(enemy_should_sleep(&obj) == expected[i]);
    }
    setup(&obj, bhvGoomba);
    gCurrLevelNum = 1;
    enemy_sleep_init(&obj);
    obj.oTimer = 31;
    obj.oDrawingDistance = 20000;
    inView = 1; // Distance, not camera direction or walls, governs vanilla sleeping.
    obj.header.gfx.node.flags |= GRAPH_RENDER_ACTIVE;
    for (int cycle = 0; cycle < 3; cycle++) {
        tick();
        assert(sleepCount == cycle + 1 && nativeCalls == cycle);
        assert(!(obj.header.gfx.node.flags & GRAPH_RENDER_ACTIVE));
        assert(obj.oTimer == 31 + cycle);
        player.oPosX = obj.oPosX;
        player.oPosZ = obj.oPosZ;
        tick();
        assert(nativeCalls == cycle + 1 && obj.oTimer == 32 + cycle);
        assert(obj.header.gfx.node.flags & GRAPH_RENDER_ACTIVE);
        player.oPosX = player.oPosZ = 0;
    }
    assert(viewCalls == 0);
    obj.oHomeX = obj.oHomeZ = 0;
    assert(!enemy_should_sleep(&obj));
    obj.oHomeX = obj.oPosX;
    obj.oHomeZ = obj.oPosZ;
    obj.oInteractStatus = 1;
    assert(!enemy_should_sleep(&obj));
    obj.oInteractStatus = INT_STATUS_NONE;
    obj.oHeldState = HELD_HELD;
    assert(!enemy_should_sleep(&obj));
    obj.oHeldState = HELD_FREE;
    gTimeStopState = TIME_STOP_ACTIVE;
    assert(!enemy_should_sleep(&obj));
    gTimeStopState = 0;
    obj.platform = &player;
    assert(!enemy_should_sleep(&obj));
    obj.platform = NULL;
    obj.oAction = obj.oPrevAction = GOOMBA_ACT_JUMP;
    assert(!enemy_should_sleep(&obj));
    obj.oAction = obj.oPrevAction = GOOMBA_ACT_WALK;
    memset(&owner, 0, sizeof(owner));
    owner.behavior = bhvGoombaTripletSpawner;
    owner.activeFlags = ACTIVE_FLAG_ACTIVE;
    owner.oAction = GOOMBA_TRIPLET_SPAWNER_ACT_LOADED;
    obj.parentObj = &owner;
    assert(enemy_should_sleep(&obj));
    owner.oAction = GOOMBA_TRIPLET_SPAWNER_ACT_UNLOADED;
    assert(!enemy_should_sleep(&obj));
    tick();
    assert(nativeCalls == 4); // Native spawner-deletion handling gets its update.

    setup(&obj, bhvBobomb);
    gCurrLevelNum = 1;
    enemy_sleep_init(&obj);
    obj.oTimer = 31;
    obj.oAction = obj.oPrevAction = BOBOMB_ACT_PATROL;
    assert(enemy_should_sleep(&obj));
    obj.oBobombFuseLit = 1;
    assert(!enemy_should_sleep(&obj));
    setup(&obj, bhvKoopa);
    gCurrLevelNum = 1;
    enemy_sleep_init(&obj);
    obj.oTimer = 31;
    obj.oKoopaMovementType = KOOPA_BP_NORMAL;
    assert(enemy_should_sleep(&obj));
    obj.oKoopaMovementType = KOOPA_BP_KOOPA_THE_QUICK_BOB;
    assert(!enemy_should_sleep(&obj));
    setup(&obj, bhvPhysicsLabTiltingPlatform);
    gCurrLevelNum = 1;
    enemy_sleep_init(&obj);
    assert(!(obj.oFlags & OBJ_FLAG_ALLOW_ENEMY_SLEEP));
    setup(&obj, bhvYellowCoin);
    gCurrLevelNum = 1;
    enemy_sleep_init(&obj);
    assert(!(obj.oFlags & OBJ_FLAG_ALLOW_ENEMY_SLEEP));
    puts(
        "vanilla sleep: cylindrical boundaries, wake/reveal, spawner cleanup and live gameplay guards");
}

static void idle_entity_sleeping(void) {
    struct Object obj, parent;
    const BehaviorScript *behaviors[] = {
        bhvBreakableBoxSmall, bhvPushableMetalBox,       bhvKoopaShell,
        bhvBobombBuddy,       bhvBobombBuddyOpensCannon, bhvFish,
    };
    for (int level = 0; level < 2; level++) {
        for (u32 i = 0; i < ARRAY_COUNT(behaviors); i++) {
            setup(&parent, bhvFishSpawner);
            parent.oAction = parent.oPrevAction = behaviors[i] == bhvFish ? FISH_SPAWNER_ACT_IDLE : 0;
            enemy_sleep_init(&parent);
            setup(&obj, behaviors[i]);
            gCurrLevelNum = level ? LEVEL_PHYSICS_LAB : 1;
            if (behaviors[i] == bhvFish) {
                obj.parentObj = &parent;
            }
            obj.oAction = obj.oPrevAction = behaviors[i] == bhvFish ? FISH_ACT_ROAM : 0;
            obj.oFlags |= native_afar_script(behaviors[i]) ? OBJ_FLAG_ACTIVE_FROM_AFAR : 0;
            enemy_sleep_init(&obj);
            assert(obj.oFlags & OBJ_FLAG_ALLOW_ENEMY_SLEEP);
            for (int cycle = 0; cycle < 3; cycle++) {
                tick();
                assert(nativeCalls == cycle && obj.oTimer == cycle + 2);
                player.oPosX = obj.oPosX;
                player.oPosZ = obj.oPosZ;
                tick();
                assert(nativeCalls == cycle + 1 && obj.oTimer == cycle + 3);
                player.oPosX = player.oPosZ = 0;
            }
            obj.oHeldState = HELD_HELD;
            assert(!enemy_should_sleep(&obj));
            obj.oHeldState = HELD_FREE;
            obj.oInteractStatus = 1;
            assert(!enemy_should_sleep(&obj));
            obj.oInteractStatus = 0;
            needsCollision = 1;
            assert(!enemy_should_sleep(&obj));
            needsCollision = 0;
            obj.curBhvCommand = obj.behavior;
            assert(!enemy_should_sleep(&obj));
        }
    }
    const BehaviorScript *excluded[] = {
        bhvCannon,      bhvCannonBarrel, bhvExclamationBox, bhvBreakableBox, bhvKoopaShellUnderwater,
        bhvToadMessage, bhvHoot,         bhvButterfly,
    };
    for (int level = 0; level < 2; level++) {
        for (u32 i = 0; i < ARRAY_COUNT(excluded); i++) {
            setup(&obj, excluded[i]);
            gCurrLevelNum = level ? LEVEL_PHYSICS_LAB : 1;
            enemy_sleep_init(&obj);
            assert(!(obj.oFlags & OBJ_FLAG_ALLOW_ENEMY_SLEEP));
            tick();
            tick();
            assert(nativeCalls == 2 && sleepCount == 0 && obj.oTimer == 4);
        }
    }
    setup(&obj, bhvExclamationBox);
    enemy_sleep_init(&obj);
    obj.oAction = obj.oPrevAction = EXCLAMATION_BOX_ACT_WAIT_FOR_RESPAWN;
    native = bhv_exclamation_box_loop;
    for (int frame = 0; frame < 301; frame++) {
        tick();
    }
    assert(nativeCalls == 301 && obj.oAction == EXCLAMATION_BOX_ACT_ACTIVE);

    setup(&obj, bhvBreakableBoxSmall);
    enemy_sleep_init(&obj);
    obj.oBreakableBoxSmallReleased = 1;
    assert(!enemy_should_sleep(&obj));
    obj.oBreakableBoxSmallReleased = 0;
    obj.oForwardVel = 1;
    assert(!enemy_should_sleep(&obj));
    obj.oForwardVel = 0;
    obj.oVelY = -1;
    assert(!enemy_should_sleep(&obj));

    setup(&parent, bhvExclamationBox);
    setup(&obj, bhvKoopaShell);
    obj.parentObj = &parent;
    enemy_sleep_init(&obj);
    assert(!enemy_should_sleep(&obj)); // Native expiry cannot freeze.
    obj.parentObj = &obj;
    obj.oAction = obj.oPrevAction = KOOPA_SHELL_ACT_MARIO_RIDING;
    assert(!enemy_should_sleep(&obj));

    setup(&obj, bhvBobombBuddyOpensCannon);
    enemy_sleep_init(&obj);
    obj.oAction = obj.oPrevAction = BOBOMB_BUDDY_ACT_TALK;
    assert(!enemy_should_sleep(&obj));
    setup(&parent, bhvFishSpawner);
    parent.oAction = parent.oPrevAction = FISH_SPAWNER_ACT_IDLE;
    setup(&obj, bhvFish);
    obj.oAction = obj.oPrevAction = FISH_ACT_ROAM;
    obj.parentObj = &parent;
    enemy_sleep_init(&obj);
    const BehaviorScript *spawners[] = {
        bhvFishSpawner,
        bhvManyBlueFishSpawner,
        bhvFewBlueFishSpawner,
    };
    for (u32 i = 0; i < ARRAY_COUNT(spawners); i++) {
        parent.behavior = spawners[i];
        assert(enemy_should_sleep(&obj));
    }
    gMarioObject = NULL;
    assert(!enemy_should_sleep(&obj));
    gMarioObject = &player;
    parent.oAction = FISH_SPAWNER_ACT_RESPAWN;
    assert(!enemy_should_sleep(&obj));
    parent.oAction = parent.oPrevAction = FISH_SPAWNER_ACT_SPAWN;
    assert(!enemy_should_sleep(&obj));
    parent.oAction = parent.oPrevAction = FISH_SPAWNER_ACT_IDLE;
    parent.activeFlags = ACTIVE_FLAG_DEACTIVATED;
    assert(!enemy_should_sleep(&obj));
    parent.activeFlags = ACTIVE_FLAG_ACTIVE;
    waterLevel = FLOOR_LOWER_LIMIT;
    assert(!enemy_should_sleep(&obj));
    setup(&obj, bhvFishSpawner);
    enemy_sleep_init(&obj);
    assert(!(obj.oFlags & OBJ_FLAG_ALLOW_ENEMY_SLEEP));

    setup(&obj, bhvPushableMetalBox);
    enemy_sleep_init(&obj);
    obj.collisionData = &obj;
    obj.oCollisionDistance = 4000;
    obj.oPosX = 4600;
    obj.oPosZ = 0;
    assert(!enemy_should_sleep(&obj));
    obj.oPosX = 4601;
    tick();
    assert(collisionPublications == 1 && nativeCalls == 0);
    puts(
        "idle entities: sleep/wake cycles, live respawn/expiry/dialogue, fish ownership and collision");
}

static void clam_drawing_range(void) {
    struct Object obj;
    setup(&obj, bhvClamShell);
    enemy_sleep_init(&obj);
    obj.oDrawingDistance = 4000;
    obj.oAction = CLAM_ACT_CLOSING;
    obj.oTimer = 100;
    obj.header.gfx.animInfo.animFrame = CLAM_CLOSING_DAMAGE_FRAME;
    gCurrLevelNum = 1; // The fixture distinguishes ordinary courses from the Physics Lab.
    for (int cycle = 0; cycle < 3; cycle++) {
        obj.oPosX = obj.oHomeX = 3900;
        obj.oPosZ = obj.oHomeZ = 0;
        assert(!enemy_should_sleep(&obj));
        obj.oAction = CLAM_ACT_OPENING;
        assert(!enemy_should_sleep(&obj));
        obj.oAction = CLAM_ACT_CLOSING;
        obj.oPosX = obj.oHomeX = 4100;
        assert(enemy_should_sleep(&obj));
    }
    puts("clams: opening and closed poses stay awake throughout the drawing range");
}

static void lab_enemy_visibility(void) {
    if (!PHYSICS_LAB_AGGRESSIVE_SLEEP) {
        return;
    }
    const BehaviorScript *families[] = { bhvToxBox,     bhvPokey,      bhvGoomba, bhvBobomb,
                                         bhvSmallBully, bhvSmallWhomp, bhvThwomp, bhvGrindel,
                                         bhvSpindel,    bhvKoopa,      bhvSwoop,  bhvFlyGuy,
                                         bhvSnufit };
    for (u32 i = 0; i < ARRAY_COUNT(families); i++) {
        struct Object obj;
        enroll_settled(&obj, families[i], 1);
        obj.oPosX = obj.oHomeX = 3000;
        obj.oPosZ = obj.oHomeZ = 0;
        obj.oDrawingDistance = 8000;
        inView = 1;
        tick();
        assert(!(obj.header.gfx.node.flags & GRAPH_RENDER_ACTIVE));
        obj.numCollidedObjs = 1;
        tick();
        assert(nativeCalls == 1); // Collision still wakes behavior.
        assert(!(obj.header.gfx.node.flags & GRAPH_RENDER_ACTIVE));
        obj.numCollidedObjs = 0;
        tick();
        assert(nativeCalls == 1);
        obj.oPosX = 2500;
        tick();
        assert(obj.header.gfx.node.flags & GRAPH_RENDER_ACTIVE);
        for (int cycle = 0; cycle < 3; cycle++) {
            obj.oPosX = 2501;
            tick();
            assert(obj.header.gfx.node.flags & GRAPH_RENDER_ACTIVE);
            obj.oPosX = 2499;
            tick();
            assert(obj.header.gfx.node.flags & GRAPH_RENDER_ACTIVE);
        }
        obj.oPosX = 2800;
        tick();
        assert(obj.header.gfx.node.flags & GRAPH_RENDER_ACTIVE);
        obj.oPosX = 2801;
        tick();
        assert(!(obj.header.gfx.node.flags & GRAPH_RENDER_ACTIVE));
        obj.oPosX = 2700;
        tick();
        assert(!(obj.header.gfx.node.flags & GRAPH_RENDER_ACTIVE));
        obj.oPosX = 0;
        obj.oPosY = 1000;
        tick();
        assert(obj.header.gfx.node.flags & GRAPH_RENDER_ACTIVE);
        obj.oPosY = 1150;
        tick();
        assert(obj.header.gfx.node.flags & GRAPH_RENDER_ACTIVE);
        obj.oPosY = 1151;
        tick();
        assert(!(obj.header.gfx.node.flags & GRAPH_RENDER_ACTIVE));
        obj.oPosY = 1100;
        tick();
        assert(!(obj.header.gfx.node.flags & GRAPH_RENDER_ACTIVE));
        obj.oPosY = 1000;
        tick();
        assert(obj.header.gfx.node.flags & GRAPH_RENDER_ACTIVE);
        enemy_sleep_init(&obj);
        assert(!(obj.oFlags & OBJ_FLAG_LAB_ENEMY_VISIBLE));
    }
    struct Object root, part;
    enroll_settled(&root, bhvPokey, POKEY_ACT_WANDER);
    root.oPosX = root.oPosZ = 0;
    root.oPosY = -1010;
    enroll_settled(&part, bhvPokeyBodyPart, 0);
    part.parentObj = &root;
    part.oPosX = part.oPosZ = 0;
    part.oDrawingDistance = 4000;
    const int heights[] = { 480, 0 };
    for (u32 i = 0; i < ARRAY_COUNT(heights); i++) {
        part.oPosY = root.oPosY + heights[i];
        tick();
        assert(!(part.header.gfx.node.flags & GRAPH_RENDER_ACTIVE));
    }
    root.oPosY = -1000;
    for (u32 i = 0; i < ARRAY_COUNT(heights); i++) {
        part.oPosY = root.oPosY + heights[i];
        tick();
        assert(part.header.gfx.node.flags & GRAPH_RENDER_ACTIVE);
    }
    root.oPosY = -1100;
    assert(!physics_lab_update_enemy_hidden_state(&part));
    root.oPosY = -1151;
    part.numCollidedObjs = 1;
    tick();
    assert(!(part.header.gfx.node.flags & GRAPH_RENDER_ACTIVE));
    root.activeFlags = ACTIVE_FLAG_DEACTIVATED;
    assert(!physics_lab_update_enemy_hidden_state(&part)); // Native owner cleanup can run.
    gCurrLevelNum = 1;
    assert(!physics_lab_update_enemy_hidden_state(&part));
    puts("Lab visibility: stable entry/exit, live contacts, shared Pokey owner and reset history");
}

static void chain_chomp_approach_visibility(void) {
    const s32 levels[] = { 9, LEVEL_PHYSICS_LAB };
    for (u32 level = 0; level < ARRAY_COUNT(levels); level++) {
        struct Object obj, pivot;
        enroll_settled(&obj, bhvChainChomp, CHAIN_CHOMP_ACT_MOVE);
        enroll_settled(&pivot, bhvChainChompChainPart, 0);
        gCurrLevelNum = levels[level];
        obj.parentObj = &pivot;
        pivot.parentObj = &obj;
        obj.oSubAction = CHAIN_CHOMP_SUB_ACT_TURN;
        obj.oPosX = obj.oHomeX = 4300;
        obj.oPosZ = obj.oHomeZ = 0;
        obj.oDrawingDistance = 8000;
        o = &obj;
        tick();
        assert(nativeCalls == 0);
        obj.oPosX = 3500;
        tick();
        assert(obj.header.gfx.node.flags & GRAPH_RENDER_ACTIVE);
        int calls = nativeCalls;
        obj.oPosX = 4580; // Full tether excursion after appearing.
        tick();
        assert(nativeCalls == calls + 1);
        assert(obj.header.gfx.node.flags & GRAPH_RENDER_ACTIVE);
        assert(!physics_lab_update_enemy_hidden_state(&pivot));
        assert(!enemy_should_sleep(&pivot));
        obj.oPosX = 4701;
        tick();
        assert(!(obj.header.gfx.node.flags & GRAPH_RENDER_ACTIVE));
        assert(physics_lab_update_enemy_hidden_state(&pivot));
        calls = nativeCalls;
        obj.oPosX = 3600;
        tick();
        assert(nativeCalls == calls);
        assert(!(obj.header.gfx.node.flags & GRAPH_RENDER_ACTIVE));
        obj.oPosX = 3500;
        tick();
        assert(obj.header.gfx.node.flags & GRAPH_RENDER_ACTIVE);
        obj.oPosX = 5000; // Native unloading must still run.
        assert(!enemy_should_sleep(&obj));
        obj.oChainChompReleaseStatus = CHAIN_CHOMP_RELEASED_TRIGGER_CUTSCENE;
        assert(!physics_lab_update_enemy_hidden_state(&obj));
        assert(!physics_lab_update_enemy_hidden_state(&pivot));
    }
}

static void bobomb_approach_visibility(void) {
    const s32 levels[] = { 9, 12, LEVEL_PHYSICS_LAB };
    const f32 distances[] = { 2499, 2501, 2499, 2501, 2001, 1999, 2500, 2800 };
    for (u32 level = 0; level < ARRAY_COUNT(levels); level++) {
        struct Object obj;
        setup(&obj, bhvBobomb);
        gCurrLevelNum = levels[level];
        enemy_sleep_init(&obj);
        obj.oTimer = 31;
        obj.oAction = obj.oPrevAction = BOBOMB_ACT_PATROL;
        obj.oMoveFlags = 0; // Native object_step supports Bob-ombs without this flag.
        obj.oDrawingDistance = 4000;
        obj.oPosX = obj.oHomeX = 3500;
        obj.oPosZ = obj.oHomeZ = 0;
        inView = 0;
        tick();
        assert(!(obj.header.gfx.node.flags & GRAPH_RENDER_ACTIVE));
        assert(nativeCalls == 0); // Distant idle sleeping remains enabled.
        for (u32 frame = 0; frame < ARRAY_COUNT(distances); frame++) {
            obj.oPosX = obj.oHomeX = distances[frame];
            tick();
            assert(obj.header.gfx.node.flags & GRAPH_RENDER_ACTIVE);
            assert(obj.header.gfx.node.flags & GRAPH_RENDER_HAS_ANIMATION);
            assert(nativeCalls == (int) frame + 1);
        }
        int calls = nativeCalls;
        obj.oPosX = obj.oHomeX = 2801;
        tick();
        assert(!(obj.header.gfx.node.flags & GRAPH_RENDER_ACTIVE));
        assert(nativeCalls == calls);
        obj.oPosX = obj.oHomeX = 2700;
        tick();
        assert(!(obj.header.gfx.node.flags & GRAPH_RENDER_ACTIVE));
        assert(nativeCalls == calls); // A hidden Bob-omb waits for the entry range.
        obj.numCollidedObjs = 1;
        tick();
        assert(nativeCalls == ++calls); // Required contact processing still runs.
        assert(!(obj.header.gfx.node.flags & GRAPH_RENDER_ACTIVE));
        obj.numCollidedObjs = 0;
        obj.oPosX = obj.oHomeX = 2499;
        tick();
        assert(obj.header.gfx.node.flags & GRAPH_RENDER_ACTIVE);
        obj.oPosY = 1100;
        tick();
        assert(obj.header.gfx.node.flags & GRAPH_RENDER_ACTIVE);
        obj.oPosY = 1151;
        obj.oHomeY = 1151;
        tick();
        assert(!(obj.header.gfx.node.flags & GRAPH_RENDER_ACTIVE));
        obj.oPosY = obj.oHomeY = 0;
        tick();
        assert(obj.header.gfx.node.flags & GRAPH_RENDER_ACTIVE);
        obj.oRoom = 1;
        marioRoom = MARIO_OUTSIDE_ROOM;
        tick();
        assert(!(obj.header.gfx.node.flags & GRAPH_RENDER_ACTIVE));
        marioRoom = MARIO_INSIDE_ROOM;
        tick();
        assert(obj.header.gfx.node.flags & GRAPH_RENDER_ACTIVE);
        obj.oRoom = -1;
        obj.oPosX = obj.oHomeX = 3500;
        obj.oBobombFuseLit = 1;
        obj.oAction = obj.oPrevAction = BOBOMB_ACT_CHASE_MARIO;
        tick();
        if (PHYSICS_LAB_AGGRESSIVE_SLEEP && gCurrLevelNum == LEVEL_PHYSICS_LAB) {
            assert(!(obj.header.gfx.node.flags & GRAPH_RENDER_ACTIVE));
        } else {
            assert(obj.header.gfx.node.flags & GRAPH_RENDER_ACTIVE);
        }
        assert(!(obj.oFlags & OBJ_FLAG_LAB_ENEMY_VISIBLE));
        obj.oPosX = obj.oHomeX = 4001;
        tick();
        assert(!(obj.header.gfx.node.flags & GRAPH_RENDER_ACTIVE));
    }
    puts("Bob-ombs: stable approach boundary across levels, retained sleeping and room limits");
}

int main(void) {
    chain_chomp_approach_visibility();
    bobomb_approach_visibility();
    lab_enemy_visibility();
    clam_drawing_range();
    grounded_patrol_sleeping(bhvSmallWhomp, 1);
    grounded_patrol_sleeping(bhvChuckya, 0);
    if (ENEMY_PATROL_SLEEP_TRIAL == 2) {
        grounded_patrol_sleeping(bhvSkeeter, SKEETER_ACT_WALK);
    }
    skeeter_water_sleeping();
    broader_patrol_states();
    idle_entity_sleeping();
    vanilla_sleeping();
    awake_exclusions();
    sleeping_visibility();
    if (PHYSICS_LAB_AGGRESSIVE_SLEEP) {
        aggressive_sleep_and_cleanup();
        angles_and_effects();
        boxes();
        return 0;
    }
    sleep_initialization();
    sleep_updates();
    sleep_guards();
    expanded_sleep_guards();
    additional_enemy_states();
    group_and_cleanup_guards();
    hazard_and_activation_guards();
    angles_and_effects();
    boxes();
    return 0;
}
