#ifndef SAVE_FILE_H
#define SAVE_FILE_H

#include <PR/ultratypes.h>

#include "types.h"
#include "area.h"

#include "course_table.h"
#include "level_table.h"

#define SRAM_SIZE 0x8000

#define NUM_SAVE_FILES 4
// Non-star high bit of the unused cake course byte; Secret Aquarium has no cannon.
#define SAVE_CAKE_CASTLE_WING_CAP_UNLOCKED 0x80
// The ending course has no stars. Its low nibble stores the next unread finale conversation.
#define SAVE_CAKE_CASTLE_LOBBY_PROGRESS_MASK 0x0F
#define SAVE_CAKE_CASTLE_SECRET_MOVED 0x10
#define SAVE_CAKE_CASTLE_LOBBY_SIGN_READ 0x20
#define CASTLE_LOBBY_CONVERSATION_COUNT 10

#define CASTLE_SECRET_MOVED (1 << 0)
#define CASTLE_SECRET_AWAY (1 << 1)
#define CASTLE_SECRET_INTRO_READ (1 << 2)
#define CASTLE_SECRET_POUNDED_ONCE (1 << 3)
#define CASTLE_SECRET_OW_READ (1 << 4)
#define CASTLE_SECRET_POUNDED_TWICE (1 << 5)
#define CASTLE_SECRET_WHY_READ (1 << 6)
#define CASTLE_SECRET_LOBBY_SIGN_READ (1 << 7)
#define CASTLE_SECRET_MOTOS_TALKED (1 << 8)

struct SaveBlockSignature {
    u16 magic;
    u16 chksum;
};

struct SaveFile {
    // Location of lost cap.
    // Note: the coordinates get set, but are never actually used, since the
    // cap can always be found in a fixed spot within the course
    u8 capLevel; // Low six bits: level. High bits: SimpleFlips visit and pending game over.
    u8 capArea;  // Low four bits: area. Bits 4-6: camera speed. High bit: pause easter egg.
    // Note: the coordinates get set, but are never actually used, since the
    // cap can always be found in a fixed spot within the course
    Vec3s capPos; // 48 bits

    u32 flags;

    // Star flags for each course.
    // The most significant bit of the byte *following* each course is set if the
    // cannon is open.
    u8 courseStars[COURSE_COUNT]; // 200 bits

    u8 courseCoinScores[COURSE_STAGES_COUNT]; // 120 bits

    struct SaveBlockSignature signature; // 32 bits
};

enum SaveFileIndex {
    SAVE_FILE_A,
    SAVE_FILE_B,
    SAVE_FILE_C,
    SAVE_FILE_D
};

// Preserve the base save layout without borrowing a castle-secret-star flag.
#define SAVE_CAP_AREA_MASK 0x0F
#define SAVE_CAP_AREA_CAMERA_SPEED_MASK 0x70
#define SAVE_CAP_AREA_CAMERA_SPEED_SHIFT 4
#define SAVE_CAP_AREA_PAUSE_EASTER_EGG 0x80
STATIC_ASSERT(AREA_COUNT <= SAVE_CAP_AREA_MASK, "Saved cap areas overlap camera speed bits");
#define SAVE_CAP_LEVEL_MASK 0x3F
#define SAVE_CAP_LEVEL_SIMPLEFLIPS_GAME_OVER 0x40
#define SAVE_CAP_LEVEL_SIMPLEFLIPS 0x80
#define SAVE_CAP_LEVEL_METADATA (SAVE_CAP_LEVEL_SIMPLEFLIPS | SAVE_CAP_LEVEL_SIMPLEFLIPS_GAME_OVER)
STATIC_ASSERT(LEVEL_COUNT <= SAVE_CAP_LEVEL_MASK, "Saved cap levels overlap the SimpleFlips metadata");

struct MainMenuSaveData {
    // Each save file has a 2 bit "age" for each course. The higher this value,
    // the older the high score is. This is used for tie-breaking when displaying
    // on the high score screen.
    u32 coinScoreAges[NUM_SAVE_FILES];
    u8 soundMode: 2;
#ifdef WIDE
    u8 wideMode: 1;
#endif
#ifdef REONUCAM
    u8 cameraSpeedSetting : 3; // Reserved legacy global setting; speed is now save-file owned.
#endif

#if MULTILANG
    u8 language: 2;
#define SUBTRAHEND 8
#else
#define SUBTRAHEND 6
#endif
    u8 firstBoot; // Reserved legacy camera byte; retain the persisted save layout.

    struct SaveBlockSignature signature;
};

struct SaveBuffer {
    // Each of the four save files has two copies. If one is bad, the other is used as a backup.
    struct SaveFile files[NUM_SAVE_FILES][2];
    // Main menu data, storing config options.
    struct MainMenuSaveData menuData;
};

