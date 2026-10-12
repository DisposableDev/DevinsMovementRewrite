#include "stats_test_world.h"
#include "player_stats.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static u8 sStorage[0x8000];
static s32 sWriteLimit = -1;
static s32 sWrites;
static s32 sReadFailure;
static s32 sFirstBankReadFailure;
static s32 sLegacyWriteFailure;
static u32 sLastOffset;
static struct MarioState sMario;
static s32 sDialog = DIALOG_NONE;
struct MarioState *gMarioState = &sMario;
struct Object gObjectPool[OBJECT_POOL_CAPACITY];
s16 gCurrSaveFileNum = 1;
s16 gCurrLevelNum = LEVEL_BOB;
s16 gCurrAreaIndex = 1;
s16 gCurrCourseNum = COURSE_BOB;
void *gCurrDemoInput;
void *gCurrCreditsEntry;
s8 gLevelToCourseNumTable[LEVEL_COUNT];
static u32 sStars;
static s32 sHeldPose;
static u8 sPalette;
static s32 sCheating;

s32 game_options_is_cheating(void) {
    return sCheating;
}

u8 mario_character_palette(void) {
    return sPalette;
}
struct Surface gWaterSurfacePseudoFloor;
s32 mario_is_soaked(struct MarioState *m) {
    return m->soakedThisUpdate;
}

s32 mario_ground_twirl_is_held_pose(struct MarioState *m) {
    return m->action == ACT_GROUND_TWIRL && sHeldPose;
}

s32 get_dialog_id(void) {
    return sDialog;
}

u32 save_file_get_star_flags(s32 file, s32 course) {
    (void) file;
    (void) course;
    return sStars;
}

s32 obj_has_model(struct Object *obj, s32 model) {
    return obj->model == model;
}

s32 obj_has_behavior(struct Object *obj, const void *behavior) {
    return obj->behavior == behavior;
}

s32 mario_ground_twirl_is_enemy(struct Object *obj) {
    return obj_has_behavior(obj, bhvGoomba) || obj_has_behavior(obj, bhvBoo);
}

s32 mario_ground_twirl_is_immune_enemy(struct Object *obj) {
    return obj_has_behavior(obj, bhvBowser) || obj_has_behavior(obj, bhvPokeyBodyPart)
           || obj_has_behavior(obj, bhvPokey);
}

static s32 read_storage(u32 offset, void *data, u32 size) {
    assert(offset + size <= sizeof(sStorage));
    if (sReadFailure) {
        return 1;
    }
    memcpy(data, sStorage + offset, size);
    return 0;
}

static s32 write_storage(u32 offset, const void *data, u32 size) {
    assert(offset >= PLAYER_STATS_LEGACY_SRAM_OFFSET && offset + size <= sizeof(sStorage));
    assert(offset % 16 == 0 && size % 16 == 0);
    assert(size <= PLAYER_STATS_SAVE_STRIDE);
    if (sLegacyWriteFailure && offset < PLAYER_STATS_SRAM_OFFSET) {
        return 1;
    }
    sWrites++;
    sLastOffset = offset;
    if (sWriteLimit >= 0 && (u32) sWriteLimit < size) {
        size = sWriteLimit;
    }
    memcpy(sStorage + offset, data, size);
    return 0;
}

static u64 value(enum PlayerStatId id) {
    return gPlayerStats[0].values[id];
}

static void action(u32 next, u32 argument) {
    player_stats_action(&sMario, next, argument);
    sMario.action = next;
    sMario.actionArg = argument;
}

static void setup(void) {
    s32 i;
    memset(sStorage, 0xA5, sizeof(sStorage));
    memset(&sMario, 0, sizeof(sMario));
    sMario.health = 0x880;
    sMario.action = ACT_WALKING;
    sMario.flags = MARIO_NORMAL_CAP | MARIO_CAP_ON_HEAD;
    for (i = 0; i < LEVEL_COUNT; i++) {
        gLevelToCourseNumTable[i] = COURSE_BOB;
    }
    gLevelToCourseNumTable[LEVEL_PHYSICS_LAB - 1] = COURSE_NONE;
    player_stats_init(read_storage, write_storage);
    player_stats_level_entered();
    player_stats_game_update();
}

static void enemy_bounce_streak(void) {
    struct Object *enemy = &gObjectPool[0];
    struct Object *box = &gObjectPool[1];
    enemy->behavior = bhvGoomba;
    box->behavior = bhvBreakableBox;
    sMario.action = ACT_FREEFALL;
    player_stats_enemy_bounce(box);
    assert(value(PS_ENEMY_BOUNCE_STREAK) == 0);
    for (int i = 0; i < 3; i++) {
        player_stats_enemy_bounce(enemy);
    }
    assert(value(PS_ENEMY_BOUNCE_STREAK) == 3);
    assert(gPlayerStats[0].levels[PS_ENEMY_BOUNCE_STREAK] == LEVEL_BOB);
    action(ACT_WALL_KICK_AIR, 0);
    player_stats_enemy_bounce(enemy);
    assert(value(PS_ENEMY_BOUNCE_STREAK) == 4);
    player_stats_ground_contact();
    player_stats_enemy_bounce(enemy);
    assert(value(PS_ENEMY_BOUNCE_STREAK) == 4);
    sMario.floor = &gWaterSurfacePseudoFloor;
    sMario.floorHeight = sMario.pos[1];
    sMario.action = ACT_IDLE;
    player_stats_mario_end(&sMario);
    enemy->behavior = bhvSmallBully;
    for (int i = 0; i < 5; i++) {
        player_stats_enemy_bounce(enemy);
    }
    assert(value(PS_ENEMY_BOUNCE_STREAK) == 5);
    gCurrDemoInput = enemy;
    player_stats_enemy_bounce(enemy);
    gCurrDemoInput = NULL;
    gCurrCreditsEntry = enemy;
    player_stats_enemy_bounce(enemy);
    gCurrCreditsEntry = NULL;
    assert(value(PS_ENEMY_BOUNCE_STREAK) == 5);
    player_stats_warp(LEVEL_WF, 0);
    player_stats_enemy_bounce(enemy);
    assert(value(PS_ENEMY_BOUNCE_STREAK) == 5);
    player_stats_death(FALSE, FALSE);
    player_stats_enemy_bounce(enemy);
    assert(value(PS_ENEMY_BOUNCE_STREAK) == 5);
    assert(player_stats_checkpoint(0));
    player_stats_init(read_storage, write_storage);
    player_stats_select_file(0);
    assert(value(PS_ENEMY_BOUNCE_STREAK) == 5);
}

static void seconds_soaked(void) {
    sMario.action = ACT_FREEFALL;
    sMario.waterLevel = -100;
    sMario.soakedThisUpdate = TRUE;
    for (int i = 0; i < 150; i++) {
        player_stats_mario_end(&sMario);
    }
    assert(value(PS_SECONDS_SOAKED) == 150);
    sMario.pos[1] = -101;
    for (int i = 0; i < 60; i++) {
        player_stats_mario_end(&sMario);
    }
    assert(value(PS_SECONDS_SOAKED) == 150);
    sMario.pos[1] = sMario.waterLevel;
    player_stats_mario_end(&sMario);
    assert(value(PS_SECONDS_SOAKED) == 150);
    sMario.pos[1] = 0;
    sMario.soakedThisUpdate = FALSE;
    player_stats_mario_end(&sMario);
    assert(value(PS_SECONDS_SOAKED) == 150);
    assert(player_stats_checkpoint(0));
    player_stats_init(read_storage, write_storage);
    player_stats_select_file(0);
    assert(value(PS_SECONDS_SOAKED) == 150);
}

static void stats_tick(void) {
    player_stats_game_update();
    player_stats_mario_begin(&sMario);
    player_stats_mario_end(&sMario);
}

static void damage_free_visits(void) {
    struct Object *star = &gObjectPool[0];
    for (int i = 0; i < 60; i++) {
        stats_tick();
    }
    assert(value(PS_LONGEST_DAMAGE_FREE_STREAK) == 60);
    player_stats_mario_begin(&sMario);
    player_stats_health(0x880, 0x840);
    sMario.health = 0x840;
    player_stats_mario_end(&sMario);
    for (int i = 0; i < 70; i++) {
        stats_tick();
    }
    assert(value(PS_LONGEST_DAMAGE_FREE_STREAK) == 70);
    player_stats_health(0x840, 0x880); /* Healing cannot restore a damage-free visit. */
    sMario.health = 0x880;
    gCurrAreaIndex = 2;
    player_stats_level_entered();
    player_stats_star(star, 0);
    assert(value(PS_STARS_COLLECTED_WITHOUT_DAMAGE) == 0);
    player_stats_warp(LEVEL_WF, TRUE);
    gCurrLevelNum = LEVEL_WF;
    player_stats_level_entered();
    stats_tick();
    assert(value(PS_LONGEST_DAMAGE_FREE_STREAK) == 71); /* Clean travel preserves the streak. */
    player_stats_star(star, 0);
    assert(value(PS_STARS_COLLECTED_WITHOUT_DAMAGE) == 1);
    sMario.hurtCounter = 1;
    player_stats_star(star, 1);
    assert(value(PS_STARS_COLLECTED_WITHOUT_DAMAGE) == 1);
    sMario.hurtCounter = 0;
    star->model = MODEL_BOWSER_KEY;
    player_stats_star(star, 0);
    assert(value(PS_STARS_COLLECTED_WITHOUT_DAMAGE) == 1);
    player_stats_death(FALSE, TRUE);
    player_stats_level_entered();
    stats_tick();
    assert(value(PS_LONGEST_DAMAGE_FREE_STREAK) == 71);
    sCheating = TRUE;
    stats_tick();
    player_stats_star(star, 0);
    sCheating = FALSE;
    stats_tick();
    assert(value(PS_LONGEST_DAMAGE_FREE_STREAK) == 71);
}

