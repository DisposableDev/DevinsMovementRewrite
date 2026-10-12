/* Production purple-switch and stair loops with controlled object scheduling.
 * No level loading, collision publication, audio synthesis, or emulator. */
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include "test_support.h"
typedef int32_t s32;
typedef uint32_t u32;
typedef uint8_t u8;
typedef int16_t s16;
typedef float f32;
typedef int Collision;
typedef int ModelID16;
#define TRUE 1
#define FALSE 0
#define COND_BIT(cond, flags, bit) ((cond) ? ((flags) |= (bit)) : ((flags) &= ~(bit)))
#define MODEL_PURPLE_SWITCH 1
#define SOUND_GENERAL2_PURPLE_SWITCH 2
#define SOUND_GENERAL2_SWITCH_TICK_FAST 3
#define SOUND_GENERAL2_SWITCH_TICK_SLOW 4
#define SHAKE_POS_SMALL 1
#define OBJ_LIST_SURFACE 2
#define ACTIVE_FLAG_DEACTIVATED 0
#define INTERACT_BREAKABLE 1
#define INT_STATUS_NONE 0
#define MODEL_BREAKABLE_BOX 5
#define MODEL_DIRT_ANIMATION 6
#define TINY_DIRT_PARTICLE_ANIM_STATE_YELLOW 0
#define SOUND_GENERAL_BREAK_BOX 7
#define SOUND_GENERAL_POUND_WOOD_POST 8
#define SOUND_GENERAL_ACTIVATE_CAP_SWITCH 10
#define SOUND_GENERAL_SWITCH_DOOR_OPEN 11
#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define BREAKABLE_BOX_BP_NO_COINS 0
#define BREAKABLE_BOX_BP_3_COINS 1
#define BREAKABLE_BOX_BP_5_COINS 2
#define BREAKABLE_BOX_BP_LARGE 3
#define BREAKABLE_BOX_ACT_HIDDEN 0
#define BREAKABLE_BOX_ACT_ACTIVE 1
#define BREAKABLE_BOX_ACT_BROKEN 2
#define BREAKABLE_BOX_ANIM_STATE_CORK_BOX 1
#include "switches_definitions.inc"

