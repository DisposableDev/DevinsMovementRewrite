#include "player_stats.h"
#include <PR/ultratypes.h>

#include "sm64.h"
#include "area.h"
#include "audio/external.h"
#include "behavior_data.h"
#include "camera.h"
#include "engine/math_util.h"
#include "game_options.h"
#include "game_options_runtime.h"
#include "interaction.h"
#include "mario.h"
#include "mario_character.h"
#include "mario_dab.h"
#include "mario_actions_moving.h"
#include "mario_step.h"
#include "object_fields.h"
#include "object_helpers.h"
#include "object_constants.h"
#include "object_list_processor.h"
#include "rumble_init.h"

// Grounded L twirl with running movement.
#define GROUND_TWIRL_FRAMES 12       // Normal twirl duration.
#define GROUND_TWIRL_ACTIVE_FRAMES 5 // Initial enemy-hit window.
#define GROUND_TWIRL_CANCEL_FRAMES 5 // Final recovery frames open to jump, attack, crouch, or walking.
#define GROUND_TWIRL_WALL_CANCEL_FRAMES 7
#define GROUND_TWIRL_WALL_WALK_CANCEL_FRAMES 5
#define GROUND_TWIRL_STUN_MIN_FRAMES 6    // Ground stun at zero Mario speed.
#define GROUND_TWIRL_STUN_MAX_FRAMES 60   // Ground stun at or above the speed threshold.
#define GROUND_TWIRL_STUN_MAX_SPEED 70.0f // Mario's horizontal speed for maximum ground stun.

#define GROUND_TWIRL_REACH 0.0f             // Extra enemy reach beyond its hitbox radius.
#define GROUND_TWIRL_PUSH_BASE_SPEED 20.0f  // Base horizontal enemy knockback.
#define GROUND_TWIRL_PUSH_SPEED_SCALE 0.75f // Multiplier on Mario's speed for knockback.
#define GROUND_TWIRL_PUSH_MAX_SPEED 70.0f   // Horizontal enemy knockback cap.
#define GROUND_TWIRL_LAUNCH_BASE_SPEED 20.0f // Base upward enemy launch.
#define GROUND_TWIRL_LAUNCH_SPEED_SCALE 0.3f // Multiplier on Mario's speed for upward launch.
#define GROUND_TWIRL_LAUNCH_MAX_SPEED 60.0f  // Upward enemy launch cap.
#define GROUND_TWIRL_LAUNCH_FRAMES 30        // Initial airborne timer; flight continues until landing.
#define METAL_ENEMY_CONTACT_TIMEOUT 15       // Updates after the first passive Metal hit.

#define HE_THICC_GROUND_POUND_RADIUS 800.0f
#define HE_THICC_GROUND_POUND_HEIGHT 200.0f
#define HE_THICC_GROUND_POUND_STUN_FRAMES 60

#define GROUND_TWIRL_WALL_SLOW_TURN_FRAMES 12    // Reduced steering after a wall bounce.
#define GROUND_TWIRL_WALL_TURN_STEP DEGREES(3.3) // Maximum turn per reduced-steering frame.
#define GROUND_TWIRL_WALL_BOOST_MULTIPLIER MARIO_TWIRL_WALL_BOOST_MULTIPLIER // Before ramp and decay.
#define GROUND_TWIRL_WALL_BOOST_RAMP_FRAMES 2      // Frames from wall hit to full speed bonus.
#define GROUND_TWIRL_WALL_BOOST_DECAY_FRAMES 6     // Frames for the bonus to fade from full to zero.
#define GROUND_TWIRL_POST_BOUNCE_SPEED_LIMIT 64.0f // Maximum speed from a wall rebound.
#define GROUND_TWIRL_EARLY_BOUNCE_FRAMES 3         // First-bounce bonus window after twirl starts.
#define GROUND_TWIRL_EARLY_BOUNCE_SPEED 10.0f      // Extra speed for an early first bounce.
#define GROUND_TWIRL_EARLY_BOUNCE_SPEED_MULTIPLIER 1.15f // Multiplier on that extra speed.
#define GROUND_TWIRL_SPEED_RECOVERY 1.4f       // Maximum per-frame rise of the stick-based speed floor.
#define GROUND_TWIRL_SLIPPERY_SLOPE_DRAG 0.98f // Speed retained per frame on slippery slopes.
#define GROUND_TWIRL_WALL_BOUNCE_SPEED_LOSS 0.06f
#define GROUND_TWIRL_WALL_BOUNCE_EXTRA_DRAG 0.15f
#define GROUND_TWIRL_WALL_BOUNCE_ZERO_SPEED_COUNT 18

#define GROUND_TWIRL_VISUAL_TURNS 1       // Full visible turns over the normal twirl.
#define GROUND_TWIRL_VISUAL_RAMP_FRAMES 2 // Normal spin's acceleration phase length.
#define GROUND_TWIRL_WALL_VISUAL_TURNS 1  // Full visible turn after a wall bounce.
#define GROUND_TWIRL_VISUAL_FRAMES 8      // Full spin duration when uninterrupted.
#define GROUND_TWIRL_WALL_VISUAL_FRAMES (GROUND_TWIRL_FRAMES - GROUND_TWIRL_WALL_CANCEL_FRAMES)
#define GROUND_TWIRL_HAT_LOSS_FRAMES 5
#define GROUND_TWIRL_SLEEP_WITH_CAP_FRAMES 300
#define GROUND_TWIRL_SLEEP_CAPLESS_FRAMES 150

// Keep twirl input, visual state, and contact generation together.
static struct {
    s32 cooldown;
    s32 cooldownResetByContact;
    s32 inputQueued; // Remaining gameplay updates for a queued L press.
    u32 bufferedInput;
    s32 bufferedInputFrames;
    u32 id;
    f32 startSpeed;
    s16 visualStartYaw;
    s16 visualPhaseYaw;
    s16 visualPrevFaceYaw;
    s32 visualTargetOffset;
    s32 visualWallBounce;
    f32 bounceBonus;
    f32 appliedBonus;
    s32 boostAge;
    s32 hasBounced;
    u8 wallBounceCount;
    s32 wallContact;
    s16 wallContactYaw;
    s32 earlySpeedCapActive;
    f32 earlySpeedCap;
    s32 lHeldThroughout;
    s32 heldPoseFrames;
} sGroundTwirl;

// Input, cooldown, and startup.
void mario_ground_twirl_reset_cooldown(void) {
    // Enemy contact or a pickup earns another twirl once the current recovery ends.
    sGroundTwirl.cooldown = 0;
    sGroundTwirl.cooldownResetByContact = TRUE;
}

void mario_ground_twirl_reset_wall_bounces(void) {
    sGroundTwirl.wallBounceCount = 0;
    sGroundTwirl.wallContact = FALSE;
}

