#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include "level_table.h"

typedef int32_t s32;
typedef uint32_t u32;
typedef int16_t s16;
typedef uint8_t u8;
typedef float f32;
#define TRUE 1
#define FALSE 0
#define UNUSED __attribute__((unused))
#define PUPPYPRINT_DEBUG
#define ENABLE_LIVES
#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define CLAMP(x, lo, hi) MIN(MAX(x, lo), hi)
#define GET_BPARAM2(x) (((x) >> 16) & 0xff)
#define COURSE_NONE 0
#define SEQ_PLAYER_SFX 2
#define SOUND_BANKS_ALL 0xffff
#define SOUND_BANKS_DISABLED_AFTER_CREDITS 1
#define assertf(condition, ...) assert(condition)

enum {
    WARP_TRANSITION_FADE_INTO_STAR,
    WARP_TRANSITION_FADE_INTO_COLOR,
    WARP_TRANSITION_FADE_INTO_MARIO,
    WARP_TRANSITION_FADE_INTO_BOWSER,
    WARP_TRANSITION_FADE_INTO_CIRCLE,
    SOUND_MENU_BOWSER_LAUGH,
    SOUND_MENU_STAR_SOUND
};
#include "confinement_definitions.inc"

struct Surface {
    int force;
};
struct Object {
    u32 oBehParams;
    struct {
        struct {
            float pos[3];
        } gfx;
    } header;
};
struct MarioState {
    u32 action, actionArg, flags;
    s32 health, hurtCounter, healCounter, invincTimer, numLives, yoshiCoinDeathPending;
    s32 poisonMushroomDeathPending;
    float pos[3], vel[3], forwardVel, slideVelX, slideVelZ;
    struct Surface *floor;
    struct Object *usedObj, *marioObj;
};
struct Area {
    int index;
};
struct ObjectWarpNode {
    struct {
        int destLevel, destArea, destNode;
    } node;
};
struct CreditsEntry {
    int levelNum, areaIndex, actNum;
};
static struct CreditsEntry sCreditsSequence[3], *gCurrCreditsEntry;
static struct Object marioObject;
static struct MarioState mario, *gMarioState = &mario;
static struct Area area, *gCurrentArea = &area;
static struct WarpDest sWarpDest;
static s32 gCurrLevelNum, gCurrAreaIndex, gCurrActNum, gSavedCourseNum;
static s32 sSimpleFlipsGameOver, sHealOnCourseEntry, gDebugLevelSelect;
static s32 secretsBlocked;
#define game_options_secrets_blocked() secretsBlocked
static s32 gPuppyWarp, gPuppyWarpArea, sCurrPlayMode, sSpecialWarpDest, sTransitionTimer;
static s32 sDelayedWarpOp, sDelayedWarpArg, sDelayedWarpTimer, sSourceWarpNodeId;
static void *gCurrDemoInput, *gGlobalSoundSource;
static u8 gLevelToCourseNumTable[LEVEL_COUNT];
static int transitions, fades, saves, dialogResets;

static void mario_set_forward_vel(struct MarioState *m, float speed) {
    m->forwardVel = speed;
    m->slideVelX = m->slideVelZ = speed;
    m->vel[0] = m->vel[2] = speed;
}

static void vec3f_set(float pos[3], float x, float y, float z) {
    pos[0] = x;
    pos[1] = y;
    pos[2] = z;
}

static void vec3f_copy(float dest[3], const float src[3]) {
    memcpy(dest, src, 3 * sizeof(float));
}

static void set_mario_action(struct MarioState *m, u32 action, u32 arg) {
    m->action = action;
    m->actionArg = arg;
}

static void set_play_mode(s16 mode) {
    sCurrPlayMode = mode;
}

static void fadeout_music(UNUSED s32 duration) {
    fades++;
}

static void play_transition(UNUSED int type, UNUSED int frames, UNUSED int r, UNUSED int g,
                            UNUSED int b) {
    transitions++;
}

static void level_set_transition(s16 timer, UNUSED void *update) {
    sTransitionTimer = timer;
}

