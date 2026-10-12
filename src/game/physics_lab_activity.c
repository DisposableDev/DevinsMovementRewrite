#include <ultra64.h>
#include "sm64.h"
#include "area.h"
#include "behavior_data.h"
#include "engine/math_util.h"
#include "engine/surface_collision.h"
#include "engine/graph_node.h"
#include "interaction.h"
#include "level_table.h"
#include "level_update.h"
#include "mario.h"
#include "memory.h"
#include "object_constants.h"
#include "object_helpers.h"
#include "object_list_processor.h"
#include "object_platform_motion.h"
#include "physics_lab.h"

// The conservative policy remains available for comparison; Lab defaults to
// aggressive distance sleeping, including visible and active objects.
#ifndef PHYSICS_LAB_AGGRESSIVE_SLEEP
#define PHYSICS_LAB_AGGRESSIVE_SLEEP 1
#endif
#ifndef SKEETER_WATER_SLEEP_TRIAL
#define SKEETER_WATER_SLEEP_TRIAL 0
#endif
#ifndef ENEMY_PATROL_SLEEP_TRIAL
#define ENEMY_PATROL_SLEEP_TRIAL 0
#endif
// Runtime toggles exist only in experiment builds, allowing matched code addresses.
#if SKEETER_WATER_SLEEP_TRIAL
volatile s32 gSkeeterWaterSleepTrial = SKEETER_WATER_SLEEP_TRIAL;
#else
#define gSkeeterWaterSleepTrial 0
#endif
#if ENEMY_PATROL_SLEEP_TRIAL
volatile s32 gEnemyPatrolSleepTrial = ENEMY_PATROL_SLEEP_TRIAL;
#else
#define gEnemyPatrolSleepTrial 0
#endif
#define LAB_SLEEP_WAKE_DISTANCE 2500.0f
#define LAB_ENEMY_VISIBILITY_HORIZONTAL_MARGIN 300.0f
#define LAB_ENEMY_VISIBILITY_VERTICAL_MARGIN 150.0f
#define CHOMP_VISIBILITY_WAKE_DISTANCE 3500.0f
#define CHOMP_VISIBILITY_EXIT_MARGIN 1200.0f
#define LAB_EFFECT_DISCARD_DISTANCE 4000.0f
#define ENEMY_SLEEP_MIN_DISTANCE 2000.0f
#define ENEMY_SLEEP_SETTLE_FRAMES 30
#define VANILLA_SLEEP_HORIZONTAL_DISTANCE 2500.0f
#define ENEMY_SLEEP_VERTICAL_DISTANCE 1000.0f

enum EnemySleepFlags {
    ENEMY_SLEEP_SETTLED = (1 << 0),
    ENEMY_SLEEP_AQUATIC = (1 << 1),
    ENEMY_SLEEP_AIRBORNE = (1 << 2),
    ENEMY_SLEEP_SURFACE = (1 << 3),
};

struct EnemySleepRule {
    const BehaviorScript *behavior;
    f32 wakeDistance;
    f32 viewRadius;
    u8 flags;
};

// Enrollment and static safety bounds live together. Native action gates remain
// explicit below; a listed attachment can sleep only through its validated owner.
static const struct EnemySleepRule sEnemySleepRules[] = {
    { bhvGoomba, ENEMY_SLEEP_MIN_DISTANCE, 600.0f, 0 },
    { bhvSmallBully, ENEMY_SLEEP_MIN_DISTANCE, 600.0f, 0 },
    { bhvSmallChillBully, ENEMY_SLEEP_MIN_DISTANCE, 600.0f, 0 },
    { bhvSpindrift, ENEMY_SLEEP_MIN_DISTANCE, 600.0f, 0 },
    { bhvSpiny, ENEMY_SLEEP_MIN_DISTANCE, 600.0f, 0 },
    { bhvPiranhaPlant, ENEMY_SLEEP_MIN_DISTANCE, 600.0f, 0 },
    { bhvSwoop, ENEMY_SLEEP_MIN_DISTANCE, 600.0f, 0 },
    { bhvSnufit, ENEMY_SLEEP_MIN_DISTANCE, 600.0f, 0 },
    { bhvFlyGuy, ENEMY_SLEEP_MIN_DISTANCE, 600.0f, 0 },
    { bhvHomingAmp, ENEMY_SLEEP_MIN_DISTANCE, 600.0f, 0 },
    { bhvCirclingAmp, ENEMY_SLEEP_MIN_DISTANCE, 600.0f, 0 },
    { bhvKoopa, ENEMY_SLEEP_MIN_DISTANCE, 600.0f, 0 },
    { bhvBobomb, ENEMY_SLEEP_MIN_DISTANCE, 600.0f, 0 },
    { bhvScuttlebug, ENEMY_SLEEP_MIN_DISTANCE, 600.0f, 0 },
    { bhvFirePiranhaPlant, ENEMY_SLEEP_MIN_DISTANCE, 600.0f, 0 },
    { bhvMoneybagHidden, ENEMY_SLEEP_MIN_DISTANCE, 600.0f, 0 },
    { bhvMadPiano, ENEMY_SLEEP_MIN_DISTANCE, 600.0f, ENEMY_SLEEP_SETTLED },
    { bhvBulletBill, ENEMY_SLEEP_MIN_DISTANCE, 600.0f, ENEMY_SLEEP_SETTLED },
    { bhvEnemyLakitu, 3000.0f, 1000.0f, ENEMY_SLEEP_SETTLED | ENEMY_SLEEP_AIRBORNE },
    { bhvHauntedChair, ENEMY_SLEEP_MIN_DISTANCE, 600.0f, ENEMY_SLEEP_SETTLED },
    { bhvBoo, ENEMY_SLEEP_MIN_DISTANCE, 600.0f, ENEMY_SLEEP_SETTLED },
    { bhvClamShell, ENEMY_SLEEP_MIN_DISTANCE, 600.0f, ENEMY_SLEEP_SETTLED | ENEMY_SLEEP_AQUATIC },
    { bhvTweester, ENEMY_SLEEP_MIN_DISTANCE, 600.0f, ENEMY_SLEEP_SETTLED },
    { bhvCloud, ENEMY_SLEEP_MIN_DISTANCE, 600.0f, ENEMY_SLEEP_SETTLED },
    { bhvFlamethrower, ENEMY_SLEEP_MIN_DISTANCE, 600.0f, ENEMY_SLEEP_SETTLED },
    { bhvWaterBombSpawner, ENEMY_SLEEP_MIN_DISTANCE, 600.0f, ENEMY_SLEEP_SETTLED },
    { bhvBookendSpawn, ENEMY_SLEEP_MIN_DISTANCE, 600.0f, ENEMY_SLEEP_SETTLED },
    { bhvBubSpawner, ENEMY_SLEEP_MIN_DISTANCE, 600.0f, ENEMY_SLEEP_SETTLED | ENEMY_SLEEP_AQUATIC },
    { bhvChuckya, ENEMY_SLEEP_MIN_DISTANCE, 1000.0f, ENEMY_SLEEP_SETTLED },
    { bhvHeaveHo, 3500.0f, 1000.0f, ENEMY_SLEEP_SETTLED },
    { bhvMrI, ENEMY_SLEEP_MIN_DISTANCE, 1000.0f, ENEMY_SLEEP_SETTLED },
    { bhvMrBlizzard, ENEMY_SLEEP_MIN_DISTANCE, 1000.0f, ENEMY_SLEEP_SETTLED },
    { bhvPokey, 3000.0f, 1400.0f, ENEMY_SLEEP_SETTLED },
    { bhvChainChomp, 3500.0f, 1600.0f, ENEMY_SLEEP_SETTLED | ENEMY_SLEEP_AIRBORNE },
    { bhvMontyMole, ENEMY_SLEEP_MIN_DISTANCE, 600.0f, ENEMY_SLEEP_SETTLED },
    { bhvBubba, ENEMY_SLEEP_MIN_DISTANCE, 600.0f,
      ENEMY_SLEEP_SETTLED | ENEMY_SLEEP_AQUATIC | ENEMY_SLEEP_AIRBORNE },
    { bhvSkeeter, ENEMY_SLEEP_MIN_DISTANCE, 600.0f, ENEMY_SLEEP_SETTLED | ENEMY_SLEEP_AQUATIC },
    { bhvUkiki, ENEMY_SLEEP_MIN_DISTANCE, 600.0f, ENEMY_SLEEP_SETTLED },
    { bhvSmallWhomp, 3500.0f, 1600.0f, ENEMY_SLEEP_SETTLED | ENEMY_SLEEP_SURFACE },
    { bhvThwomp, 3500.0f, 1600.0f, ENEMY_SLEEP_SETTLED | ENEMY_SLEEP_SURFACE },
    { bhvGrindel, 3500.0f, 1600.0f, ENEMY_SLEEP_SETTLED | ENEMY_SLEEP_SURFACE },
    { bhvSpindel, 3500.0f, 1600.0f, ENEMY_SLEEP_SETTLED | ENEMY_SLEEP_SURFACE },
    { bhvToxBox, 3500.0f, 1600.0f, ENEMY_SLEEP_SETTLED | ENEMY_SLEEP_SURFACE },
    { bhvSushiShark, 3500.0f, 1200.0f,
      ENEMY_SLEEP_SETTLED | ENEMY_SLEEP_AQUATIC | ENEMY_SLEEP_AIRBORNE },
    { bhvUnagi, 5000.0f, 2000.0f, ENEMY_SLEEP_SETTLED | ENEMY_SLEEP_AQUATIC | ENEMY_SLEEP_AIRBORNE },
    { bhvKlepto, 3000.0f, 1200.0f, ENEMY_SLEEP_SETTLED | ENEMY_SLEEP_AIRBORNE },
    { bhvBigBoulder, 3500.0f, 600.0f, ENEMY_SLEEP_SETTLED },
    { bhvBouncingFireball, ENEMY_SLEEP_MIN_DISTANCE, 600.0f, ENEMY_SLEEP_SETTLED },
    { bhvBub, ENEMY_SLEEP_MIN_DISTANCE, 600.0f,
      ENEMY_SLEEP_SETTLED | ENEMY_SLEEP_AQUATIC | ENEMY_SLEEP_AIRBORNE },
    { bhvMrIIris, ENEMY_SLEEP_MIN_DISTANCE, 600.0f, ENEMY_SLEEP_SETTLED },
    { bhvChuckyaAnchorMario, ENEMY_SLEEP_MIN_DISTANCE, 600.0f, ENEMY_SLEEP_SETTLED },
    { bhvHeaveHoThrowMario, ENEMY_SLEEP_MIN_DISTANCE, 600.0f, ENEMY_SLEEP_SETTLED },
    { bhvMrBlizzardSnowball, ENEMY_SLEEP_MIN_DISTANCE, 600.0f, ENEMY_SLEEP_SETTLED },
    { bhvPokeyBodyPart, ENEMY_SLEEP_MIN_DISTANCE, 600.0f, ENEMY_SLEEP_SETTLED },
    { bhvChainChompChainPart, ENEMY_SLEEP_MIN_DISTANCE, 600.0f, ENEMY_SLEEP_SETTLED },
    { bhvCloudPart, ENEMY_SLEEP_MIN_DISTANCE, 600.0f, ENEMY_SLEEP_SETTLED },
    { bhvCoinInsideBoo, ENEMY_SLEEP_MIN_DISTANCE, 600.0f, ENEMY_SLEEP_SETTLED },
#ifdef FLOOMBAS
    { bhvFloomba, ENEMY_SLEEP_MIN_DISTANCE, 600.0f, 0 },
#endif
};

