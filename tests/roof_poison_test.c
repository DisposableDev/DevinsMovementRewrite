#include <assert.h>
#define MUSHROOMS_HEAL
#define ENABLE_LIVES
#define ACT_READING_SIGN 0x00001308
#if defined(UNLOCK_ALL) || defined(COMPLETE_SAVE_FILE)
#define game_options_secrets_blocked() 1
#else
#define game_options_secrets_blocked() (gDebugLevelSelect)
#endif
#include <math.h>
#include <stddef.h>
#include <string.h>

typedef signed char s8;
typedef unsigned char u8;
typedef int s32;
typedef short s16;
typedef float f32;
typedef unsigned int u32;
#define TRUE 1
#define FALSE 0
#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define segmented_to_virtual(p) ((void *) (p))
#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define COURSE_MIN 1
#define COURSE_MAX 25
#define COURSE_NONE 0
#define COURSE_RR 15
#define COURSE_FLAG_CANNON_UNLOCKED 0x80
#define COURSE_BITFS 17
#define COURSE_BITS 18
#define COURSE_BITDW 16
#define COURSE_PSS 19
#define COURSE_COTMC 20
#define COURSE_TOTWC 21
#define COURSE_VCUTM 22
#define COURSE_WMOTR 23
#define COURSE_SA 24
#define COURSE_CAKE_END 25
#define ARRAY_COUNT(a) ((s32) (sizeof(a) / sizeof((a)[0])))
#define NUM_SAVE_FILES 4
#define SAVE_FLAG_FILE_EXISTS 1
#define SAVE_FLAG_HAVE_KEY_1 16
#define SAVE_FLAG_HAVE_KEY_2 32
#define SAVE_FLAG_UNLOCKED_BASEMENT_DOOR 64
#define SAVE_FLAG_UNLOCKED_UPSTAIRS_DOOR 128
#define STAR_FLAG_TO_SAVE_FLAG(x) ((x) << 24)
#define LEVEL_CASTLE_GROUNDS 16
#define EXCLAMATION_BOX_BP_WING_CAP 0
#define EXCLAMATION_BOX_BP1_NEEDS_SWITCH 0
#define GET_BPARAM1(params) ((params) >> 24)
#define COURSE_NUM_TO_INDEX(course) ((course) - 1)
#define DIALOG_YOSHI_CREDIT 220
#define DIALOG_CASTLE_SECRET_HOME 221
#define DIALOG_CASTLE_SECRET_OW 222
#define DIALOG_CASTLE_SECRET_WHY 223
#define DIALOG_CASTLE_SECRET_AWAY 224
#define DIALOG_CASTLE_SECRET_COMPLETE 225
#define DIALOG_CASTLE_BOARD_INTRUDER 226
#define SOUND_OBJ_WHOMP 99
#define SHAKE_POS_SMALL 1
#define MODEL_NONE 0
#define MODEL_LEVEL_GEOMETRY_05 5
#define MODEL_WOODEN_SIGNPOST 6
#define MODEL_DIRT_ANIMATION 7
#define SOUND_GENERAL_BREAK_BOX 11
#define SOUND_GENERAL_POUND_WOOD_POST 12
#define INTERACT_TEXT 2
#define INT_STATUS_NONE 0
#define MODEL_YOSHI_CREDIT_SIGN 1
#define MODEL_POISON_MUSHROOM 2
#define ACTIVE_FLAG_DEACTIVATED 0
#define INT_SUBTYPE_NPC 1
#define SOUND_MENU_MARIO_CASTLE_WARP 10
#define SOUND_GENERAL2_1UP_APPEAR 11
#define SOUND_GENERAL_COLLECT_1UP 13
#define PS_1_UP_MUSHROOMS_COLLECTED 1
#define MUSHROOM_ACT_INIT 0
#define MUSHROOM_ACT_MOVING 1
#define ACT_STANDING_DEATH 12
#define ACT_IDLE 13
#define MARIO_ACTION_SOUND_PLAYED 1
#define SOUND_ACTION_READ_SIGN 2
#define MARIO_ANIM_FIRST_PERSON 3
#define CUTSCENE_NONE 0
#define O_MOVE_ANGLE_YAW_INDEX 0
#define sqr(x) ((x) * (x))

struct Object {
    struct Object *parentObj;
    s32 activeFlags, oInteractionSubtype, oAction, oAnimState, oTimer, oOpacity, oIntangibleTimer;
    f32 oGravity, oFriction, oBuoyancy, oVelY, oForwardVel;
    f32 oPosX, oPosY, oPosZ, oDistanceToMario;
    s16 oMoveAnglePitch, oMoveAngleYaw, oFaceAngleYaw;
    s32 oCastleSecretChildSpawned;
    void *collisionData;
    s32 oSignpostMarioPounding, oInteractType, oInteractStatus;
    f32 hitboxHeight, oMarioReadingSignDPosX, oMarioReadingSignDPosZ;
    f32 oSignpostSinkDepth, oSignpostTargetSinkDepth, oYoshiCreditAttackHeight;
    s32 oFaceAnglePitch, oAngleVelPitch;
    s32 oMarioReadingSignDYaw, oBehParams2ndByte;
    u32 oBehParams;
    s32 *behavior;
    struct {
        struct {
            f32 pos[3], scale[3];
            s16 angle[3];
        } gfx;
    } header;
} object, child, warpChild, *o = &object, marioObject, *gMarioObject = &marioObject;
struct Surface {
    struct Object *object;
} wall;
struct MarioState {
    struct Surface *wall;
    s32 health, healCounter, hurtCounter, respawnHealCounter, yoshiCoinDeathPending,
        poisonMushroomDeathPending;
    s32 action, actionArg, flags, invincTimer, numLives, squishTimer;
    s32 actionState, actionTimer;
    f32 pos[3], floorHeight;
    s16 faceAngle[3];
    struct Object *marioObj, *usedObj;
} mario, *gMarioState = &mario;
static s32 bhvYoshiCreditSign[1], bhvPoisonMushroom[1];
static s32 bhvCastleSecretWindow[1], bhvCastleSecretRoom[1], bhvCastleSecretSign[1];
static s32 bhvCastleLobbyFloorSign[1];
static s32 bhvCastleLobbyRejectionSign[1];
static int rejectionFades;
static void castle_lobby_finale_sign_read(struct Object *sign) {
    (void) sign;
}
static void level_reject_secret_lobby(void) {
    rejectionFades++;
}
static const s16 castle_secret_room_collision[1] = { 0 };
static const s16 castle_secret_painting_room_collision[1] = { 1 };
static s32 hidden, particles, collisionLoads, pounding, roomSpawns, signSpawns, warpSpawns;
static void cur_obj_hide(void) {
    hidden = TRUE;
}
static void cur_obj_unhide(void) {
    hidden = FALSE;
}
static void load_object_collision_model(void) {
    collisionLoads++;
}
static s32 cur_obj_is_mario_ground_pounding_platform(void) {
    return pounding;
}
static void spawn_triangle_break_particles(s16 count, s16 model, f32 size, s16 animation) {
    assert(count == 12 && model == MODEL_DIRT_ANIMATION && size == 1.0f && animation == 0);
    particles++;
}
static s32 sYoshiDead, gDebugLevelSelect, gCurrSaveFileNum = 1;
static void *gCurrCreditsEntry, *gCurrDemoInput;
static s32 gCurrLevelNum, gCurrAreaIndex, gSaveFileModified, saveWrites;
static s32 gCurrCourseNum;
static s32 gMainMenuDataModified;
static u8 sModifiedFiles;
static u32 sCastleSecretSavePending;
void yoshi_credit_sign_area_exit(void);
void castle_secret_sign_reset(s32 fileIndex);
s32 castle_secret_window_is_broken(void);
#define bcopy(src, dst, size) memcpy(dst, src, size)
#ifndef UNLOCK_ALL
static const u32 sCapSaveFlags[] = { 2, 4, 8 };
#endif
struct SaveFile {
    u32 flags;
    unsigned char courseStars[25];
};
static struct {
    struct SaveFile files[4][2];
} gSaveBuffer;
static struct SaveFile persisted[4];
static void player_stats_checkpoint(s32 fileIndex) {
    assert(fileIndex >= 0 && fileIndex < NUM_SAVE_FILES);
}
static s32 statsHits, statsHealthLost, statsDeaths, statsGameOvers;
static void player_stats_damage(void) {
    statsHits++;
}
static void player_stats_health(s32 before, s32 after) {
    statsHealthLost += before - after;
}
static void player_stats_death(s32 gameOver, s32 pit) {
    assert(!pit);
    statsDeaths++;
    statsGameOvers += gameOver;
}
static void save_file_do_save(s32 fileIndex) {
    assert(gSaveFileModified);
    persisted[fileIndex] = gSaveBuffer.files[fileIndex][0];
    gSaveBuffer.files[fileIndex][1] = persisted[fileIndex];
    sCastleSecretSavePending &= ~(1u << fileIndex);
    sModifiedFiles &= ~(1u << fileIndex);
    gSaveFileModified = sModifiedFiles != 0;
    saveWrites++;
}
static u32 save_file_get_flags(void) {
    return gSaveBuffer.files[gCurrSaveFileNum - 1][0].flags;
}
static s32 starCount, spawned, soundCount, sparkleCount, steps, airLoops, overlaps;
static s32 spawnFails, readUpdates, drops, extinguishes, rumbles, deleted;
static s32 mushroomsCollected, twirlResets;
static void player_stats_add(s32 stat, s32 amount) {
    assert(stat == PS_1_UP_MUSHROOMS_COLLECTED && amount == 1);
    mushroomsCollected++;
}
static void mario_ground_twirl_reset_cooldown(void) {
    twirlResets++;
}
static struct Object intruderWindow;
static s32 windowPresent, crushCakes, shakes;
static struct Object *cur_obj_nearest_object_with_behavior(s32 *behavior) {
    assert(behavior == bhvCastleSecretWindow);
    return windowPresent ? &intruderWindow : NULL;
}
static void cur_obj_scale(f32 scale) {
    assert(scale == 3.0f);
}
static void cur_obj_shake_screen(s32 shake) {
    assert(shake == SHAKE_POS_SMALL);
    shakes++;
}
static void level_warp_to_castle_board_cake(void) {
    crushCakes++;
}

