#ifndef PLAYER_STATS_H
#define PLAYER_STATS_H

#include <PR/ultratypes.h>

#define PLAYER_STATS_SAVE_FILES 4
#define PLAYER_STATS_LEGACY_SRAM_OFFSET 0x1000
#define PLAYER_STATS_LEGACY_SAVE_STRIDE 0x600
#define PLAYER_STATS_SRAM_OFFSET 0x4000
#define PLAYER_STATS_SAVE_STRIDE 0x800
#define PLAYER_STATS_FIXED_SCALE 256
#define PLAYER_STATS_SAVE_VERSION 7

// Stored time counters use gameplay frames at the regional rate.
#ifdef VERSION_EU
#define PLAYER_STATS_FRAMES_PER_SECOND 25
#else
#define PLAYER_STATS_FRAMES_PER_SECOND 30
#endif

enum PlayerStatId {
    PS_NONE = 0,
    PS_TOTAL_PLAY_TIME = 1,
    PS_TIME_SPENT_IN_PEACH_S_CASTLE = 2,
    PS_TIME_SPENT_IN_COURSES = 3,
    PS_COURSE_ENTRIES = 4,
    PS_COURSE_EXITS_WITHOUT_COLLECTING_A_STAR = 5,
    PS_TOTAL_POWER_STARS_COLLECTED = 6,
    PS_UNIQUE_POWER_STARS_COLLECTED = 7,
    PS_PREVIOUSLY_COLLECTED_STARS_RECOLLECTED = 8,
    PS_LIVES_LOST = 9,
    PS_GAME_OVERS = 10,
    PS_TOTAL_COIN_VALUE_COLLECTED = 11,
    PS_YELLOW_COINS_COLLECTED = 12,
    PS_RED_COINS_COLLECTED = 13,
    PS_BLUE_COINS_COLLECTED = 14,
    PS_HIGHEST_COIN_TOTAL_IN_A_SINGLE_COURSE_VISIT = 15,
    PS_100_COIN_STARS_COLLECTED = 16,
    PS_RED_COIN_STARS_COLLECTED = 17,
    PS_CASTLE_SECRET_STARS_COLLECTED = 18,
    PS_1_UP_MUSHROOMS_COLLECTED = 19,
    PS_WING_CAPS_COLLECTED = 20,
    PS_METAL_CAPS_COLLECTED = 21,
    PS_VANISH_CAPS_COLLECTED = 22,
    PS_EXCLAMATION_BLOCKS_BROKEN = 23,
    PS_HIDDEN_SECRETS_ACTIVATED = 24,
    PS_TOTAL_JUMPS_PERFORMED = 25,
    PS_DOUBLE_JUMPS = 26,
    PS_TRIPLE_JUMPS = 27,
    PS_LONG_JUMPS = 28,
    PS_BACKFLIPS = 29,
    PS_SIDE_SOMERSAULTS = 30,
    PS_WALL_KICKS = 31,
    PS_GROUND_POUNDS = 32,
    PS_DIVES = 33,
    PS_DIVE_ROLLOUTS = 34,
    PS_JUMP_KICKS = 35,
    PS_SLIDE_KICKS = 36,
    PS_PUNCHES_THROWN = 37,
    PS_GROUND_KICKS_THROWN = 38,
    PS_CRAWLS_STARTED = 39,
    PS_LEDGE_GRABS = 40,
    PS_POLE_GRABS = 41,
    PS_SWIMMING_STROKES = 42,
    PS_WATER_JUMPS = 43,
    PS_TOTAL_DISTANCE_TRAVELED = 44,
    PS_DISTANCE_TRAVELED_ON_FOOT = 45,
    PS_DISTANCE_TRAVELED_IN_THE_AIR = 46,
    PS_DISTANCE_CRAWLED = 47,
    PS_DISTANCE_SWUM = 48,
    PS_DISTANCE_SLID = 49,
    PS_DISTANCE_FLOWN = 50,
    PS_DISTANCE_TRAVELED_RIDING_SHELLS = 51,
    PS_FASTEST_GROUND_SPEED = 52,
    PS_FASTEST_AIRBORNE_SPEED = 53,
    PS_LONGEST_SINGLE_LONG_JUMP = 54,
    PS_LONGEST_AIR_TIME_WITHOUT_WING_CAP = 55,
    PS_GREATEST_FALL_SURVIVED = 56,
    PS_LONGEST_WALL_KICK_CHAIN = 57,
    PS_LONGEST_CONTINUOUS_SLIDE = 58,
    PS_LONGEST_CONTINUOUS_SHELL_RIDE = 59,
    PS_TOTAL_ENEMIES_DEFEATED = 60,
    PS_ENEMIES_STOMPED = 61,
    PS_ENEMIES_PUNCHED = 62,
    PS_ENEMIES_KICKED = 63,
    PS_ENEMIES_GROUND_POUNDED = 64,
    PS_ENEMIES_DEFEATED_WHILE_RIDING_SHELLS = 65,
    PS_GOOMBAS_DEFEATED = 66,
    PS_BOOS_DEFEATED = 67,
    PS_BULLIES_KNOCKED_INTO_LAVA = 68,
    PS_OBJECTS_PICKED_UP = 69,
    PS_OBJECTS_THROWN = 70,
    PS_BOWSER_TAIL_GRABS = 71,
    PS_BOWSER_THROWS = 72,
    PS_BOWSER_BOMB_HITS = 73,
    PS_HITS_TAKEN = 74,
    PS_POWER_METER_WEDGES_LOST = 75,
    PS_POWER_METER_WEDGES_RESTORED = 76,
    PS_WALL_BONKS = 77,
    PS_FALL_DAMAGE_INCIDENTS = 78,
    PS_LAVA_BURNS = 79,
    PS_FIRE_BURNS = 80,
    PS_ELECTRIC_SHOCKS = 81,
    PS_TIMES_CRUSHED = 82,
    PS_DROWNING_DEATHS = 83,
    PS_FALLS_INTO_BOTTOMLESS_PITS = 84,
    PS_QUICKSAND_DEATHS = 85,
    PS_CANNON_LAUNCHES = 86,
    PS_HIDDEN_TELEPORTERS_USED = 87,
    PS_WARP_PIPES_ENTERED = 88,
    PS_KOOPA_SHELLS_MOUNTED = 89,
    PS_TREES_CLIMBED = 90,
    PS_SIGNS_READ = 91,
    PS_TOAD_CONVERSATIONS = 92,
    PS_STAR_DOORS_UNLOCKED = 93,
    PS_MIPS_CAUGHT = 94,
    PS_BABY_PENGUINS_RETURNED = 95,
    PS_RACES_ENTERED = 96,
    PS_RACES_WON = 97,
    PS_TIMES_MARIO_S_CAP_LOST = 98,
    PS_TIMES_MARIO_S_CAP_RECOVERED = 99,
    PS_TIMES_MARIO_FELL_ASLEEP = 100,
    PS_TIME_SPENT_IN_DIALOGUE = 101,
    PS_TOTAL_DISTANCE_TRAVELED_3D = 102,
    PS_DISTANCE_TRAVELED_ON_FOOT_3D = 103,
    PS_DISTANCE_TRAVELED_IN_THE_AIR_3D = 104,
    PS_DISTANCE_CRAWLED_3D = 105,
    PS_DISTANCE_SWUM_3D = 106,
    PS_DISTANCE_SLID_3D = 107,
    PS_DISTANCE_FLOWN_3D = 108,
    PS_DISTANCE_TRAVELED_RIDING_SHELLS_3D = 109,
    PS_GROUND_TWIRLS = 110,
    PS_AIR_TWIRLS = 111,
    PS_TWIRL_WALL_REBOUNDS = 112,
    PS_TWIRL_ENEMY_HITS = 113,
    PS_BREAKDANCE_SPINS = 114,
    PS_DABS_PERFORMED = 115,
    PS_TIME_SPENT_DABBING = 116,
    PS_DAB_INDUCED_SLEEPS = 117,
    PS_GROUND_TWIRL_WALL_REBOUNDS = 118,
    PS_AIR_TWIRL_WALL_REBOUNDS = 119,
    PS_LONGEST_TWIRL_WALL_REBOUND_CHAIN = 120,
    PS_TIME_HOLDING_TWIRL_POSE = 121,
    PS_DISTANCE_WATER_WALKED = 122,
    PS_DISTANCE_WATER_WALKED_3D = 123,
    PS_TIME_WATER_WALKING = 124,
    PS_LONGEST_CONTINUOUS_WATER_WALK = 125,
    PS_SLIDE_KICK_LIQUID_BOUNCES = 126,
    PS_DOWNWARD_SWIMMING_BURSTS = 127,
    PS_ENEMIES_STUNNED_BY_TWIRLS = 128,
    PS_ENEMIES_DEFEATED_BY_PASSIVE_METAL_CONTACT = 129,
    PS_POWER_CAPS_SHED_BY_BREAKDANCE = 130,
    PS_CAP_POWERS_LOST_TO_DAMAGE = 131,
    PS_BOWSER_TIMED_ESCAPES = 132,
    PS_HE_THICC_PLAY_TIME = 133,
    PS_HE_THICC_CLAPS = 134,
    PS_HE_THICC_WAVEDASHES = 135,
    PS_HE_THICC_BELLY_SLAMS = 136,
    PS_HE_THICC_BELLY_BOUNCES = 137,
    PS_HE_THICC_POUND_BOUNCES = 138,
    PS_HE_THICC_WALL_KICKS = 139,
    PS_HE_THICC_SLIDE_KICK_BOUNCES = 140,
    PS_HE_THICC_EXPLOSIVE_STRIKES = 141,
    PS_HE_THICC_SWIMMING_STROKES = 142,
    PS_DEFEATED_BY_BOWSER_1 = 143,
    PS_DEFEATED_BY_BOWSER_2 = 144,
    PS_DEFEATED_BY_BOWSER_3 = 145,
    PS_MISSED_BOWSER_THROWS = 146,
    PS_ENEMY_BOUNCE_STREAK = 147,
    PS_SECONDS_SOAKED = 148,
    PS_LONGEST_DAMAGE_FREE_STREAK = 149,
    PS_STARS_COLLECTED_WITHOUT_DAMAGE = 150,
    PS_MOST_ENEMIES_DEFEATED_IN_ONE_VISIT = 151,
    PS_BREAKDANCE_DEFEATS = 152,
    PS_THROWN_OBJECT_DEFEATS = 153,
    PS_TIME_HOLDING_OBJECTS = 154,
    PS_DISTANCE_TRAVELED_WHILE_SOAKED = 155,
    PS_FARTHEST_CANNON_FLIGHT = 156,
    PS_TIME_USING_WING_CAP = 157,
    PS_TIME_USING_METAL_CAP = 158,
    PS_TIME_USING_VANISH_CAP = 159,
    PLAYER_STAT_COUNT
};