static const BehaviorScript *sLabAdditionalSleepBehaviors[] = {
    bhvPhysicsLabMover,
    bhvPhysicsLabTiltingPlatform,
    bhvCheckerboardElevatorGroup,
    bhvCheckerboardPlatformSub,
    bhvBreakableBoxSmall,
    bhvPushableMetalBox,
    bhvKoopaShell,
};

// Supplies and ambient actors use their own state gates in every level.
static const BehaviorScript *sEntitySleepBehaviors[] = {
    bhvBreakableBoxSmall, bhvPushableMetalBox,       bhvKoopaShell,
    bhvBobombBuddy,       bhvBobombBuddyOpensCannon, bhvFish,
};

// Temporary objects expire instead of sleeping with a frozen lifetime.
static const BehaviorScript *sLabDisposableBehaviors[] = {
    bhvSnufitBalls,
    bhvMrIParticle,
    bhvMontyMoleRock,
    bhvFlamethrowerFlame,
    bhvBouncingFireballFlame,
    bhvMovingFlame,
    bhvMovingFlameEmber,
    bhvPurpleParticle,
    bhvWhitePuffSmoke,
    bhvWhitePuffSmoke2,
    bhvSmoke,
    bhvBlackSmokeUpward,
    bhvBobombFuseSmoke,
    bhvBobombBullyDeathSmoke,
    bhvBobombExplosionBubble,
    bhvBobombExplosionBubble3600,
    bhvObjectBubble,
    bhvSmallParticle,
    bhvSmallParticleSnow,
    bhvSmallParticleBubbles,
    bhvWaterAirBubble,
    bhvBubbleMaybe,
    bhvPlungeBubble,
    bhvBubbleSplash,
    bhvPiranhaPlantBubble,
    bhvTweesterSandParticle,
    bhvCoinSparkles,
    bhvSkeeterWave,
    bhvWaterBombShadow,
};

// Audited enemies and idle supplies enroll in every level; movers remain Lab-only.
// Projectiles may be listed solely for their validated attached state.
// Re-enroll at script entry, clearing previous enrollment and display history.
void enemy_sleep_init(struct Object *obj) {
    obj->oFlags &=
        ~(OBJ_FLAG_ALLOW_ENEMY_SLEEP | OBJ_FLAG_HIDE_WHEN_SLEEPING | OBJ_FLAG_LAB_ENEMY_VISIBLE);
    for (u32 i = 0; i < ARRAY_COUNT(sEntitySleepBehaviors); i++) {
        if (obj->behavior == segmented_to_virtual(sEntitySleepBehaviors[i])) {
            obj->oFlags |= OBJ_FLAG_ALLOW_ENEMY_SLEEP | OBJ_FLAG_COMPUTE_DIST_TO_MARIO;
            return;
        }
    }
    if (gCurrLevelNum == LEVEL_PHYSICS_LAB && PHYSICS_LAB_AGGRESSIVE_SLEEP) {
        for (u32 i = 0; i < ARRAY_COUNT(sLabAdditionalSleepBehaviors); i++) {
            if (obj->behavior == segmented_to_virtual(sLabAdditionalSleepBehaviors[i])) {
                obj->oFlags |= OBJ_FLAG_ALLOW_ENEMY_SLEEP | OBJ_FLAG_COMPUTE_DIST_TO_MARIO;
                return;
            }
        }
    }
    for (u32 i = 0; i < ARRAY_COUNT(sEnemySleepRules); i++) {
        if (obj->behavior == segmented_to_virtual(sEnemySleepRules[i].behavior)) {
            obj->oFlags |= OBJ_FLAG_ALLOW_ENEMY_SLEEP | OBJ_FLAG_COMPUTE_DIST_TO_MARIO
                           | OBJ_FLAG_HIDE_WHEN_SLEEPING;
            return;
        }
    }
}