// A new spin and Mario reset clear rebound state, but preserve contact generation.
static void mario_ground_twirl_reset_rebound_state(void) {
    sGroundTwirl.bounceBonus = 0.0f;
    sGroundTwirl.appliedBonus = 0.0f;
    sGroundTwirl.boostAge = 0;
    sGroundTwirl.hasBounced = FALSE;
    sGroundTwirl.wallBounceCount = 0;
    sGroundTwirl.wallContact = FALSE;
    sGroundTwirl.earlySpeedCapActive = FALSE;
    sGroundTwirl.earlySpeedCap = 0.0f;
}

static void mario_ground_twirl_finish(void) {
    sGroundTwirl.cooldown = sGroundTwirl.cooldownResetByContact ? 0 : MARIO_TWIRL_COOLDOWN_FRAMES;
    sGroundTwirl.bufferedInput = 0;
    sGroundTwirl.bufferedInputFrames = 0;
    sGroundTwirl.heldPoseFrames = 0;
}

void mario_ground_twirl_cancel_queued_input(void) {
    sGroundTwirl.inputQueued = FALSE;
}

// Call once per Mario update to age cooldown and buffered cancel-input lifetime.
void mario_ground_twirl_update_cooldown(void) {
    if (sGroundTwirl.bufferedInputFrames > 0 && --sGroundTwirl.bufferedInputFrames == 0) {
        sGroundTwirl.bufferedInput = 0;
    }
    if (sGroundTwirl.cooldown > 0) {
        sGroundTwirl.cooldown--;
    }
}

s32 mario_try_ground_twirl(struct MarioState *m) {
    if (!game_options_get(GO_TWIRLS)) {
        mario_ground_twirl_init();
        return FALSE;
    }
    mario_ground_twirl_update_cooldown();
    s32 actionGroup = m->action & ACT_GROUP_MASK;
    if ((m->action & ACT_FLAG_AIR) || m->health < 0x100 || actionGroup == ACT_GROUP_SUBMERGED
        || actionGroup == ACT_GROUP_CUTSCENE || actionGroup == ACT_GROUP_AUTOMATIC) {
        sGroundTwirl.inputQueued = FALSE;
        return FALSE;
    }
    // A press survives release for six updates; re-pressing refreshes that window.
    if (sGroundTwirl.inputQueued > 0) {
        sGroundTwirl.inputQueued--;
    }
    if (m->controller->buttonPressed & L_TRIG) {
        sGroundTwirl.inputQueued = 6;
    }
    if (sGroundTwirl.cooldown > 0 || !sGroundTwirl.inputQueued || (m->input & INPUT_Z_DOWN)) {
        return FALSE;
    }
    switch (m->action) {
        case ACT_IDLE:
        case ACT_PANTING:
        case ACT_WALKING:
        case ACT_TURNING_AROUND:
        case ACT_FINISH_TURNING_AROUND:
        case ACT_BRAKING:
        case ACT_BRAKING_STOP:
        case ACT_DECELERATING:
            break;
        case ACT_FREEFALL_LAND_STOP:
            if (m->prevAction != ACT_FORWARD_ROLLOUT && m->prevAction != ACT_BACKWARD_ROLLOUT) {
                return FALSE;
            }
            break;
        default:
            return FALSE;
    }
    if (m->floor == NULL || m->heldObj != NULL || m->riddenObj != NULL || m->health < 0x100
        || m->quicksandDepth > 0.0f
        || (m->input
            & (INPUT_OFF_FLOOR | INPUT_STOMPED | INPUT_SQUISHED | INPUT_A_PRESSED
               | INPUT_FIRST_PERSON))) {
        return FALSE;
    }
    // Give L-then-Z a frame to complete the dab without flashing the twirl pose.
    if ((m->controller->buttonPressed & L_TRIG) && mario_dab_can_start_from_neutral(m)) {
        return FALSE;
    }
    return mario_start_ground_twirl(m, FALSE);
}

// Zero is an untouched object's default hit ID, so skip it when the generation wraps.
static void mario_twirl_advance_contact_generation(void) {
    if (++sGroundTwirl.id == 0) {
        sGroundTwirl.id = 1;
    }
}

void mario_twirl_begin_contacts(void) {
    mario_ground_twirl_reset_contact_damage();
    mario_twirl_advance_contact_generation();
}

s32 mario_start_ground_twirl(struct MarioState *m, s32 continuation) {
    if (!game_options_get(GO_TWIRLS)) {
        return FALSE;
    }
    sGroundTwirl.cooldown = GROUND_TWIRL_FRAMES + MARIO_TWIRL_COOLDOWN_FRAMES;
    sGroundTwirl.cooldownResetByContact = FALSE;
    sGroundTwirl.inputQueued = FALSE;
    sGroundTwirl.bufferedInput = 0;
    sGroundTwirl.bufferedInputFrames = 0;
    if (!continuation) {
        player_stats_twirl_start(FALSE, continuation);
        mario_twirl_begin_contacts();
    }
    m->airTwirlContinue = FALSE;
    sGroundTwirl.startSpeed = absf(m->forwardVel);
    sGroundTwirl.visualStartYaw = continuation ? m->marioObj->header.gfx.angle[1] : m->faceAngle[1];
    sGroundTwirl.visualPhaseYaw = sGroundTwirl.visualStartYaw;
    sGroundTwirl.visualPrevFaceYaw = m->faceAngle[1];
    sGroundTwirl.visualTargetOffset = 0;
    sGroundTwirl.visualWallBounce = FALSE;
    mario_ground_twirl_reset_rebound_state();
    sGroundTwirl.lHeldThroughout = TRUE;
    sGroundTwirl.heldPoseFrames = 0;
    return set_mario_action(m, ACT_GROUND_TWIRL, 0);
}

// Enemy targeting and harmless stun.
static s32 mario_ground_twirl_is_boo(struct Object *obj) {
    const BehaviorScript *behavior = obj->behavior;
    return behavior == segmented_to_virtual(bhvBoo) || behavior == segmented_to_virtual(bhvGhostHuntBoo)
           || behavior == segmented_to_virtual(bhvMerryGoRoundBoo)
           || behavior == segmented_to_virtual(bhvBooWithCage);
}

// Mobile enemies take harmless knockback; Boos use their bounced-on reaction instead.
// Rooted enemies and bosses retain their own scripted movement.
s32 mario_ground_twirl_is_enemy(struct Object *obj) {
    const BehaviorScript *behavior = obj->behavior;
    return behavior == segmented_to_virtual(bhvGoomba)
#ifdef FLOOMBAS
           || behavior == segmented_to_virtual(bhvFloomba)
#endif
           || behavior == segmented_to_virtual(bhvKoopa) || behavior == segmented_to_virtual(bhvBobomb)
           || behavior == segmented_to_virtual(bhvChuckya)
           || behavior == segmented_to_virtual(bhvHeaveHo) || behavior == segmented_to_virtual(bhvSpiny)
           || behavior == segmented_to_virtual(bhvScuttlebug)
           || behavior == segmented_to_virtual(bhvSkeeter)
           || behavior == segmented_to_virtual(bhvFlyGuy)
           || behavior == segmented_to_virtual(bhvSpindrift)
           || behavior == segmented_to_virtual(bhvMontyMole)
           || behavior == segmented_to_virtual(bhvMoneybag)
           || behavior == segmented_to_virtual(bhvSwoop) || mario_ground_twirl_is_boo(obj)
           || behavior == segmented_to_virtual(bhvKlepto) || behavior == segmented_to_virtual(bhvSnufit)
           || behavior == segmented_to_virtual(bhvEnemyLakitu);
}