static s32 gGlobalSoundSource;
static struct {
    s32 cutscene;
} camera, *gCamera = &camera;
static s32 timeStopped, dialogs;

static void play_sound_if_no_flag(struct MarioState *m, s32 sound, s32 flag) {
    (void) m;
    (void) sound;
    (void) flag;
}
static void trigger_cutscene_dialog(s32 arg) {
    assert(arg == 1);
}
static void enable_time_stop(void) {
    timeStopped = TRUE;
}
static void enable_time_stop_including_mario(void) {
    timeStopped = TRUE;
}
static void disable_time_stop_including_mario(void) {
    timeStopped = FALSE;
}
static void disable_time_stop(void) {
    timeStopped = FALSE;
}
static void set_mario_animation(struct MarioState *m, s32 animation) {
    assert(m == &mario && animation == MARIO_ANIM_FIRST_PERSON);
}
static void create_dialog_inverted_box(s32 dialog) {
    assert(dialog == DIALOG_YOSHI_CREDIT || dialog == DIALOG_CASTLE_BOARD_INTRUDER);
    dialogs++;
}
static s32 obj_has_behavior(struct Object *obj, s32 *behavior) {
    return obj->behavior == behavior;
}
static void vec3f_set(f32 *v, f32 x, f32 y, f32 z) {
    v[0] = x;
    v[1] = y;
    v[2] = z;
}
#define vec3f_copy(dst, src) memcpy(dst, src, sizeof(f32) * 3)
static void vec3s_set(s16 *v, s16 x, s16 y, s16 z) {
    v[0] = x;
    v[1] = y;
    v[2] = z;
}

static s32 save_file_get_total_star_count(s32 file, s32 min, s32 max) {
    assert(file == gCurrSaveFileNum - 1 && min == 0 && max == 24);
    return starCount;
}
static struct Object *spawn_object_relative(s16 dialog, s16 x, s16 y, s16 z, struct Object *parent,
                                            s32 model, s32 *behavior) {
    assert(dialog == DIALOG_YOSHI_CREDIT && x == 0 && y == 0 && z == 0);
    assert(parent == o && model == MODEL_YOSHI_CREDIT_SIGN && behavior == bhvYoshiCreditSign);
    spawned++;
    return &child;
}
static struct Object *try_to_spawn_object(s16 y, f32 scale, struct Object *parent, s32 model,
                                          s32 *behavior) {
    assert(scale == 1.0f && parent == o);
    if (behavior == bhvPoisonMushroom) {
        assert(y == 100 && model == MODEL_POISON_MUSHROOM);
    } else {
        assert(y == 0);
        assert((behavior == bhvCastleSecretRoom && model == MODEL_LEVEL_GEOMETRY_05)
               || (behavior == bhvCastleSecretSign && model == MODEL_YOSHI_CREDIT_SIGN));
    }
    if (spawnFails) {
        return NULL;
    }
    spawned++;
    if (behavior == bhvCastleSecretRoom) {
        child.parentObj = parent;
        roomSpawns++;
    }
    if (behavior == bhvCastleSecretSign) {
        signSpawns++;
    }
    return &child;
}
static void actual_message_panel_loop(void);
static void bhv_message_panel_loop(void) {
    readUpdates++;
    actual_message_panel_loop();
}
static void cur_obj_become_intangible(void) {
    o->oIntangibleTimer = -1;
}
static void cur_obj_become_tangible(void) {
    o->oIntangibleTimer = 0;
}
static void cur_obj_play_sound_2(s32 sound) {
    assert(sound == SOUND_MENU_MARIO_CASTLE_WARP || sound == SOUND_GENERAL_BREAK_BOX
           || sound == SOUND_GENERAL_POUND_WOOD_POST || sound == SOUND_OBJ_WHOMP);
    soundCount++;
}
static void obj_mark_for_deletion(struct Object *obj) {
    obj->activeFlags = ACTIVE_FLAG_DEACTIVATED;
    deleted++;
}
static s16 atan2s(f32 y, f32 x) {
    return (s16) (atan2f(x, y) * (32768.0f / 3.14159265358979323846f));
}
static f32 sins(s16 angle) {
    return sinf(angle * (3.14159265358979323846f / 32768.0f));
}
static f32 coss(s16 angle) {
    return cosf(angle * (3.14159265358979323846f / 32768.0f));
}
static s16 approach_s16_symmetric(s16 current, s16 target, s16 step) {
    s16 difference = target - current;
    if (difference > step) {
        return current + step;
    }
    if (difference < -step) {
        return current - step;
    }
    return target;
}
static void obj_turn_toward_object(struct Object *obj, struct Object *target, s32 index, s16 step) {
    assert(index == O_MOVE_ANGLE_YAW_INDEX);
    s16 yaw = atan2s(target->header.gfx.pos[2] - obj->oPosZ, target->header.gfx.pos[0] - obj->oPosX);
    obj->oMoveAngleYaw = approach_s16_symmetric(obj->oMoveAngleYaw, yaw, step);
}
static s16 one_up_step(void) {
    steps++;
    o->oPosX += sins(o->oMoveAngleYaw) * o->oForwardVel;
    o->oPosZ += coss(o->oMoveAngleYaw) * o->oForwardVel;
    o->oPosY += o->oVelY;
    return 0;
}
static void one_up_loop_in_air(void) {
    airLoops++;
    o->oVelY = 40;
    o->oForwardVel = 10;
}
static void one_up_spawn_sparkles(void) {
    sparkleCount++;
}
static void play_sound(s32 sound, s32 source) {
    assert((sound == SOUND_GENERAL2_1UP_APPEAR || sound == SOUND_GENERAL_COLLECT_1UP)
           && source == gGlobalSoundSource);
    soundCount++;
}
static s32 obj_check_if_collided_with_object(struct Object *obj, struct Object *marioObj) {
    assert(obj == o && marioObj == gMarioObject);
    return overlaps;
}
static void mario_stop_riding_and_holding(struct MarioState *m) {
    assert(m == &mario);
    drops++;
}
static void mario_extinguish_fire(struct MarioState *m) {
    assert(m == &mario);
    extinguishes++;
}
static void set_mario_action(struct MarioState *m, s32 action, s32 arg) {
    m->action = action;
    m->actionArg = arg;
}
static void queue_rumble_data(s32 frames, s32 strength) {
    assert((frames == 5 || frames == 10) && strength == 80);
    rumbles++;
}
static void set_object_visibility(struct Object *obj, s32 distance) {
    assert(obj == o && distance == 3000);
}