struct EnemySleepPolicy {
    f32 wakeDistance; // Current/home distance threshold, in world units.
    f32 viewRadius;   // Conservative camera-bound half extent, in world units.
    s32 settled;      // Native state/pose replaces the generic settling timer.
    s32 aquatic;      // Permit water flags and horizontal-only Lab distance.
    s32 surface;      // Permit collision providers and expand camera bounds.
    s32 airborne;     // Permit positive Y velocity after native state checks.
};

static s32 enemy_sleep_has_behavior(struct Object *obj, const BehaviorScript *behavior) {
    return obj != NULL && obj->behavior == segmented_to_virtual(behavior);
}

// These objects have no independent AI while attached. Validate ownership before
// following it; Chain Chomp's pivot points back to the chomp, so roots never recurse.
static struct Object *enemy_sleep_parent(struct Object *obj) {
    struct Object *parent = obj->parentObj;
    if (parent == NULL || parent == obj) {
        return NULL;
    }
    const BehaviorScript *behavior = obj->behavior;
    if ((behavior == segmented_to_virtual(bhvMrIIris) && enemy_sleep_has_behavior(parent, bhvMrI))
        || (behavior == segmented_to_virtual(bhvChuckyaAnchorMario)
            && enemy_sleep_has_behavior(parent, bhvChuckya))
        || (behavior == segmented_to_virtual(bhvHeaveHoThrowMario)
            && enemy_sleep_has_behavior(parent, bhvHeaveHo))
        || (behavior == segmented_to_virtual(bhvCoinInsideBoo)
            && obj->oAction == COIN_INSIDE_BOO_ACT_CARRIED && enemy_sleep_has_behavior(parent, bhvBoo))
        || (behavior == segmented_to_virtual(bhvPokeyBodyPart) && obj->oAction == 0
            && enemy_sleep_has_behavior(parent, bhvPokey))
        || (behavior == segmented_to_virtual(bhvChainChompChainPart)
            && enemy_sleep_has_behavior(parent, bhvChainChomp))
        || (behavior == segmented_to_virtual(bhvCloud)
            && enemy_sleep_has_behavior(parent, bhvEnemyLakitu))
        || (behavior == segmented_to_virtual(bhvCloudPart)
            && enemy_sleep_has_behavior(parent, bhvCloud))
        || (behavior == segmented_to_virtual(bhvMrBlizzardSnowball)
            && obj->oAction == MR_BLIZZARD_SNOWBALL_ACT_LAUNCH
            && enemy_sleep_has_behavior(parent, bhvMrBlizzard) && parent->prevObj == obj
            && parent->oMrBlizzardHeldObj == obj)) {
        return parent;
    }
    return NULL;
}

// Keep initialization, pending interactions and live movement dependencies awake.
static s32 enemy_sleep_pending(struct Object *obj) {
    return obj->activeFlags == ACTIVE_FLAG_DEACTIVATED || obj->curBhvCommand == obj->behavior
           || obj->oAction != obj->oPrevAction || obj->oHeldState != HELD_FREE
           || obj->oInteractStatus != INT_STATUS_NONE || obj->numCollidedObjs != 0
           || obj->metalContactCooldown != 0 || obj->metalContactPendingDefeat
           || obj->oGroundTwirlStunTimer != 0 || obj->oGroundTwirlContactRebound != 0
           || obj->oGroundTwirlStunDuration != 0 || obj->platform != NULL
           || (obj->oFloor != NULL && obj->oFloor->object != NULL)
           || object_platform_motion_is_active(obj);
}

static s32 enemy_sleep_outside_view(struct Object *obj, f32 radius) {
    // Expanded group bounds must cover visible children even when the root's
    // own drawing-range flag is clear (notably invisible Pokey parents).
    // Sleep-hidden enemies still need the conservative camera-turn wake check.
    if (radius <= 600.0f && !(obj->header.gfx.node.flags & GRAPH_RENDER_ACTIVE)
        && !(obj->oFlags & OBJ_FLAG_HIDE_WHEN_SLEEPING)) {
        return TRUE;
    }
    struct PhysicsLabRenderSection bounds;
    for (s32 a = 0; a < 3; a++) {
        f32 position = (&obj->oPosVec)[a];
        if (!(position >= -30000.0f && position <= 30000.0f)
            || !(position - radius >= -32768.0f && position + radius <= 32767.0f)) {
            return FALSE;
        }
        bounds.min[a] = (s16) (position - radius);
        bounds.max[a] = (s16) (position + radius);
    }
    // Visibility is a conservative frustum test. Walls never decide sleep.
    return !physics_lab_enemy_in_view(&bounds);
}

// Compare squared world-unit distance; horizontalOnly ignores height.
static s32 enemy_sleep_far(f32 x, f32 y, f32 z, f32 distance, s32 horizontalOnly) {
    f32 dx = x - gMarioObject->oPosX;
    f32 dy = horizontalOnly ? 0.0f : y - gMarioObject->oPosY;
    f32 dz = z - gMarioObject->oPosZ;
    return dx * dx + dy * dy + dz * dz > distance * distance;
}

// The awake volume uses separate horizontal and vertical limits, without square roots.
static s32 enemy_sleep_outside_cylinder(f32 x, f32 y, f32 z, f32 horizontal) {
    f32 dx = x - gMarioObject->oPosX;
    f32 dy = y - gMarioObject->oPosY;
    f32 dz = z - gMarioObject->oPosZ;
    return dx * dx + dz * dz > horizontal * horizontal
           || dy * dy > ENEMY_SLEEP_VERTICAL_DISTANCE * ENEMY_SLEEP_VERTICAL_DISTANCE;
}

// Behavior wake-ups must not make a distant model flash. Valid attachments share
// their owner's display history, including the two-level Lakitu/cloud assembly.
// Bob-ombs and tethered Chomps use stable boundaries in every level.
s32 physics_lab_update_enemy_hidden_state(struct Object *obj) {
    s32 bobomb = enemy_sleep_has_behavior(obj, bhvBobomb);
    s32 chomp = enemy_sleep_has_behavior(obj, bhvChainChomp)
                || (enemy_sleep_has_behavior(obj, bhvChainChompChainPart)
                    && enemy_sleep_has_behavior(obj->parentObj, bhvChainChomp));
    if (bobomb && (!PHYSICS_LAB_AGGRESSIVE_SLEEP || gCurrLevelNum != LEVEL_PHYSICS_LAB)
        && (obj->oAction != BOBOMB_ACT_PATROL || obj->oBobombFuseLit || obj->oBobombFuseTimer != 0
            || obj->oHeldState != HELD_FREE)) {
        obj->oFlags &= ~OBJ_FLAG_LAB_ENEMY_VISIBLE;
        return FALSE;
    }
    if ((!bobomb && !chomp && (!PHYSICS_LAB_AGGRESSIVE_SLEEP || gCurrLevelNum != LEVEL_PHYSICS_LAB))
        || gMarioObject == NULL || !(obj->oFlags & OBJ_FLAG_HIDE_WHEN_SLEEPING)
        || obj->oHeldState != HELD_FREE || obj == gMarioObject) {
        return FALSE;
    }
    struct Object *owner = obj;
    for (s32 depth = 0; depth < 2; depth++) {
        struct Object *parent = enemy_sleep_parent(owner);
        if (parent == NULL) {
            break;
        }
        if (parent->activeFlags == ACTIVE_FLAG_DEACTIVATED
            || !(parent->oFlags & OBJ_FLAG_HIDE_WHEN_SLEEPING)) {
            return FALSE;
        }
        owner = parent;
    }
    chomp = enemy_sleep_has_behavior(owner, bhvChainChomp);
    if (chomp && owner->oChainChompReleaseStatus != CHAIN_CHOMP_NOT_RELEASED) {
        owner->oFlags &= ~OBJ_FLAG_LAB_ENEMY_VISIBLE;
        return FALSE;
    }
    f32 horizontal = chomp ? CHOMP_VISIBILITY_WAKE_DISTANCE : LAB_SLEEP_WAKE_DISTANCE;
    f32 vertical = ENEMY_SLEEP_VERTICAL_DISTANCE;
    if (owner->collisionData != NULL) {
        horizontal = MAX(horizontal, owner->oCollisionDistance + 600.0f);
    }
    if (owner->oFlags & OBJ_FLAG_LAB_ENEMY_VISIBLE) {
        horizontal += chomp ? CHOMP_VISIBILITY_EXIT_MARGIN : LAB_ENEMY_VISIBILITY_HORIZONTAL_MARGIN;
        vertical += LAB_ENEMY_VISIBILITY_VERTICAL_MARGIN;
    }
    f32 dx = owner->oPosX - gMarioObject->oPosX;
    f32 dy = owner->oPosY - gMarioObject->oPosY;
    f32 dz = owner->oPosZ - gMarioObject->oPosZ;
    s32 hidden = dx * dx + dz * dz > horizontal * horizontal || dy * dy > vertical * vertical;
    COND_BIT(!hidden, owner->oFlags, OBJ_FLAG_LAB_ENEMY_VISIBLE);
    return hidden;
}