static void play_sound(UNUSED int sound, UNUSED void *source) {
}

static s32 mario_character_is_unlock_death(UNUSED struct MarioState *m) {
    return FALSE;
}

static struct ObjectWarpNode *area_get_warp_node(UNUSED s32 node) {
    return NULL;
}

static s32 music_unchanged_through_warp(UNUSED int node) {
    return FALSE;
}

static void reset_dialog_render_state(void) {
    dialogResets++;
}

static void sound_banks_enable(UNUSED int player, UNUSED int banks) {
}

static void sound_banks_disable(UNUSED int player, UNUSED int banks) {
}

static void save_file_reload(void) {
    saves++;
}

static void save_file_arm_he_thicc(void) {
    assert(FALSE);
}

static void check_if_should_set_warp_checkpoint(UNUSED void *node) {
}

static void game_timing_warp(s32 level, s32 exitCourse) {
    (void) level;
    (void) exitCourse;
}
static void game_timing_end_course(void) {
}

// Statistics persistence is outside the room confinement and exit routing fixture.
static void player_stats_warp(UNUSED s16 destination, UNUSED s32 exitCourse) {
}

static void player_stats_death(UNUSED s32 gameOver, UNUSED s32 pit) {
}

#include "confinement_actual.inc"

static void begin(void) {
    memset(&mario, 0, sizeof(mario));
    memset(&sWarpDest, 0, sizeof(sWarpDest));
    mario.health = 0x880;
    mario.action = ACT_IDLE;
    mario.marioObj = &marioObject;
    gCurrLevelNum = LEVEL_JRB;
    area.index = gCurrAreaIndex = JRB_SIMPLEFLIPS_AREA;
    sSimpleFlipsGameOver = FALSE;
    secretsBlocked = FALSE;
    sCurrPlayMode = PLAY_MODE_NORMAL;
    sDelayedWarpOp = WARP_OP_NONE;
    sSpecialWarpDest = WARP_SPECIAL_NONE;
    sTransitionTimer = 0;
    gPuppyWarp = gPuppyWarpArea = 0;
    transitions = fades = saves = dialogResets = 0;
}

static void check_bounds(void) {
    const struct {
        float pos[3];
        float forwardVel, yVel;
    } attempts[] = {
        { { 30000, 100, 0 }, 0, -10000 },    { { -30000, 100, 0 }, 0, -10000 },
        { { 0, 100, 30000 }, 0, -10000 },    { { 0, 100, -30000 }, 0, -10000 },
        { { 0, -10000, 0 }, 30000, 0 },      { { 0, 30000, 0 }, 30000, 0 },
        { { 30000, -10000, -30000 }, 0, 0 },
    };
    for (unsigned i = 0; i < sizeof(attempts) / sizeof(attempts[0]); i++) {
        for (s32 cap = 0; cap < 4; cap++) {
            begin();
            const u32 caps[] = { 0, MARIO_VANISH_CAP, MARIO_WING_CAP, MARIO_METAL_CAP };
            mario.flags = caps[cap];
            vec3f_copy(mario.pos, attempts[i].pos);
            mario_set_forward_vel(&mario, 30000);
            mario.vel[1] = -10000;
            jrb_simpleflips_confine_mario(&mario);
            assert(mario.pos[0] >= -850 && mario.pos[0] <= 850);
            assert(mario.pos[1] >= 0 && mario.pos[1] <= 840);
            assert(mario.pos[2] >= -850 && mario.pos[2] <= 850);
            assert(memcmp(mario.pos, marioObject.header.gfx.pos, sizeof(mario.pos)) == 0);
            assert(mario.forwardVel == attempts[i].forwardVel);
            assert(mario.slideVelX == attempts[i].forwardVel
                   && mario.slideVelZ == attempts[i].forwardVel);
            assert(mario.vel[0] == attempts[i].forwardVel && mario.vel[2] == attempts[i].forwardVel);
            assert(mario.vel[1] == attempts[i].yVel);
        }
    }
    begin();
    vec3f_set(mario.pos, 100, 200, 300);
    mario_set_forward_vel(&mario, 42);
    mario.vel[1] = 12;
    jrb_simpleflips_confine_mario(&mario);
    assert(mario.forwardVel == 42 && mario.vel[1] == 12);
    mario.health = 0x780;
    mario.hurtCounter = 20;
    mario.healCounter = 4;
    jrb_simpleflips_confine_mario(&mario);
    assert(mario.health == 0x880 && mario.hurtCounter == 0 && mario.healCounter == 0);
    assert(mario.forwardVel == 42 && mario.vel[1] == 12 && mario.action == ACT_IDLE);
    mario.action = ACT_DEBUG_FREE_MOVE;
    jrb_simpleflips_confine_mario(&mario);
    assert(mario.action == ACT_IDLE);
    mario.health = 0xff;
    mario.hurtCounter = 8;
    mario.action = ACT_HARD_BACKWARD_AIR_KB;
    mario.actionArg = 1;
    jrb_simpleflips_confine_mario(&mario);
    assert(mario.health == 0x880 && mario.hurtCounter == 0);
    assert(mario.action == ACT_IDLE && mario.actionArg == 0);
    sSimpleFlipsGameOver = TRUE;
    mario.health = 0xff;
    mario.action = ACT_HARD_BACKWARD_AIR_KB;
    jrb_simpleflips_confine_mario(&mario);
    assert(mario.health == 0xff && mario.action == ACT_HARD_BACKWARD_AIR_KB);
    gCurrAreaIndex = area.index = 2;
    vec3f_set(mario.pos, -20000, -5000, 20000);
    jrb_simpleflips_confine_mario(&mario);
    assert(mario.pos[0] == -20000 && mario.pos[1] == -5000 && mario.pos[2] == 20000);
}

