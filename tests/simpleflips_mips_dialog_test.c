#include <assert.h>
#define game_options_secrets_blocked() 0
#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include "dialog_ids.h"
#include "mario_animation_ids.h"

typedef int32_t s32;
typedef uint32_t u32;
typedef int16_t s16;
#ifdef TEST_MOTOS
typedef uint16_t ModelID16;
typedef uintptr_t BehaviorScript;
typedef float f32;
#define MODEL_WHITE_PARTICLE_SMALL 1
#define MODEL_WATER_SPLASH 2
#define SOUND_OBJ_DIVING_IN_WATER 3
#endif
#define FALSE 0
#define TRUE 1
#define UNUSED __attribute__((unused))
#define FALL_THROUGH __attribute__((fallthrough))
#define BUGFIX_DIALOG_TIME_STOP

#include "dialog_definitions.inc"

struct Object {
    struct {
        struct {
            float pos[3];
            s16 angle[3];
        } gfx;
    } header;
    s32 oAction, oInteractStatus, oInteractType, oDialogState, oDialogResponse;
    s32 activeFlags, intangible, hidden, deleted;
};

struct MarioBodyState {
    s16 headAngle[3];
};

struct MarioState {
    u32 action, actionArg, flags;
    s32 actionState, actionTimer, health, capPickupLockFrames;
    u32 capPickupLockStartFrame;
    float pos[3];
    s16 faceAngle[3];
    struct Object *usedObj, *heldObj, *marioObj;
    struct MarioBodyState *marioBodyState;
};

static struct Object mips, marioObject, heldObject;
static struct Object *gCurrentObject = &mips;
#define o gCurrentObject
static struct MarioState mario;
static struct MarioState *gMarioState = &mario;
static struct MarioBodyState body;
static s32 gTimeStopState, gDialogResponse, dialogID = DIALOG_NONE, openings;

static s32 mario_ready_to_speak(void) {
    return mario.action == ACT_IDLE || mario.action == ACT_HOLD_IDLE;
}

static s32 set_mario_action(struct MarioState *m, u32 action, u32 actionArg) {
    m->action = action;
    m->actionArg = actionArg;
    m->actionState = 0;
    m->actionTimer = 0;
    return TRUE;
}

static s16 approach_angle(s16 current, s16 target, s16 step) {
    (void) current;
    (void) step;
    return target;
}

static s16 mario_obj_angle_to_object(struct MarioState *m, struct Object *object) {
    assert(m == &mario && object == &mips);
    return 0;
}

static void set_mario_animation(struct MarioState *m, s32 animation) {
    assert(m == &mario);
    assert(animation
           == (m->heldObj == NULL ? MARIO_ANIM_FIRST_PERSON : MARIO_ANIM_IDLE_WITH_LIGHT_OBJ));
}

static void vec3f_copy(float dest[3], const float src[3]) {
    memcpy(dest, src, 3 * sizeof(float));
}

static void vec3s_set(s16 dest[3], s16 x, s16 y, s16 z) {
    dest[0] = x;
    dest[1] = y;
    dest[2] = z;
}

static void create_dialog_box(s32 id) {
#ifdef TEST_MOTOS
    assert(id == DIALOG_SIMPLEFLIPS_MOTOS && dialogID == DIALOG_NONE);
#else
    assert(id == DIALOG_SIMPLEFLIPS_MIPS && dialogID == DIALOG_NONE);
#endif
    dialogID = id;
    openings++;
}

static void create_dialog_box_with_response(s32 id) {
    (void) id;
    assert(FALSE);
}

static s32 get_dialog_id(void) {
    return dialogID;
}

static void cur_obj_init_animation(s32 animation) {
#ifdef TEST_MOTOS
    assert(animation == 8);
#else
    assert(animation == 0);
#endif
}

static void cur_obj_become_intangible(void) {
    o->intangible = TRUE;
}

#ifdef TEST_MOTOS
static s32 roomActive = TRUE, sSimpleFlipsGameOver, splashes, droplets, sounds;
static s32 particleFailure;
static u32 savedFlags;
static const BehaviorScript bhvJrbSimpleFlipsMotosDroplet[1], bhvJrbSimpleFlipsMotosSplash[1];