enum PlayerStatCategory {
    PS_CATEGORY_GENERAL = 0,
    PS_CATEGORY_COLLECTIBLES = 1,
    PS_CATEGORY_MOVES = 2,
    PS_CATEGORY_DISTANCE = 3,
    PS_CATEGORY_COMBAT = 4,
    PS_CATEGORY_DAMAGE = 5,
    PS_CATEGORY_WORLD = 6,
    PS_CATEGORY_HE_THICC = 7,
    PS_CATEGORY_COUNT = 8,
};

enum PlayerStatFormat {
    PLAYER_STATS_COUNT,
    PLAYER_STATS_TIME,
    PLAYER_STATS_DISTANCE,
    PLAYER_STATS_SPEED,
    PLAYER_STATS_WEDGES,
    PLAYER_STATS_SECONDS
};

struct PlayerStatInfo {
    const char *label;
    u8 category;
    u8 format;
    u8 record;
};

/* Distances/speeds and wedges use 1/256 units; times use gameplay frames. */
struct PlayerStatsData {
    u64 values[PLAYER_STAT_COUNT];
    u16 levels[PLAYER_STAT_COUNT];
    u8 areas[PLAYER_STAT_COUNT];
};

// SRAM offsets and sizes are bytes; callbacks return zero on success.
typedef s32 (*PlayerStatsRead)(u32 offset, void *data, u32 size);
typedef s32 (*PlayerStatsWrite)(u32 offset, const void *data, u32 size);

