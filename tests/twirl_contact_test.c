/* Production twirl targeting, action and interaction dispatch. Enemy launch,
 * classification, physics and native handlers are observed boundary doubles. */
#include <stdint.h>
#include <math.h>
#include "test_support.h"
typedef int16_t s16;
typedef uint16_t u16;
typedef int32_t s32;
typedef uint32_t u32;
typedef uint8_t u8;
typedef float f32;
typedef int BehaviorScript;
#define TRUE 1
#define FALSE 0
#define FLOOMBAS
#define SOUND_MARIO_ATTACKED 1
#define SOUND_ACTION_TWIRL 2
#define L_TRIG 0x40
#define ACTIVE_FLAG_DEACTIVATED 0
#define ACTIVE_FLAG_FAR_AWAY 2
#define ACTIVE_FLAG_IN_DIFFERENT_ROOM 4
#define NUM_OBJ_LISTS 2
#define OBJ_LIST_GENACTOR 0
#define ARRAY_COUNT(a) ((s32) (sizeof(a) / sizeof((a)[0])))
#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define absf(x) fabsf(x)
#define DEGREES(x) ((s16) ((x) * 65536.0f / 360.0f))
#define sqr(x) ((x) * (x))
#include "twirl_definitions.inc"
struct Animation {
    s16 loopEnd;
};
struct AnimInfo {
    struct Animation *curAnim;
    s16 animID, animFrame;
};
struct Object {
    struct {
        struct Object *next;
        struct {
            f32 cameraToObject[3];
            s16 angle[3];
            struct AnimInfo animInfo;
        } gfx;
    } header;
    const BehaviorScript *behavior;
    struct Object *prevObj, *parentObj;
    s32 oBowserGrabCooldown, oOpacity, oMoveAngleYaw;
    struct Object *collidedObjs[8];
    s32 numCollidedObjs;
    u32 collidedObjInteractTypes;
    s32 oDamageOrCoinValue, oAction, oRoom, oIntangibleTimer, oGroundTwirlStunTimer,
        oGroundTwirlContactRebound, oGoombaSize;
    s32 oGroundTwirlStunDuration, oHeldState, oKoopaMovementType, metalContactPendingDefeat, oHealth;
    u32 activeFlags, oGroundTwirlLastHitId, oInteractType, oInteractStatus, oInteractionSubtype;
    f32 oPosX, oPosY, oPosZ, hitboxRadius, hurtboxRadius, hitboxHeight, hitboxDownOffset, launchSpeed,
        oGroundTwirlBooPushSpeed;
    s32 enemy, large, immune, boo, blocked;
};
struct Controller {
    u16 buttonDown, buttonPressed;
};
struct MarioState {
    u8 framesSinceA;
    u32 action, prevAction, actionArg, flags, input, collidedObjInteractTypes;
    u16 actionState, actionTimer;
    s16 faceAngle[3], health, invincTimer, numCoins;
    u8 hurtCounter, bullyKnockbackImmunityFrames, heaveHoThrowRecoveryPending,
        heaveHoLaunchImmunityFrames, burnCoinJumpPending;
    u8 airTwirlContinue, airTwirlFlightRollFrame;
    u8 airTwirlJumpQueued;
    u8 waterGroundPoundImpact, metalFallGroundPoundImpact;
    s16 airTwirlCooldown;
    f32 peakHeight, forwardVel, vel[3], pos[3], quicksandDepth;
    struct Object *marioObj, *interactObj, *heldObj, *riddenObj;
    void *floor, *wall;
    struct Controller *controller;
};
static struct {
    s16 reboundHiddenTimer;
    s32 reboundActive, reboundTouchedEnemy, caplessContactDamaged;
} sTwirlContact;
static struct {
    s32 cooldown, cooldownResetByContact, inputQueued, bufferedInputFrames;
    u32 bufferedInput, id;
    f32 startSpeed;
    s16 visualStartYaw, visualPhaseYaw, visualPrevFaceYaw;
    s32 visualTargetOffset, visualWallBounce;
    f32 bounceBonus, appliedBonus;
    s32 boostAge, hasBounced;
    u8 wallBounceCount;
    s32 wallContact;
    s16 wallContactYaw;
    s32 earlySpeedCapActive;
    f32 earlySpeedCap;
    s32 lHeldThroughout, heldPoseFrames;
} sGroundTwirl;
static struct {
    s32 phase, attackImpactDone;
    s32 hitObjectCount, hitWallCount;
    struct Object *hitObjects[32];
    u32 lastAction;
    u16 lastActionTimer;
} sPunchContacts;
static s32 sAttackBlasts, sAttackContact, sBlastHurt;
static s16 sInvulnerable, sDelayInvincTimer;
static s32 capQueuedImpactCalls;
static u32 mario_bowser_impact_knockback(struct MarioState *m, s32 inAir) {
    capQueuedImpactCalls++;
    m->action = ACT_HARD_BACKWARD_GROUND_KB;
    return TRUE;
}
static s32 sDisplayingDoorText, sJustTeleported, gMarioCurrentRoom;
static struct Object *sKnockbackSourceObj;
static struct Object gObjectLists[NUM_OBJ_LISTS];
static const BehaviorScript bhvBalconyBigBoo[] = { 201 };
static const BehaviorScript bhvMerryGoRoundBigBoo[] = { 202 };
static const BehaviorScript bhvGhostHuntBigBoo[] = { 203 };
static const BehaviorScript bhvWigglerHead[] = { 204 };
static const BehaviorScript bhvMrIParticle[] = { 6 }, bhvSnufitBalls[] = { 5 }, bhvChuckya[] = { 1 },
                            bhvGoomba[] = { 2 }, bhvFloomba[] = { 3 }, bhvPiranhaPlant[] = { 4 };
static const BehaviorScript bhvKoopa[] = { 7 };
static const BehaviorScript bhvKoopaShell[] = { 122 };
static const BehaviorScript bhvKoopaShellUnderwater[] = { 123 };
static const BehaviorScript bhvSnufit[] = { 8 };
static const BehaviorScript bhvSnowmansBottom[] = { 100 + 0 };
static const BehaviorScript bhvMrBlizzardSnowball[] = { 100 + 1 };
static const BehaviorScript bhvMontyMoleRock[] = { 100 + 2 };
static const BehaviorScript bhvWaterBomb[] = { 100 + 3 };
static const BehaviorScript bhvBowlingBall[] = { 100 + 4 };
static const BehaviorScript bhvExplosion[] = { 100 + 5 };
static const BehaviorScript bhvFallingPillarHitbox[] = { 100 + 6 };
static const BehaviorScript bhvBowserBomb[] = { 100 + 7 };
static const BehaviorScript bhvBowser[] = { 120 };
static const BehaviorScript bhvBowserTailAnchor[] = { 121 };
static const BehaviorScript bhvPitBowlingBall[] = { 100 + 8 };
static const BehaviorScript bhvFreeBowlingBall[] = { 100 + 9 };
static const BehaviorScript bhvJrbSlidingBox[] = { 100 + 10 };
static const BehaviorScript bhvFlyingBookend[] = { 100 + 11 };
static const BehaviorScript bhvBulletBill[] = { 100 + 12 };
static s32 sPalette, sShake, sShakeCount;
static void mario_he_thicc_attack_impact(struct MarioState *m) {
    if (sPalette == MARIO_PALETTE_HE_THICC) {
        sAttackBlasts++;
        sBlastHurt = m->hurtCounter;
    }
}
static u8 mario_character_palette(void) {
    return sPalette;
}
static void set_camera_shake_from_hit(s16 shake) {
    sShake = shake;
    sShakeCount++;
}
static s32 obj_has_behavior(struct Object *o, const BehaviorScript *b) {
    return o->behavior == b;
}
static s32 steps, launches, visuals, sounds, rumbles, handlerMode, handlerCalls, finishCalls, grabs;
static s32 sGroundStepResult, sWallBounces, sNativeInvulnerable;
static struct Animation animation = { 20 };
s32 mario_is_knocked_back(u32 action);
s32 mario_ground_twirl_rebound_from_stun(struct MarioState *m, struct Object *obj);
void mario_twirl_push_enemies(struct MarioState *m);
s32 mario_twirl_can_stun_enemy(struct Object *obj);
s32 mario_start_ground_twirl(struct MarioState *m, s32 continuation);
static s32 mario_start_air_twirl(struct MarioState *m, s32 continuation) {
    m->prevAction = m->action;
    m->action = ACT_L_AIR_TWIRL;
    return TRUE;
}
static s32 mario_ground_twirl_rebound_touches_enemy(struct MarioState *m);
static const void *segmented_to_virtual(const void *p) {
    return p;
}
static s32 mario_ground_twirl_is_enemy(struct Object *o) {
    return o->enemy;
}
static s32 mario_ground_twirl_is_immune_enemy(struct Object *o) {
    return o->immune;
}
static s32 mario_ground_twirl_is_large_enemy(struct Object *o) {
    return o->large;
}
static s32 mario_ground_twirl_is_oversized_enemy(struct Object *o) {
    return o->large;
}

static s32 mario_ground_twirl_is_boo(struct Object *o) {
    return o->boo;
}
s32 mario_ground_twirl_is_active(struct MarioState *m);

s32 mario_ground_twirl_is_held_pose(struct MarioState *m);
static s32 mario_dab_can_start_from_neutral(struct MarioState *m) {
    return FALSE;
}
static s32 mario_dab_loses_cap_on_damage(struct MarioState *m) {
    return FALSE;
}
static s32 mario_pose_exposes_cap_to_damage(struct MarioState *m) {
    return FALSE;
}