STATIC_ASSERT(sizeof(struct SaveBuffer) <= SRAM_SIZE, "ERROR: Save struct too big for SRAM");

extern u8 gLastCompletedCourseNum;
extern u8 gLastCompletedStarNum;
extern s8 sUnusedGotGlobalCoinHiScore;
extern u8 gGotFileCoinHiScore;
extern u8 gCurrCourseStarFlags;
extern u8 gSpecialTripleJump;
extern s8 gLevelToCourseNumTable[];

enum CourseFlags {
    COURSE_FLAG_CANNON_UNLOCKED      = (1 <<  7), /* 0x00000080 */
};

// game progress flags
enum SaveProgressFlags {
    SAVE_FLAG_FILE_EXISTS = (1 << 0),            /* 0x00000001 */
    SAVE_FLAG_HAVE_WING_CAP = (1 << 1),          /* 0x00000002 */
    SAVE_FLAG_HAVE_METAL_CAP = (1 << 2),         /* 0x00000004 */
    SAVE_FLAG_HAVE_VANISH_CAP = (1 << 3),        /* 0x00000008 */
    SAVE_FLAG_HAVE_KEY_1 = (1 << 4),             /* 0x00000010 */
    SAVE_FLAG_HAVE_KEY_2 = (1 << 5),             /* 0x00000020 */
    SAVE_FLAG_UNLOCKED_BASEMENT_DOOR = (1 << 6), /* 0x00000040 */
    SAVE_FLAG_UNLOCKED_UPSTAIRS_DOOR = (1 << 7), /* 0x00000080 */
    SAVE_FLAG_DDD_MOVED_BACK = (1 << 8),         /* 0x00000100 */
    SAVE_FLAG_MOAT_DRAINED = (1 << 9),           /* 0x00000200 */
    SAVE_FLAG_UNLOCKED_PSS_DOOR = (1 << 10),     /* 0x00000400 */
    SAVE_FLAG_UNLOCKED_WF_DOOR = (1 << 11),      /* 0x00000800 */
    SAVE_FLAG_UNLOCKED_CCM_DOOR = (1 << 12),     /* 0x00001000 */
    SAVE_FLAG_UNLOCKED_JRB_DOOR = (1 << 13),     /* 0x00002000 */
    SAVE_FLAG_UNLOCKED_BITDW_DOOR = (1 << 14),   /* 0x00004000 */
    SAVE_FLAG_UNLOCKED_BITFS_DOOR = (1 << 15),   /* 0x00008000 */
    SAVE_FLAG_CAP_ON_GROUND = (1 << 16),         /* 0x00010000 */
    SAVE_FLAG_CAP_ON_KLEPTO = (1 << 17),         /* 0x00020000 */
    SAVE_FLAG_CAP_ON_UKIKI = (1 << 18),          /* 0x00040000 */
    SAVE_FLAG_CAP_ON_MR_BLIZZARD = (1 << 19),    /* 0x00080000 */
    SAVE_FLAG_UNLOCKED_50_STAR_DOOR = (1 << 20), /* 0x00100000 */
    // File C only. Spare progress bits preserve the existing base save layout.
    SAVE_FLAG_HE_THICC_PENDING = (1 << 21),
    SAVE_FLAG_HE_THICC = (1 << 22),
    SAVE_FLAG_HE_THICC_INTRO = (1 << 23),
    SAVE_FLAG_COLLECTED_TOAD_STAR_1 = (1 << 24), /* 0x01000000 */
    SAVE_FLAG_COLLECTED_TOAD_STAR_2 = (1 << 25), /* 0x02000000 */
    SAVE_FLAG_COLLECTED_TOAD_STAR_3 = (1 << 26), /* 0x04000000 */
    SAVE_FLAG_COLLECTED_MIPS_STAR_1 = (1 << 27), /* 0x08000000 */
    SAVE_FLAG_COLLECTED_MIPS_STAR_2 = (1 << 28), /* 0x10000000 */
    SAVE_FLAG_TAS_THICC = (1 << 29),
    // Outside the five castle-secret-star bits; no save-layout change is needed.
    SAVE_FLAG_GAME_COMPLETED = (1U << 31),
};

enum StarFlags {
    STAR_FLAGS_NONE = (0 << 0),         // 0x00
    STAR_FLAG_ACT_1 = (1 << 0),         // 0x01
    STAR_FLAG_ACT_2 = (1 << 1),         // 0x02
    STAR_FLAG_ACT_3 = (1 << 2),         // 0x04
    STAR_FLAG_ACT_4 = (1 << 3),         // 0x08
    STAR_FLAG_ACT_5 = (1 << 4),         // 0x10
    STAR_FLAG_ACT_6 = (1 << 5),         // 0x20
    STAR_FLAG_ACT_100_COINS = (1 << 6), // 0x40
    STAR_FLAG_LAST = STAR_FLAG_ACT_100_COINS
};

