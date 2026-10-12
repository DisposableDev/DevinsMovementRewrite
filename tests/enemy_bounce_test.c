/* Real contact classification, enemy-bounce handlers and twirl action. Enemy
 * membership, action initialization, animation, audio and air movement are doubles. */
#include <stdint.h>
#include "test_support.h"
typedef int32_t s32;
typedef uint32_t u32;
typedef uint8_t u8;
typedef uint16_t u16;
typedef int16_t s16;
typedef float f32;
#define TRUE 1
#define FALSE 0
#define UNUSED
#define FLOOMBAS
#include "bounce_definitions.inc"

struct Object {
    f32 oPosY, hitboxHeight;
    u32 oInteractionSubtype;
    int isEnemy;
    struct Object *platform, *parentObj;
    s32 numCollidedObjs;
    struct Object *collidedObjs[4];
    const void *behavior;
    u32 oInteractStatus, oInteractType;
    s32 oAction, oTimer, oIntangibleTimer, oBehParams2ndByte, oAnimState, oGoombaSize;
    f32 oHomeY, oGraphYOffset, oVelY, oGravity, oExclamationBoxScaleAngle;
    f32 oExclamationBoxHorizontalScale, oExclamationBoxVerticalScale;
    struct {
        struct {
            f32 cameraToObject[3], scale[3];
            s16 angle[3];
        } gfx;
    } header;
};
struct Surface {
    struct Object *object;
};
static struct Surface gWaterSurfacePseudoFloor;
struct MarioBodyState {
    s16 headAngle[3];
};
struct MarioState {
    u8 metalFallGroundPoundImpact;
    struct Object *metalFallGroundPoundFloorObject;
    u32 action, actionArg, input, flags, particleFlags, terrainSoundAddend;
    s32 actionState, actionTimer, groundPoundBounceCooldown, waterGroundPoundImpact;
    f32 pos[3], vel[3], forwardVel, floorHeight;
    s16 faceAngle[3], angleVel[3], twirlYaw;
    struct Object *marioObj;
    struct Object *interactObj;
    struct Surface *floor;
    struct Object *waterGroundPoundFloorObject;
    struct MarioBodyState *marioBodyState;
};
static struct MarioState sMario;
static struct MarioBodyState sBody;
static struct Object sMarioObject, sEnemy, *gMarioObject = &sMarioObject, *o;
#define gMarioStates (&sMario)
static s32 sDelayInvincTimer;
static struct {
    s32 burstRecovering;
    u8 waterDownRepeatFrames;
    u16 waterDownImpactCooldown;
    f32 waterDownBubbleDistance;
} sSwimming;
enum {
    SOUND_ACTION_BOUNCE_OFF_OBJECT,
    SOUND_MARIO_TWIRL_BOUNCE,
    SOUND_ACTION_TWIRL,
    SOUND_ACTION_WATER_PLUNGE,
    SOUND_ACTION_SPIN,
    SOUND_ACTION_TERRAIN_HEAVY_LANDING,
    SOUND_ACTION_METAL_HEAVY_LANDING,
    SOUND_ACTION_UNSTUCK_FROM_GROUND,
    SOUND_MARIO_PUNCH_HOO
};
#define SOUND_TERRAIN_WATER 1
#define SOUND_TERRAIN_SAND 2
#define SOUND_TERRAIN_SNOW 3
#define SOUNDARGS_MASK_PRIORITY 0xFF00
#define SOUNDARGS_SHIFT_PRIORITY 8
static int sSounds[9], sAnimation, sAnimEnd, sAirStep, sKnockback, sWaterStep;
static int sHeavyLandings, sCameraShakes, sHitRumbles, sBodyContacts;
static u32 sLastHeavySound;
#define SHAKE_GROUND_POUND 1
static s32 b_swim_has_priority(struct MarioState *m) {
    return m->input & INPUT_B_DOWN;
}
static void set_camera_shake_from_hit(s32 shake) {
    sCameraShakes++;
}
void mario_ground_pound_impact(struct MarioState *m) {
    set_camera_shake_from_hit(SHAKE_GROUND_POUND);
}
static void mario_he_thicc_contact_shake(struct MarioState *m) {
}
static void mario_he_thicc_attack_impact(struct MarioState *m) {
}
static s16 sObjectYaw;
static s16 mario_obj_angle_to_object(struct MarioState *m, struct Object *o) {
    return sObjectYaw;
}
static int mario_ground_twirl_is_enemy(struct Object *o) {
    return o->isEnemy;
}
static void play_sound(u32 sound, f32 *position) {
    u32 kind = sound & 0xFF;
    if (kind == SOUND_ACTION_TERRAIN_HEAVY_LANDING || kind == SOUND_ACTION_METAL_HEAVY_LANDING) {
        sHeavyLandings++;
        sLastHeavySound = sound;
    }
    sSounds[kind]++;
}
static void reset_mario_pitch(struct MarioState *m) {
    m->faceAngle[0] = 0;
}
static s32 set_mario_action(struct MarioState *m, u32 action, u32 arg) {
    m->action = action;
    m->actionArg = arg;
    m->actionState = m->actionTimer = 0;
    m->flags |= MARIO_JUMPING;
    if (action == ACT_JUMP) {
        m->vel[1] = 42.0f;
    }
    return TRUE;
}
static s32 drop_and_set_mario_action(struct MarioState *m, u32 action, u32 arg) {
    return set_mario_action(m, action, arg);
}
static void queue_rumble_data(s32 a, s32 b) {
    if (a == 5 && b == 80) {
        sHitRumbles++;
    }
}
static void play_sound_with_rumble(u32 sound, f32 *position, s32 a, s32 b) {
}
static void set_mario_anim_with_accel(struct MarioState *m, s32 anim, s32 accel) {
    sAnimation = anim;
}
static s32 is_anim_at_end(struct MarioState *m) {
    return sAnimEnd;
}
static void update_swimming_yaw(struct MarioState *m) {
}
static void update_swimming_pitch(struct MarioState *m) {
}
static f32 get_buoyancy(struct MarioState *m) {
    return 0.0f;
}
static s32 perform_water_step(struct MarioState *m) {
    return sWaterStep;
}
static f32 approach_f32(f32 current, f32 target, f32 increase, f32 decrease) {
    if (current < target) {
        return current + increase > target ? target : current + increase;
    }
    return current - decrease < target ? target : current - decrease;
}
static s32 take_knockback_after_punch_or_kick(struct MarioState *m, struct Object *o, u32 i) {
    return sKnockback;
}
static void bounce_back_from_attack(struct MarioState *m, u32 i) {
}
static void hit_object_from_below(struct MarioState *m, struct Object *o) {
    m->vel[1] = 0;
}
static s32 take_damage_and_knock_back(struct MarioState *m, struct Object *o) {
    sBodyContacts++;
    return FALSE;
}
static s32 approach_s32_symmetric(s32 current, s32 target, s32 step) {
    if (current < target) {
        return current + step > target ? target : current + step;
    }
    return current - step < target ? target : current - step;
}
static void set_mario_animation(struct MarioState *m, s32 anim) {
    sAnimation = anim;
}
static s32 is_anim_past_end(struct MarioState *m) {
    return sAnimEnd;
}
static void update_lava_boost_or_twirling(struct MarioState *m) {
}
static s32 perform_air_step(struct MarioState *m, u32 arg) {
    return sAirStep;
}
static void mario_bonk_reflection(struct MarioState *m, s32 arg) {
}
static void lava_boost_on_wall(struct MarioState *m) {
    set_mario_action(m, ACT_LAVA_BOOST, 0);
}
static void reset_rumble_timers_slip(void) {
}
static const int sCapBoxBehavior;
#define bhvExclamationBox (&sCapBoxBehavior)
static const void *segmented_to_virtual(const void *ptr) {
    return ptr;
}
static const int sGoombaBehavior, sFloombaBehavior;
#define bhvGoomba (&sGoombaBehavior)
#define bhvFloomba (&sFloombaBehavior)
static const int sWigglerHeadBehavior, sWigglerBodyBehavior, sPokeyPartBehavior;
#define bhvWigglerHead (&sWigglerHeadBehavior)
#define bhvWigglerBody (&sWigglerBodyBehavior)
#define bhvPokeyBodyPart (&sPokeyPartBehavior)
static s32 obj_has_behavior(struct Object *obj, const void *behavior) {
    return obj->behavior == behavior;
}
struct ObjectHitbox {
    int unused;
};
static struct ObjectHitbox sExclamationBoxHitbox;
struct ExclamationBoxContents {
    int unused;
};
static struct ExclamationBoxContents sExclamationBoxContents[3];
static int sCapSpawns[3], sBoxSounds, sBoxHidden;
#define MODEL_CARTOON_STAR 1
#define SOUND_GENERAL_BREAK_BOX 1
static void exclamation_box_spawn_contents(struct ExclamationBoxContents *list, u8 type) {
    sCapSpawns[type]++;
}
static void obj_set_hitbox(struct Object *box, struct ObjectHitbox *hitbox) {
}
static void cur_obj_unhide(void) {
    sBoxHidden = FALSE;
}
static void cur_obj_hide(void) {
    sBoxHidden = TRUE;
}
static void cur_obj_become_tangible(void) {
    o->oIntangibleTimer = 0;
}
static void cur_obj_become_intangible(void) {
    o->oIntangibleTimer = -1;
}
static void load_object_collision_model(void) {
}
static f32 sins(s16 angle) {
    return 0;
}
static f32 coss(s16 angle) {
    return 1;
}
static void cur_obj_move_using_fvel_and_gravity(void) {
    o->oPosY += o->oVelY;
    o->oVelY += o->oGravity;
}
static void obj_scale_xyz(struct Object *box, f32 x, f32 y, f32 z) {
    box->header.gfx.scale[0] = x;
    box->header.gfx.scale[1] = y;
    box->header.gfx.scale[2] = z;
}
static void spawn_mist_particles_variable(s32 a, s32 b, f32 c) {
}
static void spawn_triangle_break_particles(s32 a, s32 b, f32 c, s32 d) {
}
static void create_sound_spawner(s32 sound) {
    sBoxSounds++;
}
static void obj_mark_for_deletion(struct Object *box) {
}
#include "bounce_sources.inc"