struct ObjectNode {
    struct {
        struct {
            u32 flags;
        } node;
        f32 scale[3];
        f32 cameraToObject[3];
    } gfx;
    struct ObjectNode *next;
};
struct Object {
    struct ObjectNode header;
    struct Object *parentObj, *platform;
    const Collision *collisionData;
    s32 oAction, oTimer, oBehParams2ndByte;
    s32 oPurpleSwitchActivationSerial, oPurpleSwitchPressedLatch, oPurpleSwitchEffectTimer;
    s32 oFloorSwitchPressAnimationTickTimer, oFloorSwitchPressAnimationDoubleFrame;
    s32 oFloorSwitchPressAnimationLastPress;
    struct Object *oHiddenObjectSwitchObj;
    s32 oHiddenObjectLastSwitchPress, oHiddenObjectInitialized;
    s32 activeFlags, oAnimState, oNumLootCoins, oInteractStatus, oIntangibleTimer;
    const void *behavior;
    f32 posX;
    f32 oPosY, oVelY, oGravity, oSignpostSinkDepth, oSignpostTargetSinkDepth, hitboxHeight;
    s32 oSignpostMarioPounding;
};
struct ObjectHitbox {
    s32 interactType, downOffset, damageOrCoinValue, health, numLootCoins;
    s32 radius, height, hurtboxRadius, hurtboxHeight;
};
struct Surface {
    struct Object *object;
};
struct MarioState {
    s32 metalFallGroundPoundImpact;
    struct Object *metalFallGroundPoundFloorObject;
    u32 action, actionArg, input, particleFlags, flags, terrainSoundAddend;
    struct Object *marioObj;
    s32 actionState, actionTimer, waterGroundPoundImpact;
    struct Surface *floor;
    struct Object *waterGroundPoundFloorObject;
};
struct FloorSwitchTriggeredAnimationFrame {
    const Collision *collision;
    ModelID16 model;
};
static struct FloorSwitchTriggeredAnimationFrame sFloorSwitchTriggeredAnimationFrames[3][5];
static s16 sAnimatesOnFloorSwitchPressTimers[] = { 250, 200, 200 };
static struct Object *gCurrentObject, *gMarioObject;
#define o gCurrentObject
static struct MarioState gMarioStates[1];
static f32 sDistance;
static s32 sPressSounds, sShakes, sRumbles, sTickSounds, sWarningTickSounds;
static f32 sGlobalSound;
static f32 *gGlobalSoundSource = &sGlobalSound;
static struct ObjectNode gObjectLists[16];
static struct ObjectHitbox sBreakableBoxHitbox;
static const Collision wdw_seg7_collision_hidden_platform[1] = { 0 };
static int sHiddenBehavior, sAliasBehavior, sHardcodedBehavior;
#define bhvHiddenBlueCoin (&sHiddenBehavior)
#define bhvFloorSwitchHiddenObjects (&sHiddenBehavior)
#define bhvPurpleSwitchHiddenBoxes (&sAliasBehavior)
#define bhvFloorSwitchHardcodedModel (&sHardcodedBehavior)
static s32 sAttacked, sCollisionCalls, sSignSounds;
static f32 sCollisionY;
static struct {
    s32 burstRecovering, waterDownImpactCooldown;
} sSwimming;
#define SOUND_ACTION_TERRAIN_HEAVY_LANDING 1
#define SOUND_ACTION_METAL_HEAVY_LANDING 9
static s32 b_swim_has_priority(struct MarioState *m) {
    return m->input & INPUT_B_DOWN;
}
static void mario_ground_pound_impact(struct MarioState *m) {
}
static void mario_attack_cap_box_surface(struct MarioState *m, struct Surface *surface,
                                         u32 interaction) {
}
static s32 set_mario_action(struct MarioState *m, u32 action, u32 arg) {
    m->action = action;
    m->actionArg = arg;
    m->actionState = m->actionTimer = 0;
    return TRUE;
}

static s32 cur_obj_is_mario_on_platform(void) {
    return gMarioObject->platform == o;
}
static f32 lateral_dist_between_objects(struct Object *a, struct Object *b) {
    return sDistance;
}
static void cur_obj_set_model(s32 model) {
}
static void cur_obj_scale(f32 scale) {
    o->header.gfx.scale[0] = o->header.gfx.scale[1] = o->header.gfx.scale[2] = scale;
}
static f32 approach_f32(f32 value, f32 target, f32 inc, f32 dec) {
    return value < target ? fminf(value + inc, target) : fmaxf(value - dec, target);
}
static void cur_obj_play_sound_2(s32 sound) {
    if (sound == SOUND_GENERAL2_PURPLE_SWITCH) {
        sPressSounds++;
    }
    if (sound == SOUND_GENERAL_POUND_WOOD_POST) {
        sSignSounds++;
    }
}
static void cur_obj_play_sound_1(s32 sound) {
    sTickSounds++;
}
static void cur_obj_shake_screen(s32 shake) {
    sShakes++;
}
static void queue_rumble_data(s32 a, s32 b) {
    sRumbles++;
}
static void play_sound(s32 sound, f32 *source) {
    sTickSounds++;
    if (sound == SOUND_GENERAL2_SWITCH_TICK_SLOW) {
        sWarningTickSounds++;
    }
}
static const void *segmented_to_virtual(const void *ptr) {
    return ptr;
}
static s32 obj_has_behavior(struct Object *obj, const void *behavior) {
    return obj->behavior == behavior;
}
static f32 dist_between_objects(struct Object *a, struct Object *b) {
    return fabsf(a->posX - b->posX);
}
static void obj_set_hitbox(struct Object *obj, struct ObjectHitbox *hitbox) {
}
static void obj_set_collision_data(struct Object *obj, const Collision *data) {
}
static s32 cur_obj_was_attacked_or_ground_pounded(void) {
    return sAttacked;
}
static void spawn_mist_particles(void) {
}
static void spawn_triangle_break_particles(s32 count, s32 model, f32 scale, s32 state) {
}
static void load_object_collision_model(void) {
    sCollisionCalls++;
    sCollisionY = o->oPosY;
}
static u32 sSavedFlags;
static void save_file_set_flags(u32 flags) {
    sSavedFlags |= flags;
}
static void cur_obj_hide(void) {
}
static void spawn_mist_particles_variable(s32 count, s32 offset, f32 size) {
}
static void cur_obj_move_using_fvel_and_gravity(void) {
    o->oPosY += o->oVelY;
}
static struct Object *cur_obj_nearest_object_with_behavior(const void *behavior) {
    return gMarioObject;
}
static void obj_mark_for_deletion(struct Object *object) {
    object->activeFlags = 0;
}
#include "switches_sources.inc"