#include "roof_poison_actual.inc"

static void reset(void) {
    memset(&object, 0, sizeof(object));
    memset(&child, 0, sizeof(child));
    memset(&warpChild, 0, sizeof(warpChild));
    hidden = particles = collisionLoads = pounding = roomSpawns = signSpawns = warpSpawns = 0;
    memset(&mario, 0, sizeof(mario));
    memset(&marioObject, 0, sizeof(marioObject));
    object.activeFlags = 1;
    object.oIntangibleTimer = -1;
    object.oOpacity = 255;
    object.oBehParams2ndByte = DIALOG_YOSHI_CREDIT;
    object.behavior = bhvYoshiCreditSign;
    mario.health = 0x880;
    mario.marioObj = gMarioObject;
    mario.usedObj = &object;
    camera.cutscene = 1;
    timeStopped = dialogs = 0;
    starCount = 0;
    sYoshiDead = gDebugLevelSelect = 0;
    gCurrCreditsEntry = NULL;
    gCurrDemoInput = NULL;
    gCurrSaveFileNum = 1;
    gCurrLevelNum = LEVEL_CASTLE_GROUNDS;
    gCurrAreaIndex = 1;
    memset(&gSaveBuffer, 0, sizeof(gSaveBuffer));
    memset(persisted, 0, sizeof(persisted));
    gSaveFileModified = saveWrites = 0;
    sModifiedFiles = 0;
    statsHits = statsHealthLost = statsDeaths = statsGameOvers = 0;
    sCastleSecretSavePending = 0;
    sYoshiCreditSignPendingExitFileIndex = -1;
    sCompletionSignDismissedFileMask = 0;
    windowPresent = crushCakes = shakes = 0;
    memset(&intruderWindow, 0, sizeof(intruderWindow));
    spawned = soundCount = sparkleCount = steps = airLoops = overlaps = 0;
    spawnFails = readUpdates = drops = extinguishes = rumbles = deleted = 0;
    mushroomsCollected = twirlResets = 0;
}

static void check_gate(void) {
    for (s32 stars = 0; stars <= 121; stars++) {
        for (s32 debug = 0; debug < 2; debug++) {
            reset();
            starCount = stars;
            gDebugLevelSelect = debug;
            bhv_yoshi_init();
#if defined(UNLOCK_ALL) || defined(COMPLETE_SAVE_FILE)
            assert(spawned == 0 && object.activeFlags == 1);
#else
            assert(spawned == (stars < 120 && !debug));
            assert(object.activeFlags == (stars >= 120 && !debug));
#endif
        }
    }
    // The encounter stays gone after a saved exit/death, without unlocking Yoshi early.
    reset();
    save_file_unlock_castle_wing_cap(0);
    assert(saveWrites == 0);
    save_file_flush_castle_secret();
    memset(&gSaveBuffer, 0, sizeof(gSaveBuffer));
    gSaveBuffer.files[0][0] = persisted[0];
    starCount = 119;
    bhv_yoshi_init();
    assert(spawned == 0);
#if !defined(UNLOCK_ALL) && !defined(COMPLETE_SAVE_FILE)
    assert(object.activeFlags == 0);
    gCurrSaveFileNum = 2;
    object.activeFlags = 1;
    bhv_yoshi_init();
    assert(spawned == 1 && object.activeFlags == 0);
#endif
    gCurrSaveFileNum = 1;
    spawned = 0;
    object.activeFlags = 1;
    starCount = 120;
    bhv_yoshi_init();
    assert(spawned == 0 && object.activeFlags == 1);
    reset();
    sYoshiDead = TRUE;
    starCount = 120;
    bhv_yoshi_init();
    assert(spawned == 0 && object.activeFlags == 0);
    reset();
    gCurrCreditsEntry = &child;
    starCount = 0;
    bhv_yoshi_init();
    assert(spawned == 0 && object.activeFlags == 1);
}

static void check_sign(void) {
    reset();
    for (s32 frame = 0; frame < 20; frame++) {
        bhv_yoshi_credit_sign_loop();
    }
    assert(readUpdates == 20 && spawned == 0 && object.oOpacity == 255);
    for (s32 frame = 0; frame < 11; frame++) {
        act_reading_sign(&mario);
        assert(object.oAction == 0);
    }
    assert(dialogs == 1 && timeStopped);
    act_reading_sign(&mario);
    assert(object.oAction == 0 && timeStopped);
    camera.cutscene = CUTSCENE_NONE;
    act_reading_sign(&mario);
    assert(object.oAction == 1 && mario.action == ACT_IDLE && !timeStopped);
    spawnFails = TRUE;
    bhv_yoshi_credit_sign_loop();
    assert(object.oAction == 1 && soundCount == 0 && object.oOpacity == 255);
    spawnFails = FALSE;
    bhv_yoshi_credit_sign_loop();
    assert(object.oAction == 2 && spawned == 1 && soundCount == 1);
    assert(object.oIntangibleTimer == -1);
    for (s32 frame = 0; frame < 32; frame++) {
        bhv_yoshi_credit_sign_loop();
    }
    assert(deleted == 1 && object.oOpacity == 0 && spawned == 1 && soundCount == 1);
    assert(saveWrites == 0);
    assert(save_file_get_castle_secret(0) & CASTLE_SECRET_MOVED);
    assert(!save_file_has_castle_wing_cap(0));
    gCurrSaveFileNum = 2; // The exit writes to the sign reader's file, even if the active file changed.
    yoshi_credit_sign_area_exit();
    assert(saveWrites == 0 && save_file_has_castle_wing_cap(0));
    assert(!(save_file_get_castle_secret(0) & CASTLE_SECRET_AWAY));
    assert(!(save_file_get_castle_secret(1) & CASTLE_SECRET_AWAY));
    assert(!save_file_has_castle_wing_cap(1));
    save_file_flush_castle_secret();
    assert(saveWrites == 1);
    assert(persisted[0].courseStars[24]
           == (SAVE_CAKE_CASTLE_WING_CAP_UNLOCKED | SAVE_CAKE_CASTLE_SECRET_MOVED));
    yoshi_credit_sign_area_exit();
    assert(saveWrites == 1);
}