// The optional conservative Lab policy retains its original sphere/XZ comparisons.
// Other levels always check cylinder height, even when horizontalOnly is requested.
static s32 enemy_sleep_outside_range(f32 x, f32 y, f32 z, f32 distance, s32 horizontalOnly) {
    if (gCurrLevelNum == LEVEL_PHYSICS_LAB) {
        return enemy_sleep_far(x, y, z, distance, horizontalOnly);
    }
    return enemy_sleep_outside_cylinder(x, y, z, MAX(distance, VANILLA_SLEEP_HORIZONTAL_DISTANCE));
}

static s32 enemy_sleep_grounded_patrol(struct Object *obj) {
    const BehaviorScript *behavior = obj->behavior;
    s32 patrol = FALSE;
    if (behavior == segmented_to_virtual(bhvSmallWhomp)) {
        patrol = obj->oBehParams2ndByte == 0 && (obj->oAction == 1 || obj->oAction == 7);
    } else if (behavior == segmented_to_virtual(bhvChuckya)) {
        patrol = obj->oAction == 0 && (obj->oSubAction == 0 || obj->oSubAction == 2)
                 && obj->oCommonAnchorAction == 0;
    } else if (gEnemyPatrolSleepTrial == 2 && behavior == segmented_to_virtual(bhvSkeeter)) {
        patrol = obj->oAction == SKEETER_ACT_WALK && !(obj->oMoveFlags & OBJ_MOVE_HIT_WALL);
    }
    // Native floor response can retain a small upward velocity without leaving the floor.
    return patrol && (obj->oMoveFlags & OBJ_MOVE_ON_GROUND)
           && !(obj->oMoveFlags
                & (OBJ_MOVE_IN_AIR | OBJ_MOVE_LEFT_GROUND | OBJ_MOVE_BOUNCE | OBJ_MOVE_MASK_IN_WATER))
           && obj->oFloor != NULL && absf(obj->oPosY - obj->oFloorHeight) < 1.0f
           && absf(obj->oVelY) <= 5.0f;
}

// Fill a per-query policy on a rule match; FALSE leaves the output untouched.
static s32 enemy_sleep_resolve_policy(struct Object *obj, struct EnemySleepPolicy *policy) {
    for (u32 i = 0; i < ARRAY_COUNT(sEnemySleepRules); i++) {
        const struct EnemySleepRule *rule = &sEnemySleepRules[i];
        if (obj->behavior != segmented_to_virtual(rule->behavior)) {
            continue;
        }
        policy->wakeDistance = rule->wakeDistance;
        policy->viewRadius = rule->viewRadius;
        policy->settled = (rule->flags & ENEMY_SLEEP_SETTLED) != 0;
        policy->aquatic = (rule->flags & ENEMY_SLEEP_AQUATIC) != 0;
        policy->airborne =
            (rule->flags & ENEMY_SLEEP_AIRBORNE) != 0 || enemy_sleep_grounded_patrol(obj);
        policy->surface = (rule->flags & ENEMY_SLEEP_SURFACE) != 0;
        // Fly Guy's idle loop resets oTimer; its expanded pose proves settling.
        if (enemy_sleep_has_behavior(obj, bhvFlyGuy)) {
            policy->settled = obj->header.gfx.scale[0] == 1.5f;
        }
        if (policy->surface) {
            policy->viewRadius = MAX(rule->viewRadius, obj->oCollisionDistance + 600.0f);
        }
        return TRUE;
    }
    return FALSE;
}