static void begin(u32 action, u32 input, u32 subtype, f32 downwardSpeed) {
    mario_set_punch_hit_phase(0);
    memset(&sMarioObject, 0, sizeof(sMarioObject));
    memset(&sMario, 0, sizeof(sMario));
    memset(sSounds, 0, sizeof(sSounds));
    sEnemy = (struct Object){
        .oPosY = 100, .hitboxHeight = 40, .oInteractionSubtype = subtype, .isEnemy = TRUE
    };
    sMario.marioObj = &sMarioObject;
    sMario.marioBodyState = &sBody;
    sMario.action = action;
    sMario.input = input;
    sMario.vel[1] = -downwardSpeed;
    sMario.pos[1] = 150;
    sMario.faceAngle[0] = 1234;
    sMario.flags = MARIO_JUMPING;
    sAnimEnd = sAirStep = sKnockback = sDelayInvincTimer = sObjectYaw = 0;
    sWaterStep = WATER_STEP_NONE;
    memset(sCapSpawns, 0, sizeof(sCapSpawns));
    sBoxSounds = sBoxHidden = 0;
    sHeavyLandings = sCameraShakes = sSwimming.burstRecovering = 0;
    sSwimming.waterDownImpactCooldown = 0;
    sLastHeavySound = 0;
    sHitRumbles = sBodyContacts = 0;
}

static int ground_pound_twirl(void) {
    for (int speed = 20; speed <= 75; speed += 5) {
        begin(ACT_GROUND_POUND, INPUT_A_DOWN | INPUT_Z_DOWN | INPUT_Z_PRESSED, INT_SUBTYPE_TWIRL_BOUNCE,
              speed);
        EXPECT_EQ(determine_interaction(&sMario, &sEnemy), INT_GROUND_POUND_OR_TWIRL);
        EXPECT_EQ(interact_bounce_top(&sMario, 0, &sEnemy), TRUE);
        EXPECT_EQ(sMario.action, ACT_TWIRLING);
        EXPECT_EQ(sMario.vel[1], speed);
        EXPECT_EQ(sMario.pos[1], 140);
        EXPECT_EQ(sMario.faceAngle[0], 0);
        EXPECT_EQ(sMario.groundPoundBounceCooldown, 5);
        EXPECT_EQ(sMario.input, INPUT_A_DOWN);
        EXPECT_EQ(sEnemy.oInteractStatus & INT_STATUS_WAS_ATTACKED, INT_STATUS_WAS_ATTACKED);
        EXPECT_EQ(sSounds[SOUND_MARIO_TWIRL_BOUNCE], 1);
        act_twirling(&sMario); // Same-frame action execution must retain the reward.
        EXPECT_EQ(sMario.action, ACT_TWIRLING);
        EXPECT_EQ(sAnimation, MARIO_ANIM_START_TWIRL);
    }
    return 1;
}

static int ordinary_twirl(void) {
    u32 inputs[] = { 0, INPUT_A_DOWN, INPUT_A_DOWN | INPUT_A_PRESSED };
    for (int handler = 0; handler < 2; handler++) {
        for (int input = 0; input < 3; input++) {
            begin(ACT_FREEFALL, inputs[input], INT_SUBTYPE_TWIRL_BOUNCE, 50);
            u32 result = handler ? interact_hit_from_below(&sMario, 0, &sEnemy)
                                 : interact_bounce_top(&sMario, 0, &sEnemy);
            EXPECT_EQ(result, TRUE);
            EXPECT_EQ(sMario.action, ACT_TWIRLING);
            EXPECT_EQ(sMario.vel[1], inputs[input] & INPUT_A_DOWN ? 92 : 80);
            EXPECT_EQ(sMario.faceAngle[0], 0);
            EXPECT_EQ(sSounds[SOUND_MARIO_TWIRL_BOUNCE], 1);
        }
    }
    return 1;
}

