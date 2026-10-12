/* Production swim input/actions and animation playback; movement, loading and audio are services. */
#include <stdint.h>
#include <math.h>
#include "test_support.h"

typedef int8_t s8;
typedef uint8_t u8;
typedef int16_t s16;
typedef uint16_t u16;
typedef int32_t s32;
typedef uint32_t u32;
typedef float f32;
typedef f32 Vec3f[3];
#define TRUE 1
#define FALSE 0
#define BIT(n) (1U << (n))
#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define GET_HIGH_S16_OF_32(v) ((s16) ((u32) (v) >> 16))
#define SET_HIGH_S16_OF_32(v, h) ((v) = (s32) (((u32) (v) & 0xffff) | ((u32) (u16) (h) << 16)))
#define VIRTUAL_TO_PHYSICAL(v) (v)
#define INT_STATUS_MARIO_DROP_OBJECT 1
#define SOUND_ACTION_SWIM 1
#define SOUND_ACTION_SWIM_FAST 2
#define L_TRIG 0x40
#include "swim_animation_definitions.inc"

struct Object {
    struct {
        struct {
            s16 angle[3];
            struct AnimInfo animInfo;
            f32 cameraToObject[3], pos[3], scale[3];
        } gfx;
    } header;
    u32 oInteractStatus;
};
struct Controller {
    f32 stickY;
    u16 buttonPressed, buttonDown;
};
struct MarioAnimList {
    struct Animation *bufTarget;
};
struct MarioState {
    u32 action, actionArg, actionState, actionTimer, input, flags;
    f32 forwardVel, pos[3], vel[3], waterLevel;
    s16 faceAngle[3], angleVel[3], animYTrans, health;
    struct Object *marioObj, *heldObj;
    struct Controller *controller;
    struct MarioAnimList *animList;
    void *floor;
};

static void *segmented_to_virtual(void *p) {
    return p;
}
static f32 sins(s16 a) {
    return sinf((u16) a * (6.283185307179586f / 65536.0f));
}
static f32 coss(s16 a) {
    return cosf((u16) a * (6.283185307179586f / 65536.0f));
}
static u16 gAreaUpdateCounter;
static struct MarioState sMario;
static struct Object sObject, sHeld;
static struct Controller sController;
static struct Animation sAnimation;
static struct MarioAnimList sAnimList;
static s32 sSteps, sStrokeSounds, sRings, sRollSounds;
static void mario_play_air_twirl_sound(struct MarioState *m) {
    sRollSounds++;
}

