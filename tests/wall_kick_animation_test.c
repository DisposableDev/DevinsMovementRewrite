/* Production animation setters, wall-pose blending and renderer setup, with a
 * reused DMA buffer. Real wall-contact/kick assets; no rasterization or ROM. */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
typedef int16_t s16;
typedef uint16_t u16;
typedef int32_t s32;
typedef uint32_t u32;
typedef uint8_t u8;
typedef float f32;
typedef s16 Vec3s[3];
typedef f32 Vec3f[3];
#define TRUE 1
#define FALSE 0
#define BIT(n) (1U << (n))
#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define VIRTUAL_TO_PHYSICAL(p) (p)
#define GET_HIGH_S16_OF_32(v) ((s16) ((u32) (v) >> 16))
#define SET_HIGH_S16_OF_32(v, h) ((v) = (s32) (((u32) (v) & 0xffff) | ((u32) (u16) (h) << 16)))
#include "test_support.h"
#include "wallpose_definitions.inc"

struct Object {
    struct {
        struct {
            struct AnimInfo animInfo;
            Vec3s angle;
            Vec3f pos;
        } gfx;
    } header;
};
struct PatchableTable {
    struct Animation *bufTarget;
    s32 id;
};
struct Controller {
    f32 stickX, stickY;
};
struct MarioState {
    struct Object *marioObj;
    struct PatchableTable *animList;
    s16 animYTrans;
    u32 action, input;
    s32 actionState, actionTimer;
    Vec3s faceAngle, angleVel;
    Vec3f pos;
    struct Controller *controller;
};
static u16 gAreaUpdateCounter;
static s32 gCurrAnimType, gCurrAnimEnabled;
static s16 gCurrAnimFrame, *gCurrAnimData;
static u16 *gCurrAnimAttribute;
static f32 gCurrAnimTranslationMultiplier;
struct GraphNodeObject {
    struct AnimInfo animInfo;
};
static struct GraphNodeObject gMirrorMario;
static void *gCurGraphNodeHeldObject;
static struct Object *gMarioObject;
static struct Object object;
static struct {
    struct Animation anim;
    s16 values[63];
    u16 index[126];
} buffer;
static struct PatchableTable table;
static struct MarioState mario;
static struct MarioState *gMarioState;
static struct Animation incoming, other;
static s16 incomingValues[63], otherValues[63];
static u16 fixtureIndex[126];
static s32 retrieve_animation_index(s32 frame, u16 **attributes);
static s32 geo_update_animation_frame(struct AnimInfo *anim, s32 *accelAssist);
static void *segmented_to_virtual(const void *ptr) {
    return (void *) ptr;
}
static s32 load_patchable_table(struct PatchableTable *list, s32 id) {
    if (list->id == id) {
        return FALSE;
    }
    const struct Animation *asset;
    switch (id) {
        case MARIO_ANIM_START_GROUND_POUND:
            asset = anim_3C;
            break;
        case MARIO_ANIM_GROUND_POUND:
            asset = anim_3D;
            break;
        case MARIO_ANIM_TRIPLE_JUMP_GROUND_POUND:
            asset = anim_3B;
            break;
        case MARIO_ANIM_START_WALLKICK:
            asset = anim_CC;
            break;
        case MARIO_ANIM_SLIDEJUMP:
            asset = anim_CB;
            break;
        case MARIO_ANIM_SLIDE:
            asset = anim_91;
            break;
        case MARIO_ANIM_SLIDEFLIP:
            asset = anim_BF;
            break;
        case MARIO_ANIM_BACKFLIP:
            asset = anim_04;
            break;
        case MARIO_ANIM_TRIPLE_JUMP:
            asset = anim_C1;
            break;
        case MARIO_ANIM_BACKWARD_SPINNING:
            asset = anim_70;
            break;
        case MARIO_ANIM_DAB:
            asset = anim_D1;
            break;
        case MARIO_ANIM_HANDSTAND_IDLE:
            asset = anim_09;
            break;
        case MARIO_ANIM_START_HANDSTAND:
            asset = anim_0B;
            break;
        case MARIO_ANIM_HANDSTAND_JUMP:
            asset = anim_0A;
            break;
        case MARIO_ANIM_GENERAL_LAND:
            asset = anim_57;
            break;
        case MARIO_ANIM_DIVE:
            asset = anim_88;
            break;
        case MARIO_ANIM_GENERAL_FALL:
            asset = &incoming;
            break;
        default:
            asset = &other;
            break;
    }
    buffer.anim = *asset;
    if (asset == &incoming || asset == &other) {
        memcpy(buffer.values, asset->values, sizeof(buffer.values));
        memcpy(buffer.index, asset->index, sizeof(buffer.index));
        buffer.anim.values = (void *) ((u8 *) buffer.values - (u8 *) &buffer.anim);
        buffer.anim.index = (void *) ((u8 *) buffer.index - (u8 *) &buffer.anim);
    } else {
        // Reuse the animation header; assets' immutable channel data stays in
        // host memory. The production setter still performs its relocation.
        buffer.anim.values = (void *) ((uintptr_t) asset->values - (uintptr_t) &buffer.anim);
        buffer.anim.index = (void *) ((uintptr_t) asset->index - (uintptr_t) &buffer.anim);
    }
    list->id = id;
    return TRUE;
}
static s32 sAirResult;
static f32 sPoleBaseY, sLandingHeight;
static struct Controller controller;
static void vec3f_copy(f32 *dest, const f32 *source) {
    memcpy(dest, source, sizeof(Vec3f));
}
static s16 return_mario_anim_y_translation(struct MarioState *m) {
    struct AnimInfo *info = &m->marioObj->header.gfx.animInfo;
    u16 *index = (u16 *) info->curAnim->index + 2;
    s32 frame = MAX(0, geo_update_animation_frame(info, NULL));
    return info->curAnim->values[retrieve_animation_index(frame, &index)] / 4;
}
static s32 set_pole_position(struct MarioState *m, f32 offset) {
    m->pos[1] = sPoleBaseY + offset;
    vec3f_copy(m->marioObj->header.gfx.pos, m->pos);
    return POLE_NONE;
}
static s32 try_cancel_pole_handstand(struct MarioState *m) {
    return FALSE;
}
static s16 set_mario_animation(struct MarioState *m, s32 animation);
static void play_mario_jump_sound(struct MarioState *m) {
}
static s32 check_kick_or_dive_in_air(struct MarioState *m) {
    return FALSE;
}
static s32 set_mario_action(struct MarioState *m, u32 action, u32 arg) {
    m->action = action;
    m->actionState = m->actionTimer = 0;
    return TRUE;
}
static u32 common_air_action_step(struct MarioState *m, u32 landing, s32 animation, u32 flags) {
    if (sAirResult == AIR_STEP_LANDED) {
        m->pos[1] = sLandingHeight;
        set_mario_action(m, landing, 0);
    } else {
        set_mario_animation(m, animation);
    }
    vec3f_copy(m->marioObj->header.gfx.pos, m->pos);
    return sAirResult;
}
#include "wallpose_sources.inc"