static int ordinary_enemy(void) {
    begin(ACT_GROUND_POUND, INPUT_A_DOWN | INPUT_Z_DOWN, 0, 63);
    EXPECT_EQ(interact_bounce_top(&sMario, 0, &sEnemy), TRUE);
    EXPECT_EQ(sMario.action, ACT_JUMP);
    EXPECT_EQ(sMario.vel[1], 63);
    EXPECT_EQ(sMario.input, INPUT_A_DOWN);
    EXPECT_EQ(sMario.groundPoundBounceCooldown, 5);
    for (int held = 0; held < 2; held++) {
        begin(ACT_FREEFALL, held ? INPUT_A_DOWN : 0, 0, 50);
        interact_bounce_top(&sMario, 0, &sEnemy);
        EXPECT_EQ(sMario.action, ACT_FREEFALL);
        EXPECT_EQ(sMario.vel[1], held ? 42 : 30);
    }
    return 1;
}

static int spin_and_recovery(void) {
    begin(ACT_GROUND_POUND, INPUT_A_DOWN, INT_SUBTYPE_TWIRL_BOUNCE, 50);
    for (int cycle = 0; cycle < 3; cycle++) {
        // Supply the next descending ground pound at the contact boundary.
        set_mario_action(&sMario, ACT_GROUND_POUND, 0);
        sMario.vel[1] = -50;
        sMario.input = INPUT_A_DOWN | (cycle ? INPUT_A_PRESSED : 0);
        sAirStep = AIR_STEP_NONE;
        sAnimEnd = FALSE;
        EXPECT_EQ(interact_bounce_top(&sMario, 0, &sEnemy), TRUE);
        EXPECT_EQ(sMario.action, ACT_TWIRLING);
        for (int i = 0; i < 20; i++) {
            act_twirling(&sMario);
        }
        EXPECT_EQ(sMario.action, ACT_TWIRLING);
        EXPECT_EQ(sMario.angleVel[1], 0x2000);
        sAnimEnd = TRUE;
        act_twirling(&sMario);
        act_twirling(&sMario);
        EXPECT_EQ(sAnimation, MARIO_ANIM_TWIRL);
        sMario.input = 0;
        for (int i = 0; i < 5; i++) {
            act_twirling(&sMario);
        }
        EXPECT_EQ(sMario.angleVel[1], 0x1800);
        EXPECT_EQ(sMario.action, ACT_TWIRLING);
        sAirStep = AIR_STEP_HIT_WALL;
        act_twirling(&sMario);
        EXPECT_EQ(sMario.action, ACT_TWIRLING);
        sAirStep = AIR_STEP_LANDED;
        act_twirling(&sMario);
        EXPECT_EQ(sMario.action, ACT_TWIRL_LAND);
    }
    EXPECT_EQ(sSounds[SOUND_MARIO_TWIRL_BOUNCE], 3);
    return 1;
}

static int ineligible_contact(void) {
    begin(ACT_GROUND_POUND, 0, INT_SUBTYPE_TWIRL_BOUNCE, 50);
    EXPECT_EQ(interact_bounce_top(&sMario, 0, &sEnemy), FALSE);
    EXPECT_EQ(sMario.action, ACT_GROUND_POUND); // No held-A rebound requested.
    begin(ACT_GROUND_POUND, INPUT_A_DOWN, INT_SUBTYPE_TWIRL_BOUNCE, 50);
    sEnemy.isEnemy = FALSE;
    interact_bounce_top(&sMario, 0, &sEnemy);
    EXPECT_EQ(sMario.action, ACT_GROUND_POUND);
    sEnemy.isEnemy = TRUE;
    sMario.vel[1] = 50;
    EXPECT_EQ(jump_after_ground_pound_enemy_hit(&sMario, &sEnemy, INT_GROUND_POUND_OR_TWIRL), FALSE);
    sMario.vel[1] = -50;
    sKnockback = TRUE;
    interact_bounce_top(&sMario, 0, &sEnemy);
    EXPECT_EQ(sMario.action, ACT_GROUND_POUND); // Earlier recoil takes priority.
    EXPECT_EQ(sSounds[SOUND_MARIO_TWIRL_BOUNCE], 0);
    return 1;
}

static int breakdance_contacts(void) {
    u32 actions[] = { ACT_PUNCHING, ACT_MOVE_PUNCHING };
    for (unsigned i = 0; i < 2; i++) {
        begin(actions[i], 0, 0, 0);
        sMario.actionArg = ACT_ARG_PUNCH_SEQUENCE_BREAKDANCE;
        sMario.flags |= MARIO_TRIPPING;
        for (int spin = 0; spin < 8; spin++) {
            sObjectYaw = (s16) (spin * 0x4000);
            mario_set_punch_hit_phase(0);
            mario_set_punch_hit_phase(4);
            EXPECT_EQ(determine_interaction(&sMario, &sEnemy), INT_TRIP);
            remember_punch_hit_object(&sEnemy);
            for (int tick = 0; tick < 8; tick++) {
                EXPECT_EQ(determine_interaction(&sMario, &sEnemy), INT_NONE);
            }
        }
    }
    // Ordinary trips retain their facing limit, and inactive spins do not hit.
    begin(ACT_PUNCHING, 0, 0, 0);
    sObjectYaw = -0x8000;
    sMario.flags |= MARIO_TRIPPING;
    EXPECT_EQ(determine_interaction(&sMario, &sEnemy), INT_NONE);
    sMario.actionArg = ACT_ARG_PUNCH_SEQUENCE_BREAKDANCE;
    sMario.flags &= ~MARIO_TRIPPING;
    EXPECT_EQ(determine_interaction(&sMario, &sEnemy), INT_NONE);
    return 1;
}

static int underwater_ground_pound_contacts(void) {
    struct Surface impactFloor = { .object = &sEnemy };
    begin(ACT_WATER_DOWN_BURST, 0, 0, 35);
    EXPECT_EQ(determine_interaction(&sMario, &sEnemy), INT_NONE);
    sMario.actionState = ACT_STATE_WATER_DOWN_BURST_DELAY;
    EXPECT_EQ(determine_interaction(&sMario, &sEnemy), INT_NONE);
    sMario.actionState = ACT_STATE_WATER_DOWN_BURST_SINK;
    EXPECT_EQ(determine_interaction(&sMario, &sEnemy), INT_GROUND_POUND_OR_TWIRL);
    sMario.vel[1] = 0;
    EXPECT_EQ(determine_interaction(&sMario, &sEnemy), INT_NONE);

    // The floor sweep changes the action before the next collision pass.
    sMario.action = ACT_WATER_ACTION_END;
    sMario.waterGroundPoundImpact = TRUE;
    EXPECT_EQ(determine_interaction(&sMario, &sEnemy), INT_GROUND_POUND_OR_TWIRL);
    sMario.waterGroundPoundImpact = FALSE;
    EXPECT_EQ(determine_interaction(&sMario, &sEnemy), INT_NONE);
    sMario.waterGroundPoundImpact = TRUE;
    sMario.action = ACT_FORWARD_WATER_KB;
    EXPECT_EQ(determine_interaction(&sMario, &sEnemy), INT_NONE);
    sMario.action = ACT_WATER_ACTION_END;

    // Object updates can recognize the new underwater floor immediately,
    // even if platform bookkeeping has not yet selected that object.
    o = &sEnemy;
    sMario.floor = &impactFloor;
    sMario.waterGroundPoundImpact = TRUE;
    sMario.waterGroundPoundFloorObject = &sEnemy;
    EXPECT_EQ(mario_has_ground_pound_impact(), TRUE);
    EXPECT_EQ(cur_obj_is_mario_ground_pounding_platform(), TRUE);
    impactFloor.object = NULL;
    EXPECT_EQ(cur_obj_is_mario_ground_pounding_platform(), TRUE);
    sMario.floor = NULL;
    EXPECT_EQ(cur_obj_is_mario_ground_pounding_platform(), TRUE);
    sMario.waterGroundPoundFloorObject = NULL;
    EXPECT_EQ(cur_obj_is_mario_ground_pounding_platform(), FALSE);
    sMario.waterGroundPoundImpact = FALSE;
    EXPECT_EQ(mario_has_ground_pound_impact(), FALSE);
    sMario.waterGroundPoundImpact = TRUE;
    sMario.action = ACT_FORWARD_WATER_KB;
    EXPECT_EQ(mario_has_ground_pound_impact(), FALSE);
    EXPECT_EQ(cur_obj_is_mario_ground_pounding_platform(), FALSE);
    sMario.waterGroundPoundImpact = FALSE;
    sMario.action = ACT_GROUND_POUND_LAND;
    sMarioObject.platform = &sEnemy;
    EXPECT_EQ(cur_obj_is_mario_ground_pounding_platform(), TRUE);
    return 1;
}