// TRUE permits sleeping; the caller still checks enrollment, proximity and support.
// Aggressive Lab sleeping uses a separate policy.
static s32 enemy_native_state_allows_sleep(struct Object *obj) {
    if (enemy_sleep_grounded_patrol(obj)) {
        return TRUE;
    }
    const BehaviorScript *behavior = obj->behavior;
    if (behavior == segmented_to_virtual(bhvGoomba)
#ifdef FLOOMBAS
        || behavior == segmented_to_virtual(bhvFloomba)
#endif
    ) {
        return obj->oAction == GOOMBA_ACT_WALK && (obj->oMoveFlags & OBJ_MOVE_ON_GROUND);
    }
    if (behavior == segmented_to_virtual(bhvKoopa)) {
        return (obj->oMoveFlags & OBJ_MOVE_ON_GROUND)
               && ((obj->oKoopaMovementType == KOOPA_BP_NORMAL
                    && (obj->oAction == KOOPA_SHELLED_ACT_WALK
                        || obj->oAction == KOOPA_SHELLED_ACT_STOPPED))
                   || (obj->oKoopaMovementType == KOOPA_BP_UNSHELLED
                       && obj->oAction == KOOPA_UNSHELLED_ACT_RUN));
    }
    if (behavior == segmented_to_virtual(bhvBobomb)) {
        // Bob-ombs use object_step's floor position, rather than oMoveFlags.
        return obj->oAction == BOBOMB_ACT_PATROL && !obj->oBobombFuseLit && obj->oBobombFuseTimer == 0
               && obj->oFloor != NULL && absf(obj->oPosY - obj->oFloorHeight) < 1.0f
               && obj->oVelY == 0.0f;
    }
    if (behavior == segmented_to_virtual(bhvScuttlebug)) {
        return obj->oAction == 0 && obj->oSubAction == 1 && !obj->oScuttlebugIsAtttacking
               && (obj->oMoveFlags & OBJ_MOVE_ON_GROUND);
    }
    if (behavior == segmented_to_virtual(bhvFirePiranhaPlant)) {
        return obj->oAction == FIRE_PIRANHA_PLANT_ACT_HIDE && !obj->oFirePiranhaPlantActive
               && obj->oFirePiranhaPlantScale == 0.0f && obj->oFirePiranhaPlantDeathSpinTimer == 0;
    }
    if (behavior == segmented_to_virtual(bhvMoneybagHidden)) {
        return obj->oAction == FAKE_MONEYBAG_COIN_ACT_IDLE;
    }
    if (behavior == segmented_to_virtual(bhvMadPiano)) {
        return obj->oAction == MAD_PIANO_ACT_WAIT && obj->oForwardVel == 0.0f;
    }
    if (behavior == segmented_to_virtual(bhvSmallBully)
        || behavior == segmented_to_virtual(bhvSmallChillBully)) {
        return obj->oAction == BULLY_ACT_PATROL && (obj->oMoveFlags & OBJ_MOVE_ON_GROUND);
    }
    if (behavior == segmented_to_virtual(bhvSpindrift)) {
        return obj->oAction == 0 && (obj->oMoveFlags & OBJ_MOVE_ON_GROUND);
    }
    if (behavior == segmented_to_virtual(bhvSpiny)) {
        return obj->oAction == SPINY_ACT_WALK && (obj->oMoveFlags & OBJ_MOVE_ON_GROUND);
    }
    if (behavior == segmented_to_virtual(bhvPiranhaPlant)) {
        // Let the native loop stop any outstanding lullaby before sleeping.
        return obj->oAction == PIRANHA_PLANT_ACT_SLEEPING && obj->oPiranhaPlantSleepMusicState != 0;
    }
    if (behavior == segmented_to_virtual(bhvSwoop)) {
        return obj->oAction == SWOOP_ACT_IDLE && obj->header.gfx.scale[0] == 1.0f;
    }
    if (behavior == segmented_to_virtual(bhvSnufit)) {
        return obj->oAction == SNUFIT_ACT_IDLE;
    }
    if (behavior == segmented_to_virtual(bhvFlyGuy)) {
        return obj->oAction == FLY_GUY_ACT_IDLE;
    }
    if (behavior == segmented_to_virtual(bhvHomingAmp)) {
        return obj->oAction == HOMING_AMP_ACT_INACTIVE;
    }
    if (behavior == segmented_to_virtual(bhvCirclingAmp)) {
        return obj->oAction == AMP_ACT_IDLE;
    }

    if (behavior == segmented_to_virtual(bhvBulletBill)) {
        return obj->oAction == 1 && obj->oForwardVel == 0.0f;
    }
    if (behavior == segmented_to_virtual(bhvEnemyLakitu)) {
        return obj->oAction == ENEMY_LAKITU_ACT_UNINITIALIZED
               || (obj->oAction == ENEMY_LAKITU_ACT_MAIN
                   && obj->oSubAction == ENEMY_LAKITU_SUB_ACT_NO_SPINY && obj->prevObj == NULL);
    }
    if (behavior == segmented_to_virtual(bhvHauntedChair)) {
        return obj->oAction == HAUNTED_CHAIR_ACT_FALL_OR_SPIN && obj->parentObj == obj
               && obj->oHauntedChairSpinTimer > 0;
    }
    if (behavior == segmented_to_virtual(bhvBoo)) {
        return obj->oAction == 0 && obj->oForwardVel == 0.0f && obj->oVelY == 0.0f
               && obj->oBooDeathStatus == BOO_DEATH_STATUS_ALIVE
               && obj->oOpacity == obj->oBooTargetOpacity;
    }
    if (behavior == segmented_to_virtual(bhvClamShell)) {
        return obj->oAction == CLAM_ACT_CLOSING && obj->oClamShakeTimer == 0
               && obj->header.gfx.animInfo.animFrame >= CLAM_CLOSING_DAMAGE_FRAME && obj->oTimer > 30;
    }
    if (behavior == segmented_to_virtual(bhvTweester)) {
        return obj->oAction == TWEESTER_ACT_IDLE && obj->oSubAction == TWEESTER_SUB_ACT_WAIT
               && obj->header.gfx.scale[0] == 0.0f;
    }
    if (behavior == segmented_to_virtual(bhvCloud)) {
        return obj->oBehParams2ndByte == CLOUD_BP_FWOOSH && obj->oAction == CLOUD_ACT_FWOOSH_HIDDEN;
    }
    if (behavior == segmented_to_virtual(bhvFlamethrower)) {
        return obj->oAction == FLAMETHROWER_ACT_IDLE;
    }
    if (behavior == segmented_to_virtual(bhvBouncingFireball)) {
        return obj->oAction == 0;
    }
    if (behavior == segmented_to_virtual(bhvWaterBombSpawner)) {
        f32 radius = 50.0f * GET_BPARAM2(obj->oBehParams) + 200.0f;
        return !obj->oWaterBombSpawnerBombActive
               && enemy_sleep_far(obj->oPosX, obj->oPosY, obj->oPosZ, radius, TRUE);
    }
    if (behavior == segmented_to_virtual(bhvBookendSpawn)) {
        return obj->oTimer > 40;
    }
    if (behavior == segmented_to_virtual(bhvBubSpawner)) {
        return obj->oAction == BUB_SPAWNER_ACT_SPAWN_BUBS;
    }
    if (behavior == segmented_to_virtual(bhvChuckya)) {
        // Sub-action 3 walks home as well as resting there; grabbing uses another action.
        return obj->oAction == 0 && obj->oSubAction == 3 && obj->oCommonAnchorAction == 0
               && (obj->oMoveFlags & OBJ_MOVE_ON_GROUND);
    }
    if (behavior == segmented_to_virtual(bhvHeaveHo)) {
        return obj->oHeaveHoThrowState == 0
               && (obj->oAction == 0
                   || ((obj->oAction == 1 || obj->oAction == 2)
                       && (obj->oMoveFlags & OBJ_MOVE_ON_GROUND)));
    }
    if (behavior == segmented_to_virtual(bhvMrI)) {
        return obj->oAction == MR_I_BODY_ACT_FAR_AWAY && !obj->oMrIBlinking && obj->oTimer > 0;
    }
    if (behavior == segmented_to_virtual(bhvMrBlizzard)) {
        // The native LAUNCH state can still wait on its owner: require the held links.
        return obj->oAction == MR_BLIZZARD_ACT_HIDE_UNHIDE && obj->oMrBlizzardGraphYOffset < 0.0f
               && obj->oMrBlizzardHeldObj != NULL && obj->prevObj == obj->oMrBlizzardHeldObj
               && obj->oMrBlizzardHeldObj->oAction == MR_BLIZZARD_SNOWBALL_ACT_LAUNCH
               && !enemy_sleep_pending(obj->oMrBlizzardHeldObj);
    }
    if (behavior == segmented_to_virtual(bhvPokey)) {
        if (obj->oAction == POKEY_ACT_UNINITIALIZED) {
            return enemy_sleep_far(obj->oPosX, obj->oPosY, obj->oPosZ, obj->oDrawingDistance, FALSE);
        }
        // Never prevent the native unload transition or freeze a regenerating stack.
        return obj->oAction == POKEY_ACT_WANDER && !obj->oPokeyHeadWasKilled
               && obj->oPokeyNumAliveBodyParts == POKEY_NUM_SEGMENTS
               && obj->oPokeyBottomBodyPartSize == 1.0f && (obj->oMoveFlags & OBJ_MOVE_ON_GROUND)
               && !enemy_sleep_far(obj->oPosX, obj->oPosY, obj->oPosZ, obj->oDrawingDistance + 500.0f,
                                   FALSE);
    }
    if (behavior == segmented_to_virtual(bhvChainChomp)) {
        if (obj->oAction == CHAIN_CHOMP_ACT_UNINITIALIZED) {
            return TRUE;
        }
        return obj->oAction == CHAIN_CHOMP_ACT_MOVE
               && obj->oChainChompReleaseStatus == CHAIN_CHOMP_NOT_RELEASED
               && obj->oSubAction == CHAIN_CHOMP_SUB_ACT_TURN && obj->oFloor != NULL
               && enemy_sleep_has_behavior(obj->parentObj, bhvChainChompChainPart)
               && obj->parentObj->activeFlags != ACTIVE_FLAG_DEACTIVATED
               && obj->oChainChompSignedMaxDistBetweenChainParts == 0.0f
               && !enemy_sleep_far(obj->oPosX, obj->oPosY, obj->oPosZ,
                                   (4000.0f + CHAIN_CHOMP_NUM_SEGMENTS * 180.0f), FALSE);
    }
    if (behavior == segmented_to_virtual(bhvMontyMole)) {
        return obj->oAction == MONTY_MOLE_ACT_SELECT_HOLE && obj->prevObj == NULL
               && physics_lab_monty_mole_can_sleep();
    }
    if (behavior == segmented_to_virtual(bhvUkiki)) {
        if (obj->oUkikiTextState != UKIKI_TEXT_DEFAULT || obj->oUkikiHasCap
            || !(obj->oMoveFlags & OBJ_MOVE_ON_GROUND)) {
            return FALSE;
        }
        if (obj->oBehParams2ndByte == UKIKI_BP_CAP) {
            // The cap monkey's native far-away reset must finish before it can pause.
            return obj->oAction == UKIKI_ACT_IDLE && obj->oForwardVel == 0.0f
                   && obj->oPosX == obj->oHomeX && obj->oPosZ == obj->oHomeZ;
        }
        return obj->oBehParams2ndByte == UKIKI_BP_CAGE
               && (obj->oAction == UKIKI_ACT_IDLE || obj->oAction == UKIKI_ACT_RUN
                   || obj->oAction == UKIKI_ACT_TURN_TO_MARIO);
    }
    if (behavior == segmented_to_virtual(bhvBubba)) {
        return obj->oAction == BUBBA_ACT_IDLE && (obj->oMoveFlags & OBJ_MOVE_MASK_IN_WATER)
               && !(obj->oMoveFlags & OBJ_MOVE_ENTERED_WATER) && obj->oBubbaLungeTimer == 0
               && obj->oBubbaAirVelY == 0.0f && obj->oBubbaJumpHeight <= 1.0f;
    }
    if (behavior == segmented_to_virtual(bhvSkeeter)) {
        if (gSkeeterWaterSleepTrial && (obj->oMoveFlags & OBJ_MOVE_AT_WATER_SURFACE)) {
            // Surface flags persist while sleeping; sample live water to wake on level changes.
            return (obj->oAction == SKEETER_ACT_IDLE || obj->oAction == SKEETER_ACT_LUNGE)
                   && obj->oVelY == 0.0f
                   && !(obj->oMoveFlags
                        & (OBJ_MOVE_MASK_ON_GROUND | OBJ_MOVE_ENTERED_WATER | OBJ_MOVE_LEAVING_WATER
                           | OBJ_MOVE_IN_AIR | OBJ_MOVE_HIT_WALL))
                   && find_water_level(obj->oPosX, obj->oPosZ) == obj->oPosY;
        }
        return obj->oAction == SKEETER_ACT_IDLE && obj->oForwardVel == 0.0f && obj->oVelY == 0.0f
               && (obj->oMoveFlags & (OBJ_MOVE_ON_GROUND | OBJ_MOVE_AT_WATER_SURFACE));
    }
    if (behavior == segmented_to_virtual(bhvBub)) {
        return enemy_sleep_has_behavior(obj->parentObj, bhvBubSpawner)
               && obj->parentObj->activeFlags != ACTIVE_FLAG_DEACTIVATED
               && obj->parentObj->oAction == BUB_SPAWNER_ACT_IDLE
               && gMarioObject->oPosY - obj->parentObj->oPosY <= 2000.0f
               && (obj->oAction == BUB_ACT_SWIMMING_TOWARDS_MARIO
                   || obj->oAction == BUB_ACT_SWIMMING_AWAY_FROM_MARIO);
    }
    if (behavior == segmented_to_virtual(bhvSushiShark)) {
        return TRUE;
    }
    if (behavior == segmented_to_virtual(bhvUnagi)) {
        return obj->oAction == UNAGI_ACT_SHIP_PATH && !obj->oUnagiHasStar;
    }
    if (behavior == segmented_to_virtual(bhvKlepto)) {
        return obj->oAction == KLEPTO_ACT_WAIT_FOR_MARIO
               && obj->oAnimState == KLEPTO_ANIM_STATE_HOLDING_NOTHING;
    }
    if (behavior == segmented_to_virtual(bhvBigBoulder)) {
        return obj->oAction == 1 && obj->oFloor != NULL && obj->oFloor->normal.y > 0.999f
               && obj->oVelY == 0.0f && obj->oPosY >= obj->oFloorHeight
               && obj->oPosY < obj->oFloorHeight + 37.0f;
    }
    if (behavior == segmented_to_virtual(bhvSmallWhomp)) {
        return obj->oAction == 0 && obj->oBehParams2ndByte == 0 && obj->oForwardVel == 0.0f;
    }
    if (behavior == segmented_to_virtual(bhvThwomp) || behavior == segmented_to_virtual(bhvGrindel)) {
        return obj->oAction == GRINDEL_THWOMP_ACT_ON_GROUND && obj->oVelY == 0.0f && obj->oTimer > 0;
    }
    if (behavior == segmented_to_virtual(bhvToxBox)) {
        return obj->oAction >= 1 && obj->oAction <= 3 && obj->oTimer > 0;
    }
    if (behavior == segmented_to_virtual(bhvSpindel)) {
        return obj->oSpindelMoveTimer == -1 && obj->oTimer > 0 && obj->oVelZ == 0.0f
               && obj->oAngleVelPitch == 0;
    }
    return FALSE;
}

