#include "player_stats.h"
#include "game_options_runtime.h"
#include "mario_character.h"

#include <string.h>
#include "sm64.h"
#include "area.h"
#include "behavior_data.h"
#include "engine/math_util.h"
#include "game_init.h"
#include "ingame_menu.h"
#include "interaction.h"
#include "dialog_ids.h"
#include "level_table.h"
#include "level_update.h"
#include "mario.h"
#include "mario_punch.h"
#include "mario_step.h"
#include "object_constants.h"
#include "object_helpers.h"
#include "object_list_processor.h"
#include "save_file.h"

#define PLAYER_STATS_CHECKPOINT_FRAMES (60 * PLAYER_STATS_FRAMES_PER_SECOND)
#define PLAYER_STATS_KILL_CREDIT_FRAMES (20 * PLAYER_STATS_FRAMES_PER_SECOND)
#define PLAYER_STATS_OBJECT_DEFEATED 1
#define PLAYER_STATS_STAR_RED_COINS 2
#define PLAYER_STATS_STAR_SECRETS 4
#define PLAYER_STATS_BREAKDANCE_CREDIT 8
#define PLAYER_STATS_THROWN_CREDIT 16
#define PLAYER_STATS_METHOD_CREDIT (PLAYER_STATS_BREAKDANCE_CREDIT | PLAYER_STATS_THROWN_CREDIT)

// Per-pool-slot provenance; reset on allocation or enemy revival.
struct PlayerStatsObject {
    u32 attackFrame;
    s8 creditFile;
    u8 flags;
    u8 defeatStat;
};

static struct PlayerStatsObject sObjects[OBJECT_POOL_CAPACITY];
// Advances only while counting is allowed; shared by hit deduplication and attack credit.
static u32 sGameplayFrames;
// Periodic-save cadence is per selected file; explicit saves do not restart it.
static u32 sCheckpointFrames;
static u32 sDamageFrame;
static s32 sDamageRegistered;
static s32 sVisitLevel;
static s32 sVisitHadStar;
// An exit/death keeps the visit closed until player_stats_level_entered() runs.
static s32 sAwaitingLevelEntry;
static u64 sVisitEnemies;
static u64 sDamageFreeFrames;
static u8 sVisitDamaged;
static u8 sHealthLostThisUpdate;
static u32 sBonkFrame;
static s32 sBonkRegistered;
static s32 sRecordingSuspended;
static struct {
    Vec3f previous;
    Vec3f longJumpStart;
    Vec3f cannonStart;
    f32 peakY;
    u64 slideDistance;
    u64 shellDistance;
    u64 waterWalkDistance;
    u64 enemyBounces;
    u32 twirlRebounds;
    u32 airFrames;
    u32 wallKicks;
    u8 positionValid;
    u8 airborne;
    u8 longJump;
    u8 wingAssisted;
    u8 sliding;
    u8 riding;
    u8 waterWalking;
    u8 cannonFlight;
} sMovement;

// Player save sessions include cutscenes and dialogue, but exclude demos and credits.
static s32 is_gameplay_session(void) {
    return gCurrSaveFileNum >= 1 && gCurrSaveFileNum <= PLAYER_STATS_SAVE_FILES
           && gCurrDemoInput == NULL && gCurrCreditsEntry == NULL;
}

static s32 can_record_gameplay(void) {
    return is_gameplay_session() && !game_options_is_cheating();
}

void player_stats_he_thicc_event(enum PlayerStatId id) {
    if (id >= PS_HE_THICC_CLAPS && id <= PS_HE_THICC_SWIMMING_STROKES && can_record_gameplay()
        && player_stats_active_file() == gCurrSaveFileNum - 1
        && mario_character_palette() == MARIO_PALETTE_HE_THICC) {
        player_stats_add(id, 1);
    }
}

static s32 castle_level(s32 level) {
    return level == LEVEL_CASTLE || level == LEVEL_CASTLE_GROUNDS || level == LEVEL_CASTLE_COURTYARD;
}

static s32 course_level(s32 level) {
    return level >= LEVEL_MIN && level <= LEVEL_MAX && !castle_level(level) && level != LEVEL_ENDING
           && gLevelToCourseNumTable[level - 1] != COURSE_CAKE_END;
}

static void record(enum PlayerStatId id, u64 value) {
    player_stats_record(id, value, gCurrLevelNum, gCurrAreaIndex);
}

static u64 fixed_distance(f32 value) {
    if (!(value > 0.0f)) {
        return 0;
    }
    /* Gameplay deltas are small; this also prevents an invalid float conversion. */
    if (value >= 16777216.0f) {
        return (u64) 16777216 * PLAYER_STATS_FIXED_SCALE;
    }
    return (u64) (value * PLAYER_STATS_FIXED_SCALE + 0.5f);
}

static struct PlayerStatsObject *object_state(struct Object *obj) {
    uintptr_t address = (uintptr_t) obj;
    uintptr_t first = (uintptr_t) gObjectPool;
    if (address < first || address >= first + sizeof(struct Object) * OBJECT_POOL_CAPACITY
        || (address - first) % sizeof(struct Object) != 0) {
        return NULL;
    }
    return &sObjects[(address - first) / sizeof(struct Object)];
}

void player_stats_object_reset(struct Object *obj) {
    struct PlayerStatsObject *state = object_state(obj);
    if (state != NULL) {
        memset(state, 0, sizeof(*state));
        state->creditFile = -1;
    }
}

void player_stats_reset_movement(void) {
    // A warp/death also terminates the current continuous movement sequence.
    if (sMovement.sliding) {
        record(PS_LONGEST_CONTINUOUS_SLIDE, sMovement.slideDistance);
    }
    if (sMovement.riding) {
        record(PS_LONGEST_CONTINUOUS_SHELL_RIDE, sMovement.shellDistance);
    }
    if (sMovement.waterWalking) {
        record(PS_LONGEST_CONTINUOUS_WATER_WALK, sMovement.waterWalkDistance);
    }
    memset(&sMovement, 0, sizeof(sMovement));
}