static void activity_timers(void) {
    sMario.heldObj = &gObjectPool[0];
    sMario.flags |= MARIO_WING_CAP | MARIO_METAL_CAP | MARIO_VANISH_CAP;
    sMario.soakedThisUpdate = TRUE;
    sMario.waterLevel = -100;
    for (int i = 0; i < 30; i++) {
        player_stats_mario_begin(&sMario);
        sMario.pos[0] += 3;
        sMario.pos[2] += 4;
        player_stats_mario_end(&sMario);
    }
    assert(value(PS_TIME_HOLDING_OBJECTS) == 30);
    assert(value(PS_TIME_USING_WING_CAP) == 30);
    assert(value(PS_TIME_USING_METAL_CAP) == 30);
    assert(value(PS_TIME_USING_VANISH_CAP) == 30);
    assert(value(PS_DISTANCE_TRAVELED_WHILE_SOAKED) == 150 * 256);
    sMario.pos[1] = sMario.waterLevel;
    player_stats_mario_begin(&sMario);
    sMario.pos[0] += 50;
    player_stats_mario_end(&sMario);
    assert(value(PS_DISTANCE_TRAVELED_WHILE_SOAKED) == 150 * 256);
    sMario.heldObj = NULL;
    sMario.flags = MARIO_NORMAL_CAP | MARIO_CAP_ON_HEAD;
    sMario.soakedThisUpdate = FALSE;
    stats_tick();
    assert(value(PS_TIME_HOLDING_OBJECTS) == 31);
    assert(value(PS_TIME_USING_WING_CAP) == 31);
    assert(value(PS_TIME_USING_METAL_CAP) == 31);
    assert(value(PS_TIME_USING_VANISH_CAP) == 31);
    player_stats_warp(LEVEL_WF, TRUE);
    sMario.pos[0] += 10000;
    sMario.soakedThisUpdate = TRUE;
    sMario.pos[1] = 0;
    player_stats_mario_end(&sMario);
    assert(value(PS_DISTANCE_TRAVELED_WHILE_SOAKED) == 150 * 256);
    sCheating = TRUE;
    sMario.heldObj = &gObjectPool[0];
    sMario.flags |= MARIO_WING_CAP | MARIO_METAL_CAP | MARIO_VANISH_CAP;
    player_stats_mario_end(&sMario);
    assert(value(PS_TIME_HOLDING_OBJECTS) == 31);
    assert(value(PS_TIME_USING_WING_CAP) == 31);
    assert(value(PS_TIME_USING_METAL_CAP) == 31);
    assert(value(PS_TIME_USING_VANISH_CAP) == 31);
}

static void new_defeat_records(void) {
    struct Object *enemy = &gObjectPool[0];
    struct Object *thrown = &gObjectPool[1];
    enemy->behavior = bhvGoomba;
    enemy->oHealth = 1;
    action(ACT_MOVE_PUNCHING, ACT_ARG_PUNCH_SEQUENCE_BREAKDANCE);
    player_stats_attack(enemy, ATTACK_KICK_OR_TRIP);
    player_stats_enemy_defeated(enemy);
    player_stats_enemy_defeated(enemy);
    assert(value(PS_BREAKDANCE_DEFEATS) == 1);
    assert(value(PS_MOST_ENEMIES_DEFEATED_IN_ONE_VISIT) == 1);
    /* A later punch overrides dance credit, even when defeat occurs while dancing. */
    player_stats_attack(enemy, ATTACK_KICK_OR_TRIP);
    action(ACT_PUNCHING, ACT_ARG_PUNCH_SEQUENCE_YAH);
    player_stats_attack(enemy, ATTACK_PUNCH);
    action(ACT_MOVE_PUNCHING, ACT_ARG_PUNCH_SEQUENCE_BREAKDANCE);
    player_stats_enemy_defeated(enemy);
    assert(value(PS_BREAKDANCE_DEFEATS) == 1);
    assert(value(PS_MOST_ENEMIES_DEFEATED_IN_ONE_VISIT) == 2);
    player_stats_throw(thrown);
    player_stats_inherit_attack(thrown, enemy);
    player_stats_attack(enemy, ATTACK_PUNCH); /* A fresh strike removes inherited throw credit. */
    player_stats_enemy_defeated(enemy);
    assert(value(PS_THROWN_OBJECT_DEFEATS) == 0);
    assert(value(PS_BREAKDANCE_DEFEATS) == 1);
    action(ACT_WALKING, 0);
    player_stats_attack(enemy, ATTACK_PUNCH);
    player_stats_inherit_attack(thrown, enemy);
    player_stats_enemy_defeated(enemy);
    assert(value(PS_THROWN_OBJECT_DEFEATS) == 1);
    assert(value(PS_MOST_ENEMIES_DEFEATED_IN_ONE_VISIT) == 4);
    gCurrAreaIndex = 2;
    player_stats_level_entered();
    player_stats_attack(enemy, ATTACK_PUNCH);
    player_stats_enemy_defeated(enemy);
    assert(value(PS_MOST_ENEMIES_DEFEATED_IN_ONE_VISIT) == 5);
    player_stats_warp(LEVEL_WF, TRUE);
    gCurrLevelNum = LEVEL_WF;
    player_stats_level_entered();
    player_stats_game_update();
    player_stats_attack(enemy, ATTACK_PUNCH);
    player_stats_enemy_defeated(enemy);
    assert(value(PS_MOST_ENEMIES_DEFEATED_IN_ONE_VISIT) == 5);
}

static void cannon_flight_record(void) {
    action(ACT_IN_CANNON, 0);
    action(ACT_SHOT_FROM_CANNON, 0);
    player_stats_mario_begin(&sMario);
    sMario.pos[0] = 3000;
    sMario.pos[2] = 4000;
    player_stats_mario_end(&sMario);
    assert(value(PS_FARTHEST_CANNON_FLIGHT) == 0);
    player_stats_ground_contact();
    assert(value(PS_FARTHEST_CANNON_FLIGHT) == 5000 * 256);
    sMario.pos[0] = 20000;
    player_stats_ground_contact();
    assert(value(PS_FARTHEST_CANNON_FLIGHT) == 5000 * 256);
    action(ACT_IN_CANNON, 0);
    action(ACT_SHOT_FROM_CANNON, 0);
    player_stats_mario_begin(&sMario);
    sMario.pos[0] += 6000;
    action(ACT_WATER_IDLE, 0);
    assert(value(PS_FARTHEST_CANNON_FLIGHT) == 6000 * 256);
    action(ACT_IN_CANNON, 0);
    action(ACT_SHOT_FROM_CANNON, 0);
    player_stats_mario_begin(&sMario);
    sMario.pos[0] += 20000;
    player_stats_warp(LEVEL_WF, TRUE);
    player_stats_ground_contact();
    assert(value(PS_FARTHEST_CANNON_FLIGHT) == 6000 * 256);
    const u32 interruptions[] = { ACT_READING_SIGN, ACT_READING_NPC_DIALOG, ACT_WAITING_FOR_DIALOG,
                                  ACT_FALL_AFTER_STAR_GRAB, ACT_STAR_DANCE_EXIT };
    for (unsigned i = 0; i < sizeof(interruptions) / sizeof(*interruptions); i++) {
        action(ACT_IN_CANNON, 0);
        action(ACT_SHOT_FROM_CANNON, 0);
        player_stats_mario_begin(&sMario);
        sMario.pos[0] += 20000;
        action(interruptions[i], 0);
        player_stats_ground_contact();
        assert(value(PS_FARTHEST_CANNON_FLIGHT) == 6000 * 256);
    }
}

static void cheating(void) {
    struct PlayerStatsData before = gPlayerStats[0];
    struct Object *enemy = &gObjectPool[0];
    enemy->behavior = bhvGoomba;
    enemy->oHealth = 1;
    player_stats_attack(enemy, ATTACK_PUNCH);
    player_stats_mario_begin(&sMario);
    sCheating = TRUE;
    player_stats_add(PS_YELLOW_COINS_COLLECTED, 9);
    player_stats_record(PS_LONGEST_CONTINUOUS_SLIDE, 999, LEVEL_BOB, 1);
    action(ACT_JUMP, 0);
    player_stats_mario_begin(&sMario);
    sMario.pos[0] = 10000;
    player_stats_mario_end(&sMario);
    player_stats_game_update();
    player_stats_enemy_defeated(enemy);
    assert(memcmp(&before, &gPlayerStats[0], sizeof(before)) == 0);
    assert(player_stats_checkpoint(0));
    sCheating = FALSE;
    player_stats_game_update();
    player_stats_enemy_defeated(enemy);
    player_stats_mario_begin(&sMario);
    sMario.pos[0] += 3;
    player_stats_mario_end(&sMario);
    assert(value(PS_TOTAL_DISTANCE_TRAVELED) == 3 * PLAYER_STATS_FIXED_SCALE);
    assert(value(PS_TOTAL_ENEMIES_DEFEATED) == 0);
    assert(value(PS_TOTAL_JUMPS_PERFORMED) == 0);
    assert(value(PS_TOTAL_PLAY_TIME) == before.values[PS_TOTAL_PLAY_TIME] + 1);
    assert(value(PS_COURSE_ENTRIES) == before.values[PS_COURSE_ENTRIES]);
    assert(value(PS_COURSE_EXITS_WITHOUT_COLLECTING_A_STAR)
           == before.values[PS_COURSE_EXITS_WITHOUT_COLLECTING_A_STAR]);
    player_stats_add(PS_YELLOW_COINS_COLLECTED, 1);
    player_stats_record(PS_LONGEST_CONTINUOUS_SLIDE, 5, LEVEL_BOB, 1);
    assert(value(PS_YELLOW_COINS_COLLECTED) == 1);
    assert(value(PS_LONGEST_CONTINUOUS_SLIDE) == 5);
}