static struct Object sSwitch, sMario, sStair;
static void begin(s32 kind) {
    memset(&sSwitch, 0, sizeof(sSwitch));
    memset(&sMario, 0, sizeof(sMario));
    memset(&sStair, 0, sizeof(sStair));
    sSwitch.oBehParams2ndByte = kind;
    sStair.parentObj = &sSwitch;
    sStair.oBehParams2ndByte = 1;
    gMarioObject = &sMario;
    memset(gMarioStates, 0, sizeof(gMarioStates));
    sDistance = 0;
    sPressSounds = sShakes = sRumbles = sTickSounds = 0;
    sWarningTickSounds = 0;
    sAttacked = 0;
    sCollisionCalls = sSignSounds = 0;
    sSwimming.burstRecovering = FALSE;
    sSwimming.waterDownImpactCooldown = 0;
    sTestGameOptions[GO_SWITCHES] = 0;
    sSavedFlags = 0;
    gMarioStates[0].marioObj = &sMario;
}
static void step_switch(void) {
    s32 oldAction = sSwitch.oAction;
    gCurrentObject = &sSwitch;
    bhv_purple_switch_loop();
    sSwitch.oTimer = sSwitch.oAction == oldAction ? sSwitch.oTimer + 1 : 0;
}
static void step_stair(void) {
    gCurrentObject = &sStair;
    bhv_animates_on_floor_switch_press_loop();
}