static void reset(void) {
    mario_transition_pose_reset();
    memset(&object, 0, sizeof(object));
    memset(&buffer, 0, sizeof(buffer));
    for (s32 i = 0; i < 63; i++) {
        fixtureIndex[i * 2] = 1;
        fixtureIndex[i * 2 + 1] = i;
        incomingValues[i] = 1000 + i * 100;
        otherValues[i] = -1000 - i * 100;
    }
    incoming =
        (struct Animation){ ANIM_FLAG_NOLOOP, 100, 0, 0, 12, 20, incomingValues, fixtureIndex, 0 };
    other = incoming;
    other.values = otherValues;
    table = (struct PatchableTable){ &buffer.anim, -1 };
    mario = (struct MarioState){ .marioObj = &object, .animList = &table, .animYTrans = 200 };
    memset(&controller, 0, sizeof(controller));
    mario.controller = &controller;
    sPoleBaseY = sLandingHeight = 0;
    gMarioState = &mario;
    sAirResult = AIR_STEP_NONE;
    object.header.gfx.animInfo.animID = -1;
    gAreaUpdateCounter = 10;
    set_mario_animation(&mario, MARIO_ANIM_GENERAL_FALL);
    geo_set_animation_globals(&object.header.gfx.animInfo, TRUE);
}

static void draw(void) {
    geo_set_animation_globals(&object.header.gfx.animInfo, TRUE);
}

static f32 rendered_root_y(void) {
    if (gCurrAnimType != ANIM_TYPE_TRANSLATION && gCurrAnimType != ANIM_TYPE_VERTICAL_TRANSLATION) {
        return 0;
    }
    u16 *index = gCurrAnimAttribute + 2;
    return gCurrAnimData[retrieve_animation_index(gCurrAnimFrame, &index)]
           * gCurrAnimTranslationMultiplier / 4.0f;
}

static int entry(void) {
    reset();
    struct AnimInfo *anim = &object.header.gfx.animInfo;
    set_mario_animation(&mario, MARIO_ANIM_START_WALLKICK);
    draw();
    EXPECT_EQ(anim->curAnim, &buffer.anim);
    EXPECT_EQ(anim->animID, MARIO_ANIM_START_WALLKICK);
    EXPECT_EQ(anim->animFrame, -1); // Same-update draws retain the native clock.
    EXPECT_EQ(gCurrAnimData[3], incomingValues[3]);
    EXPECT_EQ(gCurrAnimData[1], incomingValues[1] * 2);
    EXPECT_EQ(gCurrAnimTranslationMultiplier, 1);
    s16 target[63];
    wall_kick_sample_pose(anim, target);
    gAreaUpdateCounter++;
    draw();
    s16 expected = incomingValues[3] + (s32) ((s16) (target[3] - incomingValues[3]) * (7.0f / 27));
    EXPECT_EQ(gCurrAnimData[3], expected);
    EXPECT_EQ(anim->animFrame, 0);
    gAreaUpdateCounter += 2;
    draw();
    EXPECT_EQ(sTransitionPose.owner, NULL);
    EXPECT_EQ(gCurrAnimData, anim_CC->values);
    EXPECT_EQ(gCurrAnimFrame, 0);
    return 1;
}