static void cheating_visits(void) {
    // Repeated toggles inside a course never create visits or exits.
    for (s32 i = 0; i < 3; i++) {
        sCheating = TRUE;
        player_stats_game_update();
        sCheating = FALSE;
        player_stats_game_update();
    }
    assert(value(PS_COURSE_ENTRIES) == 1);
    assert(value(PS_COURSE_EXITS_WITHOUT_COLLECTING_A_STAR) == 0);

    // Entering another course while blocked must not count later on resume.
    sCheating = TRUE;
    player_stats_warp(LEVEL_WF, TRUE);
    gCurrLevelNum = LEVEL_WF;
    player_stats_level_entered();
    player_stats_game_update();
    sCheating = FALSE;
    player_stats_game_update();
    assert(value(PS_COURSE_ENTRIES) == 1);
    assert(value(PS_COURSE_EXITS_WITHOUT_COLLECTING_A_STAR) == 0);

    // A real death/re-entry still starts a visit, even if the death was blocked.
    sCheating = TRUE;
    player_stats_death(FALSE, FALSE);
    sCheating = FALSE;
    player_stats_level_entered();
    player_stats_game_update();
    assert(value(PS_COURSE_ENTRIES) == 2);
    assert(value(PS_LIVES_LOST) == 0);
    assert(value(PS_COURSE_EXITS_WITHOUT_COLLECTING_A_STAR) == 0);
    player_stats_warp(LEVEL_BOB, TRUE);
    gCurrLevelNum = LEVEL_BOB;
    player_stats_level_entered();
    player_stats_game_update();
    assert(value(PS_COURSE_ENTRIES) == 3);
    assert(value(PS_COURSE_EXITS_WITHOUT_COLLECTING_A_STAR) == 1);
}

static void counters(void) {
    s32 id;
    for (id = 1; id < PLAYER_STAT_COUNT; id++) {
        assert(gPlayerStatInfo[id].label != NULL);
        assert(gPlayerStatInfo[id].category < 8);
    }
    gPlayerStats[0].values[PS_TOTAL_JUMPS_PERFORMED] = ~(u64) 0 - 2;
    player_stats_add(PS_TOTAL_JUMPS_PERFORMED, 3);
    assert(value(PS_TOTAL_JUMPS_PERFORMED) == ~(u64) 0);
    player_stats_select_file(1);
    player_stats_add(PS_TOTAL_JUMPS_PERFORMED, 4);
    assert(gPlayerStats[1].values[PS_TOTAL_JUMPS_PERFORMED] == 4);
    player_stats_select_file(-1);
    player_stats_add(PS_TOTAL_JUMPS_PERFORMED, 99);
    assert(gPlayerStats[1].values[PS_TOTAL_JUMPS_PERFORMED] == 4);
}

static void journal(void) {
    player_stats_add(PS_YELLOW_COINS_COLLECTED, 5);
    assert(player_stats_checkpoint(0));
    player_stats_add(PS_YELLOW_COINS_COLLECTED, 7);
    assert(player_stats_checkpoint(0));
    assert(sLastOffset == PLAYER_STATS_SRAM_OFFSET + PLAYER_STATS_SAVE_STRIDE);
    sStorage[sLastOffset + 60] ^= 0x80;
    player_stats_init(read_storage, write_storage);
    assert(value(PS_YELLOW_COINS_COLLECTED) == 5);
    assert(sStorage[0] == 0xA5);
}

static void torn_write(void) {
    player_stats_add(PS_YELLOW_COINS_COLLECTED, 5);
    assert(player_stats_checkpoint(0));
    player_stats_add(PS_YELLOW_COINS_COLLECTED, 7);
    sWriteLimit = 40;
    assert(!player_stats_checkpoint(0));
    assert(player_stats_is_dirty(0));
    player_stats_init(read_storage, write_storage);
    assert(value(PS_YELLOW_COINS_COLLECTED) == 5);
}

static void retry_write(void) {
    player_stats_add(PS_YELLOW_COINS_COLLECTED, 5);
    sWriteLimit = 40;
    assert(!player_stats_checkpoint(0));
    sWriteLimit = -1;
    assert(player_stats_checkpoint(0));
    assert(!player_stats_is_dirty(0));
    player_stats_init(read_storage, write_storage);
    assert(value(PS_YELLOW_COINS_COLLECTED) == 5);
}

static void verify_failure(void) {
    player_stats_add(PS_YELLOW_COINS_COLLECTED, 5);
    sReadFailure = TRUE;
    assert(!player_stats_checkpoint(0));
    assert(player_stats_is_dirty(0));
    sReadFailure = FALSE;
    assert(player_stats_checkpoint(0));
    assert(!player_stats_is_dirty(0));
}

static s32 read_without_first_bank(u32 offset, void *data, u32 size) {
    if (sFirstBankReadFailure && offset == PLAYER_STATS_SRAM_OFFSET) {
        memset(data, 0xFF, size);
        return 1;
    }
    return read_storage(offset, data, size);
}

static void initial_read_failure(void) {
    u8 before[sizeof(sStorage)];
    player_stats_add(PS_YELLOW_COINS_COLLECTED, 5);
    assert(player_stats_checkpoint(0));
    player_stats_add(PS_YELLOW_COINS_COLLECTED, 7);
    assert(player_stats_checkpoint(0));
    memcpy(before, sStorage, sizeof(before));

    sReadFailure = TRUE;
    player_stats_init(read_storage, write_storage);
    sReadFailure = FALSE;
    player_stats_select_file(0);
    player_stats_add(PS_YELLOW_COINS_COLLECTED, 1);
    s32 writes = sWrites;
    assert(!player_stats_checkpoint(0));
    assert(player_stats_is_dirty(0));
    assert(sWrites == writes);
    assert(memcmp(before, sStorage, sizeof(before)) == 0);

    sFirstBankReadFailure = TRUE;
    player_stats_init(read_without_first_bank, write_storage);
    sFirstBankReadFailure = FALSE;
    assert(value(PS_YELLOW_COINS_COLLECTED) == 12);
    player_stats_select_file(0);
    player_stats_add(PS_YELLOW_COINS_COLLECTED, 1);
    assert(!player_stats_checkpoint(0));
    assert(player_stats_is_dirty(0));
    assert(sWrites == writes);
    assert(memcmp(before, sStorage, sizeof(before)) == 0);

    // A fault in file A does not block ordinary writes to file B.
    player_stats_select_file(1);
    player_stats_add(PS_LONG_JUMPS, 8);
    assert(player_stats_checkpoint(1));
    // Explicit replacement releases only the destination's protection.
    player_stats_copy(1, 0);
    assert(!player_stats_is_dirty(0));
    player_stats_init(read_storage, write_storage);
    assert(value(PS_LONG_JUMPS) == 8);
    assert(value(PS_YELLOW_COINS_COLLECTED) == 0);

    sReadFailure = TRUE;
    player_stats_init(read_storage, write_storage);
    sReadFailure = FALSE;
    player_stats_erase(0);
    assert(!player_stats_is_dirty(0));
    writes = sWrites;
    player_stats_select_file(1);
    player_stats_add(PS_LONG_JUMPS, 1);
    assert(!player_stats_checkpoint(1));
    assert(sWrites == writes);

    // A successful reload restores persisted totals and permits subsequent saves.
    player_stats_init(read_storage, write_storage);
    assert(value(PS_LONG_JUMPS) == 0);
    player_stats_select_file(1);
    assert(gPlayerStats[1].values[PS_LONG_JUMPS] == 8);
    player_stats_add(PS_LONG_JUMPS, 1);
    assert(player_stats_checkpoint(1));
    player_stats_init(read_storage, write_storage);
    assert(gPlayerStats[1].values[PS_LONG_JUMPS] == 9);
}

static void copy_read_failure(void) {
    player_stats_add(PS_LONG_JUMPS, 5);
    assert(player_stats_checkpoint(0));
    player_stats_select_file(1);
    player_stats_add(PS_LONG_JUMPS, 8);
    assert(player_stats_checkpoint(1));
    sFirstBankReadFailure = TRUE;
    player_stats_init(read_without_first_bank, write_storage);
    sFirstBankReadFailure = FALSE;
    assert(value(PS_LONG_JUMPS) == 0);
    s32 writes = sWrites;
    player_stats_copy(0, 1);
    assert(gPlayerStats[1].values[PS_LONG_JUMPS] == 8);
    assert(sWrites == writes);
    assert(!player_stats_is_dirty(1));

    player_stats_init(read_storage, write_storage);
    player_stats_copy(0, 1);
    assert(gPlayerStats[1].values[PS_LONG_JUMPS] == 5);
    // A readable blank source is still a legitimate explicit replacement.
    player_stats_copy(3, 1);
    assert(gPlayerStats[1].values[PS_LONG_JUMPS] == 0);
    player_stats_init(read_storage, write_storage);
    assert(gPlayerStats[1].values[PS_LONG_JUMPS] == 0);
}

static void copy_erase(void) {
    player_stats_add(PS_LONG_JUMPS, 8);
    player_stats_copy(0, 1);
    assert(gPlayerStats[1].values[PS_LONG_JUMPS] == 8);
    player_stats_erase(1);
    player_stats_init(read_storage, write_storage);
    assert(gPlayerStats[1].values[PS_LONG_JUMPS] == 0);
    /* Either surviving erased backup remains erased. */
    sStorage[PLAYER_STATS_SRAM_OFFSET + 2 * PLAYER_STATS_SAVE_STRIDE + 60] ^= 1;
    player_stats_init(read_storage, write_storage);
    assert(gPlayerStats[1].values[PS_LONG_JUMPS] == 0);
}

static void slot_identity(void) {
    player_stats_add(PS_LONG_JUMPS, 8);
    assert(player_stats_checkpoint(0));
    memcpy(sStorage + PLAYER_STATS_SRAM_OFFSET + 2 * PLAYER_STATS_SAVE_STRIDE,
           sStorage + PLAYER_STATS_SRAM_OFFSET, PLAYER_STATS_SAVE_STRIDE);
    player_stats_init(read_storage, write_storage);
    assert(value(PS_LONG_JUMPS) == 8);
    assert(gPlayerStats[1].values[PS_LONG_JUMPS] == 0);
}

