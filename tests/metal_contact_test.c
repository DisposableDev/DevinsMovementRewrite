/* Production Metal launch/contact, stun wrapper and native defeat routes.
 * Geometry, spawning, sounds and script timer bookkeeping are supplied services.
 * This checks rewards/ownership and death actions, not full AI or rendering. */
#include <stdint.h>
#include <math.h>
#include "test_support.h"
typedef int32_t s32;
typedef uint32_t u32;
typedef uint16_t u16;
typedef int16_t s16;
typedef int8_t s8;
typedef uint8_t u8;
typedef uintptr_t BehaviorScript;
typedef float f32;
typedef float Vec3f[3];
#define TRUE 1
#define FALSE 0
#define UNUSED
#define FLOOMBAS
#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define ABS(a) ((a) < 0 ? -(a) : (a))
#define SOUND_MARIO_WHOA 29
#define SOUND_MARIO_HRMM 30
#define SOUND_HE_THICC_BOUNCE 100
#define CLAMP(v, a, b) MIN(MAX(v, a), b)
#define sqr(a) ((a) * (a))
#define GET_BPARAM3(p) (((p) >> 8) & 255)
#define SET_FULL_BPARAM3(p, v) ((p) = ((p) & ~0xff00) | ((v) << 8))
#define SOUND_ACTION_THROW 19
#define SOUND_ACTION_SPIN 20
#define SOUND_MARIO_GROUND_POUND_WAH 21
#define SOUND_MARIO_OOOF2 22
#define SOUND_MARIO_OOOF 27
#define SOUND_MARIO_EEUH 28
#define SOUND_OBJ_BULLY_METAL 29
#define DEGREES(d) ((d) * 0x8000 / 180)
#define SOUND_ACTION_TERRAIN_HEAVY_LANDING 23
#define SOUND_MARIO_ON_FIRE 24
#define SOUND_GENERAL_FLAME_OUT 30
#define SOUND_ACTION_TERRAIN_BODY_HIT_GROUND 25
#define SOUND_MOVING_LAVA_BURN 26
#define SOUND_MARIO_WAAAOOOW 14
#define SOUND_MOVING_SHOCKED 31
#define SOUND_ACTION_TERRAIN_JUMP 15
#define SOUND_MARIO_HOOHOO 16
#define SOUND_ACTION_WATER_JUMP 17
#define SOUND_ACTION_TERRAIN_LANDING 18
#define SOUND_MARIO_ATTACKED 13
#define SOUND_ACTION_SNUFFIT_BULLET_HIT_METAL 102
#define MARIO_SPAWN_FALL_PROTECTION_NONE 0
#define SOUND_OBJ_DEFAULT_DEATH 1
#define SOUND_OBJ_STOMPED 2
#define SOUND_OBJ_DIVING_INTO_WATER 3
#define SOUND_OBJ_DIVING_IN_WATER 4
#define SOUND_OBJ_KOOPA_DAMAGE 5
#define SOUND_OBJ_KOOPA_FLYGUY_DEATH 6
#define SOUND_OBJ_SNUFIT_SKEETER_DEATH 7
#define SOUND_OBJ_DYING_ENEMY1 8
#define SOUND_OBJ_KLEPTO_ATTACKED 9
#define SOUND_OBJ_CHUCKYA_DEATH 10
#define SOUND_GENERAL_SPLATTERING 11
#define SOUND_OBJ_HEAVEHO_TOSSED 12
#define SAVE_FLAG_CAP_ON_KLEPTO 1
#define MODEL_BLUE_COIN 1
#define MODEL_KOOPA_WITHOUT_SHELL 2
#define MODEL_KOOPA_SHELL 3
#define MODEL_MARIOS_CAP 4
#define MODEL_1UP 5
#define MODEL_EXPLOSION 6
#define MODEL_BLACK_BOBOMB 7
#define MODEL_BOWSER_FLAMES 9
#define OBJ_LIST_GENACTOR 0
#define SOUND_GENERAL_BOWSER_BOMB_EXPLOSION 100
#define SOUND_GENERAL2_BOBOMB_EXPLOSION 101
#define SHAKE_POS_LARGE 1
#define SHAKE_ENV_EXPLOSION 1
#define MODEL_GROUND_TWIRL_STUN_STAR 8
#include "metal_definitions.inc"
static int sPalette;
static u8 mario_character_palette(void) {
    return sPalette;
}
struct Animation {
    s16 loopEnd;
};
struct AnimInfo {
    struct Animation *curAnim;
    s32 animID, animFrame;
};
struct MarioBodyState {
    s16 eyeState;
};
struct Object {
    u16 bowserThrowAirFrames;
    s16 numCollidedObjs;
    struct Object *collidedObjs[4];
    const BehaviorScript *behavior;
    struct Object *parentObj, *prevObj, *oMontyMoleCurrentHole;
    struct {
        struct Object *next;
        struct {
            struct {
                u32 flags;
            } node;
            struct AnimInfo animInfo;
            f32 scale[3], pos[3], cameraToObject[3];
            s16 angle[3];
        } gfx;
    } header;
    u32 activeFlags, oFlags, oInteractStatus, oInteractType, oInteractionSubtype, oMoveFlags;
    u32 oBehParams, oBehParams2ndByte, metalContactCooldown, metalContactPendingDefeat;
    s32 oAction, oPrevAction, oTimer, oSubAction, oHealth, oNumLootCoins, oDeathSound,
        oDamageOrCoinValue, oBullySubtype;
    s32 oBobombFuseTimer, oBobombFuseLit, oMarioTornadoYawVel, oMarioBurnTimer;
    f32 oMarioTornadoPosY, oMarioWhirlpoolPosY;
    s32 oGoombaSize, oKoopaMovementType, oGroundTwirlStunTimer, oGroundTwirlContactRebound,
        oGroundTwirlStunDuration;
    u32 oGroundTwirlLastHitId;
    s32 oMoveAngleYaw, oFaceAngleYaw, oIntangibleTimer, oHeldState, oAnimState, oOpacity;
    s32 oCommonAnchorAction, oScuttlebugHasNoLootCoins, oScuttlebugSpawnerIsDeactivated;
    s32 oEnemyLakituNumSpinies, oMontyMoleHoleCooldown;
    f32 oPosVec[3], oForwardVel, oVelX, oVelY, oVelZ, oGravity, oBuoyancy, oWallHitboxRadius,
        oGraphYOffset;
    f32 hitboxRadius, hitboxHeight, hurtboxRadius, hurtboxHeight, hitboxDownOffset;
    f32 oParentRelativePosX, oParentRelativePosY, oParentRelativePosZ;
    f32 oFloorHeight, oDistanceToMario, oBowserShockWaveScale;
    s32 oFloorType, oBowserTimer, oBowserStatus;
    void *oFloor;
};
#define oHeaveHoThrowState oCommonAnchorAction
#define oPosX oPosVec[0]
#define oPosY oPosVec[1]
#define oPosZ oPosVec[2]
struct Surface {
    s16 type, force;
    u8 flags;
    struct {
        f32 x, y, z;
    } normal;
    struct Object *object;
};
struct Area {
    s16 terrainType;
};
struct Controller {
    u16 buttonDown;
};
static struct Controller sController;
struct MarioState {
    u32 forcedCapFlags, naturalCapFlags;
    u32 action, actionArg, flags, input, particleFlags, capPickupLockStartFrame;
    u32 collidedObjInteractTypes;
    u8 capPickupLockFrames, airTwirlFlightRollFrame;
    u16 actionState, actionTimer, capTimer, soakedTimer;
    u8 soakedThisUpdate, soakedWaterContact, soakedDripFrames, soakedFlameProtection;
    s32 waterLevel;
    f32 floorHeight, peakHeight, quicksandDepth, windGravity, slideVelX, slideVelZ, ceilHeight;
    f32 intendedMag;
    s16 slideYaw, intendedYaw, wallYaw;
    struct MarioBodyState *marioBodyState;
    u8 metalFallGroundPoundImpact, waterGroundPoundImpact;
    u8 slideKickStartupFrames;
    struct Object *metalFallGroundPoundFloorObject;
    u8 windCatchActive, fastMetalFallSquish, bullyKnockbackActive, bullyKnockbackImmunityFrames;
    u8 heaveHoThrowPendingLanding, heaveHoThrowRecoveryPending, heaveHoLaunchImmunityFrames;
    u16 bullyKnockbackBuffer;
    struct Surface *floor;
    struct Area *area;
    struct Controller *controller;
    u8 hurtCounter;
    s16 health, invincTimer, squishTimer, spawnFallProtection;
    f32 pos[3], prevPos[3], vel[3], forwardVel, bowserImpactPos[3];
    s16 faceAngle[3];
    struct Object *interactObj, *usedObj, *marioObj, *heldObj, *riddenObj;
};
struct ObjectNode {
    struct Object *next;
};
static struct ObjectNode gObjectLists[1];
static struct Object *gCurrentObject, *gMarioObject;
#define o gCurrentObject
static struct MarioState mario, *gMarioState = &mario;
static s32 gCurrLevelNum, gCurrAreaIndex;
static s32 liquidBursts, liquidMaterial;
static f32 liquidHeight;
#define gMarioStates (&mario)
static struct {
    u32 id;
} sGroundTwirl;
#define BEHAVIOR(name) static const BehaviorScript name[] = { __LINE__ }
BEHAVIOR(bhvMrBlizzard);
BEHAVIOR(bhvGoomba);
BEHAVIOR(bhvFloomba);
BEHAVIOR(bhvKoopa);
BEHAVIOR(bhvBobomb);
BEHAVIOR(bhvWigglerHead);
BEHAVIOR(bhvWigglerBody);
BEHAVIOR(bhvPokeyBodyPart);
BEHAVIOR(bhvChuckya);
BEHAVIOR(bhvHeaveHo);
BEHAVIOR(bhvSpiny);
BEHAVIOR(bhvScuttlebug);
BEHAVIOR(bhvSkeeter);
BEHAVIOR(bhvFlyGuy);
BEHAVIOR(bhvSpindrift);
BEHAVIOR(bhvMontyMole);
BEHAVIOR(bhvMoneybag);
BEHAVIOR(bhvSwoop);
BEHAVIOR(bhvBoo);
BEHAVIOR(bhvGhostHuntBoo);
BEHAVIOR(bhvMerryGoRoundBoo);
BEHAVIOR(bhvBooWithCage);
BEHAVIOR(bhvKlepto);
BEHAVIOR(bhvSnufit);
BEHAVIOR(bhvEnemyLakitu);
BEHAVIOR(bhvGroundTwirlStunStar);
BEHAVIOR(bhvKoopaShell);
BEHAVIOR(bhvKoopaShellUnderwater);
BEHAVIOR(bhvMrIBlueCoin);
BEHAVIOR(bhvNormalCap);
BEHAVIOR(bhv1upWalking);
BEHAVIOR(bhvBowserBodyAnchor);
BEHAVIOR(bhvBowserBomb);
BEHAVIOR(bhvBowserBombExplosion);
BEHAVIOR(bhvExplosion);
BEHAVIOR(bhvFallingPillarHitbox);
BEHAVIOR(bhvMadPiano);
BEHAVIOR(bhvWaterBomb);
#define WATER_BOMB_ACT_DROP 2
static s32 sMontyMoleKillStreak;
static Vec3f sMontyMoleLastKilledPos;
static int coins, blueCoins, shells, caps, stars, oneups, respawners, explosions, mist, saveClears,
    aiUpdates;
static struct Object spawned[32];
static int spawnedCount;
static u32 nextContacts;
static s16 bobombStepContacts, sInvulnerable, sDelayInvincTimer;
static int knockbacks, damageSounds, hitShakes, contactPushes, bullyImpulses, bullyAttacks,
    bullyBounces, grabFacing, lethalCapLosses, capMusicStops;
static u32 contactInteraction;
static struct {
    s16 reboundHiddenTimer;
    s32 reboundActive, reboundTouchedEnemy, caplessContactDamaged;
} sTwirlContact;
static s32 lastMarioAnimation;
static u32 gGlobalTimer;
static int capSavePositions;
static void save_file_set_cap_pos(s16 x, s16 y, s16 z) {
    capSavePositions++;
}
static f32 suppliedWaterLevel = -10000, suppliedStepY;
static s32 suppliedAirStep;
static struct Surface floorSurface;
static struct Area area;
static struct Animation animation = { .loopEnd = 4 };
static struct MarioBodyState bodyState;
static s32 obj_has_behavior(struct Object *obj, const BehaviorScript *b) {
    return obj->behavior == b;
}
static const void *segmented_to_virtual(const void *p) {
    return p;
}
static s16 atan2s(f32 z, f32 x) {
    return (s16) (atan2f(x, z) * 32768.0f / 3.14159265f);
}
static f32 sins(s16 angle) {
    return sinf(angle * 3.14159265f / 32768.0f);
}
static f32 coss(s16 angle) {
    return cosf(angle * 3.14159265f / 32768.0f);
}
static f32 absf(f32 v) {
    return fabsf(v);
}
static void vec3_copy_y_off(f32 *d, f32 (*s)[3], f32 y) {
    memcpy(d, *s, sizeof(Vec3f));
    d[1] += y;
}
static void vec3f_copy(void *d, const void *s) {
    memcpy(d, s, sizeof(Vec3f));
}
static void vec3s_set(s16 *v, s16 x, s16 y, s16 z) {
    v[0] = x;
    v[1] = y;
    v[2] = z;
}
#define vec3_zero(v) memset((v), 0, sizeof(Vec3f))
static void vec3f_set(f32 *v, f32 x, f32 y, f32 z) {
    v[0] = x;
    v[1] = y;
    v[2] = z;
}
static void vec3_diff(f32 *d, const void *ap, const void *bp) {
    const f32 *a = ap, *b = bp;
    for (int i = 0; i < 3; i++) {
        d[i] = a[i] - b[i];
    }
}
static f32 vec3_sumsq(f32 *v) {
    return sqr(v[0]) + sqr(v[1]) + sqr(v[2]);
}
static int rumbleRequests, rumbleTime, rumbleLevel, stoppedFallSounds, fallWarnings;
static f32 *stoppedFallSource;
static s32 slipperyFloor;
static void queue_rumble_data(s32 time, s32 level) {
    rumbleRequests++;
    rumbleTime = time;
    rumbleLevel = level;
}
static void stop_sound(u32 sound, f32 *source) {
    if (sound == SOUND_MARIO_WAAAOOOW) {
        stoppedFallSounds++;
        stoppedFallSource = source;
    }
}
// The action setup is a service here; fixes.metal_lava_launch checks its native implementation.
static s32 set_mario_action(struct MarioState *m, u32 action, u32 arg) {
    m->action = action;
    m->actionArg = arg;
    m->actionState = m->actionTimer = 0;
    if (action == ACT_LAVA_BOOST) {
        m->vel[1] = 84;
        if (arg != ACT_ARG_LAVA_BOOST_WALL) {
            m->forwardVel = 0;
        }
    }
    return TRUE;
}
static s32 set_mario_animation(struct MarioState *m, s32 anim) {
    lastMarioAnimation = anim;
    m->marioObj->header.gfx.animInfo.curAnim = &animation;
    return 0;
}
// Animation setup is controlled; this fixture uses ordinary forward playback.
static void set_anim_to_frame(struct MarioState *m, s16 frame) {
    m->marioObj->header.gfx.animInfo.animFrame = frame - 1;
}

static void obj_set_gfx_pos_at_obj_pos(struct Object *a, struct Object *b) {
    memcpy(a->header.gfx.pos, b->oPosVec, sizeof(Vec3f));
}
static void cur_obj_become_intangible(void) {
    o->oIntangibleTimer = -1;
}
static void cur_obj_become_tangible(void) {
    o->oIntangibleTimer = 0;
}
static void cur_obj_hide(void) {
    o->header.gfx.node.flags |= GRAPH_RENDER_INVISIBLE;
}
static void cur_obj_unhide(void) {
    o->header.gfx.node.flags &= ~GRAPH_RENDER_INVISIBLE;
}
static int mineOverlap;
static s32 obj_check_if_collided_with_object(struct Object *a, struct Object *b) {
    return mineOverlap;
}
static void set_object_visibility(struct Object *obj, s32 distance) {
}
static void set_camera_shake_from_point(s32 shake, f32 x, f32 y, f32 z) {
}
static void set_environmental_camera_shake(s32 shake) {
}
static f32 dist_between_objects(struct Object *a, struct Object *b) {
    return sqrtf(sqr(a->oPosX - b->oPosX) + sqr(a->oPosY - b->oPosY) + sqr(a->oPosZ - b->oPosZ));
}
static void obj_mark_for_deletion(struct Object *obj) {
    obj->activeFlags = ACTIVE_FLAG_DEACTIVATED;
}
static void cur_obj_play_sound_2(s32 sound) {
}
static void create_sound_spawner(s32 sound) {
}
static void spawn_mist_particles(void) {
    mist++;
}
static void spawn_mist_particles_with_sound(s32 sound) {
    mist++;
}
static void obj_spawn_loot_yellow_coins(struct Object *obj, s32 n, f32 vel) {
    n = MIN(n, obj->oNumLootCoins);
    if (n > 0) {
        coins += n;
        obj->oNumLootCoins -= n;
    }
}
static void obj_spawn_yellow_coins(struct Object *obj, s32 n) {
    coins += n;
}
static struct Object *spawn_object(struct Object *parent, s32 model, const BehaviorScript *b) {
    if (b == bhvMrIBlueCoin) {
        blueCoins++;
    }
    if (b == bhvKoopaShell) {
        shells++;
    }
    if (b == bhvNormalCap) {
        caps++;
    }
    if (b == bhv1upWalking) {
        oneups++;
    }
    if (b == bhvExplosion) {
        explosions++;
    }
    if (spawnedCount >= 32) {
        fprintf(stderr, "HARNESS ERROR: spawn capacity\n");
        exit(2);
    }
    struct Object *result = &spawned[spawnedCount++];
    *result = (struct Object){ .behavior = b, .parentObj = parent };
    return result;
}
static struct Object *try_to_spawn_object(s32 a, f32 b, struct Object *p, s32 model,
                                          const BehaviorScript *bhv) {
    return spawn_object(p, model, bhv);
}
static void spawn_default_star(f32 x, f32 y, f32 z) {
    stars++;
}
static void save_file_clear_flags(u32 flags) {
    saveClears |= flags;
}
static s32 simpleflips_scream_try_play(void) {
    return FALSE;
}
static s32 castle_secret_lobby_active(void) {
    return FALSE;
}
static void play_puzzle_jingle(void) {
}
static void set_object_respawn_info_bits(struct Object *obj, u32 bits) {
}
static void create_respawner(s32 model, const BehaviorScript *b, s32 dist) {
    respawners++;
}
static void cur_obj_set_model(s32 model) {
}
static s16 obj_angle_to_object(struct Object *a, struct Object *b) {
    return atan2s(b->oPosZ - a->oPosZ, b->oPosX - a->oPosX);
}
static s16 mario_obj_angle_to_object(struct MarioState *m, struct Object *b) {
    return atan2s(b->oPosZ - m->pos[2], b->oPosX - m->pos[0]);
}
static void cur_obj_scale(f32 scale) {
    for (int i = 0; i < 3; i++) {
        o->header.gfx.scale[i] = scale;
    }
}
static void cur_obj_extend_animation_if_at_end(void) {
}
static void cur_obj_update_floor_and_walls(void) {
}
static void cur_obj_move_standard(s32 slope) {
}
static f32 find_water_level(f32 x, f32 z) {
    return suppliedWaterLevel;
}
static void huge_goomba_weakly_attacked(void) {
    o->oAction = GOOMBA_ACT_JUMP;
}
static void wiggler_jumped_on_attack_handler(void) {
    fprintf(stderr, "HARNESS ERROR: Wiggler route\n");
    exit(2);
}
static void obj_set_speed_to_zero(void) {
    o->oForwardVel = o->oVelY = 0;
}
static s32 approach_f32_ptr(f32 *v, f32 target, f32 step) {
    if (*v < target) {
        *v = MIN(*v + step, target);
    } else {
        *v = MAX(*v - step, target);
    }
    return *v == target;
}
static void cur_obj_move_stunned(void) {
    o->oMoveFlags = nextContacts;
    o->oVelY = 0;
}
static s16 object_step(void) {
    f32 angle = o->oMoveAngleYaw * 3.14159265f / 32768.0f;
    o->oPosX += o->oForwardVel * sinf(angle);
    o->oPosZ += o->oForwardVel * cosf(angle);
    o->oPosY += o->oVelY;
    return bobombStepContacts;
}
static void bobomb_act_patrol(void) {
    fprintf(stderr, "HARNESS ERROR: unexpected patrol\n");
    exit(2);
}
static void bobomb_act_chase_mario(void) {
    fprintf(stderr, "HARNESS ERROR: unexpected chase\n");
    exit(2);
}
static s32 obj_lava_death(void) {
    return FALSE;
}
static s32 obj_attack_collided_from_other_object(struct Object *obj) {
    return FALSE;
}
static void play_sound(s32 sound, f32 *pos) {
    damageSounds++;
    if (sound == SOUND_MARIO_WAAAOOOW) {
        fallWarnings++;
    }
}
static void stop_cap_music(void) {
    capMusicStops++;
}
static void set_camera_shake_from_hit(s32 shake) {
    hitShakes++;
}
static int groundPoundImpacts;
void mario_ground_pound_impact(struct MarioState *m) {
    groundPoundImpacts++;
    set_camera_shake_from_hit(SHAKE_GROUND_POUND);
}
static void update_mario_sound_and_camera(struct MarioState *m) {
}
static s32 mario_ground_twirl_is_active(struct MarioState *m) {
    return FALSE;
}
static s32 mario_ground_twirl_rebound_touches_enemy(struct MarioState *m) {
    return FALSE;
}
static u32 determine_interaction(struct MarioState *m, struct Object *obj) {
    return contactInteraction;
}
static void bounce_off_enemy(struct MarioState *m, struct Object *obj, f32 height) {
    bullyBounces++;
}
static void bounce_back_from_attack(struct MarioState *m, u32 interaction) {
    bullyBounces++;
}
static void attack_object(struct Object *obj, u32 interaction) {
    bullyAttacks++;
}
static void push_mario_out_of_object(struct MarioState *m, struct Object *obj, f32 padding) {
    contactPushes++;
}
static u32 bully_knock_back_mario(struct MarioState *m) {
    bullyImpulses++;
    return ACT_BACKWARD_GROUND_KB;
}
static s32 object_facing_mario(struct MarioState *m, struct Object *obj, s16 angle) {
    return grabFacing;
}
static void mario_set_forward_vel(struct MarioState *m, f32 vel) {
    m->forwardVel = vel;
}
static void mario_set_wall_bonk_momentum(struct MarioState *m, f32 speed, f32 minimum) {
    fprintf(stderr, "HARNESS ERROR: unmodeled ground-pound wall recoil\n");
    exit(2);
}
static s32 let_go_of_ledge(struct MarioState *m) {
    return set_mario_action(m, ACT_FREEFALL, 0);
}
static f32 find_floor_height_relative_polar(struct MarioState *m, s16 angle, f32 distance) {
    return m->floorHeight;
}
s32 mario_is_on_floor(struct MarioState *m);
static void mario_lose_cap_on_lethal_damage(struct MarioState *m, s16 yaw) {
    if ((m->flags & (MARIO_NORMAL_CAP | MARIO_CAP_ON_HEAD)) == (MARIO_NORMAL_CAP | MARIO_CAP_ON_HEAD)
        && m->health >= 0x100 && m->health - 0x40 * m->hurtCounter < 0x100) {
        m->flags &= ~(MARIO_NORMAL_CAP | MARIO_CAP_ON_HEAD);
        lethalCapLosses++;
    }
}
static void mario_lose_cap_on_lethal_damage_with_speed(struct MarioState *m, s16 yaw, f32 speedScale) {
    mario_lose_cap_on_lethal_damage(m, yaw);
}
static u32 drop_and_set_mario_action(struct MarioState *m, u32 action, u32 arg) {
    knockbacks++;
    m->heldObj = NULL;
    return set_mario_action(m, action, arg);
}
static void mario_stop_riding_and_holding(struct MarioState *m) {
    m->heldObj = NULL;
}
static s32 mario_has_land_movement_underwater(struct MarioState *m);
static s32 mario_is_riding_underwater_shell(struct MarioState *m) {
    return FALSE;
}
static s32 mario_transition_underwater_shell_to_swimming(struct MarioState *m) {
    return FALSE;
}
static s32 mario_is_in_vertical_wind(struct MarioState *m);
static void cutscene_put_cap_on(struct MarioState *m) {
    m->flags &= ~MARIO_CAP_IN_HAND;
    m->flags |= MARIO_CAP_ON_HEAD;
}
static s32 set_water_plunge_action(struct MarioState *m) {
    return set_mario_action(m, ACT_WATER_PLUNGE, 0);
}
static void play_mario_sound(struct MarioState *m, s32 a, s32 b) {
}
static void update_air_without_turn(struct MarioState *m) {
}
static s32 perform_air_step(struct MarioState *m, u32 arg) {
    m->pos[1] = suppliedStepY;
    return suppliedAirStep;
}
static s32 perform_slide_kick_air_step(struct MarioState *m, f32 *remaining, s32 checkWater,
                                       f32 *waterLevel) {
    f32 startY = m->pos[1];
    *remaining = 0.0f;
    *waterLevel = FLOOR_LOWER_LIMIT;
    s32 result = perform_air_step(m, 0);
    if (checkWater && startY >= suppliedWaterLevel && m->pos[1] <= suppliedWaterLevel
        && suppliedWaterLevel > m->floorHeight && m->vel[1] < 0.0f) {
        *waterLevel = suppliedWaterLevel;
        m->pos[1] = suppliedWaterLevel;
        result = AIR_STEP_NONE;
    }
    return result;
}
static s32 perform_air_step_with_fraction(struct MarioState *m, u32 arg, f32 fraction) {
    return AIR_STEP_NONE;
}
static void spawn_slide_kick_liquid_particles(struct MarioState *m, f32 height,
                                              enum LiquidParticleMaterial material) {
    liquidBursts++;
    liquidHeight = height;
    liquidMaterial = material;
}
static void stop_and_set_height_to_floor(struct MarioState *m) {
    m->pos[1] = m->floorHeight;
}
static void play_mario_landing_sound(struct MarioState *m, s32 a) {
}
static void play_mario_landing_sound_once(struct MarioState *m, s32 sound) {
}
static s32 is_anim_at_end(struct MarioState *m) {
    return FALSE;
}
static void mario_attack_breakable_box_wall(struct MarioState *m) {
}
static void play_sound_if_no_flag(struct MarioState *m, u32 sound, u32 flag) {
    m->flags |= flag;
}
static int heavyLandingSounds;
static void play_mario_heavy_landing_sound(struct MarioState *m, s32 sound) {
    heavyLandingSounds++;
}
static s32 mario_floor_is_slippery(struct MarioState *m) {
    return slipperyFloor;
}
static s32 mario_is_head_underwater(struct MarioState *m) {
    return m->pos[1] + 140.0f < m->waterLevel;
}
static f32 approach_f32(f32 v, f32 target, f32 inc, f32 dec) {
    return v < target ? MIN(v + inc, target) : MAX(v - dec, target);
}
static void update_lava_boost_or_twirling(struct MarioState *m) {
}
static void mario_bonk_reflection(struct MarioState *m, u32 speed) {
}
static void level_trigger_warp(struct MarioState *m, s32 warp) {
    fprintf(stderr, "HARNESS ERROR: unexpected warp\n");
    exit(2);
}
static void reset_rumble_timers_slip(void) {
}
s32 should_get_stuck_in_ground(struct MarioState *m);
s32 check_fall_damage_or_get_stuck(struct MarioState *m, u32 hardFallAction);
s32 mario_is_knocked_back(u32 action);
static void cur_obj_update(void);
void obj_set_hitbox(struct Object *obj, struct ObjectHitbox *hitbox);
void shelled_koopa_attack_handler(s32 attack);
void mario_remove_cap_powers(struct MarioState *m);
void mario_blow_off_cap(struct MarioState *m, f32 speed);
u32 mario_bowser_impact_knockback(struct MarioState *m, s32 inAir);
static void bowser_bounce_effects(s32 *timer) {
}
static void cur_obj_init_animation_with_sound(s32 animation) {
}
static s32 cur_obj_init_animation_and_check_if_near_end(s32 animation) {
    return FALSE;
}
static s32 mario_is_in_air_action(void) {
    return (mario.action & ACT_FLAG_AIR) != 0;
}
void mario_queue_bowser_impact(struct Object *source);
void mario_queue_bowser_impact_with_strength(struct Object *source, s32 heavy);
static s32 bowser_arena_try_drop_coin(struct Object *obj) {
    (void) obj;
    return FALSE;
}
static void bowser_arena_recover_enemy(struct Object *obj) {
    (void) obj;
}
static s32 bowser_arena_phase_in(struct Object *obj) {
    (void) obj;
    return FALSE;
}
static void bowser_arena_bomb_destroyed(struct Object *obj) {
    (void) obj;
}
static const BehaviorScript bhvBowserShockWave[] = { 0 };
#include "metal_sources.inc"
static void cur_obj_update(void) {
    aiUpdates++;
    if (o->oPrevAction != o->oAction) {
        o->oTimer = 0;
        o->oPrevAction = o->oAction;
    }
    if (o->oAction == OBJ_ACT_SQUISHED) {
        obj_act_squished(1.0f);
    } else if (o->oAction == OBJ_ACT_VERTICAL_KNOCKBACK || o->oAction == OBJ_ACT_HORIZONTAL_KNOCKBACK) {
        obj_act_knockback(1.0f);
    } else if (o->behavior == bhvBobomb) {
        if (o->oBehParams2ndByte == 0) {
            generic_bobomb_free_loop();
        } else {
            stationary_bobomb_free_loop();
        }
    } else if (o->behavior == bhvMoneybag && o->oAction == MONEYBAG_ACT_DEATH) {
        moneybag_act_death();
    }
    o->oTimer++;
}
static struct Object enemy, parent, hole, marioObj;
static void begin(const BehaviorScript *behavior) {
    mario_set_punch_hit_phase(0);
    sPalette = MARIO_PALETTE_STANDARD;
    rumbleRequests = 0;
    rumbleTime = 0;
    rumbleLevel = 0;
    stoppedFallSounds = 0;
    fallWarnings = 0;
    stoppedFallSource = NULL;
    slipperyFloor = FALSE;
    groundPoundImpacts = 0;
    heavyLandingSounds = 0;
    memset(&enemy, 0, sizeof(enemy));
    memset(&parent, 0, sizeof(parent));
    memset(&hole, 0, sizeof(hole));
    memset(&mario, 0, sizeof(mario));
    memset(&marioObj, 0, sizeof(marioObj));
    sController.buttonDown = 0;
    mario.controller = &sController;
    enemy.behavior = behavior;
    enemy.parentObj = &enemy;
    enemy.activeFlags = ACTIVE_FLAG_ACTIVE;
    if (behavior == bhvBowserBomb) {
        enemy.oInteractionSubtype = bowserAttackSubtypes[8];
    }
    enemy.oPosX = 100;
    enemy.hitboxRadius = 65;
    enemy.hitboxHeight = 100;
    for (int i = 0; i < 3; i++) {
        enemy.header.gfx.scale[i] = 1;
    }
    enemy.oGravity = -4;
    enemy.oFloor = &hole;
    enemy.oMontyMoleCurrentHole = &hole;
    gCurrentObject = &enemy;
    const struct {
        const BehaviorScript *behavior;
        struct ObjectHitbox *hitbox;
    } hitboxes[] = { { bhvBobomb, &sBobombHitbox },
                     { bhvMadPiano, &sMadPianoHitbox },
                     { bhvGoomba, &sGoombaHitbox },
                     { bhvFloomba, &sGoombaHitbox },
                     { bhvKoopa, &sKoopaHitbox },
                     { bhvSpiny, &sSpinyHitbox },
                     { bhvSnufit, &sSnufitHitbox },
                     { bhvFlyGuy, &sFlyGuyHitbox },
                     { bhvSkeeter, &sSkeeterHitbox },
                     { bhvSwoop, &sSwoopHitbox },
                     { bhvEnemyLakitu, &sEnemyLakituHitbox },
                     { bhvMontyMole, &sMontyMoleHitbox },
                     { bhvKlepto, &sKleptoHitbox },
                     { bhvScuttlebug, &sScuttlebugHitbox },
                     { bhvSpindrift, &sSpindriftHitbox } };
    for (unsigned i = 0; i < sizeof(hitboxes) / sizeof(hitboxes[0]); i++) {
        if (behavior == hitboxes[i].behavior) {
            obj_set_hitbox(&enemy, hitboxes[i].hitbox);
        }
    }
    if (behavior == bhvChuckya) {
        enemy.oNumLootCoins = 5;
    }
    if (behavior == bhvHeaveHo || behavior == bhvSpiny) {
        enemy.oNumLootCoins = 0;
    }
    gCurrentObject = &enemy;
    gMarioObject = &marioObj;
    mario.flags = MARIO_METAL_CAP;
    mario.action = ACT_WALKING;
    mario.vel[0] = 20;
    mario.marioObj = &marioObj;
    coins = blueCoins = shells = caps = stars = oneups = respawners = explosions = mist = saveClears =
        aiUpdates = spawnedCount = 0;
    mineOverlap = 0;
    gObjectLists[0].next = (struct Object *) &gObjectLists[0];
    sInvulnerable = sDelayInvincTimer = 0;
    knockbacks = damageSounds = hitShakes = lethalCapLosses = capMusicStops = 0;
    lastMarioAnimation = -1;
    capSavePositions = 0;
    contactPushes = bullyImpulses = bullyAttacks = bullyBounces = 0;
    contactInteraction = 0;
    grabFacing = TRUE;
    mario.health = 0x880;
    bobombStepContacts = OBJ_COL_FLAG_GROUNDED;
    floorSurface = (struct Surface){ .normal.y = 1 };
    area = (struct Area){ 0 };
    mario.floor = &floorSurface;
    mario.area = &area;
    mario.marioBodyState = &bodyState;
    mario.ceilHeight = 1000;
    suppliedWaterLevel = -10000;
    mario.waterLevel = -10000;
    sMontyMoleKillStreak = 0;
    memset(sMontyMoleLastKilledPos, 0, sizeof(Vec3f));
    nextContacts = OBJ_MOVE_IN_AIR;
}
static int stunned_swoop_floor_pose(void) {
    const BehaviorScript *behaviors[] = { bhvSwoop, bhvGoomba };
    const u32 contacts[] = { OBJ_MOVE_ON_GROUND, OBJ_MOVE_IN_AIR,
                             OBJ_MOVE_ON_GROUND | OBJ_MOVE_ABOVE_LAVA, OBJ_MOVE_UNDERWATER_ON_GROUND };
    for (unsigned b = 0; b < 2; b++) {
        for (unsigned c = 0; c < sizeof(contacts) / sizeof(contacts[0]); c++) {
            begin(behaviors[b]);
            enemy.oGroundTwirlStunTimer = 5;
            enemy.oGroundTwirlStunDuration = 5;
            enemy.oFloorHeight = enemy.oPosY = 100;
            enemy.oMoveFlags = contacts[c];
            nextContacts = contacts[c];
            enemy.header.gfx.scale[1] = 1.5f;
            enemy.header.gfx.angle[0] = 0x1000;
            enemy.header.gfx.angle[1] = 0x2000;
            enemy.header.gfx.angle[2] = 0x3000;
            update_object_with_ground_twirl_stun();
            s32 supportedSwoop = b == 0 && c == 0;
            EXPECT_EQ(enemy.header.gfx.angle[0], supportedSwoop ? 0 : 0x1000);
            EXPECT_EQ(enemy.header.gfx.angle[1], 0x2000);
            EXPECT_EQ(enemy.header.gfx.angle[2], supportedSwoop ? 0 : 0x3000);
            EXPECT_EQ(enemy.header.gfx.pos[1], supportedSwoop ? 115 : 100);
            EXPECT_EQ(enemy.oPosY, 100);
            EXPECT_EQ(enemy.oFloorHeight, 100);
            EXPECT_EQ(enemy.oGraphYOffset, 0);
        }
    }
    return TRUE;
}