static int exits(void) {
    const s32 exits[] = { MARIO_ANIM_SLIDEJUMP, MARIO_ANIM_BACKWARD_AIR_KB, MARIO_ANIM_GENERAL_FALL };
    for (s32 cycle = 0; cycle < 6; cycle++) {
        reset();
        set_mario_animation(&mario, MARIO_ANIM_START_WALLKICK);
        draw();
        gAreaUpdateCounter += 3;
        draw();
        s16 contact[63];
        wall_kick_sample_pose(&object.header.gfx.animInfo, contact);
        set_mario_animation(&mario, exits[cycle % 3]);
        draw();
        for (s32 i = 0; i < 63; i++) {
            EXPECT_EQ(gCurrAnimData[i], contact[i]);
        }
        EXPECT_EQ(object.header.gfx.animInfo.animID, exits[cycle % 3]);
        gAreaUpdateCounter++;
        draw();
        EXPECT_EQ(sTransitionPose.owner, &object.header.gfx.animInfo);
        gAreaUpdateCounter += 2;
        draw();
        EXPECT_EQ(sTransitionPose.owner, NULL);
        EXPECT_EQ(gCurrAnimFrame, object.header.gfx.animInfo.animFrame);
    }
    return 1;
}

static int direct_kick(void) {
    reset();
    // A kick accepted on the contact update never selects the contact asset.
    mario_transition_pose_begin(&mario, MARIO_ANIM_START_WALLKICK);
    set_mario_animation(&mario, MARIO_ANIM_SLIDEJUMP);
    draw();
    for (s32 i = 3; i < 63; i++) {
        EXPECT_EQ(gCurrAnimData[i], incomingValues[i]);
    }
    gAreaUpdateCounter++;
    draw();
    EXPECT_EQ(object.header.gfx.animInfo.animFrame, 0);
    gAreaUpdateCounter++;
    draw();
    EXPECT_EQ(object.header.gfx.animInfo.animFrame, 1);
    gAreaUpdateCounter++;
    draw();
    EXPECT_EQ(sTransitionPose.owner, NULL);
    EXPECT_EQ(object.header.gfx.animInfo.animFrame, 2);
    return 1;
}

static int side_flip_entry(void) {
    const s32 frames[] = { 0, 6, 12, 18, 23 };
    for (s32 cycle = 0; cycle < 6; cycle++) {
        reset();
        set_mario_animation(&mario, MARIO_ANIM_SLIDEFLIP);
        struct AnimInfo *anim = &object.header.gfx.animInfo;
        anim->animFrame = frames[cycle % 5];
        s16 spinning[63], upright[63];
        wall_kick_sample_pose(anim, spinning);
        if (cycle == 5) {
            // A kick accepted on the contact update skips the slide pose.
            mario_transition_pose_begin(&mario, MARIO_ANIM_START_WALLKICK);
            set_mario_animation(&mario, MARIO_ANIM_SLIDEJUMP);
        } else {
            set_mario_animation(&mario, MARIO_ANIM_START_WALLKICK);
        }
        wall_kick_sample_pose(anim, upright);
        draw();
        EXPECT_EQ(anim->animID, cycle == 5 ? MARIO_ANIM_SLIDEJUMP : MARIO_ANIM_START_WALLKICK);
        for (s32 i = MARIO_BODY_ROTATION_START; i < MARIO_BODY_ROTATION_START + 3; i++) {
            EXPECT_EQ(gCurrAnimData[i], upright[i]);
        }
        EXPECT_EQ(gCurrAnimData[9], spinning[9]); // Other joints still blend.
        gAreaUpdateCounter++;
        draw();
        wall_kick_sample_pose(anim, upright);
        for (s32 i = MARIO_BODY_ROTATION_START; i < MARIO_BODY_ROTATION_START + 3; i++) {
            EXPECT_EQ(gCurrAnimData[i], upright[i]);
        }
        EXPECT_EQ(sTransitionPose.owner, anim);
        gAreaUpdateCounter += 2;
        draw();
        EXPECT_EQ(sTransitionPose.owner, NULL);
    }
    reset();
    set_mario_animation(&mario, MARIO_ANIM_SLIDEFLIP);
    object.header.gfx.animInfo.animFrame = 12;
    set_mario_animation(&mario, MARIO_ANIM_START_WALLKICK);
    draw();
    s16 contactBody[3];
    memcpy(contactBody, &gCurrAnimData[MARIO_BODY_ROTATION_START], sizeof(contactBody));
    set_mario_animation(&mario, MARIO_ANIM_GENERAL_FALL);
    draw();
    EXPECT_EQ(sTransitionPose.straightenFlipBody, FALSE);
    EXPECT_EQ(memcmp(contactBody, &gCurrAnimData[MARIO_BODY_ROTATION_START], sizeof(contactBody)), 0);
    return 1;
}