static void bowser_defeats(void) {
    const s16 fights[] = { LEVEL_BOWSER_1, LEVEL_BOWSER_2, LEVEL_BOWSER_3 };
    const enum PlayerStatId stats[] = { PS_DEFEATED_BY_BOWSER_1, PS_DEFEATED_BY_BOWSER_2,
                                        PS_DEFEATED_BY_BOWSER_3 };
    for (int fight = 0; fight < 3; fight++) {
        gCurrLevelNum = fights[fight];
        player_stats_level_entered();
        player_stats_death(FALSE, FALSE);
        assert(value(stats[fight]) == 1);
        player_stats_level_entered();
        player_stats_death(FALSE, TRUE);
        assert(value(stats[fight]) == 2);
        player_stats_level_entered();
        player_stats_warp(LEVEL_CASTLE_GROUNDS, TRUE);
        player_stats_warp(LEVEL_CASTLE_GROUNDS, TRUE);
        assert(value(stats[fight]) == 3);
        player_stats_level_entered();
        player_stats_warp(LEVEL_CASTLE_GROUNDS, FALSE);
        assert(value(stats[fight]) == 3);
    }
    gCurrLevelNum = LEVEL_BITS;
    player_stats_level_entered();
    player_stats_death(FALSE, TRUE);
    assert(value(PS_DEFEATED_BY_BOWSER_3) == 3);
    player_stats_init(read_storage, write_storage);
    player_stats_select_file(0);
    for (int fight = 0; fight < 3; fight++) {
        assert(value(stats[fight]) == 3);
        assert(gPlayerStats[1].values[stats[fight]] == 0);
    }
}

static void clock_and_visits(void) {
    assert(value(PS_COURSE_ENTRIES) == 1);
    sDialog = DIALOG_000;
    player_stats_game_update();
    assert(value(PS_TOTAL_PLAY_TIME) == 2);
    assert(value(PS_TIME_SPENT_IN_DIALOGUE) == 1);
    gCurrDemoInput = &sMario;
    player_stats_game_update();
    assert(value(PS_TOTAL_PLAY_TIME) == 2);
    gCurrDemoInput = NULL;
    player_stats_game_update();
    player_stats_death(TRUE, FALSE);
    player_stats_game_update();
    player_stats_game_update();
    assert(value(PS_COURSE_EXITS_WITHOUT_COLLECTING_A_STAR) == 2);
    assert(value(PS_COURSE_ENTRIES) == 2); /* Demo ends one visit, normal play starts another. */
    assert(value(PS_LIVES_LOST) == 1 && value(PS_GAME_OVERS) == 1);
    player_stats_level_entered();
    player_stats_game_update();
    assert(value(PS_COURSE_ENTRIES) == 3);
}

static void lab_time(void) {
    player_stats_warp(LEVEL_PHYSICS_LAB, TRUE);
    gCurrLevelNum = LEVEL_PHYSICS_LAB;
    gCurrCourseNum = COURSE_NONE;
    player_stats_level_entered();
    player_stats_game_update();
    assert(value(PS_TIME_SPENT_IN_COURSES) == 2);
    assert(value(PS_TIME_SPENT_IN_PEACH_S_CASTLE) == 0);
    assert(value(PS_COURSE_ENTRIES) == 2);
}

static void movement(void) {
    player_stats_mario_begin(&sMario);
    sMario.pos[0] = 3;
    sMario.pos[1] = 12;
    sMario.pos[2] = 4;
    player_stats_mario_end(&sMario);
    assert(value(PS_TOTAL_DISTANCE_TRAVELED) == 5 * 256);
    assert(value(PS_TOTAL_DISTANCE_TRAVELED_3D) == 13 * 256);
    assert(value(PS_DISTANCE_TRAVELED_ON_FOOT) == 5 * 256);
    assert(value(PS_DISTANCE_TRAVELED_ON_FOOT_3D) == 13 * 256);
    player_stats_warp(LEVEL_BOB, FALSE);
    sMario.pos[0] = 1000000;
    player_stats_mario_begin(&sMario);
    player_stats_mario_end(&sMario);
    assert(value(PS_TOTAL_DISTANCE_TRAVELED) == 5 * 256);
}

static void warp_exit_actions(void) {
    static const u32 exits[] = {
        ACT_EXIT_AIRBORNE,
        ACT_FALLING_EXIT_AIRBORNE,
        ACT_SPECIAL_EXIT_AIRBORNE,
    };
    u32 i;
    s32 frame;
    for (i = 0; i < sizeof(exits) / sizeof(exits[0]); i++) {
        player_stats_level_entered();
        action(exits[i], 0);
        for (frame = 0; frame < 4; frame++) {
            player_stats_mario_begin(&sMario);
            sMario.pos[0] += 24;
            sMario.pos[1] -= 200;
            player_stats_mario_end(&sMario);
        }
        action(ACT_EXIT_LAND_SAVE_DIALOG, 0);
        player_stats_mario_begin(&sMario);
        player_stats_mario_end(&sMario);
        assert(value(PS_TOTAL_DISTANCE_TRAVELED) == 0);
        assert(value(PS_TOTAL_DISTANCE_TRAVELED_3D) == 0);
        assert(value(PS_FASTEST_AIRBORNE_SPEED) == 0);
        assert(value(PS_LONGEST_AIR_TIME_WITHOUT_WING_CAP) == 0);
        assert(value(PS_GREATEST_FALL_SURVIVED) == 0);
    }
    action(ACT_WALKING, 0);
    player_stats_mario_begin(&sMario);
    sMario.pos[0] += 12;
    player_stats_mario_end(&sMario);
    assert(value(PS_TOTAL_DISTANCE_TRAVELED) == 12 * 256);
    assert(value(PS_TOTAL_DISTANCE_TRAVELED_3D) == 12 * 256);
    assert(value(PS_DISTANCE_TRAVELED_ON_FOOT) == 12 * 256);
    assert(value(PS_FASTEST_GROUND_SPEED) == 12 * 256);
}

static void twirl_long_jump(void) {
    sMario.pos[1] = 200;
    player_stats_mario_begin(&sMario);
    action(ACT_LONG_JUMP, 0);
    sMario.pos[0] = 300;
    sMario.pos[1] = 400;
    player_stats_mario_end(&sMario);
    player_stats_game_update();
    player_stats_mario_begin(&sMario);
    action(ACT_L_AIR_TWIRL, 0);
    sMario.pos[0] = 600;
    sMario.pos[1] = 0;
    action(ACT_LONG_JUMP_LAND, 0);
    assert(value(PS_LONGEST_SINGLE_LONG_JUMP) == 600 * 256);
    assert(value(PS_GREATEST_FALL_SURVIVED) == 400 * 256);
    assert(value(PS_LONGEST_AIR_TIME_WITHOUT_WING_CAP) == 2);
    assert(value(PS_LONG_JUMPS) == 1 && value(PS_TOTAL_JUMPS_PERFORMED) == 1);
}

static void assisted_airtime(void) {
    sMario.flags |= MARIO_WING_CAP;
    action(ACT_LONG_JUMP, 0);
    player_stats_game_update();
    player_stats_mario_begin(&sMario);
    sMario.flags &= ~MARIO_WING_CAP;
    action(ACT_LONG_JUMP_LAND, 0);
    assert(value(PS_LONGEST_AIR_TIME_WITHOUT_WING_CAP) == 0);
}

static void survived_falls(void) {
    sMario.pos[1] = 500;
    action(ACT_FREEFALL, 0);
    sMario.pos[1] = 100;
    player_stats_fall_survived(&sMario);
    assert(value(PS_GREATEST_FALL_SURVIVED) == 400 * 256);
    // Three ticks remaining still owe one complete wedge, so this fall is lethal.
    sMario.health = 0x180;
    sMario.hurtCounter = 3;
    sMario.pos[1] = -1000;
    player_stats_fall_survived(&sMario);
    assert(value(PS_GREATEST_FALL_SURVIVED) == 400 * 256);
    sMario.health = 0x880;
    sMario.hurtCounter = 0;
    sMario.pos[1] = -200;
    action(ACT_WATER_PLUNGE, 0);
    assert(value(PS_GREATEST_FALL_SURVIVED) == 400 * 256);
    action(ACT_FREEFALL, 0);
    sMario.pos[1] = -1000;
    sMario.hurtCounter = 40;
    action(ACT_HARD_FORWARD_GROUND_KB, 0);
    assert(value(PS_GREATEST_FALL_SURVIVED) == 400 * 256);
}

static void coins_stars(void) {
    struct Object *obj = &gObjectPool[0];
    player_stats_object_reset(obj);
    obj->model = MODEL_RED_COIN;
    obj->oDamageOrCoinValue = 2;
    player_stats_coin(obj, 2);
    obj->model = MODEL_BLUE_COIN;
    obj->oDamageOrCoinValue = 5;
    player_stats_coin(obj, 7);
    assert(value(PS_TOTAL_COIN_VALUE_COLLECTED) == 7);
    assert(value(PS_RED_COINS_COLLECTED) == 1 && value(PS_BLUE_COINS_COLLECTED) == 1);
    obj->model = MODEL_STAR;
    player_stats_star(obj, STAR_BP_ACT_100_COINS);
    assert(value(PS_COURSE_ENTRIES) == 1);
    assert(value(PS_100_COIN_STARS_COLLECTED) == 1);
    assert(value(PS_RED_COIN_STARS_COLLECTED) == 0);
    sStars = 1 << STAR_BP_ACT_100_COINS;
    player_stats_star(obj, STAR_BP_ACT_100_COINS);
    assert(value(PS_TOTAL_POWER_STARS_COLLECTED) == 2);
    assert(value(PS_UNIQUE_POWER_STARS_COLLECTED) == 1);
    assert(value(PS_PREVIOUSLY_COLLECTED_STARS_RECOLLECTED) == 1);
    player_stats_object_reset(obj);
    player_stats_tag_star(obj, TRUE);
    player_stats_star(obj, 3);
    assert(value(PS_RED_COIN_STARS_COLLECTED) == 1);
    player_stats_warp(LEVEL_CASTLE, TRUE);
    assert(value(PS_COURSE_EXITS_WITHOUT_COLLECTING_A_STAR) == 0);
}