static void finish_cannon_flight(struct MarioState *m) {
    if (sMovement.cannonFlight && can_record_gameplay() && m != NULL && m->health >= 0x100) {
        f32 dx = m->pos[0] - sMovement.cannonStart[0];
        f32 dz = m->pos[2] - sMovement.cannonStart[2];
        record(PS_FARTHEST_CANNON_FLIGHT, fixed_distance(sqrtf(dx * dx + dz * dz)));
    }
    sMovement.cannonFlight = FALSE;
}

void player_stats_ground_contact(void) {
    sMovement.enemyBounces = 0;
    finish_cannon_flight(gMarioState);
}

static void finish_visit(void) {
    if (sVisitLevel != 0) {
        if (!sVisitHadStar) {
            player_stats_add(PS_COURSE_EXITS_WITHOUT_COLLECTING_A_STAR, 1);
        }
        sVisitLevel = 0;
        sVisitHadStar = FALSE;
    }
    sVisitEnemies = 0;
    sVisitDamaged = FALSE;
}

static void record_bowser_defeat(void) {
    if (!can_record_gameplay() || sAwaitingLevelEntry) {
        return;
    }
    switch (gCurrLevelNum) {
        case LEVEL_BOWSER_1:
            player_stats_add(PS_DEFEATED_BY_BOWSER_1, 1);
            break;
        case LEVEL_BOWSER_2:
            player_stats_add(PS_DEFEATED_BY_BOWSER_2, 1);
            break;
        case LEVEL_BOWSER_3:
            player_stats_add(PS_DEFEATED_BY_BOWSER_3, 1);
            break;
    }
}

void player_stats_warp(s16 destination, s32 exitCourse) {
    if (exitCourse) {
        record_bowser_defeat();
    }
    if (exitCourse || (sVisitLevel != 0 && destination != sVisitLevel)) {
        finish_visit();
        sAwaitingLevelEntry = TRUE;
    }
    player_stats_reset_movement();
    player_stats_checkpoint(player_stats_active_file());
}

void player_stats_death(s32 gameOver, s32 pit) {
    if (!is_gameplay_session()) {
        return;
    }
    record_bowser_defeat();
    player_stats_add(PS_LIVES_LOST, 1);
    sDamageFreeFrames = 0;
    if (gameOver) {
        player_stats_add(PS_GAME_OVERS, 1);
    }
    if (pit) {
        player_stats_add(PS_FALLS_INTO_BOTTOMLESS_PITS, 1);
    }
    finish_visit();
    sAwaitingLevelEntry = TRUE;
    player_stats_reset_movement();
    player_stats_checkpoint(player_stats_active_file());
}

void player_stats_game_update(void) {
    s32 file = is_gameplay_session() ? gCurrSaveFileNum - 1 : -1;
    if (file != player_stats_active_file()) {
        finish_visit();
        player_stats_reset_movement();
        player_stats_checkpoint(player_stats_active_file());
        sCheckpointFrames = 0;
        sDamageFreeFrames = 0;
        sHealthLostThisUpdate = FALSE;
        sRecordingSuspended = FALSE;
        player_stats_select_file(file);
        if (file < 0) {
            // Discard credit and sequences that crossed an ineligible interval.
            for (s32 i = 0; i < OBJECT_POOL_CAPACITY; i++) {
                sObjects[i].creditFile = -1;
            }
            sDamageRegistered = FALSE;
            sBonkRegistered = FALSE;
        }
    }
    if (file < 0 || gMarioState == NULL || gMarioState->action == ACT_UNINITIALIZED) {
        return;
    }
    // Keep visit identity even when recording is suspended, including actual warps.
    if (course_level(gCurrLevelNum) && !sAwaitingLevelEntry && sVisitLevel != gCurrLevelNum) {
        finish_visit();
        sVisitLevel = gCurrLevelNum;
        sVisitHadStar = FALSE;
        player_stats_add(PS_COURSE_ENTRIES, 1);
    }
    if (game_options_is_cheating()) {
        if (!sRecordingSuspended) {
            player_stats_reset_movement();
            for (s32 i = 0; i < OBJECT_POOL_CAPACITY; i++) {
                sObjects[i].creditFile = -1;
            }
            sDamageRegistered = FALSE;
            sBonkRegistered = FALSE;
            sDamageFreeFrames = 0;
            sHealthLostThisUpdate = FALSE;
            sVisitEnemies = 0;
            sRecordingSuspended = TRUE;
        }
        // Keep the visit, but exclude damage-free stars across suspended recording.
        sVisitDamaged = TRUE;
        return;
    }
    sRecordingSuspended = FALSE;
    sGameplayFrames++;
    player_stats_add(PS_TOTAL_PLAY_TIME, 1);
    if (mario_character_palette() == MARIO_PALETTE_HE_THICC) {
        player_stats_add(PS_HE_THICC_PLAY_TIME, 1);
    }
    if (castle_level(gCurrLevelNum)) {
        player_stats_add(PS_TIME_SPENT_IN_PEACH_S_CASTLE, 1);
    } else if (course_level(gCurrLevelNum)) {
        player_stats_add(PS_TIME_SPENT_IN_COURSES, 1);
    }
    if (get_dialog_id() != DIALOG_NONE) {
        player_stats_add(PS_TIME_SPENT_IN_DIALOGUE, 1);
    }
    if (++sCheckpointFrames >= PLAYER_STATS_CHECKPOINT_FRAMES) {
        player_stats_checkpoint(file);
        sCheckpointFrames = 0;
    }
}

void player_stats_level_entered(void) {
    player_stats_reset_movement();
    if (sVisitLevel != gCurrLevelNum) {
        sVisitEnemies = 0;
        sVisitDamaged = FALSE;
    }
    sAwaitingLevelEntry = FALSE;
}