static s32 object_facing_mario(struct MarioState *m, struct Object *o, s16 a) {
    return TRUE;
}
static s32 check_object_grab_mario(struct MarioState *m, u32 t, struct Object *o) {
    if (grabs) {
        m->action = ACT_GRABBED;
        return TRUE;
    }
    return FALSE;
}
static void mario_metal_stun_enemy_on_contact(struct MarioState *m, struct Object *o) {
    o->oGroundTwirlStunTimer = -30;
}
static s32 stunned_enemy_can_interact(struct MarioState *m, struct Object *o, u32 t) {
    return FALSE;
}
static void mario_rebound_stunned_enemy(struct MarioState *m, struct Object *o) {
}
static void mario_ground_twirl_reset_cooldown(void) {
}
static f32 mario_ground_twirl_push_speed(f32 s) {
    return 20 + s * .75f;
}
static s32 mario_wall_between_object(f32 *p, struct Object *o) {
    return o->blocked;
}
static void mario_launch_stunned_enemy(struct MarioState *m, struct Object *o, f32 s, u32 id) {
    launches++;
    o->oGroundTwirlLastHitId = id;
    o->oGroundTwirlStunTimer = -30;
    o->launchSpeed = s;
}
static s16 mario_obj_angle_to_object(struct MarioState *m, struct Object *o) {
    return (s16) (atan2f(o->oPosX - m->pos[0], o->oPosZ - m->pos[2]) * (65536.0f / 6.28318530718f));
}
static void mario_set_forward_vel(struct MarioState *m, f32 speed) {
    m->forwardVel = speed;
    m->vel[0] = speed * sinf(m->faceAngle[1] * (6.28318530718f / 65536));
    m->vel[2] = speed * cosf(m->faceAngle[1] * (6.28318530718f / 65536));
}
static u32 set_mario_action(struct MarioState *m, u32 a, u32 arg) {
    m->prevAction = m->action;
    m->action = a;
    m->actionArg = arg;
    m->actionState = m->actionTimer = 0;
    return TRUE;
}
static void play_sound(s32 sound, f32 *p) {
    sounds++;
}
static void queue_rumble_data(s32 t, s32 strength) {
    rumbles++;
}
static u32 determine_interaction(struct MarioState *m, struct Object *o) {
    return sAttackContact;
}
void mario_set_punch_hit_phase(s32 phase);
static void remember_punch_hit_object(struct Object *o) {
}
static void mario_blow_off_cap(struct MarioState *m, f32 speed) {
}
static void mario_extinguish_fire(struct MarioState *m) {
}
static void check_kick_or_punch_wall(struct MarioState *m) {
}
static struct Object *mario_get_collided_object_ignoring_knockback_source(struct MarioState *m, u32 t) {
    for (int i = 0; i < m->marioObj->numCollidedObjs; i++) {
        if (m->marioObj->collidedObjs[i]->oInteractType == t) {
            return m->marioObj->collidedObjs[i];
        }
    }
    return NULL;
}
static s32 mario_can_grab_bowser_tail(struct MarioState *m, struct Object *bowser);
static u32 native_handler(struct MarioState *m, u32 t, struct Object *o) {
    handlerCalls++;
    if (handlerMode == 7) {
        if (t != INTERACT_GRABBABLE || !mario_can_grab_bowser_tail(m, o)) {
            return FALSE;
        }
        m->interactObj = o;
        m->input |= INPUT_INTERACT_OBJ_GRABBABLE;
        return TRUE;
    }
    sNativeInvulnerable = sInvulnerable;
    if (handlerMode == 1) {
        set_mario_action(m, ACT_BACKWARD_GROUND_KB, 0);
        m->invincTimer = 2;
    }
    if (handlerMode == 2) {
        set_mario_action(m, ACT_BACKWARD_GROUND_KB, 0);
        m->hurtCounter += 8;
    }
    if (handlerMode == 3) {
        set_mario_action(m, ACT_SHOCKED, 0);
    }
    if (handlerMode == 4) {
        set_mario_action(m, ACT_BACKWARD_GROUND_KB, 0);
    }
    if (handlerMode == 5 || handlerMode == 6) {
        o->oInteractStatus = INT_STATUS_WAS_ATTACKED;
    }
    if (handlerMode == 6) {
        set_mario_action(m, ACT_BACKWARD_GROUND_KB, 0);
        m->hurtCounter += 8;
    }
    if (handlerMode == 5) {
        return FALSE;
    }
    return handlerMode != 0;
}
static struct {
    u32 interactType;
    u32 (*handler)(struct MarioState *, u32, struct Object *);
} sInteractionHandlers[] = {
    { INTERACT_FLAME, native_handler },    { INTERACT_SNUFIT_BULLET, native_handler },
    { INTERACT_BULLY, native_handler },    { INTERACT_MR_BLIZZARD, native_handler },
    { INTERACT_DAMAGE, native_handler },   { INTERACT_SHOCK, native_handler },
    { INTERACT_GRABBABLE, native_handler }
};

static void real_mario_ground_twirl_finish(void);
static void mario_ground_twirl_finish(void) {
    finishCalls++;
    real_mario_ground_twirl_finish();
}
static s32 set_jump_from_landing(struct MarioState *m) {
    return set_mario_action(m, ACT_JUMP, 0);
}
static s32 check_ground_dive_or_punch(struct MarioState *m) {
    return set_mario_action(m, ACT_MOVE_PUNCHING, 0);
}
static void mario_ground_twirl_update_speed(struct MarioState *m, s32 slow) {
}
static s32 perform_ground_step(struct MarioState *m) {
    steps++;
    return sGroundStepResult;
}
static s32 mario_ground_twirl_handle_wall(struct MarioState *m, s32 slow) {
    sWallBounces++;
    return TRUE;
}
static void mario_ground_twirl_update_visual(struct MarioState *m) {
    visuals++;
}
static s32 sAnimationID;
static void set_mario_animation(struct MarioState *m, s32 id) {
    sAnimationID = id;
    m->marioObj->header.gfx.animInfo.curAnim = &animation;
}
static void set_anim_to_frame(struct MarioState *m, s32 frame) {
}
static void mario_ground_twirl_prepare_walk_animation(struct MarioState *m) {
    visuals++;
}

#include "twirl_sources.inc"
#define NEAR(a, b) EXPECT_EQ(fabsf((a) - (b)) < 0.001f, TRUE)
#define RUN_CASE(name, fn)                                                                             \
    do {                                                                                               \
        if (test_selected(name))                                                                       \
            test_report(name, fn());                                                                   \
    } while (0)