static void enemy_credit(void) {
    struct Object *obj = &gObjectPool[0];
    player_stats_object_reset(obj);
    obj->behavior = bhvGoomba;
    obj->oHealth = 1;
    player_stats_attack(obj, ATTACK_PUNCH);
    assert(value(PS_TOTAL_ENEMIES_DEFEATED) == 0);
    obj->oHealth = 0;
    player_stats_enemy_defeated(obj);
    player_stats_enemy_defeated(obj);
    assert(value(PS_TOTAL_ENEMIES_DEFEATED) == 1);
    assert(value(PS_ENEMIES_PUNCHED) == 1 && value(PS_GOOMBAS_DEFEATED) == 1);
    player_stats_object_reset(obj);
    obj->behavior = bhvBoo;
    player_stats_twirl_hit(obj);
    assert(value(PS_BOOS_DEFEATED) == 0);
    player_stats_attack(obj, ATTACK_PUNCH);
    player_stats_enemy_defeated(obj);
    assert(value(PS_BOOS_DEFEATED) == 1 && value(PS_TOTAL_ENEMIES_DEFEATED) == 2);
}

static void completed_sequences(void) {
    sMario.action = ACT_BUTT_SLIDE;
    player_stats_mario_begin(&sMario);
    sMario.pos[0] = 100;
    player_stats_mario_end(&sMario);
    sMario.pos[0] = 200;
    player_stats_mario_end(&sMario);
    assert(value(PS_LONGEST_CONTINUOUS_SLIDE) == 0);
    action(ACT_WALKING, 0);
    player_stats_mario_end(&sMario);
    assert(value(PS_LONGEST_CONTINUOUS_SLIDE) == 200 * 256);
    assert(gPlayerStats[0].levels[PS_LONGEST_CONTINUOUS_SLIDE] == LEVEL_BOB);
    sMario.riddenObj = &gObjectPool[1];
    sMario.pos[0] = 500;
    player_stats_mario_end(&sMario);
    assert(value(PS_LONGEST_CONTINUOUS_SHELL_RIDE) == 0);
    player_stats_warp(LEVEL_CASTLE, TRUE);
    assert(value(PS_LONGEST_CONTINUOUS_SHELL_RIDE) == 300 * 256);
    sMario.riddenObj = NULL;
    sMario.pos[0] = 1000000;
    player_stats_mario_begin(&sMario);
    player_stats_mario_end(&sMario);
    assert(value(PS_LONGEST_CONTINUOUS_SHELL_RIDE) == 300 * 256);
}

static void indirect_defeat(void) {
    struct Object *box = &gObjectPool[1];
    struct Object *enemy = &gObjectPool[2];
    player_stats_object_reset(box);
    player_stats_object_reset(enemy);
    enemy->behavior = bhvGoomba;
    player_stats_inherit_attack(box, enemy);
    player_stats_enemy_defeated(enemy);
    assert(value(PS_TOTAL_ENEMIES_DEFEATED) == 0);
    player_stats_throw(box);
    player_stats_inherit_attack(box, enemy);
    player_stats_enemy_defeated(enemy);
    assert(value(PS_TOTAL_ENEMIES_DEFEATED) == 1);
    assert(value(PS_ENEMIES_PUNCHED) == 0 && value(PS_ENEMIES_STOMPED) == 0);
}

static void health_and_hits(void) {
    player_stats_health(0x880, 0x840);
    player_stats_health(0x840, 0x780);
    player_stats_health(0x780, 0x7C0);
    assert(value(PS_POWER_METER_WEDGES_LOST) == 256);
    assert(value(PS_POWER_METER_WEDGES_RESTORED) == 64);
    player_stats_damage();
    player_stats_damage();
    assert(value(PS_HITS_TAKEN) == 1);
    player_stats_game_update();
    player_stats_damage();
    assert(value(PS_HITS_TAKEN) == 2);
}

static void checkpoints(void) {
    s32 i;
    for (i = 1; i < 1800; i++) {
        player_stats_game_update();
    }
    assert(sWrites == 1);
    player_stats_init(read_storage, write_storage);
    assert(value(PS_TOTAL_PLAY_TIME) == 1800);
}

static u32 fixture_crc(const u8 *bytes, u32 size) {
    u32 crc = ~0u;
    u32 i;
    s32 bit;
    for (i = 0; i < size; i++) {
        crc ^= bytes[i];
        for (bit = 0; bit < 8; bit++) {
            crc = (crc >> 1) ^ (0xEDB88320u & (0u - (crc & 1u)));
        }
    }
    return ~crc;
}

static void legacy_fixture(s32 file, s32 copy, u32 generation) {
    // Shipped version-1 offsets, deliberately independent of current struct sizes.
    u8 bytes[1312] = { 0 };
    u32 header[] = { 0x50535431u, 1, generation, 1272, (u32) file, 0 };
    s32 id;
    u32 crc;
    assert(fixture_crc((const u8 *) "123456789", 9) == 0xCBF43926u);
    memcpy(bytes, header, sizeof(header));
    for (id = 1; id <= 114; id++) {
        u64 count = (u64) id * 100;
        u16 level = id + 10;
        u8 area = id % 4 + 1;
        memcpy(bytes + 24 + id * 8, &count, sizeof(count));
        memcpy(bytes + 944 + id * 2, &level, sizeof(level));
        bytes[1174 + id] = area;
    }
    crc = fixture_crc(bytes, 1296);
    memcpy(bytes + 1296, &crc, sizeof(crc));
    memcpy(sStorage + 0x1000 + (file * 2 + copy) * 0x600, bytes, sizeof(bytes));
}

static void assert_legacy_preserved(void) {
    s32 id;
    for (id = 1; id <= 114; id++) {
        assert(value(id) == (u64) id * 100);
        assert(gPlayerStats[0].levels[id] == id + 10);
        assert(gPlayerStats[0].areas[id] == id % 4 + 1);
    }
    for (id = 115; id < PLAYER_STAT_COUNT; id++) {
        assert(value(id) == 0);
        assert(gPlayerStats[0].levels[id] == 0);
        assert(gPlayerStats[0].areas[id] == 0);
    }
}

static void migration(void) {
    u32 version;
    legacy_fixture(0, 0, 7);
    legacy_fixture(0, 1, 6);
    legacy_fixture(1, 0, 9);
    player_stats_init(read_storage, write_storage);
    player_stats_select_file(0);
    assert_legacy_preserved();
    assert(player_stats_is_dirty(0));
    assert(gPlayerStats[1].values[PS_BREAKDANCE_SPINS] == 11400);
    assert(player_stats_checkpoint(0));
    assert(sLastOffset == PLAYER_STATS_SRAM_OFFSET);
    memcpy(&version, sStorage + sLastOffset + 4, sizeof(version));
    assert(version == PLAYER_STATS_SAVE_VERSION);
    // Losing the first new copy before the second is saved retains legacy fallback.
    sStorage[PLAYER_STATS_SRAM_OFFSET + 50] ^= 1;
    player_stats_init(read_storage, write_storage);
    player_stats_select_file(0);
    assert_legacy_preserved();
    assert(player_stats_checkpoint(0));
    player_stats_init(read_storage, write_storage);
    player_stats_select_file(0);
    assert_legacy_preserved();
    assert(player_stats_is_dirty(0));
    player_stats_add(PS_DABS_PERFORMED, 9);
    assert(player_stats_checkpoint(0));
    assert(!player_stats_is_dirty(0));
    assert(sStorage[0x1000] == 0 && sStorage[0x1600] == 0);
    player_stats_init(read_storage, write_storage);
    player_stats_select_file(0);
    assert(value(PS_DABS_PERFORMED) == 9);
    sStorage[PLAYER_STATS_SRAM_OFFSET + PLAYER_STATS_SAVE_STRIDE + 50] ^= 1;
    player_stats_init(read_storage, write_storage);
    player_stats_select_file(0);
    assert_legacy_preserved();
    assert(!player_stats_is_dirty(0));
    assert(sStorage[0] == 0xA5 && sStorage[0xFFF] == 0xA5);
}