static s32 jrb_simpleflips_room_active(void) {
    return roomActive;
}
static void cur_obj_scale(f32 scale) {
    assert(scale == 2.0f);
}
static void cur_obj_hide(void) {
    o->hidden = TRUE;
}
static void cur_obj_unhide(void) {
    o->hidden = FALSE;
}
static void cur_obj_become_tangible(void) {
    o->intangible = FALSE;
}
static int gCurrSaveFileNum = 1;
static int motosTalkSaved, motosSaveWrites;
static void save_file_set_castle_secret(int file, u32 flag) {
    assert(file == 0 && flag == CASTLE_SECRET_MOTOS_TALKED);
    motosTalkSaved = TRUE;
}
static void save_file_do_save(int file) {
    assert(file == 0 && motosTalkSaved);
    motosSaveWrites++;
}

static void save_file_clear_flags(u32 flags) {
    savedFlags &= ~flags;
}

static s32 particle_budget_burst_count(enum ParticlePriority priority, s32 requested) {
    assert(priority == PARTICLE_PRIORITY_DROPLET && requested == 6);
    return particleFailure ? 1 : requested;
}

static struct Object *try_spawn_particle(struct Object *parent, s32 model,
                                         const BehaviorScript *behavior,
                                         enum ParticlePriority priority) {
    assert(parent == o && model == MODEL_WATER_SPLASH);
    assert(behavior == bhvJrbSimpleFlipsMotosSplash && priority == PARTICLE_PRIORITY_FEEDBACK);
    assert(!(gTimeStopState & TIME_STOP_ENABLED));
    splashes++;
    return particleFailure ? NULL : &heldObject;
}

static struct Object *spawn_water_droplet(struct Object *parent, struct WaterDropletParams *params) {
    assert(parent == o && params->behavior == bhvJrbSimpleFlipsMotosDroplet);
    assert(params->model == MODEL_WHITE_PARTICLE_SMALL);
    assert(!(params->flags & WATER_DROPLET_FLAG_SET_Y_TO_WATER_LEVEL));
    droplets++;
    return particleFailure ? NULL : &heldObject;
}

static void create_sound_spawner(s32 sound) {
    assert(sound == SOUND_OBJ_DIVING_IN_WATER);
    sounds++;
}

static void obj_mark_for_deletion(struct Object *object) {
    assert(object == o && !(gTimeStopState & TIME_STOP_ENABLED));
    object->deleted = TRUE;
}
#endif

#include "dialog_actual.inc"

static void update_conversation(void) {
    // PLAYER runs before GENACTOR; dialogue time stop allows both to update.
    if (mario.action == ACT_READING_NPC_DIALOG) {
        act_reading_npc_dialog(&mario);
    }
#ifdef TEST_MOTOS
    bhv_jrb_simpleflips_motos_loop();
#else
    bhv_jrb_simpleflips_mips_loop();
#endif
}

static void check_release(s32 holding, s32 capInHand, s32 regainCap) {
    memset(&mips, 0, sizeof(mips));
    memset(&mario, 0, sizeof(mario));
    memset(&body, 0, sizeof(body));
    mario.marioObj = &marioObject;
    mario.marioBodyState = &body;
    mario.action = holding ? ACT_HOLD_IDLE : ACT_IDLE;
    mario.heldObj = holding ? &heldObject : NULL;
    mario.flags = capInHand ? MARIO_CAP_IN_HAND : 0;
    mario.health = 0x880;
    gTimeStopState = TIME_STOP_UNKNOWN_0;
    dialogID = DIALOG_NONE;
    openings = 0;
    mips.activeFlags = ACTIVE_FLAG_ACTIVE;
    mips.oInteractType = INTERACT_TEXT;
    mips.oInteractStatus = INT_STATUS_INTERACTED;
#ifdef TEST_MOTOS
    splashes = droplets = sounds = 0;
    motosTalkSaved = FALSE;
    motosSaveWrites = 0;
#endif
    (void) regainCap;

    for (s32 frame = 0; frame < 40 && dialogID == DIALOG_NONE; frame++) {
        update_conversation();
    }
    assert(openings == 1 && mario.action == ACT_READING_NPC_DIALOG);
    assert(mario.actionState == 8 && mario.usedObj == &mips);
    assert(gTimeStopState & TIME_STOP_ENABLED);
    assert(mips.activeFlags & ACTIVE_FLAG_INITIATED_TIME_STOP);

    for (s32 frame = 0; frame < 20; frame++) {
        update_conversation();
        assert(mario.actionState == 8 && mips.oAction == 1);
    }
#ifdef TEST_MOTOS
    assert(splashes == 0 && sounds == 0 && !mips.deleted);
    assert(!motosTalkSaved && motosSaveWrites == 0);
    if (regainCap) {
        mario.flags |= MARIO_NORMAL_CAP | MARIO_CAP_ON_HEAD;
    }
#endif
    dialogID = DIALOG_NONE; // The UI closes the final page.
    for (s32 frame = 0; frame < 40; frame++) {
        update_conversation();
    }
    u32 expectedAction;
    if (capInHand) {
        expectedAction = ACT_PUTTING_ON_CAP;
    } else {
        expectedAction = holding ? ACT_HOLD_IDLE : ACT_IDLE;
    }
    assert(mario.action == expectedAction);
    assert(body.headAngle[0] == 0);
    assert(gTimeStopState == TIME_STOP_UNKNOWN_0);
#ifdef TEST_MOTOS
    assert(mips.activeFlags == (ACTIVE_FLAG_ACTIVE | ACTIVE_FLAG_DITHERED_ALPHA));
    assert(mips.hidden && mips.deleted && splashes == 1 && sounds == 1);
    assert(motosTalkSaved && motosSaveWrites == 1);
    assert(droplets == (particleFailure ? 1 : 6));
#else
    assert(mips.activeFlags == ACTIVE_FLAG_ACTIVE);
#endif
    assert(mips.oAction == 2 && mips.oInteractType == INTERACT_NONE && mips.intangible);

    for (s32 frame = 0; frame < 10; frame++) {
        mips.oInteractStatus = INT_STATUS_INTERACTED;
        update_conversation();
    }
    assert(openings == 1 && mario.action != ACT_READING_NPC_DIALOG);
#ifdef TEST_MOTOS
    assert(splashes == 1 && sounds == 1 && mips.deleted);
#endif
    assert(gTimeStopState == TIME_STOP_UNKNOWN_0);
}