static void check_mushroom(void) {
    reset();
    overlaps = TRUE;
    for (s32 frame = 0; frame <= 37; frame++) {
        object.oTimer = frame;
        bhv_poison_mushroom_loop();
        assert(mario.health == 0x880);
        assert(object.oIntangibleTimer == (frame < 37 ? -1 : 0));
    }
    assert(soundCount == 1 && sparkleCount == 20 && steps == 38 && airLoops == 38);
    assert(object.oAction == MUSHROOM_ACT_MOVING);
    assert(object.oVelY == 0 && object.oForwardVel == 0);
    overlaps = FALSE;
    bhv_poison_mushroom_loop();
    assert(steps == 39 && mario.health == 0x880);
    for (s32 cap = 0; cap < 4; cap++) {
        reset();
        object.oAction = MUSHROOM_ACT_MOVING;
        overlaps = TRUE;
        mario.flags = 1 << cap;
        mario.invincTimer = 100;
        mario.healCounter = 31;
        mario.hurtCounter = 8;
        mario.yoshiCoinDeathPending = TRUE;
        bhv_poison_mushroom_loop();
        assert(mario.health == 0xFF && mario.healCounter == 0 && mario.hurtCounter == 0);
        assert(mario.poisonMushroomDeathPending && !mario.yoshiCoinDeathPending);
        assert(statsHits == 1 && statsHealthLost == 0x781);
        assert(statsDeaths == 0 && statsGameOvers == 0); // The ordinary death warp counts these later.
        assert(mario.action == ACT_STANDING_DEATH && mario.actionArg == 0);
        assert(drops == 1 && extinguishes == 1 && rumbles == 1 && deleted == 1);
        assert(saveWrites == 0 && save_file_has_castle_wing_cap(0));
        save_file_reload(); // Game Over flushes before restoring the backup.
        assert(saveWrites == 1 && save_file_has_castle_wing_cap(0));
        bhv_poison_mushroom_loop();
        assert(drops == 1 && rumbles == 1);
    }
}

static void check_exit_and_regular_mushroom_gate(void) {
    reset();
    yoshi_credit_sign_area_exit();
    assert(saveWrites == 0); // Leaving before reading does not unlock the box.
    object.oAction = YOSHI_CREDIT_SIGN_ACT_FADE;
    for (s32 frame = 0; frame < 31; frame++) {
        bhv_yoshi_credit_sign_loop();
    }
    yoshi_credit_sign_area_exit();
    assert(saveWrites == 0); // The sign still has visible opacity.
    for (s32 stars = 119; stars <= 120; stars++) {
        for (s32 debug = 0; debug < 2; debug++) {
            reset();
            starCount = stars;
            gDebugLevelSelect = debug;
            object.oBehParams2ndByte = MUSHROOM_BP_REQUIRES_CASTLE_COMPLETION;
            bhv_1up_init();
#if defined(UNLOCK_ALL) || defined(COMPLETE_SAVE_FILE)
            assert(object.activeFlags == 1);
#else
            assert(object.activeFlags == (stars >= 120 && !debug));
#endif
        }
    }
    reset();
    starCount = 0;
    bhv_1up_init();
    assert(object.activeFlags == 1); // Unmarked regular mushrooms keep their existing rules.
}

static void check_existing_completion_mushroom(void) {
    reset();
    starCount = CASTLE_ROOF_COMPLETION_STARS;
    object.oBehParams2ndByte = MUSHROOM_BP_REQUIRES_CASTLE_COMPLETION;
    bhv_1up_init();
    assert(object.activeFlags == 1);
    mario.health = 0x800;
    mario.hurtCounter = 3;
    mario.respawnHealCounter = 7;
    mario.numLives = 2;
    struct MarioState before = mario;
    overlaps = TRUE;
    gDebugLevelSelect = TRUE;
    bhv_1up_loop();
    assert(object.activeFlags == ACTIVE_FLAG_DEACTIVATED);
    assert(memcmp(&before, &mario, sizeof(mario)) == 0);
    assert(!mushroomsCollected && !soundCount && !twirlResets && !extinguishes && !rumbles);

    // A clean reload restores the completion reward and its normal effects.
    reset();
    starCount = CASTLE_ROOF_COMPLETION_STARS;
    object.oBehParams2ndByte = MUSHROOM_BP_REQUIRES_CASTLE_COMPLETION;
    bhv_1up_init();
    overlaps = TRUE;
    bhv_1up_loop();
    assert(object.activeFlags == ACTIVE_FLAG_DEACTIVATED);
    assert(mario.healCounter == 31 && mario.numLives == 1);
    assert(mushroomsCollected == 1 && soundCount == 1 && twirlResets == 1 && extinguishes == 1
           && rumbles == 1);

    // Ordinary mushrooms and hidden-mushroom trigger counts retain pickups.
    reset();
    object.oBehParams2ndByte = 0;
    overlaps = gDebugLevelSelect = TRUE;
    bhv_1up_loop();
    assert(mario.numLives == 1 && mushroomsCollected == 1);
    reset();
    object.oBehParams2ndByte = MUSHROOM_BP_REQUIRES_CASTLE_COMPLETION;
    overlaps = gDebugLevelSelect = TRUE;
    bhv_1up_interact();
    assert(mario.numLives == 1 && mushroomsCollected == 1);
}

static void check_saved_cap_box_scope(void) {
    reset();
    object.oBehParams2ndByte = EXCLAMATION_BOX_BP_WING_CAP;
#ifndef UNLOCK_ALL
    assert(!exclamation_box_cap_is_unlocked());
#endif
    gSaveBuffer.files[0][0].courseStars[24] = 5;
    save_file_unlock_castle_wing_cap(0);
    assert(exclamation_box_cap_is_unlocked());
    assert((gSaveBuffer.files[0][0].courseStars[24] & 0x7F) == 5);
    assert(gSaveBuffer.files[0][0].flags == SAVE_FLAG_FILE_EXISTS);
    assert(saveWrites == 0);
    save_file_flush_castle_secret();
    gSaveBuffer.files[0][0] = persisted[0]; // Reloading the saved data retains the local unlock.
    assert(save_file_has_castle_wing_cap(0));
#ifndef UNLOCK_ALL
    gCurrLevelNum = 1;
    assert(!exclamation_box_cap_is_unlocked());
    gCurrLevelNum = LEVEL_CASTLE_GROUNDS;
    object.oBehParams2ndByte = 1;
    assert(!exclamation_box_cap_is_unlocked());
    object.oBehParams2ndByte = EXCLAMATION_BOX_BP_WING_CAP;
    gCurrSaveFileNum = 2;
    assert(!exclamation_box_cap_is_unlocked());
#endif
    save_file_unlock_castle_wing_cap(-1);
    save_file_unlock_castle_wing_cap(4);
    assert(saveWrites == 1);
}

