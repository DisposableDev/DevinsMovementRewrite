/* Exercise the production pickup, cap-loss handoff, and shell object lifecycle. */
#include <stdint.h>
#include <math.h>
#include "test_support.h"

typedef int32_t s32;
typedef int16_t s16;
typedef uint32_t u32;
typedef uint8_t u8;
typedef float f32;
typedef int BehaviorScript;
#define UNUSED
#define TRUE 1
#define FALSE 0
#define SEGMENT_BEHAVIOR_DATA 0
#define INTERACT_GRABBABLE 1
#define INT_SUBTYPE_KICKABLE 1
#define INT_SUBTYPE_GRABS_MARIO 2
#define INT_SUBTYPE_NOT_GRABBABLE 4
#define INT_KICK 1
#define INT_TRIP 2
#define INT_STATUS_NONE 0
#define INT_STATUS_INTERACTED 0x8000
#define INT_STATUS_STOP_RIDING 0x400000
#define INPUT_INTERACT_OBJ_GRABBABLE 0x800
#define INPUT_B_PRESSED 0x2000
#define INPUT_A_DOWN 0x80
#define INPUT_A_PRESSED 2
#define INT_STATUS_MARIO_DROP_OBJECT 8
#define MARIO_ANIM_FLUTTERKICK_WITH_OBJ 0xA1
#define MARIO_ANIM_START_RIDING_SHELL 0
#define MARIO_ANIM_RIDING_SHELL 1
#define SOUND_MOVING_RIDING_SHELL_LAVA 0
#define SOUND_MOVING_TERRAIN_RIDING_SHELL 1
#define INPUT_Z_PRESSED 0x8000
#define PARTICLE_VERTICAL_STAR 1
#define SURFACE_BURNING 1
#define ACT_GROUP_MASK 0x1C0
#define ACT_GROUP_STATIONARY 0
#define ACT_GROUP_MOVING 0x40
#define ACT_GROUP_AIRBORNE 0x80
#define ACT_GROUP_SUBMERGED 0xC0
#define ACT_FLAG_RIDING_SHELL 0x10000
#define ACT_WATER_IDLE 0x380022C0
#define ACT_THROWING 0x80000588
#define ACT_HEAVY_THROW 0x80000589
#define ACT_RIDING_SHELL_GROUND 0x20810446
#define ACT_RIDING_SHELL_FALL 0x0081089B
#define ACT_WATER_SHELL_SWIMMING 0x300024D6
#define GRAB_POS_LIGHT_OBJ 1
#define GRAPH_RENDER_ACTIVE 1
#define GRAPH_RENDER_INVISIBLE 16
#define ACT_WALKING 0x04000440
#define ACT_HOLD_WALKING 0x00000442
#define INT_HIT_FROM_ABOVE 0x100
#define KOOPA_SHELL_ACT_MARIO_NOT_RIDING 0
#define KOOPA_SHELL_ACT_MARIO_RIDING 1
#define FLOOR_LOWER_LIMIT -11000
#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define sqr(x) ((x) * (x))
#define HELD_FREE 0
#define HELD_HELD 1
#define HELD_THROWN 2
#define HELD_DROPPED 3

struct ObjectHitbox {
    s32 interactType, downOffset, damageOrCoinValue, health, numLootCoins;
    s32 radius, height, hurtboxRadius, hurtboxHeight;
};
struct Object {
    struct {
        struct {
            struct {
                u32 flags;
            } node;
            s32 angle[3];
            f32 cameraToObject[3];
        } gfx;
    } header;
    const BehaviorScript *behavior;
    struct Object *parentObj;
    u32 activeFlags;
    u32 oInteractionSubtype, oInteractStatus;
    s32 oGroundTwirlStunTimer, oHeldState, oAction, oFaceAngleYaw, oMoveAngleYaw, oIntangibleTimer,
        oTimer;
    f32 oPosX, oPosY, oPosZ;
};
struct MarioBodyState {
    s32 grabPos;
};
struct Surface {
    s32 type;
};
struct Camera {
    s32 mode, defMode;
};
struct Area {
    struct Camera *camera;
};
struct Controller {
    f32 stickY;
};
struct MarioState {
    struct Controller *controller;
    u32 action, flags, input, actionArg, particleFlags;
    s32 terrainSoundAddend;
    s32 waterLevel, health, actionTimer;
    f32 pos[3], vel[3], floorHeight, forwardVel;
    s32 faceAngle[3], angleVel[3];
    struct Object *heldObj, *riddenObj, *usedObj, *interactObj, *marioObj;
    struct MarioBodyState *marioBodyState;
    struct Area *area;
    struct Surface *floor;
};

