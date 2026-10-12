/* Production HE THICC move/pose/spring code; collision, audio and action setup
 * are controlled services. No ROM playback, rasterization or controller feel. */
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
typedef s16 Vec3s[3];
#define TRUE 1
#define FALSE 0
#define BIT(n) (1U << (n))
#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define absf(x) fabsf(x)
#define COS45 0.70710678f
#define L_TRIG 0x20
#define SOUND_ACTION_CLAP_HANDS_COLD 1
#define SOUND_ACTION_TERRAIN_BODY_HIT_GROUND 2
#define SOUND_ACTION_TERRAIN_JUMP 3
#define SOUND_ACTION_SPIN 4
#define SOUND_GENERAL2_BOBOMB_EXPLOSION 5
#define SOUND_MARIO_OOOF2 6
#define SOUND_ACTION_SWIM 12
#define SOUND_ACTION_SWIM_FAST 13
#define INT_STATUS_MARIO_DROP_OBJECT 1
#define SOUND_ACTION_METAL_STEP_TIPTOE 7
#define SOUND_ACTION_METAL_STEP 8
#define SOUND_ACTION_QUICKSAND_STEP 9
#define SOUND_ACTION_TERRAIN_STEP_TIPTOE 10
#define SOUND_ACTION_TERRAIN_STEP 11
#define EXPECT_NEAR(actual, expected, tolerance)                                                       \
    EXPECT_EQ(fabsf((actual) - (expected)) <= (tolerance), TRUE)
#define EXPECT_TRUE(expression) EXPECT_EQ(!!(expression), TRUE)
#include "he_thicc_definitions.inc"

struct Object {
    struct {
        struct {
            struct AnimInfo animInfo;
            Vec3f scale, cameraToObject;
        } gfx;
    } header;
    f32 oPosX, oPosY, oPosZ;
    s32 oBehParams2ndByte, oInteractType, oIntangibleTimer;
    u32 oInteractStatus;
    struct Object *parentObj;
};
struct Surface {
    s32 type, flags;
    struct {
        f32 y;
    } normal;
};
struct Controller {
    u16 buttonDown;
    f32 stickMag;
};
struct Area {
    s32 terrainType;
};
static struct Area area;
struct MarioState;
void mario_set_wall_bonk_momentum(struct MarioState *m, f32 recoilSpeed, f32 minimumSpeed);
struct MarioState {
    u8 rolloutFrames;
    u8 wallBonkMomentum;
    u32 action, actionArg, particleFlags, flags;
    u16 input, actionTimer, actionState;
    u8 squishTimer;
    s16 health;
    struct Area *area;
    u8 rolloutWallKickFrames, zBPressed;
    f32 forwardVel, floorHeight, peakHeight, quicksandDepth, waterLevel, vel[3], pos[3], slideVelX,
        slideVelZ;
    s16 faceAngle[3], slideYaw, intendedYaw;
    struct Object *marioObj, *heldObj, *riddenObj;
    struct Surface *floor;
    struct Surface *wall;
    struct Controller *controller;
};
static void *gCurrDemoInput, *gCurrCreditsEntry;
static s32 sStats[PLAYER_STAT_COUNT];
static void player_stats_he_thicc_event(enum PlayerStatId id) {
    sStats[id]++;
}