static int underwater_ground_pound_impact(void) {
    struct Surface impactFloor = { .object = &sEnemy };
    begin(ACT_WATER_DOWN_BURST, 0, 0, 35);
    sMario.actionState = ACT_STATE_WATER_DOWN_BURST_SINK;
    finish_water_down_burst(&sMario, FALSE);
    EXPECT_EQ(sHeavyLandings, 0);
    EXPECT_EQ(sCameraShakes, 0);
    EXPECT_EQ(sMario.waterGroundPoundImpact, FALSE);
    EXPECT_EQ(sMario.particleFlags & (PARTICLE_MIST_CIRCLE | PARTICLE_HORIZONTAL_STAR), 0);
    EXPECT_EQ(sMario.particleFlags & PARTICLE_SHALLOW_WATER_SPLASH, 0);
    EXPECT_EQ(sSwimming.burstRecovering, TRUE);

    begin(ACT_WATER_DOWN_BURST, INPUT_A_DOWN, 0, 35);
    sMario.floor = &impactFloor;
    sMario.terrainSoundAddend = SOUND_TERRAIN_WATER << 16;
    sMario.actionState = ACT_STATE_WATER_DOWN_BURST_SINK;
    finish_water_down_burst(&sMario, TRUE);
    EXPECT_EQ(sHeavyLandings, 1);
    EXPECT_EQ(sLastHeavySound & ~SOUNDARGS_MASK_PRIORITY,
              SOUND_ACTION_TERRAIN_HEAVY_LANDING + (SOUND_TERRAIN_WATER << 16));
    EXPECT_EQ((sLastHeavySound & SOUNDARGS_MASK_PRIORITY) >> SOUNDARGS_SHIFT_PRIORITY, 0xD0);
    EXPECT_EQ(sCameraShakes, 1);
    EXPECT_EQ(sMario.waterGroundPoundImpact, TRUE);
    EXPECT_EQ(sMario.waterGroundPoundFloorObject == &sEnemy, TRUE);
    EXPECT_EQ(sMario.particleFlags & (PARTICLE_MIST_CIRCLE | PARTICLE_HORIZONTAL_STAR),
              PARTICLE_MIST_CIRCLE | PARTICLE_HORIZONTAL_STAR);
    EXPECT_EQ(sMario.particleFlags & PARTICLE_SHALLOW_WATER_SPLASH, 0);
    EXPECT_EQ(sMario.action, ACT_WATER_ACTION_END);
    EXPECT_EQ(sSwimming.burstRecovering, FALSE);

    sMario.pos[1] = sMario.floorHeight;
    sAnimEnd = FALSE;
    for (int frame = 0; frame < 5; frame++) {
        sMario.input = INPUT_A_DOWN | INPUT_A_PRESSED | INPUT_B_DOWN | INPUT_Z_DOWN;
        EXPECT_EQ(water_ground_pound_landing(&sMario), FALSE);
        EXPECT_EQ(sMario.action, ACT_WATER_ACTION_END);
        EXPECT_EQ(sAnimation, MARIO_ANIM_GROUND_POUND_LANDING);
        EXPECT_EQ(sMario.vel[1], 0);
        EXPECT_EQ(sHeavyLandings, 1);
        EXPECT_EQ(sCameraShakes, 1);
    }
    sAnimEnd = TRUE;
    EXPECT_EQ(water_ground_pound_landing(&sMario), FALSE);
    EXPECT_EQ(sMario.action, ACT_FLUTTER_KICK);
    EXPECT_EQ(sMario.actionArg, ACT_ARG_FLUTTER_KICK_B_SWIM);
    // Current held controls choose the exit; no second movement is dispatched.
    for (int input = 0; input < 3; input++) {
        set_mario_action(&sMario, ACT_WATER_ACTION_END, ACT_ARG_WATER_ACTION_END_GROUND_POUND);
        if (input == 1) {
            sMario.input = INPUT_A_DOWN;
        } else if (input == 2) {
            sMario.input = INPUT_B_DOWN;
        } else {
            sMario.input = 0;
        }
        EXPECT_EQ(water_ground_pound_landing(&sMario), FALSE);
        EXPECT_EQ(sMario.action, input ? ACT_FLUTTER_KICK : ACT_WATER_IDLE);
    }
    sMario.floor = NULL;
    sAnimEnd = FALSE;
    sMario.input = 0;
    set_mario_action(&sMario, ACT_WATER_ACTION_END, ACT_ARG_WATER_ACTION_END_GROUND_POUND);
    EXPECT_EQ(water_ground_pound_landing(&sMario), FALSE);
    EXPECT_EQ(sMario.action, ACT_WATER_IDLE);
    sMario.floor = &impactFloor;
    sAnimEnd = FALSE;

    EXPECT_EQ(sSwimming.waterDownImpactCooldown, WATER_DOWN_IMPACT_COOLDOWN_FRAMES);
    // Fresh presses can finish on a slope before the windup completes.
    for (s32 cooldown = WATER_DOWN_IMPACT_COOLDOWN_FRAMES; cooldown > 0; cooldown--) {
        sSwimming.waterDownImpactCooldown = cooldown;
        sMario.action = ACT_WATER_DOWN_BURST;
        sMario.actionState = ACT_STATE_WATER_DOWN_BURST_STARTUP;
        sMario.waterGroundPoundImpact = FALSE;
        sMario.waterGroundPoundFloorObject = NULL;
        sMario.particleFlags = 0;
        sWaterStep = WATER_STEP_HIT_FLOOR;
        act_water_down_burst(&sMario);
        EXPECT_EQ(sMario.action, ACT_WATER_ACTION_END);
        EXPECT_EQ(sMario.waterGroundPoundImpact, FALSE);
        EXPECT_EQ(sMario.waterGroundPoundFloorObject == NULL, TRUE);
        EXPECT_EQ(sMario.particleFlags & (PARTICLE_MIST_CIRCLE | PARTICLE_HORIZONTAL_STAR), 0);
        EXPECT_EQ(sHeavyLandings, 1);
        EXPECT_EQ(sCameraShakes, 1);
        EXPECT_EQ(sSwimming.waterDownImpactCooldown, cooldown);
    }
    sSwimming.waterDownImpactCooldown = 0;
    sMario.action = ACT_WATER_DOWN_BURST;
    act_water_down_burst(&sMario);
    EXPECT_EQ(sHeavyLandings, 2);
    EXPECT_EQ(sCameraShakes, 2);
    EXPECT_EQ(sMario.waterGroundPoundImpact, TRUE);
    EXPECT_EQ(sSwimming.waterDownImpactCooldown, WATER_DOWN_IMPACT_COOLDOWN_FRAMES);

    begin(ACT_WATER_DOWN_BURST, INPUT_B_DOWN, 0, 35);
    sMario.flags |= MARIO_METAL_CAP;
    sMario.terrainSoundAddend = SOUND_TERRAIN_WATER << 16;
    sMario.actionState = ACT_STATE_WATER_DOWN_BURST_STARTUP;
    finish_water_down_burst(&sMario, TRUE);
    EXPECT_EQ(sHeavyLandings, 1);
    EXPECT_EQ(sLastHeavySound & ~SOUNDARGS_MASK_PRIORITY, SOUND_ACTION_METAL_HEAVY_LANDING);
    EXPECT_EQ((sLastHeavySound & SOUNDARGS_MASK_PRIORITY) >> SOUNDARGS_SHIFT_PRIORITY, 0xD0);
    EXPECT_EQ(sMario.particleFlags & (PARTICLE_MIST_CIRCLE | PARTICLE_HORIZONTAL_STAR),
              PARTICLE_MIST_CIRCLE | PARTICLE_HORIZONTAL_STAR);
    EXPECT_EQ(sMario.particleFlags & PARTICLE_SHALLOW_WATER_SPLASH, 0);
    EXPECT_EQ(sMario.action, ACT_WATER_ACTION_END);
    EXPECT_EQ(sMario.actionArg, ACT_ARG_WATER_ACTION_END_GROUND_POUND);

    begin(ACT_GROUND_POUND_LAND, 0, 0, 0);
    sMario.terrainSoundAddend = SOUND_TERRAIN_WATER << 16;
    play_mario_heavy_landing_sound(&sMario, SOUND_ACTION_TERRAIN_HEAVY_LANDING);
    EXPECT_EQ(sHeavyLandings, 1);
    EXPECT_EQ(sMario.particleFlags & PARTICLE_SHALLOW_WATER_SPLASH, PARTICLE_SHALLOW_WATER_SPLASH);
    return 1;
}

