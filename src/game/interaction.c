#include "player_stats.h"
#include <PR/ultratypes.h>

#include "area.h"
#include "actors/common1.h"
#include "audio/external.h"
#include "behavior_actions.h"
#include "behavior_data.h"
#include "camera.h"
#include "course_table.h"
#include "dialog_ids.h"
#include "engine/math_util.h"
#include "engine/surface_collision.h"
#include "game_init.h"
#include "game_options.h"
#include "game_options_runtime.h"
#include "game_timing_runtime.h"
#include "hud.h"
#include "interaction.h"
#include "jrb_simpleflips.h"
#include "level_table.h"
#include "level_update.h"
#include "mario.h"
#include "mario_dab.h"
#include "mario_he_thicc.h"
#include "mario_actions_airborne.h"
#include "mario_actions_moving.h"
#include "mario_actions_object.h"
#include "mario_actions_submerged.h"
#include "mario_punch.h"
#include "mario_step.h"
#include "memory.h"
#include "obj_behaviors.h"
#include "object_constants.h"
#include "object_helpers.h"
#include "object_list_processor.h"
#include "save_file.h"
#include "seq_ids.h"
#include "sm64.h"
#include "castle_lobby_finale.h"
#include "sound_init.h"
#include "rumble_init.h"
#include "config.h"

u8  sDelayInvincTimer;
s16 sInvulnerable;
static void reset_mario_pitch(struct MarioState *m);
u32 interact_coin          (struct MarioState *m, u32 interactType, struct Object *obj);
u32 interact_water_ring    (struct MarioState *m, u32 interactType, struct Object *obj);
u32 interact_star_or_key   (struct MarioState *m, u32 interactType, struct Object *obj);
u32 interact_bbh_entrance  (struct MarioState *m, u32 interactType, struct Object *obj);
u32 interact_warp          (struct MarioState *m, u32 interactType, struct Object *obj);
u32 interact_warp_door     (struct MarioState *m, u32 interactType, struct Object *obj);
u32 interact_door          (struct MarioState *m, u32 interactType, struct Object *obj);
u32 interact_cannon_base   (struct MarioState *m, u32 interactType, struct Object *obj);
u32 interact_igloo_barrier (struct MarioState *m, u32 interactType, struct Object *obj);
u32 interact_tornado       (struct MarioState *m, u32 interactType, struct Object *obj);
u32 interact_whirlpool     (struct MarioState *m, u32 interactType, struct Object *obj);
u32 interact_strong_wind   (struct MarioState *m, u32 interactType, struct Object *obj);
u32 interact_flame         (struct MarioState *m, u32 interactType, struct Object *obj);
u32 interact_snufit_bullet (struct MarioState *m, u32 interactType, struct Object *obj);
u32 interact_clam_or_bubba (struct MarioState *m, u32 interactType, struct Object *obj);
u32 interact_bully         (struct MarioState *m, u32 interactType, struct Object *obj);
u32 interact_shock         (struct MarioState *m, u32 interactType, struct Object *obj);
u32 interact_mr_blizzard   (struct MarioState *m, u32 interactType, struct Object *obj);
u32 interact_hit_from_below(struct MarioState *m, u32 interactType, struct Object *obj);
u32 interact_bounce_top    (struct MarioState *m, u32 interactType, struct Object *obj);
u32 interact_spiny_walking (struct MarioState *m, u32 interactType, struct Object *obj);
u32 interact_damage        (struct MarioState *m, u32 interactType, struct Object *obj);
u32 interact_breakable     (struct MarioState *m, u32 interactType, struct Object *obj);
u32 interact_koopa_shell   (struct MarioState *m, u32 interactType, struct Object *obj);
u32 interact_pole          (struct MarioState *m, u32 interactType, struct Object *obj);
u32 interact_hoot          (struct MarioState *m, u32 interactType, struct Object *obj);
u32 interact_cap           (struct MarioState *m, u32 interactType, struct Object *obj);
u32 interact_grabbable     (struct MarioState *m, u32 interactType, struct Object *obj);
u32 interact_text          (struct MarioState *m, u32 interactType, struct Object *obj);

struct InteractionHandler {
    u32 interactType;
    u32 (*handler)(struct MarioState *m, u32 interactType, struct Object *obj);
};

static struct InteractionHandler sInteractionHandlers[] = {
    { INTERACT_COIN,           interact_coin },
    { INTERACT_WATER_RING,     interact_water_ring },
    { INTERACT_STAR_OR_KEY,    interact_star_or_key },
    { INTERACT_BBH_ENTRANCE,   interact_bbh_entrance },
    { INTERACT_WARP,           interact_warp },
    { INTERACT_WARP_DOOR,      interact_warp_door },
    { INTERACT_DOOR,           interact_door },
    { INTERACT_CANNON_BASE,    interact_cannon_base },
    { INTERACT_IGLOO_BARRIER,  interact_igloo_barrier },
    { INTERACT_TORNADO,        interact_tornado },
    { INTERACT_WHIRLPOOL,      interact_whirlpool },
    { INTERACT_STRONG_WIND,    interact_strong_wind },
    { INTERACT_FLAME,          interact_flame },
    { INTERACT_SNUFIT_BULLET,  interact_snufit_bullet },
    { INTERACT_CLAM_OR_BUBBA,  interact_clam_or_bubba },
    { INTERACT_BULLY,          interact_bully },
    { INTERACT_SHOCK,          interact_shock },
    { INTERACT_BOUNCE_TOP2,    interact_bounce_top },
    { INTERACT_MR_BLIZZARD,    interact_mr_blizzard },
    { INTERACT_HIT_FROM_BELOW, interact_hit_from_below },
    { INTERACT_BOUNCE_TOP,     interact_bounce_top },
    { INTERACT_DAMAGE,         interact_damage },
    { INTERACT_POLE,           interact_pole },
    { INTERACT_HOOT,           interact_hoot },
    { INTERACT_BREAKABLE,      interact_breakable },
    { INTERACT_KOOPA,          interact_bounce_top },
    { INTERACT_KOOPA_SHELL,    interact_koopa_shell },
    { INTERACT_SPINY_WALKING,  interact_spiny_walking },
    { INTERACT_CAP,            interact_cap },
    { INTERACT_GRABBABLE,      interact_grabbable },
    { INTERACT_TEXT,           interact_text },
};

static u32 sForwardKnockbackActions[][3] = {
//    Soft                        Normal                 Hard
    { ACT_SOFT_FORWARD_GROUND_KB, ACT_FORWARD_GROUND_KB, ACT_HARD_FORWARD_GROUND_KB }, // Ground
    { ACT_FORWARD_AIR_KB,         ACT_FORWARD_AIR_KB,    ACT_HARD_FORWARD_AIR_KB    }, // Air
    { ACT_FORWARD_WATER_KB,       ACT_FORWARD_WATER_KB,  ACT_FORWARD_WATER_KB       }, // Water
};

static u32 sBackwardKnockbackActions[][3] = {
//    Soft                         Normal                  Hard
    { ACT_SOFT_BACKWARD_GROUND_KB, ACT_BACKWARD_GROUND_KB, ACT_HARD_BACKWARD_GROUND_KB }, // Ground
    { ACT_BACKWARD_AIR_KB,         ACT_BACKWARD_AIR_KB,    ACT_HARD_BACKWARD_AIR_KB    }, // Air
    { ACT_BACKWARD_WATER_KB,       ACT_BACKWARD_WATER_KB,  ACT_BACKWARD_WATER_KB       }, // Water
};

static u8 sDisplayingDoorText = FALSE;
static u8 sJustTeleported = FALSE;
static u8 sPssSlideStarted = FALSE;

#define MAX_PUNCH_HIT_OBJECTS 32
#define MAX_PUNCH_HIT_WALLS 32
#define DIVE_HIT_PHASE 5
#define BULLY_CONTACT_HIT_PHASE 6
#define PUNCH_WALL_MIN_REBOUND_SPEED 16.0f

struct PunchWallContact {
    struct Object *object;
    f32 normalX, normalY, normalZ, originOffset;
};

static struct {
    s32 phase;
    s32 hitObjectCount;
    s32 hitWallCount;
    s32 attackImpactDone;
    u32 lastAction;
    u16 lastActionTimer;
    struct Object *hitObjects[MAX_PUNCH_HIT_OBJECTS];
    struct PunchWallContact hitWalls[MAX_PUNCH_HIT_WALLS];
} sPunchContacts;
static struct Object *sKnockbackSourceObj;

void mario_reset_knockback_source(void) {
    sKnockbackSourceObj = NULL;
}

void mario_set_punch_hit_phase(s32 phase) {
    if (phase != sPunchContacts.phase) {
        sPunchContacts.phase = phase;
        sPunchContacts.hitObjectCount = 0;
        sPunchContacts.hitWallCount = 0;
        sPunchContacts.attackImpactDone = FALSE;
    }
}

static s32 bully_attack_hit_phase(struct MarioState *m) {
    switch (m->action) {
        case ACT_SLIDE_KICK:
        case ACT_SLIDE_KICK_SLIDE:
            return ACT_SLIDE_KICK;
        case ACT_GROUND_POUND:
        case ACT_GROUND_POUND_LAND:
            return ACT_GROUND_POUND;
        case ACT_TWIRLING:
        case ACT_TWIRL_LAND:
            return ACT_TWIRLING;
        case ACT_JUMP_KICK:
            return ACT_JUMP_KICK;
        case ACT_WATER_DOWN_BURST:
            return ACT_WATER_DOWN_BURST;
    }
    if (m->waterGroundPoundImpact) {
        return ACT_WATER_DOWN_BURST;
    }
    if (m->metalFallGroundPoundImpact) {
        return ACT_GROUND_POUND;
    }
    // Shell riding, fast movement and Metal body contact have no strike end.
    return ((m->action & ACT_FLAG_ATTACKING) || (m->flags & MARIO_METAL_CAP)) ? BULLY_CONTACT_HIT_PHASE
                                                                              : 0;
}

static void clear_separated_bully_hits(struct MarioState *m) {
    for (s32 i = 0; i < sPunchContacts.hitObjectCount;) {
        struct Object *target = sPunchContacts.hitObjects[i];
        s32 touching = FALSE;
        for (s32 j = 0; j < m->marioObj->numCollidedObjs; j++) {
            if (m->marioObj->collidedObjs[j] == target) {
                touching = TRUE;
                break;
            }
        }
        if (!touching) {
            sPunchContacts.hitObjects[i] = sPunchContacts.hitObjects[--sPunchContacts.hitObjectCount];
        } else {
            i++;
        }
    }
}

static void update_attack_hit_phase(struct MarioState *m) {
    if (m->action == ACT_DIVE || m->action == ACT_DIVE_SLIDE) {
        // A new dive gets fresh records; its landing slide is the same attack.
        if (m->action == ACT_DIVE && m->actionState == 0) {
            mario_set_punch_hit_phase(0);
        }
        mario_set_punch_hit_phase(DIVE_HIT_PHASE);
    } else if (m->action != ACT_PUNCHING && m->action != ACT_MOVE_PUNCHING
               && !(m->flags & MARIO_PUNCHING)) {
        s32 phase = bully_attack_hit_phase(m);
        // Restarting the same move is a new strike; landing is still the old one.
        if (phase != BULLY_CONTACT_HIT_PHASE && m->action == sPunchContacts.lastAction
            && m->actionTimer < sPunchContacts.lastActionTimer) {
            mario_set_punch_hit_phase(0);
        }
        mario_set_punch_hit_phase(phase);
        if (phase == BULLY_CONTACT_HIT_PHASE) {
            clear_separated_bully_hits(m);
        }
    }
    sPunchContacts.lastAction = m->action;
    sPunchContacts.lastActionTimer = m->actionTimer;
}

void mario_reset_attack_hit_record(void) {
    sPunchContacts.phase = 0;
    sPunchContacts.hitObjectCount = 0;
    sPunchContacts.hitWallCount = 0;
    sPunchContacts.attackImpactDone = FALSE;
}

static void he_thicc_attack_contact(struct MarioState *m, u32 interaction) {
    // The shared hit record lasts through the whole strike, including multiple
    // targets, wall triangles and jump-kick recovery. New strikes reset it.
    if ((interaction & (INT_PUNCH | INT_KICK)) && !sPunchContacts.attackImpactDone) {
        sPunchContacts.attackImpactDone = TRUE;
        mario_he_thicc_attack_impact(m);
    }
}

static s32 punch_hit_object(struct Object *obj) {
    s32 i;
    for (i = 0; i < sPunchContacts.hitObjectCount; i++) {
        if (sPunchContacts.hitObjects[i] == obj) {
            return TRUE;
        }
    }
    return sPunchContacts.hitObjectCount == MAX_PUNCH_HIT_OBJECTS;
}

static void remember_punch_hit_object(struct Object *obj) {
    if (sPunchContacts.hitObjectCount < MAX_PUNCH_HIT_OBJECTS) {
        sPunchContacts.hitObjects[sPunchContacts.hitObjectCount++] = obj;
    }
}

static s32 same_punch_wall(struct PunchWallContact *a, struct Surface *b) {
    if (a->object != NULL && a->object == b->object) {
        return TRUE;
    }
    // A static wall can be split into several coplanar triangles.
    return a->object == NULL && b->object == NULL && absf(a->normalX - b->normal.x) < 0.01f
           && absf(a->normalY - b->normal.y) < 0.01f && absf(a->normalZ - b->normal.z) < 0.01f
           && absf(a->originOffset - b->originOffset) < 1.0f;
}

static void remember_punch_hit_wall(struct Surface *wall) {
    // Dynamic surface slots are rebuilt each update; retain their provider and plane.
    struct PunchWallContact *contact = &sPunchContacts.hitWalls[sPunchContacts.hitWallCount++];
    contact->object = wall->object;
    contact->normalX = wall->normal.x;
    contact->normalY = wall->normal.y;
    contact->normalZ = wall->normal.z;
    contact->originOffset = wall->originOffset;
}

void mario_forget_attack_object(struct Object *obj) {
    // An unloaded pool slot may become a different target during the same attack.
    for (s32 i = 0; i < sPunchContacts.hitObjectCount;) {
        if (sPunchContacts.hitObjects[i] == obj) {
            sPunchContacts.hitObjects[i] = sPunchContacts.hitObjects[--sPunchContacts.hitObjectCount];
        } else {
            i++;
        }
    }
    for (s32 i = 0; i < sPunchContacts.hitWallCount;) {
        if (sPunchContacts.hitWalls[i].object == obj) {
            sPunchContacts.hitWalls[i] = sPunchContacts.hitWalls[--sPunchContacts.hitWallCount];
        } else {
            i++;
        }
    }
}

static s32 punch_hit_wall(struct Surface *wall) {
    s32 i;
    for (i = 0; i < sPunchContacts.hitWallCount; i++) {
        if (same_punch_wall(&sPunchContacts.hitWalls[i], wall)) {
            return TRUE;
        }
    }
    return FALSE;
}

#define GROUND_TWIRL_REBOUND_FLASH_FRAMES                                                              \
    30 // Standard post-knockback invulnerability after enemy contact.
#define GROUND_TWIRL_REBOUND_HIDDEN_FRAMES 8 // Unflashed protection after the rebound ends.
#define GROUND_TWIRL_LARGE_ENEMY_KNOCKBACK_FACTOR 0.5f
#define GROUND_TWIRL_SMALL_BULLY_KNOCKBACK_CAP 16.0f

static s32 mario_ground_twirl_rebound_touches_enemy(struct MarioState *m);

static struct {
    s16 reboundHiddenTimer;
    s32 reboundActive;
    s32 reboundTouchedEnemy;
    s32 caplessContactDamaged;
} sTwirlContact;

void mario_ground_twirl_reset_contact_damage(void) {
    sTwirlContact.caplessContactDamaged = FALSE;
}

void mario_ground_twirl_reset_rebound_invulnerability(void) {
    sTwirlContact.reboundHiddenTimer = 0;
    sTwirlContact.reboundActive = FALSE;
    sTwirlContact.reboundTouchedEnemy = FALSE;
}

/**
 * Returns the type of cap Mario is wearing.
 */
u32 get_mario_cap_flag(struct Object *capObject) {
    const BehaviorScript *script = virtual_to_segmented(SEGMENT_BEHAVIOR_DATA, capObject->behavior);

    if (script == bhvNormalCap) {
        return MARIO_NORMAL_CAP;
    } else if (script == bhvMetalCap) {
        return MARIO_METAL_CAP;
    } else if (script == bhvWingCap) {
        return MARIO_WING_CAP;
    } else if (script == bhvVanishCap) {
        return MARIO_VANISH_CAP;
    }

    return 0;
}

/**
 * Returns true if the passed in object has a moving angle yaw
 * in the angular range given towards Mario.
 */
u32 object_facing_mario(struct MarioState *m, struct Object *obj, s16 angleRange) {
    f32 dx = m->pos[0] - obj->oPosX;
    f32 dz = m->pos[2] - obj->oPosZ;

    s16 angleToMario = atan2s(dz, dx);
    s16 dAngle = angleToMario - obj->oMoveAngleYaw;

    if (-angleRange <= dAngle && dAngle <= angleRange) {
        return TRUE;
    }

    return FALSE;
}

s16 mario_obj_angle_to_object(struct MarioState *m, struct Object *obj) {
    f32 dx = obj->oPosX - m->pos[0];
    f32 dz = obj->oPosZ - m->pos[2];

    return atan2s(dz, dx);
}

/**
 * Determines Mario's interaction with a given object depending on their proximity,
 * action, speed, and position.
 */
u32 determine_interaction(struct MarioState *m, struct Object *obj) {
    if (m->metalFallGroundPoundImpact) {
        return INT_GROUND_POUND_OR_TWIRL;
    }
    u32 interaction = 0;
    u32 action = m->action;
    s32 waterPoundImpact =
        m->waterGroundPoundImpact && (action == ACT_FLUTTER_KICK || action == ACT_WATER_ACTION_END);

    if ((action & ACT_FLAG_ATTACKING) || action == ACT_WATER_DOWN_BURST || waterPoundImpact) {
        if (action == ACT_PUNCHING || action == ACT_MOVE_PUNCHING || action == ACT_JUMP_KICK) {
            s16 dYawToObject = mario_obj_angle_to_object(m, obj) - m->faceAngle[1];

            if (m->flags & MARIO_PUNCHING) {
                // 120 degrees total, or 60 each way
                if (-0x2AAA <= dYawToObject && dYawToObject <= 0x2AAA) {
                    interaction = INT_PUNCH;
                }
            }
            if (m->flags & MARIO_KICKING) {
                // 120 degrees total, or 60 each way
                if (-0x2AAA <= dYawToObject && dYawToObject <= 0x2AAA) {
                    interaction = INT_KICK;
                }
            }
            if (m->flags & MARIO_TRIPPING) {
                // Breakdance trips surround Mario; other trips retain their front cone.
                if (((action == ACT_PUNCHING || action == ACT_MOVE_PUNCHING)
                     && m->actionArg == ACT_ARG_PUNCH_SEQUENCE_BREAKDANCE)
                    || (-0x4000 <= dYawToObject && dYawToObject <= 0x4000)) {
                    interaction = INT_TRIP;
                }
            }
        } else if (action == ACT_GROUND_POUND || action == ACT_TWIRLING
                   || action == ACT_WATER_DOWN_BURST) {
            if (m->vel[1] < 0.0f
                && (action != ACT_WATER_DOWN_BURST
                    || m->actionState == ACT_STATE_WATER_DOWN_BURST_SINK)) {
                interaction = INT_GROUND_POUND_OR_TWIRL;
            }
        } else if (waterPoundImpact) {
            interaction = INT_GROUND_POUND_OR_TWIRL;
        } else if (action == ACT_GROUND_POUND_LAND || action == ACT_TWIRL_LAND) {
            // Neither ground pounding nor twirling change Mario's vertical speed on landing.,
            // so the speed check is nearly always true (perhaps not if you land while going upwards?)
            // Additionally, actionState it set on each first thing in their action, so this is
            // only true prior to the very first frame (i.e. active 1 frame prior to it run).
            if (m->vel[1] < 0.0f && m->actionState == 0) {
                interaction = INT_GROUND_POUND_OR_TWIRL;
            }
        } else if (action == ACT_SLIDE_KICK) {
            // The launch is vulnerable before the airborne kick can hit or stomp.
            if (m->actionState == ACT_STATE_SLIDE_KICK_SLIDING
                && m->actionTimer < SLIDE_KICK_STARTUP_VULNERABLE_FRAMES) {
                return INT_NONE;
            }
            // A descending slide kick can stomp enemies from above like an airborne dive.
            interaction =
                (m->vel[1] < 0.0f && m->pos[1] > obj->oPosY) ? INT_HIT_FROM_ABOVE : INT_SLIDE_KICK;
        } else if (action == ACT_SLIDE_KICK_SLIDE) {
            interaction = INT_SLIDE_KICK;
        } else if (action == ACT_DIVE) {
            // Both ground-launched and airborne dives are vulnerable during startup.
            if (m->actionState < DIVE_STARTUP_VULNERABLE_FRAMES) {
                return INT_NONE;
            }
            // Landing on an enemy keeps the normal stomp and bounce behavior.
            interaction = (m->vel[1] < 0.0f && m->pos[1] > obj->oPosY) ? INT_HIT_FROM_ABOVE
                                                                       : INT_FAST_ATTACK_OR_SHELL;
        } else if (action == ACT_DIVE_SLIDE) {
            if (m->actionTimer < DIVE_SLIDE_ATTACK_FRAMES) {
                interaction = INT_FAST_ATTACK_OR_SHELL;
            }
        } else if (action & ACT_FLAG_RIDING_SHELL) {
            interaction = INT_FAST_ATTACK_OR_SHELL;
        } else if (m->forwardVel <= -26.0f || 26.0f <= m->forwardVel) {
            interaction = INT_FAST_ATTACK_OR_SHELL;
        }
    }

    // Prior to this, the interaction type could be overwritten. This requires, however,
    // that the interaction not be set prior. This specifically overrides turning a ground
    // pound into just a bounce.
    if ((interaction == INT_NONE) && (action & ACT_FLAG_AIR)) {
        if (m->vel[1] < 0.0f) {
            if (m->pos[1] > obj->oPosY) {
                interaction = INT_HIT_FROM_ABOVE;
            }
        } else {
            if (m->pos[1] < obj->oPosY) {
                interaction = INT_HIT_FROM_BELOW;
            }
        }
    }

    // A punch, ground kick or breakdance cannot hit an object twice in one phase,
    // but the object must still be able to collide with Mario.
    if (sPunchContacts.phase != 0 && (action == ACT_PUNCHING || action == ACT_MOVE_PUNCHING)
        && (interaction == INT_PUNCH
            || (m->actionArg == ACT_ARG_PUNCH_SEQUENCE_GROUND_KICK && interaction == INT_KICK)
            || (m->actionArg == ACT_ARG_PUNCH_SEQUENCE_BREAKDANCE && interaction == INT_TRIP))
        && punch_hit_object(obj)) {
        return INT_NONE;
    }

    return interaction;
}