s32 mario_ground_twirl_is_oversized_enemy(struct Object *obj) {
    const BehaviorScript *behavior = obj->behavior;

    if (behavior == segmented_to_virtual(bhvChuckya)) {
        return TRUE;
    }
    if ((behavior == segmented_to_virtual(bhvGoomba)
#ifdef FLOOMBAS
         || behavior == segmented_to_virtual(bhvFloomba)
#endif
             )
        && obj->oGoombaSize == GOOMBA_SIZE_HUGE) {
        return TRUE;
    }
    // Include other oversized mobile enemies without changing large Boos' twirl reaction.
    return mario_ground_twirl_is_enemy(obj) && !mario_ground_twirl_is_boo(obj)
           && obj->hitboxRadius >= 180.0f && obj->hitboxHeight >= 130.0f;
}

s32 mario_ground_twirl_is_immune_enemy(struct Object *obj) {
    const BehaviorScript *behavior = obj->behavior;

    return behavior == segmented_to_virtual(bhvKingBobomb)
           || behavior == segmented_to_virtual(bhvBowser)
           || behavior == segmented_to_virtual(bhvBowserBodyAnchor)
           || behavior == segmented_to_virtual(bhvPiranhaPlant)
           || behavior == segmented_to_virtual(bhvFirePiranhaPlant)
           || behavior == segmented_to_virtual(bhvSmallWhomp)
           || behavior == segmented_to_virtual(bhvWhompKingBoss)
           || behavior == segmented_to_virtual(bhvChainChomp)
           || behavior == segmented_to_virtual(bhvPokey)
           || behavior == segmented_to_virtual(bhvPokeyBodyPart)
           || behavior == segmented_to_virtual(bhvWigglerHead)
           || behavior == segmented_to_virtual(bhvWigglerBody)
           || behavior == segmented_to_virtual(bhvMrI) || behavior == segmented_to_virtual(bhvMadPiano)
           || behavior == segmented_to_virtual(bhvHauntedChair)
           || behavior == segmented_to_virtual(bhvEyerokHand)
           || behavior == segmented_to_virtual(bhvFlyingBookend)
           || behavior == segmented_to_virtual(bhvBulletBill)
           || behavior == segmented_to_virtual(bhvHomingAmp)
           || behavior == segmented_to_virtual(bhvCirclingAmp)
           || behavior == segmented_to_virtual(bhvMrBlizzard);
}

s32 mario_ground_twirl_is_large_enemy(struct Object *obj) {
    const BehaviorScript *behavior = obj->behavior;

    return mario_ground_twirl_is_oversized_enemy(obj) || behavior == segmented_to_virtual(bhvKingBobomb)
           || behavior == segmented_to_virtual(bhvBowser)
           || behavior == segmented_to_virtual(bhvChainChomp)
           || behavior == segmented_to_virtual(bhvSmallWhomp)
           || behavior == segmented_to_virtual(bhvWhompKingBoss)
           || behavior == segmented_to_virtual(bhvPokey)
           || behavior == segmented_to_virtual(bhvPokeyBodyPart)
           || behavior == segmented_to_virtual(bhvWigglerHead)
           || behavior == segmented_to_virtual(bhvWigglerBody)
           || behavior == segmented_to_virtual(bhvEyerokHand)
           || behavior == segmented_to_virtual(bhvMadPiano);
}

s32 mario_twirl_can_stun_enemy(struct Object *obj) {
    return mario_ground_twirl_is_enemy(obj) && !mario_ground_twirl_is_oversized_enemy(obj)
           && !(obj_has_behavior(obj, bhvKoopa)
                && obj->oKoopaMovementType >= KOOPA_BP_KOOPA_THE_QUICK_BASE)
           && (!mario_ground_twirl_is_boo(obj) || obj->oInteractType == INTERACT_BOUNCE_TOP);
}

static f32 mario_ground_twirl_push_speed(f32 marioSpeed) {
    return MIN(GROUND_TWIRL_PUSH_MAX_SPEED,
               GROUND_TWIRL_PUSH_BASE_SPEED + marioSpeed * GROUND_TWIRL_PUSH_SPEED_SCALE);
}

static void mario_launch_stunned_enemy(struct MarioState *m, struct Object *obj, f32 marioSpeed,
                                       u32 hitId) {
    f32 pushSpeed = mario_ground_twirl_push_speed(marioSpeed);
    f32 launchSpeed =
        MIN(GROUND_TWIRL_LAUNCH_MAX_SPEED,
            GROUND_TWIRL_LAUNCH_BASE_SPEED + marioSpeed * GROUND_TWIRL_LAUNCH_SPEED_SCALE);
    f32 dx = obj->oPosX - m->pos[0];
    f32 dz = obj->oPosZ - m->pos[2];

    obj->oGroundTwirlLastHitId = hitId;
    obj->oMoveAngleYaw = dx == 0.0f && dz == 0.0f ? m->faceAngle[1] : atan2s(dz, dx);
    obj->oForwardVel = pushSpeed;
    obj->oVelY = MAX(obj->oVelY, launchSpeed);
    // The object updater handles punch-like flight without running enemy AI or applying damage.
    obj->oGroundTwirlContactRebound = FALSE;
    obj->oGroundTwirlStunDuration =
        GROUND_TWIRL_STUN_MIN_FRAMES
        + (s32) (MIN(marioSpeed, GROUND_TWIRL_STUN_MAX_SPEED)
                 * (GROUND_TWIRL_STUN_MAX_FRAMES - GROUND_TWIRL_STUN_MIN_FRAMES)
                 / GROUND_TWIRL_STUN_MAX_SPEED);
    obj->oGroundTwirlStunTimer = -GROUND_TWIRL_LAUNCH_FRAMES;
    for (s32 i = 0; i < 3; i++) {
        struct Object *star =
            try_to_spawn_object(0, 1.0f, obj, MODEL_GROUND_TWIRL_STUN_STAR, bhvGroundTwirlStunStar);
        if (star == NULL) {
            break;
        }
        star->oBehParams2ndByte = i;
        star->oGroundTwirlLastHitId = hitId;
    }
}