static void check_warps(void) {
    begin();
    mario.invincTimer = 14;
    for (s32 op = WARP_OP_NONE; op <= WARP_OP_DEMO_END; op++) {
        assert(level_trigger_warp(&mario, op) == 0);
        assert(sDelayedWarpOp == WARP_OP_NONE && mario.invincTimer == 14);
    }
    initiate_warp(LEVEL_CASTLE, 1, WARP_NODE_MAIN_ENTRY, WARP_FLAG_EXIT_COURSE);
    initiate_warp(LEVEL_JRB, 2, WARP_NODE_MAIN_ENTRY, WARP_FLAGS_NONE);
    initiate_warp(LEVEL_JRB, 3, WARP_NODE_MAIN_ENTRY, WARP_FLAGS_NONE);
    assert(sWarpDest.type == WARP_TYPE_NOT_WARPING);
    for (s32 special = WARP_SPECIAL_LEVEL_SELECT; special <= WARP_SPECIAL_NONE; special++) {
        warp_special(special);
        fade_into_special_warp(special, 1);
    }
    assert(sCurrPlayMode == PLAY_MODE_NORMAL && sTransitionTimer == 0);
    assert(transitions == 0 && fades == 0);
    gPuppyWarp = LEVEL_BOB;
    gPuppyWarpArea = 1;
    sDelayedWarpOp = WARP_OP_DEATH;
    sDelayedWarpTimer = 1;
    initiate_delayed_warp();
    assert(!gPuppyWarp && !gPuppyWarpArea && sDelayedWarpOp == WARP_OP_NONE);
    assert(sCurrPlayMode == PLAY_MODE_NORMAL && dialogResets == 0);

    // Toad's death remains a game over even with debug/Yoshi exit requests.
    sSimpleFlipsGameOver = TRUE;
    mario.health = 0xff;
    mario.numLives = 99;
    mario.yoshiCoinDeathPending = TRUE;
    gDebugLevelSelect = TRUE;
    assert(level_trigger_warp(&mario, WARP_OP_DEATH) == 48);
    assert(sDelayedWarpOp == WARP_OP_GAME_OVER && mario.numLives == 0);
    for (s32 frame = 0; frame < 48; frame++) {
        gPuppyWarp = LEVEL_BOB;
        gPuppyWarpArea = 1;
        initiate_delayed_warp();
        assert(!gPuppyWarp && !gPuppyWarpArea);
    }
    assert(saves == 1 && sCurrPlayMode == PLAY_MODE_CHANGE_LEVEL);
    assert(sSpecialWarpDest == WARP_SPECIAL_MARIO_HEAD_DIZZY);
    assert(sWarpDest.type == WARP_TYPE_NOT_WARPING);

    // Ordinary areas retain direct warps and can still enter the encounter.
    begin();
    gCurrAreaIndex = area.index = 2;
    initiate_warp(LEVEL_JRB, 3, WARP_NODE_MAIN_ENTRY, WARP_FLAGS_NONE);
    assert(sWarpDest.type == WARP_TYPE_CHANGE_AREA && sWarpDest.areaIdx == 3);
    initiate_warp(LEVEL_CASTLE, 1, WARP_NODE_MAIN_ENTRY, WARP_FLAGS_NONE);
    assert(sWarpDest.type == WARP_TYPE_CHANGE_LEVEL && sWarpDest.levelNum == LEVEL_CASTLE);
    fade_into_special_warp(WARP_SPECIAL_LEVEL_SELECT, 1);
    assert(sCurrPlayMode == PLAY_MODE_CHANGE_LEVEL && sTransitionTimer == 30);
    assert(sSpecialWarpDest == WARP_SPECIAL_LEVEL_SELECT && transitions == 1);
}

