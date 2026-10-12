#ifndef BOWSER_ARENA_H
#define BOWSER_ARENA_H

#ifdef VERSION_EU
#define BOWSER_ARENA_FADE_FRAMES 25
#else
#define BOWSER_ARENA_FADE_FRAMES 30
#endif
#include "types.h"
void bowser_arena_bomb_destroyed(struct Object *bomb);
void bowser_arena_platform_destroyed(struct Object *bowser);
// Advance the encounter when a bomb-hit landing reveals a new health total.
void bowser_arena_landed(struct Object *bowser);
// Called once per free Bowser update; owns the recurring enemy spawn clock.
void bowser_arena_update(struct Object *bowser);
// Call once per object update with obj == gCurrentObject. TRUE skips the normal
// behavior update; this function may initialize behavior before advancing the fade.
s32 bowser_arena_phase_in(struct Object *obj);
// Safe before and after movement: latches attack knockback until a safe landing.
void bowser_arena_recover_enemy(struct Object *obj);
// TRUE means arena coin policy handled the object, including a no-coin outcome.
// Repeated calls do not reroll the object's chance.
s32 bowser_arena_try_drop_coin(struct Object *obj);
void *bowser_arena_actor_asset(const BehaviorScript *behavior, const void *asset);
ModelID32 bowser_arena_actor_model(const BehaviorScript *behavior, ModelID32 model);
#define BOWSER_ARENA_MODEL_BULLY 0x20
#define BOWSER_ARENA_MODEL_LAKITU 0x21
#define BOWSER_ARENA_MODEL_SPINY_BALL 0x22
#define BOWSER_ARENA_MODEL_SPINY 0x23
#define BOWSER_ARENA_MODEL_MR_I 0x24
#define BOWSER_ARENA_MODEL_MR_I_IRIS 0x25
#define BOWSER_ARENA_MODEL_BOO 0x26
#define BOWSER_ARENA_MODEL_SNUFIT 0x27
#endif