static const BehaviorScript sUnderwaterBehavior, sBowserBehavior, sOtherBehavior, sGroundShellBehavior;
#define bhvKoopaShell (&sGroundShellBehavior)
#define bhvExclamationBox (&sOtherBehavior)
#define bhvKoopaShellUnderwater (&sUnderwaterBehavior)
#define bhvBowser (&sBowserBehavior)
#define bhvCarrySomethingHeld (&sOtherBehavior)
#define virtual_to_segmented(segment, ptr) (ptr)
#define segmented_to_virtual(ptr) (ptr)

static struct MarioState sMario;
static struct Controller sController;
static struct MarioBodyState sBody;
static struct Camera sCamera;
static struct Area sArea = { &sCamera };
static struct Surface sFloor;
static f32 sPhysicalFloorHeight;
static struct Object sShell, sMarioObject;
static struct Object *o, *gMarioObject = &sMarioObject;
static s32 sMusicStarts, sDeleted, sIntangible, sUnrendered;
#include "shell_definitions.inc"
static s32 sBlinkCalls;
static void obj_set_blink_phase(struct Object *obj, s32 dim) {
    obj->header.gfx.node.flags &= ~GRAPH_RENDER_INVISIBLE;
    obj->activeFlags &= ~ACTIVE_FLAG_BLINKING;
}
static s32 obj_flicker_and_disappear(struct Object *obj, s32 life) {
    sBlinkCalls++;
    return FALSE;
}
static void mario_drop_held_object(struct MarioState *m) {
    m->heldObj = NULL;
}

static s32 set_mario_action(struct MarioState *m, u32 action, u32 arg) {
    m->action = action;
    return TRUE;
}
static void update_mario_sound_and_camera(struct MarioState *m) {
}
static void play_shell_music(void) {
    sMusicStarts++;
}
static u32 determine_interaction(struct MarioState *m, struct Object *obj) {
    return 0;
}
static void attack_object(struct Object *obj, u32 interaction) {
}
static void bounce_back_from_attack(struct MarioState *m, u32 interaction) {
}
static s32 check_object_grab_mario(struct MarioState *m, u32 type, struct Object *obj) {
    return FALSE;
}
static s32 able_to_grab_object(struct MarioState *m, struct Object *obj) {
    return TRUE;
}
static void push_mario_out_of_object(struct MarioState *m, struct Object *obj, f32 amount) {
}
static void obj_set_held_state(struct Object *obj, const BehaviorScript *behavior) {
    obj->oHeldState = HELD_HELD;
}
static void obj_set_hitbox(struct Object *obj, struct ObjectHitbox *hitbox) {
}
static void obj_copy_pos(struct Object *dst, struct Object *src) {
    dst->oPosX = src->oPosX;
    dst->oPosY = src->oPosY;
    dst->oPosZ = src->oPosZ;
}
static void cur_obj_become_intangible(void) {
    sIntangible++;
}
static void cur_obj_unrender_set_action_and_anim(s32 action, s32 anim) {
    sUnrendered++;
    o->oAction = action;
}
static void spawn_mist_particles(void) {
}
static void obj_mark_for_deletion(struct Object *obj) {
    sDeleted++;
}
static void set_camera_mode(struct Camera *camera, s32 mode, s32 frames) {
    camera->mode = mode;
}
static void vec3_zero(s32 *v) {
    v[0] = v[1] = v[2] = 0;
}
static void mario_set_forward_vel(struct MarioState *m, f32 vel) {
    m->forwardVel = vel;
}
static f32 find_floor(f32 x, f32 y, f32 z, struct Surface **floor) {
    *floor = &sFloor;
    return sPhysicalFloorHeight;
}