static s32 sPalette, sSounds, sRumbles, sWaves, sStep, sGroundSteps, sStop;
static s32 sAnimation, sBonks, sLava, sFallChecks, sSpawnFails, sExplosions, sFootfall;
static struct Object explosion;
static const u32 bhvExplosion[] = { 1 };
static u32 sLastSound;
static struct Object object;
static struct Surface floorSurface;
static struct Controller controller;
static struct MarioState mario;
static struct MarioState *gMarioState;
struct GraphNodeObject {
    struct AnimInfo animInfo;
};
static struct GraphNodeObject *gCurGraphNodeObject, gMirrorMario;
static struct Object *gMarioObject;
static void *gCurGraphNodeHeldObject;
static u16 gAreaUpdateCounter;
static u8 gCurrAnimType, gCurrAnimEnabled;
static s16 gCurrAnimFrame, *gCurrAnimData;
static u16 *gCurrAnimAttribute;
static f32 gCurrAnimTranslationMultiplier;
static f32 sWater;
static u8 mario_character_palette(void) {
    return sPalette;
}
static s32 retrieve_animation_index(s32 frame, u16 **attributes);
s32 mario_he_thicc_belly_slide(struct MarioState *m);
static f32 sins(s16 angle) {
    return sinf((u16) angle * (6.28318530718f / 65536));
}
static f32 coss(s16 angle) {
    return cosf((u16) angle * (6.28318530718f / 65536));
}
static void vec3f_copy(Vec3f dst, const Vec3f src) {
    memcpy(dst, src, sizeof(Vec3f));
}
static void play_sound(u32 sound, Vec3f source) {
    sSounds++;
    sLastSound = sound;
}
static void queue_rumble_data(s32 time, s32 strength) {
    sRumbles++;
}
static void play_mario_heavy_landing_sound(struct MarioState *m, s32 sound) {
}
static void mario_ground_pound_impact(struct MarioState *m) {
    sWaves++;
}
static f32 find_water_level(f32 x, f32 z) {
    return sWater;
}
static s32 mario_is_on_floor(struct MarioState *m) {
    return m->floor != NULL && fabsf(m->pos[1] - m->floorHeight) <= 4;
}
static s32 set_mario_action(struct MarioState *m, u32 action, u32 arg) {
    m->action = action;
    m->actionArg = arg;
    m->actionTimer = 0;
    m->vel[1] = 20; // Action setup is allowed to overwrite launch velocities.
    return TRUE;
}
static s32 drop_and_set_mario_action(struct MarioState *m, u32 action, u32 arg) {
    return set_mario_action(m, action, arg);
}
static s32 landing_step(struct MarioState *m, s32 anim, u32 action) {
    return FALSE;
}
static void play_mario_sound(struct MarioState *m, s32 sound, s32 voice) {
}
static void update_air_without_turn(struct MarioState *m) {
}
static s32 mario_has_wall_kick_slide_input(struct MarioState *m) {
    return FALSE;
}
static void begin_wall_kick_window(struct MarioState *m) {
}
static s32 is_anim_at_end(struct MarioState *m) {
    return FALSE;
}
static s32 is_anim_past_end(struct MarioState *m) {
    return FALSE;
}
static void finish_rollout_landing(struct MarioState *m, s32 starting, u32 arg) {
}
static s32 check_kick_or_dive_in_air(struct MarioState *m) {
    return FALSE;
}
static void play_mario_jump_sound(struct MarioState *m) {
}
static void common_air_action_step(struct MarioState *m, u32 action, s32 anim, u32 checks) {
    sAnimation = anim;
}
static s32 geo_update_animation_frame(struct AnimInfo *info, s32 *assist) {
    return info->animFrame;
}
static void *segmented_to_virtual(const void *p) {
    return (void *) p;
}
static s32 mario_transition_pose_render(struct AnimInfo *info, struct MarioRenderPose *pose) {
    return FALSE;
}
static void mario_set_forward_vel(struct MarioState *m, f32 speed) {
    m->forwardVel = speed;
    m->vel[0] = m->slideVelX = speed * sins(m->faceAngle[1]);
    m->vel[2] = m->slideVelZ = speed * coss(m->faceAngle[1]);
}
static s32 set_mario_animation(struct MarioState *m, s32 animation) {
    sAnimation = animation;
    return 0;
}
static s32 perform_air_step(struct MarioState *m, u32 checks) {
    m->pos[0] += m->vel[0];
    m->pos[2] += m->vel[2];
    m->pos[1] += m->vel[1];
    if (sStep == AIR_STEP_LANDED) {
        m->pos[1] = m->floorHeight;
    }
    return sStep;
}
static void mario_bonk_reflection(struct MarioState *m, s32 negate) {
    sBonks++;
}
static s32 lava_boost_on_wall(struct MarioState *m) {
    sLava++;
    return set_mario_action(m, ACT_LAVA_BOOST, 0);
}
static s32 update_sliding(struct MarioState *m, f32 stopSpeed) {
    return sStop;
}
static void common_slide_action(struct MarioState *m, u32 end, u32 air, s32 anim) {
    sGroundSteps++;
    m->pos[2] += m->slideVelZ;
    if (sStep == AIR_STEP_HIT_WALL) {
        set_mario_action(m, ACT_GROUND_BONK, 0);
    } else if (sStep == AIR_STEP_NONE) {
        set_mario_action(m, air, 0);
    }
}
static struct Object *try_to_spawn_object(s32 param, f32 scale, struct Object *parent, s32 model,
                                          const u32 *behavior) {
    if (sSpawnFails) {
        return NULL;
    }
    memset(&explosion, 0, sizeof(explosion));
    explosion.parentObj = parent;
    sExplosions++;
    return &explosion;
}
static s32 handstand_jump_protects_from_fall(struct MarioState *m) {
    return FALSE;
}
static s32 should_get_stuck_in_ground(struct MarioState *m) {
    return FALSE;
}
static s32 check_fall_damage(struct MarioState *m, u32 action) {
    sFallChecks++;
    return FALSE;
}
static s32 is_anim_past_frame(struct MarioState *m, s16 frame) {
    return sFootfall == frame;
}
static void play_sound_and_spawn_particles(struct MarioState *m, u32 sound, u32 type) {
    play_sound(sound, m->marioObj->header.gfx.cameraToObject);
}
static struct {
    s32 strokeCooldown, strokeBuffered;
} sSwimming;
static s16 sSwimStrength;
static s32 sSwimCancel, sIdleCancel;
static s32 check_swimming_b_input(struct MarioState *m) {
    return sSwimCancel;
}
static s32 check_water_jump(struct MarioState *m) {
    return FALSE;
}
static void reset_bob_variables(struct MarioState *m) {
}
static void queue_rumble_submerged(void) {
}
static void common_swimming_step(struct MarioState *m, f32 speed, f32 drag) {
}
static void set_anim_to_frame(struct MarioState *m, s16 frame) {
}
static void play_sound_if_no_flag(struct MarioState *m, u32 sound, u32 flag) {
    if (!(m->flags & flag)) {
        play_sound(sound, m->marioObj->header.gfx.cameraToObject);
        m->flags |= flag;
    }
}
static s32 check_common_idle_cancels(struct MarioState *m) {
    return sIdleCancel;
}
static f32 find_floor_height_relative_polar(struct MarioState *m, s16 angle, f32 distance) {
    return m->floorHeight;
}
static void stationary_ground_step(struct MarioState *m) {
}
#include "he_thicc_sources.inc"

static s32 test_he_thicc_pose(struct AnimInfo *info, s16 **values, u16 **index, s16 frame, f32 scale,
                              s32 normalized) {
    struct MarioRenderPose pose = {
        *values, *index, frame, scale, normalized ? MARIO_POSE_RENDER_ROOT : MARIO_POSE_NATIVE_ROOT,
    };
    s32 result = mario_he_thicc_render_pose(info, &pose);
    *values = pose.values;
    *index = pose.index;
    return result;
}

static void begin(void) {
    memset(&object, 0, sizeof(object));
    memset(&mario, 0, sizeof(mario));
    floorSurface.type = SURFACE_DEFAULT;
    floorSurface.normal.y = 1;
    controller.buttonDown = 0;
    mario.marioObj = &object;
    mario.area = &area;
    mario.health = 0x880;
    area.terrainType = 0;
    floorSurface.flags = 0;
    sIdleCancel = sSwimCancel = 0;
    mario.floor = &floorSurface;
    mario.controller = &controller;
    mario.action = ACT_FREEFALL;
    mario.waterLevel = sWater = -10000;
    object.header.gfx.scale[0] = object.header.gfx.scale[1] = object.header.gfx.scale[2] = 1;
    gCurrDemoInput = gCurrCreditsEntry = NULL;
    memset(sStats, 0, sizeof(sStats));
    sPalette = MARIO_PALETTE_HE_THICC;
    sSounds = sRumbles = sWaves = sGroundSteps = sStop = sAnimation = sBonks = sLava = 0;
    sStep = AIR_STEP_LANDED;
    sFallChecks = sSpawnFails = sExplosions = sFootfall = 0;
    mario_he_thicc_reset();
}