static void check_homing(void) {
    const f32 targets[][3] = { { 500, 0, 0 }, { -500, 200, 0 }, { 0, -200, 500 }, { 0, 0, -500 } };
    for (unsigned i = 0; i < sizeof(targets) / sizeof(*targets); i++) {
        reset();
        object.oAction = MUSHROOM_ACT_MOVING;
        vec3f_copy(marioObject.header.gfx.pos, targets[i]);
        f32 closest = 1000000.0f;
        for (s32 frame = 0; frame < 64; frame++) {
            bhv_poison_mushroom_loop();
            f32 distance = sqr(object.oPosX - targets[i][0])
                           + sqr(object.oPosY - targets[i][1] - 120.0f)
                           + sqr(object.oPosZ - targets[i][2]);
            if (distance < closest) {
                closest = distance;
            }
        }
        assert(closest < sqr(100.0f));
        assert(steps == 64 && mario.health == 0x880);
    }
}

static void finish_secret_dialog(void) {
    mario.actionState = ACT_STATE_READING_SIGN_IN_DIALOG;
    camera.cutscene = CUTSCENE_NONE;
    act_reading_sign(&mario);
    assert(object.oInteractType == 0 && object.oAction == 1);
    bhv_castle_secret_sign_loop();
    assert(object.oInteractType
           == ((o->oAction != 2
                && (castle_secret_room_has_full_stars()
                    || (save_file_get_castle_secret(0) & CASTLE_SECRET_AWAY)))
                   ? INTERACT_TEXT
                   : 0));
}

static void check_secret_room(void) {
    reset();
    object.behavior = bhvCastleSecretWindow;
    wall.object = &object;
    mario.wall = &wall;
    bhv_castle_secret_window_loop();
    assert(!hidden && !roomSpawns && !particles && collisionLoads == 1);
    castle_secret_window_bonk(&mario);
    assert(object.oAction == 1);
    spawnFails = TRUE;
    bhv_castle_secret_window_loop();
    assert(!hidden && particles == 0 && !roomSpawns && collisionLoads == 2);
    assert(object.oAction == CASTLE_SECRET_WINDOW_BREAK_REQUESTED);
    assert(!castle_secret_window_is_broken());
    spawnFails = FALSE;
    bhv_castle_secret_window_loop();
    assert(!hidden && !particles && roomSpawns == 1);
    bhv_castle_secret_window_loop();
    assert(!hidden && !particles && roomSpawns == 1);
    o = &child;
    bhv_castle_secret_room_loop();
    o = &object;
    assert(object.oCastleSecretChildSpawned == CASTLE_SECRET_ROOM_COLLISION_READY);
    bhv_castle_secret_window_loop();
    assert(hidden && roomSpawns == 1 && particles == 1);
    memset(&object, 0, sizeof(object));
    object.behavior = bhvCastleSecretRoom;
    bhv_castle_secret_room_loop();
    assert(!signSpawns && !warpSpawns && !object.oCastleSecretChildSpawned);
    assert(object.oAnimState == CASTLE_SECRET_PORTRAIT_BOW);
    assert(save_file_get_castle_secret(0) == 0 && saveWrites == 0);
    save_file_set_castle_secret(0, CASTLE_SECRET_MOVED);
    spawnFails = TRUE;
    bhv_castle_secret_room_loop();
    assert(!signSpawns);
    spawnFails = FALSE;
    bhv_castle_secret_room_loop();
    bhv_castle_secret_room_loop();
    assert(signSpawns == 1 && child.oPosZ == -380.0f);
    assert(object.oAnimState == CASTLE_SECRET_PORTRAIT_SMUG);
    assert(saveWrites == 0);
    save_file_flush_castle_secret();
    memset(&gSaveBuffer, 0, sizeof(gSaveBuffer));
    gSaveBuffer.files[0][0] = persisted[0];
    assert(save_file_get_castle_secret(0) == CASTLE_SECRET_MOVED);
    assert(save_file_get_castle_secret(1) == 0);

    reset();
    object.behavior = bhvCastleSecretSign;
    object.oAction = 0;
    bhv_castle_secret_sign_loop();
    assert(object.oBehParams2ndByte == DIALOG_CASTLE_SECRET_HOME
           && object.oInteractType == INTERACT_TEXT);
    pounding = TRUE; // The introductory line comes before ground-pound replies.
    bhv_castle_secret_sign_loop();
    assert(object.oBehParams2ndByte == DIALOG_CASTLE_SECRET_HOME);
    pounding = FALSE;
    finish_secret_dialog();
    pounding = TRUE;
    bhv_castle_secret_sign_loop();
    assert(object.oBehParams2ndByte == DIALOG_CASTLE_SECRET_OW
           && object.oInteractType == INTERACT_TEXT);
    for (s32 i = 0; i < 10; i++) {
        bhv_castle_secret_sign_loop();
    }
    assert(!(save_file_get_castle_secret(0) & CASTLE_SECRET_POUNDED_TWICE));
    finish_secret_dialog();
    pounding = FALSE;
    bhv_castle_secret_sign_loop();
    pounding = TRUE;
    bhv_castle_secret_sign_loop();
    assert(object.oBehParams2ndByte == DIALOG_CASTLE_SECRET_WHY
           && object.oInteractType == INTERACT_TEXT);
    finish_secret_dialog();
    pounding = FALSE;
    bhv_castle_secret_sign_loop();
    pounding = TRUE;
    bhv_castle_secret_sign_loop();
    assert(object.oInteractType == 0);
    assert(saveWrites == 0);
    save_file_flush_castle_secret();
    memset(&gSaveBuffer, 0, sizeof(gSaveBuffer));
    gSaveBuffer.files[0][0] = persisted[0];
    object.oSignpostMarioPounding = 0;
    bhv_castle_secret_sign_loop();
    assert(object.oInteractType == 0); // Reload cannot repeat a consumed line.

    // Departure changes the note only after the introductory room dialogue was read.
    for (s32 stage = 0; stage < 4; stage++) {
        reset();
        u32 progress = CASTLE_SECRET_MOVED;
        if (stage > 0) {
            progress |= CASTLE_SECRET_INTRO_READ;
        }
        if (stage > 1) {
            progress |= CASTLE_SECRET_POUNDED_ONCE | CASTLE_SECRET_OW_READ;
        }
        if (stage > 2) {
            progress |= CASTLE_SECRET_POUNDED_TWICE | CASTLE_SECRET_WHY_READ;
        }
        save_file_set_castle_secret(0, progress);
        object.behavior = bhvCastleSecretWindow;
        bhv_castle_secret_window_loop();
        yoshi_credit_sign_area_exit();
        assert(!!(save_file_get_castle_secret(0) & CASTLE_SECRET_AWAY) == (stage > 0));
        assert(saveWrites == 0); // Same-level area exit updates RAM only.
        save_file_flush_castle_secret();
        assert(saveWrites == 1);
        memset(&gSaveBuffer, 0, sizeof(gSaveBuffer));
        gSaveBuffer.files[0][0] = persisted[0];
        memset(&object, 0, sizeof(object));
        hidden = FALSE;
        roomSpawns = 0;
        object.behavior = bhvCastleSecretWindow;
        bhv_castle_secret_window_loop();
        assert(!hidden && object.oAction == 0 && roomSpawns == 0);
        wall.object = &object;
        mario.wall = &wall;
        castle_secret_window_bonk(&mario);
        bhv_castle_secret_window_loop();
        o = &child;
        bhv_castle_secret_room_loop();
        o = &object;
        bhv_castle_secret_window_loop();
        assert(hidden && object.oAction == 2 && roomSpawns == 1);
        s32 brokenSounds = soundCount;
        castle_secret_window_bonk(&mario);
        bhv_castle_secret_window_loop();
        assert(soundCount == brokenSounds && roomSpawns == 1);
        object.behavior = bhvCastleSecretSign;
        object.oAction = 0;
        bhv_castle_secret_sign_loop();
        if (stage == 0) {
            assert(object.oBehParams2ndByte == DIALOG_CASTLE_SECRET_HOME);
            finish_secret_dialog();
            assert(!(save_file_get_castle_secret(0) & CASTLE_SECRET_AWAY));
            yoshi_credit_sign_area_exit();
            bhv_castle_secret_sign_loop();
            assert(save_file_get_castle_secret(0) & CASTLE_SECRET_AWAY);
        }
        u32 permanent = save_file_get_castle_secret(0);
        s32 writes = saveWrites;
        for (s32 read = 0; read < 4; read++) {
            assert(object.oBehParams2ndByte == DIALOG_CASTLE_SECRET_AWAY);
            assert(object.oInteractType == INTERACT_TEXT);
            finish_secret_dialog();
            pounding = read & 1;
            bhv_castle_secret_sign_loop();
        }
        assert(save_file_get_castle_secret(0) == permanent && saveWrites == writes);
        assert(save_file_get_castle_secret(1) == 0);
        yoshi_credit_sign_area_exit();
        assert(saveWrites == writes); // Repeated exits do not change the permanent note.
    }

    // Earlier saves may already contain the departure bit without the introduction.
    reset();
    save_file_set_castle_secret(0, CASTLE_SECRET_MOVED | CASTLE_SECRET_AWAY);
    assert(!(save_file_get_castle_secret(0) & CASTLE_SECRET_AWAY));
    object.behavior = bhvCastleSecretWindow;
    bhv_castle_secret_window_loop();
    object.behavior = bhvCastleSecretSign;
    object.oAction = 0;
    bhv_castle_secret_sign_loop();
    assert(object.oBehParams2ndByte == DIALOG_CASTLE_SECRET_HOME);
    finish_secret_dialog();
    assert(save_file_get_castle_secret(0) & CASTLE_SECRET_INTRO_READ);
    assert(!(save_file_get_castle_secret(0) & CASTLE_SECRET_AWAY));
    assert(object.oInteractType == 0);
    yoshi_credit_sign_area_exit();
    assert(save_file_get_castle_secret(0) & CASTLE_SECRET_AWAY);

    reset();
    for (s32 i = 0; i < 25; i++) {
        gSaveBuffer.files[0][0].courseStars[i] = 0x35;
    }
    gSaveBuffer.files[0][0].courseStars[24] = 5; // Actual ending progress, not star flags.
    save_file_set_castle_secret(0, 0x7F);
    for (s32 i = 0; i < 24; i++) {
        assert((gSaveBuffer.files[0][0].courseStars[i] & 0x7F) == 0x35);
    }
    assert(gSaveBuffer.files[0][0].courseStars[24] == (5 | SAVE_CAKE_CASTLE_SECRET_MOVED));
    assert(gSaveBuffer.files[0][0].courseStars[15] == 0x35); // Rainbow Ride's cannon is untouched.
    assert(save_file_get_castle_secret(0) == 0x7F && save_file_get_castle_secret(1) == 0);
    save_file_set_castle_secret(0, 0x7F);
    save_file_set_castle_secret(-1, 0x7F);
    save_file_set_castle_secret(4, 0x7F);
    assert(saveWrites == 0);
    save_file_flush_castle_secret();
    save_file_flush_castle_secret();
    assert(saveWrites == 1);
}