static s32 sMusicStops;
static f32 sTravelPerFrame;
static void __attribute__((unused)) stop_shell_music(void) {
    sMusicStops++;
}
static s32 drop_and_set_mario_action(struct MarioState *m, u32 action, u32 arg) {
    mario_drop_held_object(m);
    return set_mario_action(m, action, arg);
}
static f32 approach_f32(f32 value, f32 target, f32 up, f32 down) {
    return value < target   ? MIN(value + up, target)
           : value > target ? MAX(value - down, target)
                            : value;
}
static void play_swimming_noise(struct MarioState *m) {
}
static void set_mario_animation(struct MarioState *m, s32 animation) {
}
static void common_swimming_step(struct MarioState *m, s32 strength, f32 drag) {
    m->pos[0] += sTravelPerFrame;
    sMarioObject.oPosX = m->pos[0];
    sMarioObject.oPosY = m->pos[1];
    sMarioObject.oPosZ = m->pos[2];
}

static s32 sGroundStep = GROUND_STEP_NONE;
static void update_shell_speed(struct MarioState *m) {
}
static s32 perform_ground_step(struct MarioState *m) {
    return sGroundStep;
}
static void mario_play_bonk_sound(struct MarioState *m) {
}
static void mario_set_wall_bonk_momentum(struct MarioState *m, f32 speed, f32 y) {
    m->forwardVel = speed;
}
static void tilt_body_ground_shell(struct MarioState *m, s16 yaw) {
}
static void play_sound(s32 sound, void *position) {
}
static void adjust_sound_for_speed(struct MarioState *m) {
}
static void reset_rumble_timers_slip(void) {
}

#include "shell_sources.inc"

static void reset_shell(void) {
    memset(&sMario, 0, sizeof(sMario));
    memset(&sBody, 0, sizeof(sBody));
    memset(&sShell, 0, sizeof(sShell));
    memset(&sMarioObject, 0, sizeof(sMarioObject));
    sMario.marioObj = &sMarioObject;
    sMario.marioBodyState = &sBody;
    sMario.area = &sArea;
    sMario.controller = &sController;
    sMario.floor = &sFloor;
    sController.stickY = 0;
    sMario.action = ACT_WATER_IDLE;
    sMario.health = 0x880;
    sMario.waterLevel = 1000;
    sShell.behavior = bhvKoopaShellUnderwater;
    o = &sShell;
    sPhysicalFloorHeight = 0;
    sMusicStarts = sDeleted = sIntangible = sUnrendered = 0;
}

static int pickup_by_cap(void) {
    for (int cap = 0; cap < 3; cap++) {
        reset_shell();
        sMario.flags = cap == 0   ? MARIO_VANISH_CAP
                       : cap == 1 ? MARIO_METAL_CAP
                                  : MARIO_VANISH_CAP | MARIO_METAL_CAP;
        EXPECT_EQ(interact_grabbable(&sMario, INTERACT_GRABBABLE, &sShell), TRUE);
        EXPECT_EQ(sMario.action, ACT_RIDING_SHELL_GROUND);
        EXPECT_EQ(sMario.riddenObj == &sShell, TRUE);
        EXPECT_EQ(sMario.heldObj == NULL, TRUE);
        EXPECT_EQ(sShell.oInteractStatus, INT_STATUS_INTERACTED);
        EXPECT_EQ(sMusicStarts, 1);
        bhv_koopa_shell_underwater_loop();
        EXPECT_EQ(sShell.oAction, 1);
        EXPECT_EQ(sIntangible, 1);
    }
    return 1;
}