static int stunned_water(void) {
    const BehaviorScript *behaviors[] = { bhvGoomba,   bhvFloomba, bhvKoopa,       bhvSpiny,
                                          bhvSkeeter,  bhvFlyGuy,  bhvMontyMole,   bhvSwoop,
                                          bhvKlepto,   bhvSnufit,  bhvEnemyLakitu, bhvChuckya,
                                          bhvHeaveHo,  bhvBobomb,  bhvMoneybag,    bhvScuttlebug,
                                          bhvSpindrift };
    for (unsigned i = 0; i < sizeof(behaviors) / sizeof(behaviors[0]); i++) {
        begin(behaviors[i]);
        enemy.oGravity = -4;
        enemy.oBuoyancy = 0;
        enemy.oHealth = 0;
        enemy.oAction = behaviors[i] == bhvChuckya ? 2 : 0;
        enemy.oGroundTwirlStunTimer = 12;
        suppliedWaterLevel = 100;
        int vulnerable = i < 12;
        update_object_with_ground_twirl_stun();
        EXPECT_EQ(aiUpdates, vulnerable);
        EXPECT_EQ(enemy.oGroundTwirlStunTimer, vulnerable ? 0 : 11);
        if (vulnerable) {
            EXPECT_EQ((enemy.oMoveFlags & OBJ_MOVE_MASK_IN_WATER) != 0, TRUE);
            if (behaviors[i] != bhvChuckya) {
                EXPECT_EQ(obj_die_if_above_lava_and_health_non_positive(), TRUE);
                EXPECT_EQ(enemy.activeFlags, ACTIVE_FLAG_DEACTIVATED);
            }
        }
    }
    for (int boundary = 0; boundary < 4; boundary++) {
        begin(bhvGoomba);
        enemy.oGravity = -4;
        enemy.oGroundTwirlStunTimer = 12;
        suppliedWaterLevel = boundary == 0 ? 9 : 100;
        if (boundary == 1) {
            enemy.oBuoyancy = 5;
        }
        if (boundary == 2) {
            enemy.oHealth = 1;
        }
        if (boundary == 3) {
            enemy.activeFlags |= ACTIVE_FLAG_IGNORE_ENV_BOXES;
        }
        update_object_with_ground_twirl_stun();
        EXPECT_EQ(aiUpdates, 0);
        EXPECT_EQ(enemy.oGroundTwirlStunTimer, 11);
    }
    return 1;
}
static int timing(void) {
    for (int surface = 0; surface < 3; surface++) {
        begin(bhvGoomba);
        mario_metal_stun_enemy_on_contact(&mario, &enemy);
        EXPECT_EQ(enemy.metalContactCooldown, 15);
        EXPECT_EQ(enemy.oGroundTwirlStunTimer, -30);
        if (surface == 0) {
            nextContacts = OBJ_MOVE_ON_GROUND;
        }
        if (surface == 1) {
            nextContacts = OBJ_MOVE_UNDERWATER_ON_GROUND;
        }
        if (surface == 2) {
            nextContacts = OBJ_MOVE_HIT_WALL | OBJ_MOVE_IN_AIR;
        }
        for (int update = 0; update < 15; update++) {
            mario_metal_stun_enemy_on_contact(&mario, &enemy);
            EXPECT_EQ(enemy.metalContactPendingDefeat, FALSE);
            update_object_with_ground_twirl_stun();
            EXPECT_EQ(enemy.metalContactCooldown, 14 - update);
            EXPECT_EQ(mist, 0);
            EXPECT_EQ(aiUpdates, 0);
        }
        // Surface impact alone has not queued or executed a kill.
        EXPECT_EQ(enemy.metalContactPendingDefeat, FALSE);
        mario_metal_stun_enemy_on_contact(&mario, &enemy);
        EXPECT_EQ(enemy.metalContactPendingDefeat, TRUE);
        mario_metal_stun_enemy_on_contact(&mario, &enemy);
        EXPECT_EQ(enemy.metalContactPendingDefeat, TRUE);
        update_object_with_ground_twirl_stun();
        EXPECT_EQ(enemy.metalContactPendingDefeat, FALSE);
        EXPECT_EQ(enemy.oGroundTwirlStunTimer, 0);
        EXPECT_EQ(enemy.oGroundTwirlStunDuration, 0);
        for (int update = 0; update < 30 && enemy.activeFlags; update++) {
            mario_metal_stun_enemy_on_contact(&mario, &enemy);
            EXPECT_EQ(enemy.metalContactPendingDefeat, FALSE);
            update_object_with_ground_twirl_stun();
        }
        EXPECT_EQ(enemy.activeFlags, ACTIVE_FLAG_DEACTIVATED);
        EXPECT_EQ(coins, 1);
        EXPECT_EQ(mist, 1);
    }
    return TRUE;
}
static int wing_air_twirl_contacts(void) {
    for (int flying = 0; flying < 2; flying++) {
        begin(bhvGoomba);
        mario.action = flying ? ACT_FLYING : ACT_L_AIR_TWIRL;
        mario.airTwirlFlightRollFrame = flying ? 1 : 0;
        mario.flags = MARIO_WING_CAP;
        mario_metal_stun_enemy_on_contact(&mario, &enemy);
        EXPECT_EQ(enemy.metalContactCooldown, 15);
        EXPECT_EQ(rumbleRequests, 1);
        EXPECT_EQ(rumbleTime, 5);
        EXPECT_EQ(rumbleLevel, 80);
        EXPECT_EQ(enemy.oGroundTwirlStunTimer, -30);
        for (int update = 0; update < 15; update++) {
            mario_metal_stun_enemy_on_contact(&mario, &enemy);
            EXPECT_EQ(enemy.metalContactPendingDefeat, FALSE);
            update_object_with_ground_twirl_stun();
        }
        EXPECT_EQ(rumbleRequests, 1);
        EXPECT_EQ(enemy.oMoveFlags & OBJ_MOVE_MASK_ON_GROUND, 0);
        EXPECT_EQ(enemy.metalContactPendingDefeat, FALSE);
        mario.vel[0] = 0;
        mario_metal_stun_enemy_on_contact(&mario, &enemy);
        EXPECT_EQ(enemy.metalContactPendingDefeat, FALSE);
        mario.vel[0] = -20;
        mario_metal_stun_enemy_on_contact(&mario, &enemy);
        EXPECT_EQ(enemy.metalContactPendingDefeat, FALSE);
        mario.vel[0] = 20;
        enemy.oGroundTwirlContactRebound = 1;
        mario_metal_stun_enemy_on_contact(&mario, &enemy);
        EXPECT_EQ(enemy.metalContactPendingDefeat, FALSE);
        enemy.oGroundTwirlContactRebound = 0;
        mario.flags = 0; // Wing expiry removes the airborne follow-up exception.
        mario_metal_stun_enemy_on_contact(&mario, &enemy);
        EXPECT_EQ(enemy.metalContactPendingDefeat, FALSE);
        mario.flags = MARIO_WING_CAP | MARIO_METAL_CAP;
        mario_metal_stun_enemy_on_contact(&mario, &enemy);
        EXPECT_EQ(enemy.metalContactPendingDefeat, TRUE);
        EXPECT_EQ(rumbleRequests, 2);
        mario_metal_stun_enemy_on_contact(&mario, &enemy);
        EXPECT_EQ(rumbleRequests, 2);
        update_object_with_ground_twirl_stun();
        for (int update = 0; update < 30 && enemy.activeFlags; update++) {
            update_object_with_ground_twirl_stun();
        }
        EXPECT_EQ(enemy.activeFlags, ACTIVE_FLAG_DEACTIVATED);
        EXPECT_EQ(coins, 1);
        EXPECT_EQ(mist, 1);
    }
    return TRUE;
}

static int eligibility(void) {
    for (int phase = 0; phase < 8; phase++) {
        begin(bhvGoomba);
        enemy.oGroundTwirlStunTimer = phase == 0 ? -1 : 6;
        enemy.oMoveFlags = phase == 0 ? OBJ_MOVE_IN_AIR : OBJ_MOVE_ON_GROUND;
        if (phase == 1) {
            mario.vel[0] = 0;
        }
        if (phase == 2) {
            mario.vel[0] = -20;
        }
        if (phase == 3) {
            mario.action = ACT_FREEFALL;
        }
        if (phase == 4) {
            enemy.oGroundTwirlContactRebound = 2;
        }
        if (phase == 5) {
            enemy.oIntangibleTimer = -1;
        }
        if (phase == 6) {
            enemy.oInteractStatus = INT_STATUS_WAS_ATTACKED;
        }
        if (phase == 7) {
            enemy.metalContactCooldown = 1;
        }
        mario_metal_stun_enemy_on_contact(&mario, &enemy);
        EXPECT_EQ(enemy.metalContactPendingDefeat, FALSE);
    }
    begin(bhvKoopa);
    enemy.oKoopaMovementType = KOOPA_BP_KOOPA_THE_QUICK_BOB;
    mario_metal_stun_enemy_on_contact(&mario, &enemy);
    EXPECT_EQ(enemy.oGroundTwirlStunTimer, 0);
    begin(bhvBoo);
    enemy.oAction = 1;
    enemy.oInteractType = INTERACT_BOUNCE_TOP;
    mario_metal_stun_enemy_on_contact(&mario, &enemy);
    EXPECT_EQ(enemy.oInteractStatus & INT_STATUS_ATTACK_MASK, ATTACK_FROM_ABOVE);
    EXPECT_EQ(enemy.metalContactPendingDefeat, FALSE);
    return TRUE;
}
static int roster(void) {
    const BehaviorScript *behaviors[] = { bhvGoomba,     bhvFloomba, bhvKoopa,     bhvBobomb,
                                          bhvChuckya,    bhvHeaveHo, bhvSpiny,     bhvScuttlebug,
                                          bhvSkeeter,    bhvFlyGuy,  bhvSpindrift, bhvMontyMole,
                                          bhvMoneybag,   bhvSwoop,   bhvKlepto,    bhvSnufit,
                                          bhvEnemyLakitu };
    for (int kind = 0; kind < 17; kind++) {
        for (int cycle = 0; cycle < 3; cycle++) {
            begin(behaviors[kind]);
            if (enemy.behavior == bhvKoopa) {
                enemy.oKoopaMovementType = cycle == 0 ? KOOPA_BP_UNSHELLED : KOOPA_BP_NORMAL;
                enemy.header.gfx.scale[0] = cycle == 2 ? 0.8f : 1.5f;
                enemy.oNumLootCoins = -1;
            }
            if (enemy.behavior == bhvGoomba || enemy.behavior == bhvFloomba) {
                enemy.oGoombaSize = cycle;
            }
            if (enemy.behavior == bhvKlepto) {
                enemy.oAnimState = cycle + 1;
            }
            if (enemy.behavior == bhvSpiny) {
                enemy.parentObj = &parent;
                parent.behavior = bhvEnemyLakitu;
                parent.oEnemyLakituNumSpinies = 1;
                parent.prevObj = &enemy;
            }
            if (enemy.behavior == bhvScuttlebug) {
                enemy.parentObj = &parent;
                enemy.oScuttlebugHasNoLootCoins = cycle == 2;
            }
            if (enemy.behavior == bhvEnemyLakitu || enemy.behavior == bhvMontyMole) {
                enemy.prevObj = &parent;
            }
            if (enemy.behavior == bhvMontyMole) {
                sMontyMoleKillStreak = 7;
            }
            enemy.oGroundTwirlStunTimer = 10;
            enemy.oMoveFlags = OBJ_MOVE_ON_GROUND;
            EXPECT_EQ(mario_ground_twirl_is_enemy(&enemy), TRUE);
            mario_metal_stun_enemy_on_contact(&mario, &enemy);
            EXPECT_EQ(enemy.metalContactPendingDefeat, TRUE);
            update_object_with_ground_twirl_stun();
            EXPECT_EQ(aiUpdates, 0);
            EXPECT_EQ(enemy.metalContactPendingDefeat, FALSE);
            for (int update = 0; update < 30 && enemy.activeFlags; update++) {
                update_object_with_ground_twirl_stun();
            }
            if (enemy.behavior == bhvMontyMole) {
                EXPECT_EQ(enemy.activeFlags, ACTIVE_FLAG_ACTIVE);
                EXPECT_EQ(enemy.oAction, MONTY_MOLE_ACT_SELECT_HOLE);
                EXPECT_EQ(hole.oMontyMoleHoleCooldown, 30);
                EXPECT_EQ(sMontyMoleKillStreak, 8);
                EXPECT_EQ(oneups, 1);
                EXPECT_EQ(enemy.prevObj, NULL);
            } else {
                EXPECT_EQ(enemy.activeFlags, ACTIVE_FLAG_DEACTIVATED);
            }
            if (enemy.behavior == bhvKoopa) {
                EXPECT_EQ(shells, cycle == 1);
                EXPECT_EQ(blueCoins, 1);
            }
            if ((enemy.behavior == bhvGoomba || enemy.behavior == bhvFloomba)
                && cycle == GOOMBA_SIZE_HUGE) {
                EXPECT_EQ(blueCoins, 1);
            }
            if (enemy.behavior == bhvBobomb) {
                EXPECT_EQ(coins, 1);
                EXPECT_EQ(respawners, 1);
                EXPECT_EQ(explosions, 1);
            }
            if (enemy.behavior == bhvMoneybag || enemy.behavior == bhvChuckya) {
                EXPECT_EQ(coins, 5);
            }
            if (enemy.behavior == bhvHeaveHo) {
                EXPECT_EQ(coins, 0);
                EXPECT_EQ(enemy.oCommonAnchorAction, 0);
            }
            if (enemy.behavior == bhvSpiny) {
                EXPECT_EQ(parent.oEnemyLakituNumSpinies, 0);
                EXPECT_EQ(parent.prevObj, NULL);
            }
            if (enemy.behavior == bhvScuttlebug) {
                EXPECT_EQ(parent.oScuttlebugSpawnerIsDeactivated, TRUE);
                EXPECT_EQ(coins, cycle == 2 ? 0 : 3);
            }
            if (enemy.behavior == bhvEnemyLakitu) {
                EXPECT_EQ(enemy.prevObj, NULL);
                EXPECT_EQ(coins, 5);
            }
            if (enemy.behavior == bhvSnufit || enemy.behavior == bhvFlyGuy) {
                EXPECT_EQ(coins, 2);
            }
            if (enemy.behavior == bhvSkeeter || enemy.behavior == bhvSpindrift) {
                EXPECT_EQ(coins, 3);
            }
            if (enemy.behavior == bhvSwoop) {
                EXPECT_EQ(coins, 1);
            }
            if (enemy.behavior == bhvGoomba || enemy.behavior == bhvFloomba) {
                EXPECT_EQ(coins, cycle == GOOMBA_SIZE_HUGE ? 0 : 1);
            }
            if (enemy.behavior == bhvKlepto || enemy.behavior == bhvMontyMole
                || enemy.behavior == bhvSpiny) {
                EXPECT_EQ(coins, 0);
            }
            if (enemy.behavior == bhvKlepto) {
                EXPECT_EQ(caps, cycle == 0);
                EXPECT_EQ(stars, cycle != 0);
                EXPECT_EQ(saveClears, cycle == 0 ? SAVE_FLAG_CAP_ON_KLEPTO : 0);
                EXPECT_EQ(enemy.oAnimState, KLEPTO_ANIM_STATE_HOLDING_NOTHING);
            }
        }
    }
    return TRUE;
}
static int grab_cleanup(void) {
    const BehaviorScript *grabbers[] = { bhvChuckya, bhvHeaveHo };
    for (int kind = 0; kind < 2; kind++) {
        begin(grabbers[kind]);
        enemy.oGroundTwirlStunTimer = 10;
        enemy.oMoveFlags = OBJ_MOVE_ON_GROUND;
        mario_metal_stun_enemy_on_contact(&mario, &enemy);
        EXPECT_EQ(enemy.metalContactPendingDefeat, TRUE);
        mario.action = ACT_GRABBED;
        mario.usedObj = &enemy;
        enemy.oCommonAnchorAction = 2;
        enemy.oInteractionSubtype = INT_SUBTYPE_GRABS_MARIO;
        update_object_with_ground_twirl_stun();
        EXPECT_EQ(enemy.oCommonAnchorAction, 0);
        EXPECT_EQ(enemy.oInteractionSubtype & INT_SUBTYPE_GRABS_MARIO, 0);
        EXPECT_EQ(marioObj.oInteractStatus
                      & (INT_STATUS_MARIO_THROWN_BY_OBJ | INT_STATUS_MARIO_DROPPED_BY_OBJ),
                  INT_STATUS_MARIO_THROWN_BY_OBJ | INT_STATUS_MARIO_DROPPED_BY_OBJ);
        EXPECT_EQ(act_grabbed(&mario), TRUE);
        EXPECT_EQ(mario.action, ACT_THROWN_FORWARD);
        EXPECT_EQ(mario.actionArg, 0);
        struct Object anchor = { .activeFlags = ACTIVE_FLAG_ACTIVE, .parentObj = &enemy };
        gCurrentObject = &anchor;
        if (kind == 0) {
            common_anchor_mario_behavior(40, 40, INT_STATUS_MARIO_DROPPED_BY_OBJ);
        } else {
            bhv_heave_ho_throw_mario_loop();
        }
        EXPECT_EQ(anchor.activeFlags, ACTIVE_FLAG_DEACTIVATED);
    }
    return TRUE;
}

static int short_contact_immunities(void) {
    for (int cycle = 0; cycle < 3; cycle++) {
        begin(bhvGoomba);
        mario.flags = 0;
        mario.action = ACT_WALKING;
        mario.bullyKnockbackImmunityFrames = 2;
        for (int frame = 0; frame < 2; frame++) {
            EXPECT_EQ(interact_bully(&mario, INTERACT_BULLY, &enemy), FALSE);
            EXPECT_EQ(bullyImpulses, 0);
            EXPECT_EQ(contactPushes, frame + 1);
            mario_tick_contact_immunities(&mario);
        }
        EXPECT_EQ(mario.bullyKnockbackImmunityFrames, 0);
        EXPECT_EQ(interact_bully(&mario, INTERACT_BULLY, &enemy), TRUE);
        EXPECT_EQ(bullyImpulses, 1);
        EXPECT_EQ(mario.action, ACT_BACKWARD_GROUND_KB);

        begin(bhvGoomba);
        mario.flags = 0;
        mario.action = ACT_WALKING;
        mario.bullyKnockbackImmunityFrames = 2;
        contactInteraction = INT_PUNCH;
        interact_bully(&mario, INTERACT_BULLY, &enemy);
        EXPECT_EQ(bullyAttacks, 1);
        EXPECT_EQ(bullyImpulses, 0);

        begin(bhvHeaveHo);
        mario.flags = 0;
        mario.action = ACT_WALKING;
        enemy.oInteractionSubtype = INT_SUBTYPE_GRABS_MARIO;
        mario.heaveHoLaunchImmunityFrames = 5;
        for (int frame = 0; frame < 5; frame++) {
            EXPECT_EQ(check_object_grab_mario(&mario, INTERACT_GRABBABLE, &enemy), FALSE);
            EXPECT_EQ(mario.action, ACT_WALKING);
            mario_tick_contact_immunities(&mario);
        }
        EXPECT_EQ(mario.heaveHoLaunchImmunityFrames, 0);
        EXPECT_EQ(check_object_grab_mario(&mario, INTERACT_GRABBABLE, &enemy), TRUE);
        EXPECT_EQ(mario.action, ACT_GRABBED);

        begin(bhvChuckya);
        mario.flags = 0;
        mario.action = ACT_WALKING;
        enemy.oInteractionSubtype = INT_SUBTYPE_GRABS_MARIO;
        mario.heaveHoLaunchImmunityFrames = 5;
        EXPECT_EQ(check_object_grab_mario(&mario, INTERACT_GRABBABLE, &enemy), TRUE);

        begin(bhvHeaveHo);
        mario.action = ACT_GRABBED;
        mario.usedObj = &enemy;
        marioObj.oInteractStatus = INT_STATUS_MARIO_THROWN_BY_OBJ;
        EXPECT_EQ(act_grabbed(&mario), TRUE);
        EXPECT_EQ(mario.heaveHoThrowPendingLanding, TRUE);
        EXPECT_EQ(mario.heaveHoLaunchImmunityFrames, 0);
        begin(bhvHeaveHo);
        mario.action = ACT_GRABBED;
        mario.usedObj = &enemy;
        marioObj.oInteractStatus = INT_STATUS_MARIO_THROWN_BY_OBJ | INT_STATUS_MARIO_DROPPED_BY_OBJ;
        EXPECT_EQ(act_grabbed(&mario), TRUE);
        EXPECT_EQ(mario.heaveHoThrowPendingLanding, FALSE);

        begin(bhvHeaveHo);
        mario.flags = 0;
        mario.action = ACT_HARD_BACKWARD_GROUND_KB;
        enemy.oInteractionSubtype = INT_SUBTYPE_GRABS_MARIO;
        mario.heaveHoThrowRecoveryPending = TRUE;
        sInvulnerable = TRUE;
        mario_start_heave_ho_recovery_immunity(&mario);
        mario_tick_contact_immunities(&mario);
        EXPECT_EQ(mario.heaveHoThrowRecoveryPending, TRUE);
        EXPECT_EQ(mario.heaveHoLaunchImmunityFrames, 0);
        mario.action = ACT_IDLE;
        mario.invincTimer = 2;
        for (int frame = 0; frame < 2; frame++) {
            mario_start_heave_ho_recovery_immunity(&mario);
            mario_tick_contact_immunities(&mario);
            EXPECT_EQ(mario.heaveHoLaunchImmunityFrames, 0);
            EXPECT_EQ(mario.heaveHoThrowRecoveryPending, TRUE);
            mario.invincTimer--;
        }
        sInvulnerable = FALSE;
        mario_start_heave_ho_recovery_immunity(&mario);
        EXPECT_EQ(mario.heaveHoThrowRecoveryPending, FALSE);
        EXPECT_EQ(mario.heaveHoLaunchImmunityFrames, 5);
        for (int frame = 0; frame < 5; frame++) {
            EXPECT_EQ(check_object_grab_mario(&mario, INTERACT_GRABBABLE, &enemy), FALSE);
            mario_tick_contact_immunities(&mario);
        }
        EXPECT_EQ(check_object_grab_mario(&mario, INTERACT_GRABBABLE, &enemy), TRUE);

        begin(bhvHeaveHo);
        mario.heaveHoThrowRecoveryPending = TRUE;
        mario.health = 0;
        mario_start_heave_ho_recovery_immunity(&mario);
        EXPECT_EQ(mario.heaveHoThrowRecoveryPending, FALSE);
        EXPECT_EQ(mario.heaveHoLaunchImmunityFrames, 0);
    }
    return TRUE;
}

static int heave_ho_fatal_fall_cap(void) {
    for (int direction = 0; direction < 2; direction++) {
        for (int fatal = 0; fatal < 2; fatal++) {
            begin(bhvHeaveHo);
            mario.flags = MARIO_NORMAL_CAP | MARIO_CAP_ON_HEAD;
            mario.action = direction ? ACT_THROWN_FORWARD : ACT_THROWN_BACKWARD;
            mario.heaveHoThrowPendingLanding = TRUE;
            mario.health = fatal ? 0x180 : 0x400;
            mario.peakHeight = FALL_DAMAGE_HEIGHT_SMALL + 1;
            mario.vel[1] = -56;
            EXPECT_EQ(check_fall_damage(&mario, ACT_HARD_BACKWARD_GROUND_KB), FALSE);
            EXPECT_EQ(mario.hurtCounter, 8);
            EXPECT_EQ(lethalCapLosses, fatal);
            EXPECT_EQ(!!(mario.flags & MARIO_CAP_ON_HEAD), !fatal);
        }
    }
    begin(bhvHeaveHo);
    mario.flags = MARIO_NORMAL_CAP | MARIO_CAP_ON_HEAD;
    mario.action = ACT_THROWN_BACKWARD;
    mario.heaveHoThrowPendingLanding = FALSE;
    mario.health = 0x180;
    mario.peakHeight = FALL_DAMAGE_HEIGHT_SMALL + 1;
    mario.vel[1] = -56;
    EXPECT_EQ(check_fall_damage(&mario, ACT_HARD_BACKWARD_GROUND_KB), FALSE);
    EXPECT_EQ(lethalCapLosses, 0);
    EXPECT_EQ(mario.flags & MARIO_CAP_ON_HEAD, MARIO_CAP_ON_HEAD);

    begin(bhvHeaveHo);
    mario.flags = MARIO_NORMAL_CAP | MARIO_CAP_ON_HEAD;
    mario.action = ACT_THROWN_BACKWARD;
    mario.heaveHoThrowPendingLanding = TRUE;
    mario.health = 0x300;
    mario.peakHeight = FALL_DAMAGE_HEIGHT_LARGE + 1;
    mario.vel[1] = -56;
    EXPECT_EQ(check_fall_damage(&mario, ACT_HARD_BACKWARD_GROUND_KB), TRUE);
    EXPECT_EQ(lethalCapLosses, 1);
    EXPECT_EQ(mario.flags & MARIO_CAP_ON_HEAD, 0);

    begin(bhvHeaveHo);
    mario.flags = MARIO_NORMAL_CAP | MARIO_CAP_ON_HEAD | MARIO_METAL_CAP;
    mario.action = ACT_THROWN_BACKWARD;
    mario.heaveHoThrowPendingLanding = TRUE;
    mario.health = 0x180;
    mario.peakHeight = FALL_DAMAGE_HEIGHT_LARGE + 1;
    mario.vel[1] = -56;
    EXPECT_EQ(check_fall_damage(&mario, ACT_HARD_BACKWARD_GROUND_KB), FALSE);
    EXPECT_EQ(lethalCapLosses, 0);
    EXPECT_EQ(mario.flags & MARIO_CAP_ON_HEAD, MARIO_CAP_ON_HEAD);
    return TRUE;
}

static int bobomb_launch_before_explosion(void) {
    for (int stationary = 0; stationary < 2; stationary++) {
        for (int direction = 0; direction < 4; direction++) {
            begin(bhvBobomb);
            enemy.oBehParams2ndByte = stationary;
            if (direction == 0) {
                enemy.oPosX = 100;
            } else if (direction == 1) {
                enemy.oPosX = -100;
            } else {
                enemy.oPosX = 0;
            }
            if (direction == 2) {
                enemy.oPosZ = 100;
            } else if (direction == 3) {
                enemy.oPosZ = -100;
            } else {
                enemy.oPosZ = 0;
            }
            enemy.oBobombFuseTimer = 151;
            enemy.oBobombFuseLit = TRUE;
            cur_obj_defeat_metal_contact_enemy();
            EXPECT_EQ(enemy.oAction, BOBOMB_ACT_LAUNCHED);
            EXPECT_EQ(enemy.oBobombFuseTimer, 0);
            EXPECT_EQ(enemy.oIntangibleTimer, -1);
            EXPECT_EQ(enemy.oForwardVel, 25);
            EXPECT_EQ(enemy.oVelY, 30);
            f32 distance = sqr(enemy.oPosX) + sqr(enemy.oPosZ);
            bobombStepContacts = 0;
            for (int update = 0; update < 3; update++) {
                update_object_with_ground_twirl_stun();
                EXPECT_EQ(enemy.oAction, BOBOMB_ACT_LAUNCHED);
                EXPECT_EQ(explosions, 0);
                EXPECT_EQ(sqr(enemy.oPosX) + sqr(enemy.oPosZ) > distance, TRUE);
                distance = sqr(enemy.oPosX) + sqr(enemy.oPosZ);
                enemy.oBobombFuseTimer++;
            }
            EXPECT_EQ(enemy.oPosY, 90);
            bobombStepContacts = OBJ_COL_FLAG_GROUNDED;
            update_object_with_ground_twirl_stun();
            EXPECT_EQ(enemy.oAction, BOBOMB_ACT_EXPLODE);
            EXPECT_EQ(explosions, 0);
            for (int update = 0; update < 6; update++) {
                update_object_with_ground_twirl_stun();
            }
            EXPECT_EQ(explosions, 1);
            EXPECT_EQ(respawners, 1);
            EXPECT_EQ(coins, 1);
            EXPECT_EQ(enemy.activeFlags, ACTIVE_FLAG_DEACTIVATED);
        }
    }
    return TRUE;
}
static int soaked_blast_drying(void) {
    const BehaviorScript *blasts[] = { bhvExplosion, bhvBowserBomb };
    for (unsigned blast = 0; blast < sizeof(blasts) / sizeof(blasts[0]); blast++) {
        for (int protection = 0; protection < 3; protection++) {
            for (int cycle = 0; cycle < 3; cycle++) {
                begin(blasts[blast]);
                enemy.oInteractType = INTERACT_DAMAGE;
                enemy.oDamageOrCoinValue = 2;
                enemy.oInteractionSubtype = blast ? INT_SUBTYPE_BOWSER_ATTACK : 0;
                mario.flags = MARIO_NORMAL_CAP | MARIO_CAP_ON_HEAD;
                if (protection == 1) {
                    mario.flags |= MARIO_METAL_CAP;
                } else if (protection == 2) {
                    mario.invincTimer = 17;
                }
                mario.waterLevel = 0;
                mario.pos[1] = 0;
                mario_update_soaked_status(&mario);
                mario.soakedDripFrames = 9;
                interact_damage(&mario, INTERACT_DAMAGE, &enemy);
                EXPECT_EQ(mario_is_soaked(&mario), FALSE);
                EXPECT_EQ(mario.soakedTimer, 0);
                EXPECT_EQ(mario.soakedDripFrames, 0);
                for (int update = 0; update < 3; update++) {
                    mario_update_soaked_status(&mario);
                    EXPECT_EQ(mario_is_soaked(&mario), FALSE);
                }
                mario.waterLevel = -10000;
                mario_update_soaked_status(&mario);
                mario.waterLevel = 0;
                mario_update_soaked_status(&mario);
                EXPECT_EQ(mario_is_soaked(&mario), TRUE);
            }
        }
    }
    // Enemy body damage does not dry Mario.
    begin(bhvGoomba);
    mario.flags = MARIO_NORMAL_CAP | MARIO_CAP_ON_HEAD;
    mario_apply_soaked_status(&mario);
    enemy.oDamageOrCoinValue = 2;
    interact_damage(&mario, INTERACT_DAMAGE, &enemy);
    EXPECT_EQ(mario_is_soaked(&mario), TRUE);
    return TRUE;
}