static void check_room_sign_sinking(void) {
    for (s32 mode = 0; mode < 3; mode++) {
        reset();
        save_file_set_castle_secret(0, CASTLE_SECRET_MOVED | CASTLE_SECRET_INTRO_READ
                                           | (mode == 1 ? CASTLE_SECRET_AWAY : 0));
        starCount = mode == 2 ? CASTLE_ROOF_COMPLETION_STARS : 0;
        object.behavior = bhvCastleSecretSign;
        object.oAction = 0;
        for (s32 impact = 0; impact < 3; impact++) {
            pounding = FALSE;
            bhv_castle_secret_sign_loop();
            pounding = TRUE;
            for (s32 frame = 0; frame < 10; frame++) {
                bhv_castle_secret_sign_loop();
                f32 depth = MIN(60.0f, impact * 30.0f + MIN(30.0f, (frame + 1) * 6.0f));
                assert(object.oSignpostSinkDepth == depth && object.oPosY == -depth);
                assert(object.hitboxHeight == 80.0f + depth);
            }
            assert(soundCount == MIN(impact + 1, 2));
            if (castle_secret_room_has_full_stars() || mode == 1) {
                assert(object.oInteractType == INTERACT_TEXT && object.oIntangibleTimer == 0);
            }
        }
        assert(saveWrites == 0);
    }
}

static void check_room_portrait_states(void) {
    reset();
    object.behavior = bhvCastleSecretRoom;
    bhv_castle_secret_room_loop();
#if defined(UNLOCK_ALL) || defined(COMPLETE_SAVE_FILE)
    assert(object.oAnimState == CASTLE_SECRET_PORTRAIT_PHYSICS_LAB);
#else
    assert(object.oAnimState == CASTLE_SECRET_PORTRAIT_BOW && !signSpawns);
    assert(object.collisionData == castle_secret_room_collision);
    save_file_set_castle_secret(0, CASTLE_SECRET_MOVED);
    bhv_castle_secret_room_loop();
    assert(object.oAnimState == CASTLE_SECRET_PORTRAIT_SMUG && signSpawns == 1);
    save_file_set_castle_secret(0, CASTLE_SECRET_INTRO_READ | CASTLE_SECRET_AWAY);
    bhv_castle_secret_room_loop();
    assert(object.oAnimState == CASTLE_SECRET_PORTRAIT_ABSENT && signSpawns == 1);
#endif
    starCount = CASTLE_ROOF_COMPLETION_STARS;
    bhv_castle_secret_room_loop();
    assert(object.oAnimState == CASTLE_SECRET_PORTRAIT_PHYSICS_LAB);
    starCount = 0;
    gDebugLevelSelect = TRUE;
    bhv_castle_secret_room_loop();
    assert(object.oAnimState == CASTLE_SECRET_PORTRAIT_ABSENT);
}