static void migration_v4(void) {
    for (int file = 0; file < PLAYER_STATS_SAVE_FILES; file++) {
        for (int copy = 0; copy < 2; copy++) {
            u8 bytes[1648] = { 0 };
            u32 header[] = { 0x50535431u, 4, 10 + copy, 1608, file, 0 };
            memcpy(bytes, header, sizeof(header));
            for (int id = 1; id <= 145; id++) {
                u64 count = id * 100 + file * 10 + copy;
                u16 level = id + 10;
                memcpy(bytes + 24 + id * 8, &count, sizeof(count));
                memcpy(bytes + 1192 + id * 2, &level, sizeof(level));
                bytes[1484 + id] = id % 4 + 1;
            }
            u32 crc = fixture_crc(bytes, 1632);
            memcpy(bytes + 1632, &crc, sizeof(crc));
            memcpy(sStorage + PLAYER_STATS_SRAM_OFFSET + (file * 2 + copy) * PLAYER_STATS_SAVE_STRIDE,
                   bytes, sizeof(bytes));
        }
    }
    player_stats_init(read_storage, write_storage);
    for (int file = 0; file < PLAYER_STATS_SAVE_FILES; file++) {
        player_stats_select_file(file);
        for (int id = 1; id <= 145; id++) {
            assert(gPlayerStats[file].values[id] == (u64) id * 100 + file * 10 + 1);
            assert(gPlayerStats[file].levels[id] == id + 10);
            assert(gPlayerStats[file].areas[id] == id % 4 + 1);
        }
        assert(gPlayerStats[file].values[PS_MISSED_BOWSER_THROWS] == 0);
    }
    // An interrupted migration leaves the previous journal copy readable.
    sWriteLimit = 700;
    assert(!player_stats_checkpoint(0));
    sWriteLimit = -1;
    player_stats_init(read_storage, write_storage);
    player_stats_select_file(0);
    assert(value(PS_DEFEATED_BY_BOWSER_3) == 14501);
    player_stats_add(PS_MISSED_BOWSER_THROWS, 3);
    assert(player_stats_checkpoint(0));
    player_stats_init(read_storage, write_storage);
    player_stats_select_file(0);
    assert(value(PS_MISSED_BOWSER_THROWS) == 3);
    assert(value(PS_DEFEATED_BY_BOWSER_3) == 14501);
    sStorage[PLAYER_STATS_SRAM_OFFSET + 50] ^= 1;
    player_stats_init(read_storage, write_storage);
    player_stats_select_file(0);
    assert(value(PS_MISSED_BOWSER_THROWS) == 3);
}

static void migration_mixed_v4(void) {
    // Reverting to an older ROM can leave different schemas in the two journal slots.
    for (int version = 5; version <= 6; version++) {
        for (int newerCopy = 0; newerCopy < 2; newerCopy++) {
            memset(sStorage, 0, sizeof(sStorage));
            for (int file = 0; file < PLAYER_STATS_SAVE_FILES; file++) {
                u8 oldBytes[1680] = { 0 };
                u32 oldHeader[] = { 0x50535431u, version, 10, version == 5 ? 1624 : 1640, file, 0 };
                memcpy(oldBytes, oldHeader, sizeof(oldHeader));
                u64 stale = 99;
                u16 staleLevel = LEVEL_BOB;
                memcpy(oldBytes + 24 + 146 * 8, &stale, sizeof(stale));
                memcpy(oldBytes + (version == 5 ? 1200 : 1216) + 146 * 2, &staleLevel,
                       sizeof(staleLevel));
                oldBytes[(version == 5 ? 1494 : 1514) + 146] = 1;
                u32 oldCrcOffset = version == 5 ? 1648 : 1664;
                u32 crc = fixture_crc(oldBytes, oldCrcOffset);
                memcpy(oldBytes + oldCrcOffset, &crc, sizeof(crc));
                memcpy(sStorage + PLAYER_STATS_SRAM_OFFSET
                           + (file * 2 + (1 - newerCopy)) * PLAYER_STATS_SAVE_STRIDE,
                       oldBytes, oldCrcOffset + 16);

                u8 newBytes[1648] = { 0 };
                u32 newHeader[] = { 0x50535431u, 4, 11, 1608, file, 0 };
                memcpy(newBytes, newHeader, sizeof(newHeader));
                u64 retained = 123 + file;
                memcpy(newBytes + 24 + PS_TOTAL_JUMPS_PERFORMED * 8, &retained, sizeof(retained));
                crc = fixture_crc(newBytes, 1632);
                memcpy(newBytes + 1632, &crc, sizeof(crc));
                memcpy(sStorage + PLAYER_STATS_SRAM_OFFSET
                           + (file * 2 + newerCopy) * PLAYER_STATS_SAVE_STRIDE,
                       newBytes, sizeof(newBytes));
            }
            player_stats_init(read_storage, write_storage);
            for (int file = 0; file < PLAYER_STATS_SAVE_FILES; file++) {
                assert(gPlayerStats[file].values[PS_TOTAL_JUMPS_PERFORMED] == (u64) 123 + file);
                for (int id = 146; id < PLAYER_STAT_COUNT; id++) {
                    assert(gPlayerStats[file].values[id] == 0);
                    assert(gPlayerStats[file].levels[id] == 0);
                    assert(gPlayerStats[file].areas[id] == 0);
                }
            }
        }
    }
}

static void migration_v5(void) {
    for (int file = 0; file < PLAYER_STATS_SAVE_FILES; file++) {
        for (int copy = 0; copy < 2; copy++) {
            u8 bytes[1664] = { 0 };
            u32 header[] = { 0x50535431u, 5, 10 + copy, 1624, file, 0 };
            memcpy(bytes, header, sizeof(header));
            for (int id = 1; id <= 146; id++) {
                u64 count = id * 100 + file * 10 + copy;
                u16 level = id + 10;
                memcpy(bytes + 24 + id * 8, &count, sizeof(count));
                memcpy(bytes + 1200 + id * 2, &level, sizeof(level));
                bytes[1494 + id] = id % 4 + 1;
            }
            u32 crc = fixture_crc(bytes, 1648);
            memcpy(bytes + 1648, &crc, sizeof(crc));
            memcpy(sStorage + PLAYER_STATS_SRAM_OFFSET + (file * 2 + copy) * PLAYER_STATS_SAVE_STRIDE,
                   bytes, sizeof(bytes));
        }
    }
    player_stats_init(read_storage, write_storage);
    for (int file = 0; file < PLAYER_STATS_SAVE_FILES; file++) {
        player_stats_select_file(file);
        for (int id = 1; id <= 146; id++) {
            assert(gPlayerStats[file].values[id] == (u64) id * 100 + file * 10 + 1);
            assert(gPlayerStats[file].levels[id] == id + 10);
            assert(gPlayerStats[file].areas[id] == id % 4 + 1);
        }
        assert(gPlayerStats[file].values[PS_ENEMY_BOUNCE_STREAK] == 0);
        assert(gPlayerStats[file].values[PS_SECONDS_SOAKED] == 0);
    }
    // An interrupted migration leaves the previous journal copy readable.
    sWriteLimit = 700;
    assert(!player_stats_checkpoint(0));
    sWriteLimit = -1;
    player_stats_init(read_storage, write_storage);
    player_stats_select_file(0);
    assert(value(PS_DEFEATED_BY_BOWSER_3) == 14501);
    player_stats_record(PS_ENEMY_BOUNCE_STREAK, 3, LEVEL_BOB, 1);
    assert(player_stats_checkpoint(0));
    player_stats_init(read_storage, write_storage);
    player_stats_select_file(0);
    assert(value(PS_ENEMY_BOUNCE_STREAK) == 3);
    assert(value(PS_DEFEATED_BY_BOWSER_3) == 14501);
    sStorage[PLAYER_STATS_SRAM_OFFSET + 50] ^= 1;
    player_stats_init(read_storage, write_storage);
    player_stats_select_file(0);
    assert(value(PS_ENEMY_BOUNCE_STREAK) == 3);
}

static void migration_v6(void) {
    for (int file = 0; file < PLAYER_STATS_SAVE_FILES; file++) {
        for (int copy = 0; copy < 2; copy++) {
            u8 bytes[1680] = { 0 };
            u32 header[] = { 0x50535431u, 6, 10 + copy, 1640, file, 0 };
            memcpy(bytes, header, sizeof(header));
            for (int id = 1; id <= 148; id++) {
                u64 count = id * 100 + file * 10 + copy;
                u16 level = id + 10;
                memcpy(bytes + 24 + id * 8, &count, sizeof(count));
                memcpy(bytes + 1216 + id * 2, &level, sizeof(level));
                bytes[1514 + id] = id % 4 + 1;
            }
            u32 crc = fixture_crc(bytes, 1664);
            memcpy(bytes + 1664, &crc, sizeof(crc));
            memcpy(sStorage + PLAYER_STATS_SRAM_OFFSET + (file * 2 + copy) * PLAYER_STATS_SAVE_STRIDE,
                   bytes, sizeof(bytes));
        }
    }
    player_stats_init(read_storage, write_storage);
    for (int file = 0; file < PLAYER_STATS_SAVE_FILES; file++) {
        player_stats_select_file(file);
        for (int id = 1; id <= 148; id++) {
            assert(gPlayerStats[file].values[id] == (u64) id * 100 + file * 10 + 1);
            assert(gPlayerStats[file].levels[id] == id + 10);
            assert(gPlayerStats[file].areas[id] == id % 4 + 1);
        }
        assert(gPlayerStats[file].values[PS_MOST_ENEMIES_DEFEATED_IN_ONE_VISIT] == 0);
        for (int id = 149; id < PLAYER_STAT_COUNT; id++) {
            assert(gPlayerStats[file].values[id] == 0);
        }
    }
    // An interrupted migration leaves the previous journal copy readable.
    sWriteLimit = 700;
    assert(!player_stats_checkpoint(0));
    sWriteLimit = -1;
    player_stats_init(read_storage, write_storage);
    player_stats_select_file(0);
    assert(value(PS_DEFEATED_BY_BOWSER_3) == 14501);
    player_stats_record(PS_MOST_ENEMIES_DEFEATED_IN_ONE_VISIT, 3, LEVEL_BOB, 1);
    assert(player_stats_checkpoint(0));
    player_stats_init(read_storage, write_storage);
    player_stats_select_file(0);
    assert(value(PS_MOST_ENEMIES_DEFEATED_IN_ONE_VISIT) == 3);
    assert(value(PS_DEFEATED_BY_BOWSER_3) == 14501);
    sStorage[PLAYER_STATS_SRAM_OFFSET + 50] ^= 1;
    player_stats_init(read_storage, write_storage);
    player_stats_select_file(0);
    assert(value(PS_MOST_ENEMIES_DEFEATED_IN_ONE_VISIT) == 3);
}