void player_stats_wall_bonk(struct MarioState *m) {
    if (m->wall != NULL && (!sBonkRegistered || sBonkFrame != sGameplayFrames)) {
        player_stats_add(PS_WALL_BONKS, 1);
        sBonkRegistered = TRUE;
        sBonkFrame = sGameplayFrames;
    }
}

void player_stats_damage(void) {
    if (!sDamageRegistered || sDamageFrame != sGameplayFrames) {
        player_stats_add(PS_HITS_TAKEN, 1);
        sDamageRegistered = TRUE;
        sDamageFrame = sGameplayFrames;
    }
}

void player_stats_health(s32 before, s32 after) {
    before = MAX(0xFF, MIN(0x880, before));
    after = MAX(0xFF, MIN(0x880, after));
    if (after < before) {
        player_stats_add(PS_POWER_METER_WEDGES_LOST, before - after);
        if (can_record_gameplay() && player_stats_active_file() == gCurrSaveFileNum - 1) {
            sDamageFreeFrames = 0;
            sHealthLostThisUpdate = TRUE;
            sVisitDamaged = TRUE;
        }
    } else if (after > before) {
        player_stats_add(PS_POWER_METER_WEDGES_RESTORED, after - before);
    }
}

void player_stats_health_non_restoring(s32 before, s32 after) {
    if (after < before) {
        player_stats_health(before, after);
    }
}

static void start_airborne(struct MarioState *m) {
    sMovement.airborne = TRUE;
    // One sample for this update; subsequent Mario updates increment at begin.
    sMovement.airFrames = 1;
    sMovement.peakY = m->pos[1];
    sMovement.wallKicks = 0;
    sMovement.wingAssisted = (m->flags & MARIO_WING_CAP) != 0 || m->action == ACT_FLYING;
}

void player_stats_fall_survived(struct MarioState *m) {
    if (sMovement.airborne && m->health >= 0x100 && mario_health_after_pending_damage(m) >= 0x100) {
        record(PS_GREATEST_FALL_SURVIVED, fixed_distance(sMovement.peakY - m->pos[1]));
        sMovement.peakY = m->pos[1];
    }
}

static void finish_airborne(struct MarioState *m) {
    f32 dx;
    f32 dz;
    player_stats_fall_survived(m);
    if (!sMovement.wingAssisted && m->health >= 0x100) {
        record(PS_LONGEST_AIR_TIME_WITHOUT_WING_CAP, sMovement.airFrames);
    }
    if (sMovement.longJump && m->health >= 0x100) {
        dx = m->pos[0] - sMovement.longJumpStart[0];
        dz = m->pos[2] - sMovement.longJumpStart[2];
        record(PS_LONGEST_SINGLE_LONG_JUMP, fixed_distance(sqrtf(dx * dx + dz * dz)));
    }
    record(PS_LONGEST_WALL_KICK_CHAIN, sMovement.wallKicks);
    sMovement.airborne = FALSE;
    sMovement.longJump = FALSE;
    finish_cannon_flight(m);
}

// Relocation/debug actions interrupt movement sampling; playtime advances independently.
static s32 action_excludes_movement(u32 action) {
    switch (action) {
        case ACT_DISAPPEARED:
        case ACT_TELEPORT_FADE_OUT:
        case ACT_TELEPORT_FADE_IN:
        case ACT_EMERGE_FROM_PIPE:
        case ACT_IN_CANNON:
        case ACT_SPAWN_SPIN_AIRBORNE:
        case ACT_SPAWN_NO_SPIN_AIRBORNE:
        case ACT_EXIT_AIRBORNE:
        case ACT_FALLING_EXIT_AIRBORNE:
        case ACT_SPECIAL_EXIT_AIRBORNE:
        case ACT_DEATH_EXIT:
        case ACT_FALLING_DEATH_EXIT:
        case ACT_UNUSED_DEATH_EXIT:
        case ACT_SPECIAL_DEATH_EXIT:
        case ACT_DEBUG_FREE_MOVE:
            return TRUE;
    }
    return FALSE;
}