static int explosion_immunity(void) {
    u32 actions[] = { ACT_WALKING, ACT_FREEFALL, ACT_WATER_IDLE, ACT_GROUND_TWIRL };
    for (unsigned a = 0; a < sizeof(actions) / sizeof(*actions); a++) {
        begin(bhvExplosion);
        enemy.oInteractType = INTERACT_DAMAGE;
        enemy.oDamageOrCoinValue = 2;
        mario.action = actions[a];
        mario.forwardVel = 32;
        mario.vel[1] = -12;
        mario.heldObj = &parent;
        mario.invincTimer = 17;
        EXPECT_EQ(interact_damage(&mario, INTERACT_DAMAGE, &enemy), FALSE);
        EXPECT_EQ(mario.action, actions[a]);
        EXPECT_EQ(mario.forwardVel, 32);
        EXPECT_EQ(mario.vel[1], -12);
        EXPECT_EQ(mario.hurtCounter, 0);
        EXPECT_EQ(mario.heldObj, &parent);
        EXPECT_EQ(enemy.oInteractStatus, 0);
        EXPECT_EQ(sDelayInvincTimer, 0);
        EXPECT_EQ(knockbacks, 0);
        EXPECT_EQ(damageSounds, 0);
        EXPECT_EQ(hitShakes, 0);
        EXPECT_EQ(mario.invincTimer, 17);
        mario.flags = MARIO_CAP_ON_HEAD;
        mario.invincTimer = 0;
        EXPECT_EQ(interact_damage(&mario, INTERACT_DAMAGE, &enemy), TRUE);
        EXPECT_EQ(mario.hurtCounter, 8);
        EXPECT_EQ(knockbacks, 1);
        EXPECT_EQ(mario.heldObj, NULL);
    }
    return TRUE;
}
static void mine_update(void) {
    bhv_bowser_bomb_loop();
    o->oTimer++;
    if (o->oAction != o->oPrevAction) {
        o->oTimer = 0;
        o->oPrevAction = o->oAction;
    }
}
static int bowser_cap_attacks(void) {
    for (int powers = 0; powers < 8; powers++) {
        u32 flags = MARIO_NORMAL_CAP | MARIO_CAP_ON_HEAD | (powers & 1 ? MARIO_WING_CAP : 0)
                    | (powers & 2 ? MARIO_METAL_CAP : 0) | (powers & 4 ? MARIO_VANISH_CAP : 0);
        begin(bhvMadPiano);
        mario.flags = flags;
        mario.capTimer = 100;
        enemy.oInteractionSubtype = bowserAttackSubtypes[0];
        enemy.oDamageOrCoinValue = 2;
        enemy.oInteractType = INTERACT_DAMAGE;
        EXPECT_EQ(mario_vanish_ignores_hostile_contact(&mario, &enemy, INTERACT_DAMAGE), FALSE);
        EXPECT_EQ(interact_damage(&mario, INTERACT_DAMAGE, &enemy), TRUE);
        EXPECT_EQ(mario.hurtCounter, 8);
        EXPECT_EQ(mario.flags & MARIO_SPECIAL_CAPS, 0);
        EXPECT_EQ(mario.flags & MARIO_NORMAL_CAP, MARIO_NORMAL_CAP);
        EXPECT_EQ(mario.flags & MARIO_CAP_ON_HEAD, MARIO_CAP_ON_HEAD);
        EXPECT_EQ(capMusicStops, powers ? 1 : 0);
        EXPECT_EQ(knockbacks, 1);
        if (powers) {
            EXPECT_EQ(mario.capTimer, 0);
        }
        // Ordinary invincibility prevents a new hit and retains the powers.
        begin(bhvMadPiano);
        mario.flags = flags;
        mario.capTimer = 100;
        sInvulnerable = TRUE;
        enemy.oInteractionSubtype = INT_SUBTYPE_BOWSER_ATTACK;
        enemy.oDamageOrCoinValue = 2;
        EXPECT_EQ(interact_damage(&mario, INTERACT_DAMAGE, &enemy), FALSE);
        EXPECT_EQ(mario.flags, flags);
        EXPECT_EQ(mario.capTimer, 100);
        EXPECT_EQ(capMusicStops, 0);
        for (unsigned flame = 1; flame < 8; flame++) {
            begin(bhvMadPiano);
            mario.flags = flags;
            mario.capTimer = 100;
            enemy.oInteractionSubtype = bowserAttackSubtypes[flame];
            EXPECT_EQ(interact_flame(&mario, INTERACT_FLAME, &enemy), TRUE);
            EXPECT_EQ(mario.action, ACT_BURNING_JUMP);
            EXPECT_EQ(mario.flags & MARIO_SPECIAL_CAPS, 0);
            EXPECT_EQ(mario.flags & MARIO_CAP_ON_HEAD, MARIO_CAP_ON_HEAD);
            EXPECT_EQ(capMusicStops, powers ? 1 : 0);
        }
        for (int attack = 0; attack < 3; attack++) {
            begin(bhvMadPiano);
            mario.flags = flags;
            mario.capTimer = 100;
            mario.action = ACT_SHOCKWAVE_BOUNCE;
            marioObj.oInteractStatus = attack == 0   ? INT_STATUS_MARIO_SHOCKWAVE
                                       : attack == 1 ? INT_STATUS_MARIO_KNOCKBACK_DMG
                                                     : INT_STATUS_MARIO_STUNNED;
            act_shockwave_bounce(&mario);
            EXPECT_EQ(mario.hurtCounter, attack == 1 ? 8 : 4);
            EXPECT_EQ(mario.flags & MARIO_SPECIAL_CAPS, 0);
            EXPECT_EQ(mario.flags & MARIO_CAP_ON_HEAD, attack == 1 ? 0 : MARIO_CAP_ON_HEAD);
            EXPECT_EQ(capSavePositions, attack == 1 ? 1 : 0);
            EXPECT_EQ(capMusicStops, powers ? 1 : 0);
        }
    }
    // Non-Bowser flame keeps Metal protection, and its ordinary damage keeps Vanish protection.
    begin(bhvMadPiano);
    mario.flags = MARIO_CAP_ON_HEAD | MARIO_METAL_CAP;
    EXPECT_EQ(interact_flame(&mario, INTERACT_FLAME, &enemy), FALSE);
    EXPECT_EQ(mario.flags & MARIO_METAL_CAP, MARIO_METAL_CAP);
    begin(bhvMadPiano);
    mario.flags = MARIO_CAP_ON_HEAD | MARIO_VANISH_CAP;
    EXPECT_EQ(interact_damage(&mario, INTERACT_DAMAGE, &enemy), FALSE);
    EXPECT_EQ(mario.flags & MARIO_VANISH_CAP, MARIO_VANISH_CAP);
    return TRUE;
}

static int damage_values(void) {
    // Zero-base native hazards charge capless Mario, including during twirl.
    const BehaviorScript *zeroHazards[] = { bhvExplosion, bhvFallingPillarHitbox };
    for (int twirl = 0; twirl < 2; twirl++) {
        for (int capped = 0; capped < 2; capped++) {
            for (int protection = 0; protection < 4; protection++) {
                for (unsigned kind = 0; kind < 2; kind++) {
                    begin(zeroHazards[kind]);
                    enemy.oDamageOrCoinValue = 0;
                    enemy.oInteractType = INTERACT_DAMAGE;
                    mario.flags = (capped ? MARIO_CAP_ON_HEAD : 0)
                                  | (protection == 1 ? MARIO_METAL_CAP : 0)
                                  | (protection == 2 ? MARIO_VANISH_CAP : 0);
                    mario.action = twirl ? ACT_GROUND_TWIRL : ACT_WALKING;
                    sInvulnerable =
                        protection == 3; // Dispatch tests separately check twirl-only immunity removal.
                    interact_damage(&mario, INTERACT_DAMAGE, &enemy);
                    EXPECT_EQ(mario.hurtCounter, !capped && protection == 0 ? 4 : 0);
                }
            }
        }
    }
    // Zero-base electrical hazards use the capless minimum before the wet multiplier.
    for (int capped = 0; capped < 2; capped++) {
        for (int powers = 0; powers < 8; powers++) {
            for (int wet = 0; wet < 2; wet++) {
                begin(bhvGoomba);
                enemy.oDamageOrCoinValue = 0;
                enemy.oInteractType = INTERACT_SHOCK;
                u32 flags = (capped ? MARIO_CAP_ON_HEAD : 0) | (powers & 1 ? MARIO_WING_CAP : 0)
                            | (powers & 2 ? MARIO_METAL_CAP : 0) | (powers & 4 ? MARIO_VANISH_CAP : 0);
                mario.flags = flags;
                mario.capTimer = 90;
                mario.soakedTimer = 150;
                mario.soakedThisUpdate = wet;
                if (wet) {
                    mario.waterLevel = 20;
                    mario.pos[1] = 0;
                }
                interact_shock(&mario, INTERACT_SHOCK, &enemy);
                int expectedDamage = 0;
                if (!capped) {
                    expectedDamage = wet ? 8 : 4;
                }
                EXPECT_EQ(mario.hurtCounter, expectedDamage);
                EXPECT_EQ(mario.flags & MARIO_SPECIAL_CAPS, capped ? (flags & MARIO_SPECIAL_CAPS) : 0);
                EXPECT_EQ(mario.soakedTimer, !capped && wet ? 0 : 150);
            }
        }
    }
    // Odd and even base hits double without rounding; each hit, not the queue, is capped.
    for (int capped = 0; capped < 2; capped++) {
        for (int base = 0; base <= 12; base++) {
            begin(bhvGoomba);
            mario.flags = capped ? MARIO_CAP_ON_HEAD : 0;
            enemy.oDamageOrCoinValue = base;
            enemy.oInteractType = INTERACT_DAMAGE;
            mario.interactObj = &enemy;
            mario.hurtCounter = 8;
            int damage;
            if (base == 0) {
                damage = capped ? 0 : 1;
            } else {
                damage = MIN(base * (capped ? 1 : 2), 7);
            }
            EXPECT_EQ(take_damage_from_interact_object(&mario), damage);
            EXPECT_EQ(mario.hurtCounter, 8 + 4 * damage);
            begin(bhvGoomba);
            mario.flags = capped ? MARIO_CAP_ON_HEAD : 0;
            enemy.oDamageOrCoinValue = base;
            enemy.oInteractType = INTERACT_SHOCK;
            mario.hurtCounter = 8;
            mario.soakedThisUpdate = TRUE;
            EXPECT_EQ(interact_shock(&mario, INTERACT_SHOCK, &enemy), TRUE);
            EXPECT_EQ(mario.hurtCounter, 8 + 4 * MIN(2 * damage, 7));
        }
    }
    for (int capped = 0; capped < 2; capped++) {
        for (int large = 0; large < 2; large++) {
            begin(bhvGoomba);
            mario.flags = capped ? MARIO_CAP_ON_HEAD : 0;
            mario.action = ACT_FREEFALL;
            mario.vel[1] = -56;
            mario.peakHeight = (large ? FALL_DAMAGE_HEIGHT_LARGE : FALL_DAMAGE_HEIGHT_SMALL) + 1;
            check_fall_damage(&mario, ACT_HARD_BACKWARD_GROUND_KB);
            int damage;
            if (large) {
                damage = capped ? 4 : 7;
            } else {
                damage = capped ? 2 : 4;
            }
            EXPECT_EQ(mario.hurtCounter, 4 * damage);
        }
    }
    for (int capped = 0; capped < 2; capped++) {
        for (int attack = 0; attack < 2; attack++) {
            begin(bhvGoomba);
            mario.flags = (capped ? MARIO_CAP_ON_HEAD : 0) | MARIO_SPECIAL_CAPS;
            mario.action = ACT_SHOCKWAVE_BOUNCE;
            marioObj.oInteractStatus =
                attack ? INT_STATUS_MARIO_SHOCKWAVE : INT_STATUS_MARIO_KNOCKBACK_DMG;
            act_shockwave_bounce(&mario);
            int damage;
            if (attack) {
                damage = capped ? 1 : 2;
            } else {
                damage = capped ? 2 : 4;
            }
            EXPECT_EQ(mario.hurtCounter, 4 * damage);
            EXPECT_EQ(mario.flags & MARIO_SPECIAL_CAPS, 0);
            if (!attack) {
                begin(bhvGoomba);
                mario.flags = (capped ? MARIO_CAP_ON_HEAD : 0) | MARIO_SPECIAL_CAPS;
                mario.action = ACT_LEDGE_GRAB;
                mario.input = INPUT_STOMPED;
                marioObj.oInteractStatus = INT_STATUS_MARIO_KNOCKBACK_DMG;
                act_ledge_grab(&mario);
                EXPECT_EQ(mario.hurtCounter, 4 * (capped ? 2 : 4));
                EXPECT_EQ(mario.flags & MARIO_SPECIAL_CAPS, 0);
                EXPECT_EQ(mario.action, ACT_HARD_BACKWARD_AIR_KB);
            }
        }
    }
    for (int capped = 0; capped < 2; capped++) {
        for (int path = 0; path < 3; path++) {
            begin(bhvGoomba);
            mario.flags = capped ? MARIO_CAP_ON_HEAD : 0;
            floorSurface.type = SURFACE_BURNING;
            if (path == 0) {
                check_lava_boost(&mario);
            }
            if (path == 1) {
                lava_boost_on_wall(&mario);
            }
            if (path == 2) {
                mario.action = ACT_LAVA_BOOST;
                mario.vel[1] = -20;
                suppliedAirStep = AIR_STEP_LANDED;
                act_lava_boost(&mario);
            }
            EXPECT_EQ(mario.hurtCounter, capped ? 12 : 24);
        }
    }
    return TRUE;
}
static int bowser_mine_damage(void) {
    for (int cap = 0; cap < 2; cap++) {
        begin(bhvBowserBomb);
        enemy.oDamageOrCoinValue = BOWSER_BOMB_DAMAGE;
        enemy.oInteractType = INTERACT_DAMAGE;
        mario.flags = cap ? (MARIO_NORMAL_CAP | MARIO_CAP_ON_HEAD) : 0;
        EXPECT_EQ(interact_damage(&mario, INTERACT_DAMAGE, &enemy), TRUE);
        EXPECT_EQ(mario.hurtCounter, 28);
        EXPECT_EQ(knockbacks, 1);
        EXPECT_EQ(caps, cap);
        EXPECT_EQ(capSavePositions, cap);
        EXPECT_EQ(mario.flags & MARIO_CAP_ON_HEAD, 0);
        EXPECT_EQ(0x880 - mario.hurtCounter * 0x40, 0x180);
    }
    for (int powers = 0; powers < 8; powers++) {
        begin(bhvBowserBomb);
        enemy.oDamageOrCoinValue = BOWSER_BOMB_DAMAGE;
        mario.flags = MARIO_NORMAL_CAP | MARIO_CAP_ON_HEAD | (powers & 1 ? MARIO_WING_CAP : 0)
                      | (powers & 2 ? MARIO_METAL_CAP : 0) | (powers & 4 ? MARIO_VANISH_CAP : 0);
        mario.capTimer = 100;
        EXPECT_EQ(interact_damage(&mario, INTERACT_DAMAGE, &enemy), TRUE);
        EXPECT_EQ(mario.hurtCounter, 28);
        EXPECT_EQ(knockbacks, 1);
        EXPECT_EQ(mario.flags & (MARIO_SPECIAL_CAPS | MARIO_NORMAL_CAP | MARIO_CAP_ON_HEAD), 0);
        EXPECT_EQ(caps, 1);
        EXPECT_EQ(capSavePositions, 1);
        EXPECT_EQ(mario.capPickupLockFrames, MARIO_CAP_PICKUP_LOCK_FRAMES);
        EXPECT_EQ(mario.capPickupLockStartFrame, gGlobalTimer);
        EXPECT_EQ(capMusicStops, powers ? 1 : 0);
    }
    begin(bhvBowserBomb);
    enemy.oDamageOrCoinValue = BOWSER_BOMB_DAMAGE;
    mario.flags = MARIO_NORMAL_CAP | MARIO_CAP_ON_HEAD | MARIO_SPECIAL_CAPS;
    mario.capTimer = 100;
    sInvulnerable = TRUE;
    EXPECT_EQ(interact_damage(&mario, INTERACT_DAMAGE, &enemy), FALSE);
    EXPECT_EQ(mario.hurtCounter, 0);
    EXPECT_EQ(knockbacks, 0);
    EXPECT_EQ(caps, 0);
    EXPECT_EQ(capSavePositions, 0);
    EXPECT_EQ(mario.flags & MARIO_CAP_ON_HEAD, MARIO_CAP_ON_HEAD);
    EXPECT_EQ(mario.flags & MARIO_SPECIAL_CAPS, MARIO_SPECIAL_CAPS);
    EXPECT_EQ(mario.capTimer, 100);
    return TRUE;
}
static int bowser_twirl_damage(void) {
    for (int powers = 0; powers < 8; powers++) {
        for (int hat = 0; hat < 2; hat++) {
            for (int mine = 0; mine < 2; mine++) {
                for (int invulnerable = 0; invulnerable < 2; invulnerable++) {
                    begin(mine ? bhvBowserBomb : bhvMadPiano);
                    mario.action = ACT_GROUND_TWIRL;
                    u32 flags = (hat ? MARIO_NORMAL_CAP | MARIO_CAP_ON_HEAD : 0)
                                | (powers & 1 ? MARIO_WING_CAP : 0) | (powers & 2 ? MARIO_METAL_CAP : 0)
                                | (powers & 4 ? MARIO_VANISH_CAP : 0);
                    mario.flags = flags;
                    mario.capTimer = 100;
                    enemy.oInteractType = INTERACT_DAMAGE;
                    enemy.oDamageOrCoinValue = mine ? BOWSER_BOMB_DAMAGE : 2;
                    enemy.oInteractionSubtype = INT_SUBTYPE_BOWSER_ATTACK;
                    sInvulnerable = invulnerable;
                    EXPECT_EQ(interact_damage(&mario, INTERACT_DAMAGE, &enemy), !invulnerable);
                    if (invulnerable) {
                        EXPECT_EQ(mario.hurtCounter, 0);
                        EXPECT_EQ(mario.flags, flags);
                        EXPECT_EQ(caps, 0);
                        EXPECT_EQ(capMusicStops, 0);
                        EXPECT_EQ(knockbacks, 0);
                    } else {
                        int damage = 7;
                        if (!mine) {
                            damage = hat ? 2 : 4;
                        }
                        EXPECT_EQ(mario.hurtCounter, 4 * damage);
                        EXPECT_EQ(mario.flags & MARIO_SPECIAL_CAPS, 0);
                        EXPECT_EQ(capMusicStops, powers ? 1 : 0);
                        EXPECT_EQ(mario.flags & MARIO_CAP_ON_HEAD,
                                  !mine && hat ? MARIO_CAP_ON_HEAD : 0);
                        EXPECT_EQ(caps, mine && hat);
                        EXPECT_EQ(capSavePositions, mine && hat);
                        EXPECT_EQ(knockbacks, 1);
                    }
                }
            }
        }
    }
    return TRUE;
}
static int bowser_mine_respawn(void) {
    begin(bhvBowserBomb);
    enemy.oIntangibleTimer = 0;
    mineOverlap = 1;
    for (int cycle = 0; cycle < 3; cycle++) {
        mine_update();
        EXPECT_EQ(explosions, cycle + 1);
        EXPECT_EQ(enemy.oAction, BOWSER_BOMB_ACT_RESPAWNING);
        EXPECT_EQ(enemy.oIntangibleTimer, -1);
        EXPECT_EQ(enemy.header.gfx.node.flags & GRAPH_RENDER_INVISIBLE, GRAPH_RENDER_INVISIBLE);
        EXPECT_EQ(enemy.activeFlags, ACTIVE_FLAG_ACTIVE);
        struct Object *child = &spawned[spawnedCount - 1];
        EXPECT_EQ(child->oBehParams2ndByte, 1);
        gCurrentObject = child;
        child->oInteractType = INTERACT_DAMAGE;
        child->oIntangibleTimer = 0;
        bhv_explosion_init();
        EXPECT_EQ(child->oInteractType, 0);
        EXPECT_EQ(child->oIntangibleTimer, -1);
        gCurrentObject = &enemy;
        for (int update = 1; update < 60; update++) {
            mine_update();
        }
        EXPECT_EQ(enemy.oAction, BOWSER_BOMB_ACT_RESPAWNING);
        EXPECT_EQ(explosions, cycle + 1);
        mine_update();
        EXPECT_EQ(enemy.oAction, BOWSER_BOMB_ACT_ARMED);
        EXPECT_EQ(enemy.oIntangibleTimer, 0);
        EXPECT_EQ(enemy.oInteractStatus, 0);
        EXPECT_EQ(enemy.header.gfx.node.flags & GRAPH_RENDER_INVISIBLE, 0);
        EXPECT_EQ(explosions, cycle + 1); // Continual overlap detonates on the next armed update.
    }
    // Bowser wins a simultaneous contact and consumes the mine permanently.
    begin(bhvBowserBomb);
    mineOverlap = 1;
    enemy.oInteractStatus = INT_STATUS_HIT_MINE;
    mine_update();
    EXPECT_EQ(enemy.activeFlags, ACTIVE_FLAG_DEACTIVATED);
    EXPECT_EQ(explosions, 0);
    EXPECT_EQ(spawnedCount, 1);
    EXPECT_EQ(spawned[0].behavior, bhvBowserBombExplosion);
    // Other generic explosions retain their damage/tangibility.
    begin(bhvExplosion);
    enemy.parentObj = &parent;
    parent.behavior = bhvBobomb;
    enemy.oBehParams2ndByte = 1;
    enemy.oInteractType = INTERACT_DAMAGE;
    enemy.oIntangibleTimer = 0;
    bhv_explosion_init();
    EXPECT_EQ(enemy.oInteractType, INTERACT_DAMAGE);
    EXPECT_EQ(enemy.oIntangibleTimer, 0);
    return TRUE;
}
static int bowser_mine_selection(void) {
    begin(bhvBowserBomb);
    struct Object bowser = { .oBowserStatus = BOWSER_STATUS_VALID_THROW },
                  near = { .behavior = bhvBowserBomb, .activeFlags = ACTIVE_FLAG_ACTIVE }, far = near;
    near.oPosX = 100;
    far.oPosX = 200;
    near.oAction = BOWSER_BOMB_ACT_RESPAWNING;
    near.oIntangibleTimer = -1;
    gObjectLists[0].next = &near;
    near.header.next = &far;
    far.header.next = (struct Object *) &gObjectLists[0];
    gCurrentObject = &bowser;
    EXPECT_EQ(bowser_check_hit_mine(), TRUE);
    EXPECT_EQ(near.oInteractStatus, 0);
    EXPECT_EQ(far.oInteractStatus, INT_STATUS_HIT_MINE);
    far.oInteractStatus = 0;
    far.oPosX = 600;
    EXPECT_EQ(bowser_check_hit_mine(), FALSE);
    EXPECT_EQ(far.oInteractStatus, 0);
    far.oPosX = 599;
    EXPECT_EQ(bowser_check_hit_mine(), TRUE);
    EXPECT_EQ(far.oInteractStatus, INT_STATUS_HIT_MINE);
    far.oInteractStatus = 0;
    far.oPosX = 600;
    near.oAction = BOWSER_BOMB_ACT_ARMED;
    near.oIntangibleTimer = 0;
    EXPECT_EQ(bowser_check_hit_mine(), TRUE);
    EXPECT_EQ(near.oInteractStatus, INT_STATUS_HIT_MINE);
    near.activeFlags = ACTIVE_FLAG_DEACTIVATED;
    EXPECT_EQ(bowser_check_hit_mine(), FALSE);
    return TRUE;
}
static int breakdance_bowser_attacks(void) {
    const u32 actions[] = { ACT_PUNCHING, ACT_MOVE_PUNCHING };
    const s32 frames[] = { 0, 1, 12, 18, 25 };
    struct Animation spin = { .loopEnd = NATIVE_BREAKDANCE_FRAMES };
    for (unsigned action = 0; action < sizeof(actions) / sizeof(*actions); action++) {
        for (unsigned frame = 0; frame < sizeof(frames) / sizeof(*frames); frame++) {
            for (int attack = 0; attack < 4; attack++) {
                begin(bhvMadPiano);
                mario.flags = MARIO_NORMAL_CAP | MARIO_CAP_ON_HEAD;
                mario.action = actions[action];
                mario.actionArg = ACT_ARG_PUNCH_SEQUENCE_BREAKDANCE;
                mario.actionTimer = 1;
                marioObj.header.gfx.animInfo = (struct AnimInfo){ .curAnim = &spin,
                                                                  .animID = MARIO_ANIM_BREAKDANCE,
                                                                  .animFrame = frames[frame] };
                enemy.oInteractionSubtype = INT_SUBTYPE_BOWSER_ATTACK;
                enemy.oDamageOrCoinValue = 2;
                if (attack == 0) {
                    EXPECT_EQ(interact_damage(&mario, INTERACT_DAMAGE, &enemy), TRUE);
                    EXPECT_EQ(mario.hurtCounter, 8);
                    EXPECT_EQ(knockbacks, 1);
                } else if (attack == 1) {
                    EXPECT_EQ(interact_flame(&mario, INTERACT_FLAME, &enemy), TRUE);
                    EXPECT_EQ(mario.action, ACT_BURNING_JUMP);
                } else {
                    marioObj.oInteractStatus =
                        attack == 3 ? INT_STATUS_MARIO_KNOCKBACK_DMG : INT_STATUS_MARIO_SHOCKWAVE;
                    EXPECT_EQ(mario_bowser_impact_knockback(&mario, FALSE), TRUE);
                    EXPECT_EQ(mario.hurtCounter, attack == 3 ? 8 : 4);
                    EXPECT_EQ(mario.action,
                              attack == 3 ? ACT_HARD_BACKWARD_GROUND_KB : ACT_SOFT_BACKWARD_GROUND_KB);
                }
            }
        }
    }
    return TRUE;
}

static int breakdance_damage_windows(void) {
    const u32 actions[] = { ACT_PUNCHING, ACT_MOVE_PUNCHING };
    struct Animation spin = { .loopEnd = NATIVE_BREAKDANCE_FRAMES };
    for (int cycle = 0; cycle < 3; cycle++) {
        for (unsigned action = 0; action < sizeof(actions) / sizeof(*actions); action++) {
            for (int frame = 0; frame < spin.loopEnd; frame++) {
                for (int contact = 0; contact < 9; contact++) {
                    begin(bhvGoomba);
                    mario.flags = MARIO_NORMAL_CAP | MARIO_CAP_ON_HEAD;
                    mario.action = actions[action];
                    mario.actionArg = ACT_ARG_PUNCH_SEQUENCE_BREAKDANCE;
                    mario.actionTimer = 1;
                    marioObj.header.gfx.animInfo = (struct AnimInfo){ .curAnim = &spin,
                                                                      .animID = MARIO_ANIM_BREAKDANCE,
                                                                      .animFrame = frame };
                    enemy.oDamageOrCoinValue = 1;
                    s32 vulnerable = frame == 0 || frame == 12;
                    sInvulnerable = mario_breakdance_protects_from_damage(&mario);
                    EXPECT_EQ(sInvulnerable, !vulnerable);
                    EXPECT_EQ(mario_protects_from_knockback(&mario), TRUE);
                    if (contact == 0) {
                        EXPECT_EQ(take_damage_and_knock_back(&mario, &enemy), vulnerable);
                    } else if (contact == 1) {
                        EXPECT_EQ(interact_snufit_bullet(&mario, INTERACT_SNUFIT_BULLET, &enemy),
                                  vulnerable);
                    } else if (contact == 2) {
                        EXPECT_EQ(interact_bully(&mario, INTERACT_BULLY, &enemy), vulnerable);
                        EXPECT_EQ(bullyImpulses, 0);
                    } else if (contact == 3) {
                        EXPECT_EQ(interact_shock(&mario, INTERACT_SHOCK, &enemy), vulnerable);
                    } else if (contact == 4) {
                        mario.pos[1] = mario.floorHeight;
                        check_lava_boost(&mario);
                    } else if (contact == 5) {
                        mario_queue_bowser_impact_with_strength(&enemy, TRUE);
                    } else if (contact == 6) {
                        EXPECT_EQ(interact_flame(&mario, INTERACT_FLAME, &enemy), vulnerable);
                    } else if (contact == 7) {
                        EXPECT_EQ(interact_strong_wind(&mario, INTERACT_STRONG_WIND, &enemy), FALSE);
                    } else {
                        EXPECT_EQ(interact_tornado(&mario, INTERACT_TORNADO, &enemy), FALSE);
                    }
                    if (contact == 5) {
                        EXPECT_EQ(knockbacks, 1);
                        EXPECT_EQ(mario.action, ACT_HARD_BACKWARD_GROUND_KB);
                        EXPECT_EQ(mario.hurtCounter, 8);
                        continue;
                    }
                    EXPECT_EQ(knockbacks, 0);
                    EXPECT_EQ(mario.action, actions[action]);
                    s32 damage = (contact == 2 || contact >= 7)   ? 0
                                 : (contact == 4 || contact == 6) ? 12
                                 : contact == 5                   ? 8
                                                                  : 4;
                    EXPECT_EQ(mario.hurtCounter, vulnerable ? damage : 0);
                }
            }
            // A fresh repeat must not inherit the previous spin's vulnerable pose.
            begin(bhvGoomba);
            mario.action = actions[action];
            mario.actionArg = ACT_ARG_PUNCH_SEQUENCE_BREAKDANCE;
            mario.actionTimer = 0;
            marioObj.header.gfx.animInfo = (struct AnimInfo){ .curAnim = &spin,
                                                              .animID = MARIO_ANIM_BREAKDANCE,
                                                              .animFrame = spin.loopEnd - 1 };
            EXPECT_EQ(mario_protects_from_knockback(&mario), TRUE);
            EXPECT_EQ(mario_breakdance_protects_from_damage(&mario), FALSE);
        }
    }
    // Existing invulnerability and Vanish still protect a damage-vulnerable frame.
    for (int immunity = 0; immunity < 2; immunity++) {
        begin(bhvGoomba);
        mario.flags = MARIO_NORMAL_CAP | MARIO_CAP_ON_HEAD | (immunity ? MARIO_VANISH_CAP : 0);
        sInvulnerable = !immunity;
        mario.action = ACT_PUNCHING;
        mario.actionArg = ACT_ARG_PUNCH_SEQUENCE_BREAKDANCE;
        mario.actionTimer = 1;
        marioObj.header.gfx.animInfo =
            (struct AnimInfo){ .curAnim = &spin, .animID = MARIO_ANIM_BREAKDANCE, .animFrame = 12 };
        EXPECT_EQ(take_damage_and_knock_back(&mario, &enemy), FALSE);
        EXPECT_EQ(interact_snufit_bullet(&mario, INTERACT_SNUFIT_BULLET, &enemy), FALSE);
        EXPECT_EQ(interact_bully(&mario, INTERACT_BULLY, &enemy), FALSE);
        EXPECT_EQ(mario.action, ACT_PUNCHING);
        EXPECT_EQ(mario.hurtCounter, 0);
        EXPECT_EQ(knockbacks, 0);
    }
    return TRUE;
}
static int light_enemy_knockback(void) {
    u32 actions[] = { ACT_WALKING, ACT_FREEFALL, ACT_WATER_IDLE };
    for (int terrain = 0; terrain < 3; terrain++) {
        for (int side = 0; side < 2; side++) {
            for (int damage = 1; damage <= 4; damage++) {
                for (int big = 0; big < 2; big++) {
                    begin(bhvMadPiano);
                    enemy.oDamageOrCoinValue = damage;
                    enemy.oInteractionSubtype = big ? INT_SUBTYPE_BIG_KNOCKBACK : 0;
                    mario.action = actions[terrain];
                    mario.faceAngle[1] = side ? 0xC000 : 0x4000;
                    mario.flags |= MARIO_CAP_ON_HEAD;
                    mario.forwardVel = 8;
                    EXPECT_EQ(interact_mr_blizzard(&mario, INTERACT_MR_BLIZZARD, &enemy), TRUE);
                    EXPECT_EQ(mario.action, side ? sForwardKnockbackActions[terrain][0]
                                                 : sBackwardKnockbackActions[terrain][0]);
                    EXPECT_EQ(absf(mario.forwardVel), terrain == 2 ? 28 : 16);
                    EXPECT_EQ(mario.hurtCounter, 0);
                    // Expiry restores the native damage tier and the big-knockback override.
                    begin(bhvMadPiano);
                    enemy.oDamageOrCoinValue = damage;
                    enemy.oInteractionSubtype = big ? INT_SUBTYPE_BIG_KNOCKBACK : 0;
                    mario.flags = MARIO_CAP_ON_HEAD;
                    mario.action = actions[terrain];
                    mario.faceAngle[1] = side ? 0xC000 : 0x4000;
                    EXPECT_EQ(interact_mr_blizzard(&mario, INTERACT_MR_BLIZZARD, &enemy), TRUE);
                    int tier;
                    if (damage >= 4) {
                        tier = 2;
                    } else if (damage >= 2) {
                        tier = 1;
                    } else {
                        tier = 0;
                    }
                    EXPECT_EQ(mario.action, side ? sForwardKnockbackActions[terrain][tier]
                                                 : sBackwardKnockbackActions[terrain][tier]);
                    EXPECT_EQ(mario.hurtCounter, 4 * damage);
                    if (big) {
                        EXPECT_EQ(absf(mario.forwardVel), 40);
                    }
                }
            }
        }
    }
    begin(bhvMadPiano);
    mario.health = 0xff;
    mario.interactObj = &enemy;
    EXPECT_EQ(determine_knockback_action(&mario, 3), ACT_HARD_BACKWARD_GROUND_KB);
    return TRUE;
}
static int wing_surface_soaked(void) {
    const u32 caps[] = { MARIO_WING_CAP, MARIO_WING_CAP | MARIO_METAL_CAP,
                         MARIO_WING_CAP | MARIO_VANISH_CAP };
    for (unsigned cap = 0; cap < sizeof(caps) / sizeof(*caps); cap++) {
        begin(bhvGoomba);
        mario.flags = caps[cap];
        mario.action = ACT_WALKING;
        mario.pos[1] = mario.waterLevel = 0.0f;
        mario_update_soaked_status(&mario);
        EXPECT_EQ(mario_is_soaked(&mario), !(caps[cap] & MARIO_VANISH_CAP));
        if (!(caps[cap] & MARIO_VANISH_CAP)) {
            EXPECT_EQ(mario.soakedWaterContact, TRUE);
            EXPECT_EQ(mario.soakedTimer, 150);
            mario_consume_soaked_status(&mario);
            mario_update_soaked_status(&mario);
            EXPECT_EQ(mario_is_soaked(&mario), FALSE);
            // Leaving and touching the surface again rearms the status.
            mario.pos[1] = 1.0f;
            mario_update_soaked_status(&mario);
            mario.pos[1] = 0.0f;
            mario_update_soaked_status(&mario);
            EXPECT_EQ(mario_is_soaked(&mario), TRUE);
            mario.pos[1] = 1.0f;
            mario_update_soaked_status(&mario);
            EXPECT_EQ(mario.soakedTimer, 149);
        }
    }
    return TRUE;
}