static struct MarioState m;
static struct Object mario, enemies[3];
static struct Controller controller;
static void begin(u32 flags) {
    memset(&m, 0, sizeof(m));
    memset(&mario, 0, sizeof(mario));
    memset(enemies, 0, sizeof(enemies));
    memset(&sTwirlContact, 0, sizeof(sTwirlContact));
    memset(&sGroundTwirl, 0, sizeof(sGroundTwirl));
    sKnockbackSourceObj = NULL;
    sPunchContacts.phase = 0;
    controller.buttonDown = L_TRIG;
    controller.buttonPressed = 0;
    m.action = ACT_GROUND_TWIRL;
    m.flags = flags;
    m.health = 0x880;
    m.marioObj = &mario;
    m.controller = &controller;
    sGroundTwirl.id = 1;
    steps = launches = visuals = sounds = rumbles = handlerMode = handlerCalls = finishCalls = grabs =
        0;
    sGroundStepResult = GROUND_STEP_NONE;
    sWallBounces = 0;
    sNativeInvulnerable = -1;
    sPalette = MARIO_PALETTE_STANDARD;
    sShake = SHAKE_NONE;
    sShakeCount = 0;
    gMarioCurrentRoom = 0;
    sPunchContacts.attackImpactDone = FALSE;
    sAttackBlasts = sAttackContact = sBlastHurt = 0;
    for (int list = 0; list < NUM_OBJ_LISTS; list++) {
        gObjectLists[list].header.next = &gObjectLists[list];
    }
    for (int i = 0; i < 3; i++) {
        enemies[i].enemy = TRUE;
        enemies[i].behavior = bhvGoomba;
        enemies[i].activeFlags = 1;
        enemies[i].hitboxRadius = 100;
        enemies[i].hitboxHeight = 160;
        enemies[i].oPosZ = 30 + i * 10;
        enemies[i].oInteractType = INTERACT_BOUNCE_TOP;
    }
    gObjectLists[0].header.next = &enemies[0];
    enemies[0].header.next = &enemies[1];
    enemies[1].header.next = &gObjectLists[0];
    mario.numCollidedObjs = 2;
    mario.collidedObjs[0] = &enemies[0];
    mario.collidedObjs[1] = &enemies[1];
}
static int stun_recoil(void) {
    const float speeds[] = { 0, 16, 40, 100 };
    const u32 flags[] = { 0, MARIO_CAP_ON_HEAD, MARIO_CAP_ON_HEAD | MARIO_METAL_CAP, MARIO_VANISH_CAP };
    for (int cycle = 0; cycle < 3; cycle++) {
        for (unsigned f = 0; f < 4; f++) {
            for (unsigned s = 0; s < 4; s++) {
                for (int behind = 0; behind < 2; behind++) {
                    begin(flags[f]);
                    if (behind) {
                        m.faceAngle[1] = (s16) 0x8000;
                    }
                    mario_set_forward_vel(&m, speeds[s]);
                    EXPECT_EQ(mario_ground_twirl_action(&m), FALSE);
                    EXPECT_EQ(steps, 1);
                    EXPECT_EQ(launches, f == 3 ? 0 : 2);
                    if (f == 0) {
                        EXPECT_EQ(m.action,
                                  behind ? ACT_SOFT_FORWARD_GROUND_KB : ACT_SOFT_BACKWARD_GROUND_KB);
                        NEAR(fabsf(m.forwardVel), 16);
                        EXPECT_EQ(m.hurtCounter, 4);
                        EXPECT_EQ(visuals, 0);
                        EXPECT_EQ(m.actionState, GROUND_TWIRL_IMMUNE_REBOUND_STATE);
                        EXPECT_EQ(m.actionTimer, 0);
                        EXPECT_EQ(sTwirlContact.reboundActive, TRUE);
                        EXPECT_EQ(sKnockbackSourceObj == &enemies[0], TRUE);
                    } else {
                        EXPECT_EQ(m.action, ACT_GROUND_TWIRL);
                        EXPECT_EQ(m.hurtCounter, 0);
                        EXPECT_EQ(visuals, 1);
                    }
                    if (f != 3) {
                        for (int i = 0; i < 2; i++) {
                            EXPECT_EQ(enemies[i].oGroundTwirlStunTimer, -30);
                            NEAR(enemies[i].launchSpeed, speeds[s]);
                        }
                    }
                }
            }
        }
    }
    begin(0);
    enemies[0].boo = TRUE;
    enemies[1].activeFlags = 0;
    EXPECT_EQ(mario_ground_twirl_action(&m), FALSE);
    EXPECT_EQ(launches, 0);
    EXPECT_EQ(m.hurtCounter, 4);
    EXPECT_EQ(mario_is_knocked_back(m.action), TRUE);
    EXPECT_EQ(enemies[0].oGroundTwirlLastHitId, 1);
    EXPECT_EQ(enemies[0].oInteractStatus,
              INT_STATUS_INTERACTED | INT_STATUS_WAS_ATTACKED | ATTACK_FROM_ABOVE);
    return TRUE;
}
static int grace_and_repeats(void) {
    for (int protected = 0; protected < 2; protected ++) {
        begin(0);
        if (protected) {
            m.invincTimer = 10;
        } else {
            sTwirlContact.reboundHiddenTimer = 4;
        }
        mario_ground_twirl_action(&m);
        EXPECT_EQ(launches, 2);
        EXPECT_EQ(m.action, ACT_GROUND_TWIRL);
        EXPECT_EQ(m.hurtCounter, 0);
    }
    begin(0);
    enemies[0].oGroundTwirlLastHitId = enemies[1].oGroundTwirlLastHitId = 1;
    mario_ground_twirl_action(&m);
    EXPECT_EQ(launches, 0);
    EXPECT_EQ(m.action, ACT_GROUND_TWIRL);
    EXPECT_EQ(m.hurtCounter, 0);
    begin(0);
    enemies[0].blocked = enemies[1].blocked = TRUE;
    mario_ground_twirl_action(&m);
    EXPECT_EQ(launches, 0);
    EXPECT_EQ(m.hurtCounter, 0);
    begin(0);
    mario_ground_twirl_action(&m);
    EXPECT_EQ(m.hurtCounter, 4);
    // A later contact in this same twirl cannot charge another wedge.
    m.action = ACT_GROUND_TWIRL;
    EXPECT_EQ(mario_ground_twirl_rebound_from_stun(&m, &enemies[1]), TRUE);
    EXPECT_EQ(m.hurtCounter, 4);
    m.action = ACT_IDLE;
    update_ground_twirl_rebound_invulnerability(&m);
    EXPECT_EQ(sTwirlContact.reboundHiddenTimer, 8);
    EXPECT_EQ(m.invincTimer, 30);
    for (int i = 0; i < 30; i++) {
        mario_process_interactions(&m);
    }
    EXPECT_EQ(m.invincTimer, 0);
    EXPECT_EQ(sTwirlContact.reboundHiddenTimer, 0);
    for (int cycle = 0; cycle < 3; cycle++) {
        set_mario_action(&m, ACT_GROUND_TWIRL, 0);
        sGroundTwirl.id++;
        mario_ground_twirl_reset_contact_damage();
        mario_ground_twirl_action(&m);
        EXPECT_EQ(m.hurtCounter, 8 + cycle * 4);
        EXPECT_EQ(mario_is_knocked_back(m.action), TRUE);
        m.action = ACT_IDLE;
        update_ground_twirl_rebound_invulnerability(&m);
        for (int i = 0; i < 30; i++) {
            mario_process_interactions(&m);
        }
    }
    return TRUE;
}
static int mobile_enemy_body_reach(void) {
    for (int cycle = 0; cycle < 3; cycle++) {
        begin(MARIO_CAP_ON_HEAD);
        enemies[1].activeFlags = ACTIVE_FLAG_DEACTIVATED;
        mario.numCollidedObjs = 0;
        mario.hitboxRadius = 37;
        enemies[0].hitboxRadius = 160;
        enemies[0].hurtboxRadius = 80;
        enemies[0].oPosZ = 120;
        mario_twirl_push_enemies(&m);
        EXPECT_EQ(launches, 0);
        enemies[0].oPosZ = 116;
        mario_twirl_push_enemies(&m);
        EXPECT_EQ(launches, 1);
    }
    return TRUE;
}
static int snufit_vertical_contact(void) {
    const f32 heights[] = { -61, -60, 120, 121, 140, 159, 160, 161 };
    const u32 flags[] = { MARIO_CAP_ON_HEAD, 0, MARIO_CAP_ON_HEAD | MARIO_VANISH_CAP };
    for (int cycle = 0; cycle < 3; cycle++) {
        for (unsigned f = 0; f < 3; f++) {
            for (unsigned h = 0; h < 8; h++) {
                begin(flags[f]);
                enemies[1].activeFlags = ACTIVE_FLAG_DEACTIVATED;
                m.pos[1] = 400;
                mario.hitboxHeight = 160;
                mario.hitboxRadius = 37;
                enemies[0].behavior = bhvSnufit;
                enemies[0].oInteractType = INTERACT_HIT_FROM_BELOW;
                enemies[0].hitboxRadius = 100;
                enemies[0].hurtboxRadius = 70;
                enemies[0].hitboxHeight = 60;
                enemies[0].oPosY = m.pos[1] + heights[h];
                mario_set_forward_vel(&m, 32);
                int contact = heights[h] >= -60 && heights[h] <= 160 && f != 2;
                mario_ground_twirl_action(&m);
                EXPECT_EQ(launches, contact);
                EXPECT_EQ(enemies[0].oGroundTwirlStunTimer, contact ? -30 : 0);
                EXPECT_EQ(m.hurtCounter, contact && f == 1 ? 4 : 0);
                if (contact) {
                    EXPECT_EQ(enemies[0].oGroundTwirlLastHitId, sGroundTwirl.id);
                    mario_twirl_push_enemies(&m);
                    EXPECT_EQ(launches, 1); // One hit per spin.
                }
            }
        }
    }
    // Raising the vertical reach does not extend horizontal reach or pass through walls.
    for (int blocked = 0; blocked < 2; blocked++) {
        begin(MARIO_CAP_ON_HEAD);
        enemies[1].activeFlags = ACTIVE_FLAG_DEACTIVATED;
        mario.hitboxRadius = 37;
        enemies[0].behavior = bhvSnufit;
        enemies[0].hitboxRadius = 100;
        enemies[0].hurtboxRadius = 70;
        enemies[0].hitboxHeight = 60;
        enemies[0].oPosY = 140;
        enemies[0].oPosZ = blocked ? 30 : 101;
        enemies[0].blocked = blocked;
        mario_ground_twirl_action(&m);
        EXPECT_EQ(launches, 0);
    }
    return TRUE;
}
static int nondamaging_contacts(void) {
    for (int displaced = 0; displaced < 3; displaced++) {
        begin(0);
        enemies[0].oGroundTwirlStunTimer = displaced == 1 ? -30 : 0;
        enemies[0].oGroundTwirlContactRebound = displaced == 2 ? 1 : 0;
        EXPECT_EQ(mario_handle_defensive_contact(&m, &enemies[0], INTERACT_BOUNCE_TOP, TRUE),
                  DEFENSIVE_CONTACT_SKIP);
        EXPECT_EQ(m.action, ACT_GROUND_TWIRL);
        EXPECT_EQ(m.hurtCounter, 0);
    }
    begin(0);
    grabs = TRUE;
    enemies[0].behavior = bhvChuckya;
    enemies[0].oInteractionSubtype = INT_SUBTYPE_GRABS_MARIO;
    EXPECT_EQ(mario_handle_defensive_contact(&m, &enemies[0], INTERACT_GRABBABLE, TRUE),
              DEFENSIVE_CONTACT_STOP);
    EXPECT_EQ(m.action, ACT_GRABBED);
    EXPECT_EQ(m.hurtCounter, 0);
    // The damage service itself refuses to charge a non-knockback action.
    mario_ground_twirl_damage_capless_contact(&m);
    EXPECT_EQ(m.hurtCounter, 0);
    begin(0);
    enemies[0].large = TRUE;
    mario_set_forward_vel(&m, 60);
    EXPECT_EQ(mario_handle_defensive_contact(&m, &enemies[0], INTERACT_BOUNCE_TOP, TRUE),
              DEFENSIVE_CONTACT_STOP);
    EXPECT_EQ(mario_is_knocked_back(m.action), TRUE);
    NEAR(fabsf(m.forwardVel), 30);
    EXPECT_EQ(m.hurtCounter, 4);
    return TRUE;
}
static void snufit_contacts(void) {
    for (int i = 0; i < 3; i++) {
        enemies[i].enemy = FALSE;
        enemies[i].behavior = bhvSnufitBalls;
        enemies[i].oAction = 0;
        enemies[i].oIntangibleTimer = 0;
        enemies[i].oInteractType = INTERACT_SNUFIT_BULLET;
    }
    m.collidedObjInteractTypes = INTERACT_SNUFIT_BULLET;
}
static int snufit_parry_recovery(void) {
    for (int cycle = 0; cycle < 3; cycle++) {
        begin(MARIO_CAP_ON_HEAD);
        snufit_contacts();
        mario_process_interactions(&m);
        EXPECT_EQ(m.action, ACT_SOFT_BACKWARD_GROUND_KB);
        EXPECT_EQ(enemies[0].oAction, 1);
        EXPECT_EQ(enemies[1].oAction, 1);
        EXPECT_EQ(m.hurtCounter, 0);
        m.action = ACT_BACKWARD_AIR_KB;
        snufit_contacts();
        mario_process_interactions(&m);
        EXPECT_EQ(enemies[1].oAction, 1);
        EXPECT_EQ(sTwirlContact.reboundHiddenTimer, 0);
        set_mario_action(&m, ACT_IDLE, 0);
        for (int frame = 0; frame < 8; frame++) {
            if (frame == 2) {
                set_mario_action(&m, ACT_WALKING, 0);
            }
            if (frame == 3) {
                set_mario_action(&m, ACT_JUMP, 0);
            }
            snufit_contacts();
            mario_process_interactions(&m);
            EXPECT_EQ(enemies[1].oAction, 1);
            EXPECT_EQ(sTwirlContact.reboundHiddenTimer, 7 - frame);
            EXPECT_EQ(m.hurtCounter, 0);
        }
        snufit_contacts();
        mario_process_interactions(&m);
        EXPECT_EQ(enemies[1].oAction, 0);
        set_mario_action(&m, ACT_GROUND_TWIRL, 0);
        snufit_contacts();
        mario_process_interactions(&m);
        EXPECT_EQ(enemies[1].oAction, 1);
        mario_ground_twirl_reset_rebound_invulnerability();
        EXPECT_EQ(sTwirlContact.reboundHiddenTimer, 0);
        EXPECT_EQ(sTwirlContact.reboundActive, FALSE);
    }
    begin(0);
    snufit_contacts();
    mario_process_interactions(&m);
    EXPECT_EQ(m.hurtCounter, 4);
    snufit_contacts();
    mario_process_interactions(&m);
    EXPECT_EQ(m.hurtCounter, 4);
    return TRUE;
}

static const BehaviorScript *sProjectileBehaviors[] = { bhvSnufitBalls,        bhvMrIParticle,
                                                        bhvMrBlizzardSnowball, bhvMontyMoleRock,
                                                        bhvFlyingBookend,      bhvWaterBomb,
                                                        bhvBulletBill };
static const s32 sProjectileActions[] = { 0,
                                          MR_I_PIRANHA_PARTICLE_ACT_MOVE,
                                          MR_BLIZZARD_SNOWBALL_ACT_COLLISION,
                                          MONTY_MOLE_ROCK_ACT_MOVE,
                                          FLYING_BOOKEND_ACT_FLY_FORWARD,
                                          WATER_BOMB_ACT_DROP,
                                          BULLET_BILL_ACT_CHASING_MARIO };