static int wavedash_chain(void) {
    for (int cycle = 0; cycle < 3; cycle++) {
        begin();
        mario.pos[1] = 100;
        mario.vel[1] = -5;
        mario.forwardVel = 40;
        mario.intendedYaw = 0x4000;
        mario.input = INPUT_Z_PRESSED | INPUT_NONZERO_ANALOG;
        EXPECT_EQ(mario_he_thicc_try_wavedash(&mario), TRUE);
        EXPECT_EQ(mario.action, ACT_HE_THICC_WAVEDASH);
        EXPECT_EQ(sStats[PS_HE_THICC_WAVEDASHES], 1);
        EXPECT_NEAR(mario.vel[0], 56, 0.001f);
        EXPECT_NEAR(mario.vel[2], 0, 0.001f);
        EXPECT_EQ(mario.input & INPUT_Z_PRESSED, 0);
        EXPECT_EQ(mario_he_thicc_try_wavedash(&mario), FALSE);
        EXPECT_EQ(sStats[PS_HE_THICC_WAVEDASHES], 1);
        EXPECT_EQ(mario_he_thicc_wavedash(&mario), FALSE);
        EXPECT_EQ(mario.action, ACT_HE_THICC_BELLY_SLIDE);
        EXPECT_NEAR(mario.pos[1], 0, 0.001f);
        EXPECT_NEAR(mario.forwardVel, 56, 0.001f);
        EXPECT_EQ(sWaves, 0);
        mario.input = 0;
        mario_he_thicc_belly_slide(&mario);
        EXPECT_EQ(sGroundSteps, 1);
        mario.input = INPUT_A_PRESSED;
        EXPECT_EQ(mario_he_thicc_belly_slide(&mario), TRUE);
        EXPECT_EQ(mario.action, ACT_FORWARD_ROLLOUT);
        EXPECT_NEAR(mario.forwardVel, 80, 0.001f);
        EXPECT_NEAR(mario.vel[1], 48, 0.001f);
        EXPECT_EQ(sStats[PS_HE_THICC_BELLY_BOUNCES], 1);
        EXPECT_EQ(sStats[PS_HE_THICC_BELLY_SLAMS], 0);
        sStep = AIR_STEP_NONE;
        act_forward_rollout(&mario);
        EXPECT_NEAR(mario.vel[1], 48, 0.001f);
        EXPECT_EQ(mario.actionState, 1);
    }
    for (int reject = 0; reject < 10; reject++) {
        begin();
        mario.pos[1] = 100;
        mario.vel[1] = -5;
        mario.input = INPUT_Z_PRESSED | INPUT_NONZERO_ANALOG;
        switch (reject) {
            case 0:
                sPalette = MARIO_PALETTE_STANDARD;
                break;
            case 1:
                mario.vel[1] = 1;
                break;
            case 2:
                mario.pos[1] = 161;
                break;
            case 3:
                mario.input = INPUT_Z_PRESSED;
                break;
            case 4:
                mario.heldObj = &object;
                break;
            case 5:
                floorSurface.type = SURFACE_BURNING;
                break;
            case 6:
                mario.action = ACT_HARD_BACKWARD_AIR_KB;
                break;
            case 7:
                mario.input |= INPUT_B_PRESSED;
                break;
            case 8:
                controller.buttonDown = L_TRIG;
                break;
            case 9:
                mario.waterLevel = 200;
                break;
        }
        EXPECT_EQ(mario_he_thicc_try_wavedash(&mario), FALSE);
    }
    begin();
    mario.action = ACT_HE_THICC_WAVEDASH;
    mario.input = INPUT_A_PRESSED;
    sStep = AIR_STEP_LANDED;
    mario_he_thicc_wavedash(&mario);
    EXPECT_EQ(mario.action, ACT_FORWARD_ROLLOUT);
    EXPECT_NEAR(mario.vel[1], 48, 0.001f);
    EXPECT_NEAR(mario.forwardVel, 80, 0.001f);
    begin();
    mario.action = ACT_HE_THICC_WAVEDASH;
    sStep = AIR_STEP_HIT_WALL;
    EXPECT_EQ(mario_he_thicc_wavedash(&mario), FALSE);
    EXPECT_EQ(mario.action, ACT_SOFT_BONK);
    EXPECT_EQ(sBonks, 1);
    begin();
    mario.action = ACT_HE_THICC_WAVEDASH;
    sStep = AIR_STEP_HIT_LAVA_WALL;
    EXPECT_EQ(mario_he_thicc_wavedash(&mario), FALSE);
    EXPECT_EQ(mario.action, ACT_LAVA_BOOST);
    EXPECT_EQ(sLava, 1);
    return 1;
}