static int water_bomb_soaks_on_hit(void) {
    begin(bhvWaterBomb);
    enemy.oAction = WATER_BOMB_ACT_DROP;
    enemy.oDamageOrCoinValue = 1;
    mario.flags = MARIO_CAP_ON_HEAD;
    EXPECT_EQ(interact_mr_blizzard(&mario, INTERACT_MR_BLIZZARD, &enemy), TRUE);
    EXPECT_EQ(mario.soakedTimer, 150);
    EXPECT_EQ(mario_is_soaked(&mario), TRUE);

    begin(bhvWaterBomb);
    enemy.oAction = WATER_BOMB_ACT_DROP;
    enemy.oDamageOrCoinValue = 1;
    mario.flags = MARIO_CAP_ON_HEAD;
    sInvulnerable = TRUE;
    EXPECT_EQ(interact_mr_blizzard(&mario, INTERACT_MR_BLIZZARD, &enemy), FALSE);
    EXPECT_EQ(mario.soakedTimer, 0);

    begin(bhvWaterBomb);
    enemy.oAction = WATER_BOMB_ACT_DROP;
    enemy.oDamageOrCoinValue = 1;
    mario.flags = MARIO_CAP_ON_HEAD | MARIO_VANISH_CAP;
    EXPECT_EQ(interact_mr_blizzard(&mario, INTERACT_MR_BLIZZARD, &enemy), FALSE);
    EXPECT_EQ(mario.soakedTimer, 0);

    begin(bhvWaterBomb);
    enemy.oAction = 0;
    enemy.oDamageOrCoinValue = 1;
    mario.flags = MARIO_CAP_ON_HEAD;
    EXPECT_EQ(interact_mr_blizzard(&mario, INTERACT_MR_BLIZZARD, &enemy), TRUE);
    EXPECT_EQ(mario.soakedTimer, 0);

    begin(bhvMadPiano);
    enemy.oDamageOrCoinValue = 1;
    mario.flags = MARIO_CAP_ON_HEAD;
    EXPECT_EQ(interact_mr_blizzard(&mario, INTERACT_MR_BLIZZARD, &enemy), TRUE);
    EXPECT_EQ(mario.soakedTimer, 0);
    return TRUE;
}
static int shell_protection(void) {
    struct Object shell = { 0 };
    const BehaviorScript *const behaviors[] = { bhvGoomba, bhvBowserBodyAnchor, bhvBowserBomb };
    for (int master = 0; master <= 1; master++) {
        for (int keep = 0; keep <= 1; keep++) {
            sTestGameOptions[GO_CHEATS_ENABLED] = master;
            sTestGameOptions[GO_KEEP_SHELL] = keep;
            sTestGameOptions[GO_KNOCKBACK] = FALSE;
            for (int kind = 0; kind < 3; kind++) {
                for (unsigned i = 0; i < sizeof(behaviors) / sizeof(*behaviors); i++) {
                    begin(behaviors[i]);
                    u32 riding = kind == 0 ? ACT_RIDING_SHELL_GROUND : ACT_WATER_SHELL_SWIMMING;
                    mario.action = kind == 2 ? ACT_HOLD_WALKING : riding;
                    shell.behavior = kind == 0 ? bhvKoopaShell : bhvKoopaShellUnderwater;
                    mario.riddenObj = kind == 0 ? &shell : NULL;
                    mario.heldObj = kind != 0 ? &shell : NULL;
                    mario.flags = MARIO_CAP_ON_HEAD | MARIO_NORMAL_CAP;
                    mario.forwardVel = 12;
                    enemy.oDamageOrCoinValue = 2;
                    if (i != 0) {
                        enemy.oInteractionSubtype |= INT_SUBTYPE_BOWSER_ATTACK;
                    }
                    EXPECT_EQ(take_damage_and_knock_back(&mario, &enemy), TRUE);
                    EXPECT_EQ(mario.hurtCounter > 0, TRUE); // Protection never grants health.
                    if (master && keep && kind < 2) {
                        EXPECT_EQ(mario.action, riding);
                        EXPECT_EQ(mario.forwardVel, 12);
                        EXPECT_EQ(kind == 0 ? mario.riddenObj == &shell : mario.heldObj == &shell,
                                  TRUE);
                    } else {
                        EXPECT_EQ(mario_is_knocked_back(mario.action), TRUE);
                    }
                }
            }
        }
    }
    sTestGameOptions[GO_CHEATS_ENABLED] = sTestGameOptions[GO_KEEP_SHELL] = TRUE;
    begin(bhvGoomba);
    mario.action = ACT_RIDING_SHELL_FALL;
    shell.behavior = bhvKoopaShell;
    mario.riddenObj = &shell;
    mario.flags = MARIO_CAP_ON_HEAD;
    mario.forwardVel = 12;
    mario.vel[1] = 10;
    mario.faceAngle[1] = 50;
    mario.wallYaw = 200;
    EXPECT_EQ(lava_boost_on_wall(&mario), FALSE);
    EXPECT_EQ(mario.action, ACT_RIDING_SHELL_FALL);
    EXPECT_EQ(mario.riddenObj == &shell, TRUE);
    EXPECT_EQ(mario.forwardVel, 12);
    EXPECT_EQ(mario.vel[1], 10);
    EXPECT_EQ(mario.faceAngle[1], 50);
    EXPECT_EQ(mario.hurtCounter > 0, TRUE);

    begin(bhvChuckya);
    mario.action = ACT_WATER_SHELL_SWIMMING;
    shell.behavior = bhvKoopaShellUnderwater;
    mario.heldObj = &shell;
    enemy.oInteractionSubtype = INT_SUBTYPE_GRABS_MARIO;
    EXPECT_EQ(check_object_grab_mario(&mario, INTERACT_GRABBABLE, &enemy), FALSE);
    EXPECT_EQ(mario.heldObj == &shell, TRUE);
    mario.flags = MARIO_CAP_ON_HEAD;
    enemy.oDamageOrCoinValue = 2;
    EXPECT_EQ(interact_shock(&mario, INTERACT_SHOCK, &enemy), TRUE);
    EXPECT_EQ(mario.action, ACT_WATER_SHELL_SWIMMING);
    EXPECT_EQ(mario.heldObj == &shell, TRUE);
    EXPECT_EQ(mario.hurtCounter > 0, TRUE);
    sTestGameOptions[GO_CHEATS_ENABLED] = sTestGameOptions[GO_KEEP_SHELL] = FALSE;
    return TRUE;
}

static int disable_knockback(void) {
    struct Object other = { 0 };
    const BehaviorScript *const behaviors[] = { bhvGoomba, bhvBowserBodyAnchor, bhvBowserBomb };
    sTestGameOptions[GO_CHEATS_ENABLED] = TRUE;
    for (int combination = 0; combination < 4; combination++) {
        sTestGameOptions[GO_INFINITE_HEALTH] = (combination & 1) != 0;
        sTestGameOptions[GO_KNOCKBACK] = (combination & 2) != 0;
        for (unsigned i = 0; i < sizeof(behaviors) / sizeof(*behaviors); i++) {
            begin(behaviors[i]);
            mario.flags = MARIO_CAP_ON_HEAD | MARIO_NORMAL_CAP;
            mario.forwardVel = 12;
            mario.vel[1] = -6;
            mario.heldObj = &other;
            enemy.oDamageOrCoinValue = 2;
            if (i != 0) {
                enemy.oInteractionSubtype |= INT_SUBTYPE_BOWSER_ATTACK;
            }
            EXPECT_EQ(take_damage_and_knock_back(&mario, &enemy), TRUE);
            EXPECT_EQ(mario.hurtCounter > 0, !(combination & 1));
            EXPECT_EQ(mario_is_knocked_back(mario.action), !(combination & 2));
            if (combination & 2) {
                EXPECT_EQ(mario.action, ACT_WALKING);
                EXPECT_EQ(mario.forwardVel, 12);
                EXPECT_EQ(mario.vel[1], -6);
                EXPECT_EQ(mario.heldObj, &other);
            }
        }
    }
    sTestGameOptions[GO_INFINITE_HEALTH] = FALSE;
    for (int original = 0; original < 2; original++) {
        begin(bhvGoomba);
        mario.flags = MARIO_CAP_ON_HEAD;
        mario.forwardVel = 12;
        mario.marioObj->oInteractStatus = INT_STATUS_MARIO_KNOCKBACK_DMG;
        sTestGameOptions[GO_BOWSER_FIGHTS] = original;
        EXPECT_EQ(mario_bowser_impact_knockback(&mario, FALSE), FALSE);
        EXPECT_EQ(mario.action, ACT_WALKING);
        EXPECT_EQ(mario.forwardVel, 12);
        EXPECT_EQ(mario.hurtCounter, original ? 12 : 8);
    }
    sTestGameOptions[GO_BOWSER_FIGHTS] = 0;
    begin(bhvGoomba);
    mario.flags = MARIO_CAP_ON_HEAD;
    enemy.oDamageOrCoinValue = 2;
    enemy.oInteractType = INTERACT_SHOCK;
    EXPECT_EQ(interact_shock(&mario, INTERACT_SHOCK, &enemy), TRUE);
    EXPECT_EQ(mario.action, ACT_WALKING);
    EXPECT_EQ(mario.hurtCounter > 0, TRUE);
    begin(bhvGoomba);
    mario.flags = MARIO_CAP_ON_HEAD;
    floorSurface.type = SURFACE_BURNING;
    check_lava_boost(&mario);
    EXPECT_EQ(mario.action, ACT_WALKING);
    EXPECT_EQ(mario.hurtCounter, 12);
    check_lava_boost(&mario);
    EXPECT_EQ(mario.hurtCounter, 12);
    begin(bhvGoomba);
    mario.flags = MARIO_CAP_ON_HEAD;
    mario.action = ACT_FREEFALL;
    mario.forwardVel = 10;
    mario.faceAngle[1] = 0;
    mario.wallYaw = 0x4000;
    EXPECT_EQ(lava_boost_on_wall(&mario), FALSE);
    EXPECT_EQ(mario.action, ACT_FREEFALL);
    EXPECT_EQ(mario.faceAngle[1], 0);
    EXPECT_EQ(mario.forwardVel, 10);
    EXPECT_EQ(mario.hurtCounter, 12);
    lava_boost_on_wall(&mario);
    EXPECT_EQ(mario.hurtCounter, 12);
    for (s32 large = 0; large < 2; large++) {
        begin(bhvGoomba);
        mario.flags = MARIO_CAP_ON_HEAD | MARIO_NORMAL_CAP;
        mario.action = ACT_FREEFALL;
        mario.peakHeight = large ? 5000 : 2000;
        mario.vel[1] = -100;
        EXPECT_EQ(check_fall_damage(&mario, ACT_HARD_BACKWARD_GROUND_KB), FALSE);
        EXPECT_EQ(mario.action, ACT_FREEFALL);
        EXPECT_EQ(mario.hurtCounter, large ? 16 : 8);
        EXPECT_EQ(mario.squishTimer, 0);
    }
    begin(bhvHeaveHo);
    enemy.oInteractionSubtype |= INT_SUBTYPE_GRABS_MARIO;
    EXPECT_EQ(check_object_grab_mario(&mario, INTERACT_GRABBABLE, &enemy), FALSE);
    EXPECT_EQ(enemy.oInteractStatus, 0);
    sTestGameOptions[GO_CHEATS_ENABLED] = FALSE;
    begin(bhvGoomba);
    mario.flags = MARIO_CAP_ON_HEAD;
    enemy.oDamageOrCoinValue = 2;
    EXPECT_EQ(take_damage_and_knock_back(&mario, &enemy), TRUE);
    EXPECT_EQ(mario_is_knocked_back(mario.action), TRUE);
    sTestGameOptions[GO_KNOCKBACK] = FALSE;
    return TRUE;
}

static int bowser_no_knockback_progress(void) {
    const u32 impacts[] = { INT_STATUS_MARIO_STUNNED, INT_STATUS_MARIO_KNOCKBACK_DMG,
                            INT_STATUS_MARIO_SHOCKWAVE };
    sTestGameOptions[GO_CHEATS_ENABLED] = TRUE;
    sTestGameOptions[GO_KNOCKBACK] = TRUE;
    sTestGameOptions[GO_BOWSER_FIGHTS] = 0;
    for (int health = 0; health < 2; health++) {
        sTestGameOptions[GO_INFINITE_HEALTH] = health;
        for (unsigned i = 0; i < sizeof(impacts) / sizeof(*impacts); i++) {
            begin(bhvGoomba);
            mario.flags = MARIO_CAP_ON_HEAD;
            mario.action = ACT_SHOCKWAVE_BOUNCE;
            mario.input = INPUT_STOMPED | INPUT_A_DOWN;
            mario.marioObj->oInteractStatus = impacts[i] | INT_STATUS_MARIO_DROPPED_BY_OBJ;
            unsigned repeats = 0;
            while (repeats < 16 && act_shockwave_bounce(&mario)) {
                repeats++;
            }
            EXPECT_EQ(repeats < 16, TRUE);
            EXPECT_EQ(mario.action, ACT_SHOCKWAVE_BOUNCE);
            EXPECT_EQ(mario.input, INPUT_A_DOWN);
            EXPECT_EQ(mario.marioObj->oInteractStatus, INT_STATUS_MARIO_DROPPED_BY_OBJ);
            EXPECT_EQ(mario.invincTimer, 30);
            EXPECT_EQ(mario.hurtCounter, health ? 0 : (i == 1 ? 8 : 4));
            u8 hurt = mario.hurtCounter;
            EXPECT_EQ(act_shockwave_bounce(&mario), FALSE);
            EXPECT_EQ(mario.actionTimer, 1);
            EXPECT_EQ(mario.hurtCounter, hurt);
        }
    }
    sTestGameOptions[GO_INFINITE_HEALTH] = FALSE;
    sTestGameOptions[GO_KNOCKBACK] = FALSE;
    return TRUE;
}

static int bowser_queued_no_knockback(void) {
    const u32 impacts[] = { INT_STATUS_MARIO_STUNNED, INT_STATUS_MARIO_KNOCKBACK_DMG,
                            INT_STATUS_MARIO_SHOCKWAVE };
    const u32 actions[] = { ACT_IDLE, ACT_WALKING };
    sTestGameOptions[GO_CHEATS_ENABLED] = TRUE;
    sTestGameOptions[GO_KNOCKBACK] = TRUE;
    for (int original = 0; original < 2; original++) {
        sTestGameOptions[GO_BOWSER_FIGHTS] = original;
        for (int health = 0; health < 2; health++) {
            sTestGameOptions[GO_INFINITE_HEALTH] = health;
            for (unsigned action = 0; action < sizeof(actions) / sizeof(*actions); action++) {
                for (unsigned impact = 0; impact < sizeof(impacts) / sizeof(*impacts); impact++) {
                    begin(bhvGoomba);
                    mario.flags = MARIO_CAP_ON_HEAD | MARIO_NORMAL_CAP;
                    mario.action = actions[action];
                    mario.forwardVel = 12;
                    mario.vel[1] = -6;
                    mario.heldObj = &parent;
                    mario.input = INPUT_STOMPED | INPUT_A_DOWN;
                    mario.marioObj->oInteractStatus = impacts[impact] | INT_STATUS_MARIO_DROPPED_BY_OBJ;
                    process_queued_bowser_impacts(&mario);
                    EXPECT_EQ(mario.action, actions[action]);
                    EXPECT_EQ(mario.forwardVel, 12);
                    EXPECT_EQ(mario.vel[1], -6);
                    EXPECT_EQ(mario.heldObj, &parent);
                    EXPECT_EQ(mario.input, INPUT_A_DOWN);
                    EXPECT_EQ(mario.marioObj->oInteractStatus, INT_STATUS_MARIO_DROPPED_BY_OBJ);
                    EXPECT_EQ(mario.invincTimer, 30);
                    s32 hurt = impact == 1 ? 8 : 4;
                    if (original) {
                        hurt = 0;
                        if (impact == 2) {
                            hurt = 4;
                        } else if (impact == 1) {
                            hurt = 12;
                        }
                    }
                    EXPECT_EQ(mario.hurtCounter, health ? 0 : hurt);
                }
            }
        }
    }
    sTestGameOptions[GO_INFINITE_HEALTH] = FALSE;
    sTestGameOptions[GO_KNOCKBACK] = FALSE;
    sTestGameOptions[GO_BOWSER_FIGHTS] = 0;
    return TRUE;
}

static int infinite_health_reactions(void) {
    begin(bhvGoomba);
    mario.flags = MARIO_CAP_ON_HEAD;
    enemy.oDamageOrCoinValue = 4;
    sTestGameOptions[GO_CHEATS_ENABLED] = TRUE;
    sTestGameOptions[GO_INFINITE_HEALTH] = TRUE;
    EXPECT_EQ(take_damage_and_knock_back(&mario, &enemy), TRUE);
    EXPECT_EQ(mario.hurtCounter, 0);
    EXPECT_EQ(mario.invincTimer, 30);
    EXPECT_EQ(mario_is_knocked_back(mario.action), TRUE);
    EXPECT_EQ(enemy.oInteractStatus & INT_STATUS_ATTACKED_MARIO, INT_STATUS_ATTACKED_MARIO);
    begin(bhvGoomba);
    mario.flags = MARIO_CAP_ON_HEAD;
    mario.interactObj = &enemy;
    enemy.oDamageOrCoinValue = 4;
    sTestGameOptions[GO_CHEATS_ENABLED] = FALSE;
    EXPECT_EQ(take_damage_from_interact_object(&mario), 4);
    EXPECT_EQ(mario.hurtCounter, 16);
    sTestGameOptions[GO_INFINITE_HEALTH] = FALSE;
    return TRUE;
}

static int environment_bypasses(void) {
    sTestGameOptions[GO_CHEATS_ENABLED] = TRUE;
    sTestGameOptions[GO_IGNORE_WIND] = TRUE;
    sTestGameOptions[GO_IGNORE_SAND] = TRUE;
    sTestGameOptions[GO_IGNORE_CURRENTS] = TRUE;
    for (int hazard = 0; hazard < 3; hazard++) {
        begin(bhvGoomba);
        mario.flags = 0;
        mario.heldObj = &parent;
        mario.forwardVel = 32;
        mario.vel[1] = -12;
        u32 result = hazard == 0   ? interact_tornado(&mario, INTERACT_TORNADO, &enemy)
                     : hazard == 1 ? interact_strong_wind(&mario, INTERACT_STRONG_WIND, &enemy)
                                   : interact_whirlpool(&mario, INTERACT_WHIRLPOOL, &enemy);
        EXPECT_EQ(result, FALSE);
        EXPECT_EQ(mario.action, ACT_WALKING);
        EXPECT_EQ(mario.heldObj, &parent);
        EXPECT_EQ(mario.forwardVel, 32);
        EXPECT_EQ(mario.vel[1], -12);
        EXPECT_EQ(enemy.oInteractStatus, 0);
    }
    const s16 sand[] = { SURFACE_SHALLOW_QUICKSAND, SURFACE_SHALLOW_MOVING_QUICKSAND,
                         SURFACE_QUICKSAND,         SURFACE_MOVING_QUICKSAND,
                         SURFACE_DEEP_QUICKSAND,    SURFACE_DEEP_MOVING_QUICKSAND,
                         SURFACE_INSTANT_QUICKSAND, SURFACE_INSTANT_MOVING_QUICKSAND };
    for (unsigned i = 0; i < sizeof(sand) / sizeof(*sand); i++) {
        begin(bhvGoomba);
        mario.flags = 0;
        floorSurface.type = sand[i];
        mario.quicksandDepth = 159.9f;
        EXPECT_EQ(mario_update_quicksand(&mario, 1), FALSE);
        EXPECT_EQ(mario.quicksandDepth, 0);
        EXPECT_EQ(mario.action, ACT_WALKING);
        mario.vel[2] = 6;
        EXPECT_EQ(mario_update_moving_sand(&mario), FALSE);
        EXPECT_EQ(mario.vel[2], 6);
    }
    for (int snow = 0; snow < 2; snow++) {
        begin(bhvGoomba);
        mario.flags = 0;
        area.terrainType = snow ? TERRAIN_SNOW : TERRAIN_SAND;
        mario.peakHeight = 2000;
        EXPECT_EQ(should_get_stuck_in_ground(&mario), snow);
    }
    begin(bhvGoomba);
    mario.flags = 0;
    floorSurface.type = SURFACE_HORIZONTAL_WIND;
    mario.vel[2] = 6;
    EXPECT_EQ(mario_update_windy_ground(&mario), FALSE);
    EXPECT_EQ(mario.vel[2], 6);
    // The same surface has a separate airborne force path.
    mario.action = ACT_FREEFALL;
    mario.slideVelX = 5;
    mario.slideVelZ = 6;
    mario.vel[0] = 7;
    mario.vel[2] = 8;
    mario.forwardVel = 9;
    mario.slideYaw = 0x1234;
    EXPECT_EQ(check_horizontal_wind(&mario), FALSE);
    EXPECT_EQ(mario.slideVelX, 5);
    EXPECT_EQ(mario.slideVelZ, 6);
    EXPECT_EQ(mario.vel[0], 7);
    EXPECT_EQ(mario.vel[2], 8);
    EXPECT_EQ(mario.forwardVel, 9);
    EXPECT_EQ(mario.slideYaw, 0x1234);
    sTestGameOptions[GO_CHEATS_ENABLED] = FALSE;
    EXPECT_EQ(check_horizontal_wind(&mario), TRUE);
    EXPECT_EQ(mario.slideVelZ > 6, TRUE);
    sTestGameOptions[GO_CHEATS_ENABLED] = TRUE;
    floorSurface.type = SURFACE_VERTICAL_WIND;
    mario.floorHeight = -3000;
    mario.pos[1] = -930;
    mario.action = ACT_FREEFALL;
    mario.vel[1] = -80;
    mario.windCatchActive = TRUE;
    Vec3f next = { 0, -950, 0 };
    EXPECT_EQ(mario_is_in_vertical_wind(&mario), FALSE);
    constrain_wind_descent(&mario, next, &floorSurface, mario.floorHeight);
    EXPECT_EQ(next[1], -950);
    EXPECT_EQ(mario.vel[1], -80);
    apply_vertical_wind(&mario, -76);
    EXPECT_EQ(mario.windCatchActive, FALSE);
    EXPECT_EQ(mario.vel[1], -80);
    sTestGameOptions[GO_CHEATS_ENABLED] = FALSE;
    EXPECT_EQ(mario_is_in_vertical_wind(&mario), TRUE);
    begin(bhvGoomba);
    mario.flags = 0;
    floorSurface.type = SURFACE_INSTANT_QUICKSAND;
    EXPECT_EQ(mario_update_quicksand(&mario, 1), TRUE);
    EXPECT_EQ(mario.action, ACT_QUICKSAND_DEATH);
    sTestGameOptions[GO_IGNORE_WIND] = FALSE;
    sTestGameOptions[GO_IGNORE_SAND] = FALSE;
    sTestGameOptions[GO_IGNORE_CURRENTS] = FALSE;
    return TRUE;
}

static int wind_immunity(void) {
    for (int gust = 0; gust < 2; gust++) {
        begin(bhvGoomba);
        mario.heldObj = &parent;
        mario.forwardVel = 32;
        mario.vel[1] = -12;
        EXPECT_EQ(gust ? interact_strong_wind(&mario, INTERACT_STRONG_WIND, &enemy)
                       : interact_tornado(&mario, INTERACT_TORNADO, &enemy),
                  FALSE);
        EXPECT_EQ(mario.action, ACT_WALKING);
        EXPECT_EQ(mario.heldObj, &parent);
        EXPECT_EQ(mario.forwardVel, 32);
        EXPECT_EQ(mario.vel[1], -12);
        EXPECT_EQ(enemy.oInteractStatus, 0);
        mario.flags = MARIO_CAP_ON_HEAD;
        EXPECT_EQ(gust ? interact_strong_wind(&mario, INTERACT_STRONG_WIND, &enemy)
                       : interact_tornado(&mario, INTERACT_TORNADO, &enemy),
                  TRUE);
        EXPECT_EQ(mario.action, gust ? ACT_GETTING_BLOWN : ACT_TORNADO_TWIRLING);
        EXPECT_EQ(mario.heldObj, NULL);
        mario.flags |= MARIO_METAL_CAP;
        mario.windCatchActive = TRUE;
        EXPECT_EQ(gust ? check_common_airborne_cancels(&mario) : check_common_automatic_cancels(&mario),
                  TRUE);
        EXPECT_EQ(mario.action, ACT_FREEFALL);
    }
    begin(bhvGoomba);
    floorSurface.type = SURFACE_HORIZONTAL_WIND;
    mario.action = ACT_IDLE;
    mario.vel[0] = 5;
    mario.vel[2] = 6;
    EXPECT_EQ(mario_update_windy_ground(&mario), FALSE);
    EXPECT_EQ(mario.vel[0], 5);
    EXPECT_EQ(mario.vel[2], 6);
    mario.flags = 0;
    EXPECT_EQ(mario_update_windy_ground(&mario), TRUE);
    EXPECT_EQ(mario.vel[2] > 6, TRUE);
    begin(bhvGoomba);
    floorSurface.type = SURFACE_HORIZONTAL_WIND;
    mario.action = ACT_FREEFALL;
    mario.slideVelX = 5;
    mario.slideVelZ = 6;
    mario.vel[0] = 7;
    mario.vel[2] = 8;
    mario.forwardVel = 9;
    EXPECT_EQ(check_horizontal_wind(&mario), FALSE);
    EXPECT_EQ(mario.vel[0], 7);
    EXPECT_EQ(mario.vel[2], 8);
    EXPECT_EQ(mario.slideVelX, 5);
    EXPECT_EQ(mario.slideVelZ, 6);
    EXPECT_EQ(mario.forwardVel, 9);
    mario.flags = 0;
    EXPECT_EQ(check_horizontal_wind(&mario), TRUE);
    EXPECT_EQ(mario.slideVelZ > 6, TRUE);
    begin(bhvGoomba);
    floorSurface.type = SURFACE_VERTICAL_WIND;
    mario.floorHeight = -3000;
    mario.action = ACT_FREEFALL;
    mario.pos[1] = -930;
    mario.vel[1] = -80;
    mario.windCatchActive = TRUE;
    Vec3f next = { 0, -950, 0 };
    EXPECT_EQ(mario_is_in_vertical_wind(&mario), FALSE);
    constrain_wind_descent(&mario, next, &floorSurface, mario.floorHeight);
    EXPECT_EQ(next[1], -950);
    EXPECT_EQ(mario.vel[1], -80);
    apply_vertical_wind(&mario, -76);
    EXPECT_EQ(mario.vel[1], -80);
    EXPECT_EQ(mario.windCatchActive, FALSE);
    mario.flags = 0;
    EXPECT_EQ(mario_is_in_vertical_wind(&mario), TRUE);
    constrain_wind_descent(&mario, next, &floorSurface, mario.floorHeight);
    EXPECT_EQ(next[1] > -950, TRUE);
    EXPECT_EQ(mario.vel[1] > -80, TRUE);
    EXPECT_EQ(mario.windCatchActive, TRUE);
    f32 before = mario.vel[1];
    apply_vertical_wind(&mario, before);
    EXPECT_EQ(mario.vel[1] > before, TRUE);
    mario.action = ACT_VERTICAL_WIND;
    mario.flags = MARIO_METAL_CAP;
    EXPECT_EQ(check_common_airborne_cancels(&mario), TRUE);
    EXPECT_EQ(mario.action, ACT_FREEFALL);
    EXPECT_EQ(mario.windCatchActive, FALSE);
    return TRUE;
}
static int solid_lava(void) {
    u32 actions[] = { ACT_IDLE,     ACT_WALKING,    ACT_CROUCHING,
                      ACT_CRAWLING, ACT_BUTT_SLIDE, ACT_SLIDE_KICK_SLIDE };
    for (unsigned a = 0; a < sizeof(actions) / sizeof(*actions); a++) {
        begin(bhvGoomba);
        floorSurface.type = SURFACE_BURNING;
        mario.action = actions[a];
        mario.forwardVel = 32;
        check_lava_boost(&mario);
        EXPECT_EQ(mario.action, actions[a]);
        EXPECT_EQ(mario.hurtCounter, 0);
        EXPECT_EQ(mario.forwardVel, 32);
        mario.flags = MARIO_CAP_ON_HEAD;
        check_lava_boost(&mario);
        EXPECT_EQ(mario.action, ACT_LAVA_BOOST);
        EXPECT_EQ(mario.hurtCounter, 12);
        mario.flags |= MARIO_METAL_CAP;
        EXPECT_EQ(check_common_airborne_cancels(&mario), TRUE);
        EXPECT_EQ(mario.action, ACT_FREEFALL);
    }
    begin(bhvGoomba);
    mario.action = ACT_LAVA_BOOST;
    mario.actionArg = 1;
    EXPECT_EQ(check_common_airborne_cancels(&mario), FALSE);
    EXPECT_EQ(mario.action, ACT_LAVA_BOOST);
    return TRUE;
}
static int soaked_lava(void) {
    begin(bhvGoomba);
    floorSurface.type = SURFACE_BURNING;
    mario.flags = MARIO_NORMAL_CAP | MARIO_CAP_ON_HEAD;
    mario.soakedTimer = 150;
    mario.soakedThisUpdate = TRUE;
    check_lava_boost(&mario);
    EXPECT_EQ(mario.action, ACT_LAVA_BOOST);
    EXPECT_EQ(mario.hurtCounter, 0);
    EXPECT_EQ(mario.soakedThisUpdate, FALSE);
    EXPECT_EQ(mario.soakedTimer, 0);
    mario.vel[1] = -20;
    suppliedAirStep = AIR_STEP_LANDED;
    suppliedStepY = 0;
    act_lava_boost(&mario);
    EXPECT_EQ(mario.hurtCounter, 12);
    mario.vel[1] = -20;
    act_lava_boost(&mario);
    EXPECT_EQ(mario.hurtCounter, 24);
    mario.wallYaw = 0x2000;
    lava_boost_on_wall(&mario);
    EXPECT_EQ(mario.hurtCounter, 36);
    mario.action = ACT_WALKING;
    check_lava_boost(&mario);
    EXPECT_EQ(mario.hurtCounter, 48);

    begin(bhvGoomba);
    floorSurface.type = SURFACE_BURNING;
    mario.flags = MARIO_METAL_CAP | MARIO_NORMAL_CAP | MARIO_CAP_ON_HEAD;
    mario.soakedTimer = 150;
    mario.soakedThisUpdate = TRUE;
    check_lava_boost(&mario);
    EXPECT_EQ(mario.action, ACT_WALKING);
    EXPECT_EQ(mario.hurtCounter, 0);
    EXPECT_EQ(mario.soakedThisUpdate, FALSE);

    begin(bhvGoomba);
    mario.flags = MARIO_NORMAL_CAP | MARIO_CAP_ON_HEAD;
    mario.soakedTimer = 150;
    mario.soakedThisUpdate = TRUE;
    mario.wallYaw = 0x2000;
    EXPECT_EQ(lava_boost_on_wall(&mario), TRUE);
    EXPECT_EQ(mario.action, ACT_LAVA_BOOST);
    EXPECT_EQ(mario.hurtCounter, 0);
    EXPECT_EQ(mario.faceAngle[1], 0x2000);
    EXPECT_EQ(mario.soakedTimer, 0);
    EXPECT_EQ(lava_boost_on_wall(&mario), TRUE);
    EXPECT_EQ(mario.hurtCounter, 12);
    begin(bhvGoomba);
    mario.flags = MARIO_NORMAL_CAP | MARIO_CAP_ON_HEAD;
    EXPECT_EQ(lava_boost_on_wall(&mario), TRUE);
    EXPECT_EQ(mario.hurtCounter, 12);

    begin(bhvGoomba);
    floorSurface.type = SURFACE_BURNING;
    mario.flags = MARIO_NORMAL_CAP | MARIO_CAP_ON_HEAD;
    mario.action = ACT_LAVA_BOOST;
    mario.soakedTimer = 150;
    mario.soakedThisUpdate = TRUE;
    mario.vel[1] = -20;
    suppliedAirStep = AIR_STEP_LANDED;
    suppliedStepY = 0;
    EXPECT_EQ(act_lava_boost(&mario), FALSE);
    EXPECT_EQ(mario.hurtCounter, 0);
    EXPECT_EQ(mario.soakedTimer, 0);
    mario.vel[1] = -20;
    EXPECT_EQ(act_lava_boost(&mario), FALSE);
    EXPECT_EQ(mario.hurtCounter, 12);
    mario.vel[1] = -20;
    EXPECT_EQ(act_lava_boost(&mario), FALSE);
    EXPECT_EQ(mario.hurtCounter, 24);
    return TRUE;
}
static int soaked_cold_surface(void) {
    for (int landing = 0; landing < 2; landing++) {
        for (int cap = 0; cap < 3; cap++) {
            begin(bhvGoomba);
            floorSurface.type = SURFACE_BURNING;
            area.terrainType = TERRAIN_SNOW;
            if (cap == 0) {
                mario.flags = MARIO_CAP_ON_HEAD;
            } else if (cap == 1) {
                mario.flags = 0;
            } else {
                mario.flags = MARIO_METAL_CAP;
            }
            for (int contact = 0; contact < 3; contact++) {
                mario.waterLevel = 0;
                mario.pos[1] = 0;
                mario_update_soaked_status(&mario);
                EXPECT_EQ(mario_is_soaked(&mario), TRUE);
                if (landing) {
                    mario.action = ACT_LAVA_BOOST;
                    mario.vel[1] = -20;
                    suppliedAirStep = AIR_STEP_LANDED;
                    suppliedStepY = 0;
                    act_lava_boost(&mario);
                } else {
                    mario.action = ACT_WALKING;
                    check_lava_boost(&mario);
                }
                int expectedDamage = 0;
                if (cap != 2) {
                    expectedDamage = (contact + 1) * (cap == 0 ? 12 : 24);
                }
                EXPECT_EQ(mario.hurtCounter, expectedDamage);
                EXPECT_EQ(mario_is_soaked(&mario), TRUE);
                EXPECT_EQ(mario.soakedTimer, 150);
            }
        }
    }
    return TRUE;
}
static int ground_pound_lava(void) {
    for (int spin = 0; spin < 2; spin++) {
        for (int triple = 0; triple < 2; triple++) {
            for (int cycle = 0; cycle < 3; cycle++) {
                begin(bhvGoomba);
                floorSurface.type = SURFACE_BURNING;
                mario.action = ACT_GROUND_POUND;
                mario.actionState = spin ? ACT_STATE_GROUND_POUND_SPIN : ACT_STATE_GROUND_POUND_FALL;
                mario.actionArg =
                    triple ? ACT_ARG_GROUND_POUND_FROM_TRIPLE_JUMP : ACT_ARG_GROUND_POUND_NORMAL;
                mario.forwardVel = 10;
                mario.vel[1] = -50;
                mario.pos[1] = 10;
                suppliedStepY = 0;
                suppliedAirStep = AIR_STEP_LANDED;
                EXPECT_EQ(act_ground_pound(&mario), FALSE);
                EXPECT_EQ(groundPoundImpacts, 1);
                EXPECT_EQ(mario.action, ACT_LAVA_BOOST);
                EXPECT_EQ(mario.actionArg, ACT_ARG_LAVA_BOOST_METAL_GROUND_POUND);
                EXPECT_EQ(mario.vel[1], 84);
                EXPECT_EQ(mario.hurtCounter, 0);
                EXPECT_EQ(mario.forwardVel, 0);
                // The intentional bounce must survive both the airborne prepass and the next update.
                EXPECT_EQ(check_common_airborne_cancels(&mario), FALSE);
                suppliedAirStep = AIR_STEP_NONE;
                suppliedStepY = 84;
                EXPECT_EQ(act_lava_boost(&mario), FALSE);
                EXPECT_EQ(mario.action, ACT_LAVA_BOOST);
                EXPECT_EQ(mario.vel[1], 84);
                EXPECT_EQ(mario.hurtCounter, 0);
                EXPECT_EQ(mario.particleFlags & PARTICLE_FIRE, 0);
                // Its return to lava has the same settling bounces as ordinary solid ground.
                suppliedAirStep = AIR_STEP_LANDED;
                suppliedStepY = 0;
                for (int landing = 0; landing < 3; landing++) {
                    mario.vel[1] = -20;
                    act_lava_boost(&mario);
                    EXPECT_EQ(mario.hurtCounter, 0);
                    if (landing < 2) {
                        EXPECT_EQ(mario.action, ACT_LAVA_BOOST);
                        EXPECT_EQ(mario.vel[1], 8);
                    }
                }
                EXPECT_EQ(mario.action, ACT_LAVA_BOOST_LAND);
                check_lava_boost(&mario);
                EXPECT_EQ(mario.action, ACT_LAVA_BOOST_LAND);
                EXPECT_EQ(mario.hurtCounter, 0);
            }
        }
    }
    begin(bhvGoomba);
    mario.action = ACT_GROUND_POUND;
    mario.actionState = ACT_STATE_GROUND_POUND_FALL;
    suppliedAirStep = AIR_STEP_LANDED;
    suppliedStepY = 0;
    act_ground_pound(&mario);
    EXPECT_EQ(mario.action, ACT_GROUND_POUND_LAND);
    EXPECT_EQ(mario.hurtCounter, 0);
    begin(bhvGoomba);
    mario.action = ACT_GROUND_POUND;
    mario.actionState = ACT_STATE_GROUND_POUND_FALL;
    floorSurface.type = SURFACE_BURNING;
    suppliedAirStep = AIR_STEP_NONE;
    suppliedStepY = 10;
    act_ground_pound(&mario);
    EXPECT_EQ(mario.action, ACT_GROUND_POUND);
    EXPECT_EQ(groundPoundImpacts, 0);
    // Without Metal, the ordinary landing/floor-hazard sequence still applies.
    begin(bhvGoomba);
    mario.flags = MARIO_CAP_ON_HEAD;
    mario.action = ACT_GROUND_POUND;
    mario.actionState = ACT_STATE_GROUND_POUND_FALL;
    floorSurface.type = SURFACE_BURNING;
    suppliedAirStep = AIR_STEP_LANDED;
    suppliedStepY = 0;
    act_ground_pound(&mario);
    EXPECT_EQ(mario.action, ACT_GROUND_POUND_LAND);
    check_lava_boost(&mario);
    EXPECT_EQ(mario.action, ACT_LAVA_BOOST);
    EXPECT_EQ(mario.hurtCounter, 12);
    return TRUE;
}