static int underwater_cap_boxes(void) {
    for (int cap = 0; cap < 3; cap++) {
        for (int attack = 0; attack < 4; attack++) {
            begin(ACT_JUMP, 0, 0, 0);
            struct Surface boxSurface = { .object = &sEnemy };
            o = &sEnemy;
            sEnemy.behavior = bhvExclamationBox;
            sEnemy.isEnemy = FALSE;
            sEnemy.oBehParams2ndByte = cap;
            sEnemy.oAction = EXCLAMATION_BOX_ACT_ACTIVE;
            sEnemy.oTimer = 1;
            if (attack == 0 || attack == 2) {
                sMario.flags |= MARIO_METAL_CAP;
            }
            for (int cycle = 0; cycle < 3; cycle++) {
                if (attack == 0) {
                    sMario.action = ACT_JUMP;
                    sMario.vel[1] = 20;
                    mario_attack_cap_box_surface(&sMario, &boxSurface, INT_HIT_FROM_BELOW);
                } else if (attack == 1) {
                    sMario.action = ACT_WATER_DOWN_BURST;
                    sMario.floor = &boxSurface;
                    finish_water_down_burst(&sMario, TRUE);
                } else {
                    sMario.action = attack == 2 ? ACT_PUNCHING : ACT_WATER_PUNCH;
                    sMario.actionState = 0;
                    sMario.flags |= MARIO_PUNCHING;
                    EXPECT_EQ(interact_breakable(&sMario, 0, &sEnemy), TRUE);
                }
                EXPECT_EQ(sEnemy.oInteractStatus & INT_STATUS_WAS_ATTACKED, INT_STATUS_WAS_ATTACKED);
                exclamation_box_act_active();
                EXPECT_EQ(sEnemy.oAction, EXCLAMATION_BOX_ACT_SCALING);
                EXPECT_EQ(sEnemy.oIntangibleTimer, -1);
                sMario.waterGroundPoundImpact = FALSE;
                sMario.waterGroundPoundFloorObject = NULL;
                // Further physical contacts cannot restart a box already breaking.
                mario_attack_cap_box_surface(&sMario, &boxSurface, INT_HIT_FROM_BELOW);
                EXPECT_EQ(sEnemy.oInteractStatus, INT_STATUS_NONE);
                for (int frame = 0; frame <= 7; frame++) {
                    sEnemy.oTimer = frame;
                    exclamation_box_act_scaling();
                }
                EXPECT_EQ(sEnemy.oAction, EXCLAMATION_BOX_ACT_EXPLODE);
                exclamation_box_act_explode();
                EXPECT_EQ(sCapSpawns[cap], cycle + 1);
                EXPECT_EQ(sBoxSounds, cycle + 1);
                EXPECT_EQ(sBoxHidden, TRUE);
                EXPECT_EQ(sEnemy.oAction, EXCLAMATION_BOX_ACT_WAIT_FOR_RESPAWN);
                sEnemy.oTimer = 300;
                exclamation_box_act_wait_for_respawn();
                EXPECT_EQ(sEnemy.oAction, EXCLAMATION_BOX_ACT_WAIT_FOR_RESPAWN);
                sEnemy.oTimer = 301;
                exclamation_box_act_wait_for_respawn();
                // The supplied respawn timer skips 301 Mario updates, exceeding the cooldown.
                sSwimming.waterDownImpactCooldown = 0;
                EXPECT_EQ(sEnemy.oAction, EXCLAMATION_BOX_ACT_ACTIVE);
                sEnemy.oTimer = 0;
                exclamation_box_act_active();
                sEnemy.oTimer = 1;
                EXPECT_EQ(sEnemy.oIntangibleTimer, 0);
                EXPECT_EQ(sBoxHidden, FALSE);
                EXPECT_EQ(sCapSpawns[cap], cycle + 1);
            }
        }
    }
    // A water grab's recovery and pickup poses are not punches; facing still matters.
    begin(ACT_WATER_PUNCH, 0, 0, 0);
    sEnemy.isEnemy = FALSE;
    sEnemy.behavior = bhvExclamationBox;
    sMario.actionState = ACT_STATE_WATER_PUNCH_START;
    sObjectYaw = 0x4000;
    EXPECT_EQ(interact_breakable(&sMario, 0, &sEnemy), FALSE);
    sObjectYaw = 0;
    for (int state = ACT_STATE_WATER_PUNCH_MISSED; state <= ACT_STATE_WATER_PUNCH_PICK_UP; state++) {
        sMario.actionState = state;
        EXPECT_EQ(interact_breakable(&sMario, 0, &sEnemy), FALSE);
    }
    struct Surface surface = { .object = &sEnemy };
    sEnemy.oAction = EXCLAMATION_BOX_ACT_OUTLINE;
    mario_attack_cap_box_surface(&sMario, &surface, INT_HIT_FROM_BELOW);
    EXPECT_EQ(sEnemy.oInteractStatus, 0);
    sEnemy.oAction = EXCLAMATION_BOX_ACT_ACTIVE;
    sEnemy.oIntangibleTimer = -1;
    mario_attack_cap_box_surface(&sMario, &surface, INT_HIT_FROM_BELOW);
    EXPECT_EQ(sEnemy.oInteractStatus, 0);
    sEnemy.oIntangibleTimer = 0;
    sEnemy.behavior = NULL;
    mario_attack_cap_box_surface(&sMario, &surface, INT_HIT_FROM_BELOW);
    EXPECT_EQ(sEnemy.oInteractStatus, 0);
    return TRUE;
}