static int ordinary_swimmer_grabs(void) {
    reset_shell();
    EXPECT_EQ(interact_grabbable(&sMario, INTERACT_GRABBABLE, &sShell), TRUE);
    EXPECT_EQ(sMario.action, ACT_WATER_IDLE);
    EXPECT_EQ(sMario.riddenObj == NULL, TRUE);
    EXPECT_EQ(sMario.input & INPUT_INTERACT_OBJ_GRABBABLE, INPUT_INTERACT_OBJ_GRABBABLE);
    EXPECT_EQ(sMusicStarts, 0);
    return 1;
}

static int released_shell_cannot_remount(void) {
    reset_shell();
    sMario.flags = MARIO_METAL_CAP;
    sShell.oHeldState = HELD_THROWN;
    EXPECT_EQ(interact_grabbable(&sMario, INTERACT_GRABBABLE, &sShell), TRUE);
    EXPECT_EQ(sMario.riddenObj == NULL, TRUE);
    sShell.oHeldState = HELD_FREE;
    sShell.oIntangibleTimer = -1;
    EXPECT_EQ(interact_grabbable(&sMario, INTERACT_GRABBABLE, &sShell), TRUE);
    EXPECT_EQ(sMario.riddenObj == NULL, TRUE);
    sShell.oIntangibleTimer = 0;
    sShell.oInteractStatus = INT_STATUS_STOP_RIDING;
    EXPECT_EQ(interact_grabbable(&sMario, INTERACT_GRABBABLE, &sShell), TRUE);
    EXPECT_EQ(sMario.riddenObj == NULL, TRUE);
    return 1;
}

static int cap_presets_handoff(void) {
    for (int metalOriginal = 0; metalOriginal < 2; metalOriginal++) {
        for (int vanishOriginal = 0; vanishOriginal < 2; vanishOriginal++) {
            for (int caps = 0; caps < 8; caps++) {
                sTestGameOptions[GO_METAL_CAP] = metalOriginal;
                sTestGameOptions[GO_VANISH_CAP] = vanishOriginal;
                reset_shell();
                u32 powers = (caps & 1 ? MARIO_WING_CAP : 0) | (caps & 2 ? MARIO_METAL_CAP : 0)
                             | (caps & 4 ? MARIO_VANISH_CAP : 0);
                sMario.flags = powers;
                sMario.action = ACT_WATER_SHELL_SWIMMING;
                sMario.heldObj = &sShell;
                sShell.oHeldState = HELD_HELD;
                play_shell_music();
                int landControls = ((caps & 2) && !metalOriginal) || ((caps & 4) && !vanishOriginal);
                EXPECT_EQ(mario_transition_underwater_shell_to_riding(&sMario), landControls);
                EXPECT_EQ(sMario.riddenObj == &sShell, !!landControls);
                EXPECT_EQ(sMario.heldObj == &sShell, !landControls);
                if (landControls) {
                    for (int cycle = 0; cycle < 3; cycle++) {
                        sTestGameOptions[GO_METAL_CAP] = sTestGameOptions[GO_VANISH_CAP] = 1;
                        EXPECT_EQ(mario_transition_underwater_shell_to_swimming(&sMario), TRUE);
                        EXPECT_EQ(sMario.action, ACT_WATER_SHELL_SWIMMING);
                        EXPECT_EQ(sMario.heldObj == &sShell, TRUE);
                        EXPECT_EQ(sMario.riddenObj == NULL, TRUE);
                        EXPECT_EQ(sShell.oHeldState, HELD_HELD);
                        sTestGameOptions[GO_METAL_CAP] = metalOriginal;
                        sTestGameOptions[GO_VANISH_CAP] = vanishOriginal;
                        EXPECT_EQ(mario_transition_underwater_shell_to_riding(&sMario), TRUE);
                        EXPECT_EQ(sMario.riddenObj == &sShell, TRUE);
                        EXPECT_EQ(sMario.heldObj == NULL, TRUE);
                        EXPECT_EQ(sShell.oHeldState, HELD_FREE);
                    }
                }
                EXPECT_EQ(sMario.flags, powers);
                EXPECT_EQ(sMusicStarts, 1);
                EXPECT_EQ(sDeleted, 0);
            }
        }
    }
    sTestGameOptions[GO_METAL_CAP] = sTestGameOptions[GO_VANISH_CAP] = 0;
    return TRUE;
}