static int held_a_lava_bounce(void) {
    const u32 caps[] = { MARIO_METAL_CAP, MARIO_METAL_CAP | MARIO_CAP_ON_HEAD,
                         MARIO_METAL_CAP | MARIO_VANISH_CAP, MARIO_METAL_CAP | MARIO_WING_CAP };
    for (unsigned cap = 0; cap < sizeof(caps) / sizeof(caps[0]); cap++) {
        begin(bhvGoomba);
        mario.flags = caps[cap];
        floorSurface.type = SURFACE_BURNING;
        mario.action = ACT_GROUND_POUND;
        mario.actionState = ACT_STATE_GROUND_POUND_FALL;
        mario.input = INPUT_A_DOWN;
        mario.vel[1] = -50;
        suppliedAirStep = AIR_STEP_LANDED;
        suppliedStepY = 0;
        EXPECT_EQ(act_ground_pound(&mario), FALSE);
        EXPECT_EQ(mario.action, ACT_LAVA_BOOST);
        EXPECT_EQ(mario.actionArg, ACT_ARG_LAVA_BOOST_METAL_GROUND_POUND);
        EXPECT_EQ(mario.vel[1], 84);
        for (int cycle = 0; cycle < 3; cycle++) {
            // Held A does not relaunch Mario until the next real lava landing.
            mario.input = INPUT_A_DOWN | INPUT_NONZERO_ANALOG;
            mario.forwardVel = 18;
            mario.vel[1] = -30;
            suppliedAirStep = AIR_STEP_NONE;
            suppliedStepY = 100;
            EXPECT_EQ(check_common_airborne_cancels(&mario), FALSE);
            EXPECT_EQ(act_lava_boost(&mario), FALSE);
            EXPECT_EQ(mario.action, ACT_LAVA_BOOST);
            EXPECT_EQ(mario.vel[1], -30);
            EXPECT_EQ(mario.hurtCounter, 0);
            // Release and re-hold before landing: current held state owns the repeat.
            mario.input = INPUT_NONZERO_ANALOG;
            act_lava_boost(&mario);
            EXPECT_EQ(mario.vel[1], -30);
            mario.input |= INPUT_A_DOWN;
            suppliedAirStep = AIR_STEP_LANDED;
            suppliedStepY = 0;
            int sounds = damageSounds;
            EXPECT_EQ(act_lava_boost(&mario), FALSE);
            EXPECT_EQ(mario.action, ACT_LAVA_BOOST);
            EXPECT_EQ(mario.vel[1], 84);
            EXPECT_EQ(mario.actionState, ACT_STATE_LAVA_BOOST_HIT_LAVA);
            EXPECT_EQ(mario.forwardVel, 18);
            EXPECT_EQ(mario.hurtCounter, 0);
            EXPECT_EQ(lastMarioAnimation, MARIO_ANIM_FIRE_LAVA_BURN);
            EXPECT_EQ(damageSounds, sounds + 1);
            EXPECT_EQ(mario.particleFlags & PARTICLE_FIRE,
                      0); // Metal keeps its normal particle immunity.
        }
        // Release on the next lava contact: restore the normal settling sequence.
        mario.input = INPUT_NONZERO_ANALOG;
        for (int landing = 0; landing < 3; landing++) {
            mario.vel[1] = -20;
            EXPECT_EQ(act_lava_boost(&mario), FALSE);
            EXPECT_EQ(mario.hurtCounter, 0);
            EXPECT_EQ(mario.action, landing < 2 ? ACT_LAVA_BOOST : ACT_LAVA_BOOST_LAND);
            if (landing < 2) {
                EXPECT_EQ(mario.vel[1], 8);
            }
        }
        mario.input = INPUT_A_DOWN;
        check_lava_boost(&mario);
        EXPECT_EQ(mario.action, ACT_LAVA_BOOST_LAND);
        EXPECT_EQ(mario.hurtCounter, 0);
    }
    // A held on dry ground cannot repeat a lava bounce, including wall-origin boosts.
    const u32 origins[] = { ACT_ARG_LAVA_BOOST_METAL_GROUND_POUND, ACT_ARG_LAVA_BOOST_WALL };
    for (unsigned origin = 0; origin < 2; origin++) {
        for (int lava = 0; lava < 2; lava++) {
            begin(bhvGoomba);
            mario.action = ACT_LAVA_BOOST;
            mario.actionArg = origins[origin];
            floorSurface.type = lava ? SURFACE_BURNING : SURFACE_DEFAULT;
            mario.input = INPUT_A_DOWN | INPUT_NONZERO_ANALOG;
            mario.vel[1] = -20;
            suppliedAirStep = AIR_STEP_LANDED;
            suppliedStepY = 0;
            EXPECT_EQ(act_lava_boost(&mario), FALSE);
            EXPECT_EQ(mario.vel[1], lava ? 84 : 8);
            EXPECT_EQ(mario.hurtCounter, 0);
        }
    }
    // Losing Metal before the next landing restores ordinary damage, held A or not.
    for (int capped = 0; capped < 2; capped++) {
        for (int heldA = 0; heldA < 2; heldA++) {
            begin(bhvGoomba);
            mario.action = ACT_LAVA_BOOST;
            mario.actionArg = ACT_ARG_LAVA_BOOST_METAL_GROUND_POUND;
            mario.flags = capped ? MARIO_CAP_ON_HEAD : 0;
            mario.input = heldA ? INPUT_A_DOWN : 0;
            floorSurface.type = SURFACE_BURNING;
            mario.vel[1] = -20;
            suppliedAirStep = AIR_STEP_LANDED;
            suppliedStepY = 0;
            EXPECT_EQ(act_lava_boost(&mario), FALSE);
            EXPECT_EQ(mario.action, ACT_LAVA_BOOST);
            EXPECT_EQ(mario.vel[1], 84);
            EXPECT_EQ(mario.hurtCounter, capped ? 12 : 24);
        }
    }
    return TRUE;
}

static int thicc_slide_bounce_wave(void) {
    for (int palette = 0; palette < 2; palette++) {
        for (int liquid = 0; liquid < 3; liquid++) {
            for (int cycle = 0; cycle < 3; cycle++) {
                begin(bhvGoomba);
                sPalette = palette;
                mario.flags = 0;
                mario.action = ACT_SLIDE_KICK;
                mario.forwardVel = 40;
                mario.pos[1] = 20;
                mario.vel[1] = -40;
                mario.floorHeight = liquid == 1 ? -500 : 0;
                suppliedWaterLevel = liquid == 1 ? 0 : -10000;
                suppliedStepY = liquid == 1 ? -20 : 0;
                suppliedAirStep = liquid == 1 ? AIR_STEP_NONE : AIR_STEP_LANDED;
                if (liquid == 2) {
                    floorSurface.type = SURFACE_BURNING;
                }
                act_slide_kick(&mario);
                EXPECT_EQ(groundPoundImpacts, palette == MARIO_PALETTE_HE_THICC ? 1 : 0);
                EXPECT_EQ(mario.vel[1], 20);
                if (liquid == 1) {
                    EXPECT_EQ(mario.actionState, ACT_STATE_SLIDE_KICK_WATER_SKIPPED);
                    mario.pos[1] = 20;
                    mario.vel[1] = -40;
                    act_slide_kick(&mario); // A repeated crossing cannot bounce again.
                } else {
                    EXPECT_EQ(mario.actionState, ACT_STATE_SLIDE_KICK_END);
                    act_slide_kick(&mario); // Settling the bounce owns no second impact.
                }
                EXPECT_EQ(groundPoundImpacts, palette == MARIO_PALETTE_HE_THICC ? 1 : 0);
            }
        }
    }
    return TRUE;
}

static int thicc_punch_explosion_safe(void) {
    for (int cycle = 0; cycle < 3; cycle++) {
        begin(bhvExplosion);
        enemy.parentObj = gMarioObject;
        enemy.oBehParams2ndByte = HE_THICC_PUNCH_EXPLOSION;
        enemy.oInteractType = INTERACT_DAMAGE;
        enemy.oIntangibleTimer = 0;
        bhv_explosion_init();
        EXPECT_EQ(enemy.oInteractType, 0);
        EXPECT_EQ(enemy.oIntangibleTimer, -1);
        // Neither another parent's tag nor an ordinary Mario-parented blast is protected.
        enemy.parentObj = &parent;
        enemy.oInteractType = INTERACT_DAMAGE;
        enemy.oIntangibleTimer = 0;
        bhv_explosion_init();
        EXPECT_EQ(enemy.oInteractType, INTERACT_DAMAGE);
        EXPECT_EQ(enemy.oIntangibleTimer, 0);
        enemy.parentObj = gMarioObject;
        enemy.oBehParams2ndByte = 0;
        bhv_explosion_init();
        EXPECT_EQ(enemy.oInteractType, INTERACT_DAMAGE);
    }
    return TRUE;
}

static int capped_water_skip(void) {
    for (int button = 0; button < 2; button++) {
        for (int age = 0; age <= 10; age++) {
            begin(bhvGoomba);
            mario.action = ACT_SLIDE_KICK;
            mario.actionState = ACT_STATE_SLIDE_KICK_END;
            mario.actionTimer = 1;
            mario.input = button ? INPUT_B_PRESSED : INPUT_A_PRESSED;
            suppliedAirStep = AIR_STEP_NONE;
            act_slide_kick(&mario);
            EXPECT_EQ(mario.actionArg, 6);
            mario.input = 0;
            for (int frame = 0; frame < age; frame++) {
                act_slide_kick(&mario);
            }
            EXPECT_EQ(mario.actionArg, MAX(6 - age, 0));
        }
    }

    u32 caps[] = { 0, MARIO_METAL_CAP, MARIO_VANISH_CAP, MARIO_METAL_CAP | MARIO_VANISH_CAP };
    for (unsigned cap = 0; cap < 4; cap++) {
        for (int held = 0; held < 2; held++) {
            for (int shallow = 0; shallow < 2; shallow++) {
                begin(bhvGoomba);
                mario.flags = caps[cap];
                mario.action = ACT_SLIDE_KICK;
                mario.forwardVel = 40;
                mario.pos[1] = 20;
                mario.vel[1] = -40;
                mario.floorHeight = shallow ? -10 : -500;
                mario.input = held ? INPUT_A_DOWN : 0;
                suppliedWaterLevel = 0;
                suppliedStepY = shallow ? -10 : -20;
                suppliedAirStep = shallow ? AIR_STEP_LANDED : AIR_STEP_NONE;
                EXPECT_EQ(act_slide_kick(&mario), FALSE);
                if (cap == 0) {
                    EXPECT_EQ(mario.pos[1], 0);
                    EXPECT_EQ(mario.actionState, ACT_STATE_SLIDE_KICK_WATER_SKIPPED);
                    EXPECT_EQ(mario.vel[1], held ? 24 : 20);
                    EXPECT_EQ(mario.particleFlags & PARTICLE_WATER_SPLASH, PARTICLE_WATER_SPLASH);
                } else {
                    EXPECT_EQ(mario.pos[1], suppliedStepY);
                    EXPECT_EQ(mario.actionState,
                              shallow ? ACT_STATE_SLIDE_KICK_END : ACT_STATE_SLIDE_KICK_SLIDING);
                    EXPECT_EQ(mario.vel[1], shallow ? 20 : -40);
                    EXPECT_EQ(mario.particleFlags & PARTICLE_WATER_SPLASH, 0);
                }
            }
        }
    }
    for (int cap = 0; cap < 2; cap++) {
        begin(bhvGoomba);
        mario.flags = cap ? MARIO_METAL_CAP : 0;
        mario.action = ACT_SLIDE_KICK;
        mario.input = INPUT_A_DOWN;
        mario.pos[1] = 20;
        mario.vel[1] = -40;
        floorSurface.type = SURFACE_BURNING;
        suppliedStepY = 0;
        suppliedAirStep = AIR_STEP_LANDED;
        act_slide_kick(&mario);
        EXPECT_EQ(mario.vel[1], cap ? 20 : 24);
        EXPECT_EQ(mario.actionState, ACT_STATE_SLIDE_KICK_END);
    }
    return TRUE;
}

static int slide_kick_liquid_effects(void) {
    for (int cycle = 0; cycle < 3; cycle++) {
        for (int surface = 0; surface < 6; surface++) {
            for (int held = 0; held < 2; held++) {
                begin(bhvGoomba);
                liquidBursts = 0;
                gCurrLevelNum = surface == 2 || surface == 3 ? LEVEL_PHYSICS_LAB : LEVEL_BOB;
                gCurrAreaIndex = surface == 3 ? 2 : 1;
                mario.area->terrainType = surface == 1 ? TERRAIN_SNOW : TERRAIN_GRASS;
                mario.action = ACT_SLIDE_KICK;
                mario.flags = 0;
                mario.input = held ? INPUT_A_DOWN : 0;
                mario.pos[0] = 4500;
                mario.pos[1] = 20;
                mario.pos[2] = 12000;
                mario.vel[1] = -40;
                mario.floorHeight = surface == 5 ? -10 : -500;
                suppliedWaterLevel = 0;
                suppliedStepY = surface == 5 ? -10 : -20;
                suppliedAirStep = surface == 5 ? AIR_STEP_LANDED : AIR_STEP_NONE;
                if (surface == 4) {
                    suppliedWaterLevel = -1000;
                    floorSurface.type = SURFACE_BURNING;
                    mario.floorHeight = suppliedStepY = 7;
                    suppliedAirStep = AIR_STEP_LANDED;
                }
                if (surface == 5) {
                    mario.area->terrainType = TERRAIN_SNOW;
                }
                act_slide_kick(&mario);
                int colored = surface == 1 || surface == 2 || surface == 4 || surface == 5;
                EXPECT_EQ(liquidBursts, colored);
                EXPECT_EQ(mario.vel[1], held ? 24 : 20);
                EXPECT_EQ(mario.action, ACT_SLIDE_KICK);
                EXPECT_EQ(mario.actionState,
                          surface == 4 ? ACT_STATE_SLIDE_KICK_END : ACT_STATE_SLIDE_KICK_WATER_SKIPPED);
                if (colored) {
                    EXPECT_EQ(liquidMaterial,
                              surface == 4 ? LIQUID_PARTICLE_LAVA : LIQUID_PARTICLE_COLD_WATER);
                    EXPECT_EQ(liquidHeight, surface == 4 ? 7 : 0);
                    EXPECT_EQ(mario.particleFlags & PARTICLE_WATER_SPLASH, 0);
                } else {
                    EXPECT_EQ(mario.particleFlags & PARTICLE_WATER_SPLASH, PARTICLE_WATER_SPLASH);
                }
                // Rising continuation, then the final landing cannot replay the burst.
                suppliedAirStep = AIR_STEP_NONE;
                suppliedStepY = 40;
                act_slide_kick(&mario);
                EXPECT_EQ(liquidBursts, colored);
                suppliedAirStep = AIR_STEP_LANDED;
                suppliedStepY = surface == 4 ? 7 : 0;
                mario.vel[1] = -20;
                act_slide_kick(&mario);
                EXPECT_EQ(liquidBursts, colored);
                EXPECT_EQ(mario.action, ACT_SLIDE_KICK_SLIDE);
            }
        }
    }
    const u32 blockedCaps[] = { MARIO_METAL_CAP, MARIO_VANISH_CAP, MARIO_METAL_CAP | MARIO_VANISH_CAP };
    for (unsigned cap = 0; cap < sizeof(blockedCaps) / sizeof(blockedCaps[0]); cap++) {
        begin(bhvGoomba);
        liquidBursts = 0;
        mario.area->terrainType = TERRAIN_SNOW;
        mario.flags = blockedCaps[cap];
        mario.action = ACT_SLIDE_KICK;
        mario.pos[1] = 20;
        mario.vel[1] = -40;
        mario.floorHeight = -500;
        suppliedWaterLevel = 0;
        suppliedStepY = -20;
        suppliedAirStep = AIR_STEP_NONE;
        act_slide_kick(&mario);
        EXPECT_EQ(liquidBursts, 0);
        EXPECT_EQ(mario.vel[1], -40);
        EXPECT_EQ(mario.actionState, ACT_STATE_SLIDE_KICK_SLIDING);
    }
    gCurrLevelNum = LEVEL_BOB;
    gCurrAreaIndex = 1;
    return TRUE;
}
static int wet_fall_feedback(void) {
    const u32 actions[] = { ACT_FREEFALL,        ACT_DIVE,           ACT_HOLD_FREEFALL,
                            ACT_AIR_THROW,       ACT_THROWN_FORWARD, ACT_THROWN_BACKWARD,
                            ACT_TOP_OF_POLE_JUMP };
    for (unsigned action = 0; action < sizeof(actions) / sizeof(actions[0]); action++) {
        for (int severe = 0; severe < 2; severe++) {
            begin(bhvGoomba);
            mario.flags = MARIO_NORMAL_CAP | MARIO_CAP_ON_HEAD | MARIO_ACTION_SOUND_PLAYED
                          | MARIO_MARIO_SOUND_PLAYED;
            mario.health = 0x880;
            mario.hurtCounter = 4; // Preserve damage queued by an earlier interaction.
            mario.heldObj = &parent;
            suppliedWaterLevel = 20;
            mario.waterLevel = -10000; // The landing query must override stale input water.
            for (int cycle = 0; cycle < 3; cycle++) {
                mario.action = actions[action];
                mario.actionState = HANDSTAND_FALL_NORMAL;
                mario.flags |= MARIO_FALL_SOUND_PLAYED;
                mario.vel[1] = -56;
                mario.peakHeight = (severe ? FALL_DAMAGE_HEIGHT_LARGE : FALL_DAMAGE_HEIGHT_SMALL) + 1;
                mario.particleFlags = PARTICLE_DUST;
                EXPECT_EQ(check_fall_damage(&mario, ACT_HARD_BACKWARD_GROUND_KB), FALSE);
                EXPECT_EQ(mario.action, actions[action]);
                EXPECT_EQ(mario.heldObj, &parent);
                EXPECT_EQ(mario.hurtCounter, 4);
                EXPECT_EQ(mario.health, 0x880);
                EXPECT_EQ(mario.squishTimer, 0);
                EXPECT_EQ(mario.particleFlags,
                          PARTICLE_DUST
                              | (severe ? PARTICLE_WATER_SPLASH : PARTICLE_SHALLOW_WATER_SPLASH));
                EXPECT_EQ(rumbleRequests, cycle + 1);
                EXPECT_EQ(rumbleTime, severe ? 5 : 3);
                EXPECT_EQ(rumbleLevel, severe ? 80 : 40);
                EXPECT_EQ(stoppedFallSounds, severe ? cycle + 1 : 0);
                if (severe) {
                    EXPECT_EQ(stoppedFallSource, marioObj.header.gfx.cameraToObject);
                }
                EXPECT_EQ(!!(mario.flags & MARIO_FALL_SOUND_PLAYED), !severe);
                EXPECT_EQ(mario.flags & (MARIO_ACTION_SOUND_PLAYED | MARIO_MARIO_SOUND_PLAYED),
                          MARIO_ACTION_SOUND_PLAYED | MARIO_MARIO_SOUND_PLAYED);
                EXPECT_EQ(damageSounds, 0);
                EXPECT_EQ(hitShakes, 0);
                EXPECT_EQ(groundPoundImpacts, 0);
                EXPECT_EQ(lethalCapLosses, 0);
                // Same-update landing/recovery may run again with vertical speed settled.
                mario.action = ACT_FREEFALL_LAND;
                mario.vel[1] = 0;
                check_fall_damage(&mario, ACT_HARD_BACKWARD_GROUND_KB);
                EXPECT_EQ(rumbleRequests, cycle + 1);
                EXPECT_EQ(stoppedFallSounds, severe ? cycle + 1 : 0);
            }
        }
    }
    return TRUE;
}

static int wet_fall_boundaries(void) {
    const struct {
        f32 height, speed, water;
        u32 cap;
        s32 slippery, expected;
    } cases[] = {
        { FALL_DAMAGE_HEIGHT_SMALL, -56, 11, 0, FALSE, FALL_DAMAGE_NONE },
        { FALL_DAMAGE_HEIGHT_SMALL + 1, -55, 11, 0, FALSE, FALL_DAMAGE_NONE },
        { FALL_DAMAGE_HEIGHT_SMALL + 1, -56, 11, 0, FALSE, FALL_DAMAGE_SMALL },
        { FALL_DAMAGE_HEIGHT_LARGE, -56, 11, 0, FALSE, FALL_DAMAGE_SMALL },
        { FALL_DAMAGE_HEIGHT_LARGE + 1, -56, 11, 0, FALSE, FALL_DAMAGE_LARGE },
        { FALL_DAMAGE_HEIGHT_SMALL + 1, -56, 11, 0, TRUE, FALL_DAMAGE_NONE },
        { FALL_DAMAGE_HEIGHT_LARGE + 1, -56, 11, 0, TRUE, FALL_DAMAGE_LARGE },
        { 1150, -56, 11, MARIO_WING_CAP, FALSE, FALL_DAMAGE_NONE },
        { 1151, -56, 11, MARIO_WING_CAP, FALSE, FALL_DAMAGE_SMALL },
        { 3000, -56, 11, MARIO_WING_CAP, FALSE, FALL_DAMAGE_SMALL },
        { 3001, -56, 11, MARIO_WING_CAP, FALSE, FALL_DAMAGE_LARGE },
        { 4000, -56, 11, MARIO_METAL_CAP, FALSE, FALL_DAMAGE_NONE },
        { 4000, -56, 11, MARIO_WING_CAP | MARIO_METAL_CAP, FALSE, FALL_DAMAGE_NONE },
        { 4000, -56, 11, MARIO_VANISH_CAP, FALSE, FALL_DAMAGE_LARGE },
    };
    for (unsigned i = 0; i < sizeof(cases) / sizeof(cases[0]); i++) {
        begin(bhvGoomba);
        mario.action = ACT_FREEFALL;
        mario.flags = MARIO_CAP_ON_HEAD | cases[i].cap;
        mario.peakHeight = cases[i].height;
        mario.vel[1] = cases[i].speed;
        suppliedWaterLevel = cases[i].water;
        slipperyFloor = cases[i].slippery;
        EXPECT_EQ(check_fall_damage(&mario, ACT_HARD_BACKWARD_GROUND_KB), FALSE);
        EXPECT_EQ(mario.hurtCounter, 0);
        EXPECT_EQ(mario.squishTimer, 0);
        EXPECT_EQ(rumbleRequests, cases[i].expected != FALL_DAMAGE_NONE);
        EXPECT_EQ(stoppedFallSounds, cases[i].expected == FALL_DAMAGE_LARGE);
        u32 expectedSplash = 0;
        if (cases[i].expected == FALL_DAMAGE_LARGE) {
            expectedSplash = PARTICLE_WATER_SPLASH;
        } else if (cases[i].expected == FALL_DAMAGE_SMALL) {
            expectedSplash = PARTICLE_SHALLOW_WATER_SPLASH;
        }
        EXPECT_EQ(mario.particleFlags, expectedSplash);
    }
    for (int protected = 0; protected < 5; protected ++) {
        begin(bhvGoomba);
        mario.action = ACT_FREEFALL;
        mario.flags = MARIO_CAP_ON_HEAD;
        mario.peakHeight = 4000;
        mario.vel[1] = -56;
        suppliedWaterLevel = 20;
        if (protected == 0) {
            mario.spawnFallProtection = 1;
        }
        if (protected == 1) {
            mario.action = ACT_TWIRLING;
        }
        if (protected == 2) {
            floorSurface.type = SURFACE_BURNING;
        }
        if (protected == 3) {
            mario.action = ACT_TOP_OF_POLE_JUMP;
            mario.actionState = HANDSTAND_FALL_GRACE;
        }
        if (protected == 4) {
            mario.floor = NULL;
        }
        check_fall_damage(&mario, ACT_HARD_BACKWARD_GROUND_KB);
        EXPECT_EQ(mario.hurtCounter, 0);
        EXPECT_EQ(rumbleRequests, 0);
        EXPECT_EQ(mario.particleFlags, 0);
        EXPECT_EQ(stoppedFallSounds, 0);
    }
    // At exactly ten units submerged, damage still applies; any deeper is protected.
    for (int severe = 0; severe < 2; severe++) {
        for (int wet = 0; wet < 2; wet++) {
            begin(bhvGoomba);
            mario.flags = MARIO_CAP_ON_HEAD;
            mario.action = ACT_FREEFALL;
            mario.peakHeight = (severe ? FALL_DAMAGE_HEIGHT_LARGE : FALL_DAMAGE_HEIGHT_SMALL) + 1;
            mario.vel[1] = -56;
            suppliedWaterLevel = wet ? 10.01f : 10;
            EXPECT_EQ(check_fall_damage(&mario, ACT_HARD_BACKWARD_GROUND_KB), severe && !wet);
            s32 dryDamage = severe ? 16 : 8;
            u32 savedSplash = severe ? PARTICLE_WATER_SPLASH : PARTICLE_SHALLOW_WATER_SPLASH;
            EXPECT_EQ(mario.hurtCounter, wet ? 0 : dryDamage);
            EXPECT_EQ(mario.particleFlags, wet ? savedSplash : 0);
        }
    }
    return TRUE;
}

static int wet_fall_scream(void) {
    begin(bhvGoomba);
    mario.flags = MARIO_CAP_ON_HEAD;
    mario.action = ACT_FREEFALL;
    mario.pos[1] = 200;
    mario.vel[1] = -80;
    mario.peakHeight = 4000;
    mario.waterLevel = suppliedWaterLevel = 20;
    play_far_fall_sound(&mario);
    EXPECT_EQ(fallWarnings, 1);
    EXPECT_EQ(mario.flags & MARIO_FALL_SOUND_PLAYED, MARIO_FALL_SOUND_PLAYED);
    mario.pos[1] = 0;
    check_fall_damage(&mario, ACT_HARD_BACKWARD_GROUND_KB);
    EXPECT_EQ(stoppedFallSounds, 1);
    EXPECT_EQ(stoppedFallSource, marioObj.header.gfx.cameraToObject);
    EXPECT_EQ(mario.flags & MARIO_FALL_SOUND_PLAYED, 0);
    // A later flight can still produce its own falling warning.
    mario.pos[1] = 200;
    play_far_fall_sound(&mario);
    EXPECT_EQ(fallWarnings, 2);
    return TRUE;
}