static int switch_presets(void) {
    for (s32 original = 0; original < 2; original++) {
        for (s32 impact = 0; impact < 5; impact++) {
            for (s32 kind = 0; kind < 3; kind++) {
                begin(PURPLE_SWITCH_BP_REVEAL_HIDDEN);
                sTestGameOptions[GO_SWITCHES] = original;
                gCurrentObject = &sSwitch;
                if (impact == 0) {
                    sMario.platform = &sSwitch;
                } else if (impact == 1) {
                    sMario.platform = &sSwitch;
                    gMarioStates[0].action = ACT_GROUND_POUND_LAND;
                } else if (impact == 2) {
                    gMarioStates[0].metalFallGroundPoundImpact = TRUE;
                    gMarioStates[0].metalFallGroundPoundFloorObject = &sSwitch;
                } else {
                    gMarioStates[0].action = ACT_FLUTTER_KICK;
                    gMarioStates[0].waterGroundPoundImpact = TRUE;
                    gMarioStates[0].waterGroundPoundFloorObject = impact == 3 ? &sSwitch : &sStair;
                }
                s32 accepted = impact == 0 || impact == 1 || (!original && impact < 4);
                if (kind == 0) {
                    if (original && (gMarioStates[0].action & MARIO_NO_PURPLE_SWITCH)) {
                        accepted = FALSE;
                    }
                    step_switch();
                    EXPECT_EQ(sSwitch.oPurpleSwitchActivationSerial, accepted);
                    step_switch();
                    EXPECT_EQ(sSwitch.oPurpleSwitchActivationSerial, accepted);
                } else if (kind == 1) {
                    accepted = impact < 2 || (!original && impact < 4);
                    sSwitch.oBehParams2ndByte = 0;
                    cap_switch_act_idle_unpressed();
                    EXPECT_EQ(sSavedFlags != 0, accepted);
                } else {
                    accepted = impact == 1 || (!original && impact >= 2 && impact < 4);
                    sSwitch.oAction = BLUE_COIN_SWITCH_ACT_IDLE;
                    bhv_blue_coin_switch_loop();
                    EXPECT_EQ(sSwitch.oAction == BLUE_COIN_SWITCH_ACT_RECEDING, accepted);
                    EXPECT_EQ(sCollisionCalls, 1);
                }
            }
        }
    }
    // A preset change while standing cannot publish a second press.
    begin(PURPLE_SWITCH_BP_REVEAL_HIDDEN);
    sMario.platform = &sSwitch;
    step_switch();
    sTestGameOptions[GO_SWITCHES] = 1;
    step_switch();
    EXPECT_EQ(sSwitch.oPurpleSwitchActivationSerial, 1);
    sMario.platform = NULL;
    step_switch();
    sMario.platform = &sSwitch;
    step_switch();
    EXPECT_EQ(sSwitch.oPurpleSwitchActivationSerial, 2);
    EXPECT_EQ(sSwitch.oPurpleSwitchEffectTimer, PURPLE_SWITCH_HIDDEN_ACTIVE_FRAMES - 1);
    return 1;
}

static int stairs_run_after_release(void) {
    for (int reverseOrder = 0; reverseOrder < 2; reverseOrder++) {
        begin(PURPLE_SWITCH_BP_ANIMATES);
        sMario.platform = &sSwitch;
        if (reverseOrder) {
            step_stair();
        }
        step_switch();
        if (!reverseOrder) {
            step_stair();
        }
        if (sSwitch.oPurpleSwitchActivationSerial != 1 || sPressSounds != 1) {
            return 0;
        }
        sMario.platform = NULL;
        for (int frame = 0; frame < 80; frame++) {
            if (reverseOrder) {
                step_stair();
            }
            step_switch();
            if (!reverseOrder) {
                step_stair();
            }
        }
        if (sStair.oFloorSwitchPressAnimationTickTimer <= 0
            || sStair.oFloorSwitchPressAnimationDoubleFrame != 9
            || sSwitch.oPurpleSwitchActivationSerial != 1) {
            return 0;
        }
        sMario.platform = &sSwitch;
        step_switch();
        step_stair();
        if (sSwitch.oPurpleSwitchActivationSerial != 2
            || sStair.oFloorSwitchPressAnimationTickTimer != 199
            || sStair.oFloorSwitchPressAnimationDoubleFrame != 9) {
            return 0;
        }
        for (int frame = 0; frame < 205; frame++) {
            step_switch();
            step_stair();
        }
        if (sSwitch.oPurpleSwitchActivationSerial != 2
            || sStair.oFloorSwitchPressAnimationTickTimer != 0 || sPressSounds != 2 || sShakes != 2
            || sRumbles != 2) {
            return 0;
        }
    }
    return 1;
}

static int hidden_effect_runs_after_release(void) {
    begin(PURPLE_SWITCH_BP_REVEAL_HIDDEN);
    sMario.platform = &sSwitch;
    step_switch();
    if (sSwitch.oPurpleSwitchActivationSerial != 1
        || sSwitch.oPurpleSwitchEffectTimer != PURPLE_SWITCH_HIDDEN_ACTIVE_FRAMES - 1) {
        return 0;
    }
    sMario.platform = NULL;
    for (int frame = 1; frame < PURPLE_SWITCH_HIDDEN_ACTIVE_FRAMES; frame++) {
        step_switch();
    }
    if (sSwitch.oPurpleSwitchEffectTimer != 0 || sSwitch.oPurpleSwitchActivationSerial != 1) {
        return 0;
    }
    EXPECT_EQ(sWarningTickSounds, 90);
    sMario.platform = &sSwitch;
    step_switch();
    if (sSwitch.oPurpleSwitchActivationSerial != 2
        || sSwitch.oPurpleSwitchEffectTimer != PURPLE_SWITCH_HIDDEN_ACTIVE_FRAMES - 1) {
        return 0;
    }
    return 1;
}