void player_stats_action(struct MarioState *m, u32 action, u32 actionArg) {
    enum PlayerStatId actionStat = PS_NONE;
    s32 countsAsJump = FALSE;
    s32 wasAirborne = (m->action & ACT_FLAG_AIR) != 0;
    if (!can_record_gameplay() || player_stats_active_file() < 0) {
        return;
    }
    if (action_excludes_movement(action)) {
        player_stats_reset_movement();
        if (action == ACT_TELEPORT_FADE_OUT) {
            player_stats_add(PS_HIDDEN_TELEPORTERS_USED, 1);
        }
        return;
    }
    if ((action & ACT_GROUP_MASK) == ACT_GROUP_CUTSCENE) {
        sMovement.cannonFlight = FALSE;
    }
    if (sMovement.airborne && wasAirborne && !(action & ACT_FLAG_AIR)) {
        if (action != ACT_READING_NPC_DIALOG && action != ACT_READING_SIGN
            && action != ACT_WAITING_FOR_DIALOG && (action & ACT_GROUP_MASK) != ACT_GROUP_CUTSCENE) {
            finish_airborne(m);
        } else {
            sMovement.airborne = FALSE;
            sMovement.longJump = FALSE;
        }
    }
    if ((action & ACT_FLAG_AIR) && !sMovement.airborne) {
        start_airborne(m);
    }
    if ((m->flags & MARIO_WING_CAP) || action == ACT_FLYING) {
        sMovement.wingAssisted = TRUE;
    }
    switch (action) {
        case ACT_JUMP:
        case ACT_HOLD_JUMP:
        case ACT_STEEP_JUMP:
        case ACT_TOP_OF_POLE_JUMP:
        case ACT_RIDING_SHELL_JUMP:
            countsAsJump = TRUE;
            break;
        case ACT_DOUBLE_JUMP:
            actionStat = PS_DOUBLE_JUMPS;
            countsAsJump = TRUE;
            break;
        case ACT_TRIPLE_JUMP:
        case ACT_FLYING_TRIPLE_JUMP:
        case ACT_SPECIAL_TRIPLE_JUMP:
            actionStat = PS_TRIPLE_JUMPS;
            countsAsJump = TRUE;
            break;
        case ACT_LONG_JUMP:
            actionStat = PS_LONG_JUMPS;
            countsAsJump = TRUE;
            sMovement.longJump = TRUE;
            vec3f_copy(sMovement.longJumpStart, m->pos);
            break;
        case ACT_BACKFLIP:
            actionStat = PS_BACKFLIPS;
            countsAsJump = TRUE;
            break;
        case ACT_SIDE_FLIP:
            actionStat = PS_SIDE_SOMERSAULTS;
            countsAsJump = TRUE;
            break;
        case ACT_WALL_KICK_AIR:
            actionStat = PS_WALL_KICKS;
            countsAsJump = TRUE;
            if (sMovement.wallKicks != ~0u) {
                sMovement.wallKicks++;
            }
            record(PS_LONGEST_WALL_KICK_CHAIN, sMovement.wallKicks);
            break;
        case ACT_WATER_JUMP:
        case ACT_HOLD_WATER_JUMP:
        case ACT_METAL_WATER_JUMP:
        case ACT_HOLD_METAL_WATER_JUMP:
            actionStat = PS_WATER_JUMPS;
            countsAsJump = TRUE;
            break;
        case ACT_GROUND_POUND:
            actionStat = PS_GROUND_POUNDS;
            break;
        case ACT_DIVE:
            actionStat = PS_DIVES;
            break;
        case ACT_FORWARD_ROLLOUT:
        case ACT_BACKWARD_ROLLOUT:
            actionStat = PS_DIVE_ROLLOUTS;
            break;
        case ACT_JUMP_KICK:
            actionStat = PS_JUMP_KICKS;
            break;
        case ACT_SLIDE_KICK:
            actionStat = PS_SLIDE_KICKS;
            break;
        case ACT_START_CRAWLING:
            actionStat = PS_CRAWLS_STARTED;
            break;
        case ACT_CRAWLING:
            if (m->action != ACT_START_CRAWLING && m->action != ACT_CRAWLING) {
                actionStat = PS_CRAWLS_STARTED;
            }
            break;
        case ACT_LEDGE_GRAB:
            actionStat = PS_LEDGE_GRABS;
            break;
        case ACT_GRAB_POLE_SLOW:
        case ACT_GRAB_POLE_FAST:
            if (!(m->action & ACT_FLAG_ON_POLE)) {
                actionStat = PS_POLE_GRABS;
            }
            break;
        case ACT_BREASTSTROKE:
        case ACT_HOLD_BREASTSTROKE:
            actionStat = PS_SWIMMING_STROKES;
            break;
        case ACT_READING_SIGN:
            actionStat = PS_SIGNS_READ;
            break;
        case ACT_DAB:
            actionStat = PS_DABS_PERFORMED;
            break;
        case ACT_START_SLEEPING:
            actionStat = PS_TIMES_MARIO_FELL_ASLEEP;
            if (m->action == ACT_DAB && actionArg != 0) {
                player_stats_add(PS_DAB_INDUCED_SLEEPS, 1);
            }
            break;
        case ACT_SHOT_FROM_CANNON:
            actionStat = PS_CANNON_LAUNCHES;
            sMovement.cannonFlight = TRUE;
            vec3f_copy(sMovement.cannonStart, m->pos);
            break;
        case ACT_DROWNING:
            actionStat = PS_DROWNING_DEATHS;
            break;
        case ACT_QUICKSAND_DEATH:
            if (actionArg == ACT_ARG_QUICKSAND_DEATH_SHALLOW_WATER
                || actionArg == ACT_ARG_QUICKSAND_DEATH_HE_THICC_UNLOCK) {
                actionStat = PS_DROWNING_DEATHS;
            } else if (actionArg == ACT_ARG_QUICKSAND_DEATH_QUICKSAND) {
                actionStat = PS_QUICKSAND_DEATHS;
            }
            break;
        case ACT_SHOCKED:
        case ACT_WATER_SHOCKED:
        case ACT_ELECTROCUTION:
            if (m->action != ACT_SHOCKED && m->action != ACT_WATER_SHOCKED
                && m->action != ACT_ELECTROCUTION) {
                actionStat = PS_ELECTRIC_SHOCKS;
                player_stats_damage();
            }
            break;
        case ACT_SQUISHED:
            actionStat = PS_TIMES_CRUSHED;
            player_stats_damage();
            break;
        case ACT_LAVA_BOOST:
            actionStat = PS_LAVA_BURNS;
            player_stats_damage();
            break;
        case ACT_BURNING_GROUND:
        case ACT_BURNING_JUMP:
        case ACT_BURNING_FALL:
            if (m->action != ACT_BURNING_GROUND && m->action != ACT_BURNING_JUMP
                && m->action != ACT_BURNING_FALL) {
                actionStat = PS_FIRE_BURNS;
                player_stats_damage();
            }
            break;
    }
    if (actionStat != PS_NONE) {
        player_stats_add(actionStat, 1);
    }
    if (action == ACT_JUMP || action == ACT_HOLD_JUMP) {
        countsAsJump = !wasAirborne; // Enemy rebounds are not newly initiated player jumps.
    }
    if (countsAsJump) {
        player_stats_add(PS_TOTAL_JUMPS_PERFORMED, 1);
    }
    if ((action == ACT_PUNCHING || action == ACT_MOVE_PUNCHING)
        && actionArg == ACT_ARG_PUNCH_SEQUENCE_GROUND_KICK) {
        player_stats_punch(actionArg);
    }
    if ((action == ACT_GRAB_POLE_SLOW || action == ACT_GRAB_POLE_FAST)
        && !(m->action & ACT_FLAG_ON_POLE) && m->usedObj != NULL
        && obj_has_behavior(m->usedObj, bhvTree)) {
        player_stats_add(PS_TREES_CLIMBED, 1);
    }
}