/**
 * Sets the interaction types for INT_STATUS_INTERACTED, INT_STATUS_WAS_ATTACKED
 */
u32 attack_object(struct Object *obj, s32 interaction) {
    u32 attackType = ATTACK_NONE;

    switch (interaction) {
        case INT_GROUND_POUND_OR_TWIRL:
            attackType = ATTACK_GROUND_POUND_OR_TWIRL;
            break;
        case INT_PUNCH:
            attackType = ATTACK_PUNCH;
            break;
        case INT_KICK:
        case INT_TRIP:
            attackType = ATTACK_KICK_OR_TRIP;
            break;
        case INT_SLIDE_KICK:
        case INT_FAST_ATTACK_OR_SHELL:
            attackType = ATTACK_FAST_ATTACK;
            break;
        case INT_HIT_FROM_ABOVE:
            attackType = ATTACK_FROM_ABOVE;
            break;
        case INT_HIT_FROM_BELOW:
            attackType = ATTACK_FROM_BELOW;
            break;
    }

    player_stats_attack(obj, attackType);
    obj->oInteractStatus = attackType + (INT_STATUS_INTERACTED | INT_STATUS_WAS_ATTACKED);
    return attackType;
}

void mario_stop_riding_object(struct MarioState *m) {
    if (m->riddenObj != NULL) {
        m->riddenObj->oInteractStatus = INT_STATUS_STOP_RIDING;
        stop_shell_music();
        m->riddenObj = NULL;
    }
}

void mario_forget_pickup_object(struct Object *obj) {
    // A deferred pickup must not acquire a new occupant of this pool slot.
    if (gMarioState != NULL && gMarioState->action == ACT_PICKING_UP && gMarioState->heldObj == NULL
        && gMarioState->usedObj == obj) {
        gMarioState->usedObj = NULL;
    }
}

void mario_grab_used_object(struct MarioState *m) {
    if (m->heldObj == NULL) {
        m->heldObj = m->usedObj;
        player_stats_grab(m->heldObj);
        obj_set_held_state(m->heldObj, bhvCarrySomethingHeld);
        m->heldObj->oGroundTwirlStunTimer = 0;
        m->heldObj->oGroundTwirlContactRebound = 0;
    }
}

void mario_drop_held_object(struct MarioState *m) {
    if (m->heldObj != NULL) {
        if (m->heldObj->behavior == segmented_to_virtual(bhvKoopaShellUnderwater)) {
            stop_shell_music();
        }

        obj_set_held_state(m->heldObj, bhvCarrySomethingDropped);

        // ! When dropping an object instead of throwing it, it will be put at Mario's
        // y-positon instead of the HOLP's y-position. This fact is often exploited when
        // cloning objects.
        m->heldObj->oPosX = m->marioBodyState->heldObjLastPosition[0];
        // m->heldObj->oPosY = m->marioBodyState->heldObjLastPosition[1];
        m->heldObj->oPosY = m->pos[1];
        m->heldObj->oPosZ = m->marioBodyState->heldObjLastPosition[2];

        m->heldObj->oMoveAngleYaw = m->faceAngle[1];

        m->heldObj = NULL;
    }
}

void mario_throw_held_object(struct MarioState *m) {
    if (m->heldObj != NULL) {
        if (m->heldObj->behavior == segmented_to_virtual(bhvKoopaShellUnderwater)) {
            stop_shell_music();
        }

        // Ground throws saved their entry speed; other throws use release speed.
        if (m->action != ACT_THROWING) {
            m->heldObj->oForwardVel = MAX(0.0f, m->forwardVel);
        }
        player_stats_throw(m->heldObj);
        obj_set_held_state(m->heldObj, bhvCarrySomethingThrown);

        m->heldObj->oPosX = m->marioBodyState->heldObjLastPosition[0] + 32.0f * sins(m->faceAngle[1]);
        m->heldObj->oPosY = m->marioBodyState->heldObjLastPosition[1];
        m->heldObj->oPosZ = m->marioBodyState->heldObjLastPosition[2] + 32.0f * coss(m->faceAngle[1]);

        m->heldObj->oMoveAngleYaw = m->faceAngle[1];

        m->heldObj = NULL;
    }
}

#define SMALL_OBJECT_THROW_SPEED_SCALE 1.1025f

f32 mario_small_object_throw_speed(struct Object *obj, f32 vanillaSpeed) {
    // The held object carries Mario's saved speed until its thrown behavior starts.
    return (vanillaSpeed + MAX(0.0f, obj->oForwardVel) * 0.5f) * SMALL_OBJECT_THROW_SPEED_SCALE;
}

void mario_stop_riding_and_holding(struct MarioState *m) {
    mario_drop_held_object(m);
    mario_stop_riding_object(m);

    if (m->action == ACT_RIDING_HOOT) {
        m->usedObj->oInteractStatus = FALSE;
        m->usedObj->oHootMarioReleaseTime = gGlobalTimer;
    }
}

u32 does_mario_have_normal_cap_on_head(struct MarioState *m) {
    return (m->flags & (MARIO_CAPS | MARIO_CAP_ON_HEAD)) == (MARIO_NORMAL_CAP | MARIO_CAP_ON_HEAD);
}

u32 mario_can_lose_cap_to_enemy(struct MarioState *m) {
    return game_options_get(GO_CAP_LOSS)
           && (m->flags & (MARIO_NORMAL_CAP | MARIO_CAP_ON_HEAD))
                  == (MARIO_NORMAL_CAP | MARIO_CAP_ON_HEAD);
}

void mario_remove_cap_powers(struct MarioState *m) {
    if (mario_natural_cap_powers(m)) {
        mario_clear_cap_flags(m);
        m->capTimer = 0;
        stop_cap_music();
    }
}

s32 mario_pose_exposes_cap_to_damage(struct MarioState *m) {
    return mario_ground_twirl_loses_cap_on_damage(m) || m->action == ACT_START_SLEEPING
           || m->action == ACT_SLEEPING;
}

#define MARIO_CAP_PICKUP_LOCK_FRAMES 30

#ifdef PREVENT_CAP_LOSS
UNUSED void mario_blow_off_cap(UNUSED struct MarioState *m, UNUSED f32 capSpeed) {
}

Bool32 mario_lose_cap_to_enemy(UNUSED u32 enemyType) {
    return FALSE;
}
#else
static void mario_blow_off_cap_in_direction(struct MarioState *m, f32 capSpeed, s16 yaw) {
    struct Object *capObject;

    if (game_options_get(GO_CAP_LOSS) && does_mario_have_normal_cap_on_head(m)) {
        save_file_set_cap_pos(m->pos[0], m->pos[1], m->pos[2]);

        player_stats_cap_lost();
        m->flags &= ~(MARIO_NORMAL_CAP | MARIO_CAP_ON_HEAD);
        m->capPickupLockFrames = MARIO_CAP_PICKUP_LOCK_FRAMES;
        m->capPickupLockStartFrame = gGlobalTimer;

        capObject = spawn_object(m->marioObj, MODEL_MARIOS_CAP, bhvNormalCap);

        capObject->oPosY += (m->action & ACT_FLAG_SHORT_HITBOX) ? 120.0f : 180.0f;
        capObject->oForwardVel = capSpeed;
        capObject->oMoveAngleYaw = yaw;
    }
}

void mario_blow_off_cap(struct MarioState *m, f32 capSpeed) {
    mario_blow_off_cap_in_direction(m, capSpeed, (s16) (m->faceAngle[1] + 0x8000));
}

Bool32 mario_lose_cap_to_enemy(u32 enemyType) {
    if (mario_can_lose_cap_to_enemy(gMarioState)) {
        save_file_set_flags(enemyType == 1 ? SAVE_FLAG_CAP_ON_KLEPTO : SAVE_FLAG_CAP_ON_UKIKI);
        mario_remove_cap_powers(gMarioState);
        player_stats_cap_lost();
        gMarioState->flags &= ~(MARIO_NORMAL_CAP | MARIO_CAP_ON_HEAD);
        gMarioState->capPickupLockFrames = MARIO_CAP_PICKUP_LOCK_FRAMES;
        gMarioState->capPickupLockStartFrame = gGlobalTimer;
        return TRUE;
    }

    return FALSE;
}
#endif

void mario_blow_off_cap_from_wind(struct MarioState *m, f32 capSpeed) {
    if (game_options_cheat_active(GO_IGNORE_WIND)) {
        return;
    }
#ifndef PREVENT_CAP_LOSS
    // Wing and Metal retain wind protection; Vanish no longer protects the hat.
    if (!(m->flags & (MARIO_WING_CAP | MARIO_METAL_CAP)) && mario_can_lose_cap_to_enemy(m)) {
        mario_remove_cap_powers(m);
        mario_blow_off_cap(m, capSpeed);
    }
#endif
}

void mario_lose_cap_to_shallow_water_death(struct MarioState *m) {
    if (!game_options_get(GO_CAP_LOSS) || !(m->flags & MARIO_CAP_ON_HEAD)) {
        return;
    }

    // Keep the ordinary cap recoverable after the death warp without showing
    // a thrown cap during the sink animation.
    if (m->flags & MARIO_NORMAL_CAP) {
        save_file_set_cap_pos(m->pos[0], m->floorHeight, m->pos[2]);
        m->capPickupLockFrames = MARIO_CAP_PICKUP_LOCK_FRAMES;
        m->capPickupLockStartFrame = gGlobalTimer;
    }
    player_stats_cap_lost();
    m->flags &= ~(MARIO_NORMAL_CAP | MARIO_CAP_ON_HEAD | MARIO_CAP_IN_HAND);
    mario_clear_cap_flags(m);
    if (m->forcedCapFlags) {
        m->flags |= MARIO_CAP_ON_HEAD;
    }
    m->capTimer = 0;
    stop_cap_music();
}

Bool32 mario_retrieve_cap(void) {
    if (!(gMarioState->flags & MARIO_NORMAL_CAP) && gMarioState->capPickupLockFrames != 0) {
        return FALSE;
    }

    if (!(gMarioState->flags & MARIO_NORMAL_CAP)) {
        player_stats_cap_recovered();
    }
    mario_drop_held_object(gMarioState);
    save_file_clear_flags(SAVE_FLAG_CAP_ON_KLEPTO | SAVE_FLAG_CAP_ON_UKIKI);
    gMarioState->flags &= ~MARIO_CAP_ON_HEAD;
    gMarioState->flags |= MARIO_NORMAL_CAP | MARIO_CAP_IN_HAND;
    return TRUE;
}

static s32 mario_bowser_tail_grab_action(struct MarioState *m) {
    if (game_options_get(GO_BOWSER_FIGHTS) == 1) {
        return m->action == ACT_DIVE || m->action == ACT_DIVE_SLIDE
               || ((m->action == ACT_PUNCHING || m->action == ACT_MOVE_PUNCHING) && m->actionArg < 2);
    }
    return mario_is_first_punch(m) || (m->action == ACT_DIVE && m->actionState < 3);
}

static s32 mario_can_grab_bowser_tail(struct MarioState *m, struct Object *bowser) {
    s16 behindDYaw = mario_obj_angle_to_object(m, bowser) - bowser->oMoveAngleYaw;
    s16 facingDYaw = m->faceAngle[1] - bowser->oMoveAngleYaw;
    return mario_bowser_tail_grab_action(m) && bowser->oBowserGrabCooldown == 0
           && bowser->oHeldState == HELD_FREE && bowser->oOpacity >= 100
           && bowser->oAction != BOWSER_ACT_DEAD && bowser->oAction != BOWSER_ACT_THROWN
           && bowser->oAction != BOWSER_ACT_HIT_MINE && bowser->oAction != BOWSER_ACT_JUMP_ONTO_STAGE
           && bowser->oAction != BOWSER_ACT_TILT_LAVA_PLATFORM && bowser->oAction != BOWSER_ACT_TELEPORT
           && (game_options_get(GO_BOWSER_FIGHTS) == 1
                   ? facingDYaw >= -0x5555 && facingDYaw <= 0x5555
                   : behindDYaw >= -0x2AAA && behindDYaw <= 0x2AAA && facingDYaw >= -0x638E
                         && facingDYaw <= 0x638E);
}

static s32 mario_overlaps_grab_hitbox(struct MarioState *m, struct Object *obj) {
    f32 radius = m->marioObj->hitboxRadius + obj->hitboxRadius;
    f32 marioBottom = m->pos[1] - m->marioObj->hitboxDownOffset;
    f32 objectBottom = obj->oPosY - obj->hitboxDownOffset;
    return sqr(m->pos[0] - obj->oPosX) + sqr(m->pos[2] - obj->oPosZ) < sqr(radius)
           && marioBottom <= objectBottom + obj->hitboxHeight
           && marioBottom + m->marioObj->hitboxHeight >= objectBottom;
}

static struct Object *mario_get_bowser_tail_contact(struct MarioState *m) {
    if (!mario_bowser_tail_grab_action(m)) {
        return NULL;
    }
    struct Object *head = (struct Object *) &gObjectLists[OBJ_LIST_GENACTOR];
    for (struct Object *bowser = (struct Object *) head->header.next; bowser != head;
         bowser = (struct Object *) bowser->header.next) {
        if (bowser->activeFlags == ACTIVE_FLAG_DEACTIVATED || !obj_has_behavior(bowser, bhvBowser)) {
            continue;
        }
        struct Object *tail = bowser->prevObj;
        if (tail != NULL && tail->activeFlags != ACTIVE_FLAG_DEACTIVATED && tail->parentObj == bowser
            && obj_has_behavior(tail, bhvBowserTailAnchor) && mario_can_grab_bowser_tail(m, bowser)
            && mario_overlaps_grab_hitbox(m, bowser) && mario_overlaps_grab_hitbox(m, tail)) {
            return bowser;
        }
    }
    return NULL;
}

u32 able_to_grab_object(struct MarioState *m, struct Object *obj) {
    if (obj_has_behavior(obj, bhvBowser)) {
        return mario_can_grab_bowser_tail(m, obj);
    }

    u32 action = m->action;

    if (action == ACT_DIVE_SLIDE || action == ACT_DIVE) {
        if (!(obj->oInteractionSubtype & INT_SUBTYPE_GRABS_MARIO)) {
            return TRUE;
        }
    } else if (mario_is_first_punch(m)) {
        return TRUE;
    }

    return FALSE;
}

struct Object *mario_get_collided_object(struct MarioState *m, u32 interactType) {
    s32 i;
    struct Object *object;

    for (i = 0; i < m->marioObj->numCollidedObjs; i++) {
        object = m->marioObj->collidedObjs[i];

        if (object->oInteractType == interactType) {
            return object;
        }
    }

    return NULL;
}

u32 mario_check_object_grab(struct MarioState *m) {
    u32 result = FALSE;
    const BehaviorScript *script;

    if (m->action == ACT_DIVE && m->actionState < 3) {
        // Dive entry can happen after the interaction pass on this same update.
        struct Object *bowser = mario_get_bowser_tail_contact(m);
        if (bowser != NULL) {
            m->interactObj = bowser;
            m->input |= INPUT_INTERACT_OBJ_GRABBABLE;
        }
    }

    if (m->input & INPUT_INTERACT_OBJ_GRABBABLE) {
        script = virtual_to_segmented(SEGMENT_BEHAVIOR_DATA, m->interactObj->behavior);

        if (script == bhvBowser) {
            if (mario_can_grab_bowser_tail(m, m->interactObj)) {
                m->interactObj->oBowserTargetOpacity = 255;
                m->interactObj->oOpacity = m->interactObj->oBowserTargetOpacity;
                m->faceAngle[1] = m->interactObj->oMoveAngleYaw;
                m->usedObj = m->interactObj;
                m->pos[1] = m->floorHeight;
                m->vel[1] = 0.0f;
                result = set_mario_action(m, ACT_PICKING_UP_BOWSER, 0);
            }
        } else {
            s16 facingDYaw = mario_obj_angle_to_object(m, m->interactObj) - m->faceAngle[1];
            s32 passedDuringPunchLunge = FALSE;
            if (mario_is_first_punch(m) && m->actionState == ACT_STATE_PUNCH_LUNGE_ACTIVE) {
                s16 previousFacingDYaw =
                    atan2s(m->interactObj->oPosZ - m->prevPos[2], m->interactObj->oPosX - m->prevPos[0])
                    - m->faceAngle[1];
                passedDuringPunchLunge = previousFacingDYaw >= -0x2AAA && previousFacingDYaw <= 0x2AAA;
            }
            if ((facingDYaw >= -0x2AAA && facingDYaw <= 0x2AAA) || passedDuringPunchLunge) {
                m->usedObj = m->interactObj;

                if (m->action == ACT_DIVE_SLIDE) {
                    m->forwardVel *= 0.8f;
                    m->slideVelX *= 0.8f;
                    m->slideVelZ *= 0.8f;
                    m->vel[0] *= 0.8f;
                    m->vel[2] *= 0.8f;
                }

                if (!(m->action & ACT_FLAG_AIR)) {
                    set_mario_action(
                        m, (m->action & ACT_FLAG_DIVING) ? ACT_DIVE_PICKING_UP : ACT_PICKING_UP, 0);
                }

                result = TRUE;
            }
        }
    }

    return result;
}

s32 mario_wall_between_object(Vec3f marioPos, struct Object *obj) {
    Vec3f origin;
    Vec3f direction;
    Vec3f hitPos;
    struct Surface *wall;

    vec3f_set(origin, marioPos[0], marioPos[1] + 60.0f, marioPos[2]);
    vec3f_set(direction, obj->oPosX - origin[0],
              CLAMP(origin[1], obj->oPosY, obj->oPosY + obj->hitboxHeight) - origin[1],
              obj->oPosZ - origin[2]);
    if (sqr(direction[0]) + sqr(direction[2]) < 1.0f) {
        return FALSE;
    }

    find_surface_on_ray(origin, direction, &wall, hitPos, RAYCAST_FIND_WALL);
    return wall != NULL;
}

u32 mario_check_punch_lunge_grab(struct MarioState *m) {
    struct Object *nearest = NULL;
    f32 nearestProgress = 2.0f;
    f32 dx = m->pos[0] - m->prevPos[0];
    f32 dz = m->pos[2] - m->prevPos[2];
    f32 pathLengthSquared = dx * dx + dz * dz;
    s32 list;

    if (!mario_is_first_punch(m) || m->actionState != ACT_STATE_PUNCH_LUNGE_ACTIVE
        || !(m->flags & MARIO_PUNCHING)) {
        return FALSE;
    }

    for (list = 0; list < NUM_OBJ_LISTS; list++) {
        struct Object *head = (struct Object *) &gObjectLists[list];
        struct Object *obj = (struct Object *) head->header.next;
        for (; obj != head; obj = (struct Object *) obj->header.next) {
            f32 toObjX, toObjZ, progress, distanceX, distanceZ, marioBottom, marioTop;
            f32 objBottom, objTop, radius;
            s16 facingDYaw;

            if (obj->activeFlags == ACTIVE_FLAG_DEACTIVATED
                || (obj->activeFlags & (ACTIVE_FLAG_FAR_AWAY | ACTIVE_FLAG_IN_DIFFERENT_ROOM))
                || (obj->oRoom > 0 && gMarioCurrentRoom > 0 && obj->oRoom != gMarioCurrentRoom)
                || obj->oIntangibleTimer != 0 || obj->oInteractType != INTERACT_GRABBABLE
                || (obj->oInteractionSubtype & (INT_SUBTYPE_NOT_GRABBABLE | INT_SUBTYPE_GRABS_MARIO))
                || (obj->oInteractStatus & INT_STATUS_INTERACTED)) {
                continue;
            }

            toObjX = obj->oPosX - m->prevPos[0];
            toObjZ = obj->oPosZ - m->prevPos[2];
            facingDYaw = atan2s(toObjZ, toObjX) - m->faceAngle[1];
            if (facingDYaw < -0x2AAA || facingDYaw > 0x2AAA) {
                continue;
            }

            progress = pathLengthSquared > 0.0f
                           ? CLAMP((toObjX * dx + toObjZ * dz) / pathLengthSquared, 0.0f, 1.0f)
                           : 0.0f;
            if (progress >= nearestProgress) {
                continue;
            }
            distanceX = toObjX - progress * dx;
            distanceZ = toObjZ - progress * dz;
            radius = m->marioObj->hitboxRadius + obj->hitboxRadius;
            if (distanceX * distanceX + distanceZ * distanceZ >= radius * radius) {
                continue;
            }

            marioBottom =
                m->prevPos[1] + progress * (m->pos[1] - m->prevPos[1]) - m->marioObj->hitboxDownOffset;
            marioTop = marioBottom + m->marioObj->hitboxHeight;
            objBottom = obj->oPosY - obj->hitboxDownOffset;
            objTop = objBottom + obj->hitboxHeight;
            if (marioBottom > objTop || marioTop < objBottom) {
                continue;
            }
            if (mario_wall_between_object(m->prevPos, obj)) {
                continue;
            }

            nearest = obj;
            nearestProgress = progress;
        }
    }

    if (nearest != NULL) {
        m->interactObj = nearest;
        m->input |= INPUT_INTERACT_OBJ_GRABBABLE;
        if (mario_check_object_grab(m)) {
            return TRUE;
        }
        m->input &= ~INPUT_INTERACT_OBJ_GRABBABLE;
    }
    return FALSE;
}