static int underwater_impact_presses_switch(void) {
    const u32 inputs[] = { 0, INPUT_A_DOWN, INPUT_B_DOWN };
    for (int input = 0; input < 3; input++) {
        begin(PURPLE_SWITCH_BP_ANIMATES);
        struct Surface floor = { .object = &sSwitch };
        struct Object otherSwitch = { 0 };
        struct MarioState *m = &gMarioStates[0];
        for (int cycle = 0; cycle < 3; cycle++) {
            // Surface objects run before Mario: ordinary swimming contact must
            // not press the switch, even when platform bookkeeping has caught up.
            m->action = ACT_WATER_ACTION_END;
            sMario.platform = &sSwitch;
            step_switch();
            EXPECT_EQ(sSwitch.oPurpleSwitchActivationSerial, cycle);
            sMario.platform = NULL;
            m->action = ACT_WATER_DOWN_BURST;
            m->actionState = ACT_STATE_WATER_DOWN_BURST_SINK;
            m->input = inputs[input];
            m->floor = &floor;
            floor.object = &sSwitch;
            finish_water_down_burst(m, FALSE);
            step_switch();
            EXPECT_EQ(sSwitch.oPurpleSwitchActivationSerial, cycle);
            // A new deliberate impact follows expiry of the shared cooldown.
            sSwimming.waterDownImpactCooldown = 0;
            finish_water_down_burst(m, TRUE);
            EXPECT_EQ((m->action & MARIO_NO_PURPLE_SWITCH) != 0, TRUE);
            // The next surface pass can overwrite the old floor allocation.
            floor.object = &otherSwitch;
            gCurrentObject = &otherSwitch;
            EXPECT_EQ(cur_obj_is_mario_ground_pounding_platform(), FALSE);
            m->floor = NULL;
            step_switch();
            EXPECT_EQ(sSwitch.oPurpleSwitchActivationSerial, cycle + 1);
            EXPECT_EQ(sPressSounds, cycle + 1);
            step_switch();
            EXPECT_EQ(sSwitch.oPurpleSwitchActivationSerial, cycle + 1);
            // Clear the one-update event before Mario's next movement.
            m->waterGroundPoundImpact = FALSE;
            m->waterGroundPoundFloorObject = NULL;
            step_switch();
            EXPECT_EQ(sSwitch.oPurpleSwitchPressedLatch, FALSE);
        }
        // An unrelated action interrupt makes an unconsumed impact ineligible.
        floor.object = &sSwitch;
        m->floor = &floor;
        finish_water_down_burst(m, TRUE);
        m->action = ACT_FORWARD_WATER_KB;
        step_switch();
        EXPECT_EQ(sSwitch.oPurpleSwitchActivationSerial, 3);
    }
    return 1;
}

static void step_box(struct Object *box, s32 breakable) {
    s32 oldAction = box->oAction;
    gCurrentObject = box;
    if (breakable) {
        hidden_breakable_box_actions();
    } else {
        hidden_unbreakable_box_actions();
    }
    box->oTimer = box->oAction == oldAction ? box->oTimer + 1 : 0;
}