void player_stats_mario_begin(struct MarioState *m) {
    sHealthLostThisUpdate = FALSE;
    if (!can_record_gameplay() || player_stats_active_file() < 0) {
        memset(&sMovement, 0, sizeof(sMovement));
        return;
    }
    if (!sMovement.positionValid && !action_excludes_movement(m->action)) {
        vec3f_copy(sMovement.previous, m->pos);
        sMovement.positionValid = TRUE;
    }
    if (sMovement.airborne) {
        if (sMovement.airFrames != ~0u) {
            sMovement.airFrames++;
        }
    } else if ((m->action & ACT_FLAG_AIR) && !action_excludes_movement(m->action)) {
        start_airborne(m);
    }
}

static enum PlayerStatId movement_distance_stat(struct MarioState *m) {
    if (m->riddenObj != NULL || (m->action & ACT_FLAG_RIDING_SHELL)) {
        return PS_DISTANCE_TRAVELED_RIDING_SHELLS;
    }
    if (m->action == ACT_FLYING) {
        return PS_DISTANCE_FLOWN;
    }
    if ((m->action & ACT_FLAG_SWIMMING) || (m->action & ACT_FLAG_METAL_WATER)) {
        return PS_DISTANCE_SWUM;
    }
    if (m->action == ACT_CRAWLING || m->action == ACT_START_CRAWLING) {
        return PS_DISTANCE_CRAWLED;
    }
    if (m->action & ACT_FLAG_BUTT_OR_STOMACH_SLIDE) {
        return PS_DISTANCE_SLID;
    }
    if (m->action & ACT_FLAG_AIR) {
        return PS_DISTANCE_TRAVELED_IN_THE_AIR;
    }
    return PS_DISTANCE_TRAVELED_ON_FOOT;
}

void player_stats_mario_end(struct MarioState *m) {
    enum PlayerStatId distanceStat;
    f32 dx;
    f32 dy;
    f32 dz;
    f32 horizontal;
    u64 horizontalFixed;
    u64 spatialFixed;
    s32 sliding;
    s32 riding;
    s32 waterWalking;
    if (!can_record_gameplay() || player_stats_active_file() < 0) {
        memset(&sMovement, 0, sizeof(sMovement));
        return;
    }
    if (action_excludes_movement(m->action)) {
        sMovement.positionValid = FALSE;
        return;
    }
    if (!(m->action & ACT_FLAG_AIR) && m->floor != NULL && m->pos[1] <= m->floorHeight) {
        player_stats_ground_contact();
    }
    // Per-Mario-update timers do not require a valid distance baseline.
    if (mario_is_soaked(m) && !mario_has_water_contact(m, m->waterLevel)) {
        player_stats_add(PS_SECONDS_SOAKED, 1);
    }
    if (m->health >= 0x100 && !sHealthLostThisUpdate) {
        if (sDamageFreeFrames != ~(u64) 0) {
            sDamageFreeFrames++;
        }
        record(PS_LONGEST_DAMAGE_FREE_STREAK, sDamageFreeFrames);
    }
    if (m->heldObj != NULL) {
        player_stats_add(PS_TIME_HOLDING_OBJECTS, 1);
    }
    if (m->flags & MARIO_WING_CAP) {
        player_stats_add(PS_TIME_USING_WING_CAP, 1);
    }
    if (m->flags & MARIO_METAL_CAP) {
        player_stats_add(PS_TIME_USING_METAL_CAP, 1);
    }
    if (m->flags & MARIO_VANISH_CAP) {
        player_stats_add(PS_TIME_USING_VANISH_CAP, 1);
    }
    if (m->action == ACT_DAB) {
        player_stats_add(PS_TIME_SPENT_DABBING, 1);
    }
    if (mario_ground_twirl_is_held_pose(m)) {
        player_stats_add(PS_TIME_HOLDING_TWIRL_POSE, 1);
    }
    if (sMovement.airborne) {
        sMovement.peakY = MAX(sMovement.peakY, m->pos[1]);
        if ((m->flags & MARIO_WING_CAP) || m->action == ACT_FLYING) {
            sMovement.wingAssisted = TRUE;
        }
    }
    if (!sMovement.positionValid) {
        vec3f_copy(sMovement.previous, m->pos);
        sMovement.positionValid = TRUE;
        return;
    }
    dx = m->pos[0] - sMovement.previous[0];
    dy = m->pos[1] - sMovement.previous[1];
    dz = m->pos[2] - sMovement.previous[2];
    horizontal = sqrtf(dx * dx + dz * dz);
    horizontalFixed = fixed_distance(horizontal);
    spatialFixed = dy == 0.0f ? horizontalFixed : fixed_distance(sqrtf(dx * dx + dy * dy + dz * dz));
    if (mario_is_soaked(m) && !mario_has_water_contact(m, m->waterLevel)) {
        player_stats_add(PS_DISTANCE_TRAVELED_WHILE_SOAKED, horizontalFixed);
    }
    distanceStat = movement_distance_stat(m);
    player_stats_add(PS_TOTAL_DISTANCE_TRAVELED, horizontalFixed);
    player_stats_add(PS_TOTAL_DISTANCE_TRAVELED_3D, spatialFixed);
    player_stats_add(distanceStat, horizontalFixed);
    // Horizontal and 3D category IDs have matching offsets from their total IDs.
    player_stats_add(PS_TOTAL_DISTANCE_TRAVELED_3D + distanceStat - PS_TOTAL_DISTANCE_TRAVELED,
                     spatialFixed);
    if (m->action & ACT_FLAG_AIR) {
        record(PS_FASTEST_AIRBORNE_SPEED, horizontalFixed);
    } else if ((m->action & ACT_GROUP_MASK) == ACT_GROUP_MOVING
               || (m->action & ACT_GROUP_MASK) == ACT_GROUP_STATIONARY) {
        record(PS_FASTEST_GROUND_SPEED, horizontalFixed);
    }
    sliding = distanceStat == PS_DISTANCE_SLID;
    riding = distanceStat == PS_DISTANCE_TRAVELED_RIDING_SHELLS;
    if (sliding) {
        sMovement.slideDistance = horizontalFixed > ~(u64) 0 - sMovement.slideDistance
                                      ? ~(u64) 0
                                      : sMovement.slideDistance + horizontalFixed;
    } else {
        if (sMovement.sliding) {
            record(PS_LONGEST_CONTINUOUS_SLIDE, sMovement.slideDistance);
        }
        sMovement.slideDistance = 0;
    }
    if (riding) {
        sMovement.shellDistance = horizontalFixed > ~(u64) 0 - sMovement.shellDistance
                                      ? ~(u64) 0
                                      : sMovement.shellDistance + horizontalFixed;
    } else {
        if (sMovement.riding) {
            record(PS_LONGEST_CONTINUOUS_SHELL_RIDE, sMovement.shellDistance);
        }
        sMovement.shellDistance = 0;
    }
    // The pseudo-floor may also be a candidate during flight or shell riding.
    // Count actual supported feet, rather than querying water-walk eligibility again.
    waterWalking =
        (m->flags & MARIO_WING_CAP) && m->floor == &gWaterSurfacePseudoFloor && m->riddenObj == NULL
        && !(m->action
             & (ACT_FLAG_AIR | ACT_FLAG_SWIMMING | ACT_FLAG_METAL_WATER | ACT_FLAG_RIDING_SHELL))
        && ABS(m->pos[1] - m->floorHeight) <= 0.01f;
    if (waterWalking) {
        player_stats_add(PS_TIME_WATER_WALKING, 1);
        player_stats_add(PS_DISTANCE_WATER_WALKED, horizontalFixed);
        player_stats_add(PS_DISTANCE_WATER_WALKED_3D, spatialFixed);
        sMovement.waterWalkDistance = horizontalFixed > ~(u64) 0 - sMovement.waterWalkDistance
                                          ? ~(u64) 0
                                          : sMovement.waterWalkDistance + horizontalFixed;
    } else {
        if (sMovement.waterWalking) {
            record(PS_LONGEST_CONTINUOUS_WATER_WALK, sMovement.waterWalkDistance);
        }
        sMovement.waterWalkDistance = 0;
    }
    sMovement.waterWalking = waterWalking;
    sMovement.sliding = sliding;
    sMovement.riding = riding;
    vec3f_copy(sMovement.previous, m->pos);
}