static int belly_landings(void) {
    for (int cycle = 0; cycle < 3; cycle++) {
        begin();
        mario.action = ACT_DIVE;
        mario.peakHeight = HE_THICC_BELLY_SLAM_HEIGHT;
        mario.vel[1] = -100;
        mario.forwardVel = 50;
        EXPECT_EQ(mario_he_thicc_try_belly_landing(&mario), TRUE);
        EXPECT_EQ(mario.action, ACT_HE_THICC_BELLY_SLIDE);
        EXPECT_EQ(sStats[PS_HE_THICC_BELLY_SLAMS], 1);
        EXPECT_NEAR(mario.forwardVel, 75, 0.001f);
        EXPECT_NEAR(mario.vel[1], 0, 0.001f);
        EXPECT_EQ(sWaves, 1);
        EXPECT_EQ(mario_he_thicc_try_belly_landing(&mario), FALSE);
        EXPECT_EQ(sWaves, 1);
        mario.input = INPUT_A_DOWN;
        for (int frame = 0; frame < 6; frame++) {
            EXPECT_EQ(mario_he_thicc_belly_slide(&mario), FALSE);
        }
        EXPECT_EQ(mario.action, ACT_HE_THICC_BELLY_SLIDE);
        mario.input = INPUT_A_PRESSED;
        mario_he_thicc_belly_slide(&mario);
        EXPECT_NEAR(mario.forwardVel, 75, 0.001f);
        EXPECT_NEAR(mario.vel[1], 20, 0.001f);
        EXPECT_EQ(sStats[PS_HE_THICC_BELLY_SLAMS], 1);
        EXPECT_EQ(sStats[PS_HE_THICC_BELLY_BOUNCES], 0);
    }
    for (int reject = 0; reject < 13; reject++) {
        begin();
        mario.action = ACT_DIVE;
        mario.forwardVel = 50;
        mario.peakHeight = HE_THICC_BELLY_SLAM_HEIGHT;
        mario.vel[1] = -100;
        switch (reject) {
            case 0:
                sPalette = MARIO_PALETTE_STANDARD;
                break;
            case 1:
                mario.vel[1] = -54;
                break;
            case 2:
                mario.pos[1] = 5;
                break;
            case 3:
                floorSurface.normal.y = 0.5f;
                break;
            case 4:
                floorSurface.type = SURFACE_INSTANT_QUICKSAND;
                break;
            case 5:
                mario.action = ACT_GROUND_POUND;
                break;
            case 6:
                mario.action = ACT_THROWN_FORWARD;
                break;
            case 7:
                mario.heldObj = &object;
                break;
            case 8:
                sWater = 11;
                break;
            case 9:
                mario.peakHeight = HE_THICC_BELLY_SLAM_HEIGHT - 1;
                break;
            case 10:
                mario.forwardVel = 0;
                break;
            case 11:
                mario.forwardVel = -50;
                break;
            case 12:
                mario.riddenObj = &object;
                break;
        }
        EXPECT_EQ(mario_he_thicc_try_belly_landing(&mario), FALSE);
        EXPECT_EQ(sWaves, 0);
    }
    begin();
    mario.action = ACT_DIVE;
    mario.forwardVel = 50;
    mario.peakHeight = HE_THICC_BELLY_SLAM_HEIGHT;
    mario.vel[1] = -55;
    EXPECT_EQ(mario_he_thicc_try_belly_landing(&mario), TRUE);
    mario.input = 0;
    sStep = AIR_STEP_NONE;
    mario_he_thicc_belly_slide(&mario);
    EXPECT_EQ(mario.action, ACT_FREEFALL);
    begin();
    mario.action = ACT_DIVE;
    mario.forwardVel = 50;
    mario.peakHeight = HE_THICC_BELLY_SLAM_HEIGHT;
    mario.vel[1] = -100;
    mario_he_thicc_try_belly_landing(&mario);
    sStep = AIR_STEP_HIT_WALL;
    mario_he_thicc_belly_slide(&mario);
    EXPECT_EQ(mario.action, ACT_GROUND_BONK);
    const u32 actions[] = { ACT_FREEFALL,         ACT_JUMP,      ACT_DOUBLE_JUMP,
                            ACT_TRIPLE_JUMP,      ACT_BACKFLIP,  ACT_SIDE_FLIP,
                            ACT_WALL_KICK_AIR,    ACT_LONG_JUMP, ACT_FORWARD_ROLLOUT,
                            ACT_BACKWARD_ROLLOUT, ACT_JUMP_KICK, ACT_SLIDE_KICK,
                            ACT_THROWN_BACKWARD };
    for (unsigned i = 0; i < sizeof(actions) / sizeof(*actions); i++) {
        begin();
        mario.action = actions[i];
        mario.forwardVel = 50;
        mario.peakHeight = 3000;
        mario.vel[1] = -100;
        EXPECT_EQ(check_fall_damage_or_get_stuck(&mario, ACT_HARD_BACKWARD_GROUND_KB), FALSE);
        EXPECT_EQ(mario.action, actions[i]);
        EXPECT_EQ(sFallChecks, 1); // These landings reach the real damage service boundary.
        EXPECT_EQ(sWaves, 0);
    }
    begin();
    mario.action = ACT_DIVE;
    mario.forwardVel = 50;
    mario.vel[1] = -100;
    mario.pos[1] = mario.floorHeight = 1000;
    mario.peakHeight = 1599;
    EXPECT_EQ(check_fall_damage_or_get_stuck(&mario, ACT_HARD_BACKWARD_GROUND_KB), FALSE);
    EXPECT_EQ(sFallChecks, 1);
    mario.peakHeight = 1600;
    EXPECT_EQ(check_fall_damage_or_get_stuck(&mario, ACT_HARD_BACKWARD_GROUND_KB), TRUE);
    EXPECT_EQ(sFallChecks, 1);
    EXPECT_EQ(sWaves, 1);
    return 1;
}

static int pound_and_wall(void) {
    for (int cycle = 0; cycle < 3; cycle++) {
        begin();
        mario.action = ACT_GROUND_POUND_LAND;
        mario.input = INPUT_A_DOWN;
        EXPECT_EQ(mario_he_thicc_try_pound_bounce(&mario), FALSE);
        mario.input = INPUT_A_PRESSED | INPUT_Z_PRESSED;
        mario.actionTimer = 5;
        EXPECT_EQ(mario_he_thicc_try_pound_bounce(&mario), TRUE);
        EXPECT_EQ(mario.action, ACT_TRIPLE_JUMP);
        EXPECT_EQ(sStats[PS_HE_THICC_POUND_BOUNCES], 1);
        EXPECT_NEAR(mario.vel[1], 84, 0.001f);
        EXPECT_NEAR(mario.forwardVel, 48, 0.001f);
        EXPECT_EQ(mario.input & INPUT_Z_PRESSED, 0);
        EXPECT_EQ(mario_he_thicc_try_pound_bounce(&mario), FALSE);
        begin();
        mario.action = ACT_GROUND_POUND_LAND;
        mario.input = INPUT_A_PRESSED;
        mario.actionTimer = 6;
        EXPECT_EQ(mario_he_thicc_try_pound_bounce(&mario), FALSE);
        mario.actionTimer = 0;
        sPalette = MARIO_PALETTE_STANDARD;
        EXPECT_EQ(mario_he_thicc_try_pound_bounce(&mario), FALSE);
        begin();
        mario.action = ACT_WALL_KICK_AIR;
        mario.vel[1] = 62;
        mario_he_thicc_wall_kick(&mario);
        EXPECT_EQ(mario.actionArg, ACT_ARG_WALL_KICK_HE_THICC);
        EXPECT_EQ(sStats[PS_HE_THICC_WALL_KICKS], 1);
        EXPECT_EQ(sStats[PS_HE_THICC_CLAPS], 1);
        EXPECT_NEAR(mario.vel[1], 62, 0.001f);
        EXPECT_EQ(sSounds, 1);
        EXPECT_EQ(sRumbles, 1);
    }
    return 1;
}