extern struct PlayerStatsData gPlayerStats[PLAYER_STATS_SAVE_FILES];
extern const struct PlayerStatInfo gPlayerStatInfo[PLAYER_STAT_COUNT];

void player_stats_init(PlayerStatsRead read, PlayerStatsWrite write);
// File indices are zero-based; an invalid selection disables statistic updates.
void player_stats_select_file(s32 fileIndex);
s32 player_stats_active_file(void);
void player_stats_add(enum PlayerStatId id, u64 amount);
void player_stats_record(enum PlayerStatId id, u64 value, u16 level, u8 area);
// TRUE means clean or successfully saved/read back. FALSE means invalid file or I/O failure;
// a dirty valid file retains its pending work for a later retry.
// Initial read failures block writes until successful reload or explicit copy/erase.
s32 player_stats_checkpoint(s32 fileIndex);
// Reclaim old banks only after both new copies and legacy retirement are verified.
// An initial transport failure blocks reuse until reload or explicit file replacement.
s32 player_stats_retire_legacy(s32 fileIndex);
void player_stats_erase(s32 fileIndex);
// Readable blank files qualify; initial read failures forbid copying until reload or replacement.
s32 player_stats_can_copy(s32 source);
void player_stats_copy(s32 source, s32 destination);
s32 player_stats_is_dirty(s32 fileIndex);