static s16 set_mario_animation(struct MarioState *m, s32 id);
static s16 set_mario_anim_with_accel(struct MarioState *m, s32 id, s32 accel);
static void set_anim_to_frame(struct MarioState *m, s16 frame);
static void mario_transition_pose_begin(struct MarioState *m, s32 id) {
}
static s32 load_patchable_table(struct MarioAnimList *list, s32 id) {
    const struct Animation *anim = NULL;
    switch (id) {
        case MARIO_ANIM_START_GROUND_POUND:
            anim = anim_3C;
            break;
        case MARIO_ANIM_WATER_IDLE:
            anim = anim_B2;
            break;
        case MARIO_ANIM_SWIM_PART1:
            anim = anim_AA;
            break;
        case MARIO_ANIM_SWIM_PART2:
            anim = anim_AB;
            break;
        case MARIO_ANIM_FLUTTERKICK:
            anim = anim_AC;
            break;
        case MARIO_ANIM_WATER_IDLE_WITH_OBJ:
            anim = anim_A4;
            break;
        case MARIO_ANIM_SWIM_WITH_OBJ_PART1:
            anim = anim_9F;
            break;
        case MARIO_ANIM_SWIM_WITH_OBJ_PART2:
            anim = anim_A0;
            break;
        case MARIO_ANIM_FLUTTERKICK_WITH_OBJ:
            anim = anim_A1;
            break;
        default:
            fprintf(stderr, "HARNESS ERROR: unmodeled swim animation %d\n", id);
            exit(2);
    }
    *list->bufTarget = *anim;
    return FALSE; // Supplied assets already use host pointers.
}
static u32 set_mario_action(struct MarioState *m, u32 action, u32 arg) {
    m->action = action;
    m->actionArg = arg;
    m->actionState = 0;
    m->actionTimer = 0;
    m->flags &= ~MARIO_ACTION_SOUND_PLAYED;
    return TRUE;
}
static u32 drop_and_set_mario_action(struct MarioState *m, u32 action, u32 arg) {
    m->heldObj = NULL;
    return set_mario_action(m, action, arg);
}
static f32 approach_f32(f32 value, f32 target, f32 up, f32 down) {
    return value < target ? fminf(value + up, target) : fmaxf(value - down, target);
}
static void vec3_zero(s16 *v) {
    memset(v, 0, 3 * sizeof(*v));
}
static void common_idle_step(struct MarioState *m, s32 animation, s32 accel) {
    if (accel) {
        set_mario_anim_with_accel(m, animation, accel);
    } else {
        set_mario_animation(m, animation);
    }
}
static void common_swimming_step(struct MarioState *m, s32 strength, f32 drag) {
    sSteps++;
}
static void play_swimming_noise(struct MarioState *m) {
}
static void queue_rumble_submerged(void) {
}
static void reset_bob_variables(struct MarioState *m) {
}
static void mario_he_thicc_breaststroke(struct MarioState *m) {
    sRings++;
}
static void play_sound(s32 sound, f32 *pos) {
    sStrokeSounds++;
}
static void play_sound_if_no_flag(struct MarioState *m, s32 sound, u32 flag) {
    if (!(m->flags & flag)) {
        sStrokeSounds++;
        m->flags |= flag;
    }
}
static s32 geo_update_animation_frame(struct AnimInfo *obj, s32 *accelAssist);
static s32 retrieve_animation_index(s32 frame, u16 **attributes);
static void mario_stop_underwater_shell(struct MarioState *m) {
    (void) m;
}
#include "swim_animation_sources.inc"

static void render_frame(void) {
    struct AnimInfo *info = &sObject.header.gfx.animInfo;
    info->animFrame = geo_update_animation_frame(info, &info->animFrameAccelAssist);
    info->animTimer = gAreaUpdateCounter;
}
static void begin(s32 holding, s16 idleFrame, s32 fastIdle) {
    memset(&sMario, 0, sizeof(sMario));
    memset(&sObject, 0, sizeof(sObject));
    memset(&sHeld, 0, sizeof(sHeld));
    memset(&sController, 0, sizeof(sController));
    memset(&sSwimming, 0, sizeof(sSwimming));
    sSwimStrength = MIN_SWIM_STRENGTH;
    sWasAtSurface = FALSE;
    sSteps = sStrokeSounds = sRings = sRollSounds = 0;
    sMario.health = 0x880;
    sMario.animYTrans = 189;
    sObject.header.gfx.scale[0] = sObject.header.gfx.scale[1] = sObject.header.gfx.scale[2] = 1.0f;
    gAreaUpdateCounter = 1;
    sAnimList.bufTarget = &sAnimation;
    sMario.marioObj = &sObject;
    sMario.heldObj = holding ? &sHeld : NULL;
    sMario.controller = &sController;
    sMario.animList = &sAnimList;
    sMario.floor = &sHeld;
    sMario.pos[1] = -500.0f;
    sMario.action = holding ? ACT_HOLD_WATER_IDLE : ACT_WATER_IDLE;
    sMario.faceAngle[0] = fastIdle ? -0x2000 : 0;
    sObject.header.gfx.animInfo.animID = -1;
    if (holding) {
        act_hold_water_idle(&sMario);
    } else {
        act_water_idle(&sMario);
    }
    set_anim_to_frame(&sMario, idleFrame);
    render_frame();
}
static void update(u32 input) {
    sMario.input = input;
    gAreaUpdateCounter++;
    mario_swimming_update(&sMario);
    for (s32 dispatch = 0; dispatch < 8; dispatch++) {
        s32 again;
        switch (sMario.action) {
            case ACT_WATER_IDLE:
                again = act_water_idle(&sMario);
                break;
            case ACT_HOLD_WATER_IDLE:
                again = act_hold_water_idle(&sMario);
                break;
            case ACT_FLUTTER_KICK:
                again = act_flutter_kick(&sMario);
                break;
            case ACT_HOLD_FLUTTER_KICK:
                again = act_hold_flutter_kick(&sMario);
                break;
            case ACT_BREASTSTROKE:
                again = act_breaststroke(&sMario);
                break;
            case ACT_HOLD_BREASTSTROKE:
                again = act_hold_breaststroke(&sMario);
                break;
            case ACT_SWIMMING_END:
                again = act_swimming_end(&sMario);
                break;
            case ACT_HOLD_SWIMMING_END:
                again = act_hold_swimming_end(&sMario);
                break;
            default:
                return; // Interruption or end of the tested recovery.
        }
        clear_interrupted_swimming_controls(&sMario);
        if (!again) {
            apply_swimming_roll(&sMario);
            render_frame();
            return;
        }
    }
    fprintf(stderr, "HARNESS ERROR: swimming action loop did not settle\n");
    exit(2);
}