u32 bully_knock_back_mario(struct MarioState *mario) {
    struct BullyCollisionData marioData;
    struct BullyCollisionData bullyData;
    s16 newMarioYaw;
    s16 newBullyYaw;
    s16 marioDYaw;

    u32 bonkAction = 0;

    struct Object *bully = mario->interactObj;

    //! Conversion ratios multiply to more than 1 (could allow unbounded speed
    // with bonk cancel - but this isn't important for regular bully battery)
    f32 bullyToMarioRatio = bully->hitboxRadius * 3 / 53;
    f32 marioToBullyRatio = 53.0f / bully->hitboxRadius;

    init_bully_collision_data(&marioData, mario->pos[0], mario->pos[2], mario->forwardVel,
                              mario->faceAngle[1], bullyToMarioRatio, 52.0f);

    init_bully_collision_data(&bullyData, bully->oPosX, bully->oPosZ, bully->oForwardVel,
                              bully->oMoveAngleYaw, marioToBullyRatio, bully->hitboxRadius + 2.0f);

    if (mario->forwardVel != 0.0f) {
        transfer_bully_speed(&marioData, &bullyData);
    } else {
        transfer_bully_speed(&bullyData, &marioData);
    }

    newMarioYaw = atan2s(marioData.velZ, marioData.velX);
    newBullyYaw = atan2s(bullyData.velZ, bullyData.velX);

    marioDYaw = newMarioYaw - mario->faceAngle[1];

    mario->faceAngle[1] = newMarioYaw;
    mario->forwardVel = sqrtf(sqr(marioData.velX) + sqr(marioData.velZ));
    mario->pos[0] = marioData.posX;
    mario->pos[2] = marioData.posZ;

    bully->oMoveAngleYaw = newBullyYaw;
    bully->oForwardVel = sqrtf(sqr(bullyData.velX) + sqr(bullyData.velZ));
    bully->oPosX = bullyData.posX;
    bully->oPosZ = bullyData.posZ;

    if (marioDYaw < -0x4000 || marioDYaw > 0x4000) {
        mario->faceAngle[1] += 0x8000;
        mario->forwardVel *= -1.0f;

        if (mario->action & ACT_FLAG_AIR) {
            bonkAction = ACT_BACKWARD_AIR_KB;
        } else {
            bonkAction = ACT_SOFT_BACKWARD_GROUND_KB;
        }
    } else {
        if (mario->action & ACT_FLAG_AIR) {
            bonkAction = ACT_FORWARD_AIR_KB;
        } else {
            bonkAction = ACT_SOFT_FORWARD_GROUND_KB;
        }
    }

    return bonkAction;
}

void bounce_off_object(struct MarioState *m, struct Object *obj, f32 velY) {
    if (m->vel[1] <= -40.0f) {
        mario_he_thicc_contact_shake(m);
    }
    m->pos[1] = obj->oPosY + obj->hitboxHeight;
    player_stats_fall_survived(m);
    player_stats_enemy_bounce(obj);
    m->vel[1] = velY;

    m->flags &= ~MARIO_JUMPING;

    play_sound(SOUND_ACTION_BOUNCE_OFF_OBJECT, m->marioObj->header.gfx.cameraToObject);
}

static void bounce_off_enemy(struct MarioState *m, struct Object *obj, f32 velY) {
    if (m->vel[1] < 0.0f) {
        bounce_off_object(m, obj, velY + ((m->input & INPUT_A_DOWN) ? 12.0f : 0.0f));
    }
}

static s32 jump_after_ground_pound_enemy_hit(struct MarioState *m, struct Object *obj,
                                             u32 interaction) {
    f32 bounceVel;

    if (m->action != ACT_GROUND_POUND || interaction != INT_GROUND_POUND_OR_TWIRL
        || !(m->input & INPUT_A_DOWN) || m->vel[1] >= 0.0f || !mario_ground_twirl_is_enemy(obj)) {
        return FALSE;
    }

    bounceVel = -m->vel[1];
    bounce_off_object(m, obj, bounceVel);
    if (obj->oInteractionSubtype & INT_SUBTYPE_TWIRL_BOUNCE) {
        // Held-A ground-pound rebounds keep the enemy's twirl reward.
        reset_mario_pitch(m);
        play_sound(SOUND_MARIO_TWIRL_BOUNCE, m->marioObj->header.gfx.cameraToObject);
        drop_and_set_mario_action(m, ACT_TWIRLING, 0);
    } else {
        set_mario_action(m, ACT_JUMP, 0);
    }
    m->vel[1] = bounceVel;
    m->groundPoundBounceCooldown = 5;
    m->input &= ~(INPUT_Z_DOWN | INPUT_Z_PRESSED);
    return TRUE;
}

void hit_object_from_below(struct MarioState *m, UNUSED struct Object *obj) {
    m->vel[1] = 0.0f;
    set_camera_shake_from_hit(SHAKE_HIT_FROM_BELOW);
}

UNUSED static u32 unused_determine_knockback_action(struct MarioState *m) {
    u32 bonkAction;
    s16 angleToObject = mario_obj_angle_to_object(m, m->interactObj);
    s16 facingDYaw = angleToObject - m->faceAngle[1];

    if (m->forwardVel < 16.0f) {
        m->forwardVel = 16.0f;
    }

    m->faceAngle[1] = angleToObject;

    if (facingDYaw >= -0x4000 && facingDYaw <= 0x4000) {
        m->forwardVel *= -1.0f;
        if (m->action & (ACT_FLAG_AIR | ACT_FLAG_ON_POLE | ACT_FLAG_HANGING)) {
            bonkAction = ACT_BACKWARD_AIR_KB;
        } else {
            bonkAction = ACT_SOFT_BACKWARD_GROUND_KB;
        }
    } else {
        m->faceAngle[1] += 0x8000;
        if (m->action & (ACT_FLAG_AIR | ACT_FLAG_ON_POLE | ACT_FLAG_HANGING)) {
            bonkAction = ACT_FORWARD_AIR_KB;
        } else {
            bonkAction = ACT_SOFT_FORWARD_GROUND_KB;
        }
    }

    return bonkAction;
}

static void mario_lose_cap_on_lethal_damage_with_speed(struct MarioState *m, UNUSED s16 yaw,
                                                       UNUSED f32 speedScale) {
    if (!game_options_get(GO_CAP_LOSS)) {
        return;
    }
    // Crushing keeps the cap on, including hits on the frame the squish reaction starts.
    if (m->action == ACT_SQUISHED || (m->input & INPUT_SQUISHED)) {
        return;
    }

    // Damage drains over several frames; knock off the cap when queued damage is lethal.
    // Include queued healing so a hit that Mario will survive does not remove his cap.
    if (m->hurtCounter > 0 && m->health >= 0x100 && mario_health_after_pending_damage(m) < 0x100) {
#ifndef PREVENT_CAP_LOSS
        if (m->flags & MARIO_CAP_ON_HEAD) {
            // A lethal hit removes the equipped power-up as well as the regular cap.
            // Normal wind and cap-stealing checks still retain their power-up protection.
            if (m->flags & MARIO_SPECIAL_CAPS) {
                player_stats_cap_powers_removed(mario_natural_cap_powers(m), FALSE);
                mario_remove_cap_powers(m);
            }
            f32 capSpeed = ABS(m->forwardVel) + 5.0f;
            if (capSpeed < 15.0f) {
                capSpeed = 15.0f;
            }
            mario_blow_off_cap_in_direction(m, capSpeed * speedScale, yaw);
        }
#endif
    }
}

void mario_lose_cap_on_lethal_damage(struct MarioState *m, s16 yaw) {
    mario_lose_cap_on_lethal_damage_with_speed(m, yaw, 1.0f);
}

u32 determine_knockback_action(struct MarioState *m, UNUSED s32 arg) {
    u32 bonkAction;
    s16 capYaw;

    s16 terrainIndex = 0; // 1 = air, 2 = water, 0 = default
    s16 strengthIndex = 0;

    s16 angleToObject = mario_obj_angle_to_object(m, m->interactObj);
    s16 facingDYaw = angleToObject - m->faceAngle[1];
    s16 remainingHealth = m->health - 0x100 * (((s32) m->hurtCounter + 3) / 4);

    if (m->action & (ACT_FLAG_SWIMMING | ACT_FLAG_METAL_WATER)) {
        terrainIndex = 2;
    } else if (m->action & (ACT_FLAG_AIR | ACT_FLAG_ON_POLE | ACT_FLAG_HANGING)) {
        terrainIndex = 1;
    }

    if (remainingHealth < 0x100) {
        strengthIndex = 2;
    } else if (mario_has_current_metal_cap(m)) {
        // Ordinary Metal hits keep the light reaction even on high-damage enemies.
        strengthIndex = 0;
    } else if (m->interactObj->oDamageOrCoinValue >= 4) {
        strengthIndex = 2;
    } else if (m->interactObj->oDamageOrCoinValue >= 2) {
        strengthIndex = 1;
    }

    m->faceAngle[1] = angleToObject;

    if (terrainIndex == 2) {
        if (m->forwardVel < 28.0f) {
            mario_set_forward_vel(m, 28.0f);
        }

        if (m->pos[1] >= m->interactObj->oPosY) {
            if (m->vel[1] < 20.0f) {
                m->vel[1] = 20.0f;
            }
        } else {
            if (m->vel[1] > 0.0f) {
                m->vel[1] = 0.0f;
            }
        }
    } else {
        if (m->forwardVel < 16.0f) {
            mario_set_forward_vel(m, 16.0f);
        }
    }

    if (-0x4000 <= facingDYaw && facingDYaw <= 0x4000) {
        m->forwardVel *= -1.0f;
        bonkAction = sBackwardKnockbackActions[terrainIndex][strengthIndex];
        capYaw = (s16) (m->faceAngle[1] + 0x8000);
    } else {
        m->faceAngle[1] += 0x8000;
        bonkAction = sForwardKnockbackActions[terrainIndex][strengthIndex];
        capYaw = m->faceAngle[1];
    }

    // Throw the cap forward or backward to match the selected reaction animation.
#ifndef PREVENT_CAP_LOSS
    if (strengthIndex == 2 && (m->interactObj->oInteractionSubtype & INT_SUBTYPE_BOWSER_ATTACK)) {
        mario_blow_off_cap_in_direction(m, 2.0f * MAX(15.0f, ABS(m->forwardVel) + 5.0f), capYaw);
    }
#endif
    mario_lose_cap_on_lethal_damage_with_speed(
        m, capYaw, (m->interactObj->oInteractionSubtype & INT_SUBTYPE_BOWSER_ATTACK) ? 2.0f : 1.0f);
    return bonkAction;
}

void push_mario_out_of_object(struct MarioState *m, struct Object *obj, f32 padding) {
    f32 minDistance = obj->hitboxRadius + m->marioObj->hitboxRadius + padding;

    f32 offsetX = m->pos[0] - obj->oPosX;
    f32 offsetZ = m->pos[2] - obj->oPosZ;
    f32 distanceSquared = (sqr(offsetX) + sqr(offsetZ));

    if (distanceSquared < sqr(minDistance)) {
        struct Surface *floor;
        s16 pushAngle;
        f32 newMarioX;
        f32 newMarioZ;

        if (distanceSquared == 0.0f) {
            pushAngle = m->faceAngle[1];
        } else {
            pushAngle = atan2s(offsetZ, offsetX);
        }

        newMarioX = obj->oPosX + minDistance * sins(pushAngle);
        newMarioZ = obj->oPosZ + minDistance * coss(pushAngle);

        f32_find_wall_collision(&newMarioX, &m->pos[1], &newMarioZ, 60.0f, 50.0f);

        f32 floorHeight = find_floor(newMarioX, m->pos[1], newMarioZ, &floor);
        if (floor != NULL) {
            m->pos[0] = newMarioX;
            m->pos[2] = newMarioZ;
            set_mario_floor(m, floor, floorHeight); // Prevent oob death
        }
    }
}

// A connected punch sharply reduces travel speed without launching Mario backward.
#define PUNCH_HIT_MOMENTUM_FACTOR 0.15f

static s32 mario_is_breakdancing(struct MarioState *m) {
    return (m->action == ACT_PUNCHING || m->action == ACT_MOVE_PUNCHING)
           && m->actionArg == ACT_ARG_PUNCH_SEQUENCE_BREAKDANCE;
}

static s32 mario_protects_from_knockback(struct MarioState *m) {
    return mario_blocks_harmful_knockback(m) || mario_is_breakdancing(m);
}

s32 mario_breakdance_protects_from_damage(struct MarioState *m) {
    if (!mario_is_breakdancing(m)) {
        return FALSE;
    }
    // A fresh spin's first frame is vulnerable, even with an old animation pose.
    if (m->actionTimer == 0) {
        return FALSE;
    }
    struct AnimInfo *animInfo = &m->marioObj->header.gfx.animInfo;
    return animInfo->animID == MARIO_ANIM_BREAKDANCE && animInfo->curAnim != NULL
           && animInfo->animFrame != 0 && animInfo->animFrame != 12;
}
static void slow_mario_after_punch_hit(struct MarioState *m) {
    if (m->action == ACT_PUNCHING) {
        m->action = ACT_MOVE_PUNCHING;
    }
    mario_set_forward_vel(m, m->forwardVel * PUNCH_HIT_MOMENTUM_FACTOR);
}

void bounce_back_from_attack(struct MarioState *m, u32 interaction) {
    if (interaction & (INT_PUNCH | INT_KICK | INT_TRIP)) {
        if (interaction & INT_PUNCH) {
            slow_mario_after_punch_hit(m);
        } else if (!(interaction & INT_TRIP) || !mario_is_breakdancing(m)) {
            if (m->action == ACT_PUNCHING) {
                m->action = ACT_MOVE_PUNCHING;
            }
            mario_set_forward_vel(m, (m->action & ACT_FLAG_AIR) ? -16.0f : -48.0f);
        }

        set_camera_shake_from_hit(SHAKE_ATTACK);
        m->particleFlags |= PARTICLE_TRIANGLE;
    }

    if (interaction & (INT_PUNCH | INT_KICK | INT_TRIP | INT_FAST_ATTACK_OR_SHELL)) {
        mario_he_thicc_contact_shake(m);
        play_sound(SOUND_ACTION_HIT_2, m->marioObj->header.gfx.cameraToObject);
    }
}

#define DOOR_OPEN_MIN_STICK_MAG 32.0f
#define DOOR_OPEN_MAX_ANGLE DEGREES(45)

static s32 mario_has_door_open_intent(struct MarioState *m, struct Object *obj) {
    // A deliberate wall contact opens the contacted leaf even when its center
    // bearing or Mario's sidling pose would fail the ordinary approach check.
    if (m->wall != NULL && m->wall->object == obj && m->intendedMag > 0.0f
        && abs_angle_diff(m->intendedYaw, m->wallYaw + 0x8000) <= WALK_WALL_HUG_MAX_ANGLE) {
        return TRUE;
    }

    if (m->controller->stickMag < DOOR_OPEN_MIN_STICK_MAG) {
        return FALSE;
    }

    // World-space bearing admits deliberate approaches from either door side.
    s16 doorYaw = mario_obj_angle_to_object(m, obj);
    return abs_angle_diff(m->intendedYaw, doorYaw) <= DOOR_OPEN_MAX_ANGLE
           && abs_angle_diff(m->faceAngle[1], doorYaw) <= DOOR_OPEN_MAX_ANGLE;
}

u32 should_push_or_pull_door(struct MarioState *m, struct Object *obj) {
    f32 dx = obj->oPosX - m->pos[0];
    f32 dz = obj->oPosZ - m->pos[2];

    s16 dYaw = abs_angle_diff(obj->oMoveAngleYaw, atan2s(dz, dx));

    return (dYaw <= 0x4000) ? WARP_FLAG_DOOR_PULLED : WARP_FLAG_DOOR_FLIP_MARIO;
}

u32 take_damage_from_interact_object(struct MarioState *m) {
    if (game_options_cheat_active(GO_INFINITE_HEALTH)) {
        m->invincTimer = MAX(m->invincTimer, 30);
        return 0;
    }
    if (mario_breakdance_protects_from_damage(m)
        && !(m->interactObj->oInteractionSubtype & INT_SUBTYPE_BOWSER_ATTACK)) {
        return 0;
    }
    if (castle_secret_lobby_active()) {
        return 0;
    }

    s32 shake;
    s32 damage = m->interactObj->oDamageOrCoinValue;

    if (damage >= 4) {
        shake = SHAKE_LARGE_DAMAGE;
    } else if (damage >= 2) {
        shake = SHAKE_MED_DAMAGE;
    } else {
        shake = SHAKE_SMALL_DAMAGE;
    }

    if (mario_has_current_metal_cap(m) && m->interactObj->oInteractType == INTERACT_SHOCK
        && damage > 0) {
        // Electric contact has a fixed three-wedge Metal base; water/Soaked
        // applies its single multiplier in interact_shock.
        damage = 3;
    } else if (!mario_has_cap_for_damage(m) && !obj_has_behavior(m->interactObj, bhvBowserBomb)) {
        // Even a zero-base hazard costs one wedge without the regular hat.
        damage = MAX(1, damage * 2);
    }

    if ((m->flags & MARIO_METAL_CAP)
        && (game_options_get(GO_METAL_CAP) == 1 || m->interactObj->oInteractType != INTERACT_SHOCK)) {
        damage = 0;
    }

    // Limit this hit, preserving damage already queued by separate contacts.
    damage = MIN(damage, 7);
    if (damage > 0) {
        player_stats_damage();
    }
    m->hurtCounter += 4 * damage;

    queue_rumble_data(5, 80);
    set_camera_shake_from_hit(shake);

    return damage;
}

void mario_queue_bowser_impact_with_strength(struct Object *source, s32 heavy) {
    struct MarioState *m = gMarioState;
    if (m->invincTimer != 0 || (m->action & ACT_FLAG_AIR)
        || ((m->action & ACT_FLAG_INVULNERABLE) && m->action != ACT_GROUND_TWIRL)
        || sTwirlContact.reboundHiddenTimer != 0) {
        return;
    }
    u32 status = heavy ? INT_STATUS_MARIO_KNOCKBACK_DMG : INT_STATUS_MARIO_STUNNED;
    if (!(m->marioObj->oInteractStatus & INT_STATUS_MARIO_KNOCKBACK_DMG)) {
        vec3f_set(m->bowserImpactPos, source->oPosX, source->oPosY, source->oPosZ);
    }
    m->marioObj->oInteractStatus |= status;
    if (mario_is_breakdancing(m)) {
        mario_bowser_impact_knockback(m, FALSE);
        m->marioObj->oInteractStatus &= ~status;
    }
}

void mario_queue_bowser_impact(struct Object *source) {
    f32 dx = gMarioState->pos[0] - source->oPosX;
    f32 dz = gMarioState->pos[2] - source->oPosZ;
    if (dx * dx + dz * dz > BOWSER_MAX_IMPACT_RADIUS * BOWSER_MAX_IMPACT_RADIUS) {
        return;
    }
    mario_queue_bowser_impact_with_strength(
        source, dx * dx + dz * dz < BOWSER_NEAR_IMPACT_RADIUS * BOWSER_NEAR_IMPACT_RADIUS);
}

u32 mario_bowser_impact_knockback(struct MarioState *m, s32 inAir) {
    if (mario_blocks_harmful_knockback(m)) {
        s32 original = game_options_get(GO_BOWSER_FIGHTS) == 1;
        s32 heavy = (m->marioObj->oInteractStatus & INT_STATUS_MARIO_KNOCKBACK_DMG) != 0;
        s32 hurt;
        if (original) {
            hurt = (m->marioObj->oInteractStatus & INT_STATUS_MARIO_SHOCKWAVE) ? 4 : heavy ? 12 : 0;
        } else {
            hurt = (heavy ? 8 : 4) * (mario_has_cap_for_damage(m) ? 1 : 2);
        }
        if (!original) {
            player_stats_cap_powers_removed(mario_natural_cap_powers(m), FALSE);
            mario_remove_cap_powers(m);
        }
        if (hurt != 0 && !game_options_cheat_active(GO_INFINITE_HEALTH)) {
            player_stats_damage();
            m->hurtCounter += hurt;
        }
        m->invincTimer = MAX(m->invincTimer, 30);
        queue_rumble_data(70, 40);
        // Consume the impact without requesting redispatch of the unchanged action.
        m->input &= ~INPUT_STOMPED;
        m->marioObj->oInteractStatus &=
            ~(INT_STATUS_MARIO_STUNNED | INT_STATUS_MARIO_KNOCKBACK_DMG | INT_STATUS_MARIO_SHOCKWAVE);
        return FALSE;
    }

    if (game_options_get(GO_BOWSER_FIGHTS) == 1) {
        if (m->marioObj->oInteractStatus & INT_STATUS_MARIO_SHOCKWAVE) {
            queue_rumble_data(70, 40);
            return hurt_and_set_mario_action(m, ACT_SHOCKED, 0, 4);
        }
        if (m->marioObj->oInteractStatus & INT_STATUS_MARIO_KNOCKBACK_DMG) {
            queue_rumble_data(70, 40);
            return hurt_and_set_mario_action(m, inAir ? ACT_BACKWARD_AIR_KB : ACT_BACKWARD_GROUND_KB, 0,
                                             12);
        }
        return drop_and_set_mario_action(m, ACT_SHOCKWAVE_BOUNCE, 0);
    }
    player_stats_cap_powers_removed(mario_natural_cap_powers(m), FALSE);
    mario_remove_cap_powers(m);
    s32 heavy = (m->marioObj->oInteractStatus & INT_STATUS_MARIO_KNOCKBACK_DMG) != 0;
    s32 damage = heavy ? 2 : 1;
    if (!mario_has_cap_for_damage(m)) {
        damage *= 2;
    }
    if (damage > 0) {
        player_stats_damage();
    }
    m->hurtCounter += 4 * damage;
    m->faceAngle[1] = atan2s(m->bowserImpactPos[2] - m->pos[2], m->bowserImpactPos[0] - m->pos[0]);
    if (heavy) {
        mario_blow_off_cap(m, 2.0f * MAX(15.0f, ABS(m->forwardVel) + 5.0f));
    }
    mario_lose_cap_on_lethal_damage_with_speed(m, m->faceAngle[1] + 0x8000, 2.0f);
    queue_rumble_data(70, 40);
    u32 action = heavy ? ACT_HARD_BACKWARD_GROUND_KB : ACT_SOFT_BACKWARD_GROUND_KB;
    if (inAir) {
        action = heavy ? ACT_HARD_BACKWARD_AIR_KB : ACT_BACKWARD_AIR_KB;
    }
    u32 result = drop_and_set_mario_action(m, action, damage);
    mario_set_forward_vel(m, heavy ? -40.0f : -16.0f);
    if (inAir) {
        m->vel[1] = heavy ? 50.0f : 20.0f;
    }
    return result;
}