static int jiggle_and_clap(void) {
    begin();
    mario.action = ACT_WALKING;
    mario_he_thicc_update(&mario);
    for (int frame = 0; frame < 6; frame++) {
        object.header.gfx.scale[0] = object.header.gfx.scale[1] = object.header.gfx.scale[2] = 1;
        mario.pos[2] += 40;
        mario_he_thicc_update(&mario);
    }
    EXPECT_EQ(sSounds, 2);
    EXPECT_EQ(sRumbles, 0);
    EXPECT_EQ((sLastSound & SOUNDARGS_MASK_BANK) >> SOUNDARGS_SHIFT_BANK, SOUND_BANK_DETAILS);
    EXPECT_TRUE(sHeThicc.jiggle > 0);
    f32 largest = 0;
    for (int frame = 0; frame < 120; frame++) {
        object.header.gfx.scale[0] = object.header.gfx.scale[1] = object.header.gfx.scale[2] = 1;
        mario_he_thicc_update(&mario);
        largest = MAX(largest, absf(sHeThicc.jiggle));
        EXPECT_TRUE(object.header.gfx.scale[1] >= 0.8f && object.header.gfx.scale[1] <= 1.2f);
    }
    EXPECT_TRUE(largest > 0.1f);
    EXPECT_NEAR(sHeThicc.jiggle, 0, 0.001f);
    EXPECT_EQ(sSounds, 2);
    mario.action = ACT_GROUND_POUND_LAND;
    mario_he_thicc_update(&mario);
    mario_he_thicc_update(&mario);
    EXPECT_EQ(sSounds, 3);
    mario.squishTimer = 0xFF;
    object.header.gfx.scale[1] = 0.4f;
    for (int frame = 0; frame < 20; frame++) {
        mario_he_thicc_update(&mario);
        EXPECT_NEAR(object.header.gfx.scale[1], 0.4f, 0.0001f);
    }
    gCurrDemoInput = &mario;
    mario_he_thicc_update(&mario);
    EXPECT_EQ(sHeThicc.initialized, FALSE);
    EXPECT_NEAR(sHeThicc.jiggle, 0, 0.001f);
    return 1;
}

static int inverse_dab_pose(void) {
    begin();
    struct AnimInfo info = { .curAnim = (struct Animation *) anim_D1, .animID = MARIO_ANIM_DAB };
    for (int sample = 0; sample < DAB_ANALOG_FRAME_COUNT; sample++) {
        int frame = DAB_ANALOG_START_FRAME + sample;
        s16 *pose = (s16 *) anim_D1_values;
        u16 *index = (u16 *) anim_D1_indices;
        EXPECT_EQ(test_he_thicc_pose(&info, &pose, &index, frame, 1, FALSE), TRUE);
        for (int channel = 12; channel < 39; channel++) {
            int count = anim_D1_indices[channel * 2];
            int offset = anim_D1_indices[channel * 2 + 1];
            EXPECT_EQ(pose[channel], anim_D1_values[offset + MIN(frame, count - 1)]);
            if (sample == 0 || sample == DAB_ANALOG_FRAME_COUNT - 1) {
                int original = sample == 0 ? 7 : 2;
                EXPECT_EQ(pose[channel], anim_D1_values[offset + MIN(original, count - 1)]);
            }
        }
    }
    s16 *pose = (s16 *) anim_D1_values;
    u16 *index = (u16 *) anim_D1_indices;
    info.animID = MARIO_ANIM_CRAWLING;
    test_he_thicc_pose(&info, &pose, &index, DAB_ANALOG_START_FRAME + 63, 1, FALSE);
    EXPECT_EQ(pose[12], -4824); // Other moves retain the baked pose.
    sPalette = MARIO_PALETTE_STANDARD;
    pose = (s16 *) anim_D1_values;
    index = (u16 *) anim_D1_indices;
    info.animID = MARIO_ANIM_DAB;
    EXPECT_EQ(test_he_thicc_pose(&info, &pose, &index, 115, 1, FALSE), FALSE);
    EXPECT_TRUE(pose == anim_D1_values && index == anim_D1_indices);
    return 1;
}

static int dab_pose(void) {
    s16 values[63];
    u16 attributes[126];
    struct Animation anim = { .unusedBoneCount = 20 };
    struct AnimInfo info = { .curAnim = &anim };
    begin();
    for (int i = 0; i < 63; i++) {
        values[i] = i * 20;
        attributes[i * 2] = 1;
        attributes[i * 2 + 1] = i;
    }
    for (int flags = 0; flags < 4; flags++) {
        anim.flags = flags == 0   ? 0
                     : flags == 1 ? ANIM_FLAG_NO_TRANS
                     : flags == 2 ? ANIM_FLAG_HOR_TRANS
                                  : ANIM_FLAG_VERT_TRANS;
        s16 *pose = values;
        u16 *index = attributes;
        EXPECT_EQ(test_he_thicc_pose(&info, &pose, &index, 3, 2, FALSE), TRUE);
        EXPECT_EQ(pose[1], flags == 1 || flags == 3 ? 0 : 40);
        EXPECT_EQ(pose[2], flags == 1 || flags == 2 ? 0 : 80);
        EXPECT_EQ(pose[12], -4824);
        EXPECT_EQ(pose[21], 1);
        EXPECT_EQ(pose[30], 8893);
        EXPECT_EQ(pose[38], 0);
        EXPECT_EQ(pose[45], values[45]);
        EXPECT_EQ(pose[62], values[62]);
        EXPECT_EQ(values[30], 600); // Source animation is never patched.
    }
    anim.flags = ANIM_FLAG_NO_TRANS;
    s16 *pose = values;
    u16 *index = attributes;
    EXPECT_EQ(test_he_thicc_pose(&info, &pose, &index, 0, 3, TRUE), TRUE);
    EXPECT_EQ(pose[1], values[1]); // Preceding wall blend already normalized root.
    f32 spring = sHeThicc.jiggle;
    pose = values;
    index = attributes;
    test_he_thicc_pose(&info, &pose, &index, 0, 1, FALSE);
    EXPECT_NEAR(sHeThicc.jiggle, spring, 0.0001f);
    sPalette = MARIO_PALETTE_STANDARD;
    pose = values;
    index = attributes;
    EXPECT_EQ(test_he_thicc_pose(&info, &pose, &index, 0, 1, FALSE), FALSE);
    EXPECT_TRUE(pose == values && index == attributes);
    sPalette = MARIO_PALETTE_HE_THICC;
    anim.unusedBoneCount = 19;
    EXPECT_EQ(test_he_thicc_pose(&info, &pose, &index, 0, 1, FALSE), FALSE);
    return 1;
}