void mario_metal_stun_enemy_on_contact(struct MarioState *m, struct Object *obj) {
    s32 wingTwirl = mario_air_twirl_is_active(m) && (m->flags & MARIO_WING_CAP);
    if (obj->oIntangibleTimer != 0 || obj->metalContactPendingDefeat
        || (obj->oInteractStatus & INT_STATUS_WAS_ATTACKED)
        || (obj_has_behavior(obj, bhvKoopa)
            && obj->oKoopaMovementType >= KOOPA_BP_KOOPA_THE_QUICK_BASE)) {
        return;
    }
    if (obj->oGroundTwirlStunTimer != 0 || obj->oGroundTwirlContactRebound != 0) {
        f32 dx = obj->oPosX - m->pos[0];
        f32 dz = obj->oPosZ - m->pos[2];
        s32 grounded = obj->oGroundTwirlStunTimer > 0
                       && (obj->oMoveFlags & (OBJ_MOVE_MASK_ON_GROUND | OBJ_MOVE_UNDERWATER_ON_GROUND))
                       && obj->oVelY <= 5.0f;
        s32 atWall = (obj->oMoveFlags & OBJ_MOVE_HIT_WALL) != 0;
        // Wing twirls also accept airborne targets after the same contact grace.
        // Stationary/retreating overlap and contact rebounds stay harmless.
        if (obj->metalContactCooldown == 0 && obj->oGroundTwirlContactRebound == 0
            && (wingTwirl || grounded || atWall)
            && (wingTwirl || m->action == ACT_WALKING || m->action == ACT_DECELERATING
                || m->action == ACT_BRAKING || m->action == ACT_TURNING_AROUND
                || m->action == ACT_FINISH_TURNING_AROUND || m->action == ACT_METAL_WATER_WALKING)
            && m->vel[0] * dx + m->vel[2] * dz > 0.0f) {
            obj->metalContactPendingDefeat = TRUE;
            if (wingTwirl) {
                player_stats_twirl_hit(obj);
                queue_rumble_data(5, 80);
            }
            player_stats_metal_contact(obj, !wingTwirl);
        }
        return;
    }
    if (obj->metalContactCooldown != 0) {
        return;
    }
    if (mario_ground_twirl_is_boo(obj)) {
        if (obj->oInteractType == INTERACT_BOUNCE_TOP && obj->oAction == 1) {
            obj->oInteractStatus = INT_STATUS_INTERACTED | INT_STATUS_WAS_ATTACKED | ATTACK_FROM_ABOVE;
            if (wingTwirl) {
                player_stats_twirl_hit(obj);
                queue_rumble_data(5, 80);
            }
        }
        return;
    }
    if (!wingTwirl) {
        mario_twirl_advance_contact_generation();
    }
    mario_launch_stunned_enemy(m, obj, sqrtf(sqr(m->vel[0]) + sqr(m->vel[2])), sGroundTwirl.id);
    obj->metalContactCooldown = METAL_ENEMY_CONTACT_TIMEOUT;
    if (wingTwirl) {
        player_stats_twirl_hit(obj);
        player_stats_add(PS_ENEMIES_STUNNED_BY_TWIRLS, 1);
        queue_rumble_data(5, 80);
    }
}

// One shockwave at actual floor contact, including the underwater down-burst.
// Reuse harmless stun ownership, native damage/hazard handling and resume hooks.
void mario_ground_pound_impact(struct MarioState *m) {
    if (mario_character_palette() != MARIO_PALETTE_HE_THICC) {
        set_camera_shake_from_hit(SHAKE_GROUND_POUND);
        return;
    }
    set_camera_shake_from_hit(SHAKE_HE_THICC_GROUND_POUND);
    mario_twirl_advance_contact_generation();

    for (s32 list = 0; list < NUM_OBJ_LISTS; list++) {
        struct Object *head = (struct Object *) &gObjectLists[list];
        for (struct Object *obj = (struct Object *) head->header.next; obj != head;
             obj = (struct Object *) obj->header.next) {
            f32 enemyBottom = obj->oPosY - obj->hitboxDownOffset;
            if (obj->activeFlags == ACTIVE_FLAG_DEACTIVATED
                || (obj->activeFlags & (ACTIVE_FLAG_FAR_AWAY | ACTIVE_FLAG_IN_DIFFERENT_ROOM))
                || (obj->oRoom > 0 && gMarioCurrentRoom > 0 && obj->oRoom != gMarioCurrentRoom)
                || obj->oIntangibleTimer != 0 || obj->oHeldState != HELD_FREE
                || obj->hitboxRadius <= 0.0f || obj->oGroundTwirlContactRebound != 0
                || obj->metalContactPendingDefeat || (obj->oInteractStatus & INT_STATUS_WAS_ATTACKED)
                || !mario_ground_twirl_is_enemy(obj)
                || (obj_has_behavior(obj, bhvKoopa)
                    && obj->oKoopaMovementType >= KOOPA_BP_KOOPA_THE_QUICK_BASE)
                || m->pos[1] + HE_THICC_GROUND_POUND_HEIGHT < enemyBottom
                || enemyBottom + obj->hitboxHeight < m->pos[1] - HE_THICC_GROUND_POUND_HEIGHT) {
                continue;
            }
            f32 dx = obj->oPosX - m->pos[0];
            f32 dz = obj->oPosZ - m->pos[2];
            if (dx * dx + dz * dz > sqr(HE_THICC_GROUND_POUND_RADIUS)
                || mario_wall_between_object(m->pos, obj)) {
                continue;
            }
            if (mario_ground_twirl_is_boo(obj)) {
                // Native Boo bounce is harmless and keeps its flight/visibility AI.
                if (obj->oInteractType == INTERACT_BOUNCE_TOP && obj->oAction == 1) {
                    obj->oGroundTwirlBooPushSpeed = GROUND_TWIRL_PUSH_BASE_SPEED;
                    obj->oInteractStatus =
                        INT_STATUS_INTERACTED | INT_STATUS_WAS_ATTACKED | ATTACK_FROM_ABOVE;
                }
                continue;
            }
            if (obj->oGroundTwirlStunTimer != 0) {
                // Refresh an existing stun without relaunching or duplicating stars.
                obj->oGroundTwirlStunDuration =
                    MAX(obj->oGroundTwirlStunDuration, HE_THICC_GROUND_POUND_STUN_FRAMES);
                if (obj->oGroundTwirlStunTimer > 0) {
                    obj->oGroundTwirlStunTimer =
                        MAX(obj->oGroundTwirlStunTimer, HE_THICC_GROUND_POUND_STUN_FRAMES);
                }
            } else {
                mario_launch_stunned_enemy(m, obj, 0.0f, sGroundTwirl.id);
                obj->oGroundTwirlStunDuration = HE_THICC_GROUND_POUND_STUN_FRAMES;
            }
        }
    }
}

