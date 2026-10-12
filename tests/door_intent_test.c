/* Production door eligibility and dispatch; contact and save state are supplied. */
#include <math.h>
#include <stdint.h>
#include <string.h>
#include "test_support.h"

typedef int16_t s16;
typedef uint16_t u16;
typedef int32_t s32;
typedef uint32_t u32;
typedef uint8_t u8;
typedef float f32;
#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define TRUE 1
#define FALSE 0
#define UNUSED
#define abss(value) ((value) < 0 ? -(value) : (value))
#define sqr(value) ((value) * (value))
#define game_options_cheat_active(id) FALSE
#define SOUND_MOVING_TERRAIN_SLIDE 5
// Save-range arguments are supplied service details, not door eligibility.
#define COURSE_MIN 1
#define COURSE_MAX 25
#define SOUND_GENERAL_OPEN_WOOD_DOOR 1
#define SOUND_GENERAL_OPEN_IRON_DOOR 2
#define SOUND_GENERAL_CLOSE_WOOD_DOOR 3
#define SOUND_GENERAL_CLOSE_IRON_DOOR 4
#include "doors_definitions.inc"

typedef f32 Vec3f[3];
struct Animation {
    s16 loopEnd;
};
struct AnimInfo {
    s16 animFrame;
    s32 animFrameAccelAssist, animAccel;
    struct Animation *curAnim;
};
struct Object {
    f32 oPosX, oPosY, oPosZ;
    s16 oMoveAngleYaw;
    u32 oBehParams, oInteractionSubtype;
    u32 oInteractStatus, oInteractType;
    s32 oAction, oTimer;
    const void *behavior;
    struct {
        struct {
            struct AnimInfo animInfo;
            Vec3f cameraToObject;
            s16 angle[3];
        } gfx;
    } header;
};
struct Controller {
    f32 stickMag;
};
struct Surface {
    struct Object *object;
};
struct MarioState {
    u32 action, actionArg;
    f32 pos[3], intendedMag;
    s16 intendedYaw, faceAngle[3];
    struct Controller *controller;
    struct Object *interactObj, *usedObj, *marioObj;
    u16 actionTimer, actionState;
    struct Surface *wall;
    s16 wallYaw, terrainSoundAddend;
    f32 forwardVel;
    u32 flags, particleFlags;
};
static const int bhvDoor[1], bhvDoorWarp[1], bhvStarDoor[1];
static struct MarioState *gMarioState;
static struct Object *o;
static struct Animation sDoorAnimation = { 80 };
static struct Animation sMarioDoorAnimation;
static struct Object sMarioObject;
static s32 sAnim, sAnimAccel, sFrame, sFrameEnd, sWarpCalls, sWarpTimer, sSoundCount;
static s32 sSounds[4], sSoundTimers[4], gTimeStopState;
static f32 sDoorSpeed;
static s32 sMetalDoor;
static void *segmented_to_virtual(const void *p) {
    return (void *) p;
}
static s16 set_mario_anim_with_accel(struct MarioState *m, s32 animation, s32 accel) {
    sAnim = animation;
    sAnimAccel = accel;
    sMarioDoorAnimation.loopEnd = sFrameEnd;
    m->marioObj->header.gfx.animInfo.curAnim = &sMarioDoorAnimation;
    m->marioObj->header.gfx.animInfo.animAccel = accel;
    return sFrame;
}
static void update_mario_pos_for_anim(struct MarioState *m) {
    m->pos[2] += sFrame; // Supplied root motion: observe every accelerated pose.
}
static void stop_and_set_height_to_floor(struct MarioState *m) {
}
static s32 level_trigger_warp(struct MarioState *m, s32 op) {
    sWarpCalls++;
    sWarpTimer = m->actionTimer;
    return TRUE;
}
static void cur_obj_init_animation_with_accel_and_sound(s32 anim, f32 speed) {
    sAnim = anim;
    sDoorSpeed = speed;
    o->header.gfx.animInfo.curAnim = &sDoorAnimation;
}
static s32 cur_obj_has_model(s32 model) {
    return sMetalDoor;
}
static void cur_obj_play_sound_2(s32 sound) {
    if (sSoundCount < 4) {
        sSounds[sSoundCount] = sound;
        sSoundTimers[sSoundCount] = o->oTimer;
    }
    sSoundCount++;
}
struct Camera {
    s32 cutscene;
    Vec3f pos, focus;
};
static struct {
    Vec3f point;
} sCutsceneVars[2];
static s16 gCutsceneTimer;
static f32 sCameraStep;
static void vec3f_copy(Vec3f to, const Vec3f from) {
    memcpy(to, from, sizeof(Vec3f));
}
static void set_focus_rel_mario(struct Camera *c, f32 x, f32 y, f32 z, s16 yaw) {
}
static void vec3f_get_dist_and_angle(Vec3f from, Vec3f to, f32 *dist, s16 *pitch, s16 *yaw) {
    *dist = 250;
    *pitch = *yaw = 0;
}
static s32 camera_approach_f32_symmetric_bool(f32 *value, f32 target, f32 step) {
    sCameraStep = step;
    return TRUE;
}
static void vec3f_set_dist_and_angle(Vec3f from, Vec3f to, f32 dist, s16 pitch, s16 yaw) {
}
static void update_camera_yaw(struct Camera *c) {
}
static u8 sDisplayingDoorText;
static s32 gCurrSaveFileNum = 1;
static u32 sSaveFlags;
static s16 sStars;
static s32 sActionCalls;
static u32 save_file_get_flags(void) {
    return sSaveFlags;
}
static s16 save_file_get_total_star_count(s32 file, s32 first, s32 last) {
    return sStars;
}
static u32 set_mario_action(struct MarioState *m, u32 action, u32 arg) {
    m->action = action;
    m->actionArg = arg;
    sActionCalls++;
    return TRUE;
}
static s32 sWalkingCalls;
static void anim_and_audio_for_walk(struct MarioState *m) {
    sWalkingCalls++;
}
static void mario_set_forward_vel(struct MarioState *m, f32 speed) {
    m->forwardVel = speed;
}
static void set_mario_animation(struct MarioState *m, s32 animation) {
    sAnim = animation;
}
static void play_step_sound(struct MarioState *m, s32 first, s32 second) {
}
static void play_sound(s32 sound, Vec3f position) {
}
static s16 find_floor_slope(struct MarioState *m, s16 yaw) {
    return 0;
}
#include "doors_sources.inc"