static int flip_body_entry(void) {
    const s32 animations[] = { MARIO_ANIM_BACKFLIP, MARIO_ANIM_TRIPLE_JUMP,
                               MARIO_ANIM_BACKWARD_SPINNING };
    for (s32 action = 0; action < 3; action++) {
        for (s32 immediate = 0; immediate < 2; immediate++) {
            reset();
            set_mario_animation(&mario, animations[action]);
            s32 length = object.header.gfx.animInfo.curAnim->loopEnd;
            for (s32 frame = 0; frame < length; frame++) {
                reset();
                set_mario_animation(&mario, animations[action]);
                struct AnimInfo *anim = &object.header.gfx.animInfo;
                anim->animFrame = frame;
                s16 spinning[63], upright[63];
                wall_kick_sample_pose(anim, spinning);
                if (immediate) {
                    mario_transition_pose_begin(&mario, MARIO_ANIM_START_WALLKICK);
                    set_mario_animation(&mario, MARIO_ANIM_SLIDEJUMP);
                } else {
                    set_mario_animation(&mario, MARIO_ANIM_START_WALLKICK);
                }
                wall_kick_sample_pose(anim, upright);
                draw();
                for (s32 i = 6; i < 9; i++) {
                    EXPECT_EQ(gCurrAnimData[i], upright[i]);
                }
                EXPECT_EQ(gCurrAnimData[9], spinning[9]);
                EXPECT_EQ(anim->animFrame, -1);
                gAreaUpdateCounter++;
                draw();
                wall_kick_sample_pose(anim, upright);
                for (s32 i = 6; i < 9; i++) {
                    EXPECT_EQ(gCurrAnimData[i], upright[i]);
                }
                // Interrupt from the visible upright contact, retaining its pose.
                s16 visible[63];
                memcpy(visible, gCurrAnimData, sizeof(visible));
                set_mario_animation(&mario, MARIO_ANIM_GENERAL_FALL);
                draw();
                EXPECT_EQ(memcmp(visible, gCurrAnimData, sizeof(visible)), 0);
                gAreaUpdateCounter += 3;
                draw();
                EXPECT_EQ(sTransitionPose.owner, NULL);
            }
        }
    }
    // Normal flip playback remains native away from a wall transition.
    reset();
    set_mario_animation(&mario, MARIO_ANIM_BACKFLIP);
    object.header.gfx.animInfo.animFrame = 20;
    draw();
    EXPECT_EQ(sTransitionPose.owner, NULL);
    EXPECT_EQ(gCurrAnimData, anim_04->values);
    return 1;
}

static int interruption(void) {
    reset();
    set_mario_animation(&mario, MARIO_ANIM_START_WALLKICK);
    draw();
    gAreaUpdateCounter++;
    draw();
    s16 visible[63];
    memcpy(visible, gCurrAnimData, sizeof(visible));
    // Two action/animation changes before either can render.
    set_mario_animation(&mario, MARIO_ANIM_SLIDEJUMP);
    set_mario_anim_with_accel(&mario, MARIO_ANIM_BACKWARD_AIR_KB, 0x18000);
    draw();
    for (s32 i = 0; i < 63; i++) {
        EXPECT_EQ(gCurrAnimData[i], visible[i]);
    }
    EXPECT_EQ(object.header.gfx.animInfo.animAccel, 0x18000);
    gAreaUpdateCounter++;
    draw();
    EXPECT_EQ(object.header.gfx.animInfo.animFrame, 0);
    gAreaUpdateCounter++;
    draw();
    gAreaUpdateCounter++;
    draw();
    EXPECT_EQ(object.header.gfx.animInfo.animFrame, 3);
    EXPECT_EQ(sTransitionPose.owner, NULL);
    return 1;
}

static int facing(void) {
    reset();
    object.header.gfx.angle[1] = 32000;
    set_mario_animation(&mario, MARIO_ANIM_START_WALLKICK);
    object.header.gfx.angle[1] = -32000;
    mario_transition_pose_update_gfx(&mario);
    EXPECT_EQ(object.header.gfx.angle[1], 32000);
    draw();
    gAreaUpdateCounter++;
    object.header.gfx.angle[1] = -32000;
    mario_transition_pose_update_gfx(&mario);
    EXPECT_EQ(object.header.gfx.angle[1], 32398);
    draw();
    // A same-update cancelled kick keeps the turn that was actually visible.
    set_mario_animation(&mario, MARIO_ANIM_SLIDEJUMP);
    object.header.gfx.angle[1] = 1000;
    set_mario_animation(&mario, MARIO_ANIM_BACKWARD_AIR_KB);
    mario_transition_pose_update_gfx(&mario);
    EXPECT_EQ(object.header.gfx.angle[1], 32398);
    gAreaUpdateCounter += 3;
    object.header.gfx.angle[1] = 1000;
    mario_transition_pose_update_gfx(&mario);
    EXPECT_EQ(object.header.gfx.angle[1], 1000);
    return 1;
}