static const u32 sProjectileInteractions[] = { INTERACT_SNUFIT_BULLET, INTERACT_DAMAGE,
                                               INTERACT_MR_BLIZZARD,   INTERACT_DAMAGE,
                                               INTERACT_DAMAGE,        INTERACT_MR_BLIZZARD,
                                               INTERACT_DAMAGE };

static void projectile_contacts(int type) {
    m.collidedObjInteractTypes = 0;
    mario.numCollidedObjs = 3;
    for (int i = 0; i < 3; i++) {
        int kind = (type + i) % 7;
        enemies[i].enemy = FALSE;
        enemies[i].behavior = sProjectileBehaviors[kind];
        enemies[i].oAction = sProjectileActions[kind];
        enemies[i].oIntangibleTimer = 0;
        enemies[i].oInteractStatus = 0;
        enemies[i].oInteractType = sProjectileInteractions[kind];
        enemies[i].oHealth = 1;
        m.collidedObjInteractTypes |= enemies[i].oInteractType;
        mario.collidedObjs[i] = &enemies[i];
    }
}

static int projectile_immunity_contacts(void) {
    for (int cycle = 0; cycle < 3; cycle++) {
        for (int immunity = 0; immunity < 7; immunity++) {
            for (int type = 0; type < 7; type++) {
                begin(MARIO_CAP_ON_HEAD);
                m.action = ACT_WALKING;
                if (immunity == 0) {
                    m.invincTimer = 2;
                } else if (immunity == 1) {
                    m.action = ACT_SOFT_BACKWARD_GROUND_KB;
                } else if (immunity == 2) {
                    m.flags |= MARIO_METAL_CAP;
                } else if (immunity == 3) {
                    m.flags |= MARIO_VANISH_CAP;
                } else if (immunity == 4) {
                    sTwirlContact.reboundHiddenTimer = 1;
                } else if (immunity == 5) {
                    handlerMode = 1; // Damage handler starts immunity during this update.
                } else {
                    m.action = ACT_GROUND_TWIRL;
                }
                projectile_contacts(type);
                mario_process_interactions(&m);
                for (int i = 0; i < 3; i++) {
                    int kind = (type + i) % 7;
                    EXPECT_EQ(enemies[i].oIntangibleTimer, -1);
                    if (kind == 0 || kind == 1) {
                        EXPECT_EQ(enemies[i].oAction, 1);
                    } else if (kind == 2) {
                        EXPECT_EQ(enemies[i].oInteractStatus,
                                  INT_STATUS_INTERACTED | INT_STATUS_ATTACKED_MARIO);
                    } else if (kind == 3) {
                        EXPECT_EQ(enemies[i].oInteractStatus,
                                  INT_STATUS_INTERACTED | INT_STATUS_WAS_ATTACKED);
                    } else if (kind == 4) {
                        EXPECT_EQ(enemies[i].oAction, -1);
                        EXPECT_EQ(enemies[i].oHealth, 0);
                    } else {
                        EXPECT_EQ(enemies[i].oInteractStatus, INT_STATUS_INTERACTED);
                    }
                }
                EXPECT_EQ(m.hurtCounter, 0);
                if (immunity == 4) {
                    EXPECT_EQ(sTwirlContact.reboundHiddenTimer, 0);
                }
            }
        }
    }
    return TRUE;
}

static int projectile_immunity_boundaries(void) {
    for (int type = 0; type < 7; type++) {
        begin(MARIO_CAP_ON_HEAD);
        m.action = ACT_WALKING;
        m.invincTimer = 2;
        for (int frame = 0; frame < 3; frame++) {
            projectile_contacts(type);
            mario_process_interactions(&m);
            EXPECT_EQ(enemies[0].oIntangibleTimer, frame < 2 ? -1 : 0);
            EXPECT_EQ(m.invincTimer, frame == 0 ? 1 : 0);
        }
        // Immunity never clears separated rooms, dead objects or intangible shots.
        for (int excluded = 0; excluded < 3; excluded++) {
            begin(MARIO_CAP_ON_HEAD | MARIO_METAL_CAP);
            m.action = ACT_WALKING;
            projectile_contacts(type);
            if (excluded == 0) {
                enemies[0].activeFlags |= ACTIVE_FLAG_IN_DIFFERENT_ROOM;
            } else if (excluded == 1) {
                enemies[0].activeFlags = ACTIVE_FLAG_DEACTIVATED;
            } else {
                enemies[0].oIntangibleTimer = 1;
            }
            mario_process_interactions(&m);
            EXPECT_EQ(enemies[0].oAction, sProjectileActions[type]);
            EXPECT_EQ(enemies[0].oIntangibleTimer, excluded == 2 ? 1 : 0);
        }
        // Existing impact flags and motion are not replayed on subsequent scans.
        begin(MARIO_CAP_ON_HEAD);
        m.action = ACT_WALKING;
        m.invincTimer = 5;
        projectile_contacts(type);
        mario_process_interactions(&m);
        s32 action = enemies[0].oAction;
        u32 status = enemies[0].oInteractStatus;
        mario_process_interactions(&m);
        EXPECT_EQ(enemies[0].oAction, action);
        EXPECT_EQ(enemies[0].oInteractStatus, status);
        EXPECT_EQ(m.invincTimer, 3);
    }
    const s32 inactiveActions[] = { 1,
                                    MR_I_PIRANHA_PARTICLE_ACT_INTERACTED,
                                    MR_BLIZZARD_SNOWBALL_ACT_LAUNCH,
                                    MONTY_MOLE_ROCK_ACT_HELD,
                                    FLYING_BOOKEND_ACT_GROW,
                                    WATER_BOMB_ACT_INIT,
                                    BULLET_BILL_ACT_IDLE };
    for (int type = 0; type < 7; type++) {
        begin(MARIO_CAP_ON_HEAD | MARIO_METAL_CAP);
        m.action = ACT_WALKING;
        projectile_contacts(type);
        enemies[0].oAction = inactiveActions[type];
        mario_process_interactions(&m);
        EXPECT_EQ(enemies[0].oAction, inactiveActions[type]);
        EXPECT_EQ(enemies[0].oIntangibleTimer, 0);
    }
    begin(MARIO_CAP_ON_HEAD);
    m.action = ACT_WALKING;
    m.invincTimer = 3;
    enemies[0].enemy = FALSE;
    enemies[0].behavior = bhvBowlingBall;
    mario_process_interactions(&m);
    EXPECT_EQ(enemies[0].oIntangibleTimer, 0);
    EXPECT_EQ(enemies[0].oAction, 0);
    begin(MARIO_CAP_ON_HEAD | MARIO_METAL_CAP);
    m.action = ACT_DISAPPEARED;
    projectile_contacts(0);
    mario_process_interactions(&m);
    EXPECT_EQ(enemies[0].oAction, 0);
    return TRUE;
}

static int breakdance_damage_dispatch(void) {
    struct Animation spin = { .loopEnd = 24 };
    const u32 actions[] = { ACT_PUNCHING, ACT_MOVE_PUNCHING };
    for (unsigned action = 0; action < sizeof(actions) / sizeof(*actions); action++) {
        for (int frame = 0; frame < 24; frame++) {
            begin(MARIO_CAP_ON_HEAD);
            m.action = actions[action];
            m.actionArg = ACT_ARG_PUNCH_SEQUENCE_BREAKDANCE;
            m.actionTimer = 1;
            mario.header.gfx.animInfo = (struct AnimInfo){ .curAnim = &spin,
                                                           .animID = MARIO_ANIM_BREAKDANCE,
                                                           .animFrame = frame };
            enemies[0].oInteractType = INTERACT_BULLY;
            mario.collidedObjs[0] = &enemies[0];
            mario.numCollidedObjs = 1;
            mario.collidedObjInteractTypes = m.collidedObjInteractTypes = INTERACT_BULLY;
            mario_process_interactions(&m);
            EXPECT_EQ(handlerCalls, 1);
            EXPECT_EQ(sNativeInvulnerable, frame != 0 && frame != 12);
        }
    }
    return TRUE;
}

