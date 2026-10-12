#ifndef SPAWN_OBJECT_H
#define SPAWN_OBJECT_H

#include "types.h"
#include "particle_budget.h"

void init_free_object_list(void);
void clear_object_lists(struct ObjectNode *objLists);
void unload_object(struct Object *obj);
// Required creators reclaim when full; exhaustion with no eligible victim does not return.
struct Object *create_object(const BehaviorScript *bhvScript);

// Required creation protects parent from reclamation; parent linkage is initialized by the caller.
struct Object *create_object_for_parent(const BehaviorScript *bhvScript, struct Object *parent);
// NULL for invalid optional priorities or no reclaimable capacity; parent is protected.
struct Object *try_create_particle(const BehaviorScript *bhvScript, struct Object *parent,
                                   enum ParticlePriority priority);

#endif // SPAWN_OBJECT_H