static int cap_expiry_handoff(void) {
    reset_shell();
    sMario.flags = MARIO_METAL_CAP;
    interact_grabbable(&sMario, INTERACT_GRABBABLE, &sShell);
    bhv_koopa_shell_underwater_loop();
    EXPECT_EQ(mario_transition_underwater_shell_to_swimming(&sMario), FALSE);
    sMario.flags = 0; /* The cap timer clears this flag after action dispatch. */
    EXPECT_EQ(mario_transition_underwater_shell_to_swimming(&sMario), TRUE);
    EXPECT_EQ(sMario.action, ACT_WATER_SHELL_SWIMMING);
    EXPECT_EQ(sMario.riddenObj == NULL, TRUE);
    EXPECT_EQ(sMario.heldObj == &sShell, TRUE);
    EXPECT_EQ(sShell.oHeldState, HELD_HELD);
    EXPECT_EQ(sBody.grabPos, GRAB_POS_LIGHT_OBJ);
    EXPECT_EQ(sMusicStarts, 1);
    bhv_koopa_shell_underwater_loop();
    EXPECT_EQ(sUnrendered, 1);
    EXPECT_EQ(sDeleted, 0);
    return 1;
}

static int cap_pickup_while_holding(void) {
    for (int cap = 0; cap < 2; cap++) {
        reset_shell();
        sMario.action = ACT_WATER_SHELL_SWIMMING;
        sMario.heldObj = &sShell;
        sShell.oHeldState = HELD_HELD;
        sMario.flags = cap == 0 ? MARIO_VANISH_CAP : MARIO_METAL_CAP;
        sMario.pos[1] = cap == 0 ? 0 : 300;
        sMario.floorHeight = 0;
        EXPECT_EQ(mario_transition_underwater_shell_to_riding(&sMario), TRUE);
        EXPECT_EQ(sMario.action, cap == 0 ? ACT_RIDING_SHELL_GROUND : ACT_RIDING_SHELL_FALL);
        EXPECT_EQ(sMario.heldObj == NULL, TRUE);
        EXPECT_EQ(sMario.riddenObj == &sShell, TRUE);
        EXPECT_EQ(sShell.oHeldState, HELD_FREE);
        EXPECT_EQ(sShell.header.gfx.node.flags & GRAPH_RENDER_ACTIVE, GRAPH_RENDER_ACTIVE);
        sMario.flags = 0;
        EXPECT_EQ(mario_transition_underwater_shell_to_swimming(&sMario), TRUE);
        EXPECT_EQ(sMario.heldObj == &sShell, TRUE);
    }
    return 1;
}

static int surface_cap_expiry_swims(void) {
    reset_shell();
    sMario.flags = MARIO_VANISH_CAP;
    interact_grabbable(&sMario, INTERACT_GRABBABLE, &sShell);
    sMario.flags = 0;
    sMario.pos[1] = 1000;
    EXPECT_EQ(mario_transition_underwater_shell_to_swimming(&sMario), TRUE);
    EXPECT_EQ(sMario.heldObj == &sShell, TRUE);
    EXPECT_EQ(sMario.pos[1], 920);
    return 1;
}

