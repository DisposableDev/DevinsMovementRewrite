#include "player_stats.h"
#include "game_options_runtime.h"

#include <stddef.h>
#include <string.h>

#define PLAYER_STATS_MAGIC 0x50535431u
#define PLAYER_STATS_MAX_VALUE (~(u64) 0)
#define PLAYER_STATS_BOTH_COPIES 3

struct PlayerStatsSave {
    u32 magic;
    u32 version;
    u32 generation;
    u32 payloadSize;
    u32 fileIndex;
    u32 headerReserved;
    struct PlayerStatsData data;
    u32 checksum;
    u32 reserved;
    u32 cachePadding[4];
};

/* Version 1 shipped IDs 0..114. Array offsets change when IDs are appended. */
struct PlayerStatsDataV1 {
    u64 values[115];
    u16 levels[115];
    u8 areas[115];
};

struct PlayerStatsSaveV1 {
    u32 magic;
    u32 version;
    u32 generation;
    u32 payloadSize;
    u32 fileIndex;
    u32 headerReserved;
    struct PlayerStatsDataV1 data;
    u32 checksum;
    u32 reserved;
    u32 cachePadding[2];
};

struct PlayerStatsDataV2 {
    u64 values[133];
    u16 levels[133];
    u8 areas[133];
};

struct PlayerStatsSaveV2 {
    u32 magic;
    u32 version;
    u32 generation;
    u32 payloadSize;
    u32 fileIndex;
    u32 headerReserved;
    struct PlayerStatsDataV2 data;
    u32 checksum;
    u32 reserved;
    u32 cachePadding[2];
};

struct PlayerStatsDataV3 {
    u64 values[143];
    u16 levels[143];
    u8 areas[143];
};

struct PlayerStatsSaveV3 {
    u32 magic;
    u32 version;
    u32 generation;
    u32 payloadSize;
    u32 fileIndex;
    u32 headerReserved;
    struct PlayerStatsDataV3 data;
    u32 checksum;
    u32 reserved;
    u32 cachePadding[2];
};

struct PlayerStatsDataV4 {
    u64 values[146];
    u16 levels[146];
    u8 areas[146];
};

struct PlayerStatsSaveV4 {
    u32 magic;
    u32 version;
    u32 generation;
    u32 payloadSize;
    u32 fileIndex;
    u32 headerReserved;
    struct PlayerStatsDataV4 data;
    u32 checksum;
    u32 reserved;
    u32 cachePadding[2];
};

struct PlayerStatsDataV5 {
    u64 values[147];
    u16 levels[147];
    u8 areas[147];
};

struct PlayerStatsSaveV5 {
    u32 magic;
    u32 version;
    u32 generation;
    u32 payloadSize;
    u32 fileIndex;
    u32 headerReserved;
    struct PlayerStatsDataV5 data;
    u32 checksum;
    u32 reserved;
    u32 cachePadding[2];
};

struct PlayerStatsDataV6 {
    u64 values[149];
    u16 levels[149];
    u8 areas[149];
};

struct PlayerStatsSaveV6 {
    u32 magic;
    u32 version;
    u32 generation;
    u32 payloadSize;
    u32 fileIndex;
    u32 headerReserved;
    struct PlayerStatsDataV6 data;
    u32 checksum;
    u32 reserved;
    u32 cachePadding[2];
};

union PlayerStatsSaveScratch {
    struct PlayerStatsSave current;
    struct PlayerStatsSaveV1 v1;
    struct PlayerStatsSaveV2 v2;
    struct PlayerStatsSaveV3 v3;
    struct PlayerStatsSaveV4 v4;
    struct PlayerStatsSaveV5 v5;
    struct PlayerStatsSaveV6 v6;
};

_Static_assert(sizeof(struct PlayerStatsSaveV6) == 1680, "Version 6 statistics layout changed");
_Static_assert(offsetof(struct PlayerStatsSaveV6, checksum) == 1664,
               "Version 6 statistics CRC offset changed");
_Static_assert(sizeof(struct PlayerStatsSaveV5) == 1664, "Version 5 statistics layout changed");
_Static_assert(offsetof(struct PlayerStatsSaveV5, checksum) == 1648,
               "Version 5 statistics CRC offset changed");
_Static_assert(sizeof(struct PlayerStatsSaveV4) == 1648, "Version 4 statistics layout changed");
_Static_assert(offsetof(struct PlayerStatsSaveV4, checksum) == 1632,
               "Version 4 statistics CRC offset changed");
_Static_assert(sizeof(struct PlayerStatsSaveV3) == 1616, "Version 3 statistics layout changed");
_Static_assert(offsetof(struct PlayerStatsSaveV3, checksum) == 1600,
               "Version 3 statistics CRC offset changed");
_Static_assert(sizeof(struct PlayerStatsSaveV2) == 1504, "Version 2 statistics layout changed");
_Static_assert(offsetof(struct PlayerStatsSaveV2, checksum) == 1488,
               "Version 2 statistics CRC offset changed");
_Static_assert(PLAYER_STATS_LEGACY_SRAM_OFFSET
                       + PLAYER_STATS_SAVE_FILES * 2 * PLAYER_STATS_LEGACY_SAVE_STRIDE
                   <= PLAYER_STATS_SRAM_OFFSET,
               "Current statistics must preserve legacy SRAM during migration");
_Static_assert(sizeof(struct PlayerStatsSaveV1) == 1312, "Legacy statistics layout changed");
_Static_assert(offsetof(struct PlayerStatsSaveV1, checksum) == 1296,
               "Legacy statistics CRC offset changed");
_Static_assert(sizeof(struct PlayerStatsSave) <= PLAYER_STATS_SAVE_STRIDE,
               "Statistics exceed their SRAM record allocation");
_Static_assert(sizeof(struct PlayerStatsSave) % 16 == 0, "Statistics DMA must be aligned");
_Static_assert(PLAYER_STATS_SRAM_OFFSET + PLAYER_STATS_SAVE_FILES * 2 * PLAYER_STATS_SAVE_STRIDE
                   <= 0x8000,
               "Statistics exceed cartridge SRAM");

struct PlayerStatsData gPlayerStats[PLAYER_STATS_SAVE_FILES];
static PlayerStatsRead sRead;
static PlayerStatsWrite sWrite;
static s32 sActiveFile = -1;
static u8 sDirty[PLAYER_STATS_SAVE_FILES];
// Current-region bank selected by load or the last verified write; -1 if none.
static s8 sSelectedCopy[PLAYER_STATS_SAVE_FILES];
static u32 sGeneration[PLAYER_STATS_SAVE_FILES];
// Bank masks use bits 0/1; retirement/replacement work survives failed-write retries.
static u8 sCurrentFormatCopies[PLAYER_STATS_SAVE_FILES];
static u8 sLegacyCopiesToRetire[PLAYER_STATS_SAVE_FILES];
static u8 sLoadReadFailed[PLAYER_STATS_SAVE_FILES];
/* Erase/copy must replace both histories, including across a failed-write retry. */
static u8 sCopiesToReplace[PLAYER_STATS_SAVE_FILES];
/* DMA scratch is shared only by synchronous main-thread save operations. */
static union PlayerStatsSaveScratch sSaveScratch[2] __attribute__((aligned(16)));