static int water_down_impulse_particles(void) {
    struct Surface floor = { NULL };
    for (s32 fromAir = FALSE; fromAir <= TRUE; fromAir++) {
        for (s32 clearance = 149; clearance <= 151; clearance++) {
            begin(ACT_WATER_DOWN_BURST, INPUT_Z_DOWN, 0, 65);
            sMario.floor = &floor;
            sMario.floorHeight = -500.0f;
            if (fromAir) {
                sMario.actionArg = ACT_ARG_WATER_DOWN_BURST_FROM_AIR;
            } else {
                sMario.actionState = ACT_STATE_WATER_DOWN_BURST_DELAY;
                sMario.actionTimer = WATER_DOWN_BURST_DELAY_FRAMES - 1;
            }
            for (s32 frame = 0; frame < 4; frame++) {
                sMario.pos[1] = sMario.floorHeight + clearance;
                sMario.particleFlags = 0;
                act_water_down_burst(&sMario);
                EXPECT_EQ((sMario.particleFlags & PARTICLE_WATER_DOWN_MIST) != 0,
                          frame == 2 && clearance >= 150);
            }
        }
        for (s32 landingFrame = 0; landingFrame <= 2; landingFrame++) {
            begin(ACT_WATER_DOWN_BURST, INPUT_Z_DOWN, 0, 65);
            sMario.floor = &floor;
            sMario.pos[1] = 500.0f;
            if (fromAir) {
                sMario.actionArg = ACT_ARG_WATER_DOWN_BURST_FROM_AIR;
            } else {
                sMario.actionState = ACT_STATE_WATER_DOWN_BURST_DELAY;
                sMario.actionTimer = WATER_DOWN_BURST_DELAY_FRAMES - 1;
            }
            for (s32 frame = 0; frame <= landingFrame; frame++) {
                sMario.particleFlags = 0;
                sWaterStep = frame == landingFrame ? WATER_STEP_HIT_FLOOR : WATER_STEP_NONE;
                act_water_down_burst(&sMario);
                EXPECT_EQ(sMario.particleFlags & PARTICLE_WATER_DOWN_MIST, 0);
            }
            EXPECT_EQ(sMario.action, ACT_WATER_ACTION_END);
            EXPECT_EQ(sMario.particleFlags & PARTICLE_MIST_CIRCLE, PARTICLE_MIST_CIRCLE);
        }
    }
    begin(ACT_WATER_DOWN_BURST, INPUT_Z_DOWN, 0, 65);
    sMario.actionState = ACT_STATE_WATER_DOWN_BURST_SINK;
    sMario.actionTimer = 1;
    sMario.pos[1] = 500.0f;
    act_water_down_burst(&sMario);
    EXPECT_EQ(sMario.particleFlags & PARTICLE_WATER_DOWN_MIST, 0);
    return TRUE;
}

static int water_down_repeat_buffer(void) {
    for (int age = 0; age <= 6; age++) {
        begin(ACT_WATER_DOWN_BURST, INPUT_Z_DOWN | INPUT_Z_PRESSED, 0, 0);
        sMario.actionState = ACT_STATE_WATER_DOWN_BURST_SINK;
        sMario.actionArg = ACT_ARG_WATER_DOWN_BURST_STARTED_STRONG;
        sMario.actionTimer = WATER_DOWN_BURST_FRAMES - 2 - age;
        act_water_down_burst(&sMario);
        sMario.input = INPUT_Z_DOWN;
        for (int frame = 0; frame < age; frame++) {
            update_water_down_repeat_buffer(&sMario);
            act_water_down_burst(&sMario);
        }
        EXPECT_EQ(sMario.actionState == ACT_STATE_WATER_DOWN_BURST_STARTUP, age < 6);
        sMario.action = ACT_WATER_IDLE;
        update_water_down_repeat_buffer(&sMario);
        EXPECT_EQ(sSwimming.waterDownRepeatFrames, 0);
    }
    return TRUE;
}