void mario_twirl_push_enemies(struct MarioState *m) {
    s32 list;
    struct Object *firstHit = NULL;
    s32 airTwirl = mario_air_twirl_is_active(m);
    f32 marioSpeed = sqrtf(sqr(m->vel[0]) + sqr(m->vel[2]));
    for (list = 0; list < NUM_OBJ_LISTS; list++) {
        struct Object *head = (struct Object *) &gObjectLists[list];
        struct Object *obj = (struct Object *) head->header.next;
        for (; obj != head; obj = (struct Object *) obj->header.next) {
            f32 enemyBottom = obj->oPosY - obj->hitboxDownOffset;
            f32 dx;
            f32 dz;
            f32 reach;
            s32 isBoo;
            if (mario_vanish_ignores_hostile_contact(m, obj, obj->oInteractType)) {
                continue;
            }
            if (obj->activeFlags == ACTIVE_FLAG_DEACTIVATED
                || (obj->activeFlags & (ACTIVE_FLAG_FAR_AWAY | ACTIVE_FLAG_IN_DIFFERENT_ROOM))
                || (obj->oRoom > 0 && gMarioCurrentRoom > 0 && obj->oRoom != gMarioCurrentRoom)
                || obj->oIntangibleTimer != 0 || obj->hitboxRadius <= 0.0f
                || obj->oGroundTwirlContactRebound != 0
                // Match Mario's full collision height during the ground twirl.
                || m->pos[1] + 160.0f < enemyBottom || enemyBottom + obj->hitboxHeight < m->pos[1]) {
                continue;
            }
            dx = obj->oPosX - m->pos[0];
            dz = obj->oPosZ - m->pos[2];
            // Some enemies use a broad interaction radius around a smaller body.
            reach = GROUND_TWIRL_REACH
                    + (obj->hurtboxRadius > 0.0f
                           ? MIN(obj->hitboxRadius, obj->hurtboxRadius + m->marioObj->hitboxRadius)
                           : obj->hitboxRadius);
            if (dx * dx + dz * dz > reach * reach) {
                continue;
            }
            if (!mario_ground_twirl_is_enemy(obj) || mario_ground_twirl_is_oversized_enemy(obj)) {
                continue;
            }
            if (mario_wall_between_object(m->pos, obj)) {
                continue;
            }
            if (airTwirl) {
                if (!mario_twirl_can_stun_enemy(obj)) {
                    continue;
                }
                if (m->flags & MARIO_WING_CAP) {
                    mario_metal_stun_enemy_on_contact(m, obj);
                    continue;
                }
                if (obj->oGroundTwirlStunTimer != 0) {
                    continue;
                }
            }
            if (obj->oGroundTwirlLastHitId == sGroundTwirl.id) {
                continue;
            }
            isBoo = mario_ground_twirl_is_boo(obj);
            if (isBoo && obj->oInteractType != INTERACT_BOUNCE_TOP) {
                continue;
            }
            if (isBoo) {
                player_stats_twirl_hit(obj);
                obj->oGroundTwirlLastHitId = sGroundTwirl.id;
                mario_ground_twirl_reset_cooldown();
                obj->oGroundTwirlStunTimer = 0;
                obj->oGroundTwirlBooPushSpeed = mario_ground_twirl_push_speed(marioSpeed);
                // Boo interprets an attack from above as a bounce, without hurting it.
                obj->oInteractStatus =
                    INT_STATUS_INTERACTED | INT_STATUS_WAS_ATTACKED | ATTACK_FROM_ABOVE;
                if (firstHit == NULL) {
                    firstHit = obj;
                }
                continue;
            }
            mario_ground_twirl_reset_cooldown();
            player_stats_twirl_hit(obj);
            player_stats_add(PS_ENEMIES_STUNNED_BY_TWIRLS, 1);
            mario_launch_stunned_enemy(m, obj, marioSpeed, sGroundTwirl.id);
            if (firstHit == NULL) {
                firstHit = obj;
            }
        }
    }
    if (firstHit != NULL) {
        queue_rumble_data(5, 80);
        if (airTwirl) {
            mario_air_twirl_rebound_from_stun(m, firstHit);
        } else {
            mario_ground_twirl_rebound_from_stun(m, firstHit);
        }
    }
}

// Ground speed and wall rebound.
// Limit forward speed without discarding extra velocity from moving ground or wind.
static void mario_ground_twirl_limit_speed(struct MarioState *m, f32 maximumSpeed) {
    if (m->forwardVel > maximumSpeed) {
        f32 excess = m->forwardVel - maximumSpeed;
        f32 excessX = excess * sins(m->faceAngle[1]);
        f32 excessZ = excess * coss(m->faceAngle[1]);
        m->forwardVel = maximumSpeed;
        m->slideVelX -= excessX;
        m->slideVelZ -= excessZ;
        m->vel[0] -= excessX;
        m->vel[2] -= excessZ;
    }
}

static void mario_ground_twirl_drag_speed(struct MarioState *m) {
    f32 removedSpeed = m->forwardVel * (1.0f - GROUND_TWIRL_SLIPPERY_SLOPE_DRAG);
    m->forwardVel -= removedSpeed;
    m->slideVelX -= removedSpeed * sins(m->faceAngle[1]);
    m->slideVelZ -= removedSpeed * coss(m->faceAngle[1]);
    m->vel[0] -= removedSpeed * sins(m->faceAngle[1]);
    m->vel[2] -= removedSpeed * coss(m->faceAngle[1]);
}

static void mario_ground_twirl_add_speed(struct MarioState *m, f32 addedSpeed) {
    m->forwardVel += addedSpeed;
    m->slideVelX += addedSpeed * sins(m->faceAngle[1]);
    m->slideVelZ += addedSpeed * coss(m->faceAngle[1]);
    m->vel[0] += addedSpeed * sins(m->faceAngle[1]);
    m->vel[2] += addedSpeed * coss(m->faceAngle[1]);
}

static f32 mario_ground_twirl_boost_for_age(void) {
    // The wall-hit frame is age 1; full bonus arrives at the configured ramp age.
    if (sGroundTwirl.boostAge <= GROUND_TWIRL_WALL_BOOST_RAMP_FRAMES) {
        return sGroundTwirl.bounceBonus * sGroundTwirl.boostAge / GROUND_TWIRL_WALL_BOOST_RAMP_FRAMES;
    }
    return sGroundTwirl.bounceBonus
           * MAX(0, GROUND_TWIRL_WALL_BOOST_RAMP_FRAMES + GROUND_TWIRL_WALL_BOOST_DECAY_FRAMES
                        - sGroundTwirl.boostAge)
           / GROUND_TWIRL_WALL_BOOST_DECAY_FRAMES;
}

static s32 mario_ground_twirl_cancel_frames(void) {
    return sGroundTwirl.hasBounced ? GROUND_TWIRL_WALL_CANCEL_FRAMES : GROUND_TWIRL_CANCEL_FRAMES;
}

static u32 mario_ground_twirl_visual_frames(void) {
    return sGroundTwirl.visualWallBounce ? GROUND_TWIRL_WALL_VISUAL_FRAMES : GROUND_TWIRL_VISUAL_FRAMES;
}

