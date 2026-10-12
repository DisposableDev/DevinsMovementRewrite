#include "particle_budget.h"

#include "behavior_data.h"
#include "engine/graph_node.h"
#include "object_fields.h"
#include "memory.h"
#include "object_list_processor.h"

struct ParticleBehaviorPolicy {
    const BehaviorScript *behavior;
    u8 priority;
};

static const struct ParticleBehaviorPolicy sParticleBehaviorPolicies[] = {
    { bhvBobombExplosionBubble, PARTICLE_PRIORITY_EXPLOSION_BUBBLE },
    { bhvBreakBoxTriangle, PARTICLE_PRIORITY_DEBRIS },
    { bhvPurpleParticle, PARTICLE_PRIORITY_DEBRIS },
    { bhvBobombBullyDeathSmoke, PARTICLE_PRIORITY_MIST },
    { bhvWaterDroplet, PARTICLE_PRIORITY_DROPLET },
    { bhvJrbSimpleFlipsMotosDroplet, PARTICLE_PRIORITY_DROPLET },
    { bhvJrbSimpleFlipsMotosSplash, PARTICLE_PRIORITY_FEEDBACK },
    { bhvWaterDropletSplash, PARTICLE_PRIORITY_DROPLET },
    { bhvBubbleSplash, PARTICLE_PRIORITY_DROPLET },
    { bhvObjectWaterSplash, PARTICLE_PRIORITY_DROPLET },
    { bhvThrownObjectWaterSplash, PARTICLE_PRIORITY_DROPLET },
    { bhvWind, PARTICLE_PRIORITY_AMBIENT },
    { bhvTreeSnow, PARTICLE_PRIORITY_AMBIENT },
    { bhvTreeLeaf, PARTICLE_PRIORITY_AMBIENT },
    { bhvWhitePuffSmoke2, PARTICLE_PRIORITY_AMBIENT },
    { bhvWhitePuffSmoke, PARTICLE_PRIORITY_AMBIENT },
    { bhvSmoke, PARTICLE_PRIORITY_AMBIENT },
    { bhvBobombFuseSmoke, PARTICLE_PRIORITY_AMBIENT },
    { bhvBlackSmokeMario, PARTICLE_PRIORITY_AMBIENT },
    { bhvBlackSmokeBowser, PARTICLE_PRIORITY_AMBIENT },
    { bhvTweesterSandParticle, PARTICLE_PRIORITY_AMBIENT },
    { bhvTinyStrongWindParticle, PARTICLE_PRIORITY_AMBIENT },
    { bhvObjectWaveTrail, PARTICLE_PRIORITY_AMBIENT },
    { bhvObjectWaterWave, PARTICLE_PRIORITY_AMBIENT },
    { bhvObjectBubble, PARTICLE_PRIORITY_AMBIENT },
    { bhvSmallParticle, PARTICLE_PRIORITY_AMBIENT },
    { bhvSmallParticleSnow, PARTICLE_PRIORITY_AMBIENT },
    { bhvSmallParticleBubbles, PARTICLE_PRIORITY_AMBIENT },
    { bhvPiranhaPlantWakingBubbles, PARTICLE_PRIORITY_AMBIENT },
    { bhvWhitePuffExplosion, PARTICLE_PRIORITY_FEEDBACK },
    { bhvSmallWaterWave, PARTICLE_PRIORITY_FEEDBACK },
    { bhvWaveTrail, PARTICLE_PRIORITY_FEEDBACK },
    { bhvCoinSparkles, PARTICLE_PRIORITY_FEEDBACK },
    { bhvCoinSparklesShort, PARTICLE_PRIORITY_FEEDBACK },
    { bhvWallTinyStarParticle, PARTICLE_PRIORITY_FEEDBACK },
    { bhvPoundTinyStarParticle, PARTICLE_PRIORITY_FEEDBACK },
    { bhvPunchTinyTriangle, PARTICLE_PRIORITY_FEEDBACK },
    { bhvGroundTwirlStunStar, PARTICLE_PRIORITY_FEEDBACK },
    { bhvSparkle, PARTICLE_PRIORITY_FEEDBACK },
    { bhvSparkleShort, PARTICLE_PRIORITY_FEEDBACK },
    { bhvWhitePuff1, PARTICLE_PRIORITY_FEEDBACK },
    { bhvWhitePuff2, PARTICLE_PRIORITY_FEEDBACK },
    { bhvReleasedPowerUpCap, PARTICLE_PRIORITY_FEEDBACK },
};
#define PARTICLE_POLICY_COUNT (sizeof(sParticleBehaviorPolicies) / sizeof(sParticleBehaviorPolicies[0]))
static const BehaviorScript *sParticleVirtualBehaviors[PARTICLE_POLICY_COUNT];
static s32 sParticleBehaviorsReady;