static int native_knockback(void) {
    const u32 flags[] = { 0, MARIO_CAP_ON_HEAD, MARIO_VANISH_CAP };
    for (int cycle = 0; cycle < 3; cycle++) {
        for (int mode = 0; mode <= 4; mode++) {
            for (unsigned f = 0; f < 3; f++) {
                for (int protected = 0; protected < 2; protected ++) {
                    begin(flags[f]);
                    enemies[0].enemy = FALSE;
                    enemies[0].immune = FALSE; // Bully is outside ordinary stun targeting.
                    enemies[0].oInteractType = INTERACT_BULLY;
                    mario.numCollidedObjs = 1;
                    m.collidedObjInteractTypes = INTERACT_BULLY;
                    handlerMode = mode;
                    if (protected) {
                        m.invincTimer = 10;
                    }
                    mario_process_interactions(&m);
                    EXPECT_EQ(handlerCalls, 1);
                    int extra = !f && !protected && (mode == 1 || mode == 4);
                    EXPECT_EQ(m.hurtCounter, (mode == 2 ? 8 : 0) + (extra ? 4 : 0));
                    EXPECT_EQ(sInvulnerable,
                              TRUE); // Snapshot restoration survives every handler response.
                }
            }
        }
    }
    return TRUE;
}
static void hazard_contact(const BehaviorScript *behavior, u32 type, int damage) {
    enemies[0].behavior = behavior;
    enemies[0].enemy = enemies[0].immune = enemies[0].large = FALSE;
    enemies[0].oInteractType = type;
    enemies[0].oDamageOrCoinValue = damage;
    mario.numCollidedObjs = 1;
    m.collidedObjInteractTypes = mario.collidedObjInteractTypes = type;
}
static int capless_hazard_recoil(void) {
    const BehaviorScript *behaviors[] = { bhvBowlingBall, bhvPitBowlingBall, bhvFreeBowlingBall,
                                          bhvJrbSlidingBox, bhvWaterBomb };
    const u32 flags[] = { 0, MARIO_CAP_ON_HEAD, MARIO_VANISH_CAP };
    for (int cycle = 0; cycle < 3; cycle++) {
        for (unsigned kind = 0; kind < 5; kind++) {
            for (unsigned f = 0; f < 3; f++) {
                for (int zero = 0; zero < 2; zero++) {
                    begin(flags[f]);
                    int damage = 0;
                    if (!zero) {
                        damage = kind >= 3 ? 1 : 2;
                    }
                    hazard_contact(behaviors[kind], kind == 4 ? INTERACT_MR_BLIZZARD : INTERACT_DAMAGE,
                                   damage);
                    mario_process_interactions(&m);
                    EXPECT_EQ(m.hurtCounter, f == 0 ? 4 : 0);
                    EXPECT_EQ(mario_is_knocked_back(m.action), f != 2);
                    EXPECT_EQ(handlerCalls, f == 2 ? 1 : 0);
                    if (f == 0) {
                        EXPECT_EQ(sTwirlContact.caplessContactDamaged, TRUE);
                        // Another recoil in this spin cannot charge a second wedge.
                        m.action = ACT_GROUND_TWIRL;
                        sTwirlContact.reboundActive = FALSE;
                        mario_handle_defensive_contact(&m, &enemies[0], INTERACT_DAMAGE, TRUE);
                        EXPECT_EQ(m.hurtCounter, 4);
                    }
                }
            }
        }
    }
    // Existing flashing and hidden rebound grace still protect capless Mario.
    for (int hidden = 0; hidden < 2; hidden++) {
        for (int zero = 0; zero < 2; zero++) {
            begin(0);
            hazard_contact(bhvPitBowlingBall, INTERACT_DAMAGE, zero ? 0 : 2);
            if (hidden) {
                sTwirlContact.reboundHiddenTimer = 8;
            } else {
                m.invincTimer = 2;
            }
            mario_process_interactions(&m);
            EXPECT_EQ(m.hurtCounter, 0);
            EXPECT_EQ(m.action, ACT_GROUND_TWIRL);
        }
    }
    return TRUE;
}
static int bowser_twirl_dispatch(void) {
    for (int mode = 0; mode < 4; mode++) {
        for (int powers = 0; powers < 8; powers++) {
            for (int hat = 0; hat < 2; hat++) {
                for (int mine = 0; mine < 2; mine++) {
                    for (int large = 0; large < 2; large++) {
                        for (int grace = 0; grace < 3; grace++) {
                            begin((hat ? MARIO_CAP_ON_HEAD | MARIO_NORMAL_CAP : 0)
                                  | (powers & 1 ? MARIO_WING_CAP : 0)
                                  | (powers & 2 ? MARIO_METAL_CAP : 0)
                                  | (powers & 4 ? MARIO_VANISH_CAP : 0));
                            const u32 actions[] = { ACT_GROUND_TWIRL, ACT_L_AIR_TWIRL,
                                                    ACT_PUTTING_ON_CAP, ACT_PUTTING_ON_CAP_AIR };
                            m.action = actions[mode];
                            hazard_contact(mine ? bhvBowserBomb : bhvChuckya, INTERACT_DAMAGE,
                                           mine ? 7 : 2);
                            enemies[0].oInteractionSubtype = INT_SUBTYPE_BOWSER_ATTACK;
                            // Cover small body anchors and large boss bodies; mines are generic
                            // hazards.
                            enemies[0].immune = !mine;
                            enemies[0].large = !mine && large;
                            if (grace == 1) {
                                m.invincTimer = 2;
                            }
                            if (grace == 2) {
                                sTwirlContact.reboundHiddenTimer = 8;
                            }
                            mario_process_interactions(&m);
                            EXPECT_EQ(handlerCalls, 1);
                            EXPECT_EQ(sNativeInvulnerable, grace != 0);
                            EXPECT_EQ(sTwirlContact.reboundActive, FALSE);
                            EXPECT_EQ(m.hurtCounter, 0);
                        }
                    }
                }
            }
        }
    }
    for (int air = 0; air < 2; air++) {
        begin(MARIO_NORMAL_CAP | MARIO_CAP_IN_HAND);
        m.action = air ? ACT_PUTTING_ON_CAP_AIR : ACT_PUTTING_ON_CAP;
        hazard_contact(bhvExplosion, INTERACT_DAMAGE, 2);
        mario_process_interactions(&m);
        EXPECT_EQ(handlerCalls, 0);
        EXPECT_EQ(m.flags & MARIO_CAP_IN_HAND, MARIO_CAP_IN_HAND);
        enemies[1] = enemies[0];
        enemies[1].oInteractionSubtype = INT_SUBTYPE_BOWSER_ATTACK;
        mario.collidedObjs[1] = &enemies[1];
        mario.numCollidedObjs = 2;
        m.collidedObjInteractTypes = INTERACT_DAMAGE;
        handlerMode = 1;
        mario_process_interactions(&m);
        EXPECT_EQ(handlerCalls, 1);
        EXPECT_EQ(m.flags & MARIO_CAP_IN_HAND, 0);
        EXPECT_EQ(m.flags & MARIO_CAP_ON_HEAD, MARIO_CAP_ON_HEAD);
    }
    for (int flashing = 0; flashing < 2; flashing++) {
        begin(MARIO_NORMAL_CAP | MARIO_CAP_IN_HAND);
        m.action = ACT_PUTTING_ON_CAP;
        m.input = INPUT_STOMPED;
        m.invincTimer = flashing ? 5 : 0;
        mario.oInteractStatus = INT_STATUS_MARIO_KNOCKBACK_DMG;
        capQueuedImpactCalls = 0;
        mario_process_interactions(&m);
        EXPECT_EQ(capQueuedImpactCalls, !flashing);
        EXPECT_EQ(m.input & INPUT_STOMPED, 0);
        EXPECT_EQ(mario.oInteractStatus & INT_STATUS_MARIO_KNOCKBACK_DMG, 0);
        EXPECT_EQ(m.action, flashing ? ACT_PUTTING_ON_CAP : ACT_HARD_BACKWARD_GROUND_KB);
    }
    // Fire, electrical contact, explosions and falling pillars retain native handling.
    const BehaviorScript *behaviors[] = { bhvExplosion, bhvFallingPillarHitbox, bhvJrbSlidingBox,
                                          bhvJrbSlidingBox };
    const u32 types[] = { INTERACT_DAMAGE, INTERACT_DAMAGE, INTERACT_FLAME, INTERACT_SHOCK };
    for (unsigned i = 0; i < 4; i++) {
        begin(0);
        hazard_contact(behaviors[i], types[i], 2);
        mario_process_interactions(&m);
        EXPECT_EQ(handlerCalls, 1);
        EXPECT_EQ(sNativeInvulnerable, FALSE);
        EXPECT_EQ(sTwirlContact.reboundActive, FALSE);
    }
    return TRUE;
}
static int startup_priority(void) {
    for (int backward = 0; backward < 2; backward++) {
        begin(MARIO_CAP_ON_HEAD);
        m.action = ACT_FREEFALL_LAND_STOP;
        m.prevAction = backward ? ACT_BACKWARD_ROLLOUT : ACT_FORWARD_ROLLOUT;
        m.floor = &mario;
        controller.buttonPressed = L_TRIG;
        EXPECT_EQ(mario_try_ground_twirl(&m), TRUE);
        EXPECT_EQ(m.action, ACT_GROUND_TWIRL);
    }
    begin(MARIO_CAP_ON_HEAD);
    m.action = ACT_FREEFALL_LAND_STOP;
    m.prevAction = ACT_FREEFALL;
    m.floor = &mario;
    controller.buttonPressed = L_TRIG;
    EXPECT_EQ(mario_try_ground_twirl(&m), FALSE);

    for (int cycle = 0; cycle < 3; cycle++) {
        for (int sliding = 0; sliding < 2; sliding++) {
            begin(MARIO_CAP_ON_HEAD);
            m.action = ACT_WALKING;
            m.floor = &mario;
            mario_set_forward_vel(&m, 48);
            controller.buttonPressed = L_TRIG;
            m.input = sliding ? INPUT_ABOVE_SLIDE : 0;
            EXPECT_EQ(mario_try_ground_twirl(&m), TRUE);
            EXPECT_EQ(m.action, ACT_GROUND_TWIRL);
            EXPECT_EQ(m.prevAction, ACT_WALKING);
            EXPECT_EQ(sGroundTwirl.startSpeed, 48);
            sGroundStepResult = GROUND_STEP_HIT_WALL;
            EXPECT_EQ(mario_ground_twirl_action(&m), FALSE);
            EXPECT_EQ(m.action, ACT_GROUND_TWIRL);
            EXPECT_EQ(steps, 1);
            EXPECT_EQ(sWallBounces, 1);
            EXPECT_EQ(mario_try_ground_twirl(&m), FALSE); // No second start from the held press.
        }
    }
    begin(MARIO_CAP_ON_HEAD);
    m.action = ACT_WALKING;
    m.floor = &mario;
    controller.buttonPressed = L_TRIG;
    m.input = INPUT_ABOVE_SLIDE | INPUT_OFF_FLOOR;
    EXPECT_EQ(mario_try_ground_twirl(&m), FALSE);
    EXPECT_EQ(m.action, ACT_WALKING);
    for (int age = 1; age <= 6; age++) {
        begin(MARIO_CAP_ON_HEAD);
        m.action = ACT_WALKING;
        m.floor = &mario;
        sGroundTwirl.cooldown = 10;
        controller.buttonPressed = L_TRIG;
        EXPECT_EQ(mario_try_ground_twirl(&m), FALSE);
        controller.buttonPressed = controller.buttonDown = 0;
        for (int frame = 1; frame < age; frame++) {
            EXPECT_EQ(mario_try_ground_twirl(&m), FALSE);
        }
        sGroundTwirl.cooldown = 0;
        EXPECT_EQ(mario_try_ground_twirl(&m), age < 6);
        EXPECT_EQ(sGroundTwirl.inputQueued, 0);
    }
    begin(MARIO_CAP_ON_HEAD);
    m.action = ACT_WALKING;
    m.floor = &mario;
    sGroundTwirl.cooldown = 10;
    controller.buttonPressed = L_TRIG;
    mario_try_ground_twirl(&m);
    controller.buttonPressed = controller.buttonDown = 0;
    for (int frame = 0; frame < 5; frame++) {
        mario_try_ground_twirl(&m);
    }
    controller.buttonPressed = L_TRIG;
    mario_try_ground_twirl(&m);
    controller.buttonPressed = controller.buttonDown = 0;
    sGroundTwirl.cooldown = 0;
    EXPECT_EQ(mario_try_ground_twirl(&m), TRUE);
    return TRUE;
}
static int thicc_pound_range(void) {
    for (int cycle = 0; cycle < 3; cycle++) {
        begin(MARIO_CAP_ON_HEAD);
        sPalette = MARIO_PALETTE_HE_THICC;
        enemies[0].oPosX = 800;
        enemies[0].oPosZ = 0;
        enemies[1].oPosX = 800;
        enemies[1].oPosZ = 1;
        mario_ground_pound_impact(&m);
        EXPECT_EQ(launches, 1);
        EXPECT_EQ(sShakeCount, 1);
        EXPECT_EQ(sShake, SHAKE_HE_THICC_GROUND_POUND);
        EXPECT_EQ(enemies[0].oGroundTwirlStunTimer, -30);
        EXPECT_EQ(enemies[0].oGroundTwirlStunDuration, 60);
        EXPECT_EQ(enemies[0].launchSpeed, 0);
        EXPECT_EQ(enemies[1].oGroundTwirlStunTimer, 0);
        EXPECT_EQ(m.action, ACT_GROUND_TWIRL);
        EXPECT_EQ(m.hurtCounter, 0); // No recoil/damage.
        u32 id = enemies[0].oGroundTwirlLastHitId;
        mario_ground_pound_impact(&m);
        EXPECT_EQ(launches, 1);
        EXPECT_EQ(enemies[0].oGroundTwirlLastHitId, id);
        enemies[0].oGroundTwirlStunTimer = 10;
        mario_ground_pound_impact(&m);
        EXPECT_EQ(enemies[0].oGroundTwirlStunTimer, 60);
        EXPECT_EQ(launches, 1);
    }
    begin(MARIO_CAP_ON_HEAD);
    mario_ground_pound_impact(&m);
    EXPECT_EQ(launches, 0);
    EXPECT_EQ(sShake, SHAKE_GROUND_POUND);
    EXPECT_EQ(sGroundTwirl.id, 1);
    return TRUE;
}
static int thicc_pound_eligibility(void) {
    for (int condition = 0; condition < 17; condition++) {
        begin(MARIO_CAP_ON_HEAD);
        sPalette = MARIO_PALETTE_HE_THICC;
        gMarioCurrentRoom = 1;
        enemies[1].enemy = FALSE;
        switch (condition) {
            case 0:
                enemies[0].blocked = TRUE;
                break;
            case 1:
                enemies[0].activeFlags = ACTIVE_FLAG_DEACTIVATED;
                break;
            case 2:
                enemies[0].activeFlags |= ACTIVE_FLAG_FAR_AWAY;
                break;
            case 3:
                enemies[0].activeFlags |= ACTIVE_FLAG_IN_DIFFERENT_ROOM;
                break;
            case 4:
                enemies[0].oRoom = 2;
                break;
            case 5:
                enemies[0].oIntangibleTimer = -1;
                break;
            case 6:
                enemies[0].oHeldState = HELD_HELD;
                break;
            case 7:
                enemies[0].hitboxRadius = 0;
                break;
            case 8:
                enemies[0].oGroundTwirlContactRebound = 5;
                break;
            case 9:
                enemies[0].metalContactPendingDefeat = TRUE;
                break;
            case 10:
                enemies[0].oInteractStatus = INT_STATUS_WAS_ATTACKED;
                break;
            case 11:
                enemies[0].enemy = FALSE;
                enemies[0].immune = TRUE;
                break;
            case 12:
                enemies[0].behavior = bhvKoopa;
                enemies[0].oKoopaMovementType = KOOPA_BP_KOOPA_THE_QUICK_BASE;
                break;
            case 13:
                enemies[0].oPosY = 201;
                break;
            case 14:
                enemies[0].oPosY = -361;
                break; // Entire hitbox below the height window.
            case 15:
                enemies[0].oPosY = 200;
                break;
            case 16:
                enemies[0].behavior = bhvChuckya;
                enemies[0].large = TRUE;
                break;
        }
        mario_ground_pound_impact(&m);
        EXPECT_EQ(launches, condition >= 15 ? 1 : 0);
        EXPECT_EQ(sShakeCount, 1);
        EXPECT_EQ(enemies[0].metalContactPendingDefeat, condition == 9);
    }
    // The second object list participates; a Boo keeps its harmless native bounce.
    begin(MARIO_VANISH_CAP);
    sPalette = MARIO_PALETTE_HE_THICC;
    gObjectLists[1].header.next = &enemies[2];
    enemies[2].header.next = &gObjectLists[1];
    enemies[0].boo = TRUE;
    enemies[0].oAction = 1;
    enemies[1].enemy = FALSE;
    mario_ground_pound_impact(&m);
    EXPECT_EQ(launches, 1);
    EXPECT_EQ(enemies[0].oInteractStatus,
              INT_STATUS_INTERACTED | INT_STATUS_WAS_ATTACKED | ATTACK_FROM_ABOVE);
    EXPECT_EQ(enemies[0].oGroundTwirlStunTimer, 0);
    EXPECT_EQ(enemies[2].oGroundTwirlStunDuration, 60);
    return TRUE;
}
static int offset_hitbox(void) {
    for (int pound = 0; pound < 2; pound++) {
        for (int cycle = 0; cycle < 3; cycle++) {
            for (int offset = 0; offset <= 20; offset += 20) {
                f32 top = pound ? 200 : 160;
                f32 bottom = pound ? -200 : 0;
                const f32 origins[] = { top + offset - 10, top + offset, top + offset + .1f,
                                        bottom + offset - 100, bottom + offset - 100.1f };
                const int overlap[] = { 1, 1, 0, 1, 0 };
                for (int i = 0; i < 5; i++) {
                    begin(MARIO_CAP_ON_HEAD);
                    sPalette = MARIO_PALETTE_HE_THICC;
                    enemies[1].activeFlags = 0;
                    enemies[0].hitboxRadius = 180;
                    enemies[0].hitboxHeight = 100;
                    enemies[0].hitboxDownOffset = offset;
                    m.pos[1] = cycle * 100;
                    enemies[0].oPosY = origins[i] + m.pos[1];
                    if (pound) {
                        mario_ground_pound_impact(&m);
                    } else {
                        mario_twirl_push_enemies(&m);
                    }
                    EXPECT_EQ(launches, overlap[i]);
                }
            }
        }
    }
    return TRUE;
}