const struct PlayerStatInfo gPlayerStatInfo[PLAYER_STAT_COUNT] = {
    [PS_TOTAL_PLAY_TIME] = { "Total Play Time", PS_CATEGORY_GENERAL, PLAYER_STATS_TIME, 0 },
    [PS_TIME_SPENT_IN_PEACH_S_CASTLE] = { "Time Spent in Peach's Castle", PS_CATEGORY_GENERAL,
                                          PLAYER_STATS_TIME, 0 },
    [PS_TIME_SPENT_IN_COURSES] = { "Time Spent in Courses", PS_CATEGORY_GENERAL, PLAYER_STATS_TIME, 0 },
    [PS_COURSE_ENTRIES] = { "Course Entries", PS_CATEGORY_GENERAL, PLAYER_STATS_COUNT, 0 },
    [PS_COURSE_EXITS_WITHOUT_COLLECTING_A_STAR] = { "Course Exits with No Star", PS_CATEGORY_GENERAL,
                                                    PLAYER_STATS_COUNT, 0 },
    [PS_TOTAL_POWER_STARS_COLLECTED] = { "Total Power Stars Collected", PS_CATEGORY_COLLECTIBLES,
                                         PLAYER_STATS_COUNT, 0 },
    [PS_UNIQUE_POWER_STARS_COLLECTED] = { "Unique Power Stars Collected", PS_CATEGORY_COLLECTIBLES,
                                          PLAYER_STATS_COUNT, 0 },
    [PS_PREVIOUSLY_COLLECTED_STARS_RECOLLECTED] = { "Previously Collected Stars Recollected",
                                                    PS_CATEGORY_COLLECTIBLES, PLAYER_STATS_COUNT, 0 },
    [PS_LIVES_LOST] = { "Lives Lost", PS_CATEGORY_DAMAGE, PLAYER_STATS_COUNT, 0 },
    [PS_GAME_OVERS] = { "Game Overs", PS_CATEGORY_DAMAGE, PLAYER_STATS_COUNT, 0 },
    [PS_TOTAL_COIN_VALUE_COLLECTED] = { "Total Coin Value Collected", PS_CATEGORY_COLLECTIBLES,
                                        PLAYER_STATS_COUNT, 0 },
    [PS_YELLOW_COINS_COLLECTED] = { "Yellow Coins Collected", PS_CATEGORY_COLLECTIBLES,
                                    PLAYER_STATS_COUNT, 0 },
    [PS_RED_COINS_COLLECTED] = { "Red Coins Collected", PS_CATEGORY_COLLECTIBLES, PLAYER_STATS_COUNT,
                                 0 },
    [PS_BLUE_COINS_COLLECTED] = { "Blue Coins Collected", PS_CATEGORY_COLLECTIBLES, PLAYER_STATS_COUNT,
                                  0 },
    [PS_HIGHEST_COIN_TOTAL_IN_A_SINGLE_COURSE_VISIT] = { "Highest Coin Value in One Visit",
                                                         PS_CATEGORY_COLLECTIBLES, PLAYER_STATS_COUNT,
                                                         1 },
    [PS_100_COIN_STARS_COLLECTED] = { "100-Coin Stars Collected", PS_CATEGORY_COLLECTIBLES,
                                      PLAYER_STATS_COUNT, 0 },
    [PS_RED_COIN_STARS_COLLECTED] = { "Red-Coin Stars Collected", PS_CATEGORY_COLLECTIBLES,
                                      PLAYER_STATS_COUNT, 0 },
    [PS_CASTLE_SECRET_STARS_COLLECTED] = { "Castle Secret Stars Collected", PS_CATEGORY_COLLECTIBLES,
                                           PLAYER_STATS_COUNT, 0 },
    [PS_1_UP_MUSHROOMS_COLLECTED] = { "1-Up Mushrooms Collected", PS_CATEGORY_COLLECTIBLES,
                                      PLAYER_STATS_COUNT, 0 },
    [PS_WING_CAPS_COLLECTED] = { "Wing Caps Collected", PS_CATEGORY_COLLECTIBLES, PLAYER_STATS_COUNT,
                                 0 },
    [PS_METAL_CAPS_COLLECTED] = { "Metal Caps Collected", PS_CATEGORY_COLLECTIBLES, PLAYER_STATS_COUNT,
                                  0 },
    [PS_VANISH_CAPS_COLLECTED] = { "Vanish Caps Collected", PS_CATEGORY_COLLECTIBLES,
                                   PLAYER_STATS_COUNT, 0 },
    [PS_EXCLAMATION_BLOCKS_BROKEN] = { "Exclamation Blocks Broken", PS_CATEGORY_WORLD,
                                       PLAYER_STATS_COUNT, 0 },
    [PS_HIDDEN_SECRETS_ACTIVATED] = { "Hidden Secrets Activated", PS_CATEGORY_WORLD, PLAYER_STATS_COUNT,
                                      0 },
    [PS_TOTAL_JUMPS_PERFORMED] = { "Total Jumps Performed", PS_CATEGORY_MOVES, PLAYER_STATS_COUNT, 0 },
    [PS_DOUBLE_JUMPS] = { "Double Jumps", PS_CATEGORY_MOVES, PLAYER_STATS_COUNT, 0 },
    [PS_TRIPLE_JUMPS] = { "Triple Jumps", PS_CATEGORY_MOVES, PLAYER_STATS_COUNT, 0 },
    [PS_LONG_JUMPS] = { "Long Jumps", PS_CATEGORY_MOVES, PLAYER_STATS_COUNT, 0 },
    [PS_BACKFLIPS] = { "Backflips", PS_CATEGORY_MOVES, PLAYER_STATS_COUNT, 0 },
    [PS_SIDE_SOMERSAULTS] = { "Side Somersaults", PS_CATEGORY_MOVES, PLAYER_STATS_COUNT, 0 },
    [PS_WALL_KICKS] = { "Wall Kicks", PS_CATEGORY_MOVES, PLAYER_STATS_COUNT, 0 },
    [PS_GROUND_POUNDS] = { "Ground Pounds", PS_CATEGORY_MOVES, PLAYER_STATS_COUNT, 0 },
    [PS_DIVES] = { "Dives", PS_CATEGORY_MOVES, PLAYER_STATS_COUNT, 0 },
    [PS_DIVE_ROLLOUTS] = { "Dive Rollouts", PS_CATEGORY_MOVES, PLAYER_STATS_COUNT, 0 },
    [PS_JUMP_KICKS] = { "Jump Kicks", PS_CATEGORY_MOVES, PLAYER_STATS_COUNT, 0 },
    [PS_SLIDE_KICKS] = { "Slide Kicks", PS_CATEGORY_MOVES, PLAYER_STATS_COUNT, 0 },
    [PS_PUNCHES_THROWN] = { "Punches Thrown", PS_CATEGORY_MOVES, PLAYER_STATS_COUNT, 0 },
    [PS_GROUND_KICKS_THROWN] = { "Ground Kicks Thrown", PS_CATEGORY_MOVES, PLAYER_STATS_COUNT, 0 },
    [PS_CRAWLS_STARTED] = { "Crawls Started", PS_CATEGORY_MOVES, PLAYER_STATS_COUNT, 0 },
    [PS_LEDGE_GRABS] = { "Ledge Grabs", PS_CATEGORY_MOVES, PLAYER_STATS_COUNT, 0 },
    [PS_POLE_GRABS] = { "Pole Grabs", PS_CATEGORY_MOVES, PLAYER_STATS_COUNT, 0 },
    [PS_SWIMMING_STROKES] = { "Swimming Strokes", PS_CATEGORY_MOVES, PLAYER_STATS_COUNT, 0 },
    [PS_WATER_JUMPS] = { "Water Jumps", PS_CATEGORY_MOVES, PLAYER_STATS_COUNT, 0 },
    [PS_TOTAL_DISTANCE_TRAVELED] = { "Total Distance Traveled", PS_CATEGORY_DISTANCE,
                                     PLAYER_STATS_DISTANCE, 0 },
    [PS_DISTANCE_TRAVELED_ON_FOOT] = { "Distance Traveled on Foot", PS_CATEGORY_DISTANCE,
                                       PLAYER_STATS_DISTANCE, 0 },
    [PS_DISTANCE_TRAVELED_IN_THE_AIR] = { "Distance Traveled in the Air", PS_CATEGORY_DISTANCE,
                                          PLAYER_STATS_DISTANCE, 0 },
    [PS_DISTANCE_CRAWLED] = { "Distance Crawled", PS_CATEGORY_DISTANCE, PLAYER_STATS_DISTANCE, 0 },
    [PS_DISTANCE_SWUM] = { "Distance Swum", PS_CATEGORY_DISTANCE, PLAYER_STATS_DISTANCE, 0 },
    [PS_DISTANCE_SLID] = { "Distance Slid", PS_CATEGORY_DISTANCE, PLAYER_STATS_DISTANCE, 0 },
    [PS_DISTANCE_FLOWN] = { "Distance Flown", PS_CATEGORY_DISTANCE, PLAYER_STATS_DISTANCE, 0 },
    [PS_DISTANCE_TRAVELED_RIDING_SHELLS] = { "Distance Traveled Riding Shells", PS_CATEGORY_DISTANCE,
                                             PLAYER_STATS_DISTANCE, 0 },
    [PS_FASTEST_GROUND_SPEED] = { "Fastest Ground Speed", PS_CATEGORY_DISTANCE, PLAYER_STATS_SPEED, 1 },
    [PS_FASTEST_AIRBORNE_SPEED] = { "Fastest Airborne Speed", PS_CATEGORY_DISTANCE, PLAYER_STATS_SPEED,
                                    1 },
    [PS_LONGEST_SINGLE_LONG_JUMP] = { "Longest Single Long Jump", PS_CATEGORY_DISTANCE,
                                      PLAYER_STATS_DISTANCE, 1 },
    [PS_LONGEST_AIR_TIME_WITHOUT_WING_CAP] = { "Longest Air Time Without Wing Cap",
                                               PS_CATEGORY_DISTANCE, PLAYER_STATS_TIME, 1 },
    [PS_GREATEST_FALL_SURVIVED] = { "Greatest Fall Survived", PS_CATEGORY_DISTANCE,
                                    PLAYER_STATS_DISTANCE, 1 },
    [PS_LONGEST_WALL_KICK_CHAIN] = { "Longest Wall-Kick Chain", PS_CATEGORY_DISTANCE,
                                     PLAYER_STATS_COUNT, 1 },
    [PS_LONGEST_CONTINUOUS_SLIDE] = { "Longest Continuous Slide", PS_CATEGORY_DISTANCE,
                                      PLAYER_STATS_DISTANCE, 1 },
    [PS_LONGEST_CONTINUOUS_SHELL_RIDE] = { "Longest Continuous Shell Ride", PS_CATEGORY_DISTANCE,
                                           PLAYER_STATS_DISTANCE, 1 },
    [PS_TOTAL_ENEMIES_DEFEATED] = { "Total Enemies Defeated", PS_CATEGORY_COMBAT, PLAYER_STATS_COUNT,
                                    0 },
    [PS_ENEMIES_STOMPED] = { "Enemies Stomped", PS_CATEGORY_COMBAT, PLAYER_STATS_COUNT, 0 },
    [PS_ENEMY_BOUNCE_STREAK] = { "Enemy Bounce Streak", PS_CATEGORY_COMBAT, PLAYER_STATS_COUNT, 1 },
    [PS_SECONDS_SOAKED] = { "Seconds Soaked", PS_CATEGORY_DAMAGE, PLAYER_STATS_SECONDS, 0 },
    [PS_LONGEST_DAMAGE_FREE_STREAK] = { "Longest No-Damage Streak", PS_CATEGORY_DAMAGE,
                                        PLAYER_STATS_TIME, 1 },
    [PS_STARS_COLLECTED_WITHOUT_DAMAGE] = { "Damage-Free Stars", PS_CATEGORY_COLLECTIBLES,
                                            PLAYER_STATS_COUNT, 0 },
    [PS_MOST_ENEMIES_DEFEATED_IN_ONE_VISIT] = { "Most Defeats in One Visit", PS_CATEGORY_COMBAT,
                                                PLAYER_STATS_COUNT, 1 },
    [PS_BREAKDANCE_DEFEATS] = { "Breakdance Defeats", PS_CATEGORY_COMBAT, PLAYER_STATS_COUNT, 0 },
    [PS_THROWN_OBJECT_DEFEATS] = { "Thrown-Object Defeats", PS_CATEGORY_COMBAT, PLAYER_STATS_COUNT, 0 },
    [PS_TIME_HOLDING_OBJECTS] = { "Time Holding Objects", PS_CATEGORY_WORLD, PLAYER_STATS_TIME, 0 },
    [PS_DISTANCE_TRAVELED_WHILE_SOAKED] = { "Distance While Soaked", PS_CATEGORY_DISTANCE,
                                            PLAYER_STATS_DISTANCE, 0 },
    [PS_FARTHEST_CANNON_FLIGHT] = { "Farthest Cannon Flight", PS_CATEGORY_DISTANCE,
                                    PLAYER_STATS_DISTANCE, 1 },
    [PS_TIME_USING_WING_CAP] = { "Wing Cap Time", PS_CATEGORY_COLLECTIBLES, PLAYER_STATS_TIME, 0 },
    [PS_TIME_USING_METAL_CAP] = { "Metal Cap Time", PS_CATEGORY_COLLECTIBLES, PLAYER_STATS_TIME, 0 },
    [PS_TIME_USING_VANISH_CAP] = { "Vanish Cap Time", PS_CATEGORY_COLLECTIBLES, PLAYER_STATS_TIME, 0 },
    [PS_ENEMIES_PUNCHED] = { "Enemies Punched", PS_CATEGORY_COMBAT, PLAYER_STATS_COUNT, 0 },
    [PS_ENEMIES_KICKED] = { "Enemies Kicked", PS_CATEGORY_COMBAT, PLAYER_STATS_COUNT, 0 },
    [PS_ENEMIES_GROUND_POUNDED] = { "Enemies Ground-Pounded", PS_CATEGORY_COMBAT, PLAYER_STATS_COUNT,
                                    0 },
    [PS_ENEMIES_DEFEATED_WHILE_RIDING_SHELLS] = { "Enemies Defeated While Riding Shells",
                                                  PS_CATEGORY_COMBAT, PLAYER_STATS_COUNT, 0 },
    [PS_GOOMBAS_DEFEATED] = { "Goombas Defeated", PS_CATEGORY_COMBAT, PLAYER_STATS_COUNT, 0 },
    [PS_BOOS_DEFEATED] = { "Boos Defeated", PS_CATEGORY_COMBAT, PLAYER_STATS_COUNT, 0 },
    [PS_BULLIES_KNOCKED_INTO_LAVA] = { "Bullies Knocked into Lava", PS_CATEGORY_COMBAT,
                                       PLAYER_STATS_COUNT, 0 },
    [PS_OBJECTS_PICKED_UP] = { "Objects Picked Up", PS_CATEGORY_WORLD, PLAYER_STATS_COUNT, 0 },
    [PS_OBJECTS_THROWN] = { "Objects Thrown", PS_CATEGORY_WORLD, PLAYER_STATS_COUNT, 0 },
    [PS_BOWSER_TAIL_GRABS] = { "Bowser Tail Grabs", PS_CATEGORY_COMBAT, PLAYER_STATS_COUNT, 0 },
    [PS_BOWSER_THROWS] = { "Bowser Throws", PS_CATEGORY_COMBAT, PLAYER_STATS_COUNT, 0 },
    [PS_BOWSER_BOMB_HITS] = { "Bowser Bomb Hits", PS_CATEGORY_COMBAT, PLAYER_STATS_COUNT, 0 },
    [PS_MISSED_BOWSER_THROWS] = { "Missed Bowser Throws", PS_CATEGORY_COMBAT, PLAYER_STATS_COUNT, 0 },
    [PS_HITS_TAKEN] = { "Hits Taken", PS_CATEGORY_DAMAGE, PLAYER_STATS_COUNT, 0 },
    [PS_POWER_METER_WEDGES_LOST] = { "Power Meter Wedges Lost", PS_CATEGORY_DAMAGE, PLAYER_STATS_WEDGES,
                                     0 },
    [PS_POWER_METER_WEDGES_RESTORED] = { "Power Meter Wedges Restored", PS_CATEGORY_DAMAGE,
                                         PLAYER_STATS_WEDGES, 0 },
    [PS_WALL_BONKS] = { "Wall Bonks", PS_CATEGORY_DAMAGE, PLAYER_STATS_COUNT, 0 },
    [PS_FALL_DAMAGE_INCIDENTS] = { "Fall-Damage Incidents", PS_CATEGORY_DAMAGE, PLAYER_STATS_COUNT, 0 },
    [PS_LAVA_BURNS] = { "Lava Burns", PS_CATEGORY_DAMAGE, PLAYER_STATS_COUNT, 0 },
    [PS_FIRE_BURNS] = { "Fire Burns", PS_CATEGORY_DAMAGE, PLAYER_STATS_COUNT, 0 },
    [PS_ELECTRIC_SHOCKS] = { "Electric Shocks", PS_CATEGORY_DAMAGE, PLAYER_STATS_COUNT, 0 },
    [PS_TIMES_CRUSHED] = { "Times Crushed", PS_CATEGORY_DAMAGE, PLAYER_STATS_COUNT, 0 },
    [PS_DROWNING_DEATHS] = { "Drowning Deaths", PS_CATEGORY_DAMAGE, PLAYER_STATS_COUNT, 0 },
    [PS_FALLS_INTO_BOTTOMLESS_PITS] = { "Falls into Bottomless Pits", PS_CATEGORY_DAMAGE,
                                        PLAYER_STATS_COUNT, 0 },
    [PS_QUICKSAND_DEATHS] = { "Quicksand Deaths", PS_CATEGORY_DAMAGE, PLAYER_STATS_COUNT, 0 },
    [PS_CANNON_LAUNCHES] = { "Cannon Launches", PS_CATEGORY_WORLD, PLAYER_STATS_COUNT, 0 },
    [PS_HIDDEN_TELEPORTERS_USED] = { "Hidden Teleporters Used", PS_CATEGORY_WORLD, PLAYER_STATS_COUNT,
                                     0 },
    [PS_WARP_PIPES_ENTERED] = { "Warp Pipes Entered", PS_CATEGORY_WORLD, PLAYER_STATS_COUNT, 0 },
    [PS_KOOPA_SHELLS_MOUNTED] = { "Koopa Shells Mounted", PS_CATEGORY_WORLD, PLAYER_STATS_COUNT, 0 },
    [PS_TREES_CLIMBED] = { "Trees Climbed", PS_CATEGORY_WORLD, PLAYER_STATS_COUNT, 0 },
    [PS_SIGNS_READ] = { "Signs Read", PS_CATEGORY_WORLD, PLAYER_STATS_COUNT, 0 },
    [PS_TOAD_CONVERSATIONS] = { "Toad Conversations", PS_CATEGORY_WORLD, PLAYER_STATS_COUNT, 0 },
    [PS_STAR_DOORS_UNLOCKED] = { "Star Doors Unlocked", PS_CATEGORY_WORLD, PLAYER_STATS_COUNT, 0 },
    [PS_MIPS_CAUGHT] = { "MIPS Caught", PS_CATEGORY_WORLD, PLAYER_STATS_COUNT, 0 },
    [PS_BABY_PENGUINS_RETURNED] = { "Baby Penguins Returned", PS_CATEGORY_WORLD, PLAYER_STATS_COUNT,
                                    0 },
    [PS_RACES_ENTERED] = { "Races Entered", PS_CATEGORY_WORLD, PLAYER_STATS_COUNT, 0 },
    [PS_RACES_WON] = { "Races Won", PS_CATEGORY_WORLD, PLAYER_STATS_COUNT, 0 },
    [PS_TIMES_MARIO_S_CAP_LOST] = { "Times Mario's Cap Lost", PS_CATEGORY_WORLD, PLAYER_STATS_COUNT,
                                    0 },
    [PS_TIMES_MARIO_S_CAP_RECOVERED] = { "Times Mario's Cap Recovered", PS_CATEGORY_WORLD,
                                         PLAYER_STATS_COUNT, 0 },
    [PS_TIMES_MARIO_FELL_ASLEEP] = { "Times Mario Fell Asleep", PS_CATEGORY_WORLD, PLAYER_STATS_COUNT,
                                     0 },
    [PS_TIME_SPENT_IN_DIALOGUE] = { "Time Spent in Dialogue", PS_CATEGORY_GENERAL, PLAYER_STATS_TIME,
                                    0 },
    [PS_TOTAL_DISTANCE_TRAVELED_3D] = { "Total Distance Traveled (3D)", PS_CATEGORY_DISTANCE,
                                        PLAYER_STATS_DISTANCE, 0 },
    [PS_DISTANCE_TRAVELED_ON_FOOT_3D] = { "Distance Traveled on Foot (3D)", PS_CATEGORY_DISTANCE,
                                          PLAYER_STATS_DISTANCE, 0 },
    [PS_DISTANCE_TRAVELED_IN_THE_AIR_3D] = { "Distance Traveled in the Air (3D)", PS_CATEGORY_DISTANCE,
                                             PLAYER_STATS_DISTANCE, 0 },
    [PS_DISTANCE_CRAWLED_3D] = { "Distance Crawled (3D)", PS_CATEGORY_DISTANCE, PLAYER_STATS_DISTANCE,
                                 0 },
    [PS_DISTANCE_SWUM_3D] = { "Distance Swum (3D)", PS_CATEGORY_DISTANCE, PLAYER_STATS_DISTANCE, 0 },
    [PS_DISTANCE_SLID_3D] = { "Distance Slid (3D)", PS_CATEGORY_DISTANCE, PLAYER_STATS_DISTANCE, 0 },
    [PS_DISTANCE_FLOWN_3D] = { "Distance Flown (3D)", PS_CATEGORY_DISTANCE, PLAYER_STATS_DISTANCE, 0 },
    [PS_DISTANCE_TRAVELED_RIDING_SHELLS_3D] = { "Distance Traveled Riding Shells (3D)",
                                                PS_CATEGORY_DISTANCE, PLAYER_STATS_DISTANCE, 0 },
    [PS_GROUND_TWIRLS] = { "Ground Twirls", PS_CATEGORY_MOVES, PLAYER_STATS_COUNT, 0 },
    [PS_AIR_TWIRLS] = { "Air Twirls", PS_CATEGORY_MOVES, PLAYER_STATS_COUNT, 0 },
    [PS_TWIRL_WALL_REBOUNDS] = { "Twirl Wall Rebounds", PS_CATEGORY_MOVES, PLAYER_STATS_COUNT, 0 },
    [PS_TWIRL_ENEMY_HITS] = { "Twirl Enemy Hits", PS_CATEGORY_COMBAT, PLAYER_STATS_COUNT, 0 },
    [PS_BREAKDANCE_SPINS] = { "Breakdance Spins", PS_CATEGORY_MOVES, PLAYER_STATS_COUNT, 0 },
    [PS_DABS_PERFORMED] = { "Dabs Performed", PS_CATEGORY_MOVES, PLAYER_STATS_COUNT, 0 },
    [PS_TIME_SPENT_DABBING] = { "Time Spent Dabbing", PS_CATEGORY_MOVES, PLAYER_STATS_TIME, 0 },
    [PS_DAB_INDUCED_SLEEPS] = { "Dab-Induced Sleeps", PS_CATEGORY_MOVES, PLAYER_STATS_COUNT, 0 },
    [PS_GROUND_TWIRL_WALL_REBOUNDS] = { "Ground Twirl Wall Rebounds", PS_CATEGORY_MOVES,
                                        PLAYER_STATS_COUNT, 0 },
    [PS_AIR_TWIRL_WALL_REBOUNDS] = { "Air Twirl Wall Rebounds", PS_CATEGORY_MOVES, PLAYER_STATS_COUNT,
                                     0 },
    [PS_LONGEST_TWIRL_WALL_REBOUND_CHAIN] = { "Longest Twirl Wall Rebound Chain", PS_CATEGORY_MOVES,
                                              PLAYER_STATS_COUNT, 1 },
    [PS_TIME_HOLDING_TWIRL_POSE] = { "Time Holding Twirl Pose", PS_CATEGORY_MOVES, PLAYER_STATS_TIME,
                                     0 },
    [PS_DISTANCE_WATER_WALKED] = { "Distance Water-Walked", PS_CATEGORY_DISTANCE, PLAYER_STATS_DISTANCE,
                                   0 },
    [PS_DISTANCE_WATER_WALKED_3D] = { "Distance Water-Walked (3D)", PS_CATEGORY_DISTANCE,
                                      PLAYER_STATS_DISTANCE, 0 },
    [PS_TIME_WATER_WALKING] = { "Time Water-Walking", PS_CATEGORY_DISTANCE, PLAYER_STATS_TIME, 0 },
    [PS_LONGEST_CONTINUOUS_WATER_WALK] = { "Longest Continuous Water Walk", PS_CATEGORY_DISTANCE,
                                           PLAYER_STATS_DISTANCE, 1 },
    [PS_SLIDE_KICK_LIQUID_BOUNCES] = { "Slide-Kick Liquid Bounces", PS_CATEGORY_MOVES,
                                       PLAYER_STATS_COUNT, 0 },
    [PS_DOWNWARD_SWIMMING_BURSTS] = { "Downward Swimming Bursts", PS_CATEGORY_MOVES, PLAYER_STATS_COUNT,
                                      0 },
    [PS_ENEMIES_STUNNED_BY_TWIRLS] = { "Enemies Stunned by Twirls", PS_CATEGORY_COMBAT,
                                       PLAYER_STATS_COUNT, 0 },
    [PS_ENEMIES_DEFEATED_BY_PASSIVE_METAL_CONTACT] = { "Enemies Defeated by Metal Contact",
                                                       PS_CATEGORY_COMBAT, PLAYER_STATS_COUNT, 0 },
    [PS_POWER_CAPS_SHED_BY_BREAKDANCE] = { "Power Caps Shed by Breakdance", PS_CATEGORY_MOVES,
                                           PLAYER_STATS_COUNT, 0 },
    [PS_CAP_POWERS_LOST_TO_DAMAGE] = { "Cap Powers Lost to Damage", PS_CATEGORY_DAMAGE,
                                       PLAYER_STATS_COUNT, 0 },
    [PS_BOWSER_TIMED_ESCAPES] = { "Bowser Timed Escapes", PS_CATEGORY_COMBAT, PLAYER_STATS_COUNT, 0 },
    [PS_HE_THICC_PLAY_TIME] = { "Play Time as HE THICC", PS_CATEGORY_HE_THICC, PLAYER_STATS_TIME, 0 },
    [PS_HE_THICC_CLAPS] = { "Butt Claps", PS_CATEGORY_HE_THICC, PLAYER_STATS_COUNT, 0 },
    [PS_HE_THICC_WAVEDASHES] = { "Wavedashes", PS_CATEGORY_HE_THICC, PLAYER_STATS_COUNT, 0 },
    [PS_HE_THICC_BELLY_SLAMS] = { "Belly Slams", PS_CATEGORY_HE_THICC, PLAYER_STATS_COUNT, 0 },
    [PS_HE_THICC_BELLY_BOUNCES] = { "Timed Belly Bounces", PS_CATEGORY_HE_THICC, PLAYER_STATS_COUNT,
                                    0 },
    [PS_HE_THICC_POUND_BOUNCES] = { "Ground-Pound Bounces", PS_CATEGORY_HE_THICC, PLAYER_STATS_COUNT,
                                    0 },
    [PS_HE_THICC_WALL_KICKS] = { "Butt Wall Kicks", PS_CATEGORY_HE_THICC, PLAYER_STATS_COUNT, 0 },
    [PS_HE_THICC_SLIDE_KICK_BOUNCES] = { "Slide-Kick Bounces", PS_CATEGORY_HE_THICC, PLAYER_STATS_COUNT,
                                         0 },
    [PS_HE_THICC_EXPLOSIVE_STRIKES] = { "Explosive Strikes", PS_CATEGORY_HE_THICC, PLAYER_STATS_COUNT,
                                        0 },
    [PS_HE_THICC_SWIMMING_STROKES] = { "Swimming Strokes", PS_CATEGORY_HE_THICC, PLAYER_STATS_COUNT,
                                       0 },
    [PS_DEFEATED_BY_BOWSER_1] = { "Defeated by Bowser 1", PS_CATEGORY_COMBAT, PLAYER_STATS_COUNT, 0 },
    [PS_DEFEATED_BY_BOWSER_2] = { "Defeated by Bowser 2", PS_CATEGORY_COMBAT, PLAYER_STATS_COUNT, 0 },
    [PS_DEFEATED_BY_BOWSER_3] = { "Defeated by Bowser 3", PS_CATEGORY_COMBAT, PLAYER_STATS_COUNT, 0 },

};