static int late_kick_turn(void) {
    const s16 headings[] = { 0, 0x5000, 32000 };
    for (unsigned cycle = 0; cycle < sizeof(headings) / sizeof(*headings); cycle++) {
        for (int wait = 0; wait < 8; wait++) {
            for (int seated = 0; seated < 2; seated++) {
                reset();
                if (cycle == 2) {
                    gAreaUpdateCounter = 65534;
                    draw();
                }
                struct AnimInfo *anim = &object.header.gfx.animInfo;
                s16 facing = headings[cycle];
                s16 launchFacing = (s16) (facing + 0x8000);
                object.header.gfx.angle[1] = facing;
                object.header.gfx.angle[0] = 4000;
                object.header.gfx.angle[2] = -4000;
                if (wait != 0) {
                    set_mario_animation(&mario, MARIO_ANIM_START_WALLKICK);
                    draw();
                    for (int tick = 0; tick < wait; tick++) {
                        gAreaUpdateCounter++;
                        draw();
                    }
                }
                // Match direct and delayed kick selection, including the
                // same-update replacement with HE THICC's seated animation.
                mario_transition_pose_begin(&mario, MARIO_ANIM_START_WALLKICK);
                set_mario_animation(&mario, MARIO_ANIM_SLIDEJUMP);
                if (seated) {
                    set_mario_animation(&mario, MARIO_ANIM_SLIDE);
                }
                EXPECT_EQ(anim->animFrame, -1);
                object.header.gfx.angle[0] = object.header.gfx.angle[2] = 0;
                object.header.gfx.angle[1] = launchFacing;
                mario_transition_pose_update_gfx(&mario);
                EXPECT_EQ(object.header.gfx.angle[1], facing);
                draw();
                gAreaUpdateCounter++;
                object.header.gfx.angle[0] = object.header.gfx.angle[2] = 0;
                object.header.gfx.angle[1] = launchFacing;
                mario_transition_pose_update_gfx(&mario);
                EXPECT_EQ(object.header.gfx.angle[1],
                          wait >= 3
                              ? launchFacing
                              : (s16) (facing + (s32) ((s16) (launchFacing - facing) * (7.0f / 27))));
                EXPECT_EQ(object.header.gfx.angle[0], 2963);
                EXPECT_EQ(object.header.gfx.angle[2], -2963);
                draw();
                EXPECT_EQ(anim->animFrame, 0);          // Waiting never ages the kick.
                EXPECT_EQ(sTransitionPose.owner, anim); // Limbs retain their full blend.
                s16 shown[63];
                memcpy(shown, gCurrAnimData, sizeof(shown));
                // Paused updates and repeated draws keep the same turn and pose.
                object.header.gfx.angle[0] = object.header.gfx.angle[2] = 0;
                object.header.gfx.angle[1] = launchFacing;
                mario_transition_pose_update_gfx(&mario);
                draw();
                EXPECT_EQ(object.header.gfx.angle[1],
                          wait >= 3
                              ? launchFacing
                              : (s16) (facing + (s32) ((s16) (launchFacing - facing) * (7.0f / 27))));
                EXPECT_EQ(memcmp(shown, gCurrAnimData, sizeof(shown)), 0);
                gAreaUpdateCounter += 2;
                object.header.gfx.angle[0] = object.header.gfx.angle[2] = 0;
                object.header.gfx.angle[1] = launchFacing;
                mario_transition_pose_update_gfx(&mario);
                draw();
                EXPECT_EQ(object.header.gfx.angle[1], launchFacing);
                EXPECT_EQ(sTransitionPose.owner, NULL);
            }
        }
    }
    return 1;
}
static int pause_and_scope(void) {
    reset();
    set_mario_animation(&mario, MARIO_ANIM_START_WALLKICK);
    draw();
    gAreaUpdateCounter++;
    draw();
    s16 visible[63];
    memcpy(visible, gCurrAnimData, sizeof(visible));
    s16 frame = object.header.gfx.animInfo.animFrame;
    for (s32 i = 0; i < 100; i++) {
        draw();
        EXPECT_EQ(memcmp(visible, gCurrAnimData, sizeof(visible)), 0);
        EXPECT_EQ(object.header.gfx.animInfo.animFrame, frame);
    }
    struct AnimInfo nonMario = object.header.gfx.animInfo;
    geo_set_animation_globals(&nonMario, TRUE);
    EXPECT_EQ(gCurrAnimData, anim_CC->values);
    mario_transition_pose_reset();
    draw();
    EXPECT_EQ(gCurrAnimData, anim_CC->values);
    set_mario_animation(&mario, MARIO_ANIM_GENERAL_FALL);
    draw();
    gAreaUpdateCounter += 3;
    draw();
    set_mario_anim_with_accel(&mario, MARIO_ANIM_BACKWARD_AIR_KB, 0x20000);
    draw();
    EXPECT_EQ(sTransitionPose.owner, NULL);
    return 1;
}

static int translation_and_wrap(void) {
    const s16 flags[] = { 0, ANIM_FLAG_HOR_TRANS, ANIM_FLAG_VERT_TRANS, ANIM_FLAG_NO_TRANS };
    for (s32 f = 0; f < 4; f++) {
        reset();
        object.header.gfx.animInfo.curAnim->flags |= flags[f];
        gAreaUpdateCounter = 65535;
        incomingValues[3] = 32000;
        buffer.values[3] = 32000;
        set_mario_animation(&mario, MARIO_ANIM_START_WALLKICK);
        draw();
        EXPECT_EQ(gCurrAnimData[0], (f == 1 || f == 3) ? 0 : 2000);
        EXPECT_EQ(gCurrAnimData[1], (f == 2 || f == 3) ? 0 : 2200);
        EXPECT_EQ(gCurrAnimData[2], (f == 1 || f == 3) ? 0 : 2400);
        gAreaUpdateCounter = 0;
        draw();
        EXPECT_EQ(sTransitionPose.owner, &object.header.gfx.animInfo);
        EXPECT_EQ(gCurrAnimData[3], 23704);
        gAreaUpdateCounter = 2;
        draw();
        EXPECT_EQ(sTransitionPose.owner, NULL);
    }
    return 1;
}