// Apparent rotation follows Mario's steering while completing one turn.
static void mario_ground_twirl_update_visual(struct MarioState *m) {
    // Finish the visible turn before walking can cancel recovery.
    s32 visualFrames = mario_ground_twirl_visual_frames();
    f32 progress = (f32) MIN(m->actionArg + 1, (u32) visualFrames) / visualFrames;
    f32 eased;
    s32 turn;

    // Accumulate steering changes so crossing 180 degrees cannot reverse the visible spin.
    sGroundTwirl.visualTargetOffset += (s16) (m->faceAngle[1] - sGroundTwirl.visualPrevFaceYaw);
    sGroundTwirl.visualPrevFaceYaw = m->faceAngle[1];
    if (sGroundTwirl.visualWallBounce) {
        f32 remaining = 1.0f - progress;

        // Restart at high speed from the current pose, then settle at Mario's facing.
        eased = 1.0f - remaining * remaining * remaining;
        turn = (s32) ((GROUND_TWIRL_WALL_VISUAL_TURNS * 0x10000 + sGroundTwirl.visualTargetOffset)
                      * eased);
        m->marioObj->header.gfx.angle[1] = sGroundTwirl.visualPhaseYaw + turn;
    } else {
        f32 rampEnd = (f32) GROUND_TWIRL_VISUAL_RAMP_FRAMES / GROUND_TWIRL_VISUAL_FRAMES;
        f32 rampTurn = 3.0f * rampEnd / (2.0f + rampEnd);

        // Accelerate briefly, then spend the remaining frames easing to Mario's facing.
        if (progress <= rampEnd) {
            f32 rampProgress = progress / rampEnd;
            eased = rampTurn * rampProgress * rampProgress;
        } else {
            f32 remaining = (1.0f - progress) / (1.0f - rampEnd);
            eased = 1.0f - (1.0f - rampTurn) * remaining * remaining * remaining;
        }
        turn = (s32) ((GROUND_TWIRL_VISUAL_TURNS * 0x10000 + sGroundTwirl.visualTargetOffset) * eased);
        m->marioObj->header.gfx.angle[1] = sGroundTwirl.visualStartYaw + turn;
    }
    if (m->actionArg < GROUND_TWIRL_FRAMES) {
        m->actionArg++;
    }
}

static void mario_ground_twirl_prepare_walk_animation(struct MarioState *m) {
    f32 speed = MAX(4.0f, MAX(m->intendedMag, m->forwardVel));
    s32 animSpeed = (s32) (speed / 4.0f * 0x10000);
    s32 animation;

    if (m->quicksandDepth > 50.0f) {
        animation = MARIO_ANIM_MOVE_IN_QUICKSAND;
    } else if (speed <= 8.0f) {
        animation = MARIO_ANIM_START_TIPTOE;
    } else if (speed <= 22.0f) {
        animation = MARIO_ANIM_WALKING;
    } else {
        animation = MARIO_ANIM_RUNNING;
    }
    set_mario_anim_with_accel(m, animation, MAX(animSpeed, 0x1000));
}

// Action state and recovery.
s32 mario_ground_twirl_is_held_pose(struct MarioState *m) {
    return game_options_get(GO_TWIRLS) && game_options_get(GO_HELD_TWIRL)
           && m->action == ACT_GROUND_TWIRL && m->actionTimer >= GROUND_TWIRL_FRAMES
           && m->actionState == 0 && sGroundTwirl.lHeldThroughout && !(m->input & INPUT_NONZERO_ANALOG);
}

s32 mario_ground_twirl_loses_cap_on_damage(struct MarioState *m) {
    return mario_ground_twirl_is_held_pose(m) && (m->controller->buttonDown & L_TRIG)
           && sGroundTwirl.heldPoseFrames >= GROUND_TWIRL_HAT_LOSS_FRAMES;
}

s32 mario_ground_twirl_is_active(struct MarioState *m) {
    return game_options_get(GO_TWIRLS) && m->action == ACT_GROUND_TWIRL
           && (m->actionTimer < GROUND_TWIRL_ACTIVE_FRAMES
               || m->actionState > GROUND_TWIRL_WALL_SLOW_TURN_FRAMES - GROUND_TWIRL_ACTIVE_FRAMES + 1);
}

static void mario_ground_twirl_update_speed(struct MarioState *m, s32 slowWallSteering) {
    s32 slipperySlope = mario_floor_is_slippery(m) && mario_floor_is_slope(m);
    f32 previousSpeed = m->forwardVel;
    f32 bounceBonus = 0.0f;
    f32 walkingGain = 0.0f;

    if (sGroundTwirl.boostAge > 0
        && sGroundTwirl.boostAge
               < GROUND_TWIRL_WALL_BOOST_RAMP_FRAMES + GROUND_TWIRL_WALL_BOOST_DECAY_FRAMES) {
        mario_set_forward_vel(m, MAX(0.0f, m->forwardVel - sGroundTwirl.appliedBonus));
        sGroundTwirl.boostAge++;
        bounceBonus = mario_ground_twirl_boost_for_age();
        sGroundTwirl.appliedBonus = bounceBonus;
    }
    if (m->input & INPUT_NONZERO_ANALOG) {
        f32 minimumSpeed = 0.0f;
        s16 intendedYaw = m->intendedYaw;
        if (!slipperySlope) {
            f32 heldSpeed =
                sGroundTwirl.earlySpeedCapActive ? sGroundTwirl.earlySpeedCap : sGroundTwirl.startSpeed;
            f32 desiredSpeed;
            if (sGroundTwirl.hasBounced && !game_options_cheat_active(GO_NO_SPEED_LIMIT)) {
                heldSpeed = MIN(heldSpeed, GROUND_TWIRL_POST_BOUNCE_SPEED_LIMIT);
            }
            desiredSpeed = heldSpeed * MIN(m->intendedMag, 32.0f) / 32.0f;
            // Preserve the current rebound, but rebuild lost speed gradually after easing the stick.
            minimumSpeed =
                MAX(0.0f, MIN(desiredSpeed, previousSpeed + GROUND_TWIRL_SPEED_RECOVERY) - bounceBonus);
        }
        if (m->actionTimer == 0) {
            m->intendedYaw = m->faceAngle[1];
        } else if (slowWallSteering) {
            m->intendedYaw =
                approach_s16_symmetric(m->faceAngle[1], intendedYaw, GROUND_TWIRL_WALL_TURN_STEP);
        }
        f32 maximumSpeed = sGroundTwirl.earlySpeedCapActive
                               ? MAX(0.0f, sGroundTwirl.earlySpeedCap - bounceBonus)
                               : MAX(48.0f, minimumSpeed);
        f32 speedBeforeWalking = m->forwardVel;
        update_walking_speed_with_minimum(m, minimumSpeed, maximumSpeed);
        walkingGain = MAX(0.0f, m->forwardVel - speedBeforeWalking);
        m->intendedYaw = intendedYaw;
    } else {
        update_decelerating_speed(m);
    }
    if (slipperySlope) {
        mario_ground_twirl_drag_speed(m);
    }
    if (sGroundTwirl.earlySpeedCapActive && !game_options_cheat_active(GO_NO_SPEED_LIMIT)) {
        mario_ground_twirl_limit_speed(m, MAX(0.0f, sGroundTwirl.earlySpeedCap - bounceBonus));
    }
    if (bounceBonus > 0.0f) {
        // Keep the transient rebound speed separate from ordinary ground acceleration.
        mario_ground_twirl_add_speed(m, bounceBonus);
    }
    if (sGroundTwirl.hasBounced && !game_options_cheat_active(GO_NO_SPEED_LIMIT)) {
        mario_ground_twirl_limit_speed(m, GROUND_TWIRL_POST_BOUNCE_SPEED_LIMIT);
    }
    if (sGroundTwirl.wallBounceCount > 1) {
        f32 extraDrag = (sGroundTwirl.wallBounceCount - 1) * GROUND_TWIRL_WALL_BOUNCE_EXTRA_DRAG;
        mario_ground_twirl_limit_speed(m, MAX(0.0f, m->forwardVel - extraDrag));
    }
    if (sGroundTwirl.hasBounced && walkingGain > 0.0f && !mario_floor_is_slope(m)
        && (m->floor->type != SURFACE_SLOW || game_options_cheat_active(GO_IGNORE_SAND))) {
        f32 targetSpeed = GROUND_TWIRL_TARGET_SPEED * MIN(m->intendedMag, 32.0f) / 32.0f;
        f32 minimumSpeed = MIN(targetSpeed, previousSpeed + walkingGain);

        // The fading wall bonus and repeat-bounce drag must not cancel stick acceleration below target.
        if (m->forwardVel < minimumSpeed) {
            mario_ground_twirl_add_speed(m, minimumSpeed - m->forwardVel);
        }
    }
}