static int hidden_boxes_need_fresh_press(void) {
    struct Object hidden, alias, box, platform;
    begin(PURPLE_SWITCH_BP_REVEAL_HIDDEN);
    memset(&hidden, 0, sizeof(hidden));
    memset(&alias, 0, sizeof(alias));
    memset(&box, 0, sizeof(box));
    memset(&platform, 0, sizeof(platform));
    hidden.activeFlags = alias.activeFlags = sSwitch.activeFlags = 1;
    hidden.behavior = bhvFloorSwitchHiddenObjects;
    alias.behavior = bhvPurpleSwitchHiddenBoxes;
    sSwitch.behavior = bhvFloorSwitchHardcodedModel;
    hidden.oBehParams2ndByte = alias.oBehParams2ndByte = PURPLE_SWITCH_BP_REVEAL_HIDDEN;
    hidden.posX = 300;
    alias.posX = 100;
    sSwitch.posX = 200;
    box.posX = 95;
    gObjectLists[OBJ_LIST_SURFACE].next = &hidden.header;
    hidden.header.next = &sSwitch.header;
    sSwitch.header.next = &alias.header;
    alias.header.next = &gObjectLists[OBJ_LIST_SURFACE];

    step_box(&box, TRUE);
    if (box.oHiddenObjectSwitchObj != &alias || box.oAction != BREAKABLE_BOX_ACT_HIDDEN) {
        return 0;
    }
    alias.oPurpleSwitchActivationSerial = 1;
    alias.oPurpleSwitchEffectTimer = PURPLE_SWITCH_HIDDEN_ACTIVE_FRAMES;
    step_box(&box, TRUE);
    if (box.oAction != BREAKABLE_BOX_ACT_ACTIVE || box.oHiddenObjectLastSwitchPress != 1) {
        return 0;
    }
    sAttacked = TRUE;
    step_box(&box, TRUE);
    sAttacked = FALSE;
    if (box.oAction != BREAKABLE_BOX_ACT_BROKEN) {
        return 0;
    }
    for (int frame = 0; frame < 5; frame++) {
        step_box(&box, TRUE);
    }
    if (box.oAction != BREAKABLE_BOX_ACT_BROKEN) {
        return 0;
    }
    alias.oPurpleSwitchActivationSerial = 2;
    step_box(&box, TRUE);
    if (box.oAction != BREAKABLE_BOX_ACT_ACTIVE || box.oTimer != 0) {
        return 0;
    }
    for (int frame = 0; frame <= PURPLE_SWITCH_HIDDEN_ACTIVE_FRAMES; frame++) {
        step_box(&box, TRUE);
    }
    if (box.oAction != BREAKABLE_BOX_ACT_HIDDEN) {
        return 0;
    }
    step_box(&box, TRUE);
    if (box.oAction != BREAKABLE_BOX_ACT_HIDDEN || box.oHiddenObjectLastSwitchPress != 2) {
        return 0;
    }
    alias.oPurpleSwitchActivationSerial = 3;
    step_box(&box, TRUE);
    if (box.oAction != BREAKABLE_BOX_ACT_ACTIVE || box.oHiddenObjectLastSwitchPress != 3) {
        return 0;
    }

    memset(&platform, 0, sizeof(platform));
    platform.posX = 95;
    step_box(&platform, FALSE);
    if (platform.oHiddenObjectSwitchObj != &alias || platform.oAction != BREAKABLE_BOX_ACT_ACTIVE) {
        return 0;
    }
    alias.oPurpleSwitchActivationSerial = 4;
    step_box(&platform, FALSE);
    if (platform.oAction != BREAKABLE_BOX_ACT_ACTIVE || platform.oTimer != 1) {
        return 0;
    }
    return 1;
}