static void check_blocked_visit_exits(void) {
    begin();
    secretsBlocked = TRUE;
    initiate_warp(LEVEL_CASTLE, 1, WARP_NODE_MAIN_ENTRY, WARP_FLAG_EXIT_COURSE);
    assert(sWarpDest.type == WARP_TYPE_CHANGE_LEVEL && sWarpDest.levelNum == LEVEL_CASTLE);

    begin();
    secretsBlocked = TRUE;
    initiate_warp(LEVEL_JRB, 2, WARP_NODE_MAIN_ENTRY, WARP_FLAGS_NONE);
    assert(sWarpDest.type == WARP_TYPE_CHANGE_AREA && sWarpDest.areaIdx == 2);

    begin();
    secretsBlocked = TRUE;
    warp_special(WARP_SPECIAL_LEVEL_SELECT);
    assert(sCurrPlayMode == PLAY_MODE_CHANGE_LEVEL);
    assert(sSpecialWarpDest == WARP_SPECIAL_LEVEL_SELECT);

    begin();
    secretsBlocked = TRUE;
    fade_into_special_warp(WARP_SPECIAL_LEVEL_SELECT, 1);
    assert(sCurrPlayMode == PLAY_MODE_CHANGE_LEVEL && transitions == 1);

    begin();
    secretsBlocked = TRUE;
    gPuppyWarp = LEVEL_BOB;
    gPuppyWarpArea = 1;
    initiate_delayed_warp();
    assert(sWarpDest.type == WARP_TYPE_CHANGE_LEVEL && sWarpDest.levelNum == LEVEL_BOB);

    // Once Toad makes contact, blocking secrets cannot cancel his game over.
    begin();
    secretsBlocked = TRUE;
    sSimpleFlipsGameOver = TRUE;
    mario.health = 0xff;
    mario.numLives = 99;
    initiate_warp(LEVEL_CASTLE, 1, WARP_NODE_MAIN_ENTRY, WARP_FLAG_EXIT_COURSE);
    assert(sWarpDest.type == WARP_TYPE_NOT_WARPING);
    assert(level_trigger_warp(&mario, WARP_OP_DEATH) == 48);
    assert(sDelayedWarpOp == WARP_OP_GAME_OVER && mario.numLives == 0);
    for (s32 frame = 0; frame < 48; frame++) {
        initiate_delayed_warp();
    }
    assert(sSpecialWarpDest == WARP_SPECIAL_MARIO_HEAD_DIZZY && saves == 1);
}