static int dry_or_shallow_water_keeps_riding(void) {
    reset_shell();
    sMario.flags = MARIO_METAL_CAP;
    interact_grabbable(&sMario, INTERACT_GRABBABLE, &sShell);
    sMario.flags = 0;
    sMario.waterLevel = FLOOR_LOWER_LIMIT;
    EXPECT_EQ(mario_transition_underwater_shell_to_swimming(&sMario), FALSE);
    EXPECT_EQ(sMario.riddenObj == &sShell, TRUE);
    sMario.waterLevel = 1000;
    sPhysicalFloorHeight = 950;
    EXPECT_EQ(mario_transition_underwater_shell_to_swimming(&sMario), FALSE);
    EXPECT_EQ(sMario.riddenObj == &sShell, TRUE);
    return 1;
}

static int airborne_above_water_waits_to_swim(void) {
    reset_shell();
    sMario.flags = MARIO_METAL_CAP;
    interact_grabbable(&sMario, INTERACT_GRABBABLE, &sShell);
    sMario.flags = 0;
    sMario.pos[1] = 1200;
    EXPECT_EQ(mario_transition_underwater_shell_to_swimming(&sMario), FALSE);
    EXPECT_EQ(sMario.pos[1], 1200);
    EXPECT_EQ(sMario.riddenObj == &sShell, TRUE);
    return 1;
}

static int riding_shell_release(void) {
    reset_shell();
    sShell.oAction = 1;
    sShell.oInteractStatus = INT_STATUS_STOP_RIDING;
    bhv_koopa_shell_underwater_loop();
    EXPECT_EQ(sDeleted, 1);
    return 1;
}

static int pickup_visibility(void) {
    for (int hidden = 0; hidden < 2; hidden++) {
        reset_shell();
        sShell.header.gfx.node.flags = hidden ? GRAPH_RENDER_INVISIBLE : GRAPH_RENDER_ACTIVE;
        sShell.activeFlags = hidden ? ACTIVE_FLAG_BLINKING : 0;
        sMario.action = ACT_WALKING;
        EXPECT_EQ(interact_koopa_shell(&sMario, 0, &sShell), TRUE);
        EXPECT_EQ(sMario.riddenObj == &sShell, TRUE);
        EXPECT_EQ(sShell.header.gfx.node.flags, GRAPH_RENDER_ACTIVE);
        EXPECT_EQ(sShell.activeFlags & ACTIVE_FLAG_BLINKING, 0);
        sShell.parentObj = &sMarioObject;
        sMarioObject.behavior = bhvExclamationBox;
        sShell.oAction = KOOPA_SHELL_ACT_MARIO_RIDING;
        sShell.oTimer = 339;
        sBlinkCalls = 0;
        check_shell_despawn();
        EXPECT_EQ(sBlinkCalls, 0);
        sShell.oAction = KOOPA_SHELL_ACT_MARIO_NOT_RIDING;
        check_shell_despawn();
        EXPECT_EQ(sBlinkCalls, 1);
    }
    return TRUE;
}

static int held_shell_follows(void) {
    reset_shell();
    sMario.heldObj = &sShell;
    sShell.oHeldState = HELD_HELD;
    for (int frame = 0; frame < 100; frame++) {
        sMarioObject.oPosX = frame * 100;
        sMarioObject.oPosY = -4000;
        sMarioObject.oPosZ = -frame * 50;
        bhv_koopa_shell_underwater_loop();
        EXPECT_EQ(sShell.oPosX, sMarioObject.oPosX);
        EXPECT_EQ(sShell.oPosY, sMarioObject.oPosY);
        EXPECT_EQ(sShell.oPosZ, sMarioObject.oPosZ);
        EXPECT_EQ(sDeleted, 0);
    }
    return TRUE;
}