static int thicc_attack_contact(void) {
    const u32 attacks[] = { INT_PUNCH, INT_KICK, INT_TRIP, INT_FAST_ATTACK_OR_SHELL };
    for (int palette = 0; palette < 2; palette++) {
        for (unsigned attack = 0; attack < sizeof(attacks) / sizeof(*attacks); attack++) {
            for (int cycle = 0; cycle < 3; cycle++) {
                begin(MARIO_CAP_ON_HEAD);
                sPalette = palette;
                m.action = ACT_MOVE_PUNCHING;
                m.actionArg = attack == 1 ? ACT_ARG_PUNCH_SEQUENCE_GROUND_KICK : 0;
                sAttackContact = attacks[attack];
                sPunchContacts.phase = 1;
                enemies[0].oInteractType = INTERACT_BULLY;
                enemies[1].oInteractType = INTERACT_MR_BLIZZARD;
                m.collidedObjInteractTypes = INTERACT_BULLY | INTERACT_MR_BLIZZARD;
                mario_process_interactions(&m);
                EXPECT_EQ(sAttackBlasts, 0); // Rejected/nonattack contact has no blast.
                handlerMode = 5;
                m.collidedObjInteractTypes = INTERACT_BULLY | INTERACT_MR_BLIZZARD;
                mario_process_interactions(&m);
                int expected = palette == MARIO_PALETTE_HE_THICC && attack < 2;
                EXPECT_EQ(sAttackBlasts, expected); // Two targets share one blast.
                m.collidedObjInteractTypes = INTERACT_BULLY;
                mario_process_interactions(&m);
                EXPECT_EQ(sAttackBlasts, expected);
            }
        }
    }
    begin(MARIO_CAP_ON_HEAD);
    sPalette = MARIO_PALETTE_HE_THICC;
    m.action = ACT_JUMP_KICK;
    sAttackContact = INT_KICK;
    handlerMode = 5;
    enemies[0].oInteractType = INTERACT_BULLY;
    m.collidedObjInteractTypes = INTERACT_BULLY;
    mario_process_interactions(&m);
    EXPECT_EQ(sAttackBlasts, 1);
    enemies[0].oInteractStatus = 0;
    m.collidedObjInteractTypes = INTERACT_BULLY;
    mario_process_interactions(&m);
    EXPECT_EQ(sAttackBlasts, 1); // The interaction update preserves the air-kick record.
    m.action = ACT_FREEFALL;
    mario_process_interactions(&m);
    EXPECT_EQ(sPunchContacts.attackImpactDone, FALSE);
    m.action = ACT_JUMP_KICK;
    handlerMode = 6;
    enemies[0].oInteractStatus = 0;
    m.collidedObjInteractTypes = INTERACT_BULLY;
    mario_process_interactions(&m);
    EXPECT_EQ(sAttackBlasts, 2);
    EXPECT_EQ(m.action, ACT_BACKWARD_GROUND_KB);
    EXPECT_EQ(sBlastHurt, 8); // Native contact damage precedes the harmless blast.
    return TRUE;
}

static int enemy_hit_rumble(void) {
    const u32 actions[] = { ACT_GROUND_TWIRL, ACT_L_AIR_TWIRL };
    for (s32 action = 0; action < ARRAY_COUNT(actions); action++) {
        for (s32 boo = 0; boo < 2; boo++) {
            begin(MARIO_CAP_ON_HEAD);
            m.action = actions[action];
            enemies[0].boo = boo;
            mario_twirl_push_enemies(&m);
            EXPECT_EQ(rumbles, 1);
            mario_twirl_push_enemies(&m);
            EXPECT_EQ(rumbles, 1);
            EXPECT_EQ(m.action, actions[action]);
        }
        begin(MARIO_CAP_ON_HEAD);
        m.action = actions[action];
        enemies[0].blocked = TRUE;
        enemies[1].blocked = TRUE;
        mario_twirl_push_enemies(&m);
        EXPECT_EQ(rumbles, 0);
        begin(MARIO_CAP_ON_HEAD | MARIO_VANISH_CAP);
        m.action = actions[action];
        mario_twirl_push_enemies(&m);
        EXPECT_EQ(rumbles, 0);
    }
    return TRUE;
}