static void check_course_entry_refill(void) {
    const struct {
        s32 level;
        s32 ordinaryRefill;
    } origins[] = { { LEVEL_CASTLE, TRUE },
                    { LEVEL_CASTLE_COURTYARD, TRUE },
                    { LEVEL_CASTLE_GROUNDS, TRUE },
                    { LEVEL_WF, FALSE },
                    { LEVEL_BOB, FALSE } };
    for (u32 i = 0; i < sizeof(origins) / sizeof(origins[0]); i++) {
        begin();
        gCurrLevelNum = origins[i].level;
        area.index = 1;
        gLevelToCourseNumTable[LEVEL_BOB - 1] = 1;
        initiate_warp(LEVEL_BOB, 1, WARP_NODE_MAIN_ENTRY, WARP_FLAGS_NONE);
        assert(sHealOnCourseEntry == origins[i].ordinaryRefill);

        gPuppyWarp = LEVEL_BOB;
        initiate_warp(LEVEL_BOB, 1, WARP_NODE_MAIN_ENTRY, WARP_FLAGS_NONE);
        assert(sHealOnCourseEntry);
        assert(sWarpDest.type == WARP_TYPE_CHANGE_LEVEL);

        gPuppyWarp = 0;
        initiate_warp(LEVEL_BOB, 1, WARP_NODE_MAIN_ENTRY, WARP_FLAGS_NONE);
        assert(sHealOnCourseEntry == origins[i].ordinaryRefill);
        gPuppyWarp = LEVEL_BOB;
        gCurrDemoInput = &mario;
        initiate_warp(LEVEL_BOB, 1, WARP_NODE_MAIN_ENTRY, WARP_FLAGS_NONE);
        assert(!sHealOnCourseEntry);
        gCurrDemoInput = NULL;
        gCurrCreditsEntry = sCreditsSequence;
        initiate_warp(LEVEL_BOB, 1, WARP_NODE_MAIN_ENTRY, WARP_FLAGS_NONE);
        assert(!sHealOnCourseEntry);
        gCurrCreditsEntry = NULL;
        const s32 nonCourses[] = { LEVEL_CASTLE, LEVEL_CASTLE_COURTYARD, LEVEL_CASTLE_GROUNDS };
        for (u32 j = 0; j < sizeof(nonCourses) / sizeof(nonCourses[0]); j++) {
            gLevelToCourseNumTable[nonCourses[j] - 1] = COURSE_NONE;
            gPuppyWarp = nonCourses[j];
            initiate_warp(nonCourses[j], 1, WARP_NODE_MAIN_ENTRY, WARP_FLAGS_NONE);
            assert(sHealOnCourseEntry);
            gPuppyWarp = 0;
            initiate_warp(nonCourses[j], 1, WARP_NODE_MAIN_ENTRY, WARP_FLAGS_NONE);
            assert(!sHealOnCourseEntry);
        }
        gPuppyWarp = 0;
        gCurrLevelNum = LEVEL_WF;
        initiate_warp(LEVEL_BOB, 1, WARP_NODE_MAIN_ENTRY, WARP_FLAGS_NONE);
        assert(!sHealOnCourseEntry);
    }
}

static void check_poison_game_over(void) {
    for (s32 debug = 0; debug < 2; debug++) {
        begin();
        gCurrLevelNum = LEVEL_CASTLE_GROUNDS;
        gCurrAreaIndex = area.index = 1;
        gDebugLevelSelect = debug;
        mario.numLives = 99;
        mario.health = 0xFF;
        mario.poisonMushroomDeathPending = TRUE;
        assert(level_trigger_warp(&mario, WARP_OP_DEATH) == 48);
        assert(sDelayedWarpOp == WARP_OP_GAME_OVER && mario.numLives == 0);
        for (s32 frame = 0; frame < 48; frame++) {
            initiate_delayed_warp();
        }
        assert(saves == 1);
        assert(sSpecialWarpDest == WARP_SPECIAL_MARIO_HEAD_DIZZY);
    }
}

int main(void) {
    check_bounds();
    check_warps();
    check_blocked_visit_exits();
    check_poison_game_over();
    check_course_entry_refill();
    return 0;
}