static u32 mario_bowser_blast_knockback(struct MarioState *m, s16 yaw, f32 speed, u32 damage) {
    m->faceAngle[1] = yaw;
    mario_blow_off_cap(m, 2.0f * MAX(15.0f, ABS(m->forwardVel) + 5.0f));
    u32 result = drop_and_set_mario_action(m, ACT_HARD_BACKWARD_AIR_KB, damage);
    mario_set_forward_vel(m, -speed);
    m->vel[1] = 50.0f;
    return result;
}

u32 take_damage_and_knock_back(struct MarioState *m, struct Object *obj) {
    u32 damage;
    s32 bowserAttack = (obj->oInteractionSubtype & INT_SUBTYPE_BOWSER_ATTACK) != 0;

    if (!sInvulnerable && (!(m->flags & MARIO_VANISH_CAP) || bowserAttack)
        && !(obj->oInteractionSubtype & INT_SUBTYPE_DELAY_INVINCIBILITY)) {
        if (bowserAttack) {
            player_stats_cap_powers_removed(mario_natural_cap_powers(m), FALSE);
            mario_remove_cap_powers(m);
        }
        obj->oInteractStatus = INT_STATUS_INTERACTED | INT_STATUS_ATTACKED_MARIO;
        m->interactObj = obj;

        s32 launch = FALSE;
        if (game_options_get(GO_BOWSER_FIGHTS) == 0 && obj_has_behavior(obj, bhvBowserBodyAnchor)) {
            struct Object *bowser = obj->parentObj;
            launch = bowser->oAction == BOWSER_ACT_BIG_JUMP || bowser->oAction == BOWSER_ACT_QUICK_JUMP
                     || bowser->oAction == BOWSER_ACT_JUMP_ONTO_STAGE
                     || dist_between_objects(m->marioObj, bowser) < BOWSER_POINT_BLANK_RADIUS;
            obj->oDamageOrCoinValue = launch || bowser->oAction == BOWSER_ACT_CHARGE_MARIO ? 4 : 2;
        }
        damage = take_damage_from_interact_object(m);
        // A direct hit takes priority over a landing/wave queued on the previous update.
        m->input &= ~INPUT_STOMPED;
        m->marioObj->oInteractStatus &=
            ~(INT_STATUS_MARIO_STUNNED | INT_STATUS_MARIO_KNOCKBACK_DMG | INT_STATUS_MARIO_SHOCKWAVE);
        if (damage != 0 && obj_has_behavior(obj, bhvBowserBomb) && !mario_blocks_harmful_knockback(m)) {
            mario_blow_off_cap(m, 2.0f * MAX(15.0f, ABS(m->forwardVel) + 5.0f));
        }

        if (obj->oDamageOrCoinValue > 0) {
            play_sound(SOUND_MARIO_ATTACKED, m->marioObj->header.gfx.cameraToObject);
        }

        update_mario_sound_and_camera(m);
        if (mario_blocks_harmful_knockback(m)
            || (mario_protects_from_knockback(m)
                && !(obj->oInteractionSubtype & INT_SUBTYPE_BOWSER_ATTACK))) {
            m->invincTimer = MAX(m->invincTimer, 30);
            return TRUE;
        }
        if (obj_has_behavior(obj, bhvBowserBomb)) {
            // Bowser arenas use the world origin as their center.
            s16 outwardYaw = atan2s(m->pos[2], m->pos[0]);
            return mario_bowser_blast_knockback(m, outwardYaw, 140.0f, damage | BOWSER_MINE_LAUNCH_ARG);
        }
        if (launch) {
            return mario_bowser_blast_knockback(m, mario_obj_angle_to_object(m, obj), 60.0f, damage);
        }
        if ((obj->oInteractionSubtype & INT_SUBTYPE_BIG_KNOCKBACK) && !mario_has_current_metal_cap(m)) {
            m->forwardVel = 40.0f;
        }
        return drop_and_set_mario_action(m, determine_knockback_action(m, obj->oDamageOrCoinValue),
                                         damage);
    }

    return FALSE;
}

// Punches and kicks can still hit an enemy, but that hit does not protect Mario
// from the enemy's collision. Keep the enemy's attack result when Mario is hit.
static s32 take_knockback_after_punch_or_kick(struct MarioState *m, struct Object *obj,
                                              u32 interaction) {
    if (!(interaction & (INT_PUNCH | INT_KICK | INT_SLIDE_KICK))) {
        return FALSE;
    }

    u32 attackedStatus = obj->oInteractStatus;
    if (take_damage_and_knock_back(m, obj)) {
        // Native enemy handlers give ATTACKED_MARIO priority over WAS_ATTACKED.
        // Mario already took the collision hit; keep the enemy's strike result.
        obj->oInteractStatus = (attackedStatus | obj->oInteractStatus) & ~INT_STATUS_ATTACKED_MARIO;
        return TRUE;
    }
    return FALSE;
}

static void reset_mario_pitch(struct MarioState *m) {
    if (m->action == ACT_WATER_JUMP || m->action == ACT_SHOT_FROM_CANNON || m->action == ACT_FLYING) {
        set_camera_mode(m->area->camera, m->area->camera->defMode, 1);
        m->faceAngle[0] = 0;
    }
}

void mario_extinguish_fire(struct MarioState *m) {
    u32 nextAction;

    switch (m->action) {
        case ACT_BURNING_GROUND:
            nextAction = ACT_WALKING;
            break;
        case ACT_BURNING_JUMP:
        case ACT_BURNING_FALL:
        case ACT_LAVA_BOOST:
            nextAction = ACT_FREEFALL;
            break;
        case ACT_LAVA_BOOST_LAND:
            nextAction = (m->input & INPUT_NONZERO_ANALOG) ? ACT_WALKING : ACT_IDLE;
            break;
        default:
            return;
    }

    m->marioObj->oMarioBurnTimer = 0;
    m->particleFlags &= ~PARTICLE_FIRE;
    play_sound(SOUND_GENERAL_FLAME_OUT, m->marioObj->header.gfx.cameraToObject);
    set_mario_action(m, nextAction, 0);
}

u32 interact_coin(struct MarioState *m, UNUSED u32 interactType, struct Object *obj) {
    s32 groundedBurnRecovery = m->action == ACT_BURNING_GROUND;
    m->numCoins += obj->oDamageOrCoinValue;
    player_stats_coin(obj, m->numCoins);
    if (obj->oDamageOrCoinValue > 0) {
        hud_reveal_counters();
    }
    if ((obj->oInteractionSubtype & INT_SUBTYPE_YOSHI_GIFT_COIN) && !game_options_secrets_blocked()) {
        s32 healthBeforeGift = mario_health_after_pending_damage(m);
        player_stats_damage();
        m->hurtCounter += 4;
        if (healthBeforeGift >= 0x100 && mario_health_after_pending_damage(m) < 0x100) {
            m->yoshiCoinDeathPending = TRUE;
        }
    } else if (m->health >= 0x100) {
        m->healCounter += 4 * obj->oDamageOrCoinValue;
    }
    mario_ground_twirl_reset_cooldown();
    mario_air_twirl_reset_on_coin(m);
    mario_extinguish_fire(m);
    if (groundedBurnRecovery && m->action == ACT_WALKING
        && m->framesSinceA < MARIO_BURN_COIN_JUMP_BUFFER_FRAMES) {
        m->burnCoinJumpPending = MARIO_BURN_COIN_JUMP_BUFFER_FRAMES - m->framesSinceA;
    }
#ifdef BREATH_METER
    m->breathCounter += (4 * obj->oDamageOrCoinValue);
#endif
    obj->oInteractStatus = INT_STATUS_INTERACTED;

#ifdef X_COIN_STAR
    if (COURSE_IS_MAIN_COURSE(gCurrCourseNum) && m->numCoins - obj->oDamageOrCoinValue < X_COIN_STAR
        && m->numCoins >= X_COIN_STAR && !g100CoinStarSpawned) {
        bhv_spawn_star_no_level_exit(STAR_BP_ACT_100_COINS);
        g100CoinStarSpawned = TRUE;
    }
#endif
    if (obj->oDamageOrCoinValue >= 2) {
        queue_rumble_data(5, 80);
    }

    return FALSE;
}

u32 interact_water_ring(struct MarioState *m, UNUSED u32 interactType, struct Object *obj) {
#ifdef BREATH_METER
    m->breathCounter += 4 * obj->oDamageOrCoinValue;
#else
    if (m->health >= 0x100) {
        m->healCounter += 4 * obj->oDamageOrCoinValue;
    }
#endif
    obj->oInteractStatus = INT_STATUS_INTERACTED;
    return FALSE;
}