static struct MarioState m;
static struct Object door;
static struct Controller controller;
static void begin(s16 bearing, u32 action) {
    memset(&m, 0, sizeof(m));
    memset(&door, 0, sizeof(door));
    controller.stickMag = 64.0f;
    m.controller = &controller;
    m.action = action;
    m.intendedYaw = m.faceAngle[1] = bearing;
    // Cardinal door positions also exercise the signed-angle wrap at 180 degrees.
    if (bearing == 0) {
        door.oPosZ = 100;
    }
    if (bearing == 0x4000) {
        door.oPosX = 100;
    }
    if (bearing == (s16) 0x8000) {
        door.oPosZ = -100;
    }
    if (bearing == (s16) 0xC000) {
        door.oPosX = -100;
    }
    sStars = 70;
    sSaveFlags = 0;
    sDisplayingDoorText = FALSE;
    sActionCalls = 0;
}

static int control_release(void) {
    for (int pulled = 0; pulled < 2; pulled++) {
        for (s32 phase = 0; phase < 0x10000; phase += 0x1000) {
            begin(0, pulled ? ACT_PULLING_DOOR : ACT_PUSHING_DOOR);
            m.interactObj = m.usedObj = &door;
            m.marioObj = &sMarioObject;
            m.actionArg = (pulled ? WARP_FLAG_DOOR_PULLED : 0) | WARP_FLAG_DOOR_FLIP_MARIO;
            sFrameEnd = pulled ? 60 : 50;
            s32 oldEnd = ((sFrameEnd - 1) * 0x10000 - phase + 78642) / 78643;
            for (s32 tick = 0; tick <= oldEnd; tick++) {
                sFrame = (phase + tick * STANDARD_DOOR_ANIM_ACCEL) / 0x10000;
                sMarioObject.header.gfx.animInfo.animFrameAccelAssist =
                    phase + tick * STANDARD_DOOR_ANIM_ACCEL;
                act_going_through_door(&m);
                if (tick < oldEnd - 5) {
                    EXPECT_EQ(m.action, pulled ? ACT_PULLING_DOOR : ACT_PUSHING_DOOR);
                } else {
                    EXPECT_EQ(tick, oldEnd - 5);
                    EXPECT_EQ(m.action, ACT_IDLE);
                    EXPECT_EQ(m.faceAngle[1], (s16) 0x8000);
                    break;
                }
            }
        }
    }
    return TRUE;
}