struct MarioState;
struct Object;

/* Successful HE THICC move events; active palette and real gameplay are required. */
void player_stats_he_thicc_event(enum PlayerStatId id);
// Once per normal-play or frozen-entry-dialogue frame, before world updates.
// Synchronizes file/visit identity even while counting is blocked.
void player_stats_game_update(void);
// Bracket one Mario update; action redispatches do not advance these samples.
void player_stats_mario_begin(struct MarioState *m);
void player_stats_mario_end(struct MarioState *m);
void player_stats_reset_movement(void);
// Mario initialization releases the warp/death visit latch.
void player_stats_level_entered(void);
void player_stats_wall_bonk(struct MarioState *m);
void player_stats_warp(s16 destination, s32 exitCourse);
void player_stats_death(s32 gameOver, s32 pit);
// Called before committing action/actionArg; m->action still identifies the old action.
void player_stats_action(struct MarioState *m, u32 action, u32 actionArg);
void player_stats_damage(void);
void player_stats_health(s32 before, s32 after);
/* Respawn refill ticks still count damage, but their free healing is excluded. */
void player_stats_health_non_restoring(s32 before, s32 after);
void player_stats_fall_survived(struct MarioState *m);
void player_stats_enemy_bounce(struct Object *obj);
void player_stats_ground_contact(void);
void player_stats_punch(u32 actionArg);
void player_stats_coin(struct Object *obj, s32 courseCoins);
void player_stats_star(struct Object *obj, u32 starIndex);
// Clear provenance and credit for a newly initialized or revived object.
void player_stats_object_reset(struct Object *obj);
// Tag temporary per-file attack credit; enemy_defeated confirms and counts a kill.
void player_stats_attack(struct Object *obj, s32 attackType);
void player_stats_enemy_defeated(struct Object *obj);
// Confirm a scripted defeat after dialogue without replacing its accepted attack.
void player_stats_enemy_defeated_after_dialogue(struct Object *obj);
void player_stats_inherit_attack(struct Object *source, struct Object *target);
void player_stats_tag_star(struct Object *obj, s32 redCoins);
void player_stats_grab(struct Object *obj);
void player_stats_throw(struct Object *obj);
void player_stats_twirl_hit(struct Object *obj);
void player_stats_twirl_start(s32 airborne, s32 continuation);
void player_stats_twirl_wall_rebound(s32 airborne);
void player_stats_metal_contact(struct Object *obj, s32 passive);
void player_stats_metal_defeat(struct Object *obj);
/* Count each active Wing/Metal/Vanish power actually removed, including combinations. */
void player_stats_cap_powers_removed(u32 powers, s32 breakdance);
void player_stats_cap_lost(void);
void player_stats_cap_recovered(void);

#endif