// Side tables avoid taking behavior-specific fields or changing Object layout.
static u8 sParticlePriorities[OBJECT_POOL_CAPACITY];
static u32 sParticleBirthOrder[OBJECT_POOL_CAPACITY];
// Only reviewed, replaceable effects enter this unordered list. Selection
// resolves pool-order ties explicitly, so insertion/removal need no shifts.
static u16 sParticleCandidates[OBJECT_POOL_CAPACITY];
static s32 sParticleCandidateCount;
// One owner per Mario flag bit; an older child must not clear a newer child's bit.
static struct Object *sMarioParticleFlagOwners[32];
static u32 sParticleNextBirthOrder;
static s32 sParticleFreeSlots;

static s32 particle_budget_index(struct Object *obj) {
    uintptr_t address = (uintptr_t) obj;
    uintptr_t start = (uintptr_t) gObjectPool;
    if (address < start || address >= start + sizeof(struct Object) * OBJECT_POOL_CAPACITY
        || (address - start) % sizeof(struct Object) != 0) {
        return -1;
    }
    return (address - start) / sizeof(struct Object);
}

static void particle_budget_remove_candidate(s32 index) {
    if (sParticlePriorities[index] == PARTICLE_PRIORITY_PROTECTED) {
        return;
    }
    for (s32 i = 0; i < sParticleCandidateCount; i++) {
        if (sParticleCandidates[i] == index) {
            sParticleCandidateCount--;
            if (i < sParticleCandidateCount) {
                sParticleCandidates[i] = sParticleCandidates[sParticleCandidateCount];
            }
            return;
        }
    }
}

void particle_budget_reset(void) {
    for (s32 i = 0; i < OBJECT_POOL_CAPACITY; i++) {
        sParticlePriorities[i] = PARTICLE_PRIORITY_PROTECTED;
        sParticleBirthOrder[i] = 0;
    }
    for (s32 i = 0; i < 32; i++) {
        sMarioParticleFlagOwners[i] = NULL;
    }
    sParticleBehaviorsReady = FALSE;
    sParticleNextBirthOrder = 0;
    sParticleCandidateCount = 0;
    sParticleFreeSlots = OBJECT_POOL_CAPACITY;
}

void particle_budget_allocated(struct Object *obj) {
    s32 index = particle_budget_index(obj);
    if (index >= 0) {
        particle_budget_remove_candidate(index);
        sParticleFreeSlots--;
        sParticlePriorities[index] = PARTICLE_PRIORITY_PROTECTED;
        sParticleBirthOrder[index] = sParticleNextBirthOrder++;
    }
}

void particle_budget_freed(struct Object *obj) {
    s32 index = particle_budget_index(obj);
    for (s32 i = 0; i < 32; i++) {
        if (sMarioParticleFlagOwners[i] == obj) {
            if (gMarioObject != NULL) {
                gMarioObject->oActiveParticleFlags &= ~(1u << i);
            }
            sMarioParticleFlagOwners[i] = NULL;
        }
    }
    if (index >= 0) {
        particle_budget_remove_candidate(index);
        sParticlePriorities[index] = PARTICLE_PRIORITY_PROTECTED;
        // Allocation overwrites the birth stamp before this slot can be eligible.
        sParticleFreeSlots++;
    }
}

s32 particle_budget_free_slots(void) {
    return sParticleFreeSlots;
}

enum ParticlePriority particle_budget_priority(const BehaviorScript *behavior) {
    // Behavior symbols may be segmented. Cache virtual addresses once per reset.
    if (!sParticleBehaviorsReady) {
        for (u32 i = 0; i < PARTICLE_POLICY_COUNT; i++) {
            sParticleVirtualBehaviors[i] = segmented_to_virtual(sParticleBehaviorPolicies[i].behavior);
        }
        sParticleBehaviorsReady = TRUE;
    }
    behavior = segmented_to_virtual(behavior);
    for (u32 i = 0; i < PARTICLE_POLICY_COUNT; i++) {
        if (behavior == sParticleVirtualBehaviors[i]) {
            return sParticleBehaviorPolicies[i].priority;
        }
    }
    // Controllers, hazards, healing bubbles and unreviewed effects are protected.
    return PARTICLE_PRIORITY_PROTECTED;
}

static void particle_budget_record_priority(s32 index, enum ParticlePriority priority) {
    if (index >= 0 && priority > PARTICLE_PRIORITY_PROTECTED && priority < PARTICLE_PRIORITY_REQUIRED) {
        if (sParticlePriorities[index] == PARTICLE_PRIORITY_PROTECTED) {
            sParticleCandidates[sParticleCandidateCount++] = index;
        }
        sParticlePriorities[index] = priority;
    }
}

void particle_budget_init_priority(struct Object *obj, enum ParticlePriority requested) {
    s32 index = particle_budget_index(obj);
    if (index < 0) {
        return;
    }
    enum ParticlePriority priority = particle_budget_priority(obj->behavior);
    // Classify once at birth, including explicit overrides of shared effects.
    // An unreviewed behavior remains protected even with an explicit priority.
    if (priority != PARTICLE_PRIORITY_PROTECTED) {
        if (requested != PARTICLE_PRIORITY_REQUIRED) {
            priority = requested;
        }
        particle_budget_record_priority(index, priority);
    }
}