static s32 valid_file(s32 file) {
    return file >= 0 && file < PLAYER_STATS_SAVE_FILES;
}

static u32 record_offset(s32 file, s32 copy) {
    return PLAYER_STATS_SRAM_OFFSET + (file * 2 + copy) * PLAYER_STATS_SAVE_STRIDE;
}

static u32 legacy_record_offset(s32 file, s32 copy) {
    return PLAYER_STATS_LEGACY_SRAM_OFFSET + (file * 2 + copy) * PLAYER_STATS_LEGACY_SAVE_STRIDE;
}

static u32 record_checksum(const void *record, u32 size) {
    const u8 *bytes = record;
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

static s32 valid_record(const struct PlayerStatsSave *record, s32 file) {
    return record->magic == PLAYER_STATS_MAGIC && record->version == PLAYER_STATS_SAVE_VERSION
           && record->fileIndex == (u32) file && record->payloadSize == sizeof(struct PlayerStatsData)
           && record->checksum == record_checksum(record, offsetof(struct PlayerStatsSave, checksum));
}

static s32 valid_v1_record(const struct PlayerStatsSaveV1 *record, s32 file) {
    return record->magic == PLAYER_STATS_MAGIC && record->version == 1
           && record->fileIndex == (u32) file && record->payloadSize == sizeof(struct PlayerStatsDataV1)
           && record->checksum == record_checksum(record, offsetof(struct PlayerStatsSaveV1, checksum));
}

static s32 valid_v2_record(const struct PlayerStatsSaveV2 *record, s32 file) {
    return record->magic == PLAYER_STATS_MAGIC && record->version == 2
           && record->fileIndex == (u32) file && record->payloadSize == sizeof(struct PlayerStatsDataV2)
           && record->checksum == record_checksum(record, offsetof(struct PlayerStatsSaveV2, checksum));
}

static s32 valid_v3_record(const struct PlayerStatsSaveV3 *record, s32 file) {
    return record->magic == PLAYER_STATS_MAGIC && record->version == 3
           && record->fileIndex == (u32) file && record->payloadSize == sizeof(struct PlayerStatsDataV3)
           && record->checksum == record_checksum(record, offsetof(struct PlayerStatsSaveV3, checksum));
}

static s32 valid_v4_record(const struct PlayerStatsSaveV4 *record, s32 file) {
    return record->magic == PLAYER_STATS_MAGIC && record->version == 4
           && record->fileIndex == (u32) file && record->payloadSize == sizeof(struct PlayerStatsDataV4)
           && record->checksum == record_checksum(record, offsetof(struct PlayerStatsSaveV4, checksum));
}

static s32 valid_v5_record(const struct PlayerStatsSaveV5 *record, s32 file) {
    return record->magic == PLAYER_STATS_MAGIC && record->version == 5
           && record->fileIndex == (u32) file && record->payloadSize == sizeof(struct PlayerStatsDataV5)
           && record->checksum == record_checksum(record, offsetof(struct PlayerStatsSaveV5, checksum));
}

static s32 valid_v6_record(const struct PlayerStatsSaveV6 *record, s32 file) {
    return record->magic == PLAYER_STATS_MAGIC && record->version == 6
           && record->fileIndex == (u32) file && record->payloadSize == sizeof(struct PlayerStatsDataV6)
           && record->checksum == record_checksum(record, offsetof(struct PlayerStatsSaveV6, checksum));
}

static s32 load_record(s32 file, u32 offset, void *data, u32 size) {
    if (sRead == NULL || sRead(offset, data, size) != 0) {
        sLoadReadFailed[file] = 1;
        return 0;
    }
    return 1;
}

void player_stats_init(PlayerStatsRead read, PlayerStatsWrite write) {
    s32 file;
    s32 copy;
    sRead = read;
    sWrite = write;
    sActiveFile = -1;
    memset(gPlayerStats, 0, sizeof(gPlayerStats));
    memset(sDirty, 0, sizeof(sDirty));
    memset(sGeneration, 0, sizeof(sGeneration));
    memset(sCurrentFormatCopies, 0, sizeof(sCurrentFormatCopies));
    memset(sLegacyCopiesToRetire, 0, sizeof(sLegacyCopiesToRetire));
    memset(sLoadReadFailed, 0, sizeof(sLoadReadFailed));
    memset(sCopiesToReplace, 0, sizeof(sCopiesToReplace));
    memset(sSelectedCopy, -1, sizeof(sSelectedCopy));
    for (file = 0; file < PLAYER_STATS_SAVE_FILES; file++) {
        s32 legacySelected = 0;
        u8 previousCopies = 0;
        for (copy = 0; copy < 2; copy++) {
            struct PlayerStatsSave *record = &sSaveScratch[copy].current;
            if (!load_record(file, record_offset(file, copy), record, sizeof(*record))
                || !valid_record(record, file)) {
                continue;
            }
            sCurrentFormatCopies[file] |= 1 << copy;
            if (sSelectedCopy[file] >= 0 && (s32) (record->generation - sGeneration[file]) <= 0) {
                continue;
            }
            gPlayerStats[file] = record->data;
            sGeneration[file] = record->generation;
            sSelectedCopy[file] = copy;
        }
        for (copy = 0; copy < 2; copy++) {
            struct PlayerStatsSaveV6 *record = &sSaveScratch[copy].v6;
            if (!load_record(file, record_offset(file, copy), record, sizeof(*record))
                || !valid_v6_record(record, file)) {
                continue;
            }
            previousCopies |= 1 << copy;
            if (sCurrentFormatCopies[file] != 0
                || (sSelectedCopy[file] >= 0 && (s32) (record->generation - sGeneration[file]) <= 0)) {
                continue;
            }
            memset(&gPlayerStats[file], 0, sizeof(gPlayerStats[file]));
            memcpy(gPlayerStats[file].values, record->data.values, sizeof(record->data.values));
            memcpy(gPlayerStats[file].levels, record->data.levels, sizeof(record->data.levels));
            memcpy(gPlayerStats[file].areas, record->data.areas, sizeof(record->data.areas));
            sGeneration[file] = record->generation;
            sSelectedCopy[file] = copy;
        }
        for (copy = 0; copy < 2; copy++) {
            struct PlayerStatsSaveV5 *record = &sSaveScratch[copy].v5;
            if (!load_record(file, record_offset(file, copy), record, sizeof(*record))
                || !valid_v5_record(record, file)) {
                continue;
            }
            previousCopies |= 1 << copy;
            if (sCurrentFormatCopies[file] != 0
                || (sSelectedCopy[file] >= 0 && (s32) (record->generation - sGeneration[file]) <= 0)) {
                continue;
            }
            memset(&gPlayerStats[file], 0, sizeof(gPlayerStats[file]));
            memcpy(gPlayerStats[file].values, record->data.values, sizeof(record->data.values));
            memcpy(gPlayerStats[file].levels, record->data.levels, sizeof(record->data.levels));
            memcpy(gPlayerStats[file].areas, record->data.areas, sizeof(record->data.areas));
            sGeneration[file] = record->generation;
            sSelectedCopy[file] = copy;
        }
        for (copy = 0; copy < 2; copy++) {
            struct PlayerStatsSaveV4 *record = &sSaveScratch[copy].v4;
            if (!load_record(file, record_offset(file, copy), record, sizeof(*record))
                || !valid_v4_record(record, file)) {
                continue;
            }
            previousCopies |= 1 << copy;
            if (sCurrentFormatCopies[file] != 0
                || (sSelectedCopy[file] >= 0 && (s32) (record->generation - sGeneration[file]) <= 0)) {
                continue;
            }
            memset(&gPlayerStats[file], 0, sizeof(gPlayerStats[file]));
            memcpy(gPlayerStats[file].values, record->data.values, sizeof(record->data.values));
            memcpy(gPlayerStats[file].levels, record->data.levels, sizeof(record->data.levels));
            memcpy(gPlayerStats[file].areas, record->data.areas, sizeof(record->data.areas));
            sGeneration[file] = record->generation;
            sSelectedCopy[file] = copy;
        }
        for (copy = 0; copy < 2; copy++) {
            struct PlayerStatsSaveV3 *record = &sSaveScratch[copy].v3;
            if (!load_record(file, record_offset(file, copy), record, sizeof(*record))
                || !valid_v3_record(record, file)) {
                continue;
            }
            previousCopies |= 1 << copy;
            if (sCurrentFormatCopies[file] != 0
                || (sSelectedCopy[file] >= 0 && (s32) (record->generation - sGeneration[file]) <= 0)) {
                continue;
            }
            memset(&gPlayerStats[file], 0, sizeof(gPlayerStats[file]));
            memcpy(gPlayerStats[file].values, record->data.values, sizeof(record->data.values));
            memcpy(gPlayerStats[file].levels, record->data.levels, sizeof(record->data.levels));
            memcpy(gPlayerStats[file].areas, record->data.areas, sizeof(record->data.areas));
            sGeneration[file] = record->generation;
            sSelectedCopy[file] = copy;
        }
        sCopiesToReplace[file] = previousCopies;
        for (copy = 0; copy < 2; copy++) {
            union PlayerStatsSaveScratch *scratch = &sSaveScratch[copy];
            s32 v1;
            if (!load_record(file, legacy_record_offset(file, copy), scratch, sizeof(scratch->v2))) {
                continue;
            }
            v1 = valid_v1_record(&scratch->v1, file);
            if (!v1 && !valid_v2_record(&scratch->v2, file)) {
                continue;
            }
            sLegacyCopiesToRetire[file] |= 1 << copy;
            // Any current copy owns this slot, even after erase or generation wrap.
            if (sCurrentFormatCopies[file] != 0 || previousCopies != 0
                || (legacySelected && (s32) (scratch->v2.generation - sGeneration[file]) <= 0)) {
                continue;
            }
            memset(&gPlayerStats[file], 0, sizeof(gPlayerStats[file]));
            if (v1) {
                memcpy(gPlayerStats[file].values, scratch->v1.data.values,
                       sizeof(scratch->v1.data.values));
                memcpy(gPlayerStats[file].levels, scratch->v1.data.levels,
                       sizeof(scratch->v1.data.levels));
                memcpy(gPlayerStats[file].areas, scratch->v1.data.areas,
                       sizeof(scratch->v1.data.areas));
            } else {
                memcpy(gPlayerStats[file].values, scratch->v2.data.values,
                       sizeof(scratch->v2.data.values));
                memcpy(gPlayerStats[file].levels, scratch->v2.data.levels,
                       sizeof(scratch->v2.data.levels));
                memcpy(gPlayerStats[file].areas, scratch->v2.data.areas,
                       sizeof(scratch->v2.data.areas));
            }
            sGeneration[file] = scratch->v2.generation;
            legacySelected = 1;
        }
        // Finish both new backups before retiring legacy data, including after reboot.
        sDirty[file] = previousCopies != 0 || legacySelected || (sLegacyCopiesToRetire[file] != 0);
    }
}

void player_stats_select_file(s32 fileIndex) {
    sActiveFile = valid_file(fileIndex) ? fileIndex : -1;
}

s32 player_stats_active_file(void) {
    return sActiveFile;
}

void player_stats_add(enum PlayerStatId id, u64 amount) {
    u64 *value;
    if (!valid_file(sActiveFile) || id <= PS_NONE || id >= PLAYER_STAT_COUNT || amount == 0
        || game_options_is_cheating()) {
        return;
    }
    value = &gPlayerStats[sActiveFile].values[id];
    if (*value != PLAYER_STATS_MAX_VALUE) {
        *value = amount > PLAYER_STATS_MAX_VALUE - *value ? PLAYER_STATS_MAX_VALUE : *value + amount;
        sDirty[sActiveFile] = 1;
    }
}

void player_stats_record(enum PlayerStatId id, u64 value, u16 level, u8 area) {
    if (!valid_file(sActiveFile) || id <= PS_NONE || id >= PLAYER_STAT_COUNT
        || value <= gPlayerStats[sActiveFile].values[id] || game_options_is_cheating()) {
        return;
    }
    gPlayerStats[sActiveFile].values[id] = value;
    gPlayerStats[sActiveFile].levels[id] = level;
    gPlayerStats[sActiveFile].areas[id] = area;
    sDirty[sActiveFile] = 1;
}

s32 player_stats_checkpoint(s32 fileIndex) {
    struct PlayerStatsSave *record = &sSaveScratch[0].current;
    struct PlayerStatsSave *verify = &sSaveScratch[1].current;
    s32 copy;
    if (!valid_file(fileIndex)) {
        return 0;
    }
    if (!sDirty[fileIndex]) {
        return 1;
    }
    // Unread totals stay protected until a successful reload or explicit replacement.
    if (sWrite == NULL || sRead == NULL || sLoadReadFailed[fileIndex]) {
        return 0;
    }
    do {
        copy = sSelectedCopy[fileIndex] == 0 ? 1 : 0;
        memset(record, 0, sizeof(*record));
        record->magic = PLAYER_STATS_MAGIC;
        record->version = PLAYER_STATS_SAVE_VERSION;
        record->generation = sGeneration[fileIndex] + 1;
        record->payloadSize = sizeof(struct PlayerStatsData);
        record->fileIndex = fileIndex;
        record->data = gPlayerStats[fileIndex];
        record->checksum = record_checksum(record, offsetof(struct PlayerStatsSave, checksum));
        if (sWrite(record_offset(fileIndex, copy), record, sizeof(*record)) != 0
            || sRead(record_offset(fileIndex, copy), verify, sizeof(*verify)) != 0
            || memcmp(record, verify, sizeof(*record)) != 0) {
            return 0;
        }
        sGeneration[fileIndex] = record->generation;
        sSelectedCopy[fileIndex] = copy;
        sCurrentFormatCopies[fileIndex] |= 1 << copy;
        sCopiesToReplace[fileIndex] &= ~(1 << copy);
    } while (sCopiesToReplace[fileIndex] != 0);
    if (sCurrentFormatCopies[fileIndex] == PLAYER_STATS_BOTH_COPIES) {
        s32 legacy;
        // Keep old backups until both larger records have passed readback verification.
        // Retiring them also prevents erased/copied histories returning on CRC failure.
        memset(record, 0, 16);
        for (legacy = 0; legacy < 2; legacy++) {
            if (!(sLegacyCopiesToRetire[fileIndex] & (1 << legacy))) {
                continue;
            }
            if (sWrite(legacy_record_offset(fileIndex, legacy), record, 16) != 0
                || sRead(legacy_record_offset(fileIndex, legacy), verify, 16) != 0
                || memcmp(record, verify, 16) != 0) {
                return 0;
            }
            sLegacyCopiesToRetire[fileIndex] &= ~(1 << legacy);
        }
    }
    sDirty[fileIndex] = sLegacyCopiesToRetire[fileIndex] != 0;
    return 1;
}

s32 player_stats_retire_legacy(s32 fileIndex) {
    if (!valid_file(fileIndex) || sLoadReadFailed[fileIndex]) {
        return 0;
    }
    if (sCurrentFormatCopies[fileIndex] != PLAYER_STATS_BOTH_COPIES || sLegacyCopiesToRetire[fileIndex]
        || sCopiesToReplace[fileIndex]) {
        sDirty[fileIndex] = 1;
        sCopiesToReplace[fileIndex] |= PLAYER_STATS_BOTH_COPIES & ~sCurrentFormatCopies[fileIndex];
        if (!player_stats_checkpoint(fileIndex)) {
            return 0;
        }
    }
    return sCurrentFormatCopies[fileIndex] == PLAYER_STATS_BOTH_COPIES
           && !sLegacyCopiesToRetire[fileIndex] && !sCopiesToReplace[fileIndex];
}

void player_stats_erase(s32 fileIndex) {
    if (valid_file(fileIndex)) {
        memset(&gPlayerStats[fileIndex], 0, sizeof(gPlayerStats[fileIndex]));
        sDirty[fileIndex] = 1;
        sCopiesToReplace[fileIndex] = PLAYER_STATS_BOTH_COPIES;
        sLoadReadFailed[fileIndex] = 0;
        player_stats_checkpoint(fileIndex);
    }
}

s32 player_stats_can_copy(s32 source) {
    return valid_file(source) && sRead != NULL && !sLoadReadFailed[source];
}

void player_stats_copy(s32 source, s32 destination) {
    if (player_stats_can_copy(source) && valid_file(destination) && source != destination) {
        gPlayerStats[destination] = gPlayerStats[source];
        sDirty[destination] = 1;
        sCopiesToReplace[destination] = PLAYER_STATS_BOTH_COPIES;
        sLoadReadFailed[destination] = 0;
        player_stats_checkpoint(destination);
    }
}

s32 player_stats_is_dirty(s32 fileIndex) {
    return valid_file(fileIndex) && sDirty[fileIndex];
}