static int inverse_dab_frame_seek(void) {
    reset();
    set_mario_animation(&mario, MARIO_ANIM_DAB);
    struct AnimInfo *anim = &object.header.gfx.animInfo;
    for (int cycle = 0; cycle < 3; cycle++) {
        for (int sample = 0; sample < DAB_ANALOG_FRAME_COUNT; sample++) {
            int frame = DAB_ANALOG_START_FRAME + sample;
            gAreaUpdateCounter++;
            set_anim_to_frame(&mario, frame);
            draw();
            EXPECT_EQ(gCurrAnimFrame, frame);
            EXPECT_EQ(gCurrAnimData, anim_D1_values);
            draw(); // Repeated/mirror draw on the same update cannot advance.
            EXPECT_EQ(gCurrAnimFrame, frame);
            EXPECT_EQ(anim->animFrame, frame);
        }
    }
    for (int tick = 0; tick < 10; tick++) {
        gAreaUpdateCounter++;
        set_anim_to_frame(&mario, DAB_ANALOG_START_FRAME + 31);
        draw();
        EXPECT_EQ(gCurrAnimFrame, DAB_ANALOG_START_FRAME + 31);
    }
    gAreaUpdateCounter++;
    set_anim_to_frame(&mario, 20);
    draw();
    EXPECT_EQ(gCurrAnimFrame, 20);
    return 1;
}

static int handstand_landing_clock(void) {
    for (int cycle = 0; cycle < 3; cycle++) {
        reset();
        set_mario_animation(&mario, MARIO_ANIM_HANDSTAND_JUMP);
        draw();
        mario.action = ACT_TOP_OF_POLE_JUMP;
        sAirResult = AIR_STEP_LANDED;
        gAreaUpdateCounter++;
        EXPECT_EQ(act_top_of_pole_jump(&mario), FALSE);
        draw();
        struct AnimInfo *anim = &object.header.gfx.animInfo;
        EXPECT_EQ(anim->animID, MARIO_ANIM_GENERAL_LAND);
        EXPECT_EQ(anim->animFrame, anim_57->startFrame);
        EXPECT_EQ(mario.action, ACT_FREEFALL_LAND);
        EXPECT_EQ(mario.actionTimer, 0);
        for (int frame = 1; frame < 4; frame++) {
            gAreaUpdateCounter++;
            set_mario_animation(&mario, MARIO_ANIM_GENERAL_LAND);
            draw();
            EXPECT_EQ(anim->animFrame, anim_57->startFrame + frame);
            draw();
            EXPECT_EQ(anim->animFrame, anim_57->startFrame + frame);
        }
    }
    return TRUE;
}

static int handstand_jump_edges(void) {
    const int takeoffAnimations[] = { MARIO_ANIM_HANDSTAND_IDLE, MARIO_ANIM_START_HANDSTAND };
    const int landingFrames[] = { 0, 1, 6, 15, 35 };
    const int scales[] = { 150, 189, 240 };
    for (int cycle = 0; cycle < 3; cycle++) {
        for (unsigned start = 0; start < sizeof(takeoffAnimations) / sizeof(*takeoffAnimations);
             start++) {
            for (unsigned scale = 0; scale < sizeof(scales) / sizeof(*scales); scale++) {
                for (unsigned land = 0; land < sizeof(landingFrames) / sizeof(*landingFrames); land++) {
                    reset();
                    mario.animYTrans = scales[scale];
                    mario.action = ACT_TOP_OF_POLE;
                    mario.faceAngle[1] = (s16) (cycle * 17000);
                    struct AnimInfo *anim = &object.header.gfx.animInfo;
                    set_mario_animation(&mario, takeoffAnimations[start]);
                    anim->animFrame = start ? 15 : cycle * 17;
                    anim->animTimer = gAreaUpdateCounter;
                    sPoleBaseY = 1000 + cycle * 30;
                    set_pole_position(&mario, return_mario_anim_y_translation(&mario));
                    draw();
                    EXPECT_EQ(rendered_root_y(), 0);
                    gAreaUpdateCounter++;
                    sPoleBaseY += 7; // Refresh the current moving attachment before detaching.
                    f32 attachedHeight = sPoleBaseY + return_mario_anim_y_translation(&mario);
                    mario.input = INPUT_A_DOWN;
                    EXPECT_EQ(act_top_of_pole(&mario), TRUE);
                    EXPECT_EQ(mario.action, ACT_TOP_OF_POLE_JUMP);
                    EXPECT_EQ(mario.faceAngle[1], (s16) (cycle * 17000 + 0x8000));
                    draw();
                    EXPECT_EQ(anim->animFrame, 0);
                    EXPECT_EQ(sTransitionPose.owner, NULL);
                    EXPECT_EQ(gCurrAnimData, anim_0A_values);
                    EXPECT_EQ(fabsf(object.header.gfx.pos[1] + rendered_root_y() - attachedHeight)
                                  < 0.002f,
                              TRUE);
                    EXPECT_EQ(mario.pos[1] < attachedHeight, TRUE);
                    f32 rootScale = scales[scale] / 189.0f;
                    for (int tick = 1; tick < anim_0A->loopEnd; tick++) {
                        gAreaUpdateCounter++;
                        set_mario_animation(&mario, MARIO_ANIM_HANDSTAND_JUMP);
                        draw();
                        EXPECT_EQ(anim->animFrame, tick);
                        EXPECT_EQ(sTransitionPose.owner, NULL);
                        // Keep the small authored root motion, without the fixed pole-height bias.
                        EXPECT_EQ(rendered_root_y() >= 137 * rootScale / 4.0f - 0.002f, TRUE);
                        EXPECT_EQ(rendered_root_y() <= 170 * rootScale / 4.0f + 0.002f, TRUE);
                        draw(); // Repeated/paused views cannot reapply the origin transfer.
                        EXPECT_EQ(anim->animFrame, tick);
                        EXPECT_EQ(
                            fabsf(mario.pos[1] + pole_jump_anim_y_translation(&mario) - attachedHeight)
                                < 0.002f,
                            TRUE);
                    }
                    // The settled airborne pose uses the same root height as ordinary landing.
                    EXPECT_EQ(fabsf(rendered_root_y() - 170 * rootScale / 4.0f) < 0.002f, TRUE);
                    anim->animFrame = landingFrames[land];
                    anim->animTimer = gAreaUpdateCounter;
                    draw();
                    f32 airborneRoot = rendered_root_y();
                    sLandingHeight = cycle * 25;
                    sAirResult = AIR_STEP_LANDED;
                    gAreaUpdateCounter++;
                    EXPECT_EQ(act_top_of_pole_jump(&mario), FALSE);
                    draw();
                    EXPECT_EQ(mario.action, ACT_FREEFALL_LAND);
                    EXPECT_EQ(mario.actionTimer, 0);
                    EXPECT_EQ(anim->animID, MARIO_ANIM_GENERAL_LAND);
                    EXPECT_EQ(anim->animFrame, anim_57->startFrame);
                    EXPECT_EQ(sTransitionPose.owner, NULL);
                    EXPECT_EQ(gCurrAnimData, anim_56_57_values);
                    EXPECT_EQ(mario.pos[1], sLandingHeight);
                    // Early contact changes only the small native anticipation offset.
                    EXPECT_EQ(fabsf(rendered_root_y() - airborneRoot) <= 33 * rootScale / 4.0f + 0.002f,
                              TRUE);
                    for (int tick = 1; tick <= 3; tick++) {
                        gAreaUpdateCounter++;
                        set_mario_animation(&mario, MARIO_ANIM_GENERAL_LAND);
                        draw();
                        EXPECT_EQ(anim->animFrame, anim_57->startFrame + tick);
                        EXPECT_EQ(sTransitionPose.owner, NULL);
                    }
                }
            }
        }
    }
    // Zero-divisor assets use an unscaled root; ordinary landing stays immediate.
    reset();
    gAreaUpdateCounter++;
    set_mario_animation(&mario, MARIO_ANIM_HANDSTAND_JUMP);
    object.header.gfx.animInfo.curAnim->animYTransDivisor = 0;
    draw();
    EXPECT_EQ(fabsf(rendered_root_y() - pole_jump_anim_y_translation(&mario)) < 0.002f, TRUE);
    gAreaUpdateCounter++;
    set_mario_animation(&mario, MARIO_ANIM_GENERAL_LAND);
    draw();
    EXPECT_EQ(sTransitionPose.owner, NULL);
    return TRUE;
}