void particle_budget_set_priority(struct Object *obj, enum ParticlePriority priority) {
    s32 index = particle_budget_index(obj);
    if (index >= 0 && particle_budget_priority(obj->behavior) != PARTICLE_PRIORITY_PROTECTED) {
        particle_budget_record_priority(index, priority);
    }
}

// Candidates have already passed the protection checks. Prefer inactive slots,
// then lower priority, then older effects; subtraction retains wraparound order.
static s32 particle_budget_precedes(s32 poolIndex, s32 bestPoolIndex) {
    s32 isInactive = !(gObjectPool[poolIndex].activeFlags & ACTIVE_FLAG_ACTIVE);
    s32 bestIsInactive = !(gObjectPool[bestPoolIndex].activeFlags & ACTIVE_FLAG_ACTIVE);
    if (isInactive != bestIsInactive) {
        return isInactive;
    }
    if (sParticlePriorities[poolIndex] != sParticlePriorities[bestPoolIndex]) {
        return sParticlePriorities[poolIndex] < sParticlePriorities[bestPoolIndex];
    }
    s32 birthOrderDelta = (s32) (sParticleBirthOrder[poolIndex] - sParticleBirthOrder[bestPoolIndex]);
    return birthOrderDelta != 0 ? birthOrderDelta < 0 : poolIndex < bestPoolIndex;
}

struct Object *particle_budget_select(enum ParticlePriority incomingPriority, struct Object *parent) {
    struct Object *best = NULL;
    s32 bestPoolIndex = -1;
    for (s32 candidateIndex = 0; candidateIndex < sParticleCandidateCount; candidateIndex++) {
        s32 poolIndex = sParticleCandidates[candidateIndex];
        struct Object *obj = &gObjectPool[poolIndex];
        u8 candidatePriority = sParticlePriorities[poolIndex];
        s32 isInactive;
        if (candidatePriority == PARTICLE_PRIORITY_PROTECTED || obj == gCurrentObject || obj == parent
            || obj->respawnInfo != NULL || obj->oInteractType != 0 || obj->collisionData != NULL) {
            continue;
        }
        isInactive = !(obj->activeFlags & ACTIVE_FLAG_ACTIVE);
        if (!isInactive && candidatePriority >= incomingPriority) {
            continue;
        }
        if (best == NULL || particle_budget_precedes(poolIndex, bestPoolIndex)) {
            best = obj;
            bestPoolIndex = poolIndex;
        }
    }
    return best;
}

s32 particle_budget_burst_count(enum ParticlePriority priority, s32 requested) {
    s32 limit;
    if (requested <= 0) {
        return 0;
    }
    if (sParticleFreeSlots - requested >= PARTICLE_SOFT_RESERVE) {
        return requested;
    }
    switch (priority) {
        case PARTICLE_PRIORITY_CELEBRATION:
            limit = 12;
            break;
        case PARTICLE_PRIORITY_EXPLOSION_BUBBLE:
            limit = 8;
            break;
        case PARTICLE_PRIORITY_DEBRIS:
            limit = requested > 20 ? 8 : 6;
            break;
        case PARTICLE_PRIORITY_MIST:
            limit = 6;
            break;
        case PARTICLE_PRIORITY_DROPLET:
            limit = requested > 5 ? 6 : 2;
            break;
        case PARTICLE_PRIORITY_AMBIENT:
            limit = 2;
            break;
        default:
            limit = 8;
            break;
    }
    return requested < limit ? requested : limit;
}

void particle_budget_own_flags(struct Object *obj, struct Object *parent, u32 flagMask) {
    parent->oActiveParticleFlags |= flagMask;
    if (parent == gMarioObject) {
        for (s32 i = 0; i < 32; i++) {
            if (flagMask & (1u << i)) {
                sMarioParticleFlagOwners[i] = obj;
            }
        }
    }
}

void particle_budget_clear_parent_flags(struct Object *obj, u32 flagMask) {
    struct Object *parent = obj->parentObj;
    if (parent != NULL && parent == gMarioObject) {
        for (s32 i = 0; i < 32; i++) {
            u32 bit = 1u << i;
            if ((flagMask & bit)
                && (sMarioParticleFlagOwners[i] == NULL || sMarioParticleFlagOwners[i] == obj)) {
                parent->oActiveParticleFlags &= ~bit;
                sMarioParticleFlagOwners[i] = NULL;
            }
        }
    } else if (parent != NULL) {
        parent->oActiveParticleFlags &= ~flagMask;
    }
}

s32 particle_budget_clear_parent_field(struct Object *obj, u8 fieldIndex, u32 flagMask) {
    if (obj->parentObj != NULL
        && &obj->parentObj->rawData.asU32[fieldIndex] == &obj->parentObj->oActiveParticleFlags) {
        particle_budget_clear_parent_flags(obj, flagMask);
        return TRUE;
    }
    return FALSE;
}