static int water_down_splash_timing(void) {
    for (int strong = FALSE; strong <= TRUE; strong++) {
        begin(ACT_WATER_DOWN_BURST, strong ? INPUT_Z_DOWN : 0, 0, 0);
        for (int cycle = 0; cycle < 3; cycle++) {
            sAnimEnd = FALSE;
            for (int frame = 0; frame < 3; frame++) {
                act_water_down_burst(&sMario);
                EXPECT_EQ(sMario.actionState, ACT_STATE_WATER_DOWN_BURST_STARTUP);
                EXPECT_EQ(sSounds[SOUND_ACTION_WATER_PLUNGE], cycle);
            }
            // Play on the final startup update, before delay update 1.
            sAnimEnd = TRUE;
            act_water_down_burst(&sMario);
            EXPECT_EQ(sMario.actionState, ACT_STATE_WATER_DOWN_BURST_DELAY);
            EXPECT_EQ(sMario.actionTimer, 0);
            EXPECT_EQ(sAnimation, MARIO_ANIM_START_GROUND_POUND);
            EXPECT_EQ(sSounds[SOUND_ACTION_WATER_PLUNGE], cycle + 1);
            EXPECT_EQ(sMario.vel[1], 0);
            for (int frame = 1; frame <= WATER_DOWN_BURST_DELAY_FRAMES; frame++) {
                act_water_down_burst(&sMario);
                EXPECT_EQ(sAnimation, MARIO_ANIM_GROUND_POUND);
                EXPECT_EQ(sSounds[SOUND_ACTION_WATER_PLUNGE], cycle + 1);
                if (frame < WATER_DOWN_BURST_DELAY_FRAMES) {
                    EXPECT_EQ(sMario.actionState, ACT_STATE_WATER_DOWN_BURST_DELAY);
                    EXPECT_EQ(sMario.actionTimer, frame);
                    EXPECT_EQ(sMario.vel[1], 0);
                } else {
                    EXPECT_EQ(sMario.actionState, ACT_STATE_WATER_DOWN_BURST_SINK);
                    EXPECT_EQ(sMario.actionTimer, 0);
                    EXPECT_EQ((int) (sMario.vel[1] * 100),
                              (int) (-WATER_DOWN_BURST_SPEED
                                     * (strong ? 1.0f : WATER_DOWN_WEAK_STRENGTH) * 100));
                }
            }
            // Fresh Z queues a strong repeat; a weak burst returns to swimming.
            for (int frame = 1; frame < WATER_DOWN_BURST_FRAMES; frame++) {
                if (strong && frame == WATER_DOWN_BURST_FRAMES - 2) {
                    sMario.input |= INPUT_Z_PRESSED;
                }
                update_water_down_repeat_buffer(&sMario);
                act_water_down_burst(&sMario);
                sMario.input &= ~INPUT_Z_PRESSED;
                EXPECT_EQ(sSounds[SOUND_ACTION_WATER_PLUNGE], cycle + 1);
            }
            if (strong) {
                EXPECT_EQ(sMario.action, ACT_WATER_DOWN_BURST);
                EXPECT_EQ(sMario.actionState, ACT_STATE_WATER_DOWN_BURST_STARTUP);
                EXPECT_EQ(sMario.actionTimer, 0);
            } else {
                EXPECT_EQ(sMario.action, ACT_WATER_ACTION_END);
                set_mario_action(&sMario, ACT_WATER_DOWN_BURST, 0);
            }
            sMario.vel[1] = 0;
        }
    }

    begin(ACT_WATER_DOWN_BURST, INPUT_Z_DOWN, 0, 50);
    sMario.actionArg = ACT_ARG_WATER_DOWN_BURST_FROM_AIR;
    act_water_down_burst(&sMario);
    EXPECT_EQ(sMario.actionState, ACT_STATE_WATER_DOWN_BURST_SINK);
    EXPECT_EQ(sSounds[SOUND_ACTION_WATER_PLUNGE], 1);
    EXPECT_EQ(sMario.particleFlags & PARTICLE_WATER_SPLASH, PARTICLE_WATER_SPLASH);
    act_water_down_burst(&sMario);
    EXPECT_EQ(sSounds[SOUND_ACTION_WATER_PLUNGE], 1);

    // Interruption before windup completion and Metal conversion skip the sound.
    begin(ACT_WATER_DOWN_BURST, INPUT_Z_DOWN, 0, 0);
    sWaterStep = WATER_STEP_HIT_FLOOR;
    act_water_down_burst(&sMario);
    EXPECT_EQ(sMario.action, ACT_WATER_ACTION_END);
    EXPECT_EQ(sSounds[SOUND_ACTION_WATER_PLUNGE], 0);
    begin(ACT_WATER_DOWN_BURST, INPUT_Z_DOWN, 0, 0);
    sAnimEnd = TRUE;
    sMario.flags |= MARIO_METAL_CAP;
    act_water_down_burst(&sMario);
    EXPECT_EQ(sMario.action, ACT_METAL_WATER_FALLING);
    EXPECT_EQ(sSounds[SOUND_ACTION_WATER_PLUNGE], 0);
    return TRUE;
}

static int huge_goomba_dive_once(void) {
    const void *behaviors[] = { bhvGoomba, bhvFloomba };
    for (int behavior = 0; behavior < 2; behavior++) {
        for (int metal = FALSE; metal <= TRUE; metal++) {
            begin(ACT_DIVE, 0, 0, 0);
            sEnemy.behavior = behaviors[behavior];
            sEnemy.oGoombaSize = GOOMBA_SIZE_HUGE;
            struct Object other = sEnemy;
            if (metal) {
                sMario.flags |= MARIO_METAL_CAP;
            }
            for (int cycle = 0; cycle < 3; cycle++) {
                sMario.action = ACT_DIVE;
                sMario.actionState = 0;
                update_attack_hit_phase(&sMario);
                sMario.actionState = DIVE_STARTUP_VULNERABLE_FRAMES;
                for (int phase = 0; phase < 2; phase++) {
                    sMario.action = phase ? ACT_DIVE_SLIDE : ACT_DIVE;
                    for (int tick = 0; tick < 8; tick++) {
                        update_attack_hit_phase(&sMario);
                        // Goomba's native handler clears the status each update.
                        sEnemy.oInteractStatus = 0;
                        interact_bounce_top(&sMario, 0, &sEnemy);
                        EXPECT_EQ(!!(sEnemy.oInteractStatus & INT_STATUS_WAS_ATTACKED),
                                  phase == 0 && tick == 0);
                        if (sEnemy.oInteractStatus & INT_STATUS_WAS_ATTACKED) {
                            o = &sEnemy;
                            huge_goomba_weakly_attacked();
                            EXPECT_EQ(sEnemy.oAction, GOOMBA_ACT_ATTACKED_MARIO);
                        }
                        EXPECT_EQ(sHitRumbles, cycle * 2 + 1);
                    }
                }
                // One target's record cannot suppress a different huge Goomba.
                other.oInteractStatus = 0;
                interact_bounce_top(&sMario, 0, &other);
                EXPECT_EQ(other.oInteractStatus & INT_STATUS_WAS_ATTACKED, INT_STATUS_WAS_ATTACKED);
                EXPECT_EQ(sHitRumbles, cycle * 2 + 2);
            }
            EXPECT_EQ(sBodyContacts, 45);
            // Unloading clears its record before this pool slot is reused.
            mario_forget_attack_object(&sEnemy);
            sEnemy.oInteractStatus = 0;
            interact_bounce_top(&sMario, 0, &sEnemy);
            EXPECT_EQ(sEnemy.oInteractStatus & INT_STATUS_WAS_ATTACKED, INT_STATUS_WAS_ATTACKED);
        }
    }
    // Ordinary Goombas and real descending stomps retain native classification.
    begin(ACT_DIVE, 0, 0, 0);
    sEnemy.behavior = bhvGoomba;
    sMario.actionState = DIVE_STARTUP_VULNERABLE_FRAMES;
    update_attack_hit_phase(&sMario);
    for (int tick = 0; tick < 3; tick++) {
        sEnemy.oInteractStatus = 0;
        interact_bounce_top(&sMario, 0, &sEnemy);
        EXPECT_EQ(sEnemy.oInteractStatus & INT_STATUS_WAS_ATTACKED, INT_STATUS_WAS_ATTACKED);
    }
    sEnemy.oGoombaSize = GOOMBA_SIZE_HUGE;
    sMario.vel[1] = -10;
    EXPECT_EQ(determine_enemy_interaction(&sMario, &sEnemy), INT_HIT_FROM_ABOVE);
    return TRUE;
}