/* Only the three Toad and two MIPS bits are stars; higher bits hold file metadata. */
#define SAVE_FLAG_TO_STAR_FLAG(cmd) (((cmd) >> 24) & 0x1F)
#define STAR_FLAG_TO_SAVE_FLAG(cmd) (((cmd) & 0x1F) << 24)

// Variable for setting a warp checkpoint.

// possibly a WarpDest struct where arg is a union. HACKERSM64_DO: Check?
struct WarpCheckpoint {
    /*0x00*/ u8 actNum;
    /*0x01*/ u8 courseNum;
    /*0x02*/ u8 levelID;
    /*0x03*/ u8 areaNum;
    /*0x04*/ u8 warpNode;
};

extern struct WarpCheckpoint gWarpCheckpoint;

extern s8 gMainMenuDataModified;
extern s8 gSaveFileModified;

s32 save_file_has_completed_game(s32 fileIndex);
void save_file_mark_game_completed(void);
s32 save_file_has_pause_easter_egg(s32 fileIndex);
void save_file_mark_pause_easter_egg(void);
s32 save_file_has_simpleflips_encounter(s32 fileIndex);
void save_file_mark_simpleflips_encounter(void);
void save_file_mark_simpleflips_game_over(void);
// Consumes one pending game over on boot; returns its 1-based file number, or zero.
s32 save_file_consume_simpleflips_game_over(void);

void save_file_do_save(s32 fileIndex);
u32 save_file_get_castle_secret(s32 fileIndex);
void save_file_set_castle_secret(s32 fileIndex, u32 flags);
void save_file_flush_castle_secret(void);
s32 save_file_has_castle_wing_cap(s32 fileIndex);
void save_file_unlock_castle_wing_cap(s32 fileIndex);
void save_file_erase(s32 fileIndex);
s32 save_file_try_erase(s32 fileIndex);
// TRUE means replacement accepted/queued, not durable; FALSE leaves the destination unchanged.
s32 save_file_copy(s32 srcFileIndex, s32 destFileIndex);
s32 save_file_is_he_thicc(s32 fileIndex);
s32 save_file_is_tas_thicc(s32 fileIndex);
void save_file_unlock_tas_thicc(void);
s32 save_file_he_thicc_pending(void);
s32 save_file_he_thicc_intro_pending(s32 fileIndex);
void save_file_arm_he_thicc(void);
void save_file_start_he_thicc(s32 fileIndex);
void save_file_finish_he_thicc_intro(void);
void save_file_load_all(void);
void save_file_reload(void);
void save_file_collect_star_or_key(s16 coinScore, s16 starIndex);
s32 save_file_exists(s32 fileIndex);
u32 save_file_get_max_coin_score(s32 courseIndex);
s32 save_file_get_course_star_count(s32 fileIndex, s32 courseIndex);
s32 save_file_get_total_star_count(s32 fileIndex, s32 minCourse, s32 maxCourse);
s32 save_file_get_saved_star_count(s32 fileIndex);
s32 save_file_get_castle_lobby_progress(s32 fileIndex);
void save_file_set_castle_lobby_progress(s32 fileIndex, s32 progress);
void save_file_set_flags(u32 flags);
void save_file_clear_flags(u32 flags);
u32 save_file_get_flags(void);
u32 save_file_get_star_flags(s32 fileIndex, s32 courseIndex);
void save_file_set_star_flags(s32 fileIndex, s32 courseIndex, u32 starFlags);
s32 save_file_get_course_coin_score(s32 fileIndex, s32 courseIndex);
s32 save_file_is_cannon_unlocked(void);
void save_file_set_cannon_unlocked(void);
void save_file_set_cap_pos(s16 x, s16 y, s16 z);
s32 save_file_get_cap_pos(Vec3s capPos);
void save_file_set_sound_mode(u16 mode);
u32 save_file_get_sound_mode(void);
#ifdef WIDE
u32 save_file_get_widescreen_mode(void);
void save_file_set_widescreen_mode(u8 mode);
#endif
#ifdef REONUCAM
u8 save_file_get_camera_speed(void);
void save_file_set_camera_speed(u8 speed);
#endif

void disable_warp_checkpoint(void);
void check_if_should_set_warp_checkpoint(struct WarpNode *warpNode);
s32 check_warp_checkpoint(struct WarpNode *warpNode);

#if MULTILANG
enum EuLanguages {
    LANGUAGE_ENGLISH,
    LANGUAGE_FRENCH,
    LANGUAGE_GERMAN
};

void eu_set_language(u16 language);
u32 eu_get_language(void);
#else
#define LANGUAGE_ENGLISH 0
#endif

#endif // SAVE_FILE_H