s32 physics_lab_discard_distant_effect(struct Object *obj) {
    if (!PHYSICS_LAB_AGGRESSIVE_SLEEP || gCurrLevelNum != LEVEL_PHYSICS_LAB || gMarioObject == NULL
        || (gTimeStopState & TIME_STOP_ACTIVE) || obj->activeFlags == ACTIVE_FLAG_DEACTIVATED
        || obj->curBhvCommand == obj->behavior || obj->oHeldState != HELD_FREE
        || !enemy_sleep_far(obj->oPosX, obj->oPosY, obj->oPosZ, LAB_EFFECT_DISCARD_DISTANCE, FALSE)) {
        return FALSE;
    }
    struct Object *parent = obj->parentObj;
    if (enemy_sleep_has_behavior(obj, bhvSpiny) && enemy_sleep_has_behavior(parent, bhvEnemyLakitu)) {
        if (obj->oAction == SPINY_ACT_HELD_BY_LAKITU) {
            return FALSE;
        }
        if (parent->oEnemyLakituNumSpinies > 0) {
            parent->oEnemyLakituNumSpinies--;
        }
    } else if (enemy_sleep_has_behavior(obj, bhvWaterBomb)) {
        obj->oAction = WATER_BOMB_ACT_EXPLODE;
        if (enemy_sleep_has_behavior(parent, bhvWaterBombSpawner)) {
            parent->oWaterBombSpawnerBombActive = FALSE;
        }
    } else if (enemy_sleep_has_behavior(obj, bhvMrBlizzardSnowball)) {
        if (enemy_sleep_has_behavior(parent, bhvMrBlizzard) && parent->prevObj == obj) {
            return FALSE;
        }
    } else {
        s32 disposable = (obj->activeFlags & ACTIVE_FLAG_UNIMPORTANT) != 0;
        for (u32 i = 0; i < ARRAY_COUNT(sLabDisposableBehaviors); i++) {
            if (obj->behavior == segmented_to_virtual(sLabDisposableBehaviors[i])) {
                disposable = TRUE;
                break;
            }
        }
        if (!disposable) {
            return FALSE;
        }
    }
    obj_mark_for_deletion(obj);
    return TRUE;
}