static int initial_tap_animation(void) {
    for (s32 holding = 0; holding < 2; holding++) {
        for (s32 fast = 0; fast < 2; fast++) {
            for (s32 release = 1; release <= A_SWIM_STROKE_TAP_FRAMES; release++) {
                begin(holding, 9, fast);
                for (s32 cycle = 0; cycle < 3; cycle++) {
                    s32 idleAnim = holding ? MARIO_ANIM_WATER_IDLE_WITH_OBJ : MARIO_ANIM_WATER_IDLE;
                    s32 strokeAnim = holding ? MARIO_ANIM_SWIM_WITH_OBJ_PART1 : MARIO_ANIM_SWIM_PART1;
                    for (s32 age = 0; age < release; age++) {
                        s16 frame = sObject.header.gfx.animInfo.animFrame;
                        s32 assist;
                        gAreaUpdateCounter++;
                        s16 expected =
                            geo_update_animation_frame(&sObject.header.gfx.animInfo, &assist);
                        gAreaUpdateCounter--;
                        update(INPUT_A_DOWN | (age == 0 ? INPUT_A_PRESSED : 0));
                        EXPECT_EQ(sObject.header.gfx.animInfo.animID, idleAnim);
                        EXPECT_EQ(sObject.header.gfx.animInfo.animFrame, expected);
                        EXPECT_EQ(sObject.header.gfx.animInfo.animFrame != frame, TRUE);
                        EXPECT_EQ(sMario.forwardVel > 0.0f, TRUE);
                        EXPECT_EQ(sMario.action, holding ? ACT_HOLD_FLUTTER_KICK : ACT_FLUTTER_KICK);
                    }
                    s32 sounds = sStrokeSounds;
                    s32 rings = sRings;
                    s32 steps = sSteps;
                    update(0);
                    EXPECT_EQ(sMario.action, holding ? ACT_HOLD_BREASTSTROKE : ACT_BREASTSTROKE);
                    EXPECT_EQ(sMario.actionTimer, 1);
                    EXPECT_EQ(sObject.header.gfx.animInfo.animID, strokeAnim);
                    EXPECT_EQ(sObject.header.gfx.animInfo.animFrame, 0);
                    EXPECT_EQ(sStrokeSounds, sounds + 1);
                    EXPECT_EQ(sRings, rings + 1);
                    EXPECT_EQ(sSteps, steps + 1);
                    for (s32 frame = 1; frame < (holding ? 16 : BREASTSTROKE_DURATION - 1); frame++) {
                        update(0);
                        EXPECT_EQ(sObject.header.gfx.animInfo.animID, strokeAnim);
                        EXPECT_EQ(sObject.header.gfx.animInfo.animFrame, frame);
                        EXPECT_EQ(sMario.actionTimer, frame + 1);
                    }
                    update(0);
                    EXPECT_EQ(sMario.action, holding ? ACT_HOLD_SWIMMING_END : ACT_SWIMMING_END);
                    EXPECT_EQ(sObject.header.gfx.animInfo.animFrame, 0);
                    for (s32 recovery = 0; recovery < 16; recovery++) {
                        update(0);
                    }
                    EXPECT_EQ(sSwimming.strokeCooldown, 0);
                    // End-of-action idle placement is a service; the next tap uses the real
                    // input/actions.
                    set_mario_action(&sMario, holding ? ACT_HOLD_WATER_IDLE : ACT_WATER_IDLE, 0);
                    s32 acceleration = 0;
                    if (!holding) {
                        acceleration = fast ? 0x30000 : 0x10000;
                    }
                    common_idle_step(&sMario, idleAnim, acceleration);
                }
            }
        }
    }
    return 1;
}