static int shell_protection(void) {
    for (s32 master = 0; master <= 1; master++) {
        for (s32 keep = 0; keep <= 1; keep++) {
            sTestGameOptions[GO_CHEATS_ENABLED] = master;
            sTestGameOptions[GO_KEEP_SHELL] = keep;
            reset_shell();
            sMario.action = ACT_RIDING_SHELL_GROUND;
            sMario.riddenObj = &sShell;
            sShell.behavior = bhvKoopaShell;
            sMario.forwardVel = 50;
            sGroundStep = GROUND_STEP_HIT_WALL;
            sMusicStops = 0;
            act_riding_shell_ground(&sMario);
            EXPECT_EQ(sMario.riddenObj == &sShell, master && keep);
            EXPECT_EQ(sMario.action, master && keep ? ACT_RIDING_SHELL_GROUND : ACT_BACKWARD_GROUND_KB);
            EXPECT_EQ(sMusicStops, !(master && keep));
            if (master && keep) {
                EXPECT_EQ(sMario.forwardVel, 0);
            }

            reset_shell();
            sMario.action = ACT_WATER_SHELL_SWIMMING;
            sMario.heldObj = &sShell;
            sMarioObject.oInteractStatus = INT_STATUS_MARIO_DROP_OBJECT;
            act_water_shell_swimming(&sMario);
            EXPECT_EQ(sMario.heldObj == &sShell, master && keep);
            EXPECT_EQ(sMario.action, master && keep ? ACT_WATER_SHELL_SWIMMING : ACT_WATER_IDLE);
        }
    }
    sTestGameOptions[GO_CHEATS_ENABLED] = sTestGameOptions[GO_KEEP_SHELL] = TRUE;
    reset_shell();
    sMario.action = ACT_RIDING_SHELL_GROUND;
    sMario.riddenObj = &sShell;
    sMario.input = INPUT_Z_PRESSED;
    sMusicStops = 0;
    act_riding_shell_ground(&sMario);
    EXPECT_EQ(sMario.riddenObj == NULL, TRUE);
    EXPECT_EQ(sMario.action, ACT_CROUCH_SLIDE);
    EXPECT_EQ(sMusicStops, 1);

    reset_shell();
    sMario.action = ACT_WATER_SHELL_SWIMMING;
    sMario.heldObj = &sShell;
    sMario.input = INPUT_B_PRESSED;
    act_water_shell_swimming(&sMario);
    EXPECT_EQ(sMario.action, ACT_WATER_THROW);
    mario_stop_underwater_shell(&sMario); // Throw owns release; this no longer matches.
    EXPECT_EQ(sMario.heldObj == &sShell, TRUE);
    sMario.action = ACT_WATER_SHELL_SWIMMING;
    sMusicStops = 0;
    mario_stop_underwater_shell(&sMario); // Explicit water-exit cleanup still releases.
    EXPECT_EQ(sMario.heldObj == NULL, TRUE);
    EXPECT_EQ(sMusicStops, 1);

    reset_shell();
    sMario.action = ACT_RIDING_SHELL_GROUND;
    sMario.riddenObj = &sShell;
    sShell.behavior = bhvBowser;
    EXPECT_EQ(mario_keeps_shell(&sMario), FALSE);
    sMario.riddenObj = NULL;
    EXPECT_EQ(mario_keeps_shell(&sMario), FALSE);
    sTestGameOptions[GO_CHEATS_ENABLED] = sTestGameOptions[GO_KEEP_SHELL] = FALSE;
    return TRUE;
}