void player_stats_punch(u32 actionArg) {
    switch (actionArg) {
        case ACT_ARG_PUNCH_SEQUENCE_YAH:
        case ACT_ARG_PUNCH_SEQUENCE_WAH:
            player_stats_add(PS_PUNCHES_THROWN, 1);
            break;
        case ACT_ARG_PUNCH_SEQUENCE_GROUND_KICK:
            player_stats_add(PS_GROUND_KICKS_THROWN, 1);
            break;
        case ACT_ARG_PUNCH_SEQUENCE_BREAKDANCE:
            player_stats_add(PS_BREAKDANCE_SPINS, 1);
            break;
    }
}

void player_stats_coin(struct Object *obj, s32 courseCoins) {
    s32 value = obj->oDamageOrCoinValue;
    if (value <= 0) {
        return;
    }
    player_stats_add(PS_TOTAL_COIN_VALUE_COLLECTED, value);
    if (obj_has_model(obj, MODEL_RED_COIN) || obj_has_model(obj, MODEL_RED_COIN_NO_SHADOW)) {
        player_stats_add(PS_RED_COINS_COLLECTED, 1);
    } else if (obj_has_model(obj, MODEL_BLUE_COIN) || obj_has_model(obj, MODEL_BLUE_COIN_NO_SHADOW)) {
        player_stats_add(PS_BLUE_COINS_COLLECTED, 1);
    } else {
        player_stats_add(PS_YELLOW_COINS_COLLECTED, 1);
    }
    if (sVisitLevel != 0) {
        record(PS_HIGHEST_COIN_TOTAL_IN_A_SINGLE_COURSE_VISIT, MAX(0, courseCoins));
    }
}

void player_stats_tag_star(struct Object *obj, s32 redCoins) {
    struct PlayerStatsObject *state = object_state(obj);
    if (state != NULL) {
        state->flags |= redCoins ? PLAYER_STATS_STAR_RED_COINS : PLAYER_STATS_STAR_SECRETS;
    }
}

void player_stats_star(struct Object *obj, u32 starIndex) {
    struct PlayerStatsObject *state = object_state(obj);
    s32 course = gCurrCourseNum;
    u32 flag = 0;
    if (obj_has_model(obj, MODEL_BOWSER_KEY) || (obj->oInteractionSubtype & INT_SUBTYPE_GRAND_STAR)) {
        return;
    }
#ifdef GLOBAL_STAR_IDS
    course = starIndex / 7;
    flag = 1u << (starIndex % 7);
#else
    if (starIndex < 7) {
        flag = 1u << starIndex;
    }
#endif
    player_stats_add(PS_TOTAL_POWER_STARS_COLLECTED, 1);
    if (can_record_gameplay() && !sVisitDamaged && gMarioState != NULL && gMarioState->hurtCounter == 0
        && mario_health_after_pending_damage(gMarioState) >= gMarioState->health) {
        player_stats_add(PS_STARS_COLLECTED_WITHOUT_DAMAGE, 1);
    }
    if (flag != 0
        && (save_file_get_star_flags(gCurrSaveFileNum - 1, COURSE_NUM_TO_INDEX(course)) & flag)) {
        player_stats_add(PS_PREVIOUSLY_COLLECTED_STARS_RECOLLECTED, 1);
    } else {
        player_stats_add(PS_UNIQUE_POWER_STARS_COLLECTED, 1);
    }
    if (sVisitLevel != 0) {
        sVisitHadStar = TRUE;
    }
    if ((starIndex % 7) == STAR_BP_ACT_100_COINS) {
        player_stats_add(PS_100_COIN_STARS_COLLECTED, 1);
    }
    if (state != NULL && (state->flags & PLAYER_STATS_STAR_RED_COINS)) {
        player_stats_add(PS_RED_COIN_STARS_COLLECTED, 1);
    }
    if (castle_level(gCurrLevelNum) || gCurrCourseNum >= COURSE_BITDW) {
        player_stats_add(PS_CASTLE_SECRET_STARS_COLLECTED, 1);
    }
}