static void migration_v3(void) {
    for (int copy = 0; copy < 2; copy++) {
        u8 bytes[1616] = { 0 };
        u32 header[] = { 0x50535431u, 3, 10 + copy, 1576, 0, 0 };
        memcpy(bytes, header, sizeof(header));
        for (int id = 1; id <= 142; id++) {
            u64 count = id * 100 + copy;
            u16 level = id + 10;
            memcpy(bytes + 24 + id * 8, &count, sizeof(count));
            memcpy(bytes + 1168 + id * 2, &level, sizeof(level));
            bytes[1454 + id] = id % 4 + 1;
        }
        u32 crc = fixture_crc(bytes, 1600);
        memcpy(bytes + 1600, &crc, sizeof(crc));
        memcpy(sStorage + 0x4000 + copy * 0x800, bytes, sizeof(bytes));
    }
    player_stats_init(read_storage, write_storage);
    player_stats_select_file(0);
    for (int id = 1; id <= 142; id++) {
        assert(value(id) == (u64) id * 100 + 1);
        assert(gPlayerStats[0].levels[id] == id + 10);
        assert(gPlayerStats[0].areas[id] == id % 4 + 1);
    }
    assert(value(PS_DEFEATED_BY_BOWSER_1) == 0);
    sWriteLimit = 700;
    assert(!player_stats_checkpoint(0));
    sWriteLimit = -1;
    player_stats_init(read_storage, write_storage);
    player_stats_select_file(0);
    assert(value(PS_HE_THICC_SWIMMING_STROKES) == 14201);
    player_stats_add(PS_DEFEATED_BY_BOWSER_2, 2);
    assert(player_stats_checkpoint(0));
    player_stats_init(read_storage, write_storage);
    player_stats_select_file(0);
    assert(value(PS_HE_THICC_SWIMMING_STROKES) == 14201);
    assert(value(PS_DEFEATED_BY_BOWSER_2) == 2);
    // Either new backup can survive corruption after migration.
    sStorage[0x4000 + 50] ^= 1;
    player_stats_init(read_storage, write_storage);
    player_stats_select_file(0);
    assert(value(PS_DEFEATED_BY_BOWSER_2) == 2);
}

static void migration_v2(void) {
    s32 file;
    s32 id;
    for (file = 0; file < PLAYER_STATS_SAVE_FILES; file++) {
        u8 bytes[1504] = { 0 };
        u32 header[] = { 0x50535431u, 2, 10, 1464, (u32) file, 0 };
        u32 crc;
        memcpy(bytes, header, sizeof(header));
        for (id = 1; id <= 132; id++) {
            u64 count = (u64) id * 100 + file;
            u16 level = id + 10;
            memcpy(bytes + 24 + id * 8, &count, sizeof(count));
            memcpy(bytes + 1088 + id * 2, &level, sizeof(level));
            bytes[1354 + id] = id % 4 + 1;
        }
        crc = fixture_crc(bytes, 1488);
        memcpy(bytes + 1488, &crc, sizeof(crc));
        memcpy(sStorage + 0x1000 + file * 2 * 0x600, bytes, sizeof(bytes));
    }
    player_stats_init(read_storage, write_storage);
    // Import all slots, including the tail of the old region, before retiring any.
    for (file = 0; file < PLAYER_STATS_SAVE_FILES; file++) {
        for (id = 1; id <= 132; id++) {
            assert(gPlayerStats[file].values[id] == (u64) id * 100 + file);
            assert(gPlayerStats[file].levels[id] == id + 10);
            assert(gPlayerStats[file].areas[id] == id % 4 + 1);
        }
        for (id = 133; id < PLAYER_STAT_COUNT; id++) {
            assert(gPlayerStats[file].values[id] == 0);
        }
        assert(player_stats_checkpoint(file));
    }
    player_stats_erase(0);
    player_stats_copy(1, 2);
    // Both new backups corrupted cannot resurrect retired erase/copy histories.
    sStorage[PLAYER_STATS_SRAM_OFFSET + 50] ^= 1;
    sStorage[PLAYER_STATS_SRAM_OFFSET + PLAYER_STATS_SAVE_STRIDE + 50] ^= 1;
    player_stats_init(read_storage, write_storage);
    assert(gPlayerStats[0].values[PS_BOWSER_TIMED_ESCAPES] == 0);
    assert(gPlayerStats[2].values[PS_BOWSER_TIMED_ESCAPES] == 13201);
    assert(gPlayerStats[3].values[PS_BOWSER_TIMED_ESCAPES] == 13203);
}

static void he_thicc_stats(void) {
    s32 id;
    u64 playTime = value(PS_TOTAL_PLAY_TIME);
    for (id = PS_HE_THICC_CLAPS; id <= PS_HE_THICC_SWIMMING_STROKES; id++) {
        player_stats_he_thicc_event(id);
        assert(value(id) == 0);
    }
    player_stats_game_update();
    assert(value(PS_HE_THICC_PLAY_TIME) == 0);
    sPalette = MARIO_PALETTE_HE_THICC;
    player_stats_game_update();
    assert(value(PS_HE_THICC_PLAY_TIME) == 1);
    assert(value(PS_TOTAL_PLAY_TIME) == playTime + 2);
    for (id = PS_HE_THICC_CLAPS; id <= PS_HE_THICC_SWIMMING_STROKES; id++) {
        player_stats_he_thicc_event(id);
        assert(value(id) == 1);
    }
    gCurrDemoInput = &sMario;
    player_stats_he_thicc_event(PS_HE_THICC_WAVEDASHES);
    player_stats_game_update();
    gCurrDemoInput = NULL;
    gCurrCreditsEntry = &sMario;
    player_stats_he_thicc_event(PS_HE_THICC_WAVEDASHES);
    player_stats_game_update();
    gCurrCreditsEntry = NULL;
    player_stats_game_update();
    assert(value(PS_HE_THICC_WAVEDASHES) == 1);
    assert(value(PS_HE_THICC_PLAY_TIME) == 2);
    sPalette = MARIO_PALETTE_STANDARD;
    player_stats_he_thicc_event(PS_HE_THICC_WAVEDASHES);
    player_stats_game_update();
    assert(value(PS_HE_THICC_WAVEDASHES) == 1);
    assert(value(PS_HE_THICC_PLAY_TIME) == 2);
    sPalette = MARIO_PALETTE_HE_THICC;
    gCurrSaveFileNum = 2;
    player_stats_he_thicc_event(PS_HE_THICC_WAVEDASHES);
    assert(gPlayerStats[1].values[PS_HE_THICC_WAVEDASHES] == 0);
    player_stats_game_update();
    player_stats_he_thicc_event(PS_HE_THICC_WAVEDASHES);
    assert(gPlayerStats[1].values[PS_HE_THICC_WAVEDASHES] == 1);
    assert(player_stats_checkpoint(1));
    player_stats_init(read_storage, write_storage);
    assert(gPlayerStats[1].values[PS_HE_THICC_WAVEDASHES] == 1);
}

static void legacy_retirement(void) {
    legacy_fixture(0, 0, 7);
    player_stats_init(read_storage, write_storage);
    sLegacyWriteFailure = TRUE;
    assert(!player_stats_retire_legacy(0));
    assert_legacy_preserved();
    sLegacyWriteFailure = FALSE;
    assert(player_stats_retire_legacy(0));
    assert_legacy_preserved();
    u32 magic;
    memcpy(&magic, sStorage + PLAYER_STATS_LEGACY_SRAM_OFFSET, sizeof(magic));
    assert(magic == 0);
    // Retired banks cannot be returned to a writer after an unread initial load.
    sReadFailure = TRUE;
    player_stats_init(read_storage, write_storage);
    sReadFailure = FALSE;
    s32 writes = sWrites;
    assert(!player_stats_retire_legacy(0) && sWrites == writes);
    player_stats_init(read_storage, write_storage);
    assert(player_stats_retire_legacy(0));
    assert_legacy_preserved();
    assert(player_stats_retire_legacy(3));
    // An empty file also creates and verifies both larger backups before reuse.
    for (s32 bank = 0; bank < 2; bank++) {
        memcpy(&magic, sStorage + PLAYER_STATS_SRAM_OFFSET + (3 * 2 + bank) * PLAYER_STATS_SAVE_STRIDE,
               sizeof(magic));
        assert(magic == 0x50535431u);
    }
    assert(!player_stats_retire_legacy(-1));
    sReadFailure = TRUE;
    player_stats_init(read_storage, write_storage);
    sReadFailure = FALSE;
    assert(!player_stats_retire_legacy(3));
    player_stats_erase(3);
    assert(player_stats_retire_legacy(3));
    player_stats_init(read_storage, write_storage);
    assert_legacy_preserved();
}

static void migration_failure(void) {
    legacy_fixture(0, 0, 7);
    player_stats_init(read_storage, write_storage);
    player_stats_select_file(0);
    sWriteLimit = 700;
    assert(!player_stats_checkpoint(0));
    assert(player_stats_is_dirty(0));
    player_stats_init(read_storage, write_storage);
    player_stats_select_file(0);
    assert_legacy_preserved();
    sWriteLimit = -1;
    assert(player_stats_checkpoint(0));
    // Failed legacy retirement leaves both new copies safe and retryable.
    sLegacyWriteFailure = TRUE;
    assert(!player_stats_checkpoint(0));
    assert(player_stats_is_dirty(0));
    player_stats_init(read_storage, write_storage);
    player_stats_select_file(0);
    assert_legacy_preserved();
    assert(player_stats_is_dirty(0));
    sLegacyWriteFailure = FALSE;
    assert(player_stats_checkpoint(0));
    assert(!player_stats_is_dirty(0));
    player_stats_copy(0, 2);
    assert(gPlayerStats[2].values[PS_LONGEST_SINGLE_LONG_JUMP] == 5400);
    player_stats_erase(0);
    player_stats_init(read_storage, write_storage);
    assert(value(PS_BREAKDANCE_SPINS) == 0);
    assert(gPlayerStats[2].values[PS_BREAKDANCE_SPINS] == 11400);
}