static void check_completed_room(void) {
    for (s32 debug = 0; debug < 2; debug++) {
        for (s32 previous = 0; previous < 2; previous++) {
            reset();
            starCount = debug ? 0 : CASTLE_ROOF_COMPLETION_STARS;
            gDebugLevelSelect = debug;
            if (debug) {
                object.behavior = bhvCastleSecretWindow;
                wall.object = &object;
                mario.wall = &wall;
                castle_secret_window_bonk(&mario);
                assert(object.oAction == 0);
                object.behavior = bhvCastleSecretRoom;
                bhv_castle_secret_room_loop();
                assert(object.oAnimState == CASTLE_SECRET_PORTRAIT_ABSENT && signSpawns == 0);
                continue;
            }
            if (previous) {
                save_file_set_castle_secret(0, CASTLE_SECRET_MOVED | CASTLE_SECRET_INTRO_READ
                                                   | CASTLE_SECRET_AWAY);
            }
            u32 progress = save_file_get_castle_secret(0);
            object.behavior = bhvCastleSecretWindow;
            wall.object = &object;
            mario.wall = &wall;
            castle_secret_window_bonk(&mario);
            bhv_castle_secret_window_loop();
            o = &child;
            bhv_castle_secret_room_loop();
            o = &object;
            bhv_castle_secret_window_loop();
            assert(hidden && roomSpawns == 1);
            object = child;
            object.parentObj = NULL;
            object.behavior = bhvCastleSecretRoom;
            bhv_castle_secret_room_loop();
            bhv_castle_secret_room_loop();
            assert(signSpawns == 1 && warpSpawns == 0);
            assert(object.collisionData == castle_secret_painting_room_collision);
            assert(object.oAnimState == CASTLE_SECRET_PORTRAIT_PHYSICS_LAB);
            object.behavior = bhvCastleSecretSign;
            object.oAction = 0;
            object.oOpacity = 255;
            bhv_castle_secret_sign_loop();
            assert(object.oInteractType == INTERACT_TEXT);
            assert(object.oBehParams2ndByte == DIALOG_CASTLE_SECRET_COMPLETE);
            finish_secret_dialog();
            assert(object.oAction == 2 && object.oOpacity == 247);
            assert(object.oInteractType == 0 && object.oIntangibleTimer == -1);
            for (s32 frame = 0; frame < 31; frame++) {
                bhv_castle_secret_sign_loop();
            }
            assert(deleted == 1 && object.oOpacity == 0);
            assert(save_file_get_castle_secret(0) == progress && saveWrites == 0);
            // Re-entering the area does not recreate Lil Board or change the painting.
            memset(&object, 0, sizeof(object));
            object.behavior = bhvCastleSecretRoom;
            bhv_castle_secret_room_loop();
            assert(signSpawns == 1 && object.oAnimState == CASTLE_SECRET_PORTRAIT_PHYSICS_LAB);
            // Reload another file without clearing this file's dismissal.
            castle_secret_sign_reset(1);
            bhv_castle_secret_room_loop();
            assert(signSpawns == 1);
            save_file_reload();
            bhv_castle_secret_room_loop();
            assert(signSpawns == 2);
            // A fresh boot starts with the transient mask cleared.
            sCompletionSignDismissedFileMask = 0;
            memset(&object, 0, sizeof(object));
            bhv_castle_secret_room_loop();
            assert(signSpawns == 3);
        }
    }
}

static void check_roof_intruder_crush(void) {
    for (s32 protection = 0; protection < 2; protection++) {
        reset();
        windowPresent = TRUE;
        bhv_yoshi_credit_sign_loop();
        assert(object.oBehParams2ndByte == DIALOG_YOSHI_CREDIT);
        intruderWindow.oAction = 2;
        bhv_yoshi_credit_sign_loop();
        assert(object.oBehParams2ndByte == DIALOG_CASTLE_BOARD_INTRUDER);
        mario.flags = protection ? 0xFFFFFFFF : 0;
        mario.invincTimer = protection ? 99 : 0;
        mario.healCounter = 20;
        mario.actionState = ACT_STATE_READING_SIGN_IN_DIALOG;
        camera.cutscene = CUTSCENE_NONE;
        act_reading_sign(&mario);
        bhv_yoshi_credit_sign_loop();
        assert(object.oAction == YOSHI_CREDIT_SIGN_ACT_CRUSH && spawned == 0 && timeStopped);
        for (s32 frame = 0; frame < 40 && !crushCakes; frame++) {
            mario.pos[0] += 100;
            mario.pos[2] -= 100;
            mario.pos[1] += 5;
            object.oTimer = frame;
            bhv_yoshi_credit_sign_loop();
            assert(object.oPosX == mario.pos[0] && object.oPosZ == mario.pos[2]);
        }
        assert(crushCakes == 1 && mario.health == 0xFF && mario.numLives == 0 && !timeStopped);
        assert(object.oAction == YOSHI_CREDIT_SIGN_ACT_CRUSHED && object.oFaceAnglePitch == 0x4000);
        assert(mario.action == ACT_STANDING_DEATH && mario.healCounter == 0);
        assert(mario.squishTimer == 0xFF && marioObject.header.gfx.scale[1] == 0.05f);
        assert(shakes == 1 && soundCount == 1 && drops == 1 && extinguishes == 1);
        assert(spawned == 0 && saveWrites == 0);
        assert((save_file_get_castle_secret(0) & CASTLE_SECRET_MOVED)
               && save_file_has_castle_wing_cap(0));
        assert(hidden && deleted == 1);
        assert(save_file_get_castle_secret(1) == 0 && !save_file_has_castle_wing_cap(1));
        for (s32 frame = 0; frame < 20; frame++) {
            bhv_yoshi_credit_sign_loop();
        }
        assert(crushCakes == 1 && soundCount == 1);
        assert(statsHits == 1 && statsHealthLost == 0x781);
        assert(statsDeaths == 1 && statsGameOvers == 1);
        save_file_reload();
        assert(saveWrites == 1 && save_file_has_castle_wing_cap(0));
        assert(save_file_get_castle_secret(0) & CASTLE_SECRET_MOVED);
#if !defined(UNLOCK_ALL) && !defined(COMPLETE_SAVE_FILE)
        bhv_yoshi_init();
        assert(spawned == 0); // The rooftop sign stays gone after Game Over reloads the save.
#endif
        reset();
        object.oAction = YOSHI_CREDIT_SIGN_ACT_CRUSH;
        object.oTimer = 60;
        object.oYoshiCreditAttackHeight = 1000.0f;
        object.oFaceAnglePitch = 0;
        timeStopped = TRUE;
        bhv_yoshi_credit_sign_loop();
        assert(crushCakes == 1 && mario.health == 0xFF && !timeStopped);
        assert(hidden && deleted == 1 && save_file_has_castle_wing_cap(0));
        assert(save_file_get_castle_secret(0) & CASTLE_SECRET_MOVED);
    }
}

static void check_lobby_rejection_sign_read(void) {
    reset();
    object.behavior = bhvCastleLobbyRejectionSign;
    rejectionFades = 0;
    mario.actionState = ACT_STATE_READING_SIGN_IN_DIALOG;
    camera.cutscene = 1;
    act_reading_sign(&mario);
    assert(rejectionFades == 0);
    camera.cutscene = CUTSCENE_NONE;
    act_reading_sign(&mario);
    assert(rejectionFades == 1 && !timeStopped && mario.action == ACT_IDLE);
}