static int hidden_box_warning_timeline(void) {
    for (int breakable = FALSE; breakable <= TRUE; breakable++) {
        begin(PURPLE_SWITCH_BP_REVEAL_HIDDEN);
        sSwitch.activeFlags = 1;
        sSwitch.behavior = bhvFloorSwitchHardcodedModel;
        gObjectLists[OBJ_LIST_SURFACE].next = &sSwitch.header;
        sSwitch.header.next = &gObjectLists[OBJ_LIST_SURFACE];
        struct Object box = { .activeFlags = 1 };
        for (int cycle = 0; cycle < 3; cycle++) {
            sMario.platform = &sSwitch;
            step_switch();
            step_box(&box, breakable);
            EXPECT_EQ(box.oAction, BREAKABLE_BOX_ACT_ACTIVE);
            EXPECT_EQ(box.header.gfx.node.flags & GRAPH_RENDER_INVISIBLE, 0);
            int invisibleUpdates = 0;
            for (int age = 0; age < 404; age++) {
                EXPECT_EQ(box.oTimer, age);
                sCollisionCalls = 0;
                step_box(&box, breakable);
                EXPECT_EQ(box.oAction, BREAKABLE_BOX_ACT_ACTIVE);
                EXPECT_EQ(sCollisionCalls, 1);
                EXPECT_EQ(box.oIntangibleTimer, 0);
                EXPECT_EQ(box.header.gfx.node.flags & GRAPH_RENDER_ACTIVE, GRAPH_RENDER_ACTIVE);
                int invisible = !!(box.header.gfx.node.flags & GRAPH_RENDER_INVISIBLE);
                EXPECT_EQ(invisible, age >= 314 && ((age - 314) & 1));
                EXPECT_EQ(!!(box.activeFlags & ACTIVE_FLAG_BLINKING), invisible);
                invisibleUpdates += invisible;
            }
            EXPECT_EQ(invisibleUpdates, 45); // 90 alternating warning updates.
            sCollisionCalls = 0;
            step_box(&box, breakable);
            EXPECT_EQ(box.oAction, BREAKABLE_BOX_ACT_HIDDEN);
            EXPECT_EQ(sCollisionCalls, 0);
            EXPECT_EQ(box.oIntangibleTimer, -1);
            EXPECT_EQ(box.header.gfx.node.flags & GRAPH_RENDER_ACTIVE, 0);
            sMario.platform = NULL;
            step_switch();
        }
        // Repress on an invisible warning update restores a full, visible cycle.
        sMario.platform = &sSwitch;
        step_switch();
        step_box(&box, breakable);
        box.oTimer = 315;
        step_box(&box, breakable);
        EXPECT_EQ(box.header.gfx.node.flags & GRAPH_RENDER_INVISIBLE, GRAPH_RENDER_INVISIBLE);
        sMario.platform = NULL;
        step_switch();
        sMario.platform = &sSwitch;
        step_switch();
        sCollisionCalls = 0;
        step_box(&box, breakable);
        EXPECT_EQ(box.oTimer, 1);
        EXPECT_EQ(box.header.gfx.node.flags & GRAPH_RENDER_INVISIBLE, 0);
        EXPECT_EQ(sCollisionCalls, 1);
        for (int age = 1; age < 404; age++) {
            step_box(&box, breakable);
            EXPECT_EQ(box.oAction, BREAKABLE_BOX_ACT_ACTIVE);
        }
        step_box(&box, breakable);
        EXPECT_EQ(box.oAction, BREAKABLE_BOX_ACT_HIDDEN);
        if (breakable) {
            for (int age = 314; age <= 403; age += 89) {
                box.oAction = BREAKABLE_BOX_ACT_ACTIVE;
                box.oTimer = age;
                sAttacked = TRUE;
                sCollisionCalls = 0;
                step_box(&box, TRUE);
                EXPECT_EQ(box.oAction, BREAKABLE_BOX_ACT_BROKEN);
                EXPECT_EQ(sCollisionCalls, 0);
                EXPECT_EQ(box.header.gfx.node.flags & GRAPH_RENDER_ACTIVE, 0);
                sAttacked = FALSE;
            }
        }
    }
    return TRUE;
}