#ifdef TEST_MOTOS
static void check_cap_visibility(void) {
    memset(&mips, 0, sizeof(mips));
    memset(&mario, 0, sizeof(mario));
    roomActive = TRUE;
    const u32 caps[] = { MARIO_NORMAL_CAP, MARIO_WING_CAP, MARIO_METAL_CAP, MARIO_VANISH_CAP,
                         MARIO_NORMAL_CAP | MARIO_CAP_IN_HAND };
    for (u32 i = 0; i < sizeof(caps) / sizeof(caps[0]); i++) {
        mario.flags = caps[i];
        mips.oInteractStatus = INT_STATUS_INTERACTED;
        bhv_jrb_simpleflips_motos_loop();
        assert(mips.hidden && mips.intangible && mips.oInteractType == INTERACT_NONE);
        assert(mips.oAction == 0 && mips.oInteractStatus == INT_STATUS_NONE);
        mario.flags = 0;
        bhv_jrb_simpleflips_motos_loop();
        assert(!mips.hidden && !mips.intangible && mips.oInteractType == INTERACT_TEXT);
    }
    roomActive = FALSE;
    bhv_jrb_simpleflips_motos_loop();
    assert(mips.hidden && mips.intangible);
    roomActive = TRUE;
    sSimpleFlipsGameOver = TRUE;
    bhv_jrb_simpleflips_motos_loop();
    assert(mips.hidden && mips.intangible);
    sSimpleFlipsGameOver = FALSE;
}

static void check_cap_restoration(void) {
    for (u32 special = 0; special < 16; special += 2) {
        mario.flags = special | MARIO_CAP_IN_HAND | MARIO_TELEPORTING;
        mario.capPickupLockFrames = 100;
        mario.capPickupLockStartFrame = 900;
        savedFlags = SAVE_FLAG_CAP_ON_GROUND | SAVE_FLAG_CAP_ON_KLEPTO | SAVE_FLAG_CAP_ON_UKIKI
                     | SAVE_FLAG_CAP_ON_MR_BLIZZARD | SAVE_FLAG_HAVE_WING_CAP;
        jrb_simpleflips_restore_cap(&mario);
        assert(mario.flags == (special | MARIO_NORMAL_CAP | MARIO_CAP_ON_HEAD | MARIO_TELEPORTING));
        assert(!mario.capPickupLockFrames && !mario.capPickupLockStartFrame);
        assert(savedFlags == SAVE_FLAG_HAVE_WING_CAP);
        mario.flags = 0;
        bhv_jrb_simpleflips_motos_loop();
        assert(!mips.hidden); // Entry restoration does not continuously force the cap back on.
    }
}
#endif

int main(void) {
#ifdef TEST_MOTOS
    check_cap_visibility();
    check_cap_restoration();
#endif
    check_release(FALSE, FALSE, FALSE);
    check_release(TRUE, FALSE, FALSE);
    check_release(FALSE, TRUE, FALSE);
#ifdef TEST_MOTOS
    check_release(FALSE, FALSE, TRUE);
    particleFailure = TRUE;
    check_release(FALSE, FALSE, FALSE);
#endif
    return 0;
}