static s32 mario_ground_twirl_handle_wall(struct MarioState *m, s32 slowWallSteering) {
    f32 bounceSpeed;
    f32 retainedSpeed;
    s32 movingIntoWall;
    s32 reflectImpact;
    s32 wallDYaw;

    if (m->wall == NULL) {
        sGroundTwirl.wallContact = FALSE;
        sGroundTwirl.bounceBonus = 0.0f;
        sGroundTwirl.appliedBonus = 0.0f;
        sGroundTwirl.boostAge = 0;
        mario_set_forward_vel(m, 0.0f);
        return FALSE;
    }
    movingIntoWall = m->vel[0] * m->wall->normal.x + m->vel[2] * m->wall->normal.z < 0.0f;
    wallDYaw = abs_angle_diff(m->wallYaw, m->faceAngle[1]);
    reflectImpact = movingIntoWall && !(wallDYaw >= DEGREES(60) && wallDYaw <= DEGREES(120));
    if (!reflectImpact && sGroundTwirl.wallContact && sGroundTwirl.wallContactYaw == m->wallYaw) {
        return FALSE;
    }
    sGroundTwirl.wallContact = TRUE;
    sGroundTwirl.wallContactYaw = m->wallYaw;

    bounceSpeed = absf(m->forwardVel);
    if (sGroundTwirl.wallBounceCount < GROUND_TWIRL_WALL_BOUNCE_ZERO_SPEED_COUNT) {
        sGroundTwirl.wallBounceCount++;
    }
    retainedSpeed =
        sGroundTwirl.wallBounceCount == GROUND_TWIRL_WALL_BOUNCE_ZERO_SPEED_COUNT
            ? 0.0f
            : 1.0f - (sGroundTwirl.wallBounceCount - 1) * GROUND_TWIRL_WALL_BOUNCE_SPEED_LOSS;
    if (reflectImpact) {
        if (m->forwardVel < 0.0f) {
            m->faceAngle[1] += 0x8000;
        }
        mario_bonk_reflection(m, FALSE);
    } else {
        mario_play_wall_bonk_sound(m);
    }
    m->particleFlags |= PARTICLE_VERTICAL_STAR;
    player_stats_wall_bonk(m);
    player_stats_twirl_wall_rebound(FALSE);
    queue_rumble_data(5, 40);
    // A quick repeat bounce keeps the current bonus fading without granting a new one.
    if (!slowWallSteering && bounceSpeed > 0.0f) {
        s32 earlyBounce = !sGroundTwirl.hasBounced && m->actionTimer < GROUND_TWIRL_EARLY_BOUNCE_FRAMES;
        sGroundTwirl.bounceBonus = bounceSpeed * (GROUND_TWIRL_WALL_BOOST_MULTIPLIER - 1.0f);
        if (earlyBounce) {
            sGroundTwirl.bounceBonus +=
                GROUND_TWIRL_EARLY_BOUNCE_SPEED * GROUND_TWIRL_EARLY_BOUNCE_SPEED_MULTIPLIER;
        }
        if (earlyBounce || sGroundTwirl.earlySpeedCapActive) {
            f32 reboundLimit = (bounceSpeed + sGroundTwirl.bounceBonus) * retainedSpeed;
            if (!game_options_cheat_active(GO_NO_SPEED_LIMIT)) {
                reboundLimit = MIN(reboundLimit, GROUND_TWIRL_POST_BOUNCE_SPEED_LIMIT);
            }
            sGroundTwirl.earlySpeedCap =
                MAX(MAX(GROUND_TWIRL_TARGET_SPEED, sGroundTwirl.earlySpeedCap), reboundLimit);
            sGroundTwirl.earlySpeedCapActive = TRUE;
        }
        sGroundTwirl.boostAge = 1;
        sGroundTwirl.appliedBonus = mario_ground_twirl_boost_for_age();
        bounceSpeed += sGroundTwirl.appliedBonus;
    }
    // Reduce the whole rebound, including its fading wall bonus, to the shared ceiling.
    if (bounceSpeed > 0.0f && !game_options_cheat_active(GO_NO_SPEED_LIMIT)) {
        retainedSpeed = MIN(retainedSpeed, GROUND_TWIRL_POST_BOUNCE_SPEED_LIMIT / bounceSpeed);
    }
    bounceSpeed *= retainedSpeed;
    if (!game_options_cheat_active(GO_NO_SPEED_LIMIT)) {
        bounceSpeed = MIN(bounceSpeed, GROUND_TWIRL_POST_BOUNCE_SPEED_LIMIT);
    }
    sGroundTwirl.bounceBonus *= retainedSpeed;
    sGroundTwirl.appliedBonus *= retainedSpeed;
    sGroundTwirl.hasBounced = TRUE;
    mario_set_forward_vel(m, bounceSpeed);
    m->actionState = GROUND_TWIRL_WALL_SLOW_TURN_FRAMES;
    sGroundTwirl.bufferedInput = 0;
    sGroundTwirl.bufferedInputFrames = 0;
    sGroundTwirl.visualPhaseYaw = m->marioObj->header.gfx.angle[1];
    sGroundTwirl.visualPrevFaceYaw = m->faceAngle[1];
    sGroundTwirl.visualTargetOffset = (s16) (m->faceAngle[1] - sGroundTwirl.visualPhaseYaw);
    sGroundTwirl.visualWallBounce = TRUE;
    m->actionArg = 0;
    return TRUE;
}