static int opening_speed(void) {
    for (s32 cycle = 0; cycle < 3; cycle++) {
        for (s32 pulled = 0; pulled < 2; pulled++) {
            for (s32 warp = 0; warp < 2; warp++) {
                begin(0, pulled ? ACT_PULLING_DOOR : ACT_PUSHING_DOOR);
                m.interactObj = m.usedObj = &door;
                m.actionArg = (pulled ? WARP_FLAG_DOOR_PULLED : 0)
                              | (warp ? WARP_FLAG_DOOR_IS_WARP : WARP_FLAG_DOOR_FLIP_MARIO);
                sAnimAccel = sWarpCalls = sActionCalls = 0;
                sFrameEnd = pulled ? 60 : 50;
                for (s32 tick = 0; tick < 80; tick++) {
                    sFrame = tick * STANDARD_DOOR_ANIM_ACCEL / 0x10000;
                    if (sFrame >= sFrameEnd) {
                        sFrame = sFrameEnd - 1;
                    }
                    m.marioObj = &sMarioObject;
                    sMarioObject.header.gfx.animInfo.animFrameAccelAssist =
                        tick * STANDARD_DOOR_ANIM_ACCEL;
                    act_going_through_door(&m);
                    if (!warp && m.action == ACT_IDLE) {
                        break;
                    }
                }
                EXPECT_EQ(sAnim, pulled ? MARIO_ANIM_PULL_DOOR_WALK_IN : MARIO_ANIM_PUSH_DOOR_WALK_IN);
                EXPECT_EQ(sAnimAccel, 78643);
                EXPECT_EQ(door.oInteractStatus,
                          pulled ? INT_STATUS_DOOR_PULLED : INT_STATUS_DOOR_PUSHED);
                EXPECT_EQ(sWarpCalls, warp);
                if (warp) {
                    EXPECT_EQ(sWarpTimer, 14);
                } else {
                    EXPECT_EQ(m.action, ACT_IDLE);
                    EXPECT_EQ(m.actionTimer, pulled ? 46 : 37);
                    EXPECT_EQ(m.pos[2], door.oPosZ + (pulled ? 53 : 43));
                    EXPECT_EQ(m.faceAngle[1], (s16) 0x8000);
                }
            }
        }
        o = &door;
        for (s32 metal = 0; metal < 2; metal++) {
            for (s32 warp = 0; warp < 2; warp++) {
                sMetalDoor = metal;
                sSoundCount = gTimeStopState = 0;
                door.oAction = DOOR_ACT_PUSHED;
                for (door.oTimer = 0; door.oTimer < 80; door.oTimer++) {
                    door.header.gfx.animInfo.animFrame =
                        (warp ? 40 : 0) + door.oTimer * STANDARD_DOOR_ANIM_ACCEL / 0x10000;
                    door_animation_and_reset(warp ? DOOR_ANIM_WARP_PUSHED : DOOR_ANIM_PUSHED);
                    if (warp) {
                        play_warp_door_open_noise();
                    } else {
                        play_door_open_noise();
                    }
                    if (door.oAction == DOOR_ACT_CLOSED) {
                        break;
                    }
                }
                EXPECT_EQ(fabsf(sDoorSpeed - 1.2f) < 0.00001f, TRUE);
                EXPECT_EQ(door.oTimer, warp ? 32 : 66);
                EXPECT_EQ(sSoundCount, warp ? 1 : 2);
                EXPECT_EQ(sSounds[sSoundCount - 1],
                          metal ? SOUND_GENERAL_CLOSE_IRON_DOOR : SOUND_GENERAL_CLOSE_WOOD_DOOR);
                EXPECT_EQ(sSoundTimers[sSoundCount - 1], warp ? 25 : 59);
                EXPECT_EQ(gTimeStopState, warp ? 0 : TIME_STOP_MARIO_OPENED_DOOR);
            }
        }
        gMarioState = &m;
        m.usedObj = &door;
        struct Camera camera = { 0 };
        const void *behaviors[] = { bhvDoor, bhvDoorWarp, bhvStarDoor, NULL };
        for (u32 i = 0; i < sizeof(behaviors) / sizeof(*behaviors); i++) {
            door.behavior = behaviors[i];
            for (s32 duration = 20; duration <= 50; duration += 10) {
                gCutsceneTimer = 0;
                s32 updates = 0;
                while (gCutsceneTimer < duration) {
                    camera.cutscene = duration == 30 ? CUTSCENE_DOOR_PULL : CUTSCENE_DOOR_PUSH;
                    if (duration <= 30) {
                        cutscene_door_fix_cam(&camera);
                    } else {
                        cutscene_advance_standard_door_clock(cutscene_standard_door_speed(), duration);
                    }
                    gCutsceneTimer++;
                    updates++;
                }
                EXPECT_EQ(gCutsceneTimer, duration);
                EXPECT_EQ(updates, i < 2 ? (duration * 10 + 11) / 12 : duration);
            }
            gCutsceneTimer = 0;
            cutscene_door_follow_mario(&camera);
            EXPECT_EQ(gCutsceneTimer, 0);
            EXPECT_EQ(fabsf(sCameraStep - (i < 2 ? 8.4f : 7.0f)) < 0.00001f, TRUE);
        }
    }
    return TRUE;
}
static u32 interact(s32 warp) {
    return warp ? interact_warp_door(&m, INTERACT_WARP_DOOR, &door)
                : interact_door(&m, INTERACT_DOOR, &door);
}