static int air_twirl_contacts(void) {
    for (int capped = 0; capped < 2; capped++) {
        begin(capped ? MARIO_CAP_ON_HEAD : 0);
        m.action = ACT_L_AIR_TWIRL;
        m.vel[1] = -30;
        m.peakHeight = 5000;
        m.collidedObjInteractTypes = INTERACT_BOUNCE_TOP;
        mario_process_interactions(&m);
        EXPECT_EQ(handlerCalls, 0);
        mario_twirl_push_enemies(&m);
        EXPECT_EQ(launches, 2);
        EXPECT_EQ(m.action, capped ? ACT_L_AIR_TWIRL : ACT_BACKWARD_AIR_KB);
        EXPECT_EQ(m.hurtCounter, capped ? 0 : 4);
        NEAR(m.peakHeight, 5000);
        NEAR(m.vel[1], -30);
        mario_twirl_push_enemies(&m);
        EXPECT_EQ(launches, 2);
    }
    begin(MARIO_CAP_ON_HEAD);
    m.action = ACT_L_AIR_TWIRL;
    enemies[0].enemy = FALSE;
    enemies[0].behavior = bhvMrIParticle;
    enemies[0].immune = TRUE;
    enemies[0].oInteractType = INTERACT_DAMAGE;
    EXPECT_EQ(mario_handle_defensive_contact(&m, &enemies[0], INTERACT_DAMAGE, FALSE),
              DEFENSIVE_CONTACT_STOP);
    EXPECT_EQ(m.action, ACT_BACKWARD_AIR_KB);
    EXPECT_EQ(m.hurtCounter, 0);
    for (int flying = 0; flying < 2; flying++) {
        begin(MARIO_WING_CAP);
        m.action = flying ? ACT_FLYING : ACT_L_AIR_TWIRL;
        m.airTwirlFlightRollFrame = flying ? 1 : 0;
        mario_twirl_push_enemies(&m);
        EXPECT_EQ(enemies[0].oGroundTwirlStunTimer, -30);
        EXPECT_EQ(m.action, flying ? ACT_FLYING : ACT_L_AIR_TWIRL);
        EXPECT_EQ(m.hurtCounter, 0);
        enemies[0].enemy = FALSE;
        enemies[0].oInteractType = INTERACT_FLAME;
        EXPECT_EQ(mario_handle_defensive_contact(&m, &enemies[0], INTERACT_FLAME, FALSE),
                  DEFENSIVE_CONTACT_NORMAL);
        enemies[0].oInteractionSubtype = INT_SUBTYPE_BOWSER_ATTACK;
        EXPECT_EQ(mario_handle_defensive_contact(&m, &enemies[0], INTERACT_DAMAGE, FALSE),
                  DEFENSIVE_CONTACT_NORMAL);
        begin(MARIO_WING_CAP | MARIO_VANISH_CAP);
        m.action = flying ? ACT_FLYING : ACT_L_AIR_TWIRL;
        m.airTwirlFlightRollFrame = flying ? 1 : 0;
        mario_twirl_push_enemies(&m);
        EXPECT_EQ(enemies[0].oGroundTwirlStunTimer, 0);
    }
    begin(MARIO_CAP_ON_HEAD);
    m.action = ACT_L_AIR_TWIRL;
    enemies[0].blocked = TRUE;
    enemies[1].oRoom = 2;
    gMarioCurrentRoom = 1;
    mario_twirl_push_enemies(&m);
    EXPECT_EQ(launches, 0);
    return TRUE;
}

static int held_pose_animation(void) {
    for (int wall = 0; wall < 2; wall++) {
        begin(MARIO_CAP_ON_HEAD);
        gObjectLists[0].header.next = &gObjectLists[0];
        mario_start_ground_twirl(&m, FALSE);
        sGroundTwirl.hasBounced = wall;
        sGroundTwirl.visualWallBounce = wall;
        m.actionState = wall ? 12 : 0;
        for (int frame = 0; frame < 17; frame++) {
            // The visual service is a double; supply its completed-update clock.
            m.actionArg = frame + 1;
            EXPECT_EQ(mario_ground_twirl_action(&m), FALSE);
            EXPECT_EQ(m.action, ACT_GROUND_TWIRL);
            EXPECT_EQ(sAnimationID, MARIO_ANIM_TWIRL);
            EXPECT_EQ(sGroundTwirl.heldPoseFrames, frame < 11 ? 0 : frame - 10);
        }
    }
    for (int release = 7; release <= 12; release++) {
        begin(MARIO_CAP_ON_HEAD);
        gObjectLists[0].header.next = &gObjectLists[0];
        mario_start_ground_twirl(&m, FALSE);
        for (int frame = 0; frame <= release; frame++) {
            m.actionArg = frame + 1;
            if (frame == release) {
                controller.buttonDown = 0;
            }
            mario_ground_twirl_action(&m);
            if (m.action != ACT_GROUND_TWIRL) {
                break;
            }
        }
        EXPECT_EQ(sAnimationID, MARIO_ANIM_TWIRL_LAND);
        EXPECT_EQ(sGroundTwirl.heldPoseFrames, 0);
    }
    return TRUE;
}

static int ground_twirl_timing(void) {
    const u32 requests[] = { INPUT_A_PRESSED, INPUT_B_PRESSED, INPUT_Z_PRESSED };
    const u32 exits[] = { ACT_JUMP, ACT_MOVE_PUNCHING, ACT_CROUCH_SLIDE };
    for (unsigned button = 0; button < sizeof(requests) / sizeof(*requests); button++) {
        for (int age = 0; age <= 6; age++) {
            begin(MARIO_CAP_ON_HEAD);
            gObjectLists[0].header.next = &gObjectLists[0];
            mario_start_ground_twirl(&m, FALSE);
            m.actionState = 100;
            m.input = requests[button];
            EXPECT_EQ(mario_ground_twirl_action(&m), FALSE);
            m.input = 0;
            for (int frame = 0; frame < age; frame++) {
                mario_ground_twirl_update_cooldown();
            }
            m.actionTimer = GROUND_TWIRL_FRAMES - mario_ground_twirl_cancel_frames();
            m.actionState = 0;
            EXPECT_EQ(mario_ground_twirl_action(&m), age < 6);
            EXPECT_EQ(m.action, age < 6 ? exits[button] : ACT_GROUND_TWIRL);
            EXPECT_EQ(sGroundTwirl.bufferedInput, 0);
        }
    }
    // The next-frame L/A chord can leave a newly started moving ground twirl.
    for (int queued = FALSE; queued <= TRUE; queued++) {
        begin(MARIO_CAP_ON_HEAD);
        gObjectLists[0].header.next = &gObjectLists[0];
        mario_start_ground_twirl(&m, FALSE);
        m.actionTimer = 1;
        m.airTwirlJumpQueued = queued;
        m.input = INPUT_A_PRESSED | INPUT_A_DOWN;
        EXPECT_EQ(mario_ground_twirl_action(&m), queued);
        EXPECT_EQ(m.action, queued ? ACT_JUMP : ACT_GROUND_TWIRL);
    }
    begin(MARIO_CAP_ON_HEAD);
    gObjectLists[0].header.next = &gObjectLists[0];
    mario_start_ground_twirl(&m, FALSE);
    controller.buttonDown = 0;
    EXPECT_EQ(sGroundTwirl.cooldown, 42);
    for (int frame = 0; frame < 12; frame++) {
        EXPECT_EQ(mario_ground_twirl_is_active(&m), frame < 5);
        EXPECT_EQ(mario_ground_twirl_action(&m), FALSE);
        EXPECT_EQ(m.action, frame < 11 ? ACT_GROUND_TWIRL : ACT_DECELERATING);
    }
    EXPECT_EQ(steps, 12);
    for (int wall = 0; wall < 2; wall++) {
        begin(MARIO_CAP_ON_HEAD);
        gObjectLists[0].header.next = &gObjectLists[0];
        m.actionTimer = wall ? 12 : 0;
        m.actionState = wall ? 12 : 0;
        sGroundTwirl.hasBounced = wall;
        m.input = INPUT_A_PRESSED | INPUT_A_DOWN;
        for (int frame = 0; frame < (wall ? 5 : 7); frame++) {
            EXPECT_EQ(mario_ground_twirl_action(&m), FALSE);
            EXPECT_EQ(m.action, ACT_GROUND_TWIRL);
            m.input = INPUT_A_DOWN;
        }
        EXPECT_EQ(mario_ground_twirl_action(&m), TRUE);
        EXPECT_EQ(m.action, ACT_JUMP);
    }
    begin(MARIO_CAP_ON_HEAD);
    m.actionTimer = 12;
    for (int elapsed = 0; elapsed < 12; elapsed++) {
        m.actionState = 12 - elapsed;
        EXPECT_EQ(mario_ground_twirl_is_active(&m), elapsed < 4);
    }
    return TRUE;
}

static int air_twirl_ground_handoff(void) {
    for (int held = 0; held < 2; held++) {
        begin(MARIO_CAP_ON_HEAD);
        sGroundStepResult = GROUND_STEP_LEFT_GROUND;
        controller.buttonDown = held ? L_TRIG : 0;
        mario_ground_twirl_action(&m);
        EXPECT_EQ(m.action, held ? ACT_L_AIR_TWIRL : ACT_FREEFALL);
        EXPECT_EQ(steps, 1);
    }
    const u32 outsideGround[] = { ACT_FLYING,       ACT_WATER_IDLE,
                                  ACT_BREASTSTROKE, ACT_METAL_WATER_STANDING,
                                  ACT_HOLDING_POLE, ACT_START_HANGING,
                                  ACT_LEDGE_GRAB,   ACT_IN_CANNON };
    for (unsigned route = 0; route < sizeof(outsideGround) / sizeof(*outsideGround); route++) {
        for (int queuedBefore = 0; queuedBefore < 2; queuedBefore++) {
            begin(MARIO_CAP_ON_HEAD);
            m.floor = &m;
            if (queuedBefore) {
                m.action = ACT_WALKING;
                sGroundTwirl.cooldown = 10;
                controller.buttonPressed = L_TRIG;
                EXPECT_EQ(mario_try_ground_twirl(&m), FALSE);
                EXPECT_EQ(sGroundTwirl.inputQueued > 0, TRUE);
            }
            m.action = outsideGround[route];
            controller.buttonPressed = queuedBefore ? 0 : L_TRIG;
            EXPECT_EQ(mario_try_ground_twirl(&m), FALSE);
            m.action = ACT_IDLE;
            controller.buttonPressed = controller.buttonDown = 0;
            sGroundTwirl.cooldown = 0;
            EXPECT_EQ(mario_try_ground_twirl(&m), FALSE);
        }
    }
    // Grounded recovery still retains a released request until ordinary control returns.
    const u32 recoveries[] = { ACT_CROUCHING, ACT_THROWING };
    for (unsigned route = 0; route < sizeof(recoveries) / sizeof(*recoveries); route++) {
        begin(MARIO_CAP_ON_HEAD);
        m.floor = &m;
        m.action = recoveries[route];
        controller.buttonPressed = L_TRIG;
        EXPECT_EQ(mario_try_ground_twirl(&m), FALSE);
        m.action = ACT_IDLE;
        controller.buttonPressed = controller.buttonDown = 0;
        EXPECT_EQ(mario_try_ground_twirl(&m), TRUE);
    }
    return TRUE;
}