static int long_hold_and_interruptions(void) {
    for (s32 holding = 0; holding < 2; holding++) {
        begin(holding, 5, FALSE);
        for (s32 age = 0; age <= A_SWIM_STROKE_TAP_FRAMES; age++) {
            update(INPUT_A_DOWN | (age == 0 ? INPUT_A_PRESSED : 0));
        }
        update(INPUT_A_DOWN);
        EXPECT_EQ(sObject.header.gfx.animInfo.animID,
                  holding ? MARIO_ANIM_FLUTTERKICK_WITH_OBJ : MARIO_ANIM_FLUTTERKICK);
        EXPECT_EQ(sObject.header.gfx.animInfo.animFrame, sAnimation.startFrame);
        EXPECT_EQ(sMario.forwardVel > 0.0f, TRUE);
        update(0);
        EXPECT_EQ(sMario.action, holding ? ACT_HOLD_SWIMMING_END : ACT_SWIMMING_END);
        EXPECT_EQ(sRings, 0);

        begin(holding, 5, FALSE);
        update(INPUT_A_PRESSED | INPUT_A_DOWN);
        sMario.flags |= MARIO_METAL_CAP;
        update(INPUT_A_DOWN);
        EXPECT_EQ(sMario.action, holding ? ACT_HOLD_METAL_WATER_FALLING : ACT_METAL_WATER_FALLING);
        EXPECT_EQ(sSwimming.aTapPending, FALSE);
        EXPECT_EQ(sRings, 0);
    }
    begin(FALSE, 5, FALSE);
    update(INPUT_A_PRESSED | INPUT_A_DOWN);
    update(INPUT_A_DOWN | INPUT_B_PRESSED | INPUT_B_DOWN);
    EXPECT_EQ(sMario.actionArg, ACT_ARG_FLUTTER_KICK_B_SWIM);
    EXPECT_EQ(sObject.header.gfx.animInfo.animID, MARIO_ANIM_FLUTTERKICK);
    EXPECT_EQ(sSwimming.aTapPending, FALSE);
    update(INPUT_B_DOWN);
    EXPECT_EQ(sRings, 0);

    begin(TRUE, 5, FALSE);
    update(INPUT_A_PRESSED | INPUT_A_DOWN);
    update(INPUT_A_DOWN | INPUT_B_PRESSED);
    EXPECT_EQ(sMario.action, ACT_WATER_THROW);
    EXPECT_EQ(sRings, 0);
    return 1;
}