static void dabs_pose(void) {
    action(ACT_DAB, 0);
    player_stats_mario_begin(&sMario);
    player_stats_mario_end(&sMario);
    player_stats_game_update();
    action(ACT_DAB, 0);
    player_stats_mario_begin(&sMario);
    player_stats_mario_end(&sMario);
    assert(value(PS_DABS_PERFORMED) == 2);
    assert(value(PS_TIME_SPENT_DABBING) == 2);
    action(ACT_START_SLEEPING, 110);
    assert(value(PS_DAB_INDUCED_SLEEPS) == 1);
    action(ACT_IDLE, 0);
    action(ACT_START_SLEEPING, 0);
    assert(value(PS_TIMES_MARIO_FELL_ASLEEP) == 2);
    assert(value(PS_DAB_INDUCED_SLEEPS) == 1);
    action(ACT_GROUND_TWIRL, 0);
    player_stats_mario_end(&sMario);
    assert(value(PS_TIME_HOLDING_TWIRL_POSE) == 0);
    sHeldPose = TRUE;
    player_stats_mario_end(&sMario);
    assert(value(PS_TIME_HOLDING_TWIRL_POSE) == 1);
    action(ACT_WALKING, 0);
    player_stats_mario_end(&sMario);
    assert(value(PS_TIME_HOLDING_TWIRL_POSE) == 1);
}

static void twirl_chains(void) {
    player_stats_twirl_start(FALSE, FALSE);
    player_stats_twirl_wall_rebound(FALSE);
    player_stats_twirl_start(TRUE, TRUE);
    player_stats_twirl_wall_rebound(TRUE);
    player_stats_twirl_wall_rebound(TRUE);
    assert(value(PS_GROUND_TWIRLS) == 1 && value(PS_AIR_TWIRLS) == 0);
    assert(value(PS_TWIRL_WALL_REBOUNDS) == 3);
    assert(value(PS_GROUND_TWIRL_WALL_REBOUNDS) == 1);
    assert(value(PS_AIR_TWIRL_WALL_REBOUNDS) == 2);
    assert(value(PS_LONGEST_TWIRL_WALL_REBOUND_CHAIN) == 3);
    player_stats_twirl_start(TRUE, FALSE);
    player_stats_twirl_wall_rebound(TRUE);
    assert(value(PS_AIR_TWIRLS) == 1);
    assert(value(PS_LONGEST_TWIRL_WALL_REBOUND_CHAIN) == 3);
    player_stats_reset_movement();
    player_stats_twirl_wall_rebound(TRUE);
    player_stats_twirl_wall_rebound(TRUE);
    assert(value(PS_LONGEST_TWIRL_WALL_REBOUND_CHAIN) == 3);
    assert(gPlayerStats[0].levels[PS_LONGEST_TWIRL_WALL_REBOUND_CHAIN] == LEVEL_BOB);
}

static void water_walk(void) {
    sMario.flags |= MARIO_WING_CAP;
    sMario.floor = &gWaterSurfacePseudoFloor;
    player_stats_mario_begin(&sMario);
    sMario.pos[0] = 3.0f;
    sMario.pos[2] = 4.0f;
    sMario.pos[1] = sMario.floorHeight = 12.0f;
    player_stats_mario_end(&sMario);
    assert(value(PS_TIME_WATER_WALKING) == 1);
    assert(value(PS_DISTANCE_WATER_WALKED) == 5 * 256);
    assert(value(PS_DISTANCE_WATER_WALKED_3D) == 13 * 256);
    player_stats_mario_begin(&sMario);
    player_stats_mario_end(&sMario);
    assert(value(PS_TIME_WATER_WALKING) == 2);
    assert(value(PS_DISTANCE_WATER_WALKED) == 5 * 256);
    action(ACT_FREEFALL, 0);
    sMario.pos[0] += 100.0f;
    player_stats_mario_end(&sMario);
    assert(value(PS_TIME_WATER_WALKING) == 2);
    assert(value(PS_LONGEST_CONTINUOUS_WATER_WALK) == 5 * 256);
    action(ACT_WALKING, 0);
    sMario.flags &= ~MARIO_WING_CAP;
    player_stats_mario_end(&sMario);
    assert(value(PS_TIME_WATER_WALKING) == 2);
    sMario.flags |= MARIO_WING_CAP;
    sMario.riddenObj = &gObjectPool[0];
    player_stats_mario_end(&sMario);
    assert(value(PS_TIME_WATER_WALKING) == 2);
    sMario.riddenObj = NULL;
    player_stats_mario_begin(&sMario);
    sMario.pos[0] += 6.0f;
    player_stats_mario_end(&sMario);
    player_stats_warp(LEVEL_BOB, FALSE);
    assert(value(PS_LONGEST_CONTINUOUS_WATER_WALK) == 6 * 256);
    player_stats_mario_end(&sMario);
    assert(value(PS_DISTANCE_WATER_WALKED) == 11 * 256);
}

static void metal_origin(void) {
    struct Object *obj = &gObjectPool[0];
    obj->behavior = bhvGoomba;
    obj->oHealth = 1;
    player_stats_object_reset(obj);
    player_stats_metal_contact(obj, TRUE);
    assert(value(PS_ENEMIES_DEFEATED_BY_PASSIVE_METAL_CONTACT) == 0);
    player_stats_game_update();
    player_stats_metal_defeat(obj);
    player_stats_enemy_defeated(obj);
    player_stats_enemy_defeated(obj);
    assert(value(PS_TOTAL_ENEMIES_DEFEATED) == 1);
    assert(value(PS_ENEMIES_DEFEATED_BY_PASSIVE_METAL_CONTACT) == 1);
    player_stats_object_reset(obj);
    player_stats_twirl_hit(obj);
    player_stats_metal_contact(obj, FALSE);
    player_stats_game_update();
    player_stats_metal_defeat(obj);
    player_stats_enemy_defeated(obj);
    assert(value(PS_TOTAL_ENEMIES_DEFEATED) == 2);
    assert(value(PS_ENEMIES_DEFEATED_BY_PASSIVE_METAL_CONTACT) == 1);
    player_stats_object_reset(obj);
    player_stats_metal_contact(obj, TRUE);
    player_stats_attack(obj, ATTACK_PUNCH);
    player_stats_enemy_defeated(obj);
    assert(value(PS_ENEMIES_DEFEATED_BY_PASSIVE_METAL_CONTACT) == 1);
    assert(value(PS_ENEMIES_PUNCHED) == 1);
}

static void cap_powers(void) {
    player_stats_cap_powers_removed(MARIO_NORMAL_CAP | MARIO_CAP_ON_HEAD, FALSE);
    assert(value(PS_CAP_POWERS_LOST_TO_DAMAGE) == 0);
    player_stats_cap_powers_removed(MARIO_WING_CAP | MARIO_METAL_CAP | MARIO_VANISH_CAP, TRUE);
    assert(value(PS_POWER_CAPS_SHED_BY_BREAKDANCE) == 3);
    player_stats_cap_powers_removed(MARIO_WING_CAP, TRUE);
    assert(value(PS_POWER_CAPS_SHED_BY_BREAKDANCE) == 4);
    player_stats_cap_powers_removed(MARIO_METAL_CAP | MARIO_VANISH_CAP, FALSE);
    assert(value(PS_CAP_POWERS_LOST_TO_DAMAGE) == 2);
    player_stats_cap_powers_removed(0, FALSE);
    assert(value(PS_CAP_POWERS_LOST_TO_DAMAGE) == 2);
}

int main(int argc, char **argv) {
    struct Test {
        const char *name;
        void (*run)(void);
    } tests[] = {
        { "counters", counters },
        { "cheating", cheating },
        { "cheating_visits", cheating_visits },
        { "migration", migration },
        { "migration_v2", migration_v2 },
        { "migration_v3", migration_v3 },
        { "migration_v4", migration_v4 },
        { "migration_mixed_v4", migration_mixed_v4 },
        { "migration_v5", migration_v5 },
        { "damage_free_visits", damage_free_visits },
        { "activity_timers", activity_timers },
        { "new_defeat_records", new_defeat_records },
        { "cannon_flight_record", cannon_flight_record },
        { "migration_v6", migration_v6 },
        { "enemy_bounce_streak", enemy_bounce_streak },
        { "seconds_soaked", seconds_soaked },
        { "bowser_defeats", bowser_defeats },
        { "he_thicc_stats", he_thicc_stats },
        { "migration_failure", migration_failure },
        { "legacy_retirement", legacy_retirement },
        { "dabs_pose", dabs_pose },
        { "twirl_chains", twirl_chains },
        { "water_walk", water_walk },
        { "metal_origin", metal_origin },
        { "cap_powers", cap_powers },
        { "journal", journal },
        { "torn_write", torn_write },
        { "retry_write", retry_write },
        { "verify_failure", verify_failure },
        { "initial_read_failure", initial_read_failure },
        { "copy_read_failure", copy_read_failure },
        { "copy_erase", copy_erase },
        { "slot_identity", slot_identity },
        { "clock_and_visits", clock_and_visits },
        { "lab_time", lab_time },
        { "movement", movement },
        { "warp_exit_actions", warp_exit_actions },
        { "twirl_long_jump", twirl_long_jump },
        { "assisted_airtime", assisted_airtime },
        { "survived_falls", survived_falls },
        { "coins_stars", coins_stars },
        { "enemy_credit", enemy_credit },
        { "health_and_hits", health_and_hits },
        { "checkpoints", checkpoints },
        { "completed_sequences", completed_sequences },
        { "indirect_defeat", indirect_defeat },
    };
    unsigned int i;
    assert(argc == 2);
    setup();
    for (i = 0; i < sizeof(tests) / sizeof(tests[0]); i++) {
        if (strcmp(argv[1], tests[i].name) == 0) {
            tests[i].run();
            printf("%s passed\n", tests[i].name);
            return 0;
        }
    }
    return 1;
}