s32 mario_ground_twirl_action(struct MarioState *m) {
    s32 step;
    s32 attackActive = mario_ground_twirl_is_active(m);
    s32 slowWallSteering = m->actionState > 0;
    s32 walkCancelFrames =
        sGroundTwirl.hasBounced ? GROUND_TWIRL_WALL_WALK_CANCEL_FRAMES : GROUND_TWIRL_CANCEL_FRAMES;
    u32 pressed;

    if (!game_options_get(GO_HELD_TWIRL) || !(m->controller->buttonDown & L_TRIG)) {
        sGroundTwirl.lHeldThroughout = FALSE;
    }
    if (m->actionTimer == 0) {
        play_sound(SOUND_ACTION_TWIRL, m->marioObj->header.gfx.cameraToObject);
    }
    pressed = m->input & (INPUT_A_PRESSED | INPUT_B_PRESSED | INPUT_Z_PRESSED);
    if (pressed != 0) {
        // The latest press wins and remains valid for six gameplay updates.
        sGroundTwirl.bufferedInput = pressed;
        sGroundTwirl.bufferedInputFrames = 6;
    }
    // L may have started this spin before the next-frame A completes the air-twirl chord.
    s32 jumpTwirlChord = m->airTwirlJumpQueued && m->actionTimer == 1;
    if (jumpTwirlChord
        || (m->actionTimer >= GROUND_TWIRL_FRAMES - mario_ground_twirl_cancel_frames()
            && m->actionState <= mario_ground_twirl_cancel_frames())) {
        // Released presses remain valid until the six-update buffer expires.
        m->input |= sGroundTwirl.bufferedInput;
        if (m->input & INPUT_A_PRESSED) {
            mario_ground_twirl_finish();
#ifdef EASIER_LONG_JUMPS
            if ((m->input & INPUT_Z_PRESSED) && m->forwardVel > 10.0f) {
                return set_mario_action(m, ACT_CROUCH_SLIDE, 0);
            }
#endif
            return set_jump_from_landing(m);
        }
        if (m->input & INPUT_B_PRESSED) {
            mario_ground_twirl_finish();
            return check_ground_dive_or_punch(m);
        }
        if (m->input & INPUT_Z_PRESSED) {
            mario_ground_twirl_finish();
            return set_mario_action(m, ACT_CROUCH_SLIDE, 0);
        }
        if ((m->input & INPUT_NONZERO_ANALOG)
            && m->actionTimer >= GROUND_TWIRL_FRAMES - walkCancelFrames
            && m->actionState <= walkCancelFrames) {
            f32 exitSpeed = m->forwardVel;
            mario_ground_twirl_finish();
            set_mario_action(m, ACT_WALKING, 0);
            mario_set_forward_vel(m, exitSpeed);
            return TRUE; // Run walking this frame; the twirl has not taken a movement step.
        }
    }
    if (m->actionState > 0) {
        m->actionState--;
    }
    mario_ground_twirl_update_speed(m, slowWallSteering);
    step = perform_ground_step(m);
    if (m->wall == NULL) {
        sGroundTwirl.wallContact = FALSE;
    }
    if (step == GROUND_STEP_LEFT_GROUND) {
        if (m->controller->buttonDown & L_TRIG) {
            mario_start_air_twirl(m, TRUE);
            return FALSE; // The ground step already moved Mario this update.
        }
        set_mario_action(m, ACT_FREEFALL, 0);
        return FALSE; // The ground step already moved Mario this update.
    }
    if (step == GROUND_STEP_HIT_WALL && mario_ground_twirl_handle_wall(m, slowWallSteering)) {
        attackActive = TRUE;
    }
    // A wall bounce starts another hit window without re-hitting enemies from this twirl.
    if (attackActive) {
        mario_twirl_push_enemies(m);
        if (m->action != ACT_GROUND_TWIRL) {
            // The twirl already spent this update's movement before the recoil.
            return FALSE;
        }
    }
    mario_ground_twirl_update_visual(m);
    if (m->actionTimer < GROUND_TWIRL_FRAMES) {
        m->actionTimer++;
    }
    if (mario_ground_twirl_is_held_pose(m)) {
        // Keep the old final-frame T pose while L is held with neutral stick.
        set_mario_animation(m, MARIO_ANIM_TWIRL);
        set_anim_to_frame(m, m->marioObj->header.gfx.animInfo.curAnim->loopEnd - 1);
        sGroundTwirl.heldPoseFrames++;
        if (sGroundTwirl.heldPoseFrames > ((m->flags & MARIO_CAP_ON_HEAD)
                                               ? GROUND_TWIRL_SLEEP_WITH_CAP_FRAMES
                                               : GROUND_TWIRL_SLEEP_CAPLESS_FRAMES)) {
            mario_ground_twirl_finish();
            mario_set_forward_vel(m, 0.0f);
            return set_mario_action(m, ACT_START_SLEEPING, 0);
        }
        return FALSE;
    }
    sGroundTwirl.heldPoseFrames = 0;
    // Start the matching walk animation on the frame before recovery can
    // cancel into walking. This changes only the pose, not the twirl timer.
    if ((m->input & INPUT_NONZERO_ANALOG) && sGroundTwirl.bufferedInput == 0
        && m->actionTimer >= GROUND_TWIRL_FRAMES - walkCancelFrames
        && m->actionState <= walkCancelFrames) {
        mario_ground_twirl_prepare_walk_animation(m);
    } else {
        // Stay in the twirl pose when recovery will lead into the held T-pose.
        s32 keepTwirlPose = m->actionArg < mario_ground_twirl_visual_frames()
                            || (sGroundTwirl.lHeldThroughout && !(m->input & INPUT_NONZERO_ANALOG));
        set_mario_animation(m, keepTwirlPose ? MARIO_ANIM_TWIRL : MARIO_ANIM_TWIRL_LAND);
    }
    // A wall bounce keeps the twirl active through every reduced-steering frame.
    if (m->actionTimer >= GROUND_TWIRL_FRAMES && m->actionState == 0) {
        f32 exitSpeed = m->forwardVel;
        mario_ground_twirl_finish();
        // Preserve the twirl's speed across the action change.
        set_mario_action(m, (m->input & INPUT_NONZERO_ANALOG) ? ACT_WALKING : ACT_DECELERATING,
                         (u32) MAX(48.0f, exitSpeed + 1.0f));
        mario_set_forward_vel(m, exitSpeed);
        return FALSE; // The next action starts on the next frame, without a second movement step.
    }
    return FALSE;
}

void mario_ground_twirl_init(void) {
    // The contact generation ID deliberately survives Mario resets.
    sGroundTwirl.cooldown = 0;
    sGroundTwirl.cooldownResetByContact = FALSE;
    sGroundTwirl.inputQueued = FALSE;
    sGroundTwirl.bufferedInput = 0;
    sGroundTwirl.bufferedInputFrames = 0;
    sGroundTwirl.startSpeed = 0.0f;
    sGroundTwirl.visualStartYaw = 0;
    sGroundTwirl.visualPhaseYaw = 0;
    sGroundTwirl.visualPrevFaceYaw = 0;
    sGroundTwirl.visualTargetOffset = 0;
    sGroundTwirl.visualWallBounce = FALSE;
    mario_ground_twirl_reset_rebound_state();
    sGroundTwirl.lHeldThroughout = FALSE;
    sGroundTwirl.heldPoseFrames = 0;
}