static int fall_ground_pound_at_height(s32 severe) {
    const u32 actions[] = { ACT_FREEFALL,      ACT_JUMP,           ACT_DIVE,
                            ACT_HOLD_FREEFALL, ACT_FORWARD_AIR_KB, ACT_THROWN_BACKWARD };
    for (int palette = 0; palette < 2; palette++) {
        for (unsigned a = 0; a < sizeof(actions) / sizeof(*actions); a++) {
            begin(bhvGoomba);
            sPalette = palette;
            for (int cycle = 0; cycle < 3; cycle++) {
                mario.metalFallGroundPoundImpact = FALSE;
                mario.metalFallGroundPoundFloorObject = NULL;
                floorSurface.object = &enemy;
                mario.squishTimer = 0;
                mario.fastMetalFallSquish = FALSE;
                mario.flags = MARIO_METAL_CAP | MARIO_CAP_ON_HEAD;
                mario.action = actions[a];
                mario.heldObj = &parent;
                mario.peakHeight = (severe ? FALL_DAMAGE_HEIGHT_LARGE : FALL_DAMAGE_HEIGHT_SMALL) + 1;
                mario.vel[1] = -56;
                EXPECT_EQ(check_fall_damage(&mario, ACT_HARD_BACKWARD_GROUND_KB), FALSE);
                EXPECT_EQ(mario.action, actions[a]);
                EXPECT_EQ(mario.actionState, 0);
                EXPECT_EQ(mario.vel[1], -56);
                EXPECT_EQ(mario.heldObj, &parent);
                EXPECT_EQ(mario.metalFallGroundPoundImpact, TRUE);
                EXPECT_EQ(mario.metalFallGroundPoundFloorObject, &enemy);
                EXPECT_EQ(knockbacks, 0);
                EXPECT_EQ(mario.hurtCounter, 0);
                EXPECT_EQ(mario.squishTimer, severe ? 30 : 0);
                EXPECT_EQ(mario.fastMetalFallSquish, severe);
                EXPECT_EQ(mario.particleFlags & (PARTICLE_MIST_CIRCLE | PARTICLE_HORIZONTAL_STAR),
                          PARTICLE_MIST_CIRCLE | PARTICLE_HORIZONTAL_STAR);
                EXPECT_EQ(heavyLandingSounds, cycle + 1);
                EXPECT_EQ(groundPoundImpacts, cycle + 1);
                EXPECT_EQ(hitShakes, cycle + 1);
                EXPECT_EQ(rumbleRequests, cycle + 1);
                EXPECT_EQ(rumbleTime, 5);
                EXPECT_EQ(rumbleLevel, 80);
                mario.vel[1] = 0;
                EXPECT_EQ(check_fall_damage(&mario, ACT_HARD_BACKWARD_GROUND_KB), FALSE);
                EXPECT_EQ(groundPoundImpacts, cycle + 1);
            }
        }
    }
    for (int condition = 0; condition < 7; condition++) {
        begin(bhvGoomba);
        mario.flags = MARIO_METAL_CAP;
        mario.action = ACT_FREEFALL;
        mario.peakHeight = (severe ? FALL_DAMAGE_HEIGHT_LARGE : FALL_DAMAGE_HEIGHT_SMALL) + 1;
        mario.vel[1] = -56;
        if (condition == 0) {
            mario.peakHeight = FALL_DAMAGE_HEIGHT_SMALL;
        }
        if (condition == 1) {
            mario.vel[1] = -55;
        }
        if (condition == 2) {
            suppliedWaterLevel = 11;
        }
        if (condition == 3) {
            mario.spawnFallProtection = 1;
        }
        if (condition == 4) {
            mario.action = ACT_TWIRLING;
        }
        if (condition == 5) {
            if (severe) {
                floorSurface.type = SURFACE_BURNING;
            } else {
                slipperyFloor = TRUE;
            }
        }
        if (condition == 6) {
            mario.flags |= MARIO_WING_CAP;
            sController.buttonDown = A_BUTTON;
        }
        EXPECT_EQ(check_fall_damage(&mario, ACT_HARD_BACKWARD_GROUND_KB), FALSE);
        EXPECT_EQ(groundPoundImpacts, 0);
        EXPECT_EQ(heavyLandingSounds, 0);
        EXPECT_EQ(rumbleRequests, 0);
        EXPECT_EQ(mario.hurtCounter, 0);
    }
    // Cap pickup retains its own recovery action while publishing the impact.
    begin(bhvGoomba);
    mario.action = ACT_PUTTING_ON_CAP_AIR;
    mario.peakHeight = (severe ? FALL_DAMAGE_HEIGHT_LARGE : FALL_DAMAGE_HEIGHT_SMALL) + 1;
    mario.vel[1] = -56;
    suppliedAirStep = AIR_STEP_LANDED;
    suppliedStepY = 0;
    // Landing changes action but must not spend a second movement update.
    EXPECT_EQ(act_putting_on_cap_air(&mario), FALSE);
    EXPECT_EQ(mario.action, ACT_PUTTING_ON_CAP);
    EXPECT_EQ(mario.metalFallGroundPoundImpact, TRUE);
    EXPECT_EQ(groundPoundImpacts, 1);
    // A deliberate Metal pound emits its existing impact exactly once.
    begin(bhvGoomba);
    mario.action = ACT_GROUND_POUND;
    mario.actionState = ACT_STATE_GROUND_POUND_FALL;
    mario.peakHeight = (severe ? FALL_DAMAGE_HEIGHT_LARGE : FALL_DAMAGE_HEIGHT_SMALL) + 1;
    mario.vel[1] = -56;
    suppliedAirStep = AIR_STEP_LANDED;
    suppliedStepY = 0;
    act_ground_pound(&mario);
    EXPECT_EQ(mario.action, ACT_GROUND_POUND_LAND);
    EXPECT_EQ(heavyLandingSounds, 1);
    EXPECT_EQ(groundPoundImpacts, 1);
    EXPECT_EQ(rumbleRequests, 1);
    EXPECT_EQ(mario.hurtCounter, 0);
    return TRUE;
}

static int light_fall_ground_pound(void) {
    return fall_ground_pound_at_height(FALSE);
}

static int heavy_fall_ground_pound(void) {
    return fall_ground_pound_at_height(TRUE);
}

static int heavy_fall_squash(void) {
    begin(bhvGoomba);
    mario.action = ACT_FREEFALL;
    mario.flags = MARIO_METAL_CAP | MARIO_CAP_ON_HEAD;
    mario.peakHeight = FALL_DAMAGE_HEIGHT_LARGE + 1;
    mario.vel[1] = -56;
    EXPECT_EQ(check_fall_damage(&mario, ACT_HARD_BACKWARD_GROUND_KB), FALSE);
    EXPECT_EQ(mario.action, ACT_FREEFALL);
    EXPECT_EQ(mario.hurtCounter, 0);
    EXPECT_EQ(mario.squishTimer, 30);
    EXPECT_EQ(mario.fastMetalFallSquish, TRUE);
    EXPECT_EQ(damageSounds, 0);
    EXPECT_EQ(hitShakes, 1);
    EXPECT_EQ(knockbacks, 0);
    EXPECT_EQ(groundPoundImpacts, 1);
    EXPECT_EQ(heavyLandingSounds, 1);
    for (int frame = 0; frame < 15; frame++) {
        squish_mario_model(&mario);
        EXPECT_EQ(mario.squishTimer, 28 - frame * 2);
        if (frame == 0) {
            EXPECT_EQ(marioObj.header.gfx.scale[1], 0.4f);
        }
    }
    squish_mario_model(&mario);
    EXPECT_EQ(mario.fastMetalFallSquish, FALSE);
    EXPECT_EQ(marioObj.header.gfx.scale[1], 1.0f);

    begin(bhvGoomba);
    mario.action = ACT_FREEFALL;
    mario.peakHeight = FALL_DAMAGE_HEIGHT_LARGE + 1;
    mario.vel[1] = -56;
    mario.flags = MARIO_CAP_ON_HEAD;
    EXPECT_EQ(check_fall_damage(&mario, ACT_HARD_BACKWARD_GROUND_KB), TRUE);
    EXPECT_EQ(mario.action, ACT_HARD_BACKWARD_GROUND_KB);
    EXPECT_EQ(mario.hurtCounter, 16);
    EXPECT_EQ(mario.squishTimer, 0);

    begin(bhvGoomba);
    mario.action = ACT_FREEFALL;
    mario.peakHeight = FALL_DAMAGE_HEIGHT_SMALL + 1;
    mario.vel[1] = -56;
    mario.flags = MARIO_CAP_ON_HEAD;
    EXPECT_EQ(check_fall_damage(&mario, ACT_HARD_BACKWARD_GROUND_KB), FALSE);
    EXPECT_EQ(mario.squishTimer, 30);
    EXPECT_EQ(mario.fastMetalFallSquish, FALSE);
    squish_mario_model(&mario);
    EXPECT_EQ(mario.squishTimer, 29);

    for (int condition = 0; condition < 6; condition++) {
        begin(bhvGoomba);
        mario.action = ACT_FREEFALL;
        mario.peakHeight = FALL_DAMAGE_HEIGHT_LARGE + 1;
        mario.vel[1] = -56;
        if (condition == 0) {
            mario.peakHeight = FALL_DAMAGE_HEIGHT_LARGE;
        }
        if (condition == 1) {
            mario.vel[1] = -55;
        }
        if (condition == 2) {
            suppliedWaterLevel = 20;
        }
        if (condition == 3) {
            mario.spawnFallProtection = 1;
        }
        if (condition == 4) {
            mario.action = ACT_TWIRLING;
        }
        if (condition == 5) {
            floorSurface.type = SURFACE_BURNING;
        }
        EXPECT_EQ(check_fall_damage(&mario, ACT_HARD_BACKWARD_GROUND_KB), FALSE);
        EXPECT_EQ(mario.metalFallGroundPoundImpact, condition == 0);
        u32 expectedAction = condition == 4 ? ACT_TWIRLING : ACT_FREEFALL;
        EXPECT_EQ(mario.action, expectedAction);
        EXPECT_EQ(mario.squishTimer, 0);
        EXPECT_EQ(mario.hurtCounter, 0);
    }
    begin(bhvGoomba);
    mario.action = ACT_FREEFALL;
    mario.peakHeight = FALL_DAMAGE_HEIGHT_LARGE + 1;
    mario.vel[1] = -56;
    mario.squishTimer = 25;
    check_fall_damage(&mario, ACT_HARD_BACKWARD_GROUND_KB);
    EXPECT_EQ(mario.squishTimer, 30);
    EXPECT_EQ(mario.fastMetalFallSquish, TRUE);
    mario.action = ACT_SQUISHED;
    squish_mario_model(&mario);
    EXPECT_EQ(mario.squishTimer, 29);
    EXPECT_EQ(mario.fastMetalFallSquish, FALSE);
    return TRUE;
}
static int thicc_fall_shockwave(void) {
    const u32 actions[] = { ACT_FREEFALL,      ACT_JUMP,           ACT_DIVE,
                            ACT_HOLD_FREEFALL, ACT_FORWARD_AIR_KB, ACT_THROWN_BACKWARD };
    const u32 reactions[] = { ACT_HARD_FORWARD_GROUND_KB, ACT_HARD_BACKWARD_GROUND_KB };
    for (int cycle = 0; cycle < 3; cycle++) {
        for (int palette = 0; palette < 2; palette++) {
            for (int severe = 0; severe < 2; severe++) {
                for (unsigned a = 0; a < sizeof(actions) / sizeof(*actions); a++) {
                    for (unsigned r = 0; r < sizeof(reactions) / sizeof(*reactions); r++) {
                        begin(bhvGoomba);
                        sPalette = palette;
                        mario.flags = MARIO_NORMAL_CAP | MARIO_CAP_ON_HEAD;
                        mario.action = actions[a];
                        mario.heldObj = &parent;
                        mario.vel[1] = -80;
                        mario.peakHeight =
                            (severe ? FALL_DAMAGE_HEIGHT_LARGE : FALL_DAMAGE_HEIGHT_SMALL) + 1;
                        EXPECT_EQ(check_fall_damage(&mario, reactions[r]), severe);
                        EXPECT_EQ(groundPoundImpacts, palette == MARIO_PALETTE_HE_THICC ? 1 : 0);
                        EXPECT_EQ(mario.hurtCounter, severe ? 16 : 8);
                        EXPECT_EQ(mario.squishTimer, severe ? 0 : 30);
                        EXPECT_EQ(mario.action, severe ? reactions[r] : actions[a]);
                        EXPECT_EQ(mario.heldObj, severe ? NULL : &parent);
                        // Settled recovery cannot repeat the automatic shockwave or queued damage.
                        mario.vel[1] = 0;
                        EXPECT_EQ(check_fall_damage(&mario, reactions[r]), FALSE);
                        EXPECT_EQ(groundPoundImpacts, palette == MARIO_PALETTE_HE_THICC ? 1 : 0);
                        EXPECT_EQ(mario.hurtCounter, severe ? 16 : 8);
                    }
                }
            }
        }
    }
    // Damage exclusions and their exact boundaries also exclude the automatic wave.
    for (int condition = 0; condition < 7; condition++) {
        begin(bhvGoomba);
        sPalette = MARIO_PALETTE_HE_THICC;
        mario.flags = MARIO_CAP_ON_HEAD;
        mario.action = ACT_FREEFALL;
        mario.vel[1] = -80;
        mario.peakHeight = FALL_DAMAGE_HEIGHT_LARGE + 1;
        if (condition == 0) {
            mario.flags |= MARIO_WING_CAP;
            sController.buttonDown = A_BUTTON;
        }
        if (condition == 1) {
            suppliedWaterLevel = 11;
        }
        if (condition == 2) {
            mario.spawnFallProtection = 1;
        }
        if (condition == 3) {
            mario.action = ACT_TWIRLING;
        }
        if (condition == 4) {
            floorSurface.type = SURFACE_BURNING;
        }
        if (condition == 5) {
            mario.vel[1] = -55;
        }
        if (condition == 6) {
            mario.peakHeight = FALL_DAMAGE_HEIGHT_SMALL;
        }
        EXPECT_EQ(check_fall_damage(&mario, ACT_HARD_BACKWARD_GROUND_KB), FALSE);
        EXPECT_EQ(mario.hurtCounter, 0);
        EXPECT_EQ(groundPoundImpacts, 0);
    }
    begin(bhvGoomba);
    sPalette = MARIO_PALETTE_HE_THICC;
    mario.flags = MARIO_CAP_ON_HEAD;
    mario.action = ACT_FREEFALL;
    mario.vel[1] = -80;
    mario.peakHeight = FALL_DAMAGE_HEIGHT_SMALL + 1;
    suppliedWaterLevel = 10;
    check_fall_damage(&mario, ACT_HARD_BACKWARD_GROUND_KB);
    EXPECT_EQ(mario.hurtCounter, 8);
    EXPECT_EQ(groundPoundImpacts, 1);
    // A deliberate pound taking fall damage still emits only its existing one wave.
    for (int severe = 0; severe < 2; severe++) {
        for (int cycle = 0; cycle < 3; cycle++) {
            begin(bhvGoomba);
            sPalette = MARIO_PALETTE_HE_THICC;
            mario.flags = MARIO_CAP_ON_HEAD;
            mario.action = ACT_GROUND_POUND;
            mario.actionState = ACT_STATE_GROUND_POUND_FALL;
            mario.vel[1] = -80;
            mario.peakHeight = (severe ? FALL_DAMAGE_HEIGHT_LARGE : FALL_DAMAGE_HEIGHT_SMALL) + 1;
            suppliedAirStep = AIR_STEP_LANDED;
            suppliedStepY = 0;
            act_ground_pound(&mario);
            EXPECT_EQ(groundPoundImpacts, 1);
            EXPECT_EQ(mario.hurtCounter, severe ? 16 : 8);
            EXPECT_EQ(mario.action, severe ? ACT_HARD_BACKWARD_GROUND_KB : ACT_GROUND_POUND_LAND);
        }
    }
    return TRUE;
}
static int vanish_pound_water_entry(void) {
    begin(bhvGoomba);
    mario.action = ACT_GROUND_POUND;
    mario.flags = MARIO_VANISH_CAP;
    mario.actionState = ACT_STATE_GROUND_POUND_FALL;
    mario.waterLevel = 0;
    mario.prevPos[1] = 0;
    suppliedAirStep = AIR_STEP_LANDED;
    suppliedStepY = -300;
    EXPECT_EQ(act_ground_pound(&mario), FALSE);
    EXPECT_EQ(mario.particleFlags & PARTICLE_VANISH_WATER_RING, PARTICLE_VANISH_WATER_RING);
    EXPECT_EQ(mario.action, ACT_GROUND_POUND_LAND);
    return TRUE;
}
static int wing_fall_damage(void) {
    const u32 powers[] = { 0, MARIO_WING_CAP, MARIO_VANISH_CAP, MARIO_WING_CAP | MARIO_VANISH_CAP,
                           MARIO_WING_CAP | MARIO_METAL_CAP };
    const u32 reactions[] = { ACT_HARD_BACKWARD_GROUND_KB, ACT_HARD_FORWARD_GROUND_KB };
    for (int cycle = 0; cycle < 3; cycle++) {
        for (unsigned power = 0; power < sizeof(powers) / sizeof(powers[0]); power++) {
            for (int held = 0; held < 2; held++) {
                for (int severe = 0; severe < 2; severe++) {
                    for (int forward = 0; forward < 2; forward++) {
                        begin(bhvGoomba);
                        mario.action = ACT_FREEFALL;
                        mario.flags = powers[power] | MARIO_CAP_ON_HEAD;
                        mario.capTimer = 90;
                        mario.vel[1] = -80.0f;
                        mario.peakHeight = severe ? 4000.0f : 1200.0f;
                        mario.input = held ? 0 : INPUT_A_DOWN | INPUT_A_PRESSED;
                        sController.buttonDown = held ? A_BUTTON : 0;
                        mario.heldObj = &parent;
                        s32 safe = (powers[power] & MARIO_METAL_CAP)
                                   || (held && (powers[power] & MARIO_WING_CAP));
                        s32 metalPound = (powers[power] & MARIO_METAL_CAP)
                                         && !(held && (powers[power] & MARIO_WING_CAP));
                        EXPECT_EQ(check_fall_damage(&mario, reactions[forward]), !safe && severe);
                        s32 expectedDamage = 0;
                        if (!safe) {
                            expectedDamage = severe ? 16 : 8;
                        }
                        EXPECT_EQ(mario.hurtCounter, expectedDamage);
                        u32 expectedAction = ACT_FREEFALL;
                        if (!safe && severe) {
                            expectedAction = reactions[forward];
                        }
                        EXPECT_EQ(mario.action, expectedAction);
                        EXPECT_EQ(mario.metalFallGroundPoundImpact, metalPound);
                        EXPECT_EQ(mario.capTimer, 90);
                        if (safe) {
                            EXPECT_EQ(mario.heldObj, &parent);
                            EXPECT_EQ(mario.flags, powers[power] | MARIO_CAP_ON_HEAD);
                        }
                    }
                }
            }
        }
    }
    // Protection preserves previously queued damage, including for capless Mario.
    begin(bhvGoomba);
    mario.action = ACT_FREEFALL;
    mario.flags = MARIO_WING_CAP;
    mario.health = 0x100;
    mario.hurtCounter = 4;
    mario.vel[1] = -80.0f;
    mario.peakHeight = 4000.0f;
    sController.buttonDown = A_BUTTON;
    EXPECT_EQ(check_fall_damage(&mario, ACT_HARD_BACKWARD_GROUND_KB), FALSE);
    EXPECT_EQ(mario.hurtCounter, 4);
    EXPECT_EQ(mario.health, 0x100);
    EXPECT_EQ(lethalCapLosses, 0);

    for (int cycle = 0; cycle < 3; cycle++) {
        begin(bhvGoomba);
        mario.action = ACT_GROUND_POUND;
        mario.actionState = ACT_STATE_GROUND_POUND_FALL;
        mario.flags = MARIO_WING_CAP | MARIO_CAP_ON_HEAD;
        mario.peakHeight = 4000.0f;
        mario.vel[1] = -80.0f;
        sController.buttonDown = A_BUTTON;
        suppliedAirStep = AIR_STEP_LANDED;
        act_ground_pound(&mario);
        EXPECT_EQ(mario.action, ACT_GROUND_POUND_LAND);
        EXPECT_EQ(mario.hurtCounter, 0);
        EXPECT_EQ(groundPoundImpacts, 1);

        // Losing Wing just before impact restores fall damage even with A held.
        begin(bhvGoomba);
        mario.action = ACT_FREEFALL;
        mario.flags = MARIO_CAP_ON_HEAD;
        mario.vel[1] = -80.0f;
        mario.peakHeight = 4000.0f;
        sController.buttonDown = A_BUTTON;
        EXPECT_EQ(check_fall_damage(&mario, ACT_HARD_BACKWARD_GROUND_KB), TRUE);
        EXPECT_EQ(mario.hurtCounter, 16);
    }
    return TRUE;
}
static int wing_preset(void) {
    for (s32 original = 0; original < 2; original++) {
        sTestGameOptions[GO_WING_CAP] = original;
        begin(bhvGoomba);
        mario.action = ACT_FREEFALL;
        mario.flags = MARIO_WING_CAP | MARIO_CAP_ON_HEAD;
        mario.vel[1] = -80.0f;
        mario.peakHeight = 4000.0f;
        sController.buttonDown = A_BUTTON;
        EXPECT_EQ(check_fall_damage(&mario, ACT_HARD_BACKWARD_GROUND_KB), original);
        EXPECT_EQ(mario.hurtCounter, original ? 16 : 0);
        begin(bhvGoomba);
        mario.flags = MARIO_WING_CAP;
        area.terrainType = TERRAIN_SNOW;
        mario.peakHeight = 1100.0f;
        sController.buttonDown = A_BUTTON;
        EXPECT_EQ(should_get_stuck_in_ground(&mario), original);
    }
    sTestGameOptions[GO_WING_CAP] = 0;
    return TRUE;
}

static int wing_burial_boundaries(void) {
    const u32 powers[] = { 0,
                           MARIO_WING_CAP,
                           MARIO_METAL_CAP,
                           MARIO_VANISH_CAP,
                           MARIO_WING_CAP | MARIO_METAL_CAP,
                           MARIO_WING_CAP | MARIO_VANISH_CAP };
    const s16 terrains[] = { TERRAIN_SNOW, TERRAIN_SAND, TERRAIN_STONE };
    for (int cycle = 0; cycle < 3; cycle++) {
        for (unsigned power = 0; power < sizeof(powers) / sizeof(powers[0]); power++) {
            for (unsigned terrain = 0; terrain < sizeof(terrains) / sizeof(terrains[0]); terrain++) {
                for (int held = 0; held < 2; held++) {
                    begin(bhvGoomba);
                    mario.flags = powers[power];
                    area.terrainType = terrains[terrain];
                    mario.peakHeight = 1100.0f;
                    // Action input may be masked or stale; physical A is authoritative.
                    mario.input = held ? 0 : INPUT_A_DOWN | INPUT_A_PRESSED;
                    sController.buttonDown = held ? A_BUTTON : 0;
                    s32 protected = held && (mario.flags & MARIO_WING_CAP);
                    s32 eligible = terrain != 2 && !(terrain == 1 && (mario.flags & MARIO_VANISH_CAP));
                    EXPECT_EQ(should_get_stuck_in_ground(&mario), eligible && !protected);
                }
            }
        }
    }
    for (int held = 0; held < 2; held++) {
        for (int boundary = 0; boundary < 8; boundary++) {
            begin(bhvGoomba);
            mario.flags = MARIO_WING_CAP;
            sController.buttonDown = held ? A_BUTTON : 0;
            area.terrainType = TERRAIN_SNOW;
            mario.peakHeight = 1000.01f;
            floorSurface.normal.y = COS30;
            if (boundary == 1) {
                mario.peakHeight = 1000.0f;
            }
            if (boundary == 2) {
                floorSurface.normal.y = COS30 - 0.001f;
            }
            if (boundary == 3) {
                floorSurface.flags = SURFACE_FLAG_DYNAMIC;
            }
            if (boundary == 4) {
                floorSurface.type = SURFACE_HARD;
            }
            if (boundary == 5) {
                floorSurface.type = SURFACE_BURNING;
            }
            if (boundary == 6) {
                mario.floor = NULL;
            }
            if (boundary == 7) {
                mario.flags = 0; // Wing removed just before impact.
            }
            EXPECT_EQ(should_get_stuck_in_ground(&mario), boundary == 7 || (boundary == 0 && !held));
        }
    }
    begin(bhvGoomba);
    mario.flags = MARIO_WING_CAP;
    sController.buttonDown = A_BUTTON;
    floorSurface.type = SURFACE_DEEP_QUICKSAND;
    EXPECT_EQ(mario_update_quicksand(&mario, 0.25f), FALSE);
    EXPECT_EQ(mario.quicksandDepth > 1.1f, TRUE);
    return TRUE;
}
static int wing_burial_landings(void) {
    for (int cycle = 0; cycle < 3; cycle++) {
        for (int terrain = 0; terrain < 2; terrain++) {
            for (int held = 0; held < 2; held++) {
                for (int pound = 0; pound < 2; pound++) {
                    begin(bhvGoomba);
                    mario.action = pound ? ACT_GROUND_POUND : ACT_FREEFALL;
                    mario.actionState = ACT_STATE_GROUND_POUND_FALL;
                    mario.flags = MARIO_WING_CAP | MARIO_CAP_ON_HEAD;
                    mario.peakHeight = 1100.0f;
                    area.terrainType = terrain ? TERRAIN_SNOW : TERRAIN_SAND;
                    sController.buttonDown = held ? A_BUTTON : 0;
                    suppliedAirStep = AIR_STEP_LANDED;
                    if (pound) {
                        act_ground_pound(&mario);
                        EXPECT_EQ(mario.action,
                                  held ? ACT_GROUND_POUND_LAND : ACT_BUTT_STUCK_IN_GROUND);
                        EXPECT_EQ(groundPoundImpacts, 1);
                    } else {
                        EXPECT_EQ(check_fall_damage_or_get_stuck(&mario, ACT_HARD_BACKWARD_GROUND_KB),
                                  !held);
                        EXPECT_EQ(mario.action, held ? ACT_FREEFALL : ACT_FEET_STUCK_IN_GROUND);
                    }
                    EXPECT_EQ(mario.hurtCounter, 0);
                }
            }
        }
    }
    // Held-A Wing protection also prevents fall damage after the burial check.
    begin(bhvGoomba);
    mario.action = ACT_FREEFALL;
    mario.flags = MARIO_WING_CAP | MARIO_CAP_ON_HEAD;
    area.terrainType = TERRAIN_SNOW;
    mario.peakHeight = 4000.0f;
    mario.vel[1] = -80.0f;
    sController.buttonDown = A_BUTTON;
    EXPECT_EQ(check_fall_damage_or_get_stuck(&mario, ACT_HARD_BACKWARD_GROUND_KB), FALSE);
    EXPECT_EQ(mario.action, ACT_FREEFALL);
    EXPECT_EQ(mario.hurtCounter, 0);
    return TRUE;
}
static int vanish_sand(void) {
    for (int terrain = 0; terrain < 2; terrain++) {
        for (int cap = 0; cap < 2; cap++) {
            for (int quick = 0; quick < 2; quick++) {
                begin(bhvGoomba);
                mario.flags = cap ? MARIO_VANISH_CAP : 0;
                area.terrainType = terrain ? TERRAIN_SNOW : TERRAIN_SAND;
                mario.peakHeight = 2000;
                floorSurface.type = quick ? SURFACE_QUICKSAND : SURFACE_DEFAULT;
                EXPECT_EQ(should_get_stuck_in_ground(&mario), !(!terrain && cap && !quick));
            }
        }
    }
    s16 types[] = { SURFACE_SHALLOW_QUICKSAND,       SURFACE_QUICKSAND,      SURFACE_MOVING_QUICKSAND,
                    SURFACE_DEEP_MOVING_QUICKSAND,   SURFACE_DEEP_QUICKSAND, SURFACE_INSTANT_QUICKSAND,
                    SURFACE_INSTANT_MOVING_QUICKSAND };
    for (unsigned t = 0; t < sizeof(types) / sizeof(*types); t++) {
        begin(bhvGoomba);
        mario.flags = MARIO_VANISH_CAP;
        floorSurface.type = types[t];
        s32 lethal =
            types[t] == SURFACE_INSTANT_QUICKSAND || types[t] == SURFACE_INSTANT_MOVING_QUICKSAND;
        EXPECT_EQ(mario_update_quicksand(&mario, 0.25f), lethal);
        if (lethal) {
            EXPECT_EQ(mario.action, ACT_QUICKSAND_DEATH);
        } else {
            EXPECT_EQ(mario.quicksandDepth > 1.1f, TRUE);
        }
        if (types[t] == SURFACE_MOVING_QUICKSAND || types[t] == SURFACE_DEEP_MOVING_QUICKSAND
            || types[t] == SURFACE_INSTANT_MOVING_QUICKSAND) {
            mario.vel[2] = 0;
            EXPECT_EQ(mario_update_moving_sand(&mario), TRUE);
            EXPECT_EQ(mario.vel[2], 12);
        }
    }
    begin(bhvGoomba);
    mario.flags = MARIO_VANISH_CAP;
    floorSurface.type = SURFACE_DEEP_QUICKSAND;
    mario.quicksandDepth = 159.9f;
    EXPECT_EQ(mario_update_quicksand(&mario, 0.25f), TRUE);
    EXPECT_EQ(mario.action, ACT_QUICKSAND_DEATH);
    return TRUE;
}
static int capped_shocks(void) {
    u32 caps[] = { 0,
                   MARIO_METAL_CAP,
                   MARIO_VANISH_CAP,
                   MARIO_WING_CAP,
                   MARIO_METAL_CAP | MARIO_VANISH_CAP,
                   MARIO_METAL_CAP | MARIO_WING_CAP,
                   MARIO_VANISH_CAP | MARIO_WING_CAP,
                   MARIO_SPECIAL_CAPS };
    u32 actions[] = { ACT_WALKING, ACT_FREEFALL, ACT_WATER_IDLE, ACT_METAL_WATER_FALLING,
                      ACT_WALKING, ACT_FREEFALL };
    for (unsigned cap = 0; cap < 8; cap++) {
        for (unsigned a = 0; a < 6; a++) {
            for (int soaked = 0; soaked <= 1; soaked++) {
                s32 waterShock = (a == 2 || a == 3 || (a >= 4 && (caps[cap] & MARIO_METAL_CAP)))
                                 && !(caps[cap] & MARIO_VANISH_CAP);
                s32 activeSoaked = soaked && !(caps[cap] & MARIO_VANISH_CAP);
                begin(bhvGoomba);
                enemy.oInteractType = INTERACT_SHOCK;
                enemy.oDamageOrCoinValue = 2;
                mario.flags = caps[cap] | MARIO_NORMAL_CAP | MARIO_CAP_ON_HEAD;
                mario.capTimer = caps[cap] ? 90 : 0;
                mario.action = actions[a];
                mario.heldObj = &parent;
                if (a >= 2) {
                    mario.waterLevel = 100;
                    mario.pos[1] = 20;
                    mario.floorHeight = -200;
                }
                mario.soakedTimer = activeSoaked ? 150 : 0;
                mario.soakedThisUpdate = activeSoaked;
                EXPECT_EQ(mario_vanish_ignores_hostile_contact(&mario, &enemy, INTERACT_SHOCK), FALSE);
                EXPECT_EQ(interact_shock(&mario, INTERACT_SHOCK, &enemy), TRUE);
                EXPECT_EQ(mario.hurtCounter, ((caps[cap] & MARIO_METAL_CAP) ? 12 : 8)
                                                 * ((activeSoaked || a >= 2) ? 2 : 1));
                EXPECT_EQ(mario.soakedThisUpdate, 0);
                EXPECT_EQ(mario.soakedTimer, 0);
                EXPECT_EQ(mario.action, waterShock ? ACT_WATER_SHOCKED : ACT_SHOCKED);
                EXPECT_EQ(mario.heldObj, NULL);
                EXPECT_EQ(mario.flags, MARIO_NORMAL_CAP | MARIO_CAP_ON_HEAD);
                EXPECT_EQ(mario.capTimer, 0);
                EXPECT_EQ(capMusicStops, caps[cap] ? 1 : 0);
                EXPECT_EQ(enemy.oInteractStatus, INT_STATUS_INTERACTED | INT_STATUS_ATTACKED_MARIO);
                EXPECT_EQ(mario.actionArg, !waterShock && a == 0);
                begin(bhvGoomba);
                enemy.oInteractType = INTERACT_SHOCK;
                enemy.oDamageOrCoinValue = 2;
                mario.flags = caps[cap] | MARIO_NORMAL_CAP | MARIO_CAP_ON_HEAD;
                mario.capTimer = 90;
                sInvulnerable = TRUE;
                EXPECT_EQ(interact_shock(&mario, INTERACT_SHOCK, &enemy), FALSE);
                EXPECT_EQ(mario.hurtCounter, 0);
                EXPECT_EQ(mario.flags, caps[cap] | MARIO_NORMAL_CAP | MARIO_CAP_ON_HEAD);
                EXPECT_EQ(mario.capTimer, 90);
                EXPECT_EQ(capMusicStops, 0);
            }
        }
    }
    begin(bhvGoomba);
    enemy.oInteractType = INTERACT_SHOCK;
    enemy.oDamageOrCoinValue = 2;
    mario.flags = MARIO_METAL_CAP | MARIO_VANISH_CAP | MARIO_CAP_ON_HEAD;
    mario.action = ACT_METAL_WATER_FALLING;
    mario.pos[1] = 100;
    mario.vel[1] = -10;
    EXPECT_EQ(interact_shock(&mario, INTERACT_SHOCK, &enemy), TRUE);
    EXPECT_EQ(mario.hurtCounter, 12);
    EXPECT_EQ(mario.action, ACT_SHOCKED);
    EXPECT_EQ(mario.actionArg, 0);
    EXPECT_EQ(mario.flags & MARIO_SPECIAL_CAPS, 0);
    // A non-air action above deep water must not be treated as floor-supported.
    begin(bhvGoomba);
    enemy.oInteractType = INTERACT_SHOCK;
    enemy.oDamageOrCoinValue = 2;
    mario.flags = MARIO_METAL_CAP | MARIO_NORMAL_CAP | MARIO_CAP_ON_HEAD;
    mario.action = ACT_WALKING;
    mario.waterLevel = 100;
    mario.pos[1] = 120;
    mario.floorHeight = -500;
    EXPECT_EQ(interact_shock(&mario, INTERACT_SHOCK, &enemy), TRUE);
    EXPECT_EQ(mario.action, ACT_SHOCKED);
    EXPECT_EQ(mario.actionArg, 0);
    EXPECT_EQ(mario.hurtCounter, 12);
    // Even a swimming action above the surface must use the airborne recovery.
    begin(bhvGoomba);
    enemy.oInteractType = INTERACT_SHOCK;
    enemy.oDamageOrCoinValue = 2;
    mario.action = ACT_WATER_IDLE;
    mario.waterLevel = 100;
    mario.pos[1] = 120;
    mario.floorHeight = -500;
    EXPECT_EQ(interact_shock(&mario, INTERACT_SHOCK, &enemy), TRUE);
    EXPECT_EQ(mario.action, ACT_SHOCKED);
    EXPECT_EQ(mario.actionArg, 0);
    // Metal shock is three wedges dry, doubled once by Soaked or water contact.
    for (int wet = 0; wet < 4; wet++) {
        for (int damage = 1; damage <= 5; damage += 2) {
            begin(bhvGoomba);
            enemy.oInteractType = INTERACT_SHOCK;
            enemy.oDamageOrCoinValue = damage;
            mario.flags = MARIO_METAL_CAP | MARIO_NORMAL_CAP | MARIO_CAP_ON_HEAD;
            mario.action = ACT_WALKING;
            if (wet & 1) {
                mario.soakedTimer = 150;
                mario.soakedThisUpdate = TRUE;
            }
            if (wet & 2) {
                mario.waterLevel = 20;
                mario.pos[1] = 0;
            }
            EXPECT_EQ(interact_shock(&mario, INTERACT_SHOCK, &enemy), TRUE);
            EXPECT_EQ(mario.hurtCounter, wet ? 24 : 12);
        }
    }
    begin(bhvGoomba);
    enemy.oInteractType = INTERACT_SHOCK;
    enemy.oDamageOrCoinValue = 2;
    mario.flags = MARIO_METAL_CAP;
    mario.action = ACT_WALKING;
    mario.soakedTimer = 150;
    mario.soakedThisUpdate = TRUE;
    EXPECT_EQ(interact_shock(&mario, INTERACT_SHOCK, &enemy), TRUE);
    EXPECT_EQ(mario.hurtCounter, 24); // No capless bonus on Metal shock.
    begin(bhvGoomba);
    enemy.oInteractType = INTERACT_SHOCK;
    enemy.oDamageOrCoinValue = 0;
    mario.flags = MARIO_WING_CAP | MARIO_NORMAL_CAP | MARIO_CAP_ON_HEAD;
    mario.capTimer = 90;
    mario.soakedTimer = 150;
    mario.soakedThisUpdate = TRUE;
    EXPECT_EQ(interact_shock(&mario, INTERACT_SHOCK, &enemy), TRUE);
    EXPECT_EQ(mario.hurtCounter, 0);
    EXPECT_EQ(mario.flags & MARIO_WING_CAP, MARIO_WING_CAP);
    EXPECT_EQ(mario.capTimer, 90);
    EXPECT_EQ(capMusicStops, 0);
    EXPECT_EQ(mario.soakedTimer, 150);
    return TRUE;
}
static int shock_above_water_recovery(void) {
    begin(bhvGoomba);
    mario.action = ACT_SHOCKED;
    mario.actionArg = 0;
    mario.flags = MARIO_NORMAL_CAP | MARIO_CAP_ON_HEAD;
    mario.waterLevel = 100;
    mario.floorHeight = -500;
    mario.pos[1] = 120;
    suppliedAirStep = AIR_STEP_NONE;
    suppliedStepY = 10;
    EXPECT_EQ(act_shocked(&mario), FALSE);
    EXPECT_EQ(mario.action, ACT_SHOCKED);
    EXPECT_EQ(mario.pos[1], 10);
    // The next update crosses the submerged threshold and changes recovery.
    suppliedStepY = -10;
    EXPECT_EQ(act_shocked(&mario), FALSE);
    EXPECT_EQ(mario.pos[1], -10);
    EXPECT_EQ(act_shocked(&mario), TRUE);
    EXPECT_EQ(mario.action, ACT_WATER_SHOCKED);
    EXPECT_EQ(mario.pos[1], -10);
    begin(bhvGoomba);
    mario.flags = MARIO_NORMAL_CAP | MARIO_CAP_ON_HEAD;
    mario.action = ACT_SHOCKED;
    mario.actionArg = 0;
    mario.waterLevel = 100;
    mario.pos[1] = 50;
    mario.floorHeight = -500;
    mario.vel[1] = -80;
    suppliedStepY = -500;
    suppliedAirStep = AIR_STEP_LANDED;
    EXPECT_EQ(act_shocked(&mario), TRUE);
    EXPECT_EQ(mario.action, ACT_WATER_SHOCKED);
    EXPECT_EQ(mario.pos[1], 50);
    return TRUE;
}
static int repeated_water_shocks(void) {
    begin(bhvGoomba);
    enemy.oInteractType = INTERACT_SHOCK;
    enemy.oDamageOrCoinValue = 2;
    mario.flags = MARIO_NORMAL_CAP | MARIO_CAP_ON_HEAD;
    mario.waterLevel = 20;
    mario.pos[1] = 0;
    mario.soakedTimer = 150;
    mario.soakedThisUpdate = TRUE;
    mario.soakedWaterContact = TRUE;
    for (int shock = 0; shock < 3; shock++) {
        mario.action = ACT_WALKING;
        mario.hurtCounter = 0;
        enemy.oInteractStatus = 0;
        EXPECT_EQ(interact_shock(&mario, INTERACT_SHOCK, &enemy), TRUE);
        EXPECT_EQ(mario.hurtCounter, 16);
        EXPECT_EQ(mario.soakedThisUpdate, FALSE);
        EXPECT_EQ(mario.soakedTimer, 0);
    }
    mario.waterLevel = FLOOR_LOWER_LIMIT;
    mario.action = ACT_WALKING;
    mario.hurtCounter = 0;
    EXPECT_EQ(interact_shock(&mario, INTERACT_SHOCK, &enemy), TRUE);
    EXPECT_EQ(mario.hurtCounter, 8);

    // A water contact doubles damage even if the Vanish cap prevents Soaked.
    begin(bhvGoomba);
    enemy.oInteractType = INTERACT_SHOCK;
    enemy.oDamageOrCoinValue = 2;
    mario.flags = MARIO_VANISH_CAP | MARIO_NORMAL_CAP | MARIO_CAP_ON_HEAD;
    mario.waterLevel = 20;
    mario.pos[1] = 20;
    EXPECT_EQ(interact_shock(&mario, INTERACT_SHOCK, &enemy), TRUE);
    EXPECT_EQ(mario.hurtCounter, 16);
    EXPECT_EQ(mario.action, ACT_SHOCKED);
    EXPECT_EQ(mario.flags & MARIO_VANISH_CAP, 0);
    return TRUE;
}
static int soaked_fire_contact(void) {
    const u32 actions[] = { ACT_WALKING, ACT_JUMP, ACT_FREEFALL };

    for (unsigned pose = 0; pose < sizeof(actions) / sizeof(actions[0]); pose++) {
        begin(bhvGoomba);
        mario.flags = MARIO_NORMAL_CAP | MARIO_CAP_ON_HEAD;
        enemy.oInteractType = INTERACT_FLAME;
        suppliedAirStep = AIR_STEP_NONE;

        for (int cycle = 0; cycle < 3; cycle++) {
            mario.action = actions[pose];
            mario.vel[1] = pose == 2 ? -1.0f : 1.0f;
            mario.health = 0x880;
            mario_apply_soaked_status(&mario);

            // A flamethrower resets its status and can supply fresh flame objects
            // every update. Continuous overlap must not ignite a delayed burn.
            for (int frame = 0; frame < 180; frame++) {
                mario.collidedObjInteractTypes = INTERACT_FLAME;
                mario_update_soaked_status(&mario);
                enemy.oInteractStatus = INT_STATUS_NONE;
                EXPECT_EQ(interact_flame(&mario, INTERACT_FLAME, &enemy), TRUE);
                if (mario.action == ACT_BURNING_JUMP) {
                    act_burning_jump(&mario);
                } else if (mario.action == ACT_BURNING_FALL) {
                    act_burning_fall(&mario);
                }
                EXPECT_EQ(mario.action, actions[pose]);
                EXPECT_EQ(mario.health, 0x880);
                EXPECT_EQ(mario.hurtCounter, 0);
                EXPECT_EQ(mario.soakedTimer, 0);
            }

            // Leaving the flames ends protection; the next dry encounter burns.
            mario.collidedObjInteractTypes = 0;
            mario_update_soaked_status(&mario);
            mario.collidedObjInteractTypes = INTERACT_FLAME;
            mario_update_soaked_status(&mario);
            enemy.oInteractStatus = INT_STATUS_NONE;
            EXPECT_EQ(interact_flame(&mario, INTERACT_FLAME, &enemy), TRUE);
            EXPECT_EQ(mario.action, pose == 2 ? ACT_BURNING_FALL : ACT_BURNING_JUMP);
            for (int frame = 0; frame < 18; frame++) {
                if (mario.action == ACT_BURNING_FALL) {
                    act_burning_fall(&mario);
                } else {
                    act_burning_jump(&mario);
                }
            }
            EXPECT_EQ(mario.health, 0x780);

            // Becoming Soaked during a burn also survives continued overlap.
            mario_apply_soaked_status(&mario);
            mario_update_soaked_status(&mario);
            EXPECT_EQ(mario.action, ACT_FREEFALL);
            for (int frame = 0; frame < 3; frame++) {
                mario_update_soaked_status(&mario);
                enemy.oInteractStatus = INT_STATUS_NONE;
                EXPECT_EQ(interact_flame(&mario, INTERACT_FLAME, &enemy), TRUE);
                EXPECT_EQ(mario.action, ACT_FREEFALL);
                EXPECT_EQ(mario.health, 0x780);
            }
            mario.collidedObjInteractTypes = 0;
            mario_update_soaked_status(&mario);
        }
    }

    // Vanish cannot keep protection acquired from a previous Soaked contact.
    begin(bhvGoomba);
    mario.flags = MARIO_NORMAL_CAP | MARIO_CAP_ON_HEAD;
    mario.collidedObjInteractTypes = INTERACT_FLAME;
    mario_apply_soaked_status(&mario);
    mario_update_soaked_status(&mario);
    EXPECT_EQ(interact_flame(&mario, INTERACT_FLAME, &enemy), TRUE);
    mario.flags |= MARIO_VANISH_CAP;
    mario_update_soaked_status(&mario);
    enemy.oInteractStatus = INT_STATUS_NONE;
    EXPECT_EQ(interact_flame(&mario, INTERACT_FLAME, &enemy), TRUE);
    EXPECT_EQ(mario.action, ACT_BURNING_JUMP);
    return TRUE;
}