static int handstand_dive_blend(void) {
    const int frames[] = { 0, 1, 6, 15, 35 };
    for (unsigned frame = 0; frame < sizeof(frames) / sizeof(frames[0]); frame++) {
        for (int heading = 0; heading < 3; heading++) {
            reset();
            struct AnimInfo *anim = &object.header.gfx.animInfo;
            set_mario_animation(&mario, MARIO_ANIM_HANDSTAND_JUMP);
            anim->animFrame = frames[frame];
            anim->animTimer = gAreaUpdateCounter;
            object.header.gfx.angle[1] = heading * 17000;
            draw();
            s16 from[63];
            wall_kick_sample_pose(anim, from);
            gAreaUpdateCounter++;
            set_mario_animation(&mario, MARIO_ANIM_DIVE);
            mario_transition_pose_update_gfx(&mario);
            draw();
            EXPECT_EQ(anim->animID, MARIO_ANIM_DIVE);
            EXPECT_EQ(anim->animFrame, 0);
            EXPECT_EQ(sTransitionPose.owner, anim);
            for (int channel = 0; channel < 63; channel++) {
                EXPECT_EQ(gCurrAnimData[channel], from[channel]);
            }
            for (int tick = 1; tick <= 3; tick++) {
                gAreaUpdateCounter++;
                set_mario_animation(&mario, MARIO_ANIM_DIVE);
                mario_transition_pose_update_gfx(&mario);
                draw();
                EXPECT_EQ(anim->animFrame, tick);
                EXPECT_EQ(object.header.gfx.angle[1], (s16) (heading * 17000));
                if (tick < 3) {
                    s16 target[63];
                    wall_kick_sample_pose(anim, target);
                    f32 amount = tick == 1 ? 7.0f / 27 : 20.0f / 27;
                    for (int channel = 0; channel < 63; channel++) {
                        s32 delta = channel < 3 ? target[channel] - from[channel]
                                                : (s16) (target[channel] - from[channel]);
                        EXPECT_EQ(gCurrAnimData[channel],
                                  (s16) (from[channel] + (s32) (delta * amount)));
                    }
                    draw();
                    EXPECT_EQ(anim->animFrame, tick);
                } else {
                    EXPECT_EQ(sTransitionPose.owner, NULL);
                    EXPECT_EQ(gCurrAnimData, anim_88_89_values);
                }
            }
        }
    }
    // Ordinary dives still select their native pose immediately.
    reset();
    gAreaUpdateCounter++;
    set_mario_animation(&mario, MARIO_ANIM_DIVE);
    draw();
    EXPECT_EQ(sTransitionPose.owner, NULL);
    EXPECT_EQ(gCurrAnimData, anim_88_89_values);
    return TRUE;
}

