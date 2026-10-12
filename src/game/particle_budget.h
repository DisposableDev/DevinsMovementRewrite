#ifndef PARTICLE_BUDGET_H
#define PARTICLE_BUDGET_H

#include "types.h"

// Larger values are retained longer. Zero is never eligible for reclamation.
enum ParticlePriority {
    PARTICLE_PRIORITY_PROTECTED,
    PARTICLE_PRIORITY_CELEBRATION,
    PARTICLE_PRIORITY_EXPLOSION_BUBBLE,
    PARTICLE_PRIORITY_DEBRIS,
    PARTICLE_PRIORITY_MIST,
    PARTICLE_PRIORITY_DROPLET,
    PARTICLE_PRIORITY_AMBIENT,
    PARTICLE_PRIORITY_FEEDBACK,
    // Required-spawn/default-policy request; never stored in the candidate table.
    PARTICLE_PRIORITY_REQUIRED,
};

// Optional spawns reclaim instead of consuming these last free object-pool slots.
#define PARTICLE_SLOT_RESERVE 30
// Cap bursts whose full request would leave fewer than this many free slots.
#define PARTICLE_SOFT_RESERVE 72

// Reset alongside the whole object pool; does not allocate or free objects itself.
void particle_budget_reset(void);
// Allocator hooks: exactly one notification per successful allocation/free.
void particle_budget_allocated(struct Object *obj);
void particle_budget_freed(struct Object *obj);
// Counts actual free slots; deactivated objects occupy their slots until freed.
s32 particle_budget_free_slots(void);
enum ParticlePriority particle_budget_priority(const BehaviorScript *behavior);
// Record only reviewed behavior priorities; REQUIRED uses the behavior's default.
void particle_budget_init_priority(struct Object *obj, enum ParticlePriority requested);
// PROTECTED/REQUIRED or unreviewed behaviors do not change an existing priority.
void particle_budget_set_priority(struct Object *obj, enum ParticlePriority priority);
// Query only: returns a reclaimable pool object without freeing it, or NULL.
// The current object and the supplied parent are never candidates.
// Order: inactive, lower priority, older birth, then lower pool index.
struct Object *particle_budget_select(enum ParticlePriority incomingPriority, struct Object *parent);
// Returns an allowed object count in [0, requested] for positive requests, otherwise zero.
// Count cap only: does not allocate objects or reserve slots.
s32 particle_budget_burst_count(enum ParticlePriority priority, s32 requested);
// Sets a valid parent's mask and tracks Mario ownership; does not assign obj->parentObj.
void particle_budget_own_flags(struct Object *obj, struct Object *parent, u32 flagMask);
// Clear only bits still owned by this child (or unowned bits) on Mario; other parents clear directly.
void particle_budget_clear_parent_flags(struct Object *obj, u32 flagMask);
// fieldIndex is a valid rawData.asU32 index; TRUE means active-particle cleanup was handled.
// FALSE leaves generic parent-field handling to the caller.
s32 particle_budget_clear_parent_field(struct Object *obj, u8 fieldIndex, u32 flagMask);

#endif