static int water_pound_cooldown(void) {
    begin(FALSE, 0, FALSE);
    gAreaUpdateCounter++;
    s32 windupUpdates = 0;
    do {
        set_mario_anim_with_accel(&sMario, MARIO_ANIM_START_GROUND_POUND,
                                  (s32) (0x10000 * WATER_DOWN_ANIM_SPEED));
        windupUpdates++;
        if (is_anim_at_end(&sMario)) {
            break;
        }
        render_frame();
        gAreaUpdateCounter++;
    } while (windupUpdates < 100);
    EXPECT_EQ(windupUpdates, WATER_DOWN_IMPACT_COOLDOWN_FRAMES);

    sSwimming.waterDownImpactCooldown = windupUpdates;
    for (s32 remaining = windupUpdates - 1; remaining >= 0; remaining--) {
        // The once-per-update owner ticks even after swimming is interrupted.
        sMario.action = remaining & 1 ? ACT_WATER_IDLE : ACT_FREEFALL;
        mario_swimming_update(&sMario);
        EXPECT_EQ(sSwimming.waterDownImpactCooldown, remaining);
    }
    mario_swimming_update(&sMario);
    EXPECT_EQ(sSwimming.waterDownImpactCooldown, 0);
    return 1;
}

static void animated_hips_position(f32 *position, f32 height, const s16 *angle, const f32 *origin) {
    // Apply Z, then X, then Y to the animated hip point, matching the model transform.
    f32 x = -height * sins(angle[2]);
    f32 y = height * coss(angle[2]);
    f32 z = y * sins(angle[0]);
    position[0] = origin[0] + x * coss(angle[1]) + z * sins(angle[1]);
    position[1] = origin[1] + y * coss(angle[0]);
    position[2] = origin[2] - x * sins(angle[1]) + z * coss(angle[1]);
}

static int swimming_roll_pivot(void) {
    const s32 animations[] = { MARIO_ANIM_WATER_IDLE, MARIO_ANIM_SWIM_PART1, MARIO_ANIM_SWIM_PART2,
                               MARIO_ANIM_FLUTTERKICK };
    for (unsigned animation = 0; animation < sizeof(animations) / sizeof(animations[0]); animation++) {
        begin(FALSE, 0, FALSE);
        set_mario_animation(&sMario, animations[animation]);
        for (s32 frame = 1; frame <= 16; frame++) {
            sObject.header.gfx.animInfo.animFrame = frame - 1;
            s16 angle[3] = { -0x1800, 0x3500, 0x0800 };
            f32 origin[3] = { 120, -480, 360 };
            memcpy(sObject.header.gfx.angle, angle, sizeof(angle));
            memcpy(sObject.header.gfx.pos, origin, sizeof(origin));
            struct AnimInfo *info = &sObject.header.gfx.animInfo;
            u16 *index = (u16 *) info->curAnim->index + 2;
            s16 renderedFrame = geo_update_animation_frame(info, NULL);
            f32 height = info->curAnim->values[retrieve_animation_index(renderedFrame, &index)] * 0.25f
                         * info->animYTrans / info->curAnim->animYTransDivisor;
            f32 before[3], after[3];
            animated_hips_position(before, height, angle, origin);
            s16 bank = angle[2] + 0x10000 * frame / 16;
            center_swimming_roll(&sMario, bank);
            sObject.header.gfx.angle[2] = bank;
            animated_hips_position(after, height, sObject.header.gfx.angle, sObject.header.gfx.pos);
            for (s32 axis = 0; axis < 3; axis++) {
                EXPECT_EQ(fabsf(before[axis] - after[axis]) < 0.001f, TRUE);
            }
            EXPECT_EQ(sMario.pos[1], -500.0f);
            EXPECT_EQ(info->animFrame, frame - 1);
        }
    }
    return TRUE;
}