u32 interact_star_or_key(struct MarioState *m, UNUSED u32 interactType, struct Object *obj) {
    u32 starIndex;
    u32 starGrabAction = ACT_STAR_DANCE_EXIT;
#ifdef NON_STOP_STARS
 #ifdef KEYS_EXIT_LEVEL
    u32 noExit = !obj_has_model(obj, MODEL_BOWSER_KEY);
 #else
    u32 noExit = TRUE;
 #endif
#else // !NON_STOP_STARS
    u32 noExit = (obj->oInteractionSubtype & INT_SUBTYPE_NO_EXIT) != 0;
#endif // !NON_STOP_STARS
    u32 grandStar = (obj->oInteractionSubtype & INT_SUBTYPE_GRAND_STAR) != 0;

    if (m->health >= 0x100) {
        mario_stop_riding_and_holding(m);
        queue_rumble_data(5, 80);

#ifdef GLOBAL_STAR_IDS
        starIndex = (obj->oBehParams >> 24) & 0xFF;
#else
        starIndex = (obj->oBehParams >> 24) & 0x1F;
#endif
        // Classify the pickup before star healing clears pending damage.
        player_stats_star(obj, starIndex);

#ifdef POWER_STARS_HEAL
        m->hurtCounter = 0;
        m->healCounter = 31;
        m->respawnHealCounter = 0;
#ifdef BREATH_METER
        m->breathCounter = 31;
#endif
        if (!noExit) {
#else // !POWER_STARS_HEAL
        if (!noExit) {
            m->hurtCounter = 0;
            m->healCounter = 0;
#ifdef BREATH_METER
            m->breathCounter = 0;
#endif
#endif // !POWER_STARS_HEAL
            if (m->capTimer > 1) {
                m->capTimer = 1;
            }
        }

        if (noExit) {
            starGrabAction = ACT_STAR_DANCE_NO_EXIT;
        }

        if (m->action & ACT_FLAG_SWIMMING) {
            starGrabAction = ACT_STAR_DANCE_WATER;
        }

        if ((m->flags & MARIO_METAL_CAP) && mario_is_head_underwater(m)) {
            starGrabAction = ACT_STAR_DANCE_WATER;
        }

        if ((m->action & ACT_FLAG_AIR) && starGrabAction != ACT_STAR_DANCE_WATER) {
            starGrabAction = ACT_FALL_AFTER_STAR_GRAB;
        }

        spawn_object(obj, MODEL_NONE, bhvStarKeyCollectionPuffSpawner);

        obj->oInteractStatus = INT_STATUS_INTERACTED;
        m->interactObj       = obj;
        m->usedObj           = obj;

        gStarModelLastCollected = obj_get_model_id(obj);

        game_timing_runtime_star(obj, starIndex);
        save_file_collect_star_or_key(m->numCoins, starIndex);

        m->numStars =
            save_file_get_total_star_count(gCurrSaveFileNum - 1, COURSE_MIN - 1, COURSE_MAX - 1);

        if (!noExit) {
            drop_queued_background_music();
            fadeout_level_music(126);
        }

        play_sound(SOUND_MENU_STAR_SOUND, m->marioObj->header.gfx.cameraToObject);
        update_mario_sound_and_camera(m);

        if (grandStar) {
            if (gCurrLevelNum == LEVEL_BOWSER_3) {
                save_file_mark_game_completed();
            }
            return set_mario_action(m, ACT_JUMBO_STAR_CUTSCENE, 0);
        }

        return set_mario_action(m, starGrabAction, noExit + 2 * grandStar);
    }

    return FALSE;
}

u32 interact_bbh_entrance(struct MarioState *m, UNUSED u32 interactType, struct Object *obj) {
    if (m->action != ACT_BBH_ENTER_SPIN && m->action != ACT_BBH_ENTER_JUMP) {
        mario_stop_riding_and_holding(m);

        obj->oInteractStatus = INT_STATUS_INTERACTED;
        m->interactObj       = obj;
        m->usedObj           = obj;

        if (m->action & ACT_FLAG_AIR) {
            return set_mario_action(m, ACT_BBH_ENTER_SPIN, 0);
        }

        return set_mario_action(m, ACT_BBH_ENTER_JUMP, 0);
    }

    return FALSE;
}

u32 interact_warp(struct MarioState *m, UNUSED u32 interactType, struct Object *obj) {
    u32 action;

    if (obj->oInteractionSubtype & INT_SUBTYPE_FADING_WARP) {
        action = m->action;

        if (action == ACT_TELEPORT_FADE_IN) {
            sJustTeleported = TRUE;

        } else if (!sJustTeleported) {
            if (action == ACT_IDLE || action == ACT_PANTING || action == ACT_STANDING_AGAINST_WALL || action == ACT_CROUCHING) {
                m->interactObj = obj;
                m->usedObj     = obj;

                sJustTeleported = TRUE;
                return set_mario_action(m, ACT_TELEPORT_FADE_OUT, 0);
            }
        }
    } else {
        if (m->action != ACT_EMERGE_FROM_PIPE) {
            obj->oInteractStatus = INT_STATUS_INTERACTED;
            m->interactObj       = obj;
            m->usedObj           = obj;

            if (obj->collisionData == segmented_to_virtual(warp_pipe_seg3_collision_03009AC8)) {
                player_stats_add(PS_WARP_PIPES_ENTERED, 1);
                play_sound(SOUND_MENU_ENTER_PIPE, m->marioObj->header.gfx.cameraToObject);
                queue_rumble_data(15, 80);
            } else {
                play_sound(SOUND_MENU_ENTER_HOLE, m->marioObj->header.gfx.cameraToObject);
                queue_rumble_data(12, 80);
            }

            mario_stop_riding_object(m);
            return set_mario_action(m, ACT_DISAPPEARED, (WARP_OP_WARP_OBJECT << 16) + 2);
        }
    }

    return FALSE;
}

u32 interact_warp_door(struct MarioState *m, UNUSED u32 interactType, struct Object *obj) {
    u32 doorAction = ACT_UNINITIALIZED;
#ifndef UNLOCK_ALL
    u32 saveFlags = save_file_get_flags();
    s16 warpDoorId = (obj->oBehParams >> 24);
#endif

    if (m->action == ACT_WALKING || m->action == ACT_DECELERATING) {
        if (!mario_has_door_open_intent(m, obj)) {
            return FALSE;
        }

#ifndef UNLOCK_ALL
        if (warpDoorId == 1 && !(saveFlags & SAVE_FLAG_UNLOCKED_UPSTAIRS_DOOR)) {
            if (!(saveFlags & SAVE_FLAG_HAVE_KEY_2)) {
                if (!sDisplayingDoorText) {
                    set_mario_action(m, ACT_READING_AUTOMATIC_DIALOG,
                                     (saveFlags & SAVE_FLAG_HAVE_KEY_1) ? DIALOG_023 : DIALOG_022);
                }
                sDisplayingDoorText = TRUE;

                return FALSE;
            }

            doorAction = ACT_UNLOCKING_KEY_DOOR;
        }

        if (warpDoorId == 2 && !(saveFlags & SAVE_FLAG_UNLOCKED_BASEMENT_DOOR)) {
            if (!(saveFlags & SAVE_FLAG_HAVE_KEY_1)) {
                if (!sDisplayingDoorText) {
                    // Moat door skip was intended confirmed
                    set_mario_action(m, ACT_READING_AUTOMATIC_DIALOG,
                                     (saveFlags & SAVE_FLAG_HAVE_KEY_2) ? DIALOG_023 : DIALOG_022);
                }
                sDisplayingDoorText = TRUE;

                return FALSE;
            }

            doorAction = ACT_UNLOCKING_KEY_DOOR;
        }
#endif

        if (m->action == ACT_WALKING || m->action == ACT_DECELERATING) {
            u32 actionArg = should_push_or_pull_door(m, obj) + WARP_FLAG_DOOR_IS_WARP;

            if (doorAction == 0) {
                if (actionArg & WARP_FLAG_DOOR_PULLED) {
                    doorAction = ACT_PULLING_DOOR;
                } else {
                    doorAction = ACT_PUSHING_DOOR;
                }
            }

            m->interactObj = obj;
            m->usedObj = obj;
            return set_mario_action(m, doorAction, actionArg);
        }
    }

    return FALSE;
}

u32 get_door_save_file_flag(struct Object *door) {
    u32 saveFileFlag = 0;
    s16 requiredNumStars = door->oBehParams >> 24;

    s16 isCcmDoor = door->oPosX < 0.0f;
    s16 isPssDoor = door->oPosY > 500.0f;

    switch (requiredNumStars) {
        case  1: saveFileFlag = (isPssDoor ? SAVE_FLAG_UNLOCKED_PSS_DOOR : SAVE_FLAG_UNLOCKED_WF_DOOR ); break;
        case  3: saveFileFlag = (isCcmDoor ? SAVE_FLAG_UNLOCKED_CCM_DOOR : SAVE_FLAG_UNLOCKED_JRB_DOOR); break;
        case  8: saveFileFlag = SAVE_FLAG_UNLOCKED_BITDW_DOOR;                                           break;
        case 30: saveFileFlag = SAVE_FLAG_UNLOCKED_BITFS_DOOR;                                           break;
        case 50: saveFileFlag = SAVE_FLAG_UNLOCKED_50_STAR_DOOR;                                         break;
    }

    return saveFileFlag;
}

u32 interact_door(struct MarioState *m, UNUSED u32 interactType, struct Object *obj) {
    s16 requiredNumStars = (obj->oBehParams >> 24);
#ifndef UNLOCK_ALL
    s16 numStars = save_file_get_total_star_count(gCurrSaveFileNum - 1, COURSE_MIN - 1, COURSE_MAX - 1);
#endif
    if (m->action == ACT_WALKING || m->action == ACT_DECELERATING) {
        if (!mario_has_door_open_intent(m, obj)) {
            return FALSE;
        }

#ifndef UNLOCK_ALL
        if (numStars >= requiredNumStars) {
#endif
            u32 actionArg = should_push_or_pull_door(m, obj);
            u32 enterDoorAction;

            if (actionArg & WARP_FLAG_DOOR_PULLED) {
                enterDoorAction = ACT_PULLING_DOOR;
            } else {
                enterDoorAction = ACT_PUSHING_DOOR;
            }

#ifndef UNLOCK_ALL
            u32 doorSaveFileFlag = get_door_save_file_flag(obj);
#endif
            m->interactObj = obj;
            m->usedObj     = obj;

            if (obj->oInteractionSubtype & INT_SUBTYPE_STAR_DOOR) {
                enterDoorAction = ACT_ENTERING_STAR_DOOR;
            }

#ifndef UNLOCK_ALL
            if (doorSaveFileFlag != 0 && !(save_file_get_flags() & doorSaveFileFlag)) {
                enterDoorAction = ACT_UNLOCKING_STAR_DOOR;
            }
#endif
            return set_mario_action(m, enterDoorAction, actionArg);
#ifndef UNLOCK_ALL
        } else if (!sDisplayingDoorText) {
            u32 text = DIALOG_022 << 16;

            switch (requiredNumStars) {
                case  1: text = DIALOG_024 << 16; break;
                case  3: text = DIALOG_025 << 16; break;
                case  8: text = DIALOG_026 << 16; break;
                case 30: text = DIALOG_027 << 16; break;
                case 50: text = DIALOG_028 << 16; break;
                case 70: text = DIALOG_029 << 16; break;
            }

            text += requiredNumStars - numStars;

            sDisplayingDoorText = TRUE;
            return set_mario_action(m, ACT_READING_AUTOMATIC_DIALOG, text);
        }
#endif
    } else if (m->action == ACT_IDLE && sDisplayingDoorText == TRUE && requiredNumStars == 70) {
        m->interactObj = obj;
        m->usedObj     = obj;
        return set_mario_action(m, ACT_ENTERING_STAR_DOOR, should_push_or_pull_door(m, obj));
    }

    return FALSE;
}

u32 interact_cannon_base(struct MarioState *m, UNUSED u32 interactType, struct Object *obj) {
    if (m->action != ACT_IN_CANNON && mario_health_after_pending_damage(m) >= 0x100) {
        mario_stop_riding_and_holding(m);
        obj->oInteractStatus = INT_STATUS_INTERACTED;
        m->interactObj = obj;
        m->usedObj = obj;
        return set_mario_action(m, ACT_IN_CANNON, 0);
    }

    return FALSE;
}

u32 interact_igloo_barrier(struct MarioState *m, UNUSED u32 interactType, struct Object *obj) {
    //! Sets used object without changing action (LOTS of interesting glitches,
    // but unfortunately the igloo barrier is the only object with this interaction
    // type)
    m->interactObj = obj;
    m->usedObj     = obj;
    push_mario_out_of_object(m, obj, 5.0f);
    return FALSE;
}

u32 interact_tornado(struct MarioState *m, UNUSED u32 interactType, struct Object *obj) {
    if (game_options_cheat_active(GO_IGNORE_WIND)) {
        return FALSE;
    }
    if (mario_has_current_metal_cap(m) || mario_protects_from_knockback(m)) {
        return FALSE;
    }
    struct Object *marioObj = m->marioObj;

    if (m->action != ACT_TORNADO_TWIRLING && m->action != ACT_SQUISHED) {
        mario_stop_riding_and_holding(m);
        mario_set_forward_vel(m, 0.0f);
        update_mario_sound_and_camera(m);

        obj->oInteractStatus = INT_STATUS_INTERACTED;
        m->interactObj       = obj;
        m->usedObj           = obj;

        marioObj->oMarioTornadoYawVel = 0x400;
        marioObj->oMarioTornadoPosY = m->pos[1] - obj->oPosY;

        play_sound(SOUND_MARIO_WAAAOOOW, m->marioObj->header.gfx.cameraToObject);
        queue_rumble_data(30, 60);
        return set_mario_action(m, ACT_TORNADO_TWIRLING, m->action == ACT_TWIRLING);
    }

    return FALSE;
}

u32 interact_whirlpool(struct MarioState *m, UNUSED u32 interactType, struct Object *obj) {
    if (game_options_cheat_active(GO_IGNORE_CURRENTS)) {
        return FALSE;
    }
    struct Object *marioObj = m->marioObj;

    if (m->action != ACT_CAUGHT_IN_WHIRLPOOL) {
        mario_stop_riding_and_holding(m);
        obj->oInteractStatus = INT_STATUS_INTERACTED;
        m->interactObj       = obj;
        m->usedObj           = obj;

        m->forwardVel = 0.0f;

        marioObj->oMarioWhirlpoolPosY = m->pos[1] - obj->oPosY;

        play_sound(SOUND_MARIO_WAAAOOOW, m->marioObj->header.gfx.cameraToObject);
        queue_rumble_data(30, 60);
        return set_mario_action(m, ACT_CAUGHT_IN_WHIRLPOOL, 0);
    }

    return FALSE;
}

u32 interact_strong_wind(struct MarioState *m, UNUSED u32 interactType, struct Object *obj) {
    if (game_options_cheat_active(GO_IGNORE_WIND)) {
        return FALSE;
    }
    if (mario_has_current_metal_cap(m) || mario_protects_from_knockback(m)) {
        return FALSE;
    }
    if (m->action != ACT_GETTING_BLOWN) {
        mario_stop_riding_and_holding(m);
        obj->oInteractStatus = INT_STATUS_INTERACTED;
        m->interactObj = obj;
        m->usedObj     = obj;

        m->faceAngle[1] = obj->oMoveAngleYaw + 0x8000;
        m->windGravity = 0.4f;
        m->forwardVel = -24.0f;
        m->vel[1] = 12.0f;

        play_sound(SOUND_MARIO_WAAAOOOW, m->marioObj->header.gfx.cameraToObject);
        update_mario_sound_and_camera(m);
        return set_mario_action(m, ACT_GETTING_BLOWN, 0);
    }

    return FALSE;
}

u32 interact_flame(struct MarioState *m, UNUSED u32 interactType, struct Object *obj) {
    u32 burningAction = ACT_BURNING_JUMP;

    if (!sInvulnerable && !(obj->oInteractionSubtype & INT_SUBTYPE_DELAY_INVINCIBILITY)) {
        if (obj->oInteractionSubtype & INT_SUBTYPE_BOWSER_ATTACK) {
            player_stats_cap_powers_removed(mario_natural_cap_powers(m), FALSE);
            mario_remove_cap_powers(m);
        }
        if ((m->flags & MARIO_VANISH_CAP) && game_options_get(GO_VANISH_CAP) == 1) {
            return FALSE;
        }
        if (mario_is_soaked(m)
            || (game_options_get(GO_SOAKED) && m->soakedFlameProtection
                && !mario_has_current_vanish_cap(m))) {
            obj->oInteractStatus = INT_STATUS_INTERACTED;
            m->interactObj = obj;
            m->soakedFlameProtection = TRUE;
            mario_consume_soaked_status(m);
            play_sound(SOUND_GENERAL_FLAME_OUT, m->marioObj->header.gfx.cameraToObject);
            return TRUE;
        }
        if (m->flags & MARIO_METAL_CAP) {
            return FALSE;
        }
        queue_rumble_data(5, 80);
        obj->oInteractStatus = INT_STATUS_INTERACTED;
        m->interactObj = obj;

        if ((m->action & (ACT_FLAG_SWIMMING | ACT_FLAG_METAL_WATER))
            || m->waterLevel - m->pos[1] > 50.0f) {
            play_sound(SOUND_GENERAL_FLAME_OUT, m->marioObj->header.gfx.cameraToObject);
        } else {
            if (game_options_cheat_active(GO_INFINITE_HEALTH)) {
                m->invincTimer = MAX(m->invincTimer, 30);
            }
            if (mario_blocks_harmful_knockback(m)
                || (mario_protects_from_knockback(m)
                    && !(obj->oInteractionSubtype & INT_SUBTYPE_BOWSER_ATTACK))) {
                // Preserve the spin instead of entering the burning launch.
                // Queue the normal complete burn loss as whole wedges.
                player_stats_damage();
                m->hurtCounter += 4 * (MARIO_BURN_DAMAGE_TOTAL / 0x100);
                m->invincTimer = MAX(m->invincTimer, 30);
                return TRUE;
            }
            m->marioObj->oMarioBurnTimer = 0;
            update_mario_sound_and_camera(m);
            play_sound(SOUND_MARIO_ON_FIRE, m->marioObj->header.gfx.cameraToObject);

            if ((m->action & ACT_FLAG_AIR) && m->vel[1] <= 0.0f) {
                burningAction = ACT_BURNING_FALL;
            }

            return drop_and_set_mario_action(m, burningAction, 1);
        }
    }

    return FALSE;
}

u32 interact_snufit_bullet(struct MarioState *m, UNUSED u32 interactType, struct Object *obj) {
    if (!sInvulnerable && !(m->flags & MARIO_VANISH_CAP)) {
        if (m->flags & MARIO_METAL_CAP) {
            obj->oInteractStatus = INT_STATUS_INTERACTED | INT_STATUS_WAS_ATTACKED;
            play_sound(SOUND_ACTION_SNUFFIT_BULLET_HIT_METAL, m->marioObj->header.gfx.cameraToObject);
        } else {
            obj->oInteractStatus = INT_STATUS_INTERACTED | INT_STATUS_ATTACKED_MARIO;
            m->interactObj = obj;
            take_damage_from_interact_object(m);

            play_sound(SOUND_MARIO_ATTACKED, m->marioObj->header.gfx.cameraToObject);
            update_mario_sound_and_camera(m);

            if (mario_protects_from_knockback(m)) {
                m->invincTimer = MAX(m->invincTimer, 30);
                return TRUE;
            }
            return drop_and_set_mario_action(m, determine_knockback_action(m, obj->oDamageOrCoinValue),
                                             obj->oDamageOrCoinValue);
        }
    }

    if (!(obj->oInteractionSubtype & INT_SUBTYPE_DELAY_INVINCIBILITY)) {
        sDelayInvincTimer = TRUE;
    }

    return FALSE;
}

u32 interact_clam_or_bubba(struct MarioState *m, UNUSED u32 interactType, struct Object *obj) {
    if (obj->oInteractionSubtype & INT_SUBTYPE_EATS_MARIO) {
        obj->oInteractStatus = INT_STATUS_INTERACTED;
        m->interactObj = obj;
        return set_mario_action(m, ACT_EATEN_BY_BUBBA, 0);
    } else if (take_damage_and_knock_back(m, obj)) {
        return TRUE;
    }

    if (!(obj->oInteractionSubtype & INT_SUBTYPE_DELAY_INVINCIBILITY)) {
        sDelayInvincTimer = TRUE;
    }

    return TRUE;
}

static void mario_tick_contact_immunities(struct MarioState *m) {
    if (m->bullyKnockbackImmunityFrames > 0) {
        m->bullyKnockbackImmunityFrames--;
    }
    if (m->heaveHoLaunchImmunityFrames > 0) {
        m->heaveHoLaunchImmunityFrames--;
    }
}

static void mario_start_heave_ho_recovery_immunity(struct MarioState *m) {
    if (!m->heaveHoThrowRecoveryPending) {
        return;
    }
    if (m->health < 0x100) {
        m->heaveHoThrowRecoveryPending = FALSE;
    } else if (!mario_is_knocked_back(m->action) && !sInvulnerable
               && !(m->action & ACT_FLAG_INTANGIBLE)) {
        // Start on the first interaction update when a Heave-Ho could grab again.
        // Knockback and ordinary invincibility must not consume the five updates.
        m->heaveHoThrowRecoveryPending = FALSE;
        m->heaveHoLaunchImmunityFrames = 5;
    }
}

static struct Object *dive_hit_target(struct MarioState *m, struct Object *obj, u32 interaction) {
    if ((m->action != ACT_DIVE && m->action != ACT_DIVE_SLIDE)
        || interaction != INT_FAST_ATTACK_OR_SHELL) {
        return NULL;
    }

    // Attached segments share one enemy's record, even after a Pokey part detaches.
    if (obj_has_behavior(obj, bhvWigglerBody) || obj_has_behavior(obj, bhvPokeyBodyPart)) {
        return obj->parentObj != NULL ? obj->parentObj : obj;
    }
    if (obj->oInteractType == INTERACT_BULLY || obj_has_behavior(obj, bhvWigglerHead)) {
        return obj;
    }
    s32 goomba = obj_has_behavior(obj, bhvGoomba);
#ifdef FLOOMBAS
    goomba = goomba || obj_has_behavior(obj, bhvFloomba);
#endif
    return goomba && obj->oGoombaSize == GOOMBA_SIZE_HUGE ? obj : NULL;
}

static u32 determine_enemy_interaction(struct MarioState *m, struct Object *obj) {
    u32 interaction =
        (m->flags & MARIO_METAL_CAP) ? INT_FAST_ATTACK_OR_SHELL : determine_interaction(m, obj);
    struct Object *target = dive_hit_target(m, obj, interaction);

    if (target == NULL && obj->oInteractType == INTERACT_BULLY && sPunchContacts.phase != 0) {
        target = obj;
    }
    if (target != NULL && punch_hit_object(target)) {
        return INT_NONE;
    }
    return interaction;
}

static void remember_enemy_dive_hit(struct MarioState *m, struct Object *obj, u32 interaction) {
    struct Object *target = dive_hit_target(m, obj, interaction);
    if (target == NULL && obj->oInteractType == INTERACT_BULLY && sPunchContacts.phase != 0) {
        target = obj;
    }
    if (target != NULL && (interaction & INT_ATTACK_NOT_FROM_BELOW)) {
        remember_punch_hit_object(target);
    }
}

u32 interact_bully(struct MarioState *m, UNUSED u32 interactType, struct Object *obj) {
    u32 interaction = determine_enemy_interaction(m, obj);

    m->interactObj = obj;

    // Running speed can classify a passive jump as a fast attack; landing on a
    // bully still bounces Mario without launching the bully.
    if ((m->action & ACT_FLAG_AIR) && !(m->action & ACT_FLAG_ATTACKING) && m->vel[1] < 0.0f
        && m->pos[1] > obj->oPosY) {
        bounce_off_enemy(m, obj, 30.0f);
        return TRUE;
    }

    if (interaction & INT_ATTACK_NOT_FROM_BELOW) {
        queue_rumble_data(5, 80);
        push_mario_out_of_object(m, obj, 5.0f);

        if (!mario_protects_from_knockback(m)) {
            m->forwardVel = -16.0f;
        }
        if (mario_is_breakdancing(m)) {
            obj->oMoveAngleYaw = mario_obj_angle_to_object(m, obj);
        } else {
            obj->oMoveAngleYaw = m->faceAngle[1];
        }
        obj->oForwardVel = 3392.0f / obj->hitboxRadius;

        attack_object(obj, interaction);
        remember_enemy_dive_hit(m, obj, interaction);
        if (!(interaction & (INT_PUNCH | INT_KICK | INT_SLIDE_KICK)) || sInvulnerable
            || (m->flags & MARIO_VANISH_CAP)
            || (obj->oInteractionSubtype & INT_SUBTYPE_DELAY_INVINCIBILITY)) {
            bounce_back_from_attack(m, interaction);
            return TRUE;
        }
    }
    if (!sInvulnerable && m->bullyKnockbackImmunityFrames == 0 && !(m->flags & MARIO_VANISH_CAP)
        && !(obj->oInteractionSubtype & INT_SUBTYPE_DELAY_INVINCIBILITY)) {
        obj->oInteractStatus |= INT_STATUS_INTERACTED;
        m->invincTimer = 2;

        update_mario_sound_and_camera(m);
        play_sound(SOUND_MARIO_EEUH, m->marioObj->header.gfx.cameraToObject);
        play_sound(SOUND_OBJ_BULLY_METAL, m->marioObj->header.gfx.cameraToObject);

        push_mario_out_of_object(m, obj, 5.0f);
        if (mario_protects_from_knockback(m)) {
            return TRUE;
        }
        s32 twirlHit = mario_ground_twirl_is_active(m);
        u32 knockbackAction = bully_knock_back_mario(m);
        if (twirlHit) {
            f32 reducedSpeed = m->forwardVel * GROUND_TWIRL_LARGE_ENEMY_KNOCKBACK_FACTOR;
            if (obj->oBehParams2ndByte == BULLY_BP_SIZE_SMALL) {
                reducedSpeed = CLAMP(reducedSpeed, -GROUND_TWIRL_SMALL_BULLY_KNOCKBACK_CAP,
                                     GROUND_TWIRL_SMALL_BULLY_KNOCKBACK_CAP);
            }
            mario_set_forward_vel(m, reducedSpeed);
        }
        drop_and_set_mario_action(m, knockbackAction, 0);
        if (twirlHit) {
            // Keep the old twirl rebound's recovery after the regular Bully collision.
            m->actionState = GROUND_TWIRL_IMMUNE_REBOUND_STATE;
            sTwirlContact.reboundActive = TRUE;
            sTwirlContact.reboundTouchedEnemy = mario_ground_twirl_rebound_touches_enemy(m);
            sTwirlContact.reboundHiddenTimer = 0;
        }
        m->bullyKnockbackActive = TRUE;
        m->bullyKnockbackBuffer = 0;
        queue_rumble_data(5, 80);
        return TRUE;
    }

    if (m->bullyKnockbackImmunityFrames != 0) {
        push_mario_out_of_object(m, obj, 5.0f);
    }
    return FALSE;
}

u32 interact_shock(struct MarioState *m, UNUSED u32 interactType, struct Object *obj) {
    if (!sInvulnerable && !((m->flags & MARIO_VANISH_CAP) && game_options_get(GO_VANISH_CAP) == 1)
        && !(obj->oInteractionSubtype & INT_SUBTYPE_DELAY_INVINCIBILITY)) {
        u32 actionArg = mario_is_on_floor(m) && m->vel[1] <= 0.0f;
        u32 damage;
        s32 vanishCap = (m->flags & MARIO_VANISH_CAP) != 0;
        // A previous shock can consume Soaked without removing water contact.
        s32 wetShock = mario_is_soaked(m)
                       || (game_options_get(GO_SOAKED) && mario_has_water_contact(m, m->waterLevel));
        s32 waterShock = !vanishCap
                         && ((m->action & (ACT_FLAG_SWIMMING | ACT_FLAG_METAL_WATER))
                             || (m->flags & MARIO_METAL_CAP))
                         && m->pos[1] <= m->waterLevel - 76.0f;

        obj->oInteractStatus = INT_STATUS_INTERACTED | INT_STATUS_ATTACKED_MARIO;
        m->interactObj = obj;

        // Electric shocks never knock off Mario's cap, even when fatal.
        damage = take_damage_from_interact_object(m);
        if (wetShock) {
            // The dry portion is already queued; cap the complete wet hit at seven.
            m->hurtCounter += 4 * (MIN(2 * damage, 7u) - damage);
        }
        if (wetShock && damage != 0) {
            mario_consume_soaked_status(m);
        }
        if (damage != 0 && (m->flags & MARIO_SPECIAL_CAPS)) {
            // Apply the shock with the powers that were active at contact,
            // then remove every active power without losing the regular cap.
            player_stats_cap_powers_removed(mario_natural_cap_powers(m), FALSE);
            mario_remove_cap_powers(m);
        }
        play_sound(SOUND_MARIO_ATTACKED, m->marioObj->header.gfx.cameraToObject);
        queue_rumble_data(70, 60);

        if (mario_protects_from_knockback(m)) {
            m->invincTimer = MAX(m->invincTimer, 30);
            return TRUE;
        }
        if (waterShock) {
            return drop_and_set_mario_action(m, ACT_WATER_SHOCKED, 0);
        } else {
            update_mario_sound_and_camera(m);
            return drop_and_set_mario_action(m, ACT_SHOCKED, actionArg);
        }
    }

    if (!(obj->oInteractionSubtype & INT_SUBTYPE_DELAY_INVINCIBILITY)) {
        sDelayInvincTimer = TRUE;
    }

    return FALSE;
}

UNUSED static u32 interact_stub(UNUSED struct MarioState *m, UNUSED u32 interactType, struct Object *obj) {
    if (!(obj->oInteractionSubtype & INT_SUBTYPE_DELAY_INVINCIBILITY)) {
        sDelayInvincTimer = TRUE;
    }
    return FALSE;
}

u32 interact_mr_blizzard(struct MarioState *m, UNUSED u32 interactType, struct Object *obj) {
    if (take_damage_and_knock_back(m, obj)) {
        if (obj_has_behavior(obj, bhvWaterBomb) && obj->oAction == WATER_BOMB_ACT_DROP) {
            mario_apply_soaked_status(m);
        }
        return TRUE;
    }

    if (!(obj->oInteractionSubtype & INT_SUBTYPE_DELAY_INVINCIBILITY)) {
        sDelayInvincTimer = TRUE;
    }

    return FALSE;
}

u32 interact_hit_from_below(struct MarioState *m, UNUSED u32 interactType, struct Object *obj) {
    u32 interaction = determine_enemy_interaction(m, obj);

    if (interaction & INT_ANY_ATTACK) {
        queue_rumble_data(5, 80);
        attack_object(obj, interaction);
        remember_enemy_dive_hit(m, obj, interaction);
        if (take_knockback_after_punch_or_kick(m, obj, interaction)) {
            return TRUE;
        }
        bounce_back_from_attack(m, interaction);

        if (interaction & INT_HIT_FROM_BELOW) {
            hit_object_from_below(m, obj);
        }

        if (interaction & INT_HIT_FROM_ABOVE) {
            if (obj->oInteractionSubtype & INT_SUBTYPE_TWIRL_BOUNCE) {
                bounce_off_enemy(m, obj, 80.0f);
                reset_mario_pitch(m);
                play_sound(SOUND_MARIO_TWIRL_BOUNCE, m->marioObj->header.gfx.cameraToObject);
                return drop_and_set_mario_action(m, ACT_TWIRLING, 0);
            } else {
                bounce_off_enemy(m, obj, 30.0f);
            }
        }
    } else if (take_damage_and_knock_back(m, obj)) {
        return TRUE;
    }

    if (!(obj->oInteractionSubtype & INT_SUBTYPE_DELAY_INVINCIBILITY)) {
        sDelayInvincTimer = TRUE;
    }

    return FALSE;
}

u32 interact_bounce_top(struct MarioState *m, UNUSED u32 interactType, struct Object *obj) {
    u32 interaction = determine_enemy_interaction(m, obj);

    if (interaction & INT_ATTACK_NOT_FROM_BELOW) {
        queue_rumble_data(5, 80);
        attack_object(obj, interaction);
        remember_enemy_dive_hit(m, obj, interaction);
        if (take_knockback_after_punch_or_kick(m, obj, interaction)) {
            return TRUE;
        }
        bounce_back_from_attack(m, interaction);

        if (jump_after_ground_pound_enemy_hit(m, obj, interaction)) {
            return TRUE;
        }

        if (interaction & INT_HIT_FROM_ABOVE) {
            if (obj->oInteractionSubtype & INT_SUBTYPE_TWIRL_BOUNCE) {
                bounce_off_enemy(m, obj, 80.0f);
                reset_mario_pitch(m);
                play_sound(SOUND_MARIO_TWIRL_BOUNCE, m->marioObj->header.gfx.cameraToObject);
                return drop_and_set_mario_action(m, ACT_TWIRLING, 0);
            } else {
                bounce_off_enemy(m, obj, 30.0f);
            }
        }
    } else if (take_damage_and_knock_back(m, obj)) {
        return TRUE;
    }

    if (!(obj->oInteractionSubtype & INT_SUBTYPE_DELAY_INVINCIBILITY)) {
        sDelayInvincTimer = TRUE;
    }

    return FALSE;
}

u32 interact_spiny_walking(struct MarioState *m, UNUSED u32 interactType, struct Object *obj) {
    u32 interaction = determine_interaction(m, obj);

    if (interaction & INT_PUNCH) {
        obj->oInteractStatus = INT_STATUS_INTERACTED | INT_STATUS_WAS_ATTACKED | ATTACK_PUNCH;
        if (take_knockback_after_punch_or_kick(m, obj, interaction)) {
            return TRUE;
        }
        bounce_back_from_attack(m, interaction);
    } else if (take_damage_and_knock_back(m, obj)) {
        return TRUE;
    }

    if (!(obj->oInteractionSubtype & INT_SUBTYPE_DELAY_INVINCIBILITY)) {
        sDelayInvincTimer = TRUE;
    }

    return FALSE;
}

u32 interact_damage(struct MarioState *m, UNUSED u32 interactType, struct Object *obj) {
    // Blast exposure dries Mario even if the hit cannot damage him. Water-bomb
    // splashes and Mario-owned visual blasts do not use these damaging contacts.
    if (obj_has_behavior(obj, bhvExplosion) || obj_has_behavior(obj, bhvBowserBomb)) {
        mario_consume_soaked_status(m);
    }
    // Ordinary explosions are harmless to Metal; Bowser attacks bypass this protection.
    // Ignore the entire contact so Metal keeps its action, momentum and invincibility timer.
    if (mario_has_current_metal_cap(m) && obj_has_behavior(obj, bhvExplosion)
        && !(obj->oInteractionSubtype & INT_SUBTYPE_BOWSER_ATTACK)) {
        return FALSE;
    }
    if (take_damage_and_knock_back(m, obj)) {
        return TRUE;
    }

    if (!(obj->oInteractionSubtype & INT_SUBTYPE_DELAY_INVINCIBILITY)) {
        sDelayInvincTimer = TRUE;
    }

    return FALSE;
}

u32 interact_breakable(struct MarioState *m, UNUSED u32 interactType, struct Object *obj) {
    u32 interaction = determine_interaction(m, obj);

    // Swimming's grab/punch animation can strike supply boxes during its reach.
    if (obj->behavior == segmented_to_virtual(bhvExclamationBox) && m->action == ACT_WATER_PUNCH
        && m->actionState == ACT_STATE_WATER_PUNCH_START) {
        s16 dYaw = mario_obj_angle_to_object(m, obj) - m->faceAngle[1];
        if (-0x2AAA <= dYaw && dYaw <= 0x2AAA) {
            interaction = INT_PUNCH;
        }
    }

    if (interaction & INT_ATTACK_NOT_WEAK_FROM_ABOVE) {
        attack_object(obj, interaction);
        bounce_back_from_attack(m, interaction);

        m->interactObj = obj;

        switch (interaction) {
            case INT_HIT_FROM_ABOVE:
                bounce_off_object(m, obj, 30.0f); //! Not in the 0x8F mask
                break;

            case INT_HIT_FROM_BELOW:
                hit_object_from_below(m, obj);
                break;
        }

        return TRUE;
    }

    return FALSE;
}

void mario_attack_cap_box_surface(struct MarioState *m, struct Surface *surface, u32 interaction) {
    struct Object *box = surface != NULL ? surface->object : NULL;
    // Solid collision can stop Mario before the next object hitbox pass.
    if (box != NULL && box->behavior == segmented_to_virtual(bhvExclamationBox)
        && box->oAction == EXCLAMATION_BOX_ACT_ACTIVE && box->oIntangibleTimer == 0
        && !(m->action & ACT_FLAG_INTANGIBLE)) {
        attack_object(box, interaction);
        he_thicc_attack_contact(m, interaction);
        m->interactObj = box;
    }
}

void mario_attack_breakable_box_wall(struct MarioState *m) {
    struct Object *box = m->wall != NULL ? m->wall->object : NULL;

    if (box != NULL && (box->oInteractType & INTERACT_BREAKABLE) && box->oIntangibleTimer == 0
        && (box->behavior == segmented_to_virtual(bhvBreakableBox)
            || box->behavior == segmented_to_virtual(bhvExclamationBox)
            || box->behavior == segmented_to_virtual(bhvHiddenObject))) {
        box->oInteractStatus = INT_STATUS_INTERACTED | INT_STATUS_WAS_ATTACKED | ATTACK_FAST_ATTACK;
    }
}

u32 interact_koopa_shell(struct MarioState *m, UNUSED u32 interactType, struct Object *obj) {
    if (!(m->action & ACT_FLAG_RIDING_SHELL)) {
        u32 interaction = determine_interaction(m, obj);

        if (interaction == INT_HIT_FROM_ABOVE || m->action == ACT_WALKING
            || m->action == ACT_HOLD_WALKING) {
            m->interactObj = obj;
            m->usedObj = obj;
            m->riddenObj = obj;
            player_stats_add(PS_KOOPA_SHELLS_MOUNTED, 1);

            obj_set_blink_phase(obj, FALSE);
            obj->header.gfx.node.flags |= GRAPH_RENDER_ACTIVE;
            attack_object(obj, interaction);
            update_mario_sound_and_camera(m);
            play_shell_music();
            mario_drop_held_object(m);

            //! Puts Mario in ground action even when in air, making it easy to
            // escape air actions into crouch slide (shell cancel)
            return set_mario_action(m, ACT_RIDING_SHELL_GROUND, 0);
        }

        push_mario_out_of_object(m, obj, 2.0f);
    }

    return FALSE;
}

u32 check_object_grab_mario(struct MarioState *m, UNUSED u32 interactType, struct Object *obj) {
    if (mario_blocks_harmful_knockback(m)) {
        return FALSE;
    }
    if ((!(m->action & (ACT_FLAG_AIR | ACT_FLAG_INVULNERABLE | ACT_FLAG_ATTACKING)) || !sInvulnerable
         || (mario_ground_twirl_is_active(m) && obj->behavior == segmented_to_virtual(bhvChuckya)))
        && (obj->oInteractionSubtype & INT_SUBTYPE_GRABS_MARIO)
        && !(m->heaveHoLaunchImmunityFrames != 0
             && obj->behavior == segmented_to_virtual(bhvHeaveHo))) {
        if (object_facing_mario(m, obj, DEGREES(60))) {
            mario_stop_riding_and_holding(m);
            obj->oInteractStatus = INT_STATUS_INTERACTED | INT_STATUS_GRABBED_MARIO;

            m->faceAngle[1] = obj->oMoveAngleYaw;
            m->interactObj  = obj;
            m->usedObj      = obj;

            update_mario_sound_and_camera(m);
            play_sound(SOUND_MARIO_OOOF, m->marioObj->header.gfx.cameraToObject);
            queue_rumble_data(5, 80);
            return set_mario_action(m, ACT_GRABBED, 0);
        }
    }

    push_mario_out_of_object(m, obj, -5.0f);
    return FALSE;
}

u32 interact_pole(struct MarioState *m, UNUSED u32 interactType, struct Object *obj) {
    s32 actionId = m->action & ACT_ID_MASK;
    if ((actionId >= ACT_GROUP_AIRBORNE && actionId < (ACT_HOLD_JUMP & ACT_ID_MASK))
        || m->action == ACT_JUMP_KICK) {
        if (!(m->prevAction & ACT_FLAG_ON_POLE) || m->usedObj != obj) {
#if defined(VERSION_SH) || defined(SHINDOU_POLES)
            f32 velConv = m->forwardVel; // conserve the velocity.
            struct Object *marioObj = m->marioObj;
            u32 lowSpeed;
#else
            u32 lowSpeed = (m->forwardVel <= 10.0f);
            struct Object *marioObj = m->marioObj;
#endif

            mario_stop_riding_and_holding(m);

#if defined(VERSION_SH) || defined(SHINDOU_POLES)
            lowSpeed = !(m->action == ACT_DIVE && velConv > 0.0f)
                       && (velConv <= 32.0f || !mario_has_dive_input(m));
#endif

            if (m->action == ACT_JUMP_KICK) {
                m->flags &= ~MARIO_KICKING;
                m->faceAngle[0] = 0;
                marioObj->header.gfx.angle[0] = 0;
            }

            m->interactObj = obj;
            m->usedObj = obj;
            m->vel[1] = 0.0f;
            m->forwardVel = 0.0f;

            // Pole fix
            // If mario is beneath the pole, clamp mario's position to the down-offset of the pole (bottom)
            marioObj->oMarioPolePos = ((m->pos[1] - obj->oPosY) < 0) ? -obj->hitboxDownOffset : (m->pos[1] - obj->oPosY);

            if (lowSpeed) {
                return set_mario_action(m, ACT_GRAB_POLE_SLOW, 0);
            }

#if defined(VERSION_SH) || defined(SHINDOU_POLES)
            m->angleVel[1] = (s32)(velConv * 0x100 + 0x1000);
#else
            m->angleVel[1] = 0x1000;
#endif
            reset_mario_pitch(m);
            queue_rumble_data(5, 80);
            return set_mario_action(m, ACT_GRAB_POLE_FAST, 0);
        }
    }

    return FALSE;
}

u32 interact_hoot(struct MarioState *m, UNUSED u32 interactType, struct Object *obj) {
    s32 actionId = m->action & ACT_ID_MASK;

    //! Can pause to advance the global timer without falling too far, allowing
    // you to regrab after letting go.
    if (
        actionId >= (ACT_JUMP & ACT_ID_MASK)
        && actionId <  (ACT_SHOT_FROM_CANNON & ACT_ID_MASK)
        && gGlobalTimer - m->usedObj->oHootMarioReleaseTime > 30
    ) {
        mario_stop_riding_and_holding(m);

        obj->oInteractStatus = TRUE; //! Note: Not a flag, treated as a TRUE/FALSE statement
        m->interactObj = obj;
        m->usedObj     = obj;

        queue_rumble_data(5, 80);
        update_mario_sound_and_camera(m);
        return set_mario_action(m, ACT_RIDING_HOOT, 0);
    }

    return FALSE;
}

// Recover the regular cap when collecting any cap, including its saved and live owners.
static void restore_normal_cap(struct MarioState *m, struct Object *collectedCap) {
    s32 list;

    m->flags |= MARIO_NORMAL_CAP;
    save_file_clear_flags(SAVE_FLAG_CAP_ON_GROUND | SAVE_FLAG_CAP_ON_KLEPTO | SAVE_FLAG_CAP_ON_UKIKI
                          | SAVE_FLAG_CAP_ON_MR_BLIZZARD);

    for (list = 0; list < NUM_OBJ_LISTS; list++) {
        struct Object *head = (struct Object *) &gObjectLists[list];
        struct Object *obj = (struct Object *) head->header.next;
        for (; obj != head; obj = (struct Object *) obj->header.next) {
            if (obj->activeFlags == ACTIVE_FLAG_DEACTIVATED) {
                continue;
            }
            if (obj->behavior == segmented_to_virtual(bhvNormalCap)) {
                // Let the collected cap finish its normal pickup cleanup.
                if (obj != collectedCap) {
                    obj_mark_for_deletion(obj);
                }
            } else if (obj->behavior == segmented_to_virtual(bhvKlepto)) {
                if (obj->oAnimState == KLEPTO_ANIM_STATE_HOLDING_CAP) {
                    obj->oAnimState = KLEPTO_ANIM_STATE_HOLDING_NOTHING;
                }
            } else if (obj->behavior == segmented_to_virtual(bhvUkiki)) {
                if (obj->oUkikiHasCap & UKIKI_CAP_ON) {
                    obj->oUkikiHasCap &= ~UKIKI_CAP_ON;
                    obj->oUkikiTextState = UKIKI_TEXT_DEFAULT;
                    obj->oAnimState = UKIKI_ANIM_STATE_DEFAULT;
                }
            } else if (obj->behavior == segmented_to_virtual(bhvMrBlizzard)) {
                obj->oAnimState = MR_BLIZZARD_ANIM_STATE_NO_CAP;
            }
        }
    }
}

static s32 mario_cap_pickup_is_blocked(struct MarioState *m, u32 capFlag) {
    // A cap cannot interrupt death or a queued lethal hit. Power-up caps also
    // respect damage and invulnerability states.
    return (!(m->flags & MARIO_NORMAL_CAP) && m->capPickupLockFrames != 0) || m->health < 0x100
           || mario_health_after_pending_damage(m) < 0x100 || (m->action & ACT_FLAG_INTANGIBLE)
           || m->action == ACT_GETTING_BLOWN || mario_is_knocked_back(m->action)
           || (capFlag != MARIO_NORMAL_CAP
               && (m->hurtCounter != 0
                   || ((m->action & ACT_FLAG_INVULNERABLE)
                       && (m->action != ACT_GROUND_TWIRL || mario_ground_twirl_is_active(m)))));
}

u32 interact_cap(struct MarioState *m, UNUSED u32 interactType, struct Object *obj) {
    u32 capFlag = get_mario_cap_flag(obj);
    s32 wasCapless = !(m->flags & MARIO_NORMAL_CAP);
    s32 pickedUpWhileCapless = !(m->flags & MARIO_CAPS);
    f32 heightAboveFloor = m->pos[1] - m->floorHeight;
    s32 snapCaplessPickupToFloor = pickedUpWhileCapless && m->floor != NULL && heightAboveFloor >= 0.0f
                                   && heightAboveFloor <= 16.0f;
    u16 capMusic = 0;
    u16 capTime = 0;

    if (mario_cap_pickup_is_blocked(m, capFlag)) {
        return FALSE;
    }

    if (capFlag != 0) {
        if (capFlag == MARIO_WING_CAP) {
            player_stats_add(PS_WING_CAPS_COLLECTED, 1);
        } else if (capFlag == MARIO_METAL_CAP) {
            player_stats_add(PS_METAL_CAPS_COLLECTED, 1);
        } else if (capFlag == MARIO_VANISH_CAP) {
            player_stats_add(PS_VANISH_CAPS_COLLECTED, 1);
        }
        if (wasCapless) {
            player_stats_cap_recovered();
        }
        m->interactObj = obj;
        obj->oInteractStatus = INT_STATUS_INTERACTED;

        if (!(m->flags & MARIO_NORMAL_CAP)) {
            restore_normal_cap(m, obj);
        }
        m->flags &= ~MARIO_CAP_ON_HEAD & ~MARIO_CAP_IN_HAND;
        mario_add_cap_power(m, capFlag);
        if (capFlag == MARIO_VANISH_CAP && mario_has_current_vanish_cap(m)) {
            mario_consume_soaked_status(m);
        }

        switch (capFlag) {
            case MARIO_VANISH_CAP: capTime =  600; capMusic = SEQUENCE_ARGS(4, SEQ_EVENT_POWERUP  ); break;
            case MARIO_METAL_CAP:  capTime =  600; capMusic = SEQUENCE_ARGS(4, SEQ_EVENT_METAL_CAP); break;
            case MARIO_WING_CAP:   capTime = 1800; capMusic = SEQUENCE_ARGS(4, SEQ_EVENT_POWERUP  ); break;
        }

        if (gCurrLevelNum == LEVEL_PHYSICS_LAB && capTime != 0) {
            capTime = 120 * 30; // Two minutes at 30 gameplay updates per second.
        }

        if (capTime != 0) {
            hud_reveal_cap_timer();
        }

        if (capTime > m->capTimer) {
            m->capTimer = capTime;
        }

        if ((m->action & ACT_FLAG_IDLE) || m->action == ACT_WALKING
            || (wasCapless && !(m->action & ACT_FLAG_INVULNERABLE)
                && ((m->action & ACT_GROUP_MASK) == ACT_GROUP_STATIONARY
                    || (m->action & ACT_GROUP_MASK) == ACT_GROUP_MOVING))) {
            m->flags |= MARIO_CAP_IN_HAND;
            set_mario_action(m, ACT_PUTTING_ON_CAP, capFlag == MARIO_NORMAL_CAP || wasCapless ? 2 : 0);
        } else if ((capFlag == MARIO_NORMAL_CAP || wasCapless) && (m->action & ACT_FLAG_AIR)
                   && !(m->action & ACT_FLAG_INVULNERABLE)) {
            m->flags |= MARIO_CAP_IN_HAND;
            u32 pickupArg;
            if (snapCaplessPickupToFloor) {
                pickupArg = 2;
            } else {
                pickupArg =
                    pickedUpWhileCapless ? ACT_ARG_CAP_PICKUP_CAPLESS : ACT_ARG_CAP_PICKUP_REGULAR;
            }
            set_mario_action(m, snapCaplessPickupToFloor ? ACT_PUTTING_ON_CAP : ACT_PUTTING_ON_CAP_AIR,
                             pickupArg);
        } else {
            m->flags |= MARIO_CAP_ON_HEAD;
        }

        if (pickedUpWhileCapless) {
            if (m->forwardVel < 0.0f) {
                mario_set_forward_vel(m, 0.0f);
            }
            if (m->vel[1] < 0.0f) {
                m->vel[1] = 0.0f;
            }
            if (snapCaplessPickupToFloor) {
                m->pos[1] = m->floorHeight;
                m->vel[1] = 0.0f;
            }
        }

        play_sound(SOUND_MENU_STAR_SOUND, m->marioObj->header.gfx.cameraToObject);
        play_sound(SOUND_MARIO_HERE_WE_GO, m->marioObj->header.gfx.cameraToObject);

        if (capMusic != 0) {
            if (m->flags & MARIO_VANISH_CAP) {
                capMusic = SEQUENCE_ARGS(4, SEQ_EVENT_POWERUP);
            }
            play_cap_music(capMusic);
        }

        return TRUE;
    }

    return FALSE;
}

u32 interact_grabbable(struct MarioState *m, u32 interactType, struct Object *obj) {
    const BehaviorScript *script = virtual_to_segmented(SEGMENT_BEHAVIOR_DATA, obj->behavior);

    // A just-released object can still overlap Mario during the throw animation.
    if ((m->action == ACT_THROWING || m->action == ACT_HEAVY_THROW) && obj == m->usedObj
        && m->heldObj == NULL) {
        return FALSE;
    }

    if (script == bhvKoopaShellUnderwater && m->heldObj == NULL && m->riddenObj == NULL
        && obj->oHeldState == HELD_FREE && obj->oIntangibleTimer == 0
        && !(obj->oInteractStatus & INT_STATUS_STOP_RIDING) && !(m->action & ACT_FLAG_RIDING_SHELL)
        && mario_has_land_movement_underwater(m) && m->pos[1] < m->waterLevel && m->health >= 0x100
        && ((m->action & ACT_GROUP_MASK) == ACT_GROUP_STATIONARY
            || (m->action & ACT_GROUP_MASK) == ACT_GROUP_MOVING
            || (m->action & ACT_GROUP_MASK) == ACT_GROUP_AIRBORNE
            || (m->action & ACT_GROUP_MASK) == ACT_GROUP_SUBMERGED)) {
        m->interactObj = obj;
        m->usedObj = obj;
        m->riddenObj = obj;
        obj->oInteractStatus = INT_STATUS_INTERACTED;
        update_mario_sound_and_camera(m);
        play_shell_music();
        return set_mario_action(m, ACT_RIDING_SHELL_GROUND, 0);
    }

    if (obj->oInteractionSubtype & INT_SUBTYPE_KICKABLE) {
        u32 interaction = determine_interaction(m, obj);
        if (interaction & (INT_KICK | INT_TRIP)) {
            attack_object(obj, interaction);
            bounce_back_from_attack(m, interaction);
            return FALSE;
        }
    }

    if ((obj->oInteractionSubtype & INT_SUBTYPE_GRABS_MARIO) && obj->oGroundTwirlStunTimer == 0) {
        if (check_object_grab_mario(m, interactType, obj)) {
            return TRUE;
        }
    }

    if (able_to_grab_object(m, obj)) {
        if (!(obj->oInteractionSubtype & INT_SUBTYPE_NOT_GRABBABLE)) {
            m->interactObj = obj;
            m->input |= INPUT_INTERACT_OBJ_GRABBABLE;
            return TRUE;
        }
    }

    if (script != bhvBowser) {
        push_mario_out_of_object(m, obj, -5.0f);
    }

    return FALSE;
}

// This flag skips the animation restriction within eligible movement actions.
// It does not bypass the stick threshold or admit otherwise excluded actions.
u32 mario_can_talk(struct MarioState *m, u32 allowAnyMovementAnimation) {
    if (m->controller->stickMag >= MARIO_STRONG_STICK_THRESHOLD) {
        return FALSE;
    }

#ifdef EASIER_DIALOG_TRIGGER
    if (m->action & (
        ACT_FLAG_AIR
        | ACT_FLAG_SHORT_HITBOX
        | ACT_FLAG_RIDING_SHELL
        | ACT_FLAG_BUTT_OR_STOMACH_SLIDE
        | ACT_FLAG_DIVING
        | ACT_FLAG_ON_POLE
        | ACT_FLAG_HANGING
        | ACT_FLAG_INTANGIBLE)
    ) {
         return FALSE;
    }

    if (m->action == ACT_FIRST_PERSON) {
        return FALSE;
    }

    if (m->action & (
        ACT_FLAG_IDLE
        | ACT_FLAG_STATIONARY
        | ACT_FLAG_ALLOW_FIRST_PERSON
    )) {
        return TRUE;
    }

    if (
        m->action == ACT_WALKING
        || m->action == ACT_DECELERATING
        || m->action == ACT_BRAKING
        || m->action == ACT_BRAKING_STOP) {
#else
    if (m->action & ACT_FLAG_IDLE) {
        return TRUE;
    }

    if (m->action == ACT_WALKING) {
#endif
        if (allowAnyMovementAnimation) {
            return TRUE;
        }

        s16 animID = m->marioObj->header.gfx.animInfo.animID;

        if (animID == MARIO_ANIM_SIDESTEP_RIGHT || animID == MARIO_ANIM_SIDESTEP_LEFT || animID == MARIO_ANIM_PUSHING) {
            return TRUE;
        }
    }

    return FALSE;
}

// Maximum facing-angle difference for sign and NPC interactions.
#define SIGN_RANGE DEGREES(110)

// Only a fresh B press commits the object and starts the sign-reading action.
u32 check_read_sign(struct MarioState *m, struct Object *obj) {
    if (m->pos[1] < m->waterLevel
        && (!mario_has_land_movement_underwater(m) || !mario_is_on_floor(m))) {
        return FALSE;
    }

#ifdef EASIER_DIALOG_TRIGGER
    s16 facingDYaw = (s16)(obj->oMoveAngleYaw + 0x8000) - m->faceAngle[1];
    if (
        mario_can_talk(m, TRUE)
        && object_facing_mario(m, obj, SIGN_RANGE)
        && (facingDYaw >= -SIGN_RANGE)
        && (facingDYaw <=  SIGN_RANGE)
        && abs_angle_diff(mario_obj_angle_to_object(m, obj), m->faceAngle[1]) <= SIGN_RANGE
    ) {
#ifdef DIALOG_INDICATOR
        struct Object *orangeNumber;
        if (obj->behavior == segmented_to_virtual(bhvSignOnWall)) {
            orangeNumber =
                spawn_object_relative(ORANGE_NUMBER_B, 0, 180, 32, obj, MODEL_NUMBER, bhvOrangeNumber);
        } else {
            orangeNumber =
                spawn_object_relative(ORANGE_NUMBER_B, 0, 160, 8, obj, MODEL_NUMBER, bhvOrangeNumber);
        }
        orangeNumber->oHomeX = orangeNumber->oPosX;
        orangeNumber->oHomeZ = orangeNumber->oPosZ;
#endif
        if (m->input & INPUT_B_PRESSED) {
#else
    if ((m->input & INPUT_B_PRESSED) && mario_can_talk(m, 0)
        && object_facing_mario(m, obj, SIGN_RANGE)) {
        s16 facingDYaw = (s16)(obj->oMoveAngleYaw + 0x8000) - m->faceAngle[1];
        if (facingDYaw >= -SIGN_RANGE && facingDYaw <= SIGN_RANGE) {
#endif
            f32 targetX = obj->oPosX + 105.0f * sins(obj->oMoveAngleYaw);
            f32 targetZ = obj->oPosZ + 105.0f * coss(obj->oMoveAngleYaw);

            m->marioObj->oMarioReadingSignDYaw = facingDYaw;
            m->marioObj->oMarioReadingSignDPosX = targetX - m->pos[0];
            m->marioObj->oMarioReadingSignDPosZ = targetZ - m->pos[2];

            m->interactObj = obj;
            m->usedObj     = obj;
            jrb_simpleflips_begin_sign_read(obj);
            return set_mario_action(m, ACT_READING_SIGN, 0);
        }
    }

    return FALSE;
}

// A fresh B press selects the NPC and starts waiting for its dialogue handler.
// Every attempt also separates Mario from the NPC, including rejected attempts.
u32 check_npc_talk(struct MarioState *m, struct Object *obj) {
#ifdef EASIER_DIALOG_TRIGGER
    if (
        mario_can_talk(m, TRUE)
        && abs_angle_diff(mario_obj_angle_to_object(m, obj), m->faceAngle[1]) <= SIGN_RANGE
    ) {
#ifdef DIALOG_INDICATOR
        struct Object *orangeNumber;
        if (obj->behavior == segmented_to_virtual(bhvYoshi)) {
            orangeNumber =
                spawn_object_relative(ORANGE_NUMBER_B, 0, 256, 64, obj, MODEL_NUMBER, bhvOrangeNumber);
        } else {
            orangeNumber =
                spawn_object_relative(ORANGE_NUMBER_B, 0, 160, 0, obj, MODEL_NUMBER, bhvOrangeNumber);
        }
        f32 guestHeight = castle_lobby_guest_focus_height(obj);
        if (orangeNumber != NULL) {
            if (guestHeight >= 0.0f) {
                orangeNumber->oPosY = obj->oPosY + guestHeight + 110.0f;
            }
            orangeNumber->oHomeX = orangeNumber->oPosX;
            orangeNumber->oHomeZ = orangeNumber->oPosZ;
        }
#endif
        if (m->input & INPUT_B_PRESSED) {
#else
    if ((m->input & INPUT_B_PRESSED) && mario_can_talk(m, 1)) {
        s16 facingDYaw = mario_obj_angle_to_object(m, obj) - m->faceAngle[1];
        if (facingDYaw >= -SIGN_RANGE && facingDYaw <= SIGN_RANGE) {
#endif
            obj->oInteractStatus = INT_STATUS_INTERACTED;

            m->interactObj = obj;
            m->usedObj     = obj;

            push_mario_out_of_object(m, obj, -10.0f);
            return set_mario_action(m, ACT_WAITING_FOR_DIALOG, 0);
        }
    }

    push_mario_out_of_object(m, obj, -10.0f);
    return FALSE;
}

u32 interact_text(struct MarioState *m, UNUSED u32 interactType, struct Object *obj) {
    u32 startedInteraction = FALSE;

    if (obj->oInteractionSubtype & INT_SUBTYPE_SIGN) {
        startedInteraction = check_read_sign(m, obj);
    } else if (obj->oInteractionSubtype & INT_SUBTYPE_NPC) {
        startedInteraction = check_npc_talk(m, obj);
    } else {
        push_mario_out_of_object(m, obj, 2.0f);
    }

    return startedInteraction;
}

void check_kick_or_punch_wall(struct MarioState *m) {
    if (m->flags & (MARIO_PUNCHING | MARIO_KICKING | MARIO_TRIPPING)) {
        struct WallCollisionData detector;
        detector.x = m->pos[0] + 50.0f * sins(m->faceAngle[1]);
        detector.z = m->pos[2] + 50.0f * coss(m->faceAngle[1]);
        detector.y = m->pos[1];
        detector.offsetY = 80.0f;
        detector.radius = 5.0f;

        if (find_wall_collisions(&detector) > 0) {
            if ((m->flags & MARIO_PUNCHING)
                || ((m->flags & MARIO_KICKING) && m->action == ACT_JUMP_KICK)
                || ((m->flags & MARIO_KICKING)
                    && (m->action == ACT_PUNCHING || m->action == ACT_MOVE_PUNCHING)
                    && m->actionArg == ACT_ARG_PUNCH_SEQUENCE_GROUND_KICK)
                || ((m->flags & MARIO_TRIPPING) && mario_is_breakdancing(m))) {
                s32 i;
                s32 newWall = FALSE;

                for (i = 0; i < detector.numWalls; i++) {
                    if (!punch_hit_wall(detector.walls[i])
                        && sPunchContacts.hitWallCount < MAX_PUNCH_HIT_WALLS) {
                        if (m->flags & MARIO_PUNCHING) {
                            mario_attack_cap_box_surface(m, detector.walls[i], INT_PUNCH);
                        }
                        remember_punch_hit_wall(detector.walls[i]);
                        newWall = TRUE;
                    }
                }
                if (!newWall) {
                    return;
                }
            }
            if (m->flags & MARIO_PUNCHING) {
                he_thicc_attack_contact(m, INT_PUNCH);
            } else if (m->flags & MARIO_KICKING) {
                he_thicc_attack_contact(m, INT_KICK);
            }
            if ((m->flags & MARIO_TRIPPING) && mario_is_breakdancing(m)) {
                play_sound(SOUND_ACTION_HIT_2, m->marioObj->header.gfx.cameraToObject);
                m->particleFlags |= PARTICLE_TRIANGLE;
            } else if (m->action != ACT_MOVE_PUNCHING || m->forwardVel >= 0.0f) {
                if (m->action == ACT_PUNCHING) {
                    m->action = ACT_MOVE_PUNCHING;
                }

                mario_set_forward_vel(m, -MAX(PUNCH_WALL_MIN_REBOUND_SPEED, m->forwardVel));
                // Do not let the remaining punch lunge erase this rebound next frame.
                if ((m->action == ACT_PUNCHING || m->action == ACT_MOVE_PUNCHING)
                    && m->actionArg != ACT_ARG_PUNCH_SEQUENCE_BREAKDANCE) {
                    m->actionTimer = 0;
                }
                play_sound(SOUND_ACTION_HIT_2, m->marioObj->header.gfx.cameraToObject);
                m->particleFlags |= PARTICLE_TRIANGLE;
            } else if (m->action & ACT_FLAG_AIR) {
                mario_set_forward_vel(m, -16.0f);
                play_sound(SOUND_ACTION_HIT_2, m->marioObj->header.gfx.cameraToObject);
                m->particleFlags |= PARTICLE_TRIANGLE;
            }
        }
    }
}

static s32 stunned_enemy_can_interact(struct MarioState *m, struct Object *obj, u32 interactType) {
    if (m->flags & MARIO_METAL_CAP) {
        // Let the Metal contact handler mark a real attack instead of the
        // preliminary push scan turning it into another harmless rebound.
        return TRUE;
    }
    u32 contact = determine_interaction(m, obj);
    u32 attack = contact;

    // A jump onto the enemy keeps its ordinary stomp, bounce, or hazard response.
    if (contact & INT_HIT_FROM_ABOVE) {
        return TRUE;
    }
    if (interactType == INTERACT_BOUNCE_TOP || interactType == INTERACT_BOUNCE_TOP2
        || interactType == INTERACT_KOOPA || interactType == INTERACT_BULLY) {
        return (attack & INT_ATTACK_NOT_FROM_BELOW) != 0;
    }
    if (interactType == INTERACT_HIT_FROM_BELOW) {
        return (attack & INT_ANY_ATTACK) != 0;
    }
    if (interactType == INTERACT_SPINY_WALKING) {
        return (attack & INT_PUNCH) != 0;
    }
    if (interactType == INTERACT_GRABBABLE) {
        if ((obj->oInteractionSubtype & INT_SUBTYPE_KICKABLE) && (attack & (INT_KICK | INT_TRIP))) {
            return TRUE;
        }
        return !(obj->oInteractionSubtype & INT_SUBTYPE_NOT_GRABBABLE) && able_to_grab_object(m, obj);
    }
    return FALSE;
}

#define GROUND_TWIRL_CONTACT_REBOUND_FRAMES 24
#define GROUND_TWIRL_CONTACT_REBOUND_BASE_SPEED 15.0f
#define GROUND_TWIRL_CONTACT_REBOUND_SPEED_SCALE 0.8f
#define GROUND_TWIRL_CONTACT_REBOUND_BASE_LIFT 12.0f
#define GROUND_TWIRL_CONTACT_REBOUND_LIFT_SCALE 0.25f

static void mario_rebound_stunned_enemy(struct MarioState *m, struct Object *obj) {
    f32 speed = sqrtf(sqr(m->vel[0]) + sqr(m->vel[2]));

    obj->oMoveAngleYaw =
        speed > 1.0f ? atan2s(m->vel[2], m->vel[0]) : mario_obj_angle_to_object(m, obj);
    obj->oForwardVel = MIN(65.0f, GROUND_TWIRL_CONTACT_REBOUND_BASE_SPEED
                                      + speed * GROUND_TWIRL_CONTACT_REBOUND_SPEED_SCALE);
    obj->oVelY = MIN(50.0f, GROUND_TWIRL_CONTACT_REBOUND_BASE_LIFT
                                + speed * GROUND_TWIRL_CONTACT_REBOUND_LIFT_SCALE);
    obj->oGroundTwirlStunTimer = 0;
    obj->oGroundTwirlContactRebound = GROUND_TWIRL_CONTACT_REBOUND_FRAMES;
    obj->oInteractStatus = INT_STATUS_NONE;
    mario_ground_twirl_reset_cooldown();
}

static s32 mario_ground_twirl_takes_hazard_damage(struct Object *obj, u32 interactType) {
    return (obj->oInteractionSubtype & INT_SUBTYPE_BOWSER_ATTACK) || interactType == INTERACT_FLAME
           || interactType == INTERACT_SHOCK
           || (interactType == INTERACT_DAMAGE
               && (obj->behavior == segmented_to_virtual(bhvExplosion)
                   || obj->behavior == segmented_to_virtual(bhvFallingPillarHitbox)));
}

static s32 damage_came_from_enemy(struct Object *obj) {
    const BehaviorScript *behavior = obj->behavior;

    return mario_ground_twirl_is_enemy(obj) || mario_ground_twirl_is_immune_enemy(obj)
           || behavior == segmented_to_virtual(bhvSnowmansBottom)
           || behavior == segmented_to_virtual(bhvSnufitBalls)
           || behavior == segmented_to_virtual(bhvMrIParticle)
           || behavior == segmented_to_virtual(bhvMrBlizzardSnowball)
           || behavior == segmented_to_virtual(bhvMontyMoleRock);
}

static s32 mario_ground_twirl_contact_can_damage(struct Object *obj) {
    // Enemy recoil retains its capless cost. Other hazards qualify through
    // their damage interaction, so behavior variants cannot miss this rule.
    return damage_came_from_enemy(obj)
           || (obj->oInteractType
               & (INTERACT_DAMAGE | INTERACT_MR_BLIZZARD | INTERACT_HIT_FROM_BELOW | INTERACT_BOUNCE_TOP
                  | INTERACT_BOUNCE_TOP2 | INTERACT_KOOPA | INTERACT_SPINY_WALKING
                  | INTERACT_SNUFIT_BULLET));
}

static void mario_ground_twirl_damage_capless_contact(struct MarioState *m) {
    if (mario_is_knocked_back(m->action) && !(m->flags & MARIO_CAP_ON_HEAD)
        && !sTwirlContact.caplessContactDamaged && m->health >= 0x100
        && sTwirlContact.reboundHiddenTimer == 0) {
        // One health wedge per twirl, even if several objects overlap Mario.
        player_stats_damage();
        m->hurtCounter += 4;
        sTwirlContact.caplessContactDamaged = TRUE;
        play_sound(SOUND_MARIO_ATTACKED, m->marioObj->header.gfx.cameraToObject);
        queue_rumble_data(5, 80);
    }
}

static s32 mario_ground_twirl_rebounds_from_object(struct Object *obj, u32 interactType) {
    if (mario_ground_twirl_is_large_enemy(obj)) {
        return TRUE;
    }
    if (mario_ground_twirl_is_enemy(obj) || mario_ground_twirl_takes_hazard_damage(obj, interactType)) {
        return FALSE;
    }
    // Rebound from enemies the twirl cannot hit, solid hazards, and projectiles.
    if (interactType == INTERACT_KOOPA || interactType == INTERACT_SPINY_WALKING
        || interactType == INTERACT_MR_BLIZZARD || interactType == INTERACT_BOUNCE_TOP
        || interactType == INTERACT_BOUNCE_TOP2 || interactType == INTERACT_HIT_FROM_BELOW
        || interactType == INTERACT_DAMAGE || interactType == INTERACT_SNUFIT_BULLET) {
        return TRUE;
    }
    return mario_ground_twirl_is_immune_enemy(obj);
}

static void mario_ground_twirl_destroy_projectile(struct Object *obj) {
    const BehaviorScript *behavior = obj->behavior;

    if (behavior == segmented_to_virtual(bhvSnufitBalls) && obj->oAction == 0) {
        obj->oIntangibleTimer = -1;
        obj->oAction = 1; // Its normal impact action spawns the death mist.
    } else if (behavior == segmented_to_virtual(bhvMrIParticle)
               && obj->oAction == MR_I_PIRANHA_PARTICLE_ACT_MOVE) {
        obj->oIntangibleTimer = -1;
        obj->oAction = MR_I_PIRANHA_PARTICLE_ACT_INTERACTED;
    } else if (behavior == segmented_to_virtual(bhvMrBlizzardSnowball)
               && obj->oAction == MR_BLIZZARD_SNOWBALL_ACT_COLLISION) {
        obj->oIntangibleTimer = -1;
        obj->oInteractStatus = INT_STATUS_INTERACTED | INT_STATUS_ATTACKED_MARIO;
    } else if (behavior == segmented_to_virtual(bhvMontyMoleRock)
               && obj->oAction == MONTY_MOLE_ROCK_ACT_MOVE) {
        obj->oIntangibleTimer = -1;
        obj->oInteractStatus = INT_STATUS_INTERACTED | INT_STATUS_WAS_ATTACKED;
    } else if (behavior == segmented_to_virtual(bhvFlyingBookend)
               && obj->oAction >= FLYING_BOOKEND_ACT_TURN_TOWARD_MARIO) {
        obj->oIntangibleTimer = -1;
        obj->oHealth = 0;
        obj->oAction = -1;
    } else if (behavior == segmented_to_virtual(bhvWaterBomb) && obj->oAction == WATER_BOMB_ACT_DROP) {
        // Its drop action plays the impact sound and shake before exploding.
        obj->oIntangibleTimer = -1;
        obj->oInteractStatus = INT_STATUS_INTERACTED;
    } else if (behavior == segmented_to_virtual(bhvBulletBill)
               && obj->oAction == BULLET_BILL_ACT_CHASING_MARIO) {
        // Bullet Bills reuse their object after the hit animation.
        obj->oIntangibleTimer = -1;
        obj->oInteractStatus = INT_STATUS_INTERACTED;
    }
}

static s32 mario_ground_twirl_rebound_is_playing(struct MarioState *m) {
    if (m->action == ACT_SOFT_BACKWARD_GROUND_KB || m->action == ACT_SOFT_FORWARD_GROUND_KB) {
        return m->actionState == GROUND_TWIRL_IMMUNE_REBOUND_STATE;
    }
    return m->action == ACT_BACKWARD_AIR_KB || m->action == ACT_FORWARD_AIR_KB
           || m->action == ACT_BACKWARD_GROUND_KB || m->action == ACT_FORWARD_GROUND_KB
           || m->action == ACT_HARD_BACKWARD_GROUND_KB || m->action == ACT_HARD_FORWARD_GROUND_KB;
}

s32 mario_is_knocked_back(u32 action) {
    switch (action) {
        case ACT_SOFT_BACKWARD_GROUND_KB:
        case ACT_SOFT_FORWARD_GROUND_KB:
        case ACT_BACKWARD_GROUND_KB:
        case ACT_FORWARD_GROUND_KB:
        case ACT_HARD_BACKWARD_GROUND_KB:
        case ACT_HARD_FORWARD_GROUND_KB:
        case ACT_BACKWARD_AIR_KB:
        case ACT_FORWARD_AIR_KB:
        case ACT_HARD_BACKWARD_AIR_KB:
        case ACT_HARD_FORWARD_AIR_KB:
        case ACT_BACKWARD_WATER_KB:
        case ACT_FORWARD_WATER_KB:
            return TRUE;
    }
    return FALSE;
}

static s32 mario_ground_twirl_rebound_touches_enemy(struct MarioState *m) {
    for (s32 i = 0; i < m->marioObj->numCollidedObjs; i++) {
        struct Object *obj = m->marioObj->collidedObjs[i];
        if (obj->oIntangibleTimer == 0
            && (mario_ground_twirl_is_enemy(obj) || mario_ground_twirl_is_immune_enemy(obj))) {
            return TRUE;
        }
    }
    return FALSE;
}

static void mario_rebound_from_twirl_object(struct MarioState *m, struct Object *obj, f32 normalSpeed) {
    s16 angleToObject = mario_obj_angle_to_object(m, obj);
    s16 facingDYaw = angleToObject - m->faceAngle[1];
    u32 action;

    if (mario_ground_twirl_is_large_enemy(obj)) {
        normalSpeed *= GROUND_TWIRL_LARGE_ENEMY_KNOCKBACK_FACTOR;
    }

    m->interactObj = obj;
    if (-0x4000 <= facingDYaw && facingDYaw <= 0x4000) {
        m->faceAngle[1] = angleToObject;
        mario_set_forward_vel(m, -normalSpeed);
        action = ACT_SOFT_BACKWARD_GROUND_KB;
    } else {
        m->faceAngle[1] = angleToObject + 0x8000;
        mario_set_forward_vel(m, normalSpeed);
        action = ACT_SOFT_FORWARD_GROUND_KB;
    }
    set_mario_action(m, action, 0);
    sKnockbackSourceObj = damage_came_from_enemy(obj) ? obj : NULL;
    m->actionState = GROUND_TWIRL_IMMUNE_REBOUND_STATE;
    sTwirlContact.reboundActive = TRUE;
    sTwirlContact.reboundTouchedEnemy =
        mario_ground_twirl_is_immune_enemy(obj) || mario_ground_twirl_rebound_touches_enemy(m);
    sTwirlContact.reboundHiddenTimer = 0;
}

s32 mario_ground_twirl_rebound_from_stun(struct MarioState *m, struct Object *obj) {
    if ((m->flags & (MARIO_CAP_ON_HEAD | MARIO_VANISH_CAP)) || m->invincTimer != 0
        || sTwirlContact.reboundHiddenTimer != 0) {
        return FALSE;
    }
    mario_rebound_from_twirl_object(m, obj, 16.0f);
    mario_ground_twirl_damage_capless_contact(m);
    return TRUE;
}

static void update_ground_twirl_rebound_invulnerability(struct MarioState *m) {
    if (!sTwirlContact.reboundActive) {
        return;
    }
    if (mario_ground_twirl_rebound_is_playing(m)) {
        if (mario_ground_twirl_rebound_touches_enemy(m)) {
            sTwirlContact.reboundTouchedEnemy = TRUE;
        }
    } else {
        // Every twirl rebound gets grace, including rebounds from stage hazards.
        sTwirlContact.reboundHiddenTimer = GROUND_TWIRL_REBOUND_HIDDEN_FRAMES;
        if (sTwirlContact.reboundTouchedEnemy) {
            m->invincTimer = MAX(m->invincTimer, GROUND_TWIRL_REBOUND_FLASH_FRAMES);
        }
        sTwirlContact.reboundActive = FALSE;
        sTwirlContact.reboundTouchedEnemy = FALSE;
    }
}

// Clear every touching projectile while immunity protects Mario, including same-update hits.
static void mario_clear_projectile_contacts(struct MarioState *m) {
    s32 immune = sInvulnerable || m->invincTimer != 0 || sTwirlContact.reboundHiddenTimer != 0
                 || (mario_has_current_metal_cap(m) || mario_has_current_vanish_cap(m))
                 || (mario_air_twirl_is_active(m) && (m->flags & MARIO_WING_CAP));
    if (!immune || (m->action & ACT_FLAG_INTANGIBLE)) {
        return;
    }
    for (s32 i = 0; i < m->marioObj->numCollidedObjs; i++) {
        struct Object *obj = m->marioObj->collidedObjs[i];
        if (obj->activeFlags != ACTIVE_FLAG_DEACTIVATED
            && !(obj->activeFlags & ACTIVE_FLAG_IN_DIFFERENT_ROOM) && obj->oIntangibleTimer == 0) {
            mario_ground_twirl_destroy_projectile(obj);
        }
    }
}

static struct Object *mario_get_collided_object_ignoring_knockback_source(struct MarioState *m,
                                                                          u32 interactType) {
    struct Object *object = mario_get_collided_object(m, interactType);
    s32 j;

    if (object != sKnockbackSourceObj || !mario_is_knocked_back(m->action)) {
        return object;
    }
    for (j = 0; j < m->marioObj->numCollidedObjs; j++) {
        struct Object *other = m->marioObj->collidedObjs[j];
        if (other != sKnockbackSourceObj && other->oInteractType == interactType) {
            return other;
        }
    }
    return NULL;
}

s32 mario_vanish_ignores_hostile_contact(struct MarioState *m, struct Object *obj, u32 interactType) {
    if (!(m->flags & MARIO_VANISH_CAP) || (obj->oInteractionSubtype & INT_SUBTYPE_BOWSER_ATTACK)) {
        return FALSE;
    }
    // Match ordinary damage immunity. Grabbing/eating and solid interactions
    // retain their own rules; this guard only precedes defensive twirl contacts.
    return (interactType
            & (INTERACT_DAMAGE | INTERACT_SNUFIT_BULLET | INTERACT_BULLY | INTERACT_MR_BLIZZARD
               | INTERACT_HIT_FROM_BELOW | INTERACT_BOUNCE_TOP | INTERACT_BOUNCE_TOP2 | INTERACT_KOOPA
               | INTERACT_SPINY_WALKING))
               != 0
           || (interactType == INTERACT_CLAM_OR_BUBBA
               && !(obj->oInteractionSubtype & INT_SUBTYPE_EATS_MARIO));
}

enum DefensiveContactResult {
    DEFENSIVE_CONTACT_NORMAL,
    DEFENSIVE_CONTACT_SKIP,
    DEFENSIVE_CONTACT_STOP,
};

static s32 mario_ground_twirl_outside_enemy_body(struct MarioState *m, struct Object *obj) {
    const BehaviorScript *behavior = obj->behavior;
    f32 radius, dx, dz;
    f32 bodyRadius;
    s32 hugeGoomba = (behavior == segmented_to_virtual(bhvGoomba)
#ifdef FLOOMBAS
                      || behavior == segmented_to_virtual(bhvFloomba)
#endif
                          )
                     && obj->oGoombaSize == GOOMBA_SIZE_HUGE;
    s32 piranha = behavior == segmented_to_virtual(bhvPiranhaPlant);
    // These scripted enemies do not use the mobile-enemy stun classification.
    s32 scriptedBody = behavior == segmented_to_virtual(bhvBalconyBigBoo)
                       || behavior == segmented_to_virtual(bhvMerryGoRoundBigBoo)
                       || behavior == segmented_to_virtual(bhvGhostHuntBigBoo)
                       || behavior == segmented_to_virtual(bhvWigglerHead);

    if (obj->hurtboxRadius <= 0.0f
        || !(hugeGoomba || piranha || scriptedBody
             || ((mario_ground_twirl_is_enemy(obj) || mario_ground_twirl_is_immune_enemy(obj))
                 && obj->hitboxRadius > obj->hurtboxRadius))) {
        return FALSE;
    }
    // The plant's base behavior uses a 100-unit body radius; its larger
    // sleeping and biting hitboxes must not extend the twirl rebound.
    bodyRadius = piranha ? MIN(100.0f, obj->hurtboxRadius) : MIN(obj->hitboxRadius, obj->hurtboxRadius);
    radius = m->marioObj->hitboxRadius + bodyRadius;
    dx = obj->oPosX - m->pos[0];
    dz = obj->oPosZ - m->pos[2];
    return dx * dx + dz * dz >= radius * radius;
}

static void mario_air_twirl_recoil(struct MarioState *m, struct Object *obj) {
    f32 velocityY = m->vel[1];
    f32 peakHeight = m->peakHeight;
    s32 protectedContact = (m->flags & (MARIO_WING_CAP | MARIO_METAL_CAP)) != 0;

    m->interactObj = obj;
    m->faceAngle[1] = mario_obj_angle_to_object(m, obj);
    sKnockbackSourceObj = obj;
    set_mario_action(m, ACT_BACKWARD_AIR_KB, 0);
    m->peakHeight = peakHeight;
    m->vel[1] = MIN(velocityY, -12.0f);
    mario_set_forward_vel(m, -16.0f);
    if (!protectedContact) {
        mario_ground_twirl_damage_capless_contact(m);
    }
    m->invincTimer = MAX(m->invincTimer, GROUND_TWIRL_REBOUND_FLASH_FRAMES);
    sInvulnerable = TRUE;
    sDelayInvincTimer = TRUE;
}

void mario_air_twirl_rebound_from_stun(struct MarioState *m, struct Object *obj) {
    if (!(m->flags & (MARIO_CAP_ON_HEAD | MARIO_WING_CAP | MARIO_METAL_CAP | MARIO_VANISH_CAP))
        && m->invincTimer == 0 && sTwirlContact.reboundHiddenTimer == 0) {
        mario_air_twirl_recoil(m, obj);
    }
}

static enum DefensiveContactResult
mario_handle_air_twirl_contact(struct MarioState *m, struct Object *obj, u32 interactType) {
    if (mario_ground_twirl_takes_hazard_damage(obj, interactType)) {
        return DEFENSIVE_CONTACT_NORMAL;
    }
    if (mario_ground_twirl_outside_enemy_body(m, obj)) {
        return DEFENSIVE_CONTACT_SKIP;
    }
    if (mario_twirl_can_stun_enemy(obj)) {
        // Shared targeting after movement stuns all reached enemies together.
        return DEFENSIVE_CONTACT_SKIP;
    }
    if (damage_came_from_enemy(obj) || mario_ground_twirl_is_large_enemy(obj)
        || interactType == INTERACT_BULLY) {
        if ((m->flags & MARIO_WING_CAP) && !mario_ground_twirl_is_large_enemy(obj)
            && !mario_ground_twirl_is_immune_enemy(obj)) {
            mario_ground_twirl_destroy_projectile(obj);
            return DEFENSIVE_CONTACT_SKIP;
        }
        if (m->invincTimer == 0 && sTwirlContact.reboundHiddenTimer == 0) {
            mario_air_twirl_recoil(m, obj);
            mario_ground_twirl_destroy_projectile(obj);
            return DEFENSIVE_CONTACT_STOP;
        }
        return DEFENSIVE_CONTACT_SKIP;
    }
    return DEFENSIVE_CONTACT_NORMAL;
}

static enum DefensiveContactResult mario_handle_defensive_contact(struct MarioState *m,
                                                                  struct Object *object,
                                                                  u32 interactType, s32 twirlActive) {
    s32 isEnemy;
    s32 displaced = object->oGroundTwirlStunTimer != 0 || object->oGroundTwirlContactRebound != 0;
    // Bowser hits must reach native damage and cap removal, even during a twirl.
    if (object->oInteractionSubtype & INT_SUBTYPE_BOWSER_ATTACK) {
        return DEFENSIVE_CONTACT_NORMAL;
    }
    if (mario_vanish_ignores_hostile_contact(m, object, interactType)) {
        return DEFENSIVE_CONTACT_NORMAL;
    }
    if (mario_air_twirl_is_active(m)) {
        return mario_handle_air_twirl_contact(m, object, interactType);
    }
    if (twirlActive && mario_ground_twirl_outside_enemy_body(m, object)) {
        return DEFENSIVE_CONTACT_SKIP;
    }

    if (!twirlActive && m->action != ACT_GROUND_TWIRL && !mario_has_current_metal_cap(m)
        && !displaced) {
        return DEFENSIVE_CONTACT_NORMAL;
    }
    isEnemy = mario_ground_twirl_is_enemy(object);

    // A launched enemy keeps its flight or stun throughout Mario's twirl.
    if (m->action == ACT_GROUND_TWIRL && isEnemy && displaced) {
        return DEFENSIVE_CONTACT_SKIP;
    }
    if (twirlActive && object->behavior == segmented_to_virtual(bhvChuckya)
        && !(object->oInteractStatus & INT_STATUS_INTERACTED) && object->oGroundTwirlStunTimer == 0
        && (object->oInteractionSubtype & INT_SUBTYPE_GRABS_MARIO)
        && object_facing_mario(m, object, DEGREES(60))
        && check_object_grab_mario(m, interactType, object)) {
        return DEFENSIVE_CONTACT_STOP;
    }
    if (twirlActive && m->invincTimer == 0 && sTwirlContact.reboundHiddenTimer == 0
        && mario_ground_twirl_rebounds_from_object(object, interactType)) {
        mario_rebound_from_twirl_object(m, object, MAX(16.0f, m->forwardVel));
        if (mario_ground_twirl_contact_can_damage(object)) {
            mario_ground_twirl_damage_capless_contact(m);
        }
        mario_ground_twirl_destroy_projectile(object);
        return DEFENSIVE_CONTACT_STOP;
    }
    // The defensive twirl displaces enemies without attacking them.
    if (twirlActive && isEnemy && !mario_ground_twirl_is_oversized_enemy(object)) {
        return DEFENSIVE_CONTACT_SKIP;
    }
    // Metal Mario's passive contact launches mobile enemies without a damaging attack.
    if (mario_has_current_metal_cap(m) && isEnemy && !(m->action & ACT_FLAG_ATTACKING)
        && !(determine_interaction(m, object) & INT_HIT_FROM_ABOVE)) {
        mario_metal_stun_enemy_on_contact(m, object);
        return DEFENSIVE_CONTACT_SKIP;
    }
    if (displaced && isEnemy && !stunned_enemy_can_interact(m, object, interactType)) {
        return DEFENSIVE_CONTACT_SKIP;
    }
    return DEFENSIVE_CONTACT_NORMAL;
}

void mario_process_interactions(struct MarioState *m) {
    s32 twirlActive = mario_ground_twirl_is_active(m);
    s32 puttingOnCap = m->action == ACT_PUTTING_ON_CAP || m->action == ACT_PUTTING_ON_CAP_AIR;
    s32 poseCapVulnerable = mario_pose_exposes_cap_to_damage(m);
    s32 dabCapVulnerable = mario_dab_loses_cap_on_damage(m);
    s16 coinsBeforeInteraction = m->numCoins;
    s32 burningGroundBeforeInteraction = m->action == ACT_BURNING_GROUND;
    if (!mario_is_knocked_back(m->action)) {
        sKnockbackSourceObj = NULL;
    }
    update_attack_hit_phase(m);
    update_ground_twirl_rebound_invulnerability(m);
    // Suppressed impacts must not enter a bounce through the action's common cancels.
    if ((m->input & INPUT_STOMPED)
        && (mario_blocks_harmful_knockback(m)
            || (game_options_get(GO_BOWSER_FIGHTS) == 0
                && (mario_is_breakdancing(m) || m->action == ACT_PUTTING_ON_CAP)))) {
        if (m->invincTimer == 0) {
            mario_bowser_impact_knockback(m, FALSE);
        }
        m->input &= ~INPUT_STOMPED;
        m->marioObj->oInteractStatus &=
            ~(INT_STATUS_MARIO_STUNNED | INT_STATUS_MARIO_KNOCKBACK_DMG | INT_STATUS_MARIO_SHOCKWAVE);
    }
    sDelayInvincTimer = FALSE;
    sInvulnerable =
        ((m->action & ACT_FLAG_INVULNERABLE) && (m->action != ACT_GROUND_TWIRL || twirlActive))
        || m->invincTimer != 0 || sTwirlContact.reboundHiddenTimer != 0
        || mario_breakdance_protects_from_damage(m);
    mario_start_heave_ho_recovery_immunity(m);

    // Tail grabs must not wait for a previous overlap or a free collision-list slot.
    struct Object *bowserTailContact = mario_get_bowser_tail_contact(m);
    if (bowserTailContact != NULL) {
        m->collidedObjInteractTypes |= INTERACT_GRABBABLE;
    }

    if ((!(m->action & ACT_FLAG_INTANGIBLE) || puttingOnCap) && m->collidedObjInteractTypes != 0) {
        s32 i;
        if (m->action != ACT_GROUND_TWIRL && !mario_air_twirl_is_active(m)) {
            for (i = 0; i < m->marioObj->numCollidedObjs; i++) {
                struct Object *object = m->marioObj->collidedObjs[i];
                if (puttingOnCap && !(object->oInteractionSubtype & INT_SUBTYPE_BOWSER_ATTACK)) {
                    continue;
                }
                if (object == sKnockbackSourceObj && mario_is_knocked_back(m->action)) {
                    continue;
                }
                if (mario_vanish_ignores_hostile_contact(m, object, object->oInteractType)) {
                    continue;
                }
                if (object->oGroundTwirlStunTimer != 0 && mario_ground_twirl_is_enemy(object)
                    && !stunned_enemy_can_interact(m, object, object->oInteractType)) {
                    mario_rebound_stunned_enemy(m, object);
                }
            }
        }
        for (i = 0; i < ARRAY_COUNT(sInteractionHandlers); i++) {
            u32 interactType = sInteractionHandlers[i].interactType;
            if (m->collidedObjInteractTypes & interactType) {
                struct Object *object =
                    mario_get_collided_object_ignoring_knockback_source(m, interactType);

                m->collidedObjInteractTypes &= ~interactType;

                if (interactType == INTERACT_GRABBABLE) {
                    if (bowserTailContact != NULL) {
                        object = bowserTailContact;
                    } else if (object != NULL && obj_has_behavior(object, bhvBowser)) {
                        // A previous tail overlap cannot authorize a grab after Mario leaves it.
                        object = NULL;
                    }
                }

                if (puttingOnCap) {
                    object = NULL;
                    for (s32 j = 0; j < m->marioObj->numCollidedObjs; j++) {
                        struct Object *candidate = m->marioObj->collidedObjs[j];
                        if ((candidate->oInteractType & interactType)
                            && (candidate->oInteractionSubtype & INT_SUBTYPE_BOWSER_ATTACK)
                            && !(candidate->oInteractStatus & INT_STATUS_INTERACTED)) {
                            object = candidate;
                            break;
                        }
                    }
                }
                if (object == NULL) {
                    continue;
                }

                enum DefensiveContactResult defensiveContact =
                    mario_handle_defensive_contact(m, object, interactType, twirlActive);
                if (defensiveContact == DEFENSIVE_CONTACT_STOP) {
                    break;
                }
                if (defensiveContact == DEFENSIVE_CONTACT_SKIP) {
                    continue;
                }
                if (!(object->oInteractStatus & INT_STATUS_INTERACTED)) {
                    s16 wasInvulnerable = sInvulnerable;
                    s32 handled;
                    u8 hurtBeforeInteraction = m->hurtCounter;
                    u32 actionBeforeInteraction = m->action;
                    u32 contact = determine_interaction(m, object);
                    s32 comboAttack = sPunchContacts.phase != 0
                                      && (m->action == ACT_PUNCHING || m->action == ACT_MOVE_PUNCHING)
                                      && (contact == INT_PUNCH
                                          || (m->actionArg == ACT_ARG_PUNCH_SEQUENCE_GROUND_KICK
                                              && contact == INT_KICK)
                                          || (m->actionArg == ACT_ARG_PUNCH_SEQUENCE_BREAKDANCE
                                              && contact == INT_TRIP));
                    s32 vanishImmune = mario_vanish_ignores_hostile_contact(m, object, interactType);
                    s32 caplessTwirlDamageAllowed = twirlActive && !vanishImmune && m->invincTimer == 0
                                                    && sTwirlContact.reboundHiddenTimer == 0;
                    // Every Bully variant uses INTERACT_BULLY and keeps its normal collision response.
                    if (((m->action == ACT_GROUND_TWIRL || mario_air_twirl_is_active(m)
                          || mario_is_breakdancing(m))
                         && (object->oInteractionSubtype & INT_SUBTYPE_BOWSER_ATTACK))
                        || (twirlActive
                            && (mario_ground_twirl_takes_hazard_damage(object, interactType)
                                || interactType == INTERACT_BULLY))) {
                        sInvulnerable = m->invincTimer != 0 || sTwirlContact.reboundHiddenTimer != 0;
                    }
                    handled = sInteractionHandlers[i].handler(m, interactType, object);
                    // Apply the blast after the native hit/damage response, so
                    // its stun cannot turn this contact into damage immunity.
                    if (object->oInteractStatus & INT_STATUS_WAS_ATTACKED) {
                        he_thicc_attack_contact(m, contact);
                    }
                    if (caplessTwirlDamageAllowed && m->hurtCounter == hurtBeforeInteraction
                        && m->action != actionBeforeInteraction && mario_is_knocked_back(m->action)
                        && (mario_ground_twirl_contact_can_damage(object)
                            || interactType == INTERACT_BULLY)) {
                        mario_ground_twirl_damage_capless_contact(m);
                    }
                    if (handled && m->action != actionBeforeInteraction
                        && mario_is_knocked_back(m->action)) {
                        sKnockbackSourceObj =
                            (damage_came_from_enemy(object) || interactType == INTERACT_BULLY) ? object
                                                                                               : NULL;
                    }
                    sInvulnerable = wasInvulnerable;
                    if (poseCapVulnerable && interactType != INTERACT_SHOCK
                        && (m->hurtCounter > hurtBeforeInteraction
                            || (interactType == INTERACT_FLAME
                                && (m->action == ACT_BURNING_JUMP || m->action == ACT_BURNING_FALL)))) {
                        mario_blow_off_cap(m, 15.0f);
                    }
                    if (dabCapVulnerable && interactType != INTERACT_SHOCK
                        && m->hurtCounter > hurtBeforeInteraction && damage_came_from_enemy(object)) {
                        mario_blow_off_cap(m, 15.0f);
                        dabCapVulnerable = FALSE;
                    }
                    if (comboAttack && (object->oInteractStatus & INT_STATUS_WAS_ATTACKED)
                        && !punch_hit_object(object)) {
                        remember_punch_hit_object(object);
                    }
                    if (handled) {
                        break;
                    }
                }
            }
        }
    }

    // Scan every contact even when the first parry stopped the handler loop.
    mario_clear_projectile_contacts(m);

    // Coins are handled before flames; a same-frame flame must not undo the pickup's extinguish.
    if (m->numCoins > coinsBeforeInteraction) {
        mario_extinguish_fire(m);
        if (burningGroundBeforeInteraction && m->action == ACT_WALKING
            && m->framesSinceA < MARIO_BURN_COIN_JUMP_BUFFER_FRAMES) {
            m->burnCoinJumpPending = MARIO_BURN_COIN_JUMP_BUFFER_FRAMES - m->framesSinceA;
        }
    }

    if (m->invincTimer > 0 && !sDelayInvincTimer) {
        m->invincTimer--;
    }
    // Match the cap animation's normal cancel cleanup after a Bowser interruption.
    if (puttingOnCap && m->action != ACT_PUTTING_ON_CAP && m->action != ACT_PUTTING_ON_CAP_AIR
        && (m->flags & MARIO_CAP_IN_HAND)) {
        m->flags &= ~MARIO_CAP_IN_HAND;
        m->flags |= MARIO_CAP_ON_HEAD;
    }
    mario_tick_contact_immunities(m);
    if (sTwirlContact.reboundHiddenTimer > 0) {
        sTwirlContact.reboundHiddenTimer--;
    }

    //! If the kick/punch flags are set and an object collision changes Mario's
    // action, he will get the kick/punch wall speed anyway.
    check_kick_or_punch_wall(m);
    m->flags &= ~MARIO_PUNCHING & ~MARIO_KICKING & ~MARIO_TRIPPING;

    if (!(m->marioObj->collidedObjInteractTypes & (INTERACT_WARP_DOOR | INTERACT_DOOR))) {
        sDisplayingDoorText = FALSE;
    }
    if (!(m->marioObj->collidedObjInteractTypes & INTERACT_WARP)) {
        sJustTeleported = FALSE;
    }
}

void check_death_barrier(struct MarioState *m) {
    if (m->windDeathGraceTimer != 0 && mario_is_inside_wind_column(m)) {
        return;
    }
    if (m->pos[1] < m->floorHeight + 2048.0f) {
        if (level_trigger_warp(m, WARP_OP_WARP_FLOOR) == 20 && !(m->flags & MARIO_FALL_SOUND_PLAYED)) {
            play_sound(SOUND_MARIO_WAAAOOOW, m->marioObj->header.gfx.cameraToObject);
        }
    }
}

void check_lava_boost(struct MarioState *m) {
    if (mario_blocks_harmful_knockback(m) && m->invincTimer != 0) {
        return;
    }
    if (mario_breakdance_protects_from_damage(m) || (mario_is_breakdancing(m) && m->invincTimer != 0)) {
        return;
    }
    if (!(m->action & ACT_FLAG_RIDING_SHELL) && m->pos[1] < m->floorHeight + 10.0f) {
        // Snow-course burning floors represent freezing water, not lava.
        s32 soaked = (m->area->terrainType & TERRAIN_MASK) != TERRAIN_SNOW && mario_is_soaked(m);
        if (soaked) {
            mario_consume_soaked_status(m);
        }
        if (m->flags & MARIO_METAL_CAP) {
            return;
        }
        if (!soaked) {
            player_stats_damage();
            m->hurtCounter += mario_has_cap_for_damage(m) ? 12 : 24;
        }

        update_mario_sound_and_camera(m);
        if (mario_protects_from_knockback(m)) {
            m->invincTimer = MAX(m->invincTimer, 30);
            return;
        }
        drop_and_set_mario_action(m, ACT_LAVA_BOOST, 0);
    }
}

void pss_begin_slide(UNUSED struct MarioState *m) {
    if (!(gHudDisplay.flags & HUD_DISPLAY_FLAG_TIMER)) {
        level_control_timer(TIMER_CONTROL_SHOW);
        level_control_timer(TIMER_CONTROL_START);
        sPssSlideStarted = TRUE;
    }
}

void pss_end_slide(struct MarioState *m) {
    if (sPssSlideStarted) {
        u16 slideTime = level_control_timer(TIMER_CONTROL_STOP);
        if ((gHudDisplay.flags & HUD_DISPLAY_FLAG_TIMER) && slideTime < 630) {
            m->marioObj->oBehParams = (1 << 24);
            spawn_default_star(-6358.0f, -4300.0f, 4700.0f);
        }
        sPssSlideStarted = FALSE;
    }
}

void mario_handle_special_floors(struct MarioState *m) {
    if ((m->action & ACT_GROUP_MASK) == ACT_GROUP_CUTSCENE) {
        return;
    }

    if (m->floor != NULL) {
        s32 floorType = m->floor->type;

        switch (floorType) {
            case SURFACE_DEATH_PLANE:
            case SURFACE_VERTICAL_WIND:
                check_death_barrier(m);
                break;

            case SURFACE_WARP:
                level_trigger_warp(m, WARP_OP_WARP_FLOOR);
                break;

            case SURFACE_TIMER_START:
                pss_begin_slide(m);
                break;

            case SURFACE_TIMER_END:
                pss_end_slide(m);
                break;
        }

        if (!(m->action & (ACT_FLAG_AIR | ACT_FLAG_SWIMMING))) {
            if (floorType == SURFACE_BURNING) {
                check_lava_boost(m);
            }
        }
    }
}