static int underwater_shell_lifetime(void) {
    for (int travel = 0; travel < 3; travel++) {
        reset_shell();
        sMario.heldObj = &sShell;
        sShell.oHeldState = HELD_HELD;
        sMario.action = ACT_WATER_SHELL_SWIMMING;
        sTravelPerFrame = travel == 0 ? 0 : 30;
        if (travel == 2) {
            sMario.pos[0] = 12000; // Cross the spawn-distance threshold immediately.
        }
        sMusicStops = 0;
        for (int frame = 0; frame < 400; frame++) {
            act_water_shell_swimming(&sMario);
            bhv_koopa_shell_underwater_loop();
            if (sMario.heldObj == NULL) {
                fprintf(stderr, "shell released: frame=%d distance=%.0f musicStops=%d deleted=%d\n",
                        frame + 1, sMario.pos[0], sMusicStops, sDeleted);
            }
            EXPECT_EQ(sMario.heldObj == &sShell, TRUE);
            EXPECT_EQ(sMario.action, ACT_WATER_SHELL_SWIMMING);
            EXPECT_EQ(sShell.oPosX, sMario.pos[0]);
            EXPECT_EQ(sDeleted, 0);
            EXPECT_EQ(sMusicStops, 0);
        }
    }
    reset_shell();
    sMario.heldObj = &sShell;
    sShell.oHeldState = HELD_HELD;
    sMario.action = ACT_WATER_SHELL_SWIMMING;
    sMario.input = INPUT_B_PRESSED;
    EXPECT_EQ(act_water_shell_swimming(&sMario), TRUE);
    EXPECT_EQ(sMario.action, ACT_WATER_THROW);
    EXPECT_EQ(sMario.heldObj == &sShell, TRUE); // The throw animation owns release.

    reset_shell();
    sMario.heldObj = &sShell;
    sMario.action = ACT_WATER_SHELL_SWIMMING;
    sMarioObject.oInteractStatus = INT_STATUS_MARIO_DROP_OBJECT;
    EXPECT_EQ(act_water_shell_swimming(&sMario), TRUE);
    EXPECT_EQ(sMario.action, ACT_WATER_IDLE);
    EXPECT_EQ(sMario.heldObj == NULL, TRUE);
    return TRUE;
}

static int water_surface_jump(void) {
    for (int blocked = 0; blocked < 5; blocked++) {
        reset_shell();
        sMusicStops = 0;
        sMario.heldObj = &sShell;
        sMario.action = ACT_WATER_SHELL_SWIMMING;
        sMario.pos[1] = blocked == 1 ? 918 : 920;
        sMario.input = blocked == 2 ? 0 : INPUT_A_PRESSED | INPUT_A_DOWN;
        sMario.faceAngle[0] = blocked == 3 ? -1 : 0;
        sController.stickY = blocked == 4 ? -60 : -64;
        EXPECT_EQ(act_water_shell_swimming(&sMario), blocked == 0);
        EXPECT_EQ(sMario.action, blocked == 0 ? ACT_WATER_JUMP : ACT_WATER_SHELL_SWIMMING);
        EXPECT_EQ(sMario.heldObj == NULL, blocked == 0);
        EXPECT_EQ(sShell.oInteractStatus, blocked == 0 ? INT_STATUS_STOP_RIDING : 0);
        EXPECT_EQ(sMusicStops, blocked == 0);
    }
    return TRUE;
}

int main(int argc, char **argv) {
    test_init(argc, argv);
    struct {
        const char *name;
        int (*run)(void);
    } cases[] = {
        { "shell.pickup_by_cap", pickup_by_cap },
        { "shell.ordinary_swimmer_grabs", ordinary_swimmer_grabs },
        { "shell.released_shell_cannot_remount", released_shell_cannot_remount },
        { "shell.cap_expiry_handoff", cap_expiry_handoff },
        { "shell.cap_presets_handoff", cap_presets_handoff },
        { "shell.cap_pickup_while_holding", cap_pickup_while_holding },
        { "shell.surface_cap_expiry_swims", surface_cap_expiry_swims },
        { "shell.dry_or_shallow_water_keeps_riding", dry_or_shallow_water_keeps_riding },
        { "shell.airborne_above_water_waits_to_swim", airborne_above_water_waits_to_swim },
        { "shell.riding_shell_release", riding_shell_release },
        { "shell.pickup_visibility", pickup_visibility },
        { "shell.held_shell_follows", held_shell_follows },
        { "shell.underwater_shell_lifetime", underwater_shell_lifetime },
        { "shell.protection", shell_protection },
        { "shell.water_surface_jump", water_surface_jump },
    };
    for (unsigned i = 0; i < sizeof(cases) / sizeof(cases[0]); i++) {
        if (test_selected(cases[i].name)) {
            test_report(cases[i].name, cases[i].run());
        }
    }
    return test_finish();
}