static int scripted_enemy_body_contact(void) {
    const BehaviorScript *behaviors[] = { bhvBalconyBigBoo, bhvMerryGoRoundBigBoo, bhvGhostHuntBigBoo,
                                          bhvWigglerHead };
    const f32 scales[] = { 1.0f, 2.5f, 3.0f, 4.0f };
    for (s32 enemy = 0; enemy < 4; enemy++) {
        for (s32 scale = 0; scale < 4; scale++) {
            f32 bodyRadius = (enemy == 3 ? 30.0f : 40.0f) * scales[scale];
            f32 hitboxRadius = (enemy == 3 ? 60.0f : 140.0f) * scales[scale];
            for (s32 air = 0; air < 2; air++) {
                for (s32 inside = 0; inside < 2; inside++) {
                    begin(MARIO_CAP_ON_HEAD);
                    mario.hitboxRadius = 37.0f;
                    m.action = air ? ACT_L_AIR_TWIRL : ACT_GROUND_TWIRL;
                    struct Object target = { .behavior = behaviors[enemy],
                                             .large = enemy == 3,
                                             .hitboxRadius = hitboxRadius,
                                             .hurtboxRadius = bodyRadius,
                                             .oPosX = 37.0f + bodyRadius + (inside ? -0.5f : 0.0f) };
                    enum DefensiveContactResult expected = DEFENSIVE_CONTACT_NORMAL;
                    if (!inside) {
                        expected = DEFENSIVE_CONTACT_SKIP;
                    } else if (!air || enemy == 3) {
                        expected = DEFENSIVE_CONTACT_STOP;
                    }
                    EXPECT_EQ(mario_handle_defensive_contact(&m, &target, INTERACT_BOUNCE_TOP, TRUE),
                              expected);
                    if (!inside) {
                        EXPECT_EQ(m.action, air ? ACT_L_AIR_TWIRL : ACT_GROUND_TWIRL);
                        EXPECT_EQ(m.hurtCounter, 0);
                        EXPECT_EQ(sounds, 0);
                    }
                    begin(MARIO_CAP_ON_HEAD);
                    m.action = ACT_IDLE;
                    EXPECT_EQ(mario_handle_defensive_contact(&m, &target, INTERACT_BOUNCE_TOP, FALSE),
                              DEFENSIVE_CONTACT_NORMAL);
                    target.oInteractionSubtype = INT_SUBTYPE_BOWSER_ATTACK;
                    m.action = ACT_GROUND_TWIRL;
                    EXPECT_EQ(mario_handle_defensive_contact(&m, &target, INTERACT_BOUNCE_TOP, TRUE),
                              DEFENSIVE_CONTACT_NORMAL);
                }
            }
        }
    }
    return TRUE;
}

static int bowser_tail_dispatch(void) {
    for (int contacts = 0; contacts <= 4; contacts++) {
        begin(MARIO_CAP_ON_HEAD);
        struct Object bowser = { .behavior = bhvBowser,
                                 .activeFlags = 1,
                                 .oOpacity = 255,
                                 .hitboxRadius = 400,
                                 .hitboxHeight = 400,
                                 .oIntangibleTimer = -1 };
        struct Object tail = { .behavior = bhvBowserTailAnchor,
                               .activeFlags = 1,
                               .parentObj = &bowser,
                               .oPosZ = -250,
                               .hitboxRadius = 150,
                               .hitboxHeight = 50,
                               .hitboxDownOffset = -50 };
        bowser.prevObj = &tail;
        gObjectLists[OBJ_LIST_GENACTOR].header.next = &bowser;
        bowser.header.next = &gObjectLists[OBJ_LIST_GENACTOR];
        m.action = ACT_PUNCHING;
        m.actionArg = ACT_ARG_PUNCH_SEQUENCE_YAH;
        m.pos[2] = -300;
        mario.hitboxRadius = 37;
        mario.hitboxHeight = 160;
        struct Object flames[4] = { 0 };
        for (int i = 0; i < contacts; i++) {
            flames[i].oInteractType = INTERACT_FLAME;
            mario.collidedObjs[i] = &flames[i];
        }
        mario.numCollidedObjs = contacts;
        m.collidedObjInteractTypes = contacts ? INTERACT_FLAME : 0;
        handlerMode = 7;
        mario_process_interactions(&m);
        EXPECT_EQ(m.interactObj == &bowser, TRUE);
        EXPECT_EQ((m.input & INPUT_INTERACT_OBJ_GRABBABLE) != 0, TRUE);
        EXPECT_EQ(mario.numCollidedObjs, contacts);
        // Repeat with only a stale Bowser contact: leaving the tail must close the grab.
        tail.oPosZ = -600;
        m.input = 0;
        m.interactObj = NULL;
        mario.numCollidedObjs = 1;
        mario.collidedObjs[0] = &bowser;
        bowser.oInteractType = INTERACT_GRABBABLE;
        m.collidedObjInteractTypes = INTERACT_GRABBABLE;
        mario_process_interactions(&m);
        EXPECT_EQ(m.interactObj == NULL, TRUE);
        EXPECT_EQ(m.input & INPUT_INTERACT_OBJ_GRABBABLE, 0);
        // Releasing cooldown cannot be bypassed by the independent contact path.
        tail.oPosZ = -250;
        bowser.oBowserGrabCooldown = 1;
        m.collidedObjInteractTypes = 0;
        mario_process_interactions(&m);
        EXPECT_EQ(m.input & INPUT_INTERACT_OBJ_GRABBABLE, 0);
        bowser.oBowserGrabCooldown = 0;
        m.pos[1] = 100;
        EXPECT_EQ(mario_get_bowser_tail_contact(&m) == &bowser, TRUE);
        m.pos[1] = 100.01f;
        EXPECT_EQ(mario_get_bowser_tail_contact(&m) == NULL, TRUE);
        m.pos[1] = 0;
        bowser.oAction = BOWSER_ACT_TELEPORT;
        EXPECT_EQ(mario_get_bowser_tail_contact(&m) == NULL, TRUE);
        bowser.oAction = BOWSER_ACT_DEFAULT;
        tail.activeFlags = ACTIVE_FLAG_DEACTIVATED;
        EXPECT_EQ(mario_get_bowser_tail_contact(&m) == NULL, TRUE);
        tail.activeFlags = 1;
        for (int age = 0; age < 5; age++) {
            m.action = ACT_DIVE;
            m.actionState = age;
            m.input = m.collidedObjInteractTypes = 0;
            m.interactObj = NULL;
            mario_process_interactions(&m);
            EXPECT_EQ((m.input & INPUT_INTERACT_OBJ_GRABBABLE) != 0, age < 3);
        }
        m.action = ACT_PUNCHING;
        m.actionState = 0;
        m.input = 0;
        // Damage still resolves before a ready grab.
        mario.collidedObjs[0] = &flames[0];
        flames[0].oInteractType = INTERACT_FLAME;
        m.collidedObjInteractTypes = INTERACT_FLAME;
        handlerMode = 2;
        mario_process_interactions(&m);
        EXPECT_EQ(m.action, ACT_BACKWARD_GROUND_KB);
        EXPECT_EQ(m.input & INPUT_INTERACT_OBJ_GRABBABLE, 0);
    }
    return TRUE;
}

static int option_entry_and_pose(void) {
    begin(MARIO_CAP_ON_HEAD);
    m.action = ACT_IDLE;
    controller.buttonPressed = L_TRIG;
    sGroundTwirl.inputQueued = 6;
    sGroundTwirl.bufferedInput = INPUT_A_PRESSED;
    sGroundTwirl.bufferedInputFrames = 6;
    sTestGameOptions[GO_TWIRLS] = FALSE;
    EXPECT_EQ(mario_try_ground_twirl(&m), FALSE);
    EXPECT_EQ(m.action, ACT_IDLE);
    EXPECT_EQ(sGroundTwirl.inputQueued | sGroundTwirl.bufferedInput | sGroundTwirl.bufferedInputFrames,
              0);
    EXPECT_EQ(mario_start_ground_twirl(&m, TRUE), FALSE);
    EXPECT_EQ(m.action, ACT_IDLE);
    m.action = ACT_GROUND_TWIRL;
    EXPECT_EQ(mario_ground_twirl_is_active(&m), FALSE);
    sTestGameOptions[GO_TWIRLS] = TRUE;
    sTestGameOptions[GO_HELD_TWIRL] = FALSE;
    begin(MARIO_CAP_ON_HEAD);
    gObjectLists[0].header.next = &gObjectLists[0];
    mario_start_ground_twirl(&m, FALSE);
    for (int frame = 0; frame < GROUND_TWIRL_FRAMES && m.action == ACT_GROUND_TWIRL; frame++) {
        m.actionArg = frame + 1;
        mario_ground_twirl_action(&m);
        EXPECT_EQ(sGroundTwirl.heldPoseFrames, 0);
    }
    EXPECT_EQ(m.action != ACT_GROUND_TWIRL, TRUE);
    sTestGameOptions[GO_HELD_TWIRL] = TRUE;
    return TRUE;
}

int main(int argc, char **argv) {
    test_init(argc, argv);
    RUN_CASE("twirl.option_entry_and_pose", option_entry_and_pose);
    RUN_CASE("twirl.held_pose_animation", held_pose_animation);
    RUN_CASE("twirl.ground_timing", ground_twirl_timing);
    RUN_CASE("twirl.air_twirl_contacts", air_twirl_contacts);
    RUN_CASE("twirl.enemy_hit_rumble", enemy_hit_rumble);
    RUN_CASE("twirl.scripted_enemy_body_contact", scripted_enemy_body_contact);
    RUN_CASE("twirl.air_twirl_ground_handoff", air_twirl_ground_handoff);
    RUN_CASE("twirl.thicc_attack_contact", thicc_attack_contact);
    RUN_CASE("twirl.offset_hitbox", offset_hitbox);
    RUN_CASE("twirl.breakdance_damage_dispatch", breakdance_damage_dispatch);
    RUN_CASE("twirl.startup_priority", startup_priority);
    RUN_CASE("twirl.thicc_pound_range", thicc_pound_range);
    RUN_CASE("twirl.thicc_pound_eligibility", thicc_pound_eligibility);
    RUN_CASE("twirl.stun_recoil", stun_recoil);
    RUN_CASE("twirl.grace_and_repeats", grace_and_repeats);
    RUN_CASE("twirl.mobile_enemy_body_reach", mobile_enemy_body_reach);
    RUN_CASE("twirl.snufit_vertical_contact", snufit_vertical_contact);
    RUN_CASE("twirl.nondamaging_contacts", nondamaging_contacts);
    RUN_CASE("twirl.native_knockback", native_knockback);
    RUN_CASE("twirl.capless_hazard_recoil", capless_hazard_recoil);
    RUN_CASE("twirl.bowser_twirl_dispatch", bowser_twirl_dispatch);
    RUN_CASE("twirl.snufit_parry_recovery", snufit_parry_recovery);
    RUN_CASE("twirl.projectile_immunity_contacts", projectile_immunity_contacts);
    RUN_CASE("twirl.projectile_immunity_boundaries", projectile_immunity_boundaries);
    RUN_CASE("twirl.bowser_tail_dispatch", bowser_tail_dispatch);
    return test_finish();
}