static void check_lobby_floor_sign(void) {
    reset();
    object.behavior = bhvCastleLobbyFloorSign;
    object.oDistanceToMario = 301.0f;
    bhv_castle_lobby_floor_sign_loop();
    assert(hidden && object.oIntangibleTimer == -1 && object.oInteractType == 0);
    assert(readUpdates == 0 && collisionLoads == 0);
    object.oDistanceToMario = 300.0f;
    bhv_castle_lobby_floor_sign_loop();
    assert(!hidden && object.oIntangibleTimer == 0 && object.oInteractType == INTERACT_TEXT);
    assert(readUpdates == 1 && collisionLoads == 1);
    object.oDistanceToMario = 300.1f;
    bhv_castle_lobby_floor_sign_loop();
    assert(hidden && readUpdates == 1 && collisionLoads == 1);
    assert(!(save_file_get_castle_secret(0) & CASTLE_SECRET_LOBBY_SIGN_READ));

    // Completing the native sign dialogue consumes this sign once for the save file.
    mario.actionState = ACT_STATE_READING_SIGN_IN_DIALOG;
    camera.cutscene = CUTSCENE_NONE;
    act_reading_sign(&mario);
    assert(object.oAction == 1 && object.oInteractType == 0);
    bhv_castle_lobby_floor_sign_loop();
    assert(hidden && deleted == 1 && object.oIntangibleTimer == -1);
    assert(save_file_get_castle_secret(0) == CASTLE_SECRET_LOBBY_SIGN_READ);
    assert(save_file_get_castle_secret(1) == 0);
    save_file_flush_castle_secret();
    save_file_reload();
    assert(save_file_get_castle_secret(0) == CASTLE_SECRET_LOBBY_SIGN_READ);
    object.oAction = 0;
    object.oDistanceToMario = 0.0f;
    bhv_castle_lobby_floor_sign_loop();
    assert(hidden && object.oInteractType == 0 && readUpdates == 1);

    // Motos's completed dialogue uses a non-star high bit and survives a reload.
    gSaveBuffer.files[0][0].courseStars[COURSE_NUM_TO_INDEX(COURSE_BITS)] = 0x35;
    save_file_set_castle_secret(0, CASTLE_SECRET_MOTOS_TALKED);
    save_file_flush_castle_secret();
    save_file_reload();
    assert(save_file_get_castle_secret(0) & CASTLE_SECRET_MOTOS_TALKED);
    assert(!(save_file_get_castle_secret(1) & CASTLE_SECRET_MOTOS_TALKED));
    assert((gSaveBuffer.files[0][0].courseStars[COURSE_NUM_TO_INDEX(COURSE_BITS)] & 0x7F) == 0x35);
}

static void check_secret_cannon_storage(void) {
    const s32 cannonCourses[] = { COURSE_RR, COURSE_WMOTR };
    for (s32 file = 0; file < NUM_SAVE_FILES; file++) {
        for (s32 cannon = 0; cannon < ARRAY_COUNT(cannonCourses); cannon++) {
            reset();
            gCurrSaveFileNum = file + 1;
            gCurrCourseNum = cannonCourses[cannon];
            save_file_set_cannon_unlocked();
            assert(save_file_is_cannon_unlocked());
            // Previously these cannon unlocks consumed unrelated encounters.
            assert(save_file_get_castle_secret(file) == 0);
            save_file_do_save(file);
            save_file_reload();
            assert(save_file_get_castle_secret(file) == 0);
            assert(save_file_is_cannon_unlocked());

            reset();
            gCurrSaveFileNum = file + 1;
            gCurrCourseNum = cannonCourses[cannon];
            // In the reverse direction, every secret must leave both cannons closed.
            gSaveBuffer.files[file][0].courseStars[24] = 9;
            save_file_set_castle_secret(file, 0x1FF);
            assert(save_file_get_castle_secret(file) == 0x1FF);
            assert(gSaveBuffer.files[file][0].courseStars[gCurrCourseNum] == 0);
#ifndef UNLOCK_ALL
            assert(!save_file_is_cannon_unlocked());
#endif
            assert((gSaveBuffer.files[file][0].courseStars[24] & SAVE_CAKE_CASTLE_LOBBY_PROGRESS_MASK)
                   == 9);
            save_file_flush_castle_secret();
            save_file_reload();
            assert(save_file_get_castle_secret(file) == 0x1FF);
            assert(gSaveBuffer.files[file][0].courseStars[gCurrCourseNum] == 0);
            assert(save_file_get_castle_secret((file + 1) % NUM_SAVE_FILES) == 0);
        }
    }

    reset();
    gCurrCourseNum = COURSE_WMOTR;
    save_file_set_cannon_unlocked();
    gCurrCourseNum = COURSE_RR;
    save_file_set_cannon_unlocked();
    // Preserve recognized legacy rooftop progress without copying ambiguous cannon bits.
    gSaveBuffer.files[0][0].courseStars[24] = SAVE_CAKE_CASTLE_WING_CAP_UNLOCKED | 6;
    assert(save_file_get_castle_secret(0) == CASTLE_SECRET_MOVED);
    gSaveBuffer.files[0][0].courseStars[24] = 6;
    gSaveBuffer.files[0][0].courseStars[COURSE_NUM_TO_INDEX(COURSE_COTMC)] = 0x80;
    assert(save_file_get_castle_secret(0) == (CASTLE_SECRET_MOVED | CASTLE_SECRET_INTRO_READ));
    save_file_set_castle_secret(0, CASTLE_SECRET_LOBBY_SIGN_READ);
    save_file_unlock_castle_wing_cap(0);
    save_file_flush_castle_secret();
    save_file_reload();
    assert(save_file_has_castle_wing_cap(0));
    assert(save_file_get_castle_secret(0) & CASTLE_SECRET_LOBBY_SIGN_READ);
    assert(gSaveBuffer.files[0][0].courseStars[COURSE_RR] == 0x80);
    assert(gSaveBuffer.files[0][0].courseStars[COURSE_WMOTR] == 0x80);
    assert((gSaveBuffer.files[0][0].courseStars[24] & SAVE_CAKE_CASTLE_LOBBY_PROGRESS_MASK) == 6);
}

static void check_developer_cheating_gate(void) {
    reset();
    starCount = CASTLE_ROOF_COMPLETION_STARS;
    bhv_yoshi_init();
    assert(object.activeFlags == 0 && spawned == 0);
    reset();
    starCount = CASTLE_ROOF_COMPLETION_STARS;
    object.oBehParams2ndByte = MUSHROOM_BP_REQUIRES_CASTLE_COMPLETION;
    bhv_1up_init();
    assert(object.activeFlags == 0);
    reset();
    starCount = CASTLE_ROOF_COMPLETION_STARS;
    object.behavior = bhvCastleSecretRoom;
    bhv_castle_secret_room_loop();
    assert(object.oAnimState == CASTLE_SECRET_PORTRAIT_ABSENT && signSpawns == 0);
    save_file_unlock_castle_wing_cap(0);
    save_file_set_castle_secret(0, CASTLE_SECRET_MOVED);
    assert(!save_file_has_castle_wing_cap(0) && !save_file_get_castle_secret(0));
}
int main(void) {
#if defined(UNLOCK_ALL) || defined(COMPLETE_SAVE_FILE)
    check_developer_cheating_gate();
    return 0;
#endif
    gMainMenuDataModified = TRUE;
    save_file_reload();
    assert(gMainMenuDataModified == TRUE);
    check_secret_cannon_storage();
    check_lobby_rejection_sign_read();
    check_lobby_floor_sign();
#if !defined(UNLOCK_ALL) && !defined(COMPLETE_SAVE_FILE)
    check_secret_room();
#endif
    check_completed_room();
    check_room_portrait_states();
    check_room_sign_sinking();
    check_gate();
    check_sign();
    check_roof_intruder_crush();
    check_mushroom();
    check_homing();
    check_exit_and_regular_mushroom_gate();
    check_existing_completion_mushroom();
    check_saved_cap_box_scope();
    return 0;
}