static int boundaries(void) {
    const s16 bearings[] = { 0, 0x4000, (s16) 0x8000, (s16) 0xC000 };
    const s16 offsets[] = { 0, 0x2000 - 1, 0x2000, 0x2000 + 1, -0x2000, -0x2001, (s16) 0x8000 };
    const f32 strengths[] = { 0, 31.36f, 31.99f, 32, 64 };
    for (int cycle = 0; cycle < 3; cycle++) {
        for (int warp = 0; warp < 2; warp++) {
            for (int coast = 0; coast < 2; coast++) {
                for (unsigned side = 0; side < sizeof(bearings) / sizeof(bearings[0]); side++) {
                    for (unsigned strength = 0; strength < sizeof(strengths) / sizeof(strengths[0]);
                         strength++) {
                        for (unsigned aim = 0; aim < sizeof(offsets) / sizeof(offsets[0]); aim++) {
                            for (unsigned face = 0; face < sizeof(offsets) / sizeof(offsets[0]);
                                 face++) {
                                begin(bearings[side], coast ? ACT_DECELERATING : ACT_WALKING);
                                controller.stickMag = strengths[strength];
                                m.intendedMag = 0; // Raw stick, not action-scaled magnitude.
                                m.intendedYaw = bearings[side] + offsets[aim];
                                m.faceAngle[1] = bearings[side] + offsets[face];
                                // Both signs of the exact 45-degree boundary qualify.
                                s32 expected = strengths[strength] >= 32 && (aim <= 2 || aim == 4)
                                               && (face <= 2 || face == 4);
                                EXPECT_EQ(interact(warp), expected);
                                EXPECT_EQ(sActionCalls, expected);
                                EXPECT_EQ(m.usedObj == &door, expected);
                                EXPECT_EQ(sDisplayingDoorText, FALSE);
                                if (expected) {
                                    u32 sideArg = should_push_or_pull_door(&m, &door);
                                    EXPECT_EQ(m.actionArg,
                                              sideArg | (warp ? WARP_FLAG_DOOR_IS_WARP : 0));
                                    EXPECT_EQ(m.action, sideArg & WARP_FLAG_DOOR_PULLED
                                                            ? ACT_PULLING_DOOR
                                                            : ACT_PUSHING_DOOR);
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    return TRUE;
}

static int wall_contacts(void) {
    Vec3f start = { 0 };
    struct Surface wall = { &door };
    const s16 headings[] = { 0, 0x4000, (s16) 0x8000, (s16) 0xC000 };
    for (int kind = 0; kind < 3; kind++) {
        s32 warp = kind != 0;
        u32 key = kind == 2 ? SAVE_FLAG_HAVE_KEY_1 : SAVE_FLAG_HAVE_KEY_2;
        u32 unlocked = kind == 2 ? SAVE_FLAG_UNLOCKED_BASEMENT_DOOR : SAVE_FLAG_UNLOCKED_UPSTAIRS_DOOR;
        for (unsigned side = 0; side < sizeof(headings) / sizeof(headings[0]); side++) {
            for (int locked = 0; locked < 3; locked++) {
                begin(headings[side], ACT_WALKING);
                m.marioObj = &sMarioObject;
                door.oInteractType = warp ? INTERACT_WARP_DOOR : INTERACT_DOOR;
                door.oInteractionSubtype = warp ? 0 : INT_SUBTYPE_STAR_DOOR;
                door.oBehParams = (warp ? (u32) kind : 70u) << 24;
                sStars = locked ? 0 : 70;
                sSaveFlags = locked == 2 ? key : unlocked;
                if (locked == 1) {
                    sSaveFlags = 0;
                }
                m.wall = &wall;
                m.wallYaw = headings[side];
                m.intendedYaw = headings[side] + 0x8000 + (locked - 1) * DEGREES(60);
                m.faceAngle[1] = headings[side] + (locked == 1 ? 0x8000 : 0);
                m.intendedMag = 8;
                controller.stickMag = 16;
                m.forwardVel = 20;
                sWalkingCalls = 0;
                push_or_sidle_wall(&m, start);
                u32 passage = should_push_or_pull_door(&m, &door) & WARP_FLAG_DOOR_PULLED
                                  ? ACT_PULLING_DOOR
                                  : ACT_PUSHING_DOOR;
                u32 expected = !warp ? (locked ? ACT_READING_AUTOMATIC_DIALOG : ACT_ENTERING_STAR_DOOR)
                                     : (locked == 1   ? ACT_READING_AUTOMATIC_DIALOG
                                        : locked == 2 ? ACT_UNLOCKING_KEY_DOOR
                                                      : passage);
                EXPECT_EQ(m.action, expected);
                EXPECT_EQ(sActionCalls, 1);
                EXPECT_EQ(m.forwardVel, 20);
                EXPECT_EQ(sWalkingCalls, 0);
                EXPECT_EQ(m.flags & MARIO_PUSHING, 0);
                EXPECT_EQ(m.actionState, 0);
            }
            begin(headings[side], ACT_WALKING);
            m.marioObj = &sMarioObject;
            door.oInteractType = warp ? INTERACT_WARP_DOOR : INTERACT_DOOR;
            m.wall = &wall;
            m.wallYaw = headings[side];
            m.intendedYaw = headings[side] + 0x8000 + DEGREES(60) + 1;
            m.intendedMag = 8;
            m.forwardVel = 20;
            push_or_sidle_wall(&m, start);
            EXPECT_EQ(m.action, ACT_WALKING);
            EXPECT_EQ(sActionCalls, 0);
            EXPECT_EQ(m.forwardVel, 20);
        }
    }
    // Both scenery walls and other moving objects retain intentional sidling.
    for (int dynamic = 0; dynamic < 2; dynamic++) {
        begin(0, ACT_WALKING);
        m.marioObj = &sMarioObject;
        wall.object = dynamic ? &door : NULL;
        door.oInteractType = INTERACT_DAMAGE;
        m.wall = &wall;
        m.intendedYaw = (s16) 0x8000;
        m.intendedMag = 8;
        m.forwardVel = 20;
        push_or_sidle_wall(&m, start);
        EXPECT_EQ(m.action, ACT_WALKING);
        EXPECT_EQ(m.forwardVel, 6);
        EXPECT_EQ(m.actionState, ACT_STATE_PUSH_OR_SIDLE_WALL_SIDLING);
    }
    return TRUE;
}

static int locks_and_reapproach(void) {
    const u32 requirements[] = { 1, 3, 8, 30, 50, 70 };
    for (int cycle = 0; cycle < 3; cycle++) {
        for (unsigned kind = 0; kind < sizeof(requirements) / sizeof(requirements[0]); kind++) {
            begin(0, ACT_WALKING);
            door.oBehParams = requirements[kind] << 24;
            sStars = 0;
            controller.stickMag = 0;
            EXPECT_EQ(interact(FALSE), FALSE);
            EXPECT_EQ(sActionCalls, 0);
            EXPECT_EQ(sDisplayingDoorText, FALSE);
            controller.stickMag = 32;
            EXPECT_EQ(interact(FALSE), TRUE);
            EXPECT_EQ(m.action, ACT_READING_AUTOMATIC_DIALOG);
            EXPECT_EQ(sDisplayingDoorText, TRUE);
            // Holding contact never repeatedly starts the same message.
            m.action = ACT_WALKING;
            EXPECT_EQ(interact(FALSE), FALSE);
            EXPECT_EQ(sActionCalls, 1);
        }
        for (u32 key = 1; key <= 2; key++) {
            begin((s16) 0x8000, ACT_DECELERATING);
            door.oBehParams = key << 24;
            m.intendedYaw = 0;
            EXPECT_EQ(interact(TRUE), FALSE);
            EXPECT_EQ(sActionCalls, 0);
            EXPECT_EQ(sDisplayingDoorText, FALSE);
            m.intendedYaw = (s16) 0x8000;
            EXPECT_EQ(interact(TRUE), FALSE); // Locked-door text keeps its native return.
            EXPECT_EQ(sActionCalls, 1);
            EXPECT_EQ(m.action, ACT_READING_AUTOMATIC_DIALOG);
        }
        begin(0, ACT_WALKING);
        controller.stickMag = 0;
        EXPECT_EQ(interact(FALSE), FALSE);
        controller.stickMag = 64;
        EXPECT_EQ(interact(FALSE), TRUE);
        EXPECT_EQ(sActionCalls, 1);
    }
    return TRUE;
}

static int unlocks_and_continuation(void) {
    for (int cycle = 0; cycle < 3; cycle++) {
        begin(0, ACT_WALKING);
        door.oBehParams = 3u << 24;
        EXPECT_EQ(interact(FALSE), TRUE);
        EXPECT_EQ(m.action, ACT_UNLOCKING_STAR_DOOR);
        controller.stickMag = 0;
        EXPECT_EQ(interact(FALSE), FALSE);
        EXPECT_EQ(sActionCalls, 1);

        for (u32 key = 1; key <= 2; key++) {
            begin(0, ACT_WALKING);
            door.oBehParams = key << 24;
            sSaveFlags = key == 1 ? SAVE_FLAG_HAVE_KEY_2 : SAVE_FLAG_HAVE_KEY_1;
            EXPECT_EQ(interact(TRUE), TRUE);
            EXPECT_EQ(m.action, ACT_UNLOCKING_KEY_DOOR);
            EXPECT_EQ(m.actionArg & WARP_FLAG_DOOR_IS_WARP, WARP_FLAG_DOOR_IS_WARP);
            controller.stickMag = 0;
            EXPECT_EQ(interact(TRUE), FALSE);
            EXPECT_EQ(sActionCalls, 1);
        }
        begin(0, ACT_WALKING);
        door.oInteractionSubtype = INT_SUBTYPE_STAR_DOOR;
        door.oBehParams = 70u << 24;
        EXPECT_EQ(interact(FALSE), TRUE);
        EXPECT_EQ(m.action, ACT_ENTERING_STAR_DOOR);
        // The committed 70-star post-dialog continuation does not require re-aiming.
        begin(0, ACT_IDLE);
        door.oBehParams = 70u << 24;
        sDisplayingDoorText = TRUE;
        controller.stickMag = 0;
        m.faceAngle[1] = m.intendedYaw = (s16) 0x8000;
        EXPECT_EQ(interact(FALSE), TRUE);
        EXPECT_EQ(m.action, ACT_ENTERING_STAR_DOOR);
        EXPECT_EQ(m.usedObj, &door);
    }
    return TRUE;
}

int main(int argc, char **argv) {
    const struct {
        const char *name;
        int (*run)(void);
    } cases[] = {
        { "doors.boundaries", boundaries },
        { "doors.wall_contacts", wall_contacts },
        { "doors.locks_and_reapproach", locks_and_reapproach },
        { "doors.unlocks_and_continuation", unlocks_and_continuation },
        { "doors.opening_speed", opening_speed },
        { "doors.control_release", control_release },
    };
    test_init(argc, argv);
    for (unsigned i = 0; i < sizeof(cases) / sizeof(cases[0]); i++) {
        if (test_selected(cases[i].name)) {
            test_report(cases[i].name, cases[i].run());
        }
    }
    return test_finish();
}