static int signs_sink_twice(void) {
    for (int contact = 0; contact < 3; contact++) {
        begin(0);
        gCurrentObject = &sSwitch;
        sSwitch.oPosY = 1000.0f;
        sSwitch.hitboxHeight = 80.0f;
        for (int impact = 0; impact < 3; impact++) {
            gMarioStates[0].action = ACT_FLUTTER_KICK;
            gMarioStates[0].waterGroundPoundImpact = FALSE;
            gMarioStates[0].metalFallGroundPoundImpact = FALSE;
            sMario.platform = NULL;
            bhv_message_panel_loop();
            if (contact == 1) {
                gMarioStates[0].waterGroundPoundImpact = TRUE;
                gMarioStates[0].waterGroundPoundFloorObject = &sSwitch;
            } else if (contact == 0) {
                gMarioStates[0].action = ACT_GROUND_POUND_LAND;
                sMario.platform = &sSwitch;
            }
            if (contact == 2) {
                gMarioStates[0].metalFallGroundPoundImpact = TRUE;
                gMarioStates[0].metalFallGroundPoundFloorObject = &sSwitch;
            }
            for (int frame = 0; frame < 12; frame++) {
                f32 oldY = sSwitch.oPosY;
                bhv_message_panel_loop();
                f32 expected = impact < 2 ? 6.0f * MIN(frame + 1, 5) + impact * 30 : 60;
                EXPECT_EQ(sSwitch.oPosY, 1000.0f - expected);
                EXPECT_EQ(oldY - sSwitch.oPosY <= 6.0f, TRUE);
                EXPECT_EQ(sCollisionY, sSwitch.oPosY);
                EXPECT_EQ(sSwitch.oPosY + sSwitch.hitboxHeight, 1080.0f);
                EXPECT_EQ(sSwitch.oIntangibleTimer, 0);
                EXPECT_EQ(sSignSounds, MIN(impact + 1, 2));
            }
        }
        EXPECT_EQ(sSwitch.oSignpostSinkDepth, 60);
        EXPECT_EQ(sSwitch.oSignpostTargetSinkDepth, 60);
        EXPECT_EQ(sCollisionCalls, 39);
    }
    return TRUE;
}

static int signs_require_top_impact(void) {
    begin(0);
    gCurrentObject = &sSwitch;
    sSwitch.oPosY = 1000.0f;
    for (int contact = 0; contact < 3; contact++) {
        gMarioStates[0].action = contact == 0 ? ACT_FLUTTER_KICK : ACT_GROUND_POUND_LAND;
        sMario.platform = contact == 1 ? &sStair : &sSwitch;
        gMarioStates[0].waterGroundPoundImpact = contact == 2;
        gMarioStates[0].waterGroundPoundFloorObject = &sStair;
        if (contact == 2) {
            sMario.platform = NULL;
        }
        bhv_message_panel_loop();
        EXPECT_EQ(sSwitch.oPosY, 1000);
        EXPECT_EQ(sSwitch.oSignpostTargetSinkDepth, 0);
        EXPECT_EQ(sSignSounds, 0);
    }
    return TRUE;
}

int main(int argc, char **argv) {
    test_init(argc, argv);
    if (test_selected("switches.presets")) {
        test_report("switches.presets", switch_presets());
    }
    if (test_selected("switches.signs_sink_twice")) {
        test_report("switches.signs_sink_twice", signs_sink_twice());
    }
    if (test_selected("switches.signs_require_top_impact")) {
        test_report("switches.signs_require_top_impact", signs_require_top_impact());
    }
    if (test_selected("switches.box_warning_timeline")) {
        test_report("switches.box_warning_timeline", hidden_box_warning_timeline());
    }
    if (test_selected("switches.stairs_release")) {
        test_report("switches.stairs_release", stairs_run_after_release());
    }
    if (test_selected("switches.hidden_release")) {
        test_report("switches.hidden_release", hidden_effect_runs_after_release());
    }
    if (test_selected("switches.box_cycles")) {
        test_report("switches.box_cycles", hidden_boxes_need_fresh_press());
    }
    if (test_selected("switches.underwater_impact")) {
        test_report("switches.underwater_impact", underwater_impact_presses_switch());
    }
    return test_finish();
}