static int action_and_render_hooks(void) {
    begin();
    mario.action = ACT_GROUND_POUND_LAND;
    for (int frame = 0; frame < 6; frame++) {
        act_ground_pound_land(&mario);
    }
    EXPECT_EQ(mario.actionTimer, 6);
    mario.input = INPUT_A_PRESSED;
    EXPECT_EQ(act_ground_pound_land(&mario), FALSE);
    EXPECT_EQ(mario.action, ACT_GROUND_POUND_LAND);
    mario.actionTimer = 5;
    EXPECT_EQ(act_ground_pound_land(&mario), TRUE);
    EXPECT_EQ(mario.action, ACT_TRIPLE_JUMP);
    begin();
    mario.action = ACT_GROUND_POUND_LAND;
    mario.input = INPUT_A_PRESSED | INPUT_STOMPED;
    EXPECT_EQ(act_ground_pound_land(&mario), TRUE);
    EXPECT_EQ(mario.action, ACT_SHOCKWAVE_BOUNCE);
    begin();
    mario.action = ACT_WALL_KICK_AIR;
    mario_he_thicc_wall_kick(&mario);
    act_wall_kick_air(&mario);
    EXPECT_EQ(sAnimation, MARIO_ANIM_SLIDE);
    mario.actionArg = 0;
    act_wall_kick_air(&mario);
    EXPECT_EQ(sAnimation, MARIO_ANIM_SLIDEJUMP); // Pole launch shares this action.

    s16 values[63];
    u16 index[126];
    for (int i = 0; i < 63; i++) {
        values[i] = i;
        index[i * 2] = 1;
        index[i * 2 + 1] = i;
    }
    struct Animation anim = { .unusedBoneCount = 20, .values = values, .index = index };
    struct GraphNodeObject unrelated = { 0 };
    object.header.gfx.animInfo.curAnim = gMirrorMario.animInfo.curAnim = &anim;
    unrelated.animInfo.curAnim = &anim;
    gMarioObject = &object;
    struct AnimInfo *owners[] = { &object.header.gfx.animInfo, &gMirrorMario.animInfo,
                                  &unrelated.animInfo, &object.header.gfx.animInfo };
    for (int owner = 0; owner < 4; owner++) {
        gCurGraphNodeHeldObject = owner == 3 ? &object : NULL;
        struct AnimInfo *info = owners[owner];
        gCurGraphNodeObject = NULL; // Production initializes before child traversal.
        geo_set_animation_globals(info, FALSE);
        EXPECT_EQ(gCurrAnimData[30], owner < 2 ? 8893 : 30);
        EXPECT_EQ(values[30], 30);
    }
    gCurGraphNodeObject = (struct GraphNodeObject *) &object;
    gCurGraphNodeHeldObject = NULL;
    sPalette = MARIO_PALETTE_STANDARD;
    geo_set_animation_globals(&object.header.gfx.animInfo, FALSE);
    EXPECT_TRUE(gCurrAnimData == values);
    return 1;
}

static int breaststroke_rings(void) {
    for (int held = 0; held < 2; held++) {
        for (int palette = 0; palette < 2; palette++) {
            for (int cycle = 0; cycle < 3; cycle++) {
                begin();
                sPalette = palette;
                mario.action = held ? ACT_HOLD_BREASTSTROKE : ACT_BREASTSTROKE;
                for (int frame = 0; frame < 10; frame++) {
                    mario.particleFlags = 0;
                    if (held) {
                        act_hold_breaststroke(&mario);
                    } else {
                        act_breaststroke(&mario);
                    }
                    EXPECT_EQ(!!(mario.particleFlags & PARTICLE_VANISH_WATER_RING),
                              palette == MARIO_PALETTE_HE_THICC && frame == 0);
                    EXPECT_EQ(sStats[PS_HE_THICC_SWIMMING_STROKES], palette == MARIO_PALETTE_HE_THICC);
                }
            }
        }
    }
    begin();
    mario.action = ACT_BREASTSTROKE;
    sSwimCancel = TRUE;
    act_breaststroke(&mario);
    EXPECT_EQ(mario.particleFlags, 0);
    return 1;
}

static int idle_tired(void) {
    for (int cold = 0; cold < 2; cold++) {
        for (int capped = 0; capped < 2; capped++) {
            for (int cycle = 0; cycle < 3; cycle++) {
                begin();
                mario.action = ACT_IDLE;
                mario.flags = capped ? MARIO_CAP_ON_HEAD : 0;
                area.terrainType = cold ? TERRAIN_SNOW : 0;
                for (int frame = 0; frame < 90; frame++) {
                    EXPECT_EQ(act_idle(&mario), FALSE);
                    EXPECT_EQ(mario.action, ACT_IDLE);
                }
                EXPECT_EQ(act_idle(&mario), TRUE);
                EXPECT_EQ(mario.action, ACT_START_SLEEPING);
                EXPECT_EQ(mario.actionState, ACT_STATE_START_SLEEPING_YAWN);
                EXPECT_EQ(mario.actionArg, 0); // Ordinary sleep uses no dab-triggered input lock.
            }
        }
    }
    begin();
    mario.action = ACT_IDLE;
    mario.flags = MARIO_CAP_ON_HEAD;
    sPalette = MARIO_PALETTE_STANDARD;
    for (int frame = 0; frame < 91; frame++) {
        act_idle(&mario);
    }
    EXPECT_EQ(mario.action, ACT_IDLE);
    begin();
    mario.action = ACT_IDLE;
    mario.actionTimer = 90;
    controller.buttonDown = L_TRIG;
    act_idle(&mario);
    EXPECT_EQ(mario.action, ACT_IDLE);
    EXPECT_EQ(mario.actionTimer, 1);
    controller.buttonDown = 0;
    mario.actionTimer = 90;
    floorSurface.flags = SURFACE_FLAG_DYNAMIC;
    act_idle(&mario);
    EXPECT_EQ(mario.action, ACT_IDLE);
    floorSurface.flags = 0;
    sIdleCancel = TRUE;
    act_idle(&mario);
    EXPECT_EQ(mario.action, ACT_IDLE);
    return 1;
}