static int ground_pound_sampling(void) {
    const s32 ids[] = { MARIO_ANIM_START_GROUND_POUND, MARIO_ANIM_TRIPLE_JUMP_GROUND_POUND };
    for (s32 cycle = 0; cycle < 3; cycle++) {
        for (s32 a = 0; a < 2; a++) {
            reset();
            gMarioObject = &object;
            mario.action = ACT_GROUND_POUND;
            set_mario_animation(&mario, ids[a]);
            struct AnimInfo *info = &object.header.gfx.animInfo;
            s32 end = info->curAnim->loopEnd;
            s16 last = 0;
            for (s32 t = 1; t <= end + 4; t++) {
                mario.actionTimer = t;
                gAreaUpdateCounter++;
                draw();
                s32 clock = info->animFrameAccelAssist;
                s16 drawn = gCurrAnimData[8];
                if (t > end && t <= end + 3) {
                    EXPECT_EQ(drawn != last, TRUE);
                }
                last = drawn;
                draw(); // A repeated/paused render cannot advance or change the pose.
                EXPECT_EQ(info->animFrameAccelAssist, clock);
                EXPECT_EQ(gCurrAnimData[8], drawn);
                EXPECT_EQ(mario.actionTimer, t);
                EXPECT_EQ(gCurrAnimFrame, 0);
                gMirrorMario.animInfo = *info;
                geo_set_animation_globals(&gMirrorMario.animInfo, TRUE);
                EXPECT_EQ(gCurrAnimData[8], drawn);
                EXPECT_EQ(gMirrorMario.animInfo.animFrameAccelAssist, clock);
            }
            u16 *indices = (u16 *) info->curAnim->index;
            s16 *values = (s16 *) info->curAnim->values;
            EXPECT_EQ(last, values[indices[17] + end - 1]);
        }
        reset();
        gMarioObject = &object;
        mario.action = ACT_WATER_DOWN_BURST;
        set_mario_anim_with_accel(&mario, MARIO_ANIM_START_GROUND_POUND, 0xC000);
        struct AnimInfo *info = &object.header.gfx.animInfo;
        info->animFrameAccelAssist = 4 * 65536 + 32768;
        info->animFrame = 4;
        info->animTimer = gAreaUpdateCounter;
        draw();
        u16 *indices = (u16 *) info->curAnim->index;
        s16 *values = (s16 *) info->curAnim->values;
        s16 from = values[indices[17] + 4], to = values[indices[17] + 5];
        EXPECT_EQ(gCurrAnimData[8], (s16) (from + (s32) ((s16) (to - from) * 0.5f)));
        EXPECT_EQ(gCurrAnimData[8] != from, TRUE); // No whole-frame truncation.
        EXPECT_EQ(info->animFrame, 4);
        set_mario_anim_with_accel(&mario, MARIO_ANIM_GROUND_POUND, 0xC000);
        draw();
        EXPECT_EQ(gCurrAnimData, values);
        EXPECT_EQ(gCurrAnimFrame, info->animFrame);
        set_mario_animation(&mario, MARIO_ANIM_DIVE);
        draw();
        EXPECT_EQ(gCurrAnimData, anim_88_89_values);
        gMarioObject = NULL;
    }
    return TRUE;
}

int main(int argc, char **argv) {
    test_init(argc, argv);
    if (test_selected("wallpose.ground_pound_sampling")) {
        test_report("wallpose.ground_pound_sampling", ground_pound_sampling());
    }
    if (test_selected("wallpose.handstand_landing_clock")) {
        test_report("wallpose.handstand_landing_clock", handstand_landing_clock());
    }
    if (test_selected("wallpose.handstand_jump_edges")) {
        test_report("wallpose.handstand_jump_edges", handstand_jump_edges());
    }
    if (test_selected("wallpose.handstand_dive_blend")) {
        test_report("wallpose.handstand_dive_blend", handstand_dive_blend());
    }
    if (test_selected("wallpose.inverse_dab_frame_seek")) {
        test_report("wallpose.inverse_dab_frame_seek", inverse_dab_frame_seek());
    }
    if (test_selected("wallpose.entry")) {
        test_report("wallpose.entry", entry());
    }
    if (test_selected("wallpose.exits")) {
        test_report("wallpose.exits", exits());
    }
    if (test_selected("wallpose.direct_kick")) {
        test_report("wallpose.direct_kick", direct_kick());
    }
    if (test_selected("wallpose.side_flip_entry")) {
        test_report("wallpose.side_flip_entry", side_flip_entry());
    }
    if (test_selected("wallpose.flip_body_entry")) {
        test_report("wallpose.flip_body_entry", flip_body_entry());
    }
    if (test_selected("wallpose.interruption")) {
        test_report("wallpose.interruption", interruption());
    }
    if (test_selected("wallpose.facing")) {
        test_report("wallpose.facing", facing());
    }
    if (test_selected("wallpose.late_kick_turn")) {
        test_report("wallpose.late_kick_turn", late_kick_turn());
    }
    if (test_selected("wallpose.pause_scope")) {
        test_report("wallpose.pause_scope", pause_and_scope());
    }
    if (test_selected("wallpose.translation_wrap")) {
        test_report("wallpose.translation_wrap", translation_and_wrap());
    }
    return test_finish();
}