static s32 physics_lab_should_sleep_aggressively(struct Object *obj) {
    if (gCurrLevelNum != LEVEL_PHYSICS_LAB || gMarioObject == NULL || obj == gMarioObject
        || !(obj->oFlags & OBJ_FLAG_ALLOW_ENEMY_SLEEP) || (gTimeStopState & TIME_STOP_ACTIVE)
        || obj->activeFlags == ACTIVE_FLAG_DEACTIVATED || obj->curBhvCommand == obj->behavior
        || obj->oHeldState != HELD_FREE || obj->oInteractStatus != INT_STATUS_NONE
        || obj->numCollidedObjs != 0 || gMarioObject->platform == obj) {
        return FALSE;
    }
    // A carried Boo coin follows its owner; a released one must run its expiry.
    if (enemy_sleep_has_behavior(obj, bhvCoinInsideBoo)
        && obj->oAction != COIN_INSIDE_BOO_ACT_CARRIED) {
        return FALSE;
    }
    if (obj->oFlags & OBJ_FLAG_HIDE_WHEN_SLEEPING) {
        // This decision also refreshes the validated owner's display history.
        return physics_lab_update_enemy_hidden_state(obj);
    }
    f32 distance = LAB_SLEEP_WAKE_DISTANCE;
    if (obj->collisionData != NULL) {
        distance = MAX(distance, obj->oCollisionDistance + 600.0f);
    }
    if (enemy_sleep_has_behavior(obj, bhvPhysicsLabMover)) {
        // Freeze movers by their common home envelope, keeping paired closing
        // walls synchronized even when one wall is closer to Mario.
        return enemy_sleep_outside_cylinder(obj->oHomeX, obj->oHomeY, obj->oHomeZ, distance + 900.0f);
    }
    if (!enemy_sleep_outside_cylinder(obj->oPosX, obj->oPosY, obj->oPosZ, distance)) {
        return FALSE;
    }
    struct Object *parent = enemy_sleep_parent(obj);
    if (parent != NULL) {
        // Keep both halves of an attachment awake near Mario, without recursive
        // ownership walks (Chain Chomp has a cyclic pivot).
        return parent->activeFlags != ACTIVE_FLAG_DEACTIVATED
               && enemy_sleep_outside_cylinder(parent->oPosX, parent->oPosY, parent->oPosZ, distance);
    }
    // Spawned supplies need not have SET_HOME; their live position is sufficient.
    return TRUE;
}

// -1 leaves existing enemy/Lab policies in charge; 0 keeps an enrolled actor live.
static s32 entity_sleep_state(struct Object *obj) {
    const BehaviorScript *behavior = obj->behavior;
    if (behavior == segmented_to_virtual(bhvBreakableBoxSmall)) {
        return obj->oAction == BREAKABLE_BOX_SMALL_ACT_MOVE && !obj->oBreakableBoxSmallReleased
               && obj->oFloor != NULL && absf(obj->oPosY - obj->oFloorHeight) < 1.0f
               && obj->oForwardVel == 0.0f && obj->oVelY == 0.0f;
    }
    if (behavior == segmented_to_virtual(bhvPushableMetalBox)) {
        return obj->oForwardVel == 0.0f && obj->oVelY == 0.0f && obj->oFloor != NULL
               && absf(obj->oPosY - obj->oFloorHeight) < 1.0f;
    }
    if (behavior == segmented_to_virtual(bhvKoopaShell)) {
        // Box-supplied shells own an expiry clock, even when resting.
        return obj->oAction == KOOPA_SHELL_ACT_MARIO_NOT_RIDING
               && !enemy_sleep_has_behavior(obj->parentObj, bhvExclamationBox)
               && obj->oForwardVel == 0.0f && obj->oVelY == 0.0f
               && (obj->oMoveFlags & OBJ_MOVE_ON_GROUND);
    }
    if (behavior == segmented_to_virtual(bhvBobombBuddy)
        || behavior == segmented_to_virtual(bhvBobombBuddyOpensCannon)) {
        return obj->oAction == BOBOMB_BUDDY_ACT_IDLE;
    }
    if (behavior == segmented_to_virtual(bhvFish)) {
        struct Object *parent = obj->parentObj;
        return (obj->oAction == FISH_ACT_ROAM || obj->oAction == FISH_ACT_FLEE)
               && (enemy_sleep_has_behavior(parent, bhvFishSpawner)
                   || enemy_sleep_has_behavior(parent, bhvManyBlueFishSpawner)
                   || enemy_sleep_has_behavior(parent, bhvFewBlueFishSpawner))
               && parent->activeFlags != ACTIVE_FLAG_DEACTIVATED
               && parent->oAction == FISH_SPAWNER_ACT_IDLE && parent->oPrevAction == parent->oAction
               && ((obj->oBehParams2ndByte & FISH_BP_FLAG_IN_FULL_WATER_LEVEL)
                   || (enemy_sleep_outside_cylinder(obj->oPosX, obj->oPosY, obj->oPosZ,
                                                    VANILLA_SLEEP_HORIZONTAL_DISTANCE)
                       && find_water_level(obj->oPosX, obj->oPosZ) >= FLOOR_LOWER_LIMIT_MISC));
    }
    return -1;
}

static s32 entity_should_sleep(struct Object *obj, s32 state) {
    if (!state || gMarioObject == NULL || obj == gMarioObject || (gTimeStopState & TIME_STOP_ACTIVE)
        || enemy_sleep_pending(obj) || obj->oTimer == 0 || gMarioObject->platform == obj
        || object_platform_motion_needs_collision(obj)) {
        return FALSE;
    }
    f32 distance = VANILLA_SLEEP_HORIZONTAL_DISTANCE;
    if (obj->collisionData != NULL) {
        distance = MAX(distance, obj->oCollisionDistance + 600.0f);
    }
    // Stationary supplies use their live position; fish retain their
    // spawner's live cleanup gate.
    return enemy_sleep_outside_cylinder(obj->oPosX, obj->oPosY, obj->oPosZ, distance);
}