static int move_feedback(void) {
    begin();
    mario_he_thicc_slide_kick_bounce(&mario);
    EXPECT_EQ(sWaves, 1);
    EXPECT_TRUE(mario.particleFlags & PARTICLE_HORIZONTAL_STAR);
    sPalette = MARIO_PALETTE_STANDARD;
    mario.particleFlags = 0;
    mario_he_thicc_slide_kick_bounce(&mario);
    mario_he_thicc_attack_impact(&mario);
    mario_he_thicc_breaststroke(&mario);
    EXPECT_EQ(sWaves, 1);
    EXPECT_EQ(sExplosions, 0);
    EXPECT_EQ(sStats[PS_HE_THICC_SLIDE_KICK_BOUNCES], 1);
    EXPECT_EQ(sStats[PS_HE_THICC_EXPLOSIVE_STRIKES], 0);
    EXPECT_EQ(sStats[PS_HE_THICC_SWIMMING_STROKES], 0);
    EXPECT_EQ(mario.particleFlags, 0);
    begin();
    mario.pos[0] = 100;
    mario.pos[1] = 200;
    mario.pos[2] = 300;
    mario.faceAngle[1] = 0x4000;
    mario_he_thicc_attack_impact(&mario);
    EXPECT_EQ(sExplosions, 1);
    EXPECT_TRUE(explosion.parentObj == &object);
    EXPECT_EQ(explosion.oBehParams2ndByte, HE_THICC_PUNCH_EXPLOSION);
    EXPECT_EQ(explosion.oInteractType, 0);
    EXPECT_EQ(explosion.oIntangibleTimer, -1);
    EXPECT_NEAR(explosion.oPosX, 180, 0.001f);
    EXPECT_NEAR(explosion.oPosY, 280, 0.001f);
    EXPECT_NEAR(explosion.oPosZ, 300, 0.001f);
    EXPECT_EQ(sWaves, 1);
    sSpawnFails = TRUE;
    mario_he_thicc_attack_impact(&mario);
    EXPECT_EQ(sExplosions, 1);
    EXPECT_EQ(sStats[PS_HE_THICC_EXPLOSIVE_STRIKES], 2);
    EXPECT_EQ(sWaves, 2); // Object exhaustion cannot remove the AOE/feedback.
    mario_he_thicc_breaststroke(&mario);
    EXPECT_TRUE(mario.particleFlags & PARTICLE_VANISH_WATER_RING);
    return 1;
}

static int running_audio(void) {
    for (int palette = 0; palette < 2; palette++) {
        begin();
        sPalette = palette;
        mario.action = ACT_WALKING;
        mario.forwardVel = 24;
        object.header.gfx.animInfo.animID = MARIO_ANIM_RUNNING;
        play_step_sound(&mario, 9, 45);
        EXPECT_EQ(sSounds, 0);
        sFootfall = 9;
        play_step_sound(&mario, 9, 45);
        EXPECT_EQ(sSounds, palette == MARIO_PALETTE_HE_THICC ? 2 : 1);
        sFootfall = 0;
        play_step_sound(&mario, 9, 45);
        EXPECT_EQ(sSounds, palette == MARIO_PALETTE_HE_THICC ? 2 : 1);
    }
    for (int reject = 0; reject < 6; reject++) {
        begin();
        mario.action = ACT_WALKING;
        mario.forwardVel = 24;
        if (reject == 0) {
            mario.forwardVel = 23;
        }
        if (reject == 1) {
            mario.pos[1] = 10;
        }
        if (reject == 2) {
            mario.quicksandDepth = 51;
        }
        if (reject == 3) {
            gCurrDemoInput = &object;
        }
        if (reject == 4) {
            gCurrCreditsEntry = &object;
        }
        if (reject == 5) {
            mario.action = ACT_CRAWLING;
        }
        mario_he_thicc_running_step(&mario);
        EXPECT_EQ(sSounds, 0);
    }
    begin();
    mario.action = ACT_HOLD_WALKING;
    mario.forwardVel = -24;
    mario_he_thicc_running_step(&mario);
    EXPECT_EQ(sLastSound, SOUND_HE_THICC_RUNNING_STEP);
    return 1;
}

static int movement_dabs(void) {
    s16 values[63] = { 0 }, first[63], second[63];
    u16 attributes[126];
    struct Animation anim = { .unusedBoneCount = 20, .loopEnd = 32 };
    struct AnimInfo info = { .curAnim = &anim };
    const s16 moves[] = { MARIO_ANIM_BACKFLIP, MARIO_ANIM_SLIDEJUMP, MARIO_ANIM_SLIDE_DIVE,
                          MARIO_ANIM_SWIM_PART1, MARIO_ANIM_CRAWLING };
    begin();
    for (int i = 0; i < 63; i++) {
        values[i] = i;
        attributes[i * 2] = 1;
        attributes[i * 2 + 1] = i;
    }
    for (unsigned move = 0; move < sizeof(moves) / sizeof(*moves); move++) {
        info.animID = moves[move];
        s16 *pose = values;
        u16 *index = attributes;
        test_he_thicc_pose(&info, &pose, &index, 0, 1, FALSE);
        memcpy(first, pose, sizeof(first));
        pose = values;
        index = attributes;
        test_he_thicc_pose(&info, &pose, &index, 8, 1, FALSE);
        memcpy(second, pose, sizeof(second));
        EXPECT_TRUE(memcmp(first + 15, second + 15, 24 * sizeof(s16)) != 0);
        for (int i = 0; i < 12; i++) {
            EXPECT_EQ(pose[i], values[i]);
        }
        for (int i = 39; i < 63; i++) {
            EXPECT_EQ(pose[i], values[i]);
        }
        pose = values;
        index = attributes;
        test_he_thicc_pose(&info, &pose, &index, 8, 1, FALSE);
        EXPECT_EQ(memcmp(second, pose, sizeof(second)), 0); // Additional renders cannot tick a pose.
        pose = values;
        index = attributes;
        test_he_thicc_pose(&info, &pose, &index, 32, 1, FALSE);
        EXPECT_EQ(memcmp(first, pose, sizeof(first)), 0);
        for (int i = 0; i < 63; i++) {
            EXPECT_EQ(values[i], i);
        }
    }
    for (int id = MARIO_ANIM_START_SLEEP_IDLE; id <= MARIO_ANIM_SLEEP_LYING; id++) {
        info.animID = id;
        s16 *pose = values;
        u16 *index = attributes;
        test_he_thicc_pose(&info, &pose, &index, 8, 1, FALSE);
        for (int i = 12; i < 39; i++) {
            EXPECT_EQ(pose[i], values[i]);
        }
    }
    return 1;
}