static int dive_contact_boundaries(void) {
    begin(ACT_DIVE, 0, 0, 0);
    sEnemy.behavior = bhvWigglerHead;
    update_attack_hit_phase(&sMario);
    interact_bounce_top(&sMario, 0, &sEnemy);
    EXPECT_EQ(sEnemy.oInteractStatus, 0);
    EXPECT_EQ(punch_hit_object(&sEnemy), FALSE);
    EXPECT_EQ(sHitRumbles, 0);
    sMario.actionState = DIVE_STARTUP_VULNERABLE_FRAMES;
    interact_bounce_top(&sMario, 0, &sEnemy);
    EXPECT_EQ(sEnemy.oInteractStatus & INT_STATUS_WAS_ATTACKED, INT_STATUS_WAS_ATTACKED);
    EXPECT_EQ(sHitRumbles, 1);

    begin(ACT_DIVE_SLIDE, 0, 0, 0);
    sEnemy.behavior = bhvWigglerHead;
    sMario.actionTimer = DIVE_SLIDE_ATTACK_FRAMES;
    update_attack_hit_phase(&sMario);
    interact_bounce_top(&sMario, 0, &sEnemy);
    EXPECT_EQ(sEnemy.oInteractStatus, 0);
    EXPECT_EQ(punch_hit_object(&sEnemy), FALSE);
    EXPECT_EQ(sHitRumbles, 0);

    // Slide kicks keep their existing contact rules rather than using dive records.
    begin(ACT_SLIDE_KICK, 0, 0, 0);
    sEnemy.behavior = bhvWigglerHead;
    sMario.actionTimer = SLIDE_KICK_STARTUP_VULNERABLE_FRAMES;
    update_attack_hit_phase(&sMario);
    for (int tick = 0; tick < 2; tick++) {
        sEnemy.oInteractStatus = 0;
        interact_bounce_top(&sMario, 0, &sEnemy);
        EXPECT_EQ(sEnemy.oInteractStatus & INT_STATUS_WAS_ATTACKED, INT_STATUS_WAS_ATTACKED);
        EXPECT_EQ(punch_hit_object(&sEnemy), FALSE);
    }
    EXPECT_EQ(sHitRumbles, 2);
    return TRUE;
}

static int segmented_enemy_dive_once(void) {
    for (int pokey = FALSE; pokey <= TRUE; pokey++) {
        for (int metal = FALSE; metal <= TRUE; metal++) {
            for (int firstPart = 0; firstPart < 3; firstPart++) {
                begin(ACT_DIVE, 0, 0, 0);
                struct Object parts[3] = { sEnemy, sEnemy, sEnemy };
                parts[0].behavior = pokey ? bhvPokeyBodyPart : bhvWigglerHead;
                parts[1].behavior = parts[2].behavior = pokey ? bhvPokeyBodyPart : bhvWigglerBody;
                struct Object parent = sEnemy;
                for (int part = 0; part < 3; part++) {
                    parts[part].parentObj = pokey ? &parent : &parts[0];
                }
                struct Object *owner = pokey ? &parent : &parts[0];
                struct Object otherOwner = *owner;
                struct Object otherPart = parts[1];
                otherPart.parentObj = &otherOwner;
                sMario.flags = metal ? MARIO_METAL_CAP : 0;
                for (int cycle = 0; cycle < 3; cycle++) {
                    sMario.action = ACT_DIVE;
                    sMario.actionState = 0;
                    update_attack_hit_phase(&sMario);
                    sMario.actionState = DIVE_STARTUP_VULNERABLE_FRAMES;
                    for (int phase = 0; phase < 2; phase++) {
                        sMario.action = phase ? ACT_DIVE_SLIDE : ACT_DIVE;
                        for (int tick = 0; tick < 9; tick++) {
                            struct Object *part = &parts[(firstPart + tick) % 3];
                            part->oInteractStatus = 0;
                            update_attack_hit_phase(&sMario);
                            if (tick & 1) {
                                interact_hit_from_below(&sMario, 0, part);
                            } else {
                                interact_bounce_top(&sMario, 0, part);
                            }
                            EXPECT_EQ(!!(part->oInteractStatus & INT_STATUS_WAS_ATTACKED),
                                      phase == 0 && tick == 0);
                            EXPECT_EQ(sHitRumbles, cycle * 2 + 1);
                        }
                    }
                    otherPart.oInteractStatus = 0;
                    interact_bounce_top(&sMario, 0, &otherPart);
                    EXPECT_EQ(otherPart.oInteractStatus & INT_STATUS_WAS_ATTACKED,
                              INT_STATUS_WAS_ATTACKED);
                    EXPECT_EQ(sHitRumbles, cycle * 2 + 2);
                }
                EXPECT_EQ(sBodyContacts, 51);
                // Removing a segment cannot let the same dive hit its siblings again.
                mario_forget_attack_object(&parts[1]);
                parts[2].oInteractStatus = 0;
                interact_bounce_top(&sMario, 0, &parts[2]);
                EXPECT_EQ(parts[2].oInteractStatus, 0);
                mario_forget_attack_object(owner);
                interact_bounce_top(&sMario, 0, &parts[2]);
                EXPECT_EQ(parts[2].oInteractStatus & INT_STATUS_WAS_ATTACKED, INT_STATUS_WAS_ATTACKED);
                if (!metal) {
                    sMario.action = ACT_DIVE;
                    sMario.vel[1] = -10;
                    EXPECT_EQ(determine_enemy_interaction(&sMario, &parts[0]), INT_HIT_FROM_ABOVE);
                }
                // An interrupted dive loses its records before the next attack.
                sMario.action = ACT_FREEFALL;
                update_attack_hit_phase(&sMario);
                EXPECT_EQ(punch_hit_object(owner), FALSE);
            }
        }
    }
    return TRUE;
}

static int metal_fall_impact_contacts(void) {
    for (int cycle = 0; cycle < 3; cycle++) {
        begin(ACT_FLUTTER_KICK, 0, 0, 0);
        sMario.metalFallGroundPoundImpact = TRUE;
        sMario.actionState = 1;
        EXPECT_EQ(determine_interaction(&sMario, &sEnemy), INT_GROUND_POUND_OR_TWIRL);
        EXPECT_EQ(sMario.action, ACT_FLUTTER_KICK);
        EXPECT_EQ(sMario.vel[1], 0);
        sMario.metalFallGroundPoundImpact = FALSE;
        EXPECT_EQ(determine_interaction(&sMario, &sEnemy), INT_NONE);
    }
    return TRUE;
}

int main(int argc, char **argv) {
    test_init(argc, argv);
    struct {
        const char *name;
        int (*run)(void);
    } cases[] = {
        { "bounce.ground_pound_twirl", ground_pound_twirl },
        { "bounce.metal_fall_impact_contacts", metal_fall_impact_contacts },
        { "bounce.ordinary_twirl", ordinary_twirl },
        { "bounce.ordinary_enemy", ordinary_enemy },
        { "bounce.spin_and_recovery", spin_and_recovery },
        { "bounce.ineligible_contact", ineligible_contact },
        { "bounce.breakdance_contacts", breakdance_contacts },
        { "bounce.underwater_ground_pound_contacts", underwater_ground_pound_contacts },
        { "bounce.underwater_ground_pound_impact", underwater_ground_pound_impact },
        { "bounce.underwater_cap_boxes", underwater_cap_boxes },
        { "bounce.water_down_splash_timing", water_down_splash_timing },
        { "bounce.water_down_repeat_buffer", water_down_repeat_buffer },
        { "bounce.water_down_impulse_particles", water_down_impulse_particles },
        { "bounce.huge_goomba_dive_once", huge_goomba_dive_once },
        { "bounce.segmented_enemy_dive_once", segmented_enemy_dive_once },
        { "bounce.dive_contact_boundaries", dive_contact_boundaries },
    };
    for (unsigned i = 0; i < sizeof(cases) / sizeof(cases[0]); i++) {
        if (test_selected(cases[i].name)) {
            test_report(cases[i].name, cases[i].run());
        }
    }
    return test_finish();
}