s32 enemy_should_sleep(struct Object *obj) {
    if (gMarioObject == NULL) {
        return FALSE;
    }
    // Stay awake through the exit margin after an approach reveals a Bob-omb.
    // Visibility is updated before this query by the shared object update path.
    if (enemy_sleep_has_behavior(obj, bhvBobomb) && obj->oAction == BOBOMB_ACT_PATROL
        && !obj->oBobombFuseLit && obj->oBobombFuseTimer == 0
        && (obj->oFlags & OBJ_FLAG_LAB_ENEMY_VISIBLE)) {
        return FALSE;
    }
    // A tether hop must not put a just-revealed Chomp immediately back to sleep.
    if (enemy_sleep_has_behavior(obj, bhvChainChomp)
        && obj->oChainChompReleaseStatus == CHAIN_CHOMP_NOT_RELEASED
        && (obj->oFlags & OBJ_FLAG_LAB_ENEMY_VISIBLE)) {
        return FALSE;
    }
    // Closed clams must not disappear where an opening clam is still drawn.
    if (enemy_sleep_has_behavior(obj, bhvClamShell)
        && !enemy_sleep_far(obj->oPosX, obj->oPosY, obj->oPosZ, obj->oDrawingDistance, FALSE)) {
        return FALSE;
    }
    if (obj->oFlags & OBJ_FLAG_ALLOW_ENEMY_SLEEP) {
        s32 state = entity_sleep_state(obj);
        if (state >= 0) {
            return entity_should_sleep(obj, state);
        }
    }
    s32 inLab = gCurrLevelNum == LEVEL_PHYSICS_LAB;
    if (inLab && PHYSICS_LAB_AGGRESSIVE_SLEEP) {
        return physics_lab_should_sleep_aggressively(obj);
    }
    if (gMarioObject == NULL || obj == gMarioObject || !(obj->oFlags & OBJ_FLAG_ALLOW_ENEMY_SLEEP)
        || ((obj->oFlags & OBJ_FLAG_ACTIVE_FROM_AFAR) && !enemy_sleep_has_behavior(obj, bhvTweester)
            && !enemy_sleep_has_behavior(obj, bhvChainChomp)
            && !enemy_sleep_has_behavior(obj, bhvKlepto)
            && !enemy_sleep_has_behavior(obj, bhvCoinInsideBoo))
        || (gTimeStopState & TIME_STOP_ACTIVE) || enemy_sleep_pending(obj)) {
        return FALSE;
    }
    // Reject nearby actors before dispatching the larger behavior policy.
    if (!enemy_sleep_outside_range(obj->oPosX, obj->oPosY, obj->oPosZ, ENEMY_SLEEP_MIN_DISTANCE,
                                   FALSE)) {
        return FALSE;
    }
    struct Object *parent = enemy_sleep_parent(obj);
    if (parent != NULL) {
        return obj->collisionData == NULL && enemy_should_sleep(parent)
               && enemy_sleep_outside_range(obj->oPosX, obj->oPosY, obj->oPosZ,
                                            ENEMY_SLEEP_MIN_DISTANCE, FALSE)
               && (!inLab || enemy_sleep_outside_view(obj, 600.0f));
    }
    const BehaviorScript *behavior = obj->behavior;
    // Bub and released Spinies retain a spawner owner but have independent AI.
    s32 tripletGoomba = !inLab && behavior == segmented_to_virtual(bhvGoomba)
                        && enemy_sleep_has_behavior(obj->parentObj, bhvGoombaTripletSpawner);
    if (tripletGoomba
        && (obj->parentObj->activeFlags == ACTIVE_FLAG_DEACTIVATED
            || obj->parentObj->oAction == GOOMBA_TRIPLET_SPAWNER_ACT_UNLOADED)) {
        return FALSE;
    }
    if (!tripletGoomba && obj->parentObj != obj && behavior != segmented_to_virtual(bhvChainChomp)
        && behavior != segmented_to_virtual(bhvBub)) {
        if (behavior != segmented_to_virtual(bhvSpiny)
            || !enemy_sleep_has_behavior(obj->parentObj, bhvEnemyLakitu)
            || enemy_sleep_outside_range(obj->oPosX, obj->oPosY, obj->oPosZ, 2500.0f, FALSE)) {
            return FALSE;
        }
    }
    if (!enemy_sleep_outside_range(obj->oHomeX, obj->oHomeY, obj->oHomeZ, ENEMY_SLEEP_MIN_DISTANCE,
                                   FALSE)) {
        return FALSE;
    }
    struct EnemySleepPolicy policy;
    if (!enemy_sleep_resolve_policy(obj, &policy) || !enemy_native_state_allows_sleep(obj)
        || (!policy.settled && obj->oTimer <= ENEMY_SLEEP_SETTLE_FRAMES)
        || (!policy.airborne && obj->oVelY > 0.0f)
        || (!policy.aquatic && (obj->oMoveFlags & OBJ_MOVE_MASK_IN_WATER))
        || (obj->oMoveFlags & (OBJ_MOVE_ABOVE_LAVA | OBJ_MOVE_ABOVE_DEATH_BARRIER))
        || (obj->collisionData != NULL && !policy.surface)
        || (policy.surface && object_platform_motion_needs_collision(obj))) {
        return FALSE;
    }
    return enemy_sleep_outside_range(obj->oPosX, obj->oPosY, obj->oPosZ, policy.wakeDistance,
                                     policy.aquatic)
           && enemy_sleep_outside_range(obj->oHomeX, obj->oHomeY, obj->oHomeZ, policy.wakeDistance,
                                        policy.aquatic)
           && (!inLab || enemy_sleep_outside_view(obj, policy.viewRadius));
}

s32 physics_lab_needs_mario_angle(struct Object *obj) {
    if (gCurrLevelNum != LEVEL_PHYSICS_LAB) {
        return TRUE;
    }
    // These four idle states do not consume the angle. Keep this decision
    // independent of sleep eligibility and avoid unnecessary aiming work.
    const BehaviorScript *behavior = obj->behavior;
    if (behavior == segmented_to_virtual(bhvHomingAmp)) {
        return obj->oAction == HOMING_AMP_ACT_CHASE;
    }
    if (behavior == segmented_to_virtual(bhvCirclingAmp)) {
        return FALSE;
    }
    if (behavior == segmented_to_virtual(bhvPiranhaPlant)) {
        return obj->oAction != PIRANHA_PLANT_ACT_IDLE;
    }
    if (behavior == segmented_to_virtual(bhvSwoop)) {
        return obj->oAction != SWOOP_ACT_IDLE || obj->oDistanceToMario < 1500.0f;
    }
    return TRUE;
}

s32 physics_lab_skip_box_update(struct Object *obj) {
    if (gCurrLevelNum != LEVEL_PHYSICS_LAB || gMarioObject == NULL
        || obj->oAction != EXCLAMATION_BOX_ACT_ACTIVE || obj->oTimer == 0
        || obj->oPrevAction != obj->oAction || obj->oInteractStatus != INT_STATUS_NONE
        || obj->numCollidedObjs != 0 || obj->oPosY != obj->oHomeY || obj->header.gfx.scale[0] != 2.0f
        || obj->header.gfx.scale[1] != 2.0f || obj->header.gfx.scale[2] != 2.0f) {
        return FALSE;
    }
#ifdef AUTO_COLLISION_DISTANCE
    if (!(obj->oFlags & OBJ_FLAG_DONT_CALC_COLL_DIST)) {
        return FALSE;
    }
#endif
    f32 dx = obj->oPosX - gMarioObject->oPosX;
    f32 dy = obj->oPosY - gMarioObject->oPosY;
    f32 dz = obj->oPosZ - gMarioObject->oPosZ;
    f32 lateral = dx * dx + dz * dz;
    if (!(lateral >= obj->oCollisionDistance * obj->oCollisionDistance)
        || object_platform_motion_needs_collision(obj)) {
        return FALSE;
    }
    // Match load_object_collision_model's visibility, including its strict
    // boundary and the sentinel used by boxes without a distance update flag.
    obj->oDrawingDistance = MAX(obj->oDrawingDistance, obj->oCollisionDistance);
    s32 visible = obj->oDistanceToMario == 19000.0f
                      ? lateral + dy * dy < obj->oDrawingDistance * obj->oDrawingDistance
                      : obj->oDistanceToMario < obj->oDrawingDistance;
    COND_BIT(visible, obj->header.gfx.node.flags, GRAPH_RENDER_ACTIVE);
    return TRUE;
}

s32 physics_lab_should_emit_water_effect(struct Object *obj) {
    if (gMarioState == NULL) {
        return TRUE;
    }
    f32 dx = obj->oPosX - gMarioState->pos[0];
    f32 dz = obj->oPosZ - gMarioState->pos[2];
    s32 nearby = dx * dx + dz * dz < 3000.0f * 3000.0f;
    // Reject before any bounds checks or allocation. The area counter pauses
    // with simulation and does not consume gameplay RNG or change source AI.
    if (gCurrLevelNum == LEVEL_PHYSICS_LAB && (gAreaUpdateCounter & (nearby ? 1 : 3))) {
        return FALSE;
    }
    if (nearby) {
        return TRUE;
    }
    struct PhysicsLabRenderSection bounds;
    for (s32 a = 0; a < 3; a++) {
        f32 position = (&obj->oPosVec)[a];
        if (!(position >= -30000.0f && position <= 30000.0f)) {
            return TRUE;
        }
        // Includes feet, full wave scale, short lifetime and a camera margin.
        bounds.min[a] = (s16) (position - 1600.0f);
        bounds.max[a] = (s16) (position + 1600.0f);
    }
    // The two callers emit deterministic cosmetic waves, not random effects,
    // parent-owned particle bits or damaging projectiles.
    return object_water_effect_in_view(&bounds);
}