static int native_movement_pose(void) {
    const s16 animations[] = {
        MARIO_ANIM_IDLE_HEAD_LEFT,
        MARIO_ANIM_IDLE_HEAD_RIGHT,
        MARIO_ANIM_IDLE_HEAD_CENTER,
        MARIO_ANIM_IDLE_WITH_LIGHT_OBJ,
        MARIO_ANIM_IDLE_HEAVY_OBJ,
        MARIO_ANIM_IDLE_IN_QUICKSAND,
        MARIO_ANIM_SHIVERING_WARMING_HAND,
        MARIO_ANIM_SHIVERING,
        MARIO_ANIM_SHIVERING_RETURN_TO_IDLE,
        MARIO_ANIM_WALK_PANTING,
        MARIO_ANIM_WAKE_FROM_SLEEP,
        MARIO_ANIM_WALKING,
        MARIO_ANIM_RUNNING,
        MARIO_ANIM_RUNNING_UNUSED,
        MARIO_ANIM_TIPTOE,
        MARIO_ANIM_WALK_WITH_LIGHT_OBJ,
        MARIO_ANIM_RUN_WITH_LIGHT_OBJ,
        MARIO_ANIM_SLOW_WALK_WITH_LIGHT_OBJ,
        MARIO_ANIM_WALK_WITH_HEAVY_OBJ,
        MARIO_ANIM_SKID_ON_GROUND,
        MARIO_ANIM_STOP_SKID,
        MARIO_ANIM_SINGLE_JUMP,
        MARIO_ANIM_DOUBLE_JUMP_RISE,
        MARIO_ANIM_DOUBLE_JUMP_FALL,
        MARIO_ANIM_TRIPLE_JUMP,
        MARIO_ANIM_GENERAL_FALL,
        MARIO_ANIM_LAND_FROM_SINGLE_JUMP,
        MARIO_ANIM_LAND_FROM_DOUBLE_JUMP,
        MARIO_ANIM_TRIPLE_JUMP_LAND,
        MARIO_ANIM_GENERAL_LAND,
        MARIO_ANIM_JUMP_WITH_LIGHT_OBJ,
        MARIO_ANIM_FALL_WITH_LIGHT_OBJ,
        MARIO_ANIM_JUMP_LAND_WITH_LIGHT_OBJ,
        MARIO_ANIM_FALL_LAND_WITH_LIGHT_OBJ,
    };
    s16 values[126];
    u16 attributes[126];
    struct Animation anim = { .unusedBoneCount = 20, .loopEnd = 2 };
    struct AnimInfo info = { .curAnim = &anim };
    begin();
    for (int i = 0; i < 63; i++) {
        values[i * 2] = i * 10;
        values[i * 2 + 1] = i * 10 + 5;
        attributes[i * 2] = 2;
        attributes[i * 2 + 1] = i * 2;
    }
    sHeThicc.jiggle = .5f;
    sHeThicc.clapFrames = 3;
    for (unsigned id = 0; id < sizeof(animations) / sizeof(*animations); id++) {
        info.animID = animations[id];
        for (int frame = 0; frame < 2; frame++) {
            for (int draw = 0; draw < 3; draw++) {
                s16 *pose = values;
                u16 *index = attributes;
                EXPECT_EQ(test_he_thicc_pose(&info, &pose, &index, frame, 1, FALSE), TRUE);
                for (int channel = 12; channel < 39; channel++) {
                    EXPECT_EQ(pose[channel], values[channel * 2 + frame]);
                }
                EXPECT_TRUE(pose[8] != values[16 + frame]); // Jiggle and clap remain active.
                EXPECT_TRUE(pose[41] != values[82 + frame]);
                EXPECT_TRUE(pose[53] != values[106 + frame]);
                EXPECT_EQ(sHeThicc.clapFrames, 3);
            }
        }
    }
    return TRUE;
}

static int unlimited_speed(void) {
    for (int master = 0; master < 2; master++) {
        for (int enabled = 0; enabled < 2; enabled++) {
            sTestGameOptions[GO_CHEATS_ENABLED] = master;
            sTestGameOptions[GO_NO_SPEED_LIMIT] = enabled;
            begin();
            mario.pos[1] = 100;
            mario.vel[1] = -5;
            mario.forwardVel = 300;
            mario.intendedYaw = 0x4000;
            mario.input = INPUT_Z_PRESSED | INPUT_NONZERO_ANALOG;
            EXPECT_EQ(mario_he_thicc_try_wavedash(&mario), TRUE);
            EXPECT_NEAR(mario.forwardVel, master && enabled ? 316 : 100, 0.001f);
            begin();
            mario.action = ACT_HE_THICC_BELLY_SLIDE;
            mario.forwardVel = 300;
            mario.input = INPUT_A_PRESSED;
            mario.actionTimer = 0;
            mario_he_thicc_belly_slide(&mario);
            EXPECT_NEAR(mario.forwardVel, master && enabled ? 324 : 100, 0.001f);
        }
    }
    sTestGameOptions[GO_CHEATS_ENABLED] = sTestGameOptions[GO_NO_SPEED_LIMIT] = 0;
    return TRUE;
}

int main(int argc, char **argv) {
    test_init(argc, argv);
#define RUN(name, fn)                                                                                  \
    if (test_selected("he_thicc." name)) {                                                             \
        test_report("he_thicc." name, fn());                                                           \
    }
    RUN("unlimited_speed", unlimited_speed);
    RUN("wavedash_chain", wavedash_chain);
    RUN("belly_landings", belly_landings);
    RUN("pound_and_wall", pound_and_wall);
    RUN("jiggle_and_clap", jiggle_and_clap);
    RUN("dab_pose", dab_pose);
    RUN("movement_dabs", movement_dabs);
    RUN("native_movement_pose", native_movement_pose);
    RUN("move_feedback", move_feedback);
    RUN("breaststroke_rings", breaststroke_rings);
    RUN("idle_tired", idle_tired);
    RUN("running_audio", running_audio);
    RUN("inverse_dab_pose", inverse_dab_pose);
    RUN("action_and_render_hooks", action_and_render_hooks);
    return test_finish();
}