void player_stats_attack(struct Object *obj, s32 attackType) {
    struct PlayerStatsObject *state = object_state(obj);
    struct MarioState *m = gMarioState;
    if (!can_record_gameplay() || state == NULL || player_stats_active_file() < 0 || m == NULL) {
        return;
    }
    if (obj->oHealth > 0) {
        state->flags &= ~PLAYER_STATS_OBJECT_DEFEATED;
    }
    state->defeatStat = PS_NONE;
    state->flags &= ~PLAYER_STATS_METHOD_CREDIT;
    if (attackType == ATTACK_KICK_OR_TRIP
        && (m->action == ACT_PUNCHING || m->action == ACT_MOVE_PUNCHING)
        && m->actionArg == ACT_ARG_PUNCH_SEQUENCE_BREAKDANCE) {
        state->flags |= PLAYER_STATS_BREAKDANCE_CREDIT;
    }
    if (m->riddenObj != NULL || (m->action & ACT_FLAG_RIDING_SHELL)) {
        state->defeatStat = PS_ENEMIES_DEFEATED_WHILE_RIDING_SHELLS;
    } else if (attackType == ATTACK_FROM_ABOVE) {
        state->defeatStat = PS_ENEMIES_STOMPED;
    } else if (attackType == ATTACK_PUNCH) {
        state->defeatStat = PS_ENEMIES_PUNCHED;
    } else if (attackType == ATTACK_KICK_OR_TRIP) {
        state->defeatStat = PS_ENEMIES_KICKED;
    } else if (attackType == ATTACK_GROUND_POUND_OR_TWIRL
               && (m->action == ACT_GROUND_POUND || m->action == ACT_GROUND_POUND_LAND)) {
        state->defeatStat = PS_ENEMIES_GROUND_POUNDED;
    }
    state->creditFile = player_stats_active_file();
    state->attackFrame = sGameplayFrames;
    if (obj_has_behavior(obj, bhvPokeyBodyPart) && obj->parentObj != NULL && obj->parentObj != obj) {
        player_stats_attack(obj->parentObj, attackType);
    }
}

static s32 is_bully(struct Object *obj) {
    return obj_has_behavior(obj, bhvSmallBully) || obj_has_behavior(obj, bhvBigBully)
           || obj_has_behavior(obj, bhvBigBullyWithMinions) || obj_has_behavior(obj, bhvSmallChillBully)
           || obj_has_behavior(obj, bhvBigChillBully);
}

void player_stats_enemy_bounce(struct Object *obj) {
    if (!can_record_gameplay() || player_stats_active_file() != gCurrSaveFileNum - 1 || obj == NULL
        || (!mario_ground_twirl_is_enemy(obj) && !mario_ground_twirl_is_immune_enemy(obj)
            && !is_bully(obj))) {
        return;
    }
    if (sMovement.enemyBounces != ~(u64) 0) {
        sMovement.enemyBounces++;
    }
    record(PS_ENEMY_BOUNCE_STREAK, sMovement.enemyBounces);
}

void player_stats_inherit_attack(struct Object *source, struct Object *target) {
    struct PlayerStatsObject *from = object_state(source);
    struct PlayerStatsObject *to = object_state(target);
    if (can_record_gameplay() && from != NULL && to != NULL
        && from->creditFile == player_stats_active_file() && from->creditFile >= 0
        && (u32) (sGameplayFrames - from->attackFrame) <= PLAYER_STATS_KILL_CREDIT_FRAMES) {
        to->creditFile = from->creditFile;
        to->attackFrame = sGameplayFrames;
        // A thrown object is a credited defeat, without inventing an attack subtype.
        to->defeatStat = PS_NONE;
        to->flags =
            (to->flags & ~PLAYER_STATS_METHOD_CREDIT) | (from->flags & PLAYER_STATS_THROWN_CREDIT);
    }
}