static int vanish_fire(void) {
    begin(bhvGoomba);
    enemy.oInteractType = INTERACT_FLAME;
    mario.flags = MARIO_WING_CAP | MARIO_NORMAL_CAP | MARIO_CAP_ON_HEAD;
    mario.capTimer = 90;
    mario.soakedTimer = 150;
    mario.soakedThisUpdate = TRUE;
    EXPECT_EQ(interact_flame(&mario, INTERACT_FLAME, &enemy), TRUE);
    EXPECT_EQ(mario.action, ACT_WALKING);
    EXPECT_EQ(mario.soakedThisUpdate, FALSE);
    EXPECT_EQ(mario.soakedTimer, 0);
    EXPECT_EQ(mario.flags & MARIO_WING_CAP, MARIO_WING_CAP);
    EXPECT_EQ(mario.capTimer, 90);
    begin(bhvGoomba);
    enemy.oInteractType = INTERACT_FLAME;
    mario.flags = MARIO_VANISH_CAP | MARIO_NORMAL_CAP | MARIO_CAP_ON_HEAD;
    mario.capTimer = 90;
    mario.action = ACT_WALKING;
    EXPECT_EQ(mario_vanish_ignores_hostile_contact(&mario, &enemy, INTERACT_FLAME), FALSE);
    EXPECT_EQ(interact_flame(&mario, INTERACT_FLAME, &enemy), TRUE);
    EXPECT_EQ(mario.action, ACT_BURNING_JUMP);
    EXPECT_EQ(mario.flags & MARIO_VANISH_CAP, MARIO_VANISH_CAP);
    EXPECT_EQ(mario.capTimer, 90);
    EXPECT_EQ(enemy.oInteractStatus, INT_STATUS_INTERACTED);
    begin(bhvGoomba);
    enemy.oInteractType = INTERACT_FLAME;
    mario.flags = MARIO_VANISH_CAP | MARIO_NORMAL_CAP | MARIO_CAP_ON_HEAD;
    mario.waterLevel = 100;
    EXPECT_EQ(interact_flame(&mario, INTERACT_FLAME, &enemy), FALSE);
    EXPECT_EQ(enemy.oInteractStatus, INT_STATUS_INTERACTED);
    EXPECT_EQ(mario.action, ACT_WALKING);
    begin(bhvGoomba);
    enemy.oInteractType = INTERACT_FLAME;
    mario.flags = MARIO_VANISH_CAP | MARIO_METAL_CAP | MARIO_NORMAL_CAP | MARIO_CAP_ON_HEAD;
    EXPECT_EQ(interact_flame(&mario, INTERACT_FLAME, &enemy), FALSE);
    EXPECT_EQ(mario.action, ACT_WALKING);
    begin(bhvGoomba);
    enemy.oInteractType = INTERACT_FLAME;
    mario.flags = MARIO_VANISH_CAP | MARIO_NORMAL_CAP | MARIO_CAP_ON_HEAD;
    sInvulnerable = TRUE;
    EXPECT_EQ(interact_flame(&mario, INTERACT_FLAME, &enemy), FALSE);
    EXPECT_EQ(mario.action, ACT_WALKING);
    return TRUE;
}

static int land_actions_underwater(void) {
    const u32 actions[] = { ACT_JUMP,
                            ACT_DOUBLE_JUMP,
                            ACT_TRIPLE_JUMP,
                            ACT_LONG_JUMP,
                            ACT_BACKFLIP,
                            ACT_SIDE_FLIP,
                            ACT_DIVE,
                            ACT_FORWARD_ROLLOUT,
                            ACT_BACKWARD_ROLLOUT,
                            ACT_WALL_KICK_AIR,
                            ACT_SLIDE_KICK,
                            ACT_JUMP_KICK,
                            ACT_GROUND_POUND,
                            ACT_HOLD_JUMP,
                            ACT_HOLD_FREEFALL,
                            ACT_RIDING_SHELL_JUMP };
    const u32 caps[] = { MARIO_METAL_CAP, MARIO_VANISH_CAP, MARIO_METAL_CAP | MARIO_VANISH_CAP };
    for (unsigned c = 0; c < sizeof(caps) / sizeof(caps[0]); c++) {
        for (unsigned a = 0; a < sizeof(actions) / sizeof(actions[0]); a++) {
            begin(bhvGoomba);
            mario.flags = caps[c];
            mario.action = actions[a];
            mario.waterLevel = 500;
            mario.pos[1] = 100;
            mario.floorHeight = 0;
            mario.vel[0] = 12;
            mario.vel[1] = -8;
            mario.vel[2] = 16;
            mario.forwardVel = 20;
            EXPECT_EQ(check_common_airborne_cancels(&mario), FALSE);
            EXPECT_EQ(mario.action, actions[a]);
            EXPECT_EQ(mario.vel[1], -8);
            EXPECT_EQ(mario.forwardVel, 20);
            mario.flags = 0;
            EXPECT_EQ(check_common_airborne_cancels(&mario), TRUE);
            EXPECT_EQ(mario.action, ACT_WATER_PLUNGE);
        }
    }
    const u32 automatic[] = { ACT_HOLDING_POLE, ACT_TOP_OF_POLE, ACT_HANGING, ACT_LEDGE_GRAB };
    for (unsigned a = 0; a < sizeof(automatic) / sizeof(automatic[0]); a++) {
        begin(bhvGoomba);
        mario.flags = MARIO_METAL_CAP;
        mario.action = automatic[a];
        mario.waterLevel = 500;
        mario.pos[1] = 100;
        EXPECT_EQ(check_common_automatic_cancels(&mario), FALSE);
        EXPECT_EQ(mario.action, automatic[a]);
        mario.flags = 0;
        EXPECT_EQ(check_common_automatic_cancels(&mario), TRUE);
        EXPECT_EQ(mario.action, ACT_WATER_PLUNGE);
    }
    return TRUE;
}

static int breakdance_bully_direction(void) {
    const f32 positions[][2] = { { 0, 100 },  { 100, 0 }, { 0, -100 },
                                 { -100, 0 }, { 70, 70 }, { -70, -70 } };
    const u32 actions[] = { ACT_PUNCHING, ACT_MOVE_PUNCHING };
    for (unsigned action = 0; action < sizeof(actions) / sizeof(*actions); action++) {
        for (int big = FALSE; big <= TRUE; big++) {
            for (unsigned direction = 0; direction < sizeof(positions) / sizeof(*positions);
                 direction++) {
                for (int facing = 0; facing < 4; facing++) {
                    begin(bhvGoomba);
                    enemy.oInteractType = INTERACT_BULLY;
                    enemy.oPosX = 300 + positions[direction][0];
                    enemy.oPosZ = -200 + positions[direction][1];
                    enemy.hitboxRadius = big ? 120 : 60;
                    mario.pos[0] = 300;
                    mario.pos[2] = -200;
                    mario.flags = 0;
                    mario.action = actions[action];
                    mario.actionArg = ACT_ARG_PUNCH_SEQUENCE_BREAKDANCE;
                    mario.faceAngle[1] = (s16) (facing * 0x4000);
                    mario.forwardVel = 24.0f;
                    contactInteraction = INT_TRIP;
                    EXPECT_EQ(interact_bully(&mario, INTERACT_BULLY, &enemy), TRUE);
                    EXPECT_EQ((s16) enemy.oMoveAngleYaw,
                              atan2s(positions[direction][1], positions[direction][0]));
                    EXPECT_EQ(absf(enemy.oForwardVel - 3392.0f / enemy.hitboxRadius) < 0.01f, TRUE);
                    EXPECT_EQ(mario.forwardVel, 24.0f);
                    EXPECT_EQ(bullyAttacks, 1);
                    EXPECT_EQ(bullyImpulses, 0);
                }
            }
        }
    }
    begin(bhvGoomba);
    mario.flags = 0;
    mario.action = ACT_PUNCHING;
    mario.actionArg = 0;
    mario.faceAngle[1] = -0x4000;
    contactInteraction = INT_PUNCH;
    sInvulnerable = TRUE;
    interact_bully(&mario, INTERACT_BULLY, &enemy);
    EXPECT_EQ((s16) enemy.oMoveAngleYaw, -0x4000);
    return TRUE;
}

static int bully_dive_once(void) {
    const u32 caps[] = { 0, MARIO_VANISH_CAP, MARIO_METAL_CAP };
    for (int cap = 0; cap < 3; cap++) {
        for (int big = FALSE; big <= TRUE; big++) {
            for (int chill = FALSE; chill <= TRUE; chill++) {
                begin(bhvGoomba);
                enemy.oInteractType = INTERACT_BULLY;
                enemy.oBehParams2ndByte = big ? BULLY_BP_SIZE_BIG : BULLY_BP_SIZE_SMALL;
                enemy.hitboxRadius = big ? 120 : 60;
                enemy.oBullySubtype = chill ? BULLY_STYPE_CHILL : 0;
                struct Object other = enemy;
                mario.flags = caps[cap];
                contactInteraction = INT_FAST_ATTACK_OR_SHELL;
                sInvulnerable = TRUE;
                for (int cycle = 0; cycle < 3; cycle++) {
                    mario.action = ACT_DIVE;
                    mario.actionState = 0;
                    update_attack_hit_phase(&mario);
                    mario.actionState = 3;
                    for (int phase = 0; phase < 2; phase++) {
                        mario.action = phase ? ACT_DIVE_SLIDE : ACT_DIVE;
                        for (int tick = 0; tick < 8; tick++) {
                            update_attack_hit_phase(&mario);
                            enemy.oInteractStatus = 0;
                            enemy.oForwardVel = 7;
                            mario.forwardVel = 40;
                            s32 firstHit = phase == 0 && tick == 0;
                            EXPECT_EQ(interact_bully(&mario, INTERACT_BULLY, &enemy), firstHit);
                            EXPECT_EQ(enemy.oForwardVel, firstHit ? 3392.0f / enemy.hitboxRadius : 7);
                            EXPECT_EQ(mario.forwardVel, firstHit ? -16 : 40);
                            EXPECT_EQ(bullyAttacks, cycle * 2 + 1);
                            EXPECT_EQ(bullyBounces, cycle * 2 + 1);
                            EXPECT_EQ(rumbleRequests, cycle * 2 + 1);
                        }
                    }
                    interact_bully(&mario, INTERACT_BULLY, &other);
                    EXPECT_EQ(bullyAttacks, cycle * 2 + 2);
                }
                mario_forget_attack_object(&enemy);
                interact_bully(&mario, INTERACT_BULLY, &enemy);
                EXPECT_EQ(bullyAttacks, 7);
                // Existing recovery immunity still pushes a duplicate contact away.
                mario.bullyKnockbackImmunityFrames = 2;
                sInvulnerable = FALSE;
                s32 pushes = contactPushes;
                interact_bully(&mario, INTERACT_BULLY, &enemy);
                EXPECT_EQ(contactPushes, pushes + 1);
                EXPECT_EQ(bullyAttacks, 7);
                EXPECT_EQ(bullyImpulses, 0);
                if (caps[cap] == 0) {
                    mario.bullyKnockbackImmunityFrames = 0;
                    EXPECT_EQ(interact_bully(&mario, INTERACT_BULLY, &enemy), TRUE);
                    EXPECT_EQ(bullyImpulses, 1);
                    EXPECT_EQ(mario.action, ACT_BACKWARD_GROUND_KB);
                    EXPECT_EQ(bullyAttacks, 7);
                }
                mario.action = ACT_FREEFALL;
                update_attack_hit_phase(&mario);
                EXPECT_EQ(punch_hit_object(&enemy), FALSE);
            }
        }
    }
    return TRUE;
}

static int bully_all_attacks_once(void) {
    const u32 actions[] = { ACT_PUNCHING,     ACT_MOVE_PUNCHING, ACT_JUMP_KICK,       ACT_SLIDE_KICK,
                            ACT_GROUND_POUND, ACT_TWIRLING,      ACT_WATER_DOWN_BURST };
    const u32 contacts[] = { INT_PUNCH,
                             INT_TRIP,
                             INT_KICK,
                             INT_SLIDE_KICK,
                             INT_GROUND_POUND_OR_TWIRL,
                             INT_GROUND_POUND_OR_TWIRL,
                             INT_GROUND_POUND_OR_TWIRL };
    const u32 landings[] = { ACT_PUNCHING,         ACT_MOVE_PUNCHING,     ACT_JUMP_KICK,
                             ACT_SLIDE_KICK_SLIDE, ACT_GROUND_POUND_LAND, ACT_TWIRL_LAND,
                             ACT_WATER_ACTION_END };
    for (int metal = FALSE; metal <= TRUE; metal++) {
        for (int big = FALSE; big <= TRUE; big++) {
            for (int chill = FALSE; chill <= TRUE; chill++) {
                for (unsigned attack = 0; attack < sizeof(actions) / sizeof(*actions); attack++) {
                    begin(bhvGoomba);
                    enemy.oInteractType = INTERACT_BULLY;
                    enemy.hitboxRadius = big ? 120 : 60;
                    enemy.oBullySubtype = chill ? BULLY_STYPE_CHILL : 0;
                    enemy.oBehParams2ndByte = big ? BULLY_BP_SIZE_BIG : BULLY_BP_SIZE_SMALL;
                    struct Object other = enemy;
                    mario.flags = metal ? MARIO_METAL_CAP : 0;
                    contactInteraction = contacts[attack];
                    sInvulnerable = TRUE;
                    for (int cycle = 0; cycle < 2; cycle++) {
                        mario.action = ACT_IDLE;
                        update_attack_hit_phase(&mario);
                        mario.action = actions[attack];
                        mario.actionTimer = 2;
                        if (attack < 2) {
                            mario_set_punch_hit_phase(attack + 1);
                        }
                        update_attack_hit_phase(&mario);
                        for (int tick = 0; tick < 8; tick++) {
                            enemy.oInteractStatus = 0;
                            enemy.oForwardVel = 7;
                            interact_bully(&mario, INTERACT_BULLY, &enemy);
                            EXPECT_EQ(bullyAttacks, cycle * 2 + 1);
                            EXPECT_EQ(enemy.oForwardVel, tick == 0 ? 3392.0f / enemy.hitboxRadius : 7);
                        }
                        mario.action = landings[attack];
                        mario.waterGroundPoundImpact = attack == 6;
                        update_attack_hit_phase(&mario);
                        interact_bully(&mario, INTERACT_BULLY, &enemy);
                        EXPECT_EQ(bullyAttacks, cycle * 2 + 1);
                        interact_bully(&mario, INTERACT_BULLY, &other);
                        EXPECT_EQ(bullyAttacks, cycle * 2 + 2);
                        mario.waterGroundPoundImpact = FALSE;
                    }
                    if (attack >= 2) {
                        mario.action = actions[attack];
                        mario.actionTimer = 9;
                        update_attack_hit_phase(&mario);
                        mario.actionTimer = 0;
                        update_attack_hit_phase(&mario);
                        interact_bully(&mario, INTERACT_BULLY, &enemy);
                        EXPECT_EQ(bullyAttacks, 5);
                    }
                }
            }
        }
    }
    // Continuous attacks re-arm individual targets only after physical separation.
    const u32 continuous[] = { ACT_RIDING_SHELL_GROUND, ACT_FLYING, ACT_WALKING };
    for (unsigned kind = 0; kind < sizeof(continuous) / sizeof(*continuous); kind++) {
        begin(bhvGoomba);
        enemy.oInteractType = INTERACT_BULLY;
        enemy.hitboxRadius = 60;
        struct Object other = enemy;
        mario.action = continuous[kind];
        mario.flags = kind == 2 ? MARIO_METAL_CAP : 0;
        mario.marioObj->numCollidedObjs = 2;
        mario.marioObj->collidedObjs[0] = &enemy;
        mario.marioObj->collidedObjs[1] = &other;
        contactInteraction = INT_FAST_ATTACK_OR_SHELL;
        sInvulnerable = TRUE;
        for (int tick = 0; tick < 8; tick++) {
            update_attack_hit_phase(&mario);
            interact_bully(&mario, INTERACT_BULLY, &enemy);
            interact_bully(&mario, INTERACT_BULLY, &other);
            EXPECT_EQ(bullyAttacks, 2);
        }
        mario.marioObj->numCollidedObjs = 1;
        mario.marioObj->collidedObjs[0] = &other;
        update_attack_hit_phase(&mario);
        mario.marioObj->numCollidedObjs = 2;
        mario.marioObj->collidedObjs[1] = &enemy;
        update_attack_hit_phase(&mario);
        interact_bully(&mario, INTERACT_BULLY, &enemy);
        interact_bully(&mario, INTERACT_BULLY, &other);
        EXPECT_EQ(bullyAttacks, 3);
    }
    return TRUE;
}