static int swimming_aileron_roll(void) {
    begin(FALSE, 0, FALSE);
    sController.buttonPressed = sController.buttonDown = L_TRIG;
    const f32 speed = sMario.forwardVel;
    const u32 action = sMario.action;
    sMario.faceAngle[2] = 100;
    for (int frame = 1; frame <= 16; frame++) {
        update(0);
        EXPECT_EQ(sMario.action, action);
        EXPECT_EQ(sMario.forwardVel, speed);
        EXPECT_EQ(sObject.header.gfx.angle[2], (s16) (100 + 0x10000 * frame / 16));
        EXPECT_EQ(sSwimming.rollFrame, frame < 16 ? frame : 0);
        sController.buttonPressed = 0;
    }
    EXPECT_EQ(sRollSounds, 1);
    update(0);
    EXPECT_EQ(sSwimming.rollFrame, 0);
    EXPECT_EQ(sObject.header.gfx.angle[2], 100);
    EXPECT_EQ(sSwimming.rollCooldown, 29);
    for (int frame = 0; frame < 28; frame++) {
        sController.buttonPressed = L_TRIG;
        update(0);
        EXPECT_EQ(sSwimming.rollFrame, 0);
    }
    sController.buttonPressed = 0;
    update(0); // Holding L alone never repeats the roll.
    EXPECT_EQ(sSwimming.rollFrame, 0);
    sController.buttonPressed = L_TRIG;
    update(0);
    EXPECT_EQ(sSwimming.rollFrame, 1);
    EXPECT_EQ(sRollSounds, 2);
    // Action interruption clears the displayed roll before another action runs.
    sMario.action = ACT_FREEFALL;
    mario_swimming_update(&sMario);
    EXPECT_EQ(sSwimming.rollFrame, 0);
    EXPECT_EQ(sObject.header.gfx.angle[2], 100);
    for (int holding = 0; holding < 2; holding++) {
        begin(holding, 0, FALSE);
        sController.buttonPressed = L_TRIG;
        update(INPUT_Z_PRESSED);
        EXPECT_EQ(sSwimming.rollFrame, 0);
        EXPECT_EQ(sRollSounds, 0);
    }
    begin(TRUE, 0, FALSE);
    sController.buttonPressed = L_TRIG;
    update(0);
    EXPECT_EQ(sSwimming.rollFrame, 0);
    EXPECT_EQ(sRollSounds, 0);
    u32 actions[16], timers[16];
    f32 speeds[16];
    s16 animations[16], animationFrames[16];
    for (int rolling = 0; rolling < 2; rolling++) {
        begin(FALSE, 0, FALSE);
        sController.buttonPressed = rolling ? L_TRIG : 0;
        for (int frame = 0; frame < 16; frame++) {
            update(INPUT_A_DOWN);
            sController.buttonPressed = 0;
            if (!rolling) {
                actions[frame] = sMario.action;
                timers[frame] = sMario.actionTimer;
                speeds[frame] = sMario.forwardVel;
                animations[frame] = sObject.header.gfx.animInfo.animID;
                animationFrames[frame] = sObject.header.gfx.animInfo.animFrame;
            } else {
                EXPECT_EQ(sMario.action, actions[frame]);
                EXPECT_EQ(sMario.actionTimer, timers[frame]);
                EXPECT_EQ(sMario.forwardVel, speeds[frame]);
                EXPECT_EQ(sObject.header.gfx.animInfo.animID, animations[frame]);
                EXPECT_EQ(sObject.header.gfx.animInfo.animFrame, animationFrames[frame]);
                EXPECT_EQ(sObject.header.gfx.angle[2], (s16) (0x10000 * (frame + 1) / 16));
            }
        }
    }
    return TRUE;
}

int main(int argc, char **argv) {
    test_init(argc, argv);
    if (test_selected("swim_animation.roll_pivot")) {
        test_report("swim_animation.roll_pivot", swimming_roll_pivot());
    }
    if (test_selected("swim_animation.aileron_roll")) {
        test_report("swim_animation.aileron_roll", swimming_aileron_roll());
    }
    if (test_selected("swim_animation.water_pound_cooldown")) {
        test_report("swim_animation.water_pound_cooldown", water_pound_cooldown());
    }
    if (test_selected("swim_animation.initial_tap")) {
        test_report("swim_animation.initial_tap", initial_tap_animation());
    }
    if (test_selected("swim_animation.hold_and_interruptions")) {
        test_report("swim_animation.hold_and_interruptions", long_hold_and_interruptions());
    }
    return test_finish();
}