void player_stats_enemy_defeated(struct Object *obj) {
    struct PlayerStatsObject *state = object_state(obj);
    if (!can_record_gameplay() || state == NULL || (state->flags & PLAYER_STATS_OBJECT_DEFEATED)
        || state->creditFile != player_stats_active_file() || state->creditFile < 0
        || (u32) (sGameplayFrames - state->attackFrame) > PLAYER_STATS_KILL_CREDIT_FRAMES) {
        return;
    }
    if (obj_has_behavior(obj, bhvPokeyBodyPart) || obj_has_behavior(obj, bhvWigglerBody)
        || obj_has_behavior(obj, bhvEyerokHand)) {
        return;
    }
    if (!mario_ground_twirl_is_enemy(obj) && !mario_ground_twirl_is_immune_enemy(obj) && !is_bully(obj)
        && !obj_has_behavior(obj, bhvGhostHuntBigBoo) && !obj_has_behavior(obj, bhvMerryGoRoundBigBoo)
        && !obj_has_behavior(obj, bhvBalconyBigBoo) && !obj_has_behavior(obj, bhvEyerokBoss)) {
        return;
    }
    state->flags |= PLAYER_STATS_OBJECT_DEFEATED;
    player_stats_add(PS_TOTAL_ENEMIES_DEFEATED, 1);
    if (sVisitLevel != 0) {
        if (sVisitEnemies != ~(u64) 0) {
            sVisitEnemies++;
        }
        record(PS_MOST_ENEMIES_DEFEATED_IN_ONE_VISIT, sVisitEnemies);
    }
    if (state->flags & PLAYER_STATS_BREAKDANCE_CREDIT) {
        player_stats_add(PS_BREAKDANCE_DEFEATS, 1);
    }
    if (state->flags & PLAYER_STATS_THROWN_CREDIT) {
        player_stats_add(PS_THROWN_OBJECT_DEFEATS, 1);
    }
    if (state->defeatStat != PS_NONE) {
        player_stats_add(state->defeatStat, 1);
    }
    if (obj_has_behavior(obj, bhvGoomba)
#ifdef FLOOMBAS
        || obj_has_behavior(obj, bhvFloomba)
#endif
    ) {
        player_stats_add(PS_GOOMBAS_DEFEATED, 1);
    }
    if (obj_has_behavior(obj, bhvBoo) || obj_has_behavior(obj, bhvGhostHuntBoo)
        || obj_has_behavior(obj, bhvMerryGoRoundBoo) || obj_has_behavior(obj, bhvGhostHuntBigBoo)
        || obj_has_behavior(obj, bhvMerryGoRoundBigBoo) || obj_has_behavior(obj, bhvBalconyBigBoo)
        || obj_has_behavior(obj, bhvBooWithCage)) {
        player_stats_add(PS_BOOS_DEFEATED, 1);
    }
    if (is_bully(obj) && obj->oAction == OBJ_ACT_LAVA_DEATH) {
        player_stats_add(PS_BULLIES_KNOCKED_INTO_LAVA, 1);
    }
}

void player_stats_enemy_defeated_after_dialogue(struct Object *obj) {
    struct PlayerStatsObject *state = object_state(obj);
    if (!can_record_gameplay() || state == NULL || state->creditFile < 0
        || state->creditFile != player_stats_active_file()) {
        return;
    }
    // A confirmed scripted defeat retains its accepted strike through the dialogue.
    // Refresh only its age: never replace the method, owner or defeated flag.
    state->attackFrame = sGameplayFrames;
    player_stats_enemy_defeated(obj);
}

void player_stats_grab(struct Object *obj) {
    if (obj == NULL) {
        return;
    }
    player_stats_add(PS_OBJECTS_PICKED_UP, 1);
    if (obj_has_behavior(obj, bhvBowser)) {
        player_stats_add(PS_BOWSER_TAIL_GRABS, 1);
    }
    if (obj_has_behavior(obj, bhvMips)) {
        player_stats_add(PS_MIPS_CAUGHT, 1);
    }
}

void player_stats_throw(struct Object *obj) {
    if (obj == NULL) {
        return;
    }
    player_stats_add(PS_OBJECTS_THROWN, 1);
    player_stats_attack(obj, ATTACK_NONE);
    struct PlayerStatsObject *state = object_state(obj);
    if (state != NULL && can_record_gameplay() && state->creditFile == player_stats_active_file()) {
        state->flags &= ~PLAYER_STATS_BREAKDANCE_CREDIT;
        state->flags |= PLAYER_STATS_THROWN_CREDIT;
    }
    if (obj_has_behavior(obj, bhvBowser)) {
        player_stats_add(PS_BOWSER_THROWS, 1);
    }
}

void player_stats_twirl_hit(struct Object *obj) {
    player_stats_add(PS_TWIRL_ENEMY_HITS, 1);
    player_stats_attack(obj, ATTACK_NONE);
}

void player_stats_twirl_start(s32 airborne, s32 continuation) {
    if (!continuation) {
        sMovement.twirlRebounds = 0;
        player_stats_add(airborne ? PS_AIR_TWIRLS : PS_GROUND_TWIRLS, 1);
    }
}

void player_stats_twirl_wall_rebound(s32 airborne) {
    player_stats_add(PS_TWIRL_WALL_REBOUNDS, 1);
    player_stats_add(airborne ? PS_AIR_TWIRL_WALL_REBOUNDS : PS_GROUND_TWIRL_WALL_REBOUNDS, 1);
    if (sMovement.twirlRebounds != ~0u) {
        sMovement.twirlRebounds++;
    }
    record(PS_LONGEST_TWIRL_WALL_REBOUND_CHAIN, sMovement.twirlRebounds);
}

void player_stats_metal_contact(struct Object *obj, s32 passive) {
    struct PlayerStatsObject *state = object_state(obj);
    player_stats_attack(obj, ATTACK_NONE);
    if (state != NULL && passive && state->creditFile == player_stats_active_file()
        && state->creditFile >= 0) {
        state->defeatStat = PS_ENEMIES_DEFEATED_BY_PASSIVE_METAL_CONTACT;
    }
}

void player_stats_metal_defeat(struct Object *obj) {
    struct PlayerStatsObject *state = object_state(obj);
    s32 passive = state != NULL && state->creditFile == player_stats_active_file()
                  && state->defeatStat == PS_ENEMIES_DEFEATED_BY_PASSIVE_METAL_CONTACT;
    // Native defeat may run on the next object update; retain its committed origin.
    player_stats_metal_contact(obj, passive);
}

void player_stats_cap_powers_removed(u32 powers, s32 breakdance) {
    u64 count = ((powers & MARIO_WING_CAP) != 0) + ((powers & MARIO_METAL_CAP) != 0)
                + ((powers & MARIO_VANISH_CAP) != 0);
    player_stats_add(breakdance ? PS_POWER_CAPS_SHED_BY_BREAKDANCE : PS_CAP_POWERS_LOST_TO_DAMAGE,
                     count);
}

void player_stats_cap_lost(void) {
    player_stats_add(PS_TIMES_MARIO_S_CAP_LOST, 1);
}

void player_stats_cap_recovered(void) {
    player_stats_add(PS_TIMES_MARIO_S_CAP_RECOVERED, 1);
}