static int bowser_body_reactions(void) {
    const s32 actions[] = { BOWSER_ACT_DEFAULT, BOWSER_ACT_BIG_JUMP, BOWSER_ACT_QUICK_JUMP,
                            BOWSER_ACT_JUMP_ONTO_STAGE, BOWSER_ACT_CHARGE_MARIO };
    for (int capped = 0; capped < 2; capped++) {
        for (unsigned kind = 0; kind < sizeof(actions) / sizeof(*actions); kind++) {
            for (int close = 0; close < 2; close++) {
                begin(bhvBowserBodyAnchor);
                enemy.parentObj = &parent;
                parent.oAction = actions[kind];
                parent.oPosX = close ? 100 : 300;
                enemy.oInteractionSubtype = bowserAttackSubtypes[0];
                enemy.oInteractType = INTERACT_DAMAGE;
                mario.flags = (capped ? MARIO_NORMAL_CAP | MARIO_CAP_ON_HEAD : 0) | MARIO_SPECIAL_CAPS;
                mario.hurtCounter = 4;
                mario.heldObj = &hole;
                int launch = close || (kind >= 1 && kind <= 3);
                int damage = launch || kind == 4 ? 4 : 2;
                EXPECT_EQ(interact_damage(&mario, INTERACT_DAMAGE, &enemy), TRUE);
                EXPECT_EQ(mario.hurtCounter, 4 + 4 * MIN(damage * (capped ? 1 : 2), 7));
                EXPECT_EQ(mario.flags & MARIO_SPECIAL_CAPS, 0);
                EXPECT_EQ(mario.heldObj, NULL);
                EXPECT_EQ(capSavePositions, capped && (launch || kind == 4) ? 1 : 0);
                EXPECT_EQ(mario.flags & MARIO_CAP_ON_HEAD,
                          capped && !launch && kind != 4 ? MARIO_CAP_ON_HEAD : 0);
                if (capSavePositions != 0) {
                    EXPECT_EQ(spawned[0].oForwardVel, launch ? 30 : 90);
                }
                if (launch) {
                    EXPECT_EQ(mario.action, ACT_HARD_BACKWARD_AIR_KB);
                    EXPECT_EQ(mario.forwardVel, -60);
                    EXPECT_EQ(mario.vel[1], 50);
                } else if (kind == 4) {
                    EXPECT_EQ(mario.action, ACT_HARD_BACKWARD_GROUND_KB);
                }
            }
        }
    }
    return TRUE;
}
static int bowser_impact_queue(void) {
    const u32 capActions[] = { ACT_PUTTING_ON_CAP, ACT_PUTTING_ON_CAP_AIR };
    for (unsigned action = 0; action < sizeof(capActions) / sizeof(*capActions); action++) {
        begin(bhvGoomba);
        mario.action = capActions[action];
        mario.flags = MARIO_NORMAL_CAP | MARIO_CAP_IN_HAND;
        enemy.oDamageOrCoinValue = 3;
        enemy.oInteractType = INTERACT_DAMAGE;
        mario.interactObj = &enemy;
        EXPECT_EQ(take_damage_from_interact_object(&mario), 3);
        EXPECT_EQ(mario.hurtCounter, 12);
        EXPECT_EQ(mario.flags & MARIO_CAP_ON_HEAD, 0);
        for (int heavy = 0; heavy < 2; heavy++) {
            begin(bhvGoomba);
            mario.action = capActions[action];
            mario.flags = MARIO_NORMAL_CAP | MARIO_CAP_IN_HAND;
            marioObj.oInteractStatus = heavy ? INT_STATUS_MARIO_KNOCKBACK_DMG : 0;
            mario_bowser_impact_knockback(&mario, action != 0);
            EXPECT_EQ(mario.hurtCounter, heavy ? 8 : 4);
        }
    }

    for (int capped = 0; capped < 2; capped++) {
        for (int near = 0; near < 2; near++) {
            begin(bhvGoomba);
            sTwirlContact.reboundHiddenTimer = 0;
            mario.flags = (capped ? MARIO_NORMAL_CAP | MARIO_CAP_ON_HEAD : 0) | MARIO_SPECIAL_CAPS;
            enemy.oPosX = near ? 1249 : 1250;
            enemy.oPosY = 0;
            mario_queue_bowser_impact(&enemy);
            EXPECT_EQ(marioObj.oInteractStatus,
                      near ? INT_STATUS_MARIO_KNOCKBACK_DMG : INT_STATUS_MARIO_STUNNED);
            EXPECT_EQ(mario.bowserImpactPos[0], enemy.oPosX);
            mario.hurtCounter = 8;
            mario.action = ACT_SHOCKWAVE_BOUNCE;
            act_shockwave_bounce(&mario);
            EXPECT_EQ(mario.hurtCounter, 8 + 4 * (near ? 2 : 1) * (capped ? 1 : 2));
            EXPECT_EQ(mario.forwardVel, near ? -40 : -16);
            EXPECT_EQ(mario.action, near ? ACT_HARD_BACKWARD_GROUND_KB : ACT_SOFT_BACKWARD_GROUND_KB);
            EXPECT_EQ(capSavePositions, capped && near ? 1 : 0);
        }
    }
    begin(bhvGoomba);
    sTwirlContact.reboundHiddenTimer = 0;
    mario_queue_bowser_impact(&enemy); // Near takes priority over a later far wave.
    enemy.oPosX = 1500;
    mario_queue_bowser_impact(&enemy);
    EXPECT_EQ(mario.bowserImpactPos[0], 100);
    for (int beyond = 0; beyond < 2; beyond++) {
        begin(bhvGoomba);
        sTwirlContact.reboundHiddenTimer = 0;
        enemy.oPosX = beyond ? 10000 : 9999;
        enemy.oPosY = 0;
        mario_queue_bowser_impact(&enemy);
        EXPECT_EQ(marioObj.oInteractStatus, beyond ? 0 : INT_STATUS_MARIO_STUNNED);
    }
    for (int grace = 0; grace < 3; grace++) {
        begin(bhvGoomba);
        sTwirlContact.reboundHiddenTimer = grace == 2 ? 3 : 0;
        mario.action = grace == 0 ? ACT_FREEFALL : ACT_WALKING;
        mario.invincTimer = grace == 1 ? 10 : 0;
        mario_queue_bowser_impact(&enemy);
        EXPECT_EQ(marioObj.oInteractStatus, 0);
    }
    sTwirlContact.reboundHiddenTimer = 0;
    return TRUE;
}
static int bowser_wave_reactions(void) {
    for (int capped = 0; capped < 2; capped++) {
        for (int near = 0; near < 2; near++) {
            begin(bhvGoomba);
            mario.flags = capped ? MARIO_NORMAL_CAP | MARIO_CAP_ON_HEAD : 0;
            enemy.oPosX = near ? 475 : 950;
            enemy.oDistanceToMario = enemy.oPosX;
            enemy.oOpacity = 255;
            // At a fixed distance the faster outer ring passes first.
            enemy.oTimer = near ? 10 : 20;
            bhv_bowser_shock_wave_loop();
            EXPECT_EQ(marioObj.oInteractStatus, INT_STATUS_MARIO_STUNNED);
            mario.action = ACT_SHOCKWAVE_BOUNCE;
            act_shockwave_bounce(&mario);
            EXPECT_EQ(mario.hurtCounter, capped ? 4 : 8);
            EXPECT_EQ(mario.action, ACT_SOFT_BACKWARD_GROUND_KB);
            EXPECT_EQ(capSavePositions, 0);
            // A vulnerable Mario later meets the inner ring at the same distance.
            mario.action = ACT_WALKING;
            mario.actionTimer = 0;
            mario.invincTimer = 0;
            marioObj.oInteractStatus = 0;
            enemy.oBehParams2ndByte = BOWSER_SHOCKWAVE_DELAYED;
            enemy.oTimer = (near ? 10 : 20) + BOWSER_SHOCKWAVE_DELAY_FRAMES;
            bhv_bowser_shock_wave_loop();
            EXPECT_EQ(marioObj.oInteractStatus, INT_STATUS_MARIO_KNOCKBACK_DMG);
            mario.action = ACT_SHOCKWAVE_BOUNCE;
            act_shockwave_bounce(&mario);
            EXPECT_EQ(mario.hurtCounter, capped ? 12 : 24);
            EXPECT_EQ(mario.action, ACT_HARD_BACKWARD_GROUND_KB);
            EXPECT_EQ(capSavePositions, capped ? 1 : 0);
            if (capped) {
                EXPECT_EQ(spawned[0].oForwardVel, 42);
            }
            EXPECT_EQ(mario.flags & MARIO_CAP_ON_HEAD, 0);
            marioObj.oInteractStatus = 0;
            bhv_bowser_shock_wave_loop();
            EXPECT_EQ(marioObj.oInteractStatus, 0);
        }
    }
    begin(bhvGoomba);
    enemy.oTimer = 20;
    enemy.oDistanceToMario = 1050;
    enemy.oOpacity = 255;
    bhv_bowser_shock_wave_loop();
    EXPECT_EQ(marioObj.oInteractStatus, 0);
    const f32 distances[] = { 2941, 2953, 2955, 3485, 3487, 3499 };
    const u32 hits[] = { 0, 0, INT_STATUS_MARIO_STUNNED, INT_STATUS_MARIO_STUNNED, 0, 0 };
    for (unsigned i = 0; i < sizeof(distances) / sizeof(*distances); i++) {
        begin(bhvGoomba);
        enemy.oTimer = 69;
        enemy.oOpacity = 255;
        enemy.oDistanceToMario = distances[i];
        bhv_bowser_shock_wave_loop();
        EXPECT_EQ(marioObj.oInteractStatus, hits[i]);
        marioObj.oInteractStatus = 0;
        enemy.oTimer = 70;
        bhv_bowser_shock_wave_loop();
        EXPECT_EQ(marioObj.oInteractStatus, 0);
    }
    // Light variants use only the outward ring.
    begin(bhvGoomba);
    mario.flags = MARIO_NORMAL_CAP | MARIO_CAP_ON_HEAD;
    enemy.oBehParams2ndByte = BOWSER_SHOCKWAVE_LIGHT;
    enemy.oTimer = 10;
    enemy.oDistanceToMario = 475;
    enemy.oOpacity = 255;
    bhv_bowser_shock_wave_loop();
    EXPECT_EQ(marioObj.oInteractStatus, INT_STATUS_MARIO_STUNNED);
    mario.action = ACT_SHOCKWAVE_BOUNCE;
    act_shockwave_bounce(&mario);
    EXPECT_EQ(mario.hurtCounter, 4);
    EXPECT_EQ(mario.action, ACT_SOFT_BACKWARD_GROUND_KB);
    EXPECT_EQ(capSavePositions, 0);
    marioObj.oInteractStatus = 0;
    mario.invincTimer = 0;
    enemy.oTimer = 20;
    bhv_bowser_shock_wave_loop();
    EXPECT_EQ(marioObj.oInteractStatus, 0);
    begin(bhvGoomba);
    enemy.oBehParams2ndByte = BOWSER_SHOCKWAVE_DELAYED;
    enemy.oDistanceToMario = 475;
    for (int frame = 0; frame < BOWSER_SHOCKWAVE_DELAY_FRAMES; frame++) {
        enemy.oTimer = frame;
        bhv_bowser_shock_wave_loop();
        EXPECT_EQ(enemy.oOpacity, 0);
        EXPECT_EQ(marioObj.oInteractStatus, 0);
    }
    enemy.oTimer = BOWSER_SHOCKWAVE_DELAY_FRAMES;
    bhv_bowser_shock_wave_loop();
    EXPECT_EQ(enemy.oOpacity > 250, TRUE);
    enemy.oTimer = BOWSER_SHOCKWAVE_DELAY_FRAMES + 10;
    bhv_bowser_shock_wave_loop();
    EXPECT_EQ(marioObj.oInteractStatus, INT_STATUS_MARIO_KNOCKBACK_DMG);
    marioObj.oInteractStatus = 0;
    enemy.oTimer = BOWSER_SHOCKWAVE_DELAY_FRAMES + 20;
    bhv_bowser_shock_wave_loop();
    EXPECT_EQ(marioObj.oInteractStatus, 0);
    marioObj.oInteractStatus = 0;
    enemy.oTimer = BOWSER_SHOCKWAVE_DELAY_FRAMES + 70;
    bhv_bowser_shock_wave_loop();
    EXPECT_EQ(marioObj.oInteractStatus, 0);
    const f32 orderedDistances[] = { 475, 950, 1900 };
    for (unsigned distance = 0; distance < 3; distance++) {
        int firstLight = -1, firstHeavy = -1;
        for (int frame = 0; frame < 80; frame++) {
            for (int heavy = 0; heavy < 2; heavy++) {
                begin(bhvGoomba);
                enemy.oBehParams2ndByte = heavy ? BOWSER_SHOCKWAVE_DELAYED : BOWSER_SHOCKWAVE_LIGHT;
                enemy.oDistanceToMario = orderedDistances[distance];
                enemy.oTimer = frame;
                bhv_bowser_shock_wave_loop();
                if (marioObj.oInteractStatus != 0) {
                    EXPECT_EQ(marioObj.oInteractStatus,
                              heavy ? INT_STATUS_MARIO_KNOCKBACK_DMG : INT_STATUS_MARIO_STUNNED);
                    int *first = heavy ? &firstHeavy : &firstLight;
                    if (*first < 0) {
                        *first = frame;
                    }
                }
            }
        }
        EXPECT_EQ(firstLight >= 0, TRUE);
        EXPECT_EQ(firstHeavy, firstLight + BOWSER_SHOCKWAVE_DELAY_FRAMES);
    }
    for (int wave = 0; wave < 3; wave++) {
        begin(bhvGoomba);
        enemy.oBehParams2ndByte = wave < 2 ? BOWSER_SHOCKWAVE_LIGHT : BOWSER_SHOCKWAVE_HEAVY;
        enemy.oBowserTimer = wave * BOWSER_SHOCKWAVE_DELAY_FRAMES;
        enemy.oDistanceToMario = 475;
        for (int frame = 0; frame < enemy.oBowserTimer; frame++) {
            enemy.oTimer = frame;
            bhv_bowser_shock_wave_loop();
            EXPECT_EQ(marioObj.oInteractStatus, 0);
            EXPECT_EQ(enemy.oOpacity, 0);
        }
        enemy.oTimer = enemy.oBowserTimer + 10;
        bhv_bowser_shock_wave_loop();
        EXPECT_EQ(marioObj.oInteractStatus,
                  wave < 2 ? INT_STATUS_MARIO_STUNNED : INT_STATUS_MARIO_KNOCKBACK_DMG);
    }
    return TRUE;
}

static int bowser_mine_impulse(void) {
    for (int side = -1; side <= 1; side += 2) {
        begin(bhvBowserBomb);
        enemy.oDamageOrCoinValue = BOWSER_BOMB_DAMAGE;
        mario.flags = MARIO_NORMAL_CAP | MARIO_CAP_ON_HEAD;
        mario.pos[0] = side * 1000;
        mario.input = INPUT_STOMPED;
        marioObj.oInteractStatus = INT_STATUS_MARIO_KNOCKBACK_DMG;
        interact_damage(&mario, INTERACT_DAMAGE, &enemy);
        EXPECT_EQ(mario.action, ACT_HARD_BACKWARD_AIR_KB);
        EXPECT_EQ(mario.forwardVel, -140);
        EXPECT_EQ(mario.actionArg, BOWSER_BOMB_DAMAGE | BOWSER_MINE_LAUNCH_ARG);
        EXPECT_EQ(mario.vel[1], 50);
        EXPECT_EQ(spawned[0].oForwardVel, 30);
        EXPECT_EQ(mario.forwardVel * sins(mario.faceAngle[1]) * mario.pos[0] < 0, TRUE);
        EXPECT_EQ(mario.input & INPUT_STOMPED, 0);
        EXPECT_EQ(marioObj.oInteractStatus & INT_STATUS_MARIO_KNOCKBACK_DMG, 0);
    }
    return TRUE;
}
static int bowser_throw_velocity(void) {
    begin(bhvBowserBomb);
    struct Object bowser = { .oHealth = 2 },
                  mine = { .behavior = bhvBowserBomb, .activeFlags = ACTIVE_FLAG_ACTIVE, .oPosX = 100 };
    gObjectLists[0].next = &mine;
    mine.header.next = (struct Object *) &gObjectLists[0];
    gCurrentObject = &bowser;
    bowser.oForwardVel = 100;
    EXPECT_EQ(bowser_check_hit_mine(), FALSE);
    bowser.oBowserStatus = BOWSER_STATUS_VALID_THROW;
    for (int speed = -24; speed <= 24; speed += 24) {
        bowser.oForwardVel = speed;
        EXPECT_EQ(bowser_check_hit_mine(), TRUE);
    }
    // Landing resets velocity after the strong throw has been checked.
    bowser.oMoveFlags = OBJ_MOVE_ON_GROUND;
    bowser.oAction = BOWSER_ACT_THROWN;
    bowser_act_thrown();
    EXPECT_EQ(bowser.oForwardVel, 0);
    EXPECT_EQ(bowser.oHealth, 1);
    EXPECT_EQ(bowser.oAction, BOWSER_ACT_HIT_MINE);
    EXPECT_EQ(bowser.oBowserStatus & BOWSER_STATUS_VALID_THROW, 0);
    // An eligible throw that lands without a mine expires before standing up.
    bowser.oBowserStatus = BOWSER_STATUS_VALID_THROW;
    bowser.oSubAction = 0;
    mine.oPosX = 1000;
    bowser_act_thrown();
    EXPECT_EQ(bowser.oBowserStatus & BOWSER_STATUS_VALID_THROW, 0);
    mine.oPosX = 100;
    EXPECT_EQ(bowser_check_hit_mine(), FALSE);
    return TRUE;
}

static int cap_preset_damage(void) {
    for (int metalOriginal = 0; metalOriginal < 2; metalOriginal++) {
        for (int vanishOriginal = 0; vanishOriginal < 2; vanishOriginal++) {
            sTestGameOptions[GO_METAL_CAP] = metalOriginal;
            sTestGameOptions[GO_VANISH_CAP] = vanishOriginal;
            for (int caps = 0; caps < 8; caps++) {
                u32 powers = (caps & 1 ? MARIO_WING_CAP : 0) | (caps & 2 ? MARIO_METAL_CAP : 0)
                             | (caps & 4 ? MARIO_VANISH_CAP : 0);
                int ignoredShock = (caps & 4) && vanishOriginal;
                int protectedShock = (caps & 2) && metalOriginal;
                int currentMetal = (caps & 2) && !metalOriginal;
                begin(bhvGoomba);
                mario.flags = powers | MARIO_NORMAL_CAP | MARIO_CAP_ON_HEAD;
                mario.capTimer = powers ? 90 : 0;
                mario.waterLevel = FLOOR_LOWER_LIMIT;
                enemy.oInteractType = INTERACT_SHOCK;
                enemy.oDamageOrCoinValue = 2;
                EXPECT_EQ(interact_shock(&mario, INTERACT_SHOCK, &enemy), !ignoredShock);
                EXPECT_EQ(mario.hurtCounter, ignoredShock || protectedShock ? 0
                                             : currentMetal                 ? 12
                                                                            : 8);
                EXPECT_EQ(mario.flags & MARIO_SPECIAL_CAPS,
                          ignoredShock || protectedShock ? powers : 0);
                EXPECT_EQ(mario.capTimer, ignoredShock || protectedShock ? 90 : 0);

                begin(bhvGoomba);
                mario.flags = powers | MARIO_NORMAL_CAP | MARIO_CAP_ON_HEAD;
                mario.action = ACT_FREEFALL;
                mario.vel[1] = -80.0f;
                mario.peakHeight = 4000.0f;
                check_fall_damage(&mario, ACT_HARD_BACKWARD_GROUND_KB);
                EXPECT_EQ(mario.hurtCounter, currentMetal ? 0 : 16);
                EXPECT_EQ(groundPoundImpacts, currentMetal ? 1 : 0);

                begin(bhvGoomba);
                mario.flags = powers | MARIO_NORMAL_CAP | MARIO_CAP_ON_HEAD;
                mario.waterLevel = FLOOR_LOWER_LIMIT;
                int immuneFire = (caps & 2) || ((caps & 4) && vanishOriginal);
                EXPECT_EQ(interact_flame(&mario, INTERACT_FLAME, &enemy), !immuneFire);
                EXPECT_EQ(mario.flags & MARIO_SPECIAL_CAPS, powers);
            }
        }
    }
    sTestGameOptions[GO_METAL_CAP] = sTestGameOptions[GO_VANISH_CAP] = 0;
    return TRUE;
}

static int cap_preset_lava(void) {
    for (int original = 0; original < 2; original++) {
        sTestGameOptions[GO_METAL_CAP] = original;
        begin(bhvGoomba);
        floorSurface.type = SURFACE_BURNING;
        mario.action = ACT_GROUND_POUND;
        mario.actionState = ACT_STATE_GROUND_POUND_FALL;
        mario.vel[1] = -50;
        mario.pos[1] = 10;
        suppliedStepY = 0;
        suppliedAirStep = AIR_STEP_LANDED;
        act_ground_pound(&mario);
        EXPECT_EQ(mario.action, original ? ACT_GROUND_POUND_LAND : ACT_LAVA_BOOST);
        EXPECT_EQ(mario.hurtCounter, 0);
        begin(bhvGoomba);
        floorSurface.type = SURFACE_BURNING;
        mario.action = ACT_LAVA_BOOST;
        mario.vel[1] = -20;
        suppliedStepY = 0;
        suppliedAirStep = AIR_STEP_LANDED;
        act_lava_boost(&mario);
        EXPECT_EQ(mario.vel[1], original ? 84 : 8);
        EXPECT_EQ(mario.hurtCounter, 0);
    }
    sTestGameOptions[GO_METAL_CAP] = 0;
    return TRUE;
}

static int cap_preset_knockback(void) {
    const u32 actions[] = { ACT_WALKING, ACT_FREEFALL, ACT_WATER_IDLE };
    const int damages[] = { 1, 2, 4 };
    for (int original = 0; original < 2; original++) {
        sTestGameOptions[GO_METAL_CAP] = original;
        for (int terrain = 0; terrain < 3; terrain++) {
            for (int side = 0; side < 2; side++) {
                for (int damage = 0; damage < 3; damage++) {
                    for (int big = 0; big < 2; big++) {
                        begin(bhvMadPiano);
                        enemy.oDamageOrCoinValue = damages[damage];
                        enemy.oInteractionSubtype = big ? INT_SUBTYPE_BIG_KNOCKBACK : 0;
                        mario.action = actions[terrain];
                        mario.faceAngle[1] = side ? 0xC000 : 0x4000;
                        mario.flags |= MARIO_CAP_ON_HEAD;
                        mario.forwardVel = 8;
                        EXPECT_EQ(interact_mr_blizzard(&mario, INTERACT_MR_BLIZZARD, &enemy), TRUE);
                        int tier = original ? damage : 0;
                        EXPECT_EQ(mario.action, side ? sForwardKnockbackActions[terrain][tier]
                                                     : sBackwardKnockbackActions[terrain][tier]);
                        EXPECT_EQ(absf(mario.forwardVel), original && big ? 40
                                                          : terrain == 2  ? 28
                                                                          : 16);
                        EXPECT_EQ(mario.hurtCounter, 0);
                    }
                }
            }
        }
    }
    sTestGameOptions[GO_METAL_CAP] = 0;
    return TRUE;
}

static int cap_preset_status(void) {
    for (int metalOriginal = 0; metalOriginal < 2; metalOriginal++) {
        for (int vanishOriginal = 0; vanishOriginal < 2; vanishOriginal++) {
            sTestGameOptions[GO_METAL_CAP] = metalOriginal;
            sTestGameOptions[GO_VANISH_CAP] = vanishOriginal;
            for (int caps = 0; caps < 8; caps++) {
                begin(bhvGoomba);
                mario.flags = MARIO_CAP_ON_HEAD | (caps & 1 ? MARIO_WING_CAP : 0)
                              | (caps & 2 ? MARIO_METAL_CAP : 0) | (caps & 4 ? MARIO_VANISH_CAP : 0);
                int currentMetal = (caps & 2) && !metalOriginal;
                int currentVanish = (caps & 4) && !vanishOriginal;
                EXPECT_EQ(!!mario_has_land_movement_underwater(&mario),
                          !!(currentMetal || currentVanish));
                EXPECT_EQ(!!mario_has_water_damage_protection(&mario),
                          !!(currentVanish || ((caps & 2) && metalOriginal)));
                mario.waterLevel = 500;
                mario_update_soaked_status(&mario);
                EXPECT_EQ(!!mario_is_soaked(&mario), !currentVanish);
                mario.waterLevel = FLOOR_LOWER_LIMIT;
                EXPECT_EQ(interact_tornado(&mario, INTERACT_TORNADO, &enemy), !currentMetal);
                EXPECT_EQ(mario.flags & MARIO_METAL_CAP, caps & 2 ? MARIO_METAL_CAP : 0);
            }
        }
    }
    sTestGameOptions[GO_METAL_CAP] = sTestGameOptions[GO_VANISH_CAP] = 0;
    return TRUE;
}

static int soaked_option_effects(void) {
    for (int enabled = 0; enabled < 2; enabled++) {
        sTestGameOptions[GO_SOAKED] = enabled;
        for (int water = 0; water < 2; water++) {
            begin(bhvGoomba);
            mario.flags = MARIO_NORMAL_CAP | MARIO_CAP_ON_HEAD | MARIO_METAL_CAP;
            mario.waterLevel = water ? 100 : FLOOR_LOWER_LIMIT;
            mario.pos[1] = 0;
            mario.soakedThisUpdate = TRUE;
            enemy.oInteractType = INTERACT_SHOCK;
            enemy.oDamageOrCoinValue = 2;
            EXPECT_EQ(interact_shock(&mario, INTERACT_SHOCK, &enemy), TRUE);
            EXPECT_EQ(mario.hurtCounter, enabled ? 24 : 12);
            EXPECT_EQ(mario.flags & MARIO_CAP_ON_HEAD, MARIO_CAP_ON_HEAD);
        }
        begin(bhvGoomba);
        mario.flags = MARIO_NORMAL_CAP | MARIO_CAP_ON_HEAD;
        mario.action = ACT_WALKING;
        mario.soakedThisUpdate = mario.soakedFlameProtection = TRUE;
        mario.vel[1] = -1;
        enemy.oInteractType = INTERACT_FLAME;
        EXPECT_EQ(interact_flame(&mario, INTERACT_FLAME, &enemy), TRUE);
        EXPECT_EQ(mario.action, enabled ? ACT_WALKING : ACT_BURNING_JUMP);
    }
    sTestGameOptions[GO_SOAKED] = TRUE;
    return TRUE;
}

static int fall_damage_option(void) {
    sTestGameOptions[GO_FALL_DAMAGE] = FALSE;
    for (int powers = 0; powers < 8; powers++) {
        for (int severe = 0; severe < 2; severe++) {
            for (int reaction = 0; reaction < 2; reaction++) {
                begin(bhvGoomba);
                mario.action = ACT_FREEFALL;
                mario.flags = MARIO_CAP_ON_HEAD | (powers & 1 ? MARIO_WING_CAP : 0)
                              | (powers & 2 ? MARIO_METAL_CAP : 0)
                              | (powers & 4 ? MARIO_VANISH_CAP : 0);
                mario.peakHeight = severe ? 4000 : 1200;
                mario.vel[1] = -80;
                EXPECT_EQ(check_fall_damage(&mario, reaction ? ACT_HARD_FORWARD_GROUND_KB
                                                             : ACT_HARD_BACKWARD_GROUND_KB),
                          FALSE);
                EXPECT_EQ(mario.hurtCounter, 0);
                EXPECT_EQ(mario.action, ACT_FREEFALL);
                EXPECT_EQ(damageSounds, 0);
                EXPECT_EQ(groundPoundImpacts, (powers & 2) != 0);
                EXPECT_EQ(mario.fastMetalFallSquish, (powers & 2) && severe);
            }
        }
    }
    sTestGameOptions[GO_FALL_DAMAGE] = TRUE;
    begin(bhvGoomba);
    mario.action = ACT_FREEFALL;
    mario.flags = MARIO_CAP_ON_HEAD;
    mario.peakHeight = FALL_DAMAGE_HEIGHT_LARGE + 1;
    mario.vel[1] = -80;
    EXPECT_EQ(check_fall_damage(&mario, ACT_HARD_BACKWARD_GROUND_KB), TRUE);
    EXPECT_EQ(mario.hurtCounter, 16);
    return TRUE;
}
static int original_bowser_damage(void) {
    for (int cap = 0; cap < 8; cap++) {
        for (int hit = 0; hit < 3; hit++) {
            begin(bhvBowserBodyAnchor);
            sTestGameOptions[GO_BOWSER_FIGHTS] = 1;
            u32 flags = MARIO_NORMAL_CAP | MARIO_CAP_ON_HEAD;
            if (cap & 1) {
                flags |= MARIO_WING_CAP;
            }
            if (cap & 2) {
                flags |= MARIO_METAL_CAP;
            }
            if (cap & 4) {
                flags |= MARIO_VANISH_CAP;
            }
            mario.flags = flags;
            mario.capTimer = 123;
            mario.action = ACT_SHOCKWAVE_BOUNCE;
            marioObj.oInteractStatus = hit == 0   ? INT_STATUS_MARIO_SHOCKWAVE
                                       : hit == 1 ? INT_STATUS_MARIO_KNOCKBACK_DMG
                                                  : INT_STATUS_MARIO_STUNNED;
            act_shockwave_bounce(&mario);
            EXPECT_EQ(mario.action, hit == 0   ? ACT_SHOCKED
                                    : hit == 1 ? ACT_BACKWARD_GROUND_KB
                                               : ACT_SHOCKWAVE_BOUNCE);
            EXPECT_EQ(mario.hurtCounter, hit == 0 ? 4 : hit == 1 ? 12 : 0);
            EXPECT_EQ(mario.flags, flags);
            EXPECT_EQ(mario.capTimer, 123);
            EXPECT_EQ(capSavePositions, 0);
        }
    }
    return TRUE;
}

static int original_bowser_wave(void) {
    const float distances[] = { 180, 215, 241, 330, 400, 440, 481, 510 };
    for (unsigned i = 0; i < sizeof(distances) / sizeof(*distances); i++) {
        begin(bhvGoomba);
        sTestGameOptions[GO_BOWSER_FIGHTS] = 1;
        enemy.oTimer = 10;
        enemy.oOpacity = 255;
        enemy.oDistanceToMario = distances[i];
        bhv_bowser_shock_wave_loop();
        EXPECT_EQ(enemy.oAnimState, 1);
        EXPECT_EQ(enemy.oBowserShockWaveScale, 100);
        EXPECT_EQ(marioObj.oInteractStatus, i == 1 || i == 5 ? INT_STATUS_MARIO_SHOCKWAVE : 0);
    }
    return TRUE;
}

static int original_bowser_mines(void) {
    begin(bhvBowserBomb);
    sTestGameOptions[GO_BOWSER_FIGHTS] = 1;
    bhv_bowser_bomb_init();
    EXPECT_EQ(enemy.oInteractType, 0);
    EXPECT_EQ(enemy.oDamageOrCoinValue, 0);
    EXPECT_EQ(enemy.hitboxRadius, 40);
    struct Object boss = { .oHealth = 3 },
                  mine = { .behavior = bhvBowserBomb, .activeFlags = ACTIVE_FLAG_ACTIVE, .oPosX = 700 };
    gObjectLists[0].next = &mine;
    mine.header.next = (struct Object *) &gObjectLists[0];
    gCurrentObject = &boss;
    EXPECT_EQ(bowser_check_hit_mine(), TRUE);
    EXPECT_EQ(mine.oInteractStatus & INT_STATUS_HIT_MINE, INT_STATUS_HIT_MINE);
    mine.oPosX = 800;
    EXPECT_EQ(bowser_check_hit_mine(), FALSE);
    return TRUE;
}

static int infinite_health_fire_feedback(void) {
    sTestGameOptions[GO_CHEATS_ENABLED] = TRUE;
    sTestGameOptions[GO_INFINITE_HEALTH] = TRUE;
    sTestGameOptions[GO_KNOCKBACK] = FALSE;
    for (int bowser = FALSE; bowser <= TRUE; bowser++) {
        for (int airborne = 0; airborne < 3; airborne++) {
            for (int longer = FALSE; longer <= TRUE; longer++) {
                begin(bhvMadPiano);
                mario.flags = MARIO_CAP_ON_HEAD | MARIO_NORMAL_CAP;
                mario.action = airborne ? ACT_FREEFALL : ACT_WALKING;
                mario.vel[1] = airborne == 1 ? 12.0f : -12.0f;
                mario.invincTimer = longer ? 60 : 0;
                enemy.oInteractionSubtype = bowser ? INT_SUBTYPE_BOWSER_ATTACK : 0;
                EXPECT_EQ(interact_flame(&mario, INTERACT_FLAME, &enemy), TRUE);
                EXPECT_EQ(mario.invincTimer, longer ? 60 : 30);
                EXPECT_EQ(mario.action, airborne == 2 ? ACT_BURNING_FALL : ACT_BURNING_JUMP);
                EXPECT_EQ(mario.hurtCounter, 0);
                EXPECT_EQ(enemy.oInteractStatus, INT_STATUS_INTERACTED);
            }
        }
    }
    // A disabled health cheat or master retains the ordinary fire reaction.
    for (int master = FALSE; master <= TRUE; master++) {
        sTestGameOptions[GO_CHEATS_ENABLED] = master;
        sTestGameOptions[GO_INFINITE_HEALTH] = !master;
        begin(bhvMadPiano);
        mario.flags = MARIO_CAP_ON_HEAD | MARIO_NORMAL_CAP;
        EXPECT_EQ(interact_flame(&mario, INTERACT_FLAME, &enemy), TRUE);
        EXPECT_EQ(mario.invincTimer, 0);
        EXPECT_EQ(mario.action, ACT_BURNING_JUMP);
    }
    sTestGameOptions[GO_CHEATS_ENABLED] = TRUE;
    sTestGameOptions[GO_INFINITE_HEALTH] = TRUE;
    begin(bhvMadPiano);
    mario.flags = MARIO_CAP_ON_HEAD | MARIO_METAL_CAP;
    EXPECT_EQ(interact_flame(&mario, INTERACT_FLAME, &enemy), FALSE);
    EXPECT_EQ(mario.invincTimer, 0);
    begin(bhvMadPiano);
    mario.waterLevel = mario.pos[1] + 51.0f;
    EXPECT_EQ(interact_flame(&mario, INTERACT_FLAME, &enemy), FALSE);
    EXPECT_EQ(mario.invincTimer, 0);
    EXPECT_EQ(mario.action, ACT_WALKING);
    sTestGameOptions[GO_INFINITE_HEALTH] = FALSE;
    return TRUE;
}

int main(int argc, char **argv) {
    test_init(argc, argv);
    struct {
        const char *name;
        int (*run)(void);
    } cases[] = { { "metal.original_bowser_damage", original_bowser_damage },
                  { "metal.original_bowser_wave", original_bowser_wave },
                  { "metal.original_bowser_mines", original_bowser_mines },
                  { "metal.soaked_option_effects", soaked_option_effects },
                  { "metal.fall_damage_option", fall_damage_option },
                  { "metal.wet_fall_feedback", wet_fall_feedback },
                  { "metal.wet_fall_boundaries", wet_fall_boundaries },
                  { "metal.wet_fall_scream", wet_fall_scream },
                  { "metal.thicc_fall_shockwave", thicc_fall_shockwave },
                  { "metal.timeout_and_contacts", timing },
                  { "metal.wing_air_twirl_contacts", wing_air_twirl_contacts },
                  { "metal.eligibility", eligibility },
                  { "metal.enemy_roster", roster },
                  { "metal.stunned_water", stunned_water },
                  { "metal.stunned_swoop_floor_pose", stunned_swoop_floor_pose },
                  { "metal.grab_cleanup", grab_cleanup },
                  { "metal.short_contact_immunities", short_contact_immunities },
                  { "metal.bully_dive_once", bully_dive_once },
                  { "metal.bully_all_attacks_once", bully_all_attacks_once },
                  { "metal.breakdance_bully_direction", breakdance_bully_direction },
                  { "metal.heave_ho_fatal_fall_cap", heave_ho_fatal_fall_cap },
                  { "metal.bobomb_launch_before_explosion", bobomb_launch_before_explosion },
                  { "metal.explosion_immunity", explosion_immunity },
                  { "metal.cap_preset_damage", cap_preset_damage },
                  { "metal.cap_preset_status", cap_preset_status },
                  { "metal.cap_preset_knockback", cap_preset_knockback },
                  { "metal.cap_preset_lava", cap_preset_lava },
                  { "metal.soaked_blast_drying", soaked_blast_drying },
                  { "metal.bowser_cap_attacks", bowser_cap_attacks },
                  { "metal.bowser_body_reactions", bowser_body_reactions },
                  { "metal.bowser_impact_queue", bowser_impact_queue },
                  { "metal.bowser_wave_reactions", bowser_wave_reactions },
                  { "metal.bowser_mine_impulse", bowser_mine_impulse },
                  { "metal.bowser_throw_velocity", bowser_throw_velocity },
                  { "metal.bowser_mine_damage", bowser_mine_damage },
                  { "metal.bowser_mine_respawn", bowser_mine_respawn },
                  { "metal.bowser_mine_selection", bowser_mine_selection },
                  { "metal.bowser_twirl_damage", bowser_twirl_damage },
                  { "metal.damage_values", damage_values },
                  { "metal.light_enemy_knockback", light_enemy_knockback },
                  { "metal.breakdance_damage_windows", breakdance_damage_windows },
                  { "metal.breakdance_bowser_attacks", breakdance_bowser_attacks },
                  { "metal.wing_surface_soaked", wing_surface_soaked },
                  { "metal.water_bomb_soaks_on_hit", water_bomb_soaks_on_hit },
                  { "metal.disable_knockback", disable_knockback },
                  { "metal.shell_protection", shell_protection },
                  { "metal.bowser_no_knockback_progress", bowser_no_knockback_progress },
                  { "metal.bowser_queued_no_knockback", bowser_queued_no_knockback },
                  { "metal.infinite_health_reactions", infinite_health_reactions },
                  { "metal.infinite_health_fire_feedback", infinite_health_fire_feedback },
                  { "metal.environment_bypasses", environment_bypasses },
                  { "metal.wind_immunity", wind_immunity },
                  { "metal.solid_lava", solid_lava },
                  { "metal.soaked_lava", soaked_lava },
                  { "metal.soaked_cold_surface", soaked_cold_surface },
                  { "metal.ground_pound_lava", ground_pound_lava },
                  { "metal.held_a_lava_bounce", held_a_lava_bounce },
                  { "metal.capped_water_skip", capped_water_skip },
                  { "metal.slide_kick_liquid_effects", slide_kick_liquid_effects },
                  { "metal.thicc_slide_bounce_wave", thicc_slide_bounce_wave },
                  { "metal.thicc_punch_explosion_safe", thicc_punch_explosion_safe },
                  { "metal.heavy_fall_squash", heavy_fall_squash },
                  { "metal.light_fall_ground_pound", light_fall_ground_pound },
                  { "metal.heavy_fall_ground_pound", heavy_fall_ground_pound },
                  { "metal.vanish_pound_water_entry", vanish_pound_water_entry },
                  { "metal.wing_fall_damage", wing_fall_damage },
                  { "metal.wing_preset", wing_preset },
                  { "metal.wing_burial_boundaries", wing_burial_boundaries },
                  { "metal.wing_burial_landings", wing_burial_landings },
                  { "metal.vanish_sand", vanish_sand },
                  { "metal.capped_shocks", capped_shocks },
                  { "metal.shock_above_water_recovery", shock_above_water_recovery },
                  { "metal.repeated_water_shocks", repeated_water_shocks },
                  { "metal.soaked_fire_contact", soaked_fire_contact },
                  { "metal.vanish_fire", vanish_fire },
                  { "metal.land_actions_underwater", land_actions_underwater } };
    for (unsigned i = 0; i < sizeof(cases) / sizeof(cases[0]); i++) {
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
