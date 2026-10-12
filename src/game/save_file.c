#include "game_options_runtime.h"
#include "player_stats.h"
#include "game_options.h"
#include "game_timing.h"
#include <ultra64.h>

#include "sm64.h"
#include "game_init.h"
#include "main.h"
#include "audio/external.h"
#include "engine/math_util.h"
#include "area.h"
#include "camera.h"
#include "level_update.h"
#include "save_file.h"
#include "behavior_actions.h"
#include "sound_init.h"
#include "level_table.h"
#include "course_table.h"
#include "level_commands.h"
#include "rumble_init.h"
#include "config.h"
#include "emutest.h"
#include "sram.h"

#ifdef UNIQUE_SAVE_DATA
u16 MENU_DATA_MAGIC = 0x4849;
u16 SAVE_FILE_MAGIC = 0x4441;
#else
#define MENU_DATA_MAGIC 0x4849
#define SAVE_FILE_MAGIC 0x4441
#endif

extern struct SaveBuffer gSaveBuffer;

struct WarpCheckpoint gWarpCheckpoint;

s8 gMainMenuDataModified;
s8 gSaveFileModified;
static u8 sModifiedFiles;
// Failed explicit saves still own a retry after restoring their cached backup.
static u8 sFailedSaveFiles;
// A transport failure leaves persisted data unknown, rather than proven invalid.
static u8 sUnreadFiles;
static u8 sMenuDataUnread;
static u32 sCastleSecretSavePending;

u8 gLastCompletedCourseNum = COURSE_NONE;
u8 gLastCompletedStarNum = 0;
s8 sUnusedGotGlobalCoinHiScore = FALSE;
u8 gGotFileCoinHiScore = FALSE;
u8 gCurrCourseStarFlags = 0;

u8 gSpecialTripleJump = FALSE;

#define STUB_LEVEL(_0, _1, courseenum, _3, _4, _5, _6, _7, _8) courseenum,
#define DEFINE_LEVEL(_0, _1, courseenum, _3, _4, _5, _6, _7, _8, _9, _10) courseenum,

s8 gLevelToCourseNumTable[] = {
    #include "levels/level_defines.h"
};
#undef STUB_LEVEL
#undef DEFINE_LEVEL

STATIC_ASSERT(ARRAY_COUNT(gLevelToCourseNumTable) == LEVEL_COUNT - 1,
              "change this array if you are adding levels");
_Static_assert(sizeof(struct SaveBuffer) <= GAME_OPTIONS_SRAM_OFFSET,
               "Base save overlaps options storage");
_Static_assert(sizeof(struct SaveBuffer) <= PLAYER_STATS_LEGACY_SRAM_OFFSET,
               "Base save overlaps player statistics");

static s32 player_stats_storage_read(u32 offset, void *data, u32 size) {
    s32 status = 1;
    s32 tries = 4;
    if (gSramProbe == 0 || offset > SRAM_SIZE || size > SRAM_SIZE - offset) {
        return status;
    }
    do {
        block_until_rumble_pak_free();
        status = nuPiReadSram(offset, data, size);
        release_rumble_pak_control();
    } while (status != 0 && --tries > 0);
    return status;
}

static s32 player_stats_storage_write(u32 offset, const void *data, u32 size) {
    s32 status = 1;
    s32 tries = 4;
    if (gSramProbe == 0 || offset > SRAM_SIZE || size > SRAM_SIZE - offset) {
        return status;
    }
    do {
        block_until_rumble_pak_free();
        status = nuPiWriteSram(offset, (void *) data, size);
        release_rumble_pak_control();
    } while (status != 0 && --tries > 0);
    return status;
}

/**
 * Read from SRAM to a given address.
 * The SRAM address is computed using the offset of the destination address from gSaveBuffer.
 * Try at most 4 times, and return 0 on success. On failure, return the status returned from
 * nuPiReadSram. It also returns 0 if SRAM isn't loaded correctly in the system.
 */
static s32 read_sram_data(void *buffer, s32 size) {
    s32 status = 0;

    if (gSramProbe != 0) {
        s32 triesLeft = 4;
        u32 offset = (u32)((u8 *) buffer - (u8 *) &gSaveBuffer);

        do {
            block_until_rumble_pak_free();
            triesLeft--;
            status = nuPiReadSram(offset, buffer, ALIGN4(size));
            release_rumble_pak_control();
        } while (triesLeft > 0 && status != 0);
    }

    return status;
}

/**
 * Write data to SRAM.
 * The SRAM address is computed using the offset of the source address from gSaveBuffer.
 * Try at most 4 times, and return 0 on success. On failure, return the status returned from
 * nuPiWriteSram. Unlike read_sram_data, return 1 if SRAM isn't loaded.
 */
static s32 write_sram_data(void *buffer, s32 size) {
    s32 status = 1;

    if (gSramProbe != 0) {
        s32 triesLeft = 4;
        u32 offset = (u32)((u8 *) buffer - (u8 *) &gSaveBuffer);

        do {
            block_until_rumble_pak_free();
            triesLeft--;
            status = nuPiWriteSram(offset, buffer, ALIGN4(size));
            release_rumble_pak_control();
        } while (triesLeft > 0 && status != 0);
    }

    return status;
}

/**
 * Sum the bytes in data to data + size - 2. The last two bytes are ignored
 * because that is where the checksum is stored.
 */
static u16 calc_checksum(u8 *data, s32 size) {
    u16 chksum = 0;

    while (size-- > 2) {
        chksum += *data++;
    }
    return chksum;
}

/**
 * Verify the signature at the end of the block to check if the data is valid.
 */
static s32 verify_save_block_signature(void *buffer, s32 size, u16 magic) {
    struct SaveBlockSignature *sig = (struct SaveBlockSignature *) ((size - 4) + (u8 *) buffer);

    if (sig->magic != magic) {
        return FALSE;
    }
    if (sig->chksum != calc_checksum(buffer, size)) {
        return FALSE;
    }
    return TRUE;
}

/**
 * Write a signature at the end of the block to make sure the data is valid
 */
static void add_save_block_signature(void *buffer, s32 size, u16 magic) {
    struct SaveBlockSignature *sig = (struct SaveBlockSignature *) ((size - 4) + (u8 *) buffer);

    sig->magic = magic;
    sig->chksum = calc_checksum(buffer, size);
}

static void save_main_menu_data(void) {
    if (gMainMenuDataModified && !sMenuDataUnread) {
        // Compute checksum
        add_save_block_signature(&gSaveBuffer.menuData, sizeof(gSaveBuffer.menuData), MENU_DATA_MAGIC);

        // Write to SRAM
        if (write_sram_data(&gSaveBuffer.menuData, sizeof(gSaveBuffer.menuData)) == 0) {
            gMainMenuDataModified = FALSE;
        }
    }
}

static void wipe_main_menu_data(void) {
    bzero(&gSaveBuffer.menuData, sizeof(gSaveBuffer.menuData));

    // Set score ages for all courses to 3, 2, 1, and 0, respectively.
    gSaveBuffer.menuData.coinScoreAges[0] = 0x3FFFFFFF;
    gSaveBuffer.menuData.coinScoreAges[1] = 0x2AAAAAAA;
    gSaveBuffer.menuData.coinScoreAges[2] = 0x15555555;

    gMainMenuDataModified = TRUE;
    save_main_menu_data();
}

static s32 get_coin_score_age(s32 fileIndex, s32 courseIndex) {
    return (gSaveBuffer.menuData.coinScoreAges[fileIndex] >> (2 * courseIndex)) & 0x3;
}

static void set_coin_score_age(s32 fileIndex, s32 courseIndex, s32 age) {
    s32 mask = 0x3 << (2 * courseIndex);

    gSaveBuffer.menuData.coinScoreAges[fileIndex] &= ~mask;
    gSaveBuffer.menuData.coinScoreAges[fileIndex] |= age << (2 * courseIndex);
}

/**
 * Mark a coin score for a save file as the newest out of all save files.
 */
static void touch_coin_score_age(s32 fileIndex, s32 courseIndex) {
    s32 i;
    u32 age;
    u32 currentAge = get_coin_score_age(fileIndex, courseIndex);

    if (currentAge != 0) {
        for (i = 0; i < NUM_SAVE_FILES; i++) {
            age = get_coin_score_age(i, courseIndex);
            if (age < currentAge) {
                set_coin_score_age(i, courseIndex, age + 1);
            }
        }

        set_coin_score_age(fileIndex, courseIndex, 0);
        gMainMenuDataModified = TRUE;
    }
}

/**
 * Mark all coin scores for a save file as new.
 */
static void touch_high_score_ages(s32 fileIndex) {
    s32 i;

    for (i = COURSE_NUM_TO_INDEX(COURSE_MIN); i <= COURSE_NUM_TO_INDEX(COURSE_STAGES_MAX); i++) {
        touch_coin_score_age(fileIndex, i);
    }
}

/**
 * Copy save file data from one backup slot to the other slot.
 */
static void restore_save_file_data(s32 fileIndex, s32 srcSlot) {
    s32 destSlot = srcSlot ^ 1;

    // Compute checksum on source data
    add_save_block_signature(&gSaveBuffer.files[fileIndex][srcSlot],
                             sizeof(gSaveBuffer.files[fileIndex][srcSlot]), SAVE_FILE_MAGIC);

    // Copy source data to destination slot
    bcopy(&gSaveBuffer.files[fileIndex][srcSlot], &gSaveBuffer.files[fileIndex][destSlot],
          sizeof(gSaveBuffer.files[fileIndex][destSlot]));

    // Write destination data to SRAM
    write_sram_data(&gSaveBuffer.files[fileIndex][destSlot],
                    sizeof(gSaveBuffer.files[fileIndex][destSlot]));
}

static void mark_save_file_modified(s32 fileIndex) {
    sModifiedFiles |= 1u << fileIndex;
    gSaveFileModified = TRUE;
}

static s32 save_file_write_progress(s32 fileIndex) {
    if (sUnreadFiles & (1u << fileIndex)) {
        return FALSE;
    }
    if (sModifiedFiles & (1u << fileIndex)) {
        // Compute checksum
        add_save_block_signature(&gSaveBuffer.files[fileIndex][0],
                                 sizeof(gSaveBuffer.files[fileIndex][0]), SAVE_FILE_MAGIC);

        // Copy to backup slot
        bcopy(&gSaveBuffer.files[fileIndex][0], &gSaveBuffer.files[fileIndex][1],
              sizeof(gSaveBuffer.files[fileIndex][1]));

        // Write to SRAM
        if (write_sram_data(&gSaveBuffer.files[fileIndex], sizeof(gSaveBuffer.files[fileIndex])) == 0) {
            sModifiedFiles &= ~(1u << fileIndex);
            sFailedSaveFiles &= ~(1u << fileIndex);
            gSaveFileModified = sModifiedFiles != 0;
            sCastleSecretSavePending &= ~(1u << fileIndex);
        } else {
            sFailedSaveFiles |= 1u << fileIndex;
            return FALSE;
        }
    }

    return TRUE;
}

void save_file_do_save(s32 fileIndex) {
    if (sUnreadFiles & (1u << fileIndex)) {
        return;
    }
    save_file_write_progress(fileIndex);

    player_stats_checkpoint(fileIndex);
    game_timing_checkpoint(fileIndex);
    if (gCurrDemoInput == NULL && gCurrCreditsEntry == NULL) {
#ifdef REONUCAM
        if (fileIndex >= 0 && fileIndex < NUM_SAVE_FILES && !game_options_has_record(fileIndex)) {
            u8 encoded = (gSaveBuffer.files[fileIndex][0].capArea & SAVE_CAP_AREA_CAMERA_SPEED_MASK)
                         >> SAVE_CAP_AREA_CAMERA_SPEED_SHIFT;
            game_options_set_saved_camera_speed(
                fileIndex,
                encoded > 0 && encoded <= REONUCAM_SPEED_COUNT ? encoded - 1 : REONUCAM_DEFAULT_SPEED);
        }
#endif
        game_options_save(fileIndex);
    }
    save_main_menu_data();
}

s32 save_file_has_completed_game(s32 fileIndex) {
    return fileIndex >= 0 && fileIndex < NUM_SAVE_FILES
           && (gSaveBuffer.files[fileIndex][0].flags & SAVE_FLAG_GAME_COMPLETED) != 0;
}

void save_file_mark_game_completed(void) {
    s32 fileIndex = gCurrSaveFileNum - 1;
    if (gCurrDemoInput != NULL || gCurrCreditsEntry != NULL || fileIndex < 0
        || fileIndex >= NUM_SAVE_FILES) {
        return;
    }
    save_file_set_flags(SAVE_FLAG_GAME_COMPLETED);
    save_file_do_save(fileIndex);
}

s32 save_file_has_pause_easter_egg(s32 fileIndex) {
    return fileIndex >= 0 && fileIndex < NUM_SAVE_FILES
           && (gSaveBuffer.files[fileIndex][0].capArea & SAVE_CAP_AREA_PAUSE_EASTER_EGG) != 0;
}

void save_file_mark_pause_easter_egg(void) {
    if (game_options_secrets_blocked()) {
        return;
    }
    s32 fileIndex = gCurrSaveFileNum - 1;

    if (gCurrDemoInput != NULL || gCurrCreditsEntry != NULL || fileIndex < 0
        || fileIndex >= NUM_SAVE_FILES) {
        return;
    }
    gSaveBuffer.files[fileIndex][0].capArea |= SAVE_CAP_AREA_PAUSE_EASTER_EGG;
    save_file_set_flags(SAVE_FLAG_FILE_EXISTS);
    save_file_do_save(fileIndex);
}

s32 save_file_has_simpleflips_encounter(s32 fileIndex) {
    return fileIndex >= 0 && fileIndex < NUM_SAVE_FILES
           && (gSaveBuffer.files[fileIndex][0].capLevel & SAVE_CAP_LEVEL_SIMPLEFLIPS) != 0;
}

void save_file_mark_simpleflips_encounter(void) {
    if (game_options_secrets_blocked()) {
        return;
    }
    s32 fileIndex = gCurrSaveFileNum - 1;

    if (gCurrDemoInput != NULL || gCurrCreditsEntry != NULL || fileIndex < 0
        || fileIndex >= NUM_SAVE_FILES || save_file_has_simpleflips_encounter(fileIndex)) {
        return;
    }
    gSaveBuffer.files[fileIndex][0].capLevel |= SAVE_CAP_LEVEL_SIMPLEFLIPS;
    save_file_set_flags(SAVE_FLAG_FILE_EXISTS);
    save_file_do_save(fileIndex);
}

void save_file_mark_simpleflips_game_over(void) {
    if (game_options_secrets_blocked()) {
        return;
    }
    s32 fileIndex = gCurrSaveFileNum - 1;

    if (gCurrDemoInput != NULL || gCurrCreditsEntry != NULL || fileIndex < 0
        || fileIndex >= NUM_SAVE_FILES) {
        return;
    }
    gSaveBuffer.files[fileIndex][0].capLevel |= SAVE_CAP_LEVEL_SIMPLEFLIPS_GAME_OVER;
    save_file_set_flags(SAVE_FLAG_FILE_EXISTS);
    // Finish the SRAM write before the deliberate assertion stops the game thread.
    save_file_do_save(fileIndex);
}

s32 save_file_consume_simpleflips_game_over(void) {
    for (s32 fileIndex = 0; fileIndex < NUM_SAVE_FILES; fileIndex++) {
        struct SaveFile *saveFile = &gSaveBuffer.files[fileIndex][0];

        if ((saveFile->flags & SAVE_FLAG_FILE_EXISTS)
            && (saveFile->capLevel & SAVE_CAP_LEVEL_SIMPLEFLIPS_GAME_OVER)) {
            saveFile->capLevel &= ~SAVE_CAP_LEVEL_SIMPLEFLIPS_GAME_OVER;
            mark_save_file_modified(fileIndex);
            save_file_do_save(fileIndex);
            return fileIndex + 1;
        }
    }
    return 0;
}

void save_file_erase(s32 fileIndex) {
    sUnreadFiles &= ~(1u << fileIndex);
    player_stats_erase(fileIndex);
    game_timing_erase(fileIndex);
    game_options_erase(fileIndex);
    u32 pending = fileIndex == SAVE_FILE_C
                      ? gSaveBuffer.files[fileIndex][0].flags & SAVE_FLAG_HE_THICC_PENDING
                      : 0;
    touch_high_score_ages(fileIndex);
    bzero(&gSaveBuffer.files[fileIndex][0], sizeof(gSaveBuffer.files[fileIndex][0]));
    // An earned secret can wait for an occupied C to be erased. Once claimed,
    // the pending bit is gone, so erasing HE THICC really removes the unlock.
    gSaveBuffer.files[fileIndex][0].flags = pending;

    mark_save_file_modified(fileIndex);
    save_file_do_save(fileIndex);
}

s32 save_file_try_erase(s32 fileIndex) {
    // Menu erasure must preserve a trusted file when its base write is rejected.
    struct SaveFile previousFile[2];
    u8 fileMask = 1u << fileIndex;
    u8 previousUnread = sUnreadFiles & fileMask;
    u8 previousModified = sModifiedFiles & fileMask;
    u8 previousFailed = sFailedSaveFiles & fileMask;
    bcopy(&gSaveBuffer.files[fileIndex], previousFile, sizeof(previousFile));
    u32 pending = fileIndex == SAVE_FILE_C
                      ? gSaveBuffer.files[fileIndex][0].flags & SAVE_FLAG_HE_THICC_PENDING
                      : 0;
    sUnreadFiles &= ~fileMask;
    bzero(&gSaveBuffer.files[fileIndex][0], sizeof(gSaveBuffer.files[fileIndex][0]));
    gSaveBuffer.files[fileIndex][0].flags = pending;
    mark_save_file_modified(fileIndex);
    if (!save_file_write_progress(fileIndex)) {
        bcopy(previousFile, &gSaveBuffer.files[fileIndex], sizeof(previousFile));
        sUnreadFiles = (sUnreadFiles & ~fileMask) | previousUnread;
        sModifiedFiles = (sModifiedFiles & ~fileMask) | previousModified;
        sFailedSaveFiles = (sFailedSaveFiles & ~fileMask) | previousFailed;
        gSaveFileModified = sModifiedFiles != 0;
        return FALSE;
    }
    player_stats_erase(fileIndex);
    game_timing_erase(fileIndex);
    game_options_erase(fileIndex);
    touch_high_score_ages(fileIndex);
    save_file_do_save(fileIndex);
    return TRUE;
}

s32 save_file_copy(s32 srcFileIndex, s32 destFileIndex) {
    // Reject an incomplete source before replacing any part of the destination.
    if ((sUnreadFiles & (1u << srcFileIndex)) || !player_stats_can_copy(srcFileIndex)
        || !game_timing_can_copy(srcFileIndex) || !game_options_can_copy(srcFileIndex)) {
        return FALSE;
    }
    struct SaveFile previousDestination[2];
    u8 destinationMask = 1u << destFileIndex;
    u8 previousUnread = sUnreadFiles & destinationMask;
    u8 previousModified = sModifiedFiles & destinationMask;
    u8 previousFailed = sFailedSaveFiles & destinationMask;
    bcopy(&gSaveBuffer.files[destFileIndex], previousDestination, sizeof(previousDestination));
    u32 characterFlags = destFileIndex == SAVE_FILE_C
                             ? gSaveBuffer.files[destFileIndex][0].flags
                                   & (SAVE_FLAG_HE_THICC_PENDING | SAVE_FLAG_HE_THICC
                                      | SAVE_FLAG_HE_THICC_INTRO | SAVE_FLAG_TAS_THICC)
                             : 0;
    sUnreadFiles &= ~destinationMask;
    bcopy(&gSaveBuffer.files[srcFileIndex][0], &gSaveBuffer.files[destFileIndex][0],
          sizeof(gSaveBuffer.files[destFileIndex][0]));
    gSaveBuffer.files[destFileIndex][0].flags &= ~(SAVE_FLAG_HE_THICC_PENDING | SAVE_FLAG_HE_THICC
                                                   | SAVE_FLAG_HE_THICC_INTRO | SAVE_FLAG_TAS_THICC);
    gSaveBuffer.files[destFileIndex][0].flags |= characterFlags;

    mark_save_file_modified(destFileIndex);
    if (!save_file_write_progress(destFileIndex)) {
        // A rejected copy must leave an empty destination available for retry.
        bcopy(previousDestination, &gSaveBuffer.files[destFileIndex], sizeof(previousDestination));
        sUnreadFiles = (sUnreadFiles & ~destinationMask) | previousUnread;
        sModifiedFiles = (sModifiedFiles & ~destinationMask) | previousModified;
        sFailedSaveFiles = (sFailedSaveFiles & ~destinationMask) | previousFailed;
        gSaveFileModified = sModifiedFiles != 0;
        return FALSE;
    }
    player_stats_copy(srcFileIndex, destFileIndex);
    game_timing_copy(srcFileIndex, destFileIndex);
    game_options_copy(srcFileIndex, destFileIndex);
    touch_high_score_ages(destFileIndex);
    save_file_do_save(destFileIndex);
    return TRUE;
}

s32 save_file_is_he_thicc(s32 fileIndex) {
    return fileIndex == SAVE_FILE_C
           && (gSaveBuffer.files[SAVE_FILE_C][0].flags & SAVE_FLAG_HE_THICC) != 0;
}

s32 save_file_he_thicc_pending(void) {
    return (gSaveBuffer.files[SAVE_FILE_C][0].flags & SAVE_FLAG_HE_THICC_PENDING) != 0;
}

s32 save_file_is_tas_thicc(s32 fileIndex) {
    return save_file_is_he_thicc(fileIndex)
           && (gSaveBuffer.files[SAVE_FILE_C][0].flags & SAVE_FLAG_TAS_THICC) != 0;
}

void save_file_unlock_tas_thicc(void) {
    if (game_options_secrets_blocked()) {
        return;
    }
    // Convert C in place: the shortcut never erases stars, coins or other progress.
    gSaveBuffer.files[SAVE_FILE_C][0].flags &= ~(SAVE_FLAG_HE_THICC_PENDING | SAVE_FLAG_HE_THICC_INTRO);
    gSaveBuffer.files[SAVE_FILE_C][0].flags |=
        SAVE_FLAG_FILE_EXISTS | SAVE_FLAG_HE_THICC | SAVE_FLAG_TAS_THICC;
    mark_save_file_modified(SAVE_FILE_C);
    save_file_do_save(SAVE_FILE_C);
}

s32 save_file_he_thicc_intro_pending(s32 fileIndex) {
    return save_file_is_he_thicc(fileIndex)
           && (gSaveBuffer.files[SAVE_FILE_C][0].flags & SAVE_FLAG_HE_THICC_INTRO) != 0;
}

void save_file_arm_he_thicc(void) {
    if (game_options_secrets_blocked()) {
        return;
    }
    if (!save_file_is_he_thicc(SAVE_FILE_C) && !save_file_he_thicc_pending()) {
        gSaveBuffer.files[SAVE_FILE_C][0].flags |= SAVE_FLAG_HE_THICC_PENDING;
        mark_save_file_modified(SAVE_FILE_C);
        save_file_do_save(SAVE_FILE_C);
    }
}

void save_file_start_he_thicc(s32 fileIndex) {
    if (fileIndex == SAVE_FILE_C && save_file_he_thicc_pending()
        && !(gSaveBuffer.files[SAVE_FILE_C][0].flags & SAVE_FLAG_FILE_EXISTS)) {
        gSaveBuffer.files[SAVE_FILE_C][0].flags &= ~SAVE_FLAG_HE_THICC_PENDING;
        gSaveBuffer.files[SAVE_FILE_C][0].flags |=
            SAVE_FLAG_FILE_EXISTS | SAVE_FLAG_HE_THICC | SAVE_FLAG_HE_THICC_INTRO;
        mark_save_file_modified(SAVE_FILE_C);
        save_file_do_save(SAVE_FILE_C);
    }
}

void save_file_finish_he_thicc_intro(void) {
    if (gCurrDemoInput == NULL && gCurrCreditsEntry == NULL
        && save_file_he_thicc_intro_pending(gCurrSaveFileNum - 1)) {
        gSaveBuffer.files[SAVE_FILE_C][0].flags &= ~SAVE_FLAG_HE_THICC_INTRO;
        mark_save_file_modified(SAVE_FILE_C);
        save_file_do_save(SAVE_FILE_C);
    }
}

#ifdef UNIQUE_SAVE_DATA
// This should only be called once on boot and never again.
static void calculate_unique_save_magic(void) {
    u16 checksum = 0;

    for (s32 i = 0; i < 20; i++) {
        checksum += (u16) INTERNAL_ROM_NAME[i] << (i & 0x07);
    }

    MENU_DATA_MAGIC += checksum;
    SAVE_FILE_MAGIC += checksum;
}
#endif

void save_file_load_all(void) {
    s32 file;
    s32 validSlots;

#ifdef UNIQUE_SAVE_DATA
    calculate_unique_save_magic(); // This should only be called once on boot and never again.
#endif

    gMainMenuDataModified = FALSE;
    gSaveFileModified = FALSE;
    sModifiedFiles = 0;
    sFailedSaveFiles = 0;

    bzero(&gSaveBuffer, sizeof(gSaveBuffer));
    sMenuDataUnread = read_sram_data(&gSaveBuffer, sizeof(gSaveBuffer)) != 0;
    sUnreadFiles = sMenuDataUnread ? (1u << NUM_SAVE_FILES) - 1 : 0;
    if (sMenuDataUnread) {
        // Failed reads may have partially filled the buffer with untrusted bytes.
        bzero(&gSaveBuffer, sizeof(gSaveBuffer));
    }
    player_stats_init(player_stats_storage_read, player_stats_storage_write);
    game_timing_init(player_stats_storage_read, player_stats_storage_write, player_stats_retire_legacy);
    game_options_init(player_stats_storage_read, player_stats_storage_write);
    game_options_set_record_invalidator(game_timing_disqualify_file);

    // Verify the main menu data and wipe it if invalid.
    validSlots = verify_save_block_signature(&gSaveBuffer.menuData, sizeof(gSaveBuffer.menuData), MENU_DATA_MAGIC);
    if (!validSlots)
        wipe_main_menu_data();

    for (file = 0; file < NUM_SAVE_FILES; file++) {
        castle_secret_sign_reset(file);
        if (sUnreadFiles & (1u << file)) {
            continue;
        }
        // Verify the save file and create a backup copy if only one of the slots is valid.
        validSlots = verify_save_block_signature(&gSaveBuffer.files[file][0], sizeof(gSaveBuffer.files[file][0]), SAVE_FILE_MAGIC);
        validSlots |= verify_save_block_signature(&gSaveBuffer.files[file][1], sizeof(gSaveBuffer.files[file][1]), SAVE_FILE_MAGIC) << 1;
        switch (validSlots) {
            case 0: // Neither copy is correct; do not retain untrusted unlock bits.
                gSaveBuffer.files[file][0].flags = 0;
                save_file_erase(file);
                break;
            case 1: // Slot 0 is correct and slot 1 is incorrect
                restore_save_file_data(file, 0);
                break;
            case 2: // Slot 1 is correct and slot 0 is incorrect
                restore_save_file_data(file, 1);
                break;
        }
    }
}

/**
 * Reload the current save file from its backup copy, which is effectively a
 * a cached copy of what has been written to SRAM.
 * This is used after getting a game over.
 */
void save_file_reload(void) {
    player_stats_checkpoint(gCurrSaveFileNum - 1);
    castle_secret_sign_reset(gCurrSaveFileNum - 1);
    yoshi_credit_sign_area_exit();
    save_file_flush_castle_secret();
    // Copy save file data from backup
    bcopy(&gSaveBuffer.files[gCurrSaveFileNum - 1][1], &gSaveBuffer.files[gCurrSaveFileNum - 1][0],
          sizeof(gSaveBuffer.files[gCurrSaveFileNum - 1][0]));

    if (!(sFailedSaveFiles & (1u << (gCurrSaveFileNum - 1)))) {
        sModifiedFiles &= ~(1u << (gCurrSaveFileNum - 1));
    }
    gSaveFileModified = sModifiedFiles != 0;
}

/**
 * Update the current save file after collecting a star or a key.
 * If coin score is greater than the current high score, update it.
 */
void save_file_collect_star_or_key(s16 coinScore, s16 starIndex) {
    s32 fileIndex = gCurrSaveFileNum - 1;
    s32 courseIndex = COURSE_NUM_TO_INDEX(gCurrCourseNum);
#ifdef GLOBAL_STAR_IDS
    s32 starByte = COURSE_NUM_TO_INDEX(starIndex / 7);
    s32 starFlag = 1 << (starIndex % 7);
#else
    s32 starFlag = 1 << starIndex;
#endif

    gLastCompletedCourseNum = courseIndex + 1;
    gLastCompletedStarNum = starIndex + 1;
    sUnusedGotGlobalCoinHiScore = FALSE;
    gGotFileCoinHiScore = FALSE;

    if (courseIndex >= COURSE_NUM_TO_INDEX(COURSE_MIN)
        && courseIndex <= COURSE_NUM_TO_INDEX(COURSE_STAGES_MAX)) {
        //! Compares the coin score as a 16 bit value, but only writes the 8 bit
        // truncation. This can allow a high score to decrease.

        if (coinScore > ((u16) save_file_get_max_coin_score(courseIndex) & 0xFFFF)) {
            sUnusedGotGlobalCoinHiScore = TRUE;
        }

        if (coinScore > save_file_get_course_coin_score(fileIndex, courseIndex)) {
            gSaveBuffer.files[fileIndex][0].courseCoinScores[courseIndex] = coinScore;
            touch_coin_score_age(fileIndex, courseIndex);

            gGotFileCoinHiScore = TRUE;
            mark_save_file_modified(fileIndex);
        }
    }

    switch (gCurrLevelNum) {
        case LEVEL_BOWSER_1:
            if (!(save_file_get_flags() & (SAVE_FLAG_HAVE_KEY_1 | SAVE_FLAG_UNLOCKED_BASEMENT_DOOR))) {
                save_file_set_flags(SAVE_FLAG_HAVE_KEY_1);
            }
            break;

        case LEVEL_BOWSER_2:
            if (!(save_file_get_flags() & (SAVE_FLAG_HAVE_KEY_2 | SAVE_FLAG_UNLOCKED_UPSTAIRS_DOOR))) {
                save_file_set_flags(SAVE_FLAG_HAVE_KEY_2);
            }
            break;

        case LEVEL_BOWSER_3:
            break;

        default:
#ifdef GLOBAL_STAR_IDS
            if (!(save_file_get_star_flags(fileIndex, starByte) & starFlag)) {
                save_file_set_star_flags(fileIndex, starByte, starFlag);
            }
#else
            if (!(save_file_get_star_flags(fileIndex, courseIndex) & starFlag)) {
                save_file_set_star_flags(fileIndex, courseIndex, starFlag);
            }
#endif
            break;
    }
}

s32 save_file_exists(s32 fileIndex) {
    return (gSaveBuffer.files[fileIndex][0].flags & SAVE_FLAG_FILE_EXISTS) != 0;
}

/**
 * Get the maximum coin score across all files for a course. The lower 16 bits
 * of the returned value are the score, and the upper 16 bits are the file number
 * of the save file with this score.
 */
u32 save_file_get_max_coin_score(s32 courseIndex) {
    s32 fileIndex;
    s32 maxCoinScore = -1;
    s32 maxScoreAge = -1;
    s32 maxScoreFileNum = 0;

    for (fileIndex = 0; fileIndex < NUM_SAVE_FILES; fileIndex++) {
        if (save_file_get_star_flags(fileIndex, courseIndex) != 0) {
            s32 coinScore = save_file_get_course_coin_score(fileIndex, courseIndex);
            s32 scoreAge = get_coin_score_age(fileIndex, courseIndex);

            if (coinScore > maxCoinScore || (coinScore == maxCoinScore && scoreAge > maxScoreAge)) {
                maxCoinScore = coinScore;
                maxScoreAge = scoreAge;
                maxScoreFileNum = fileIndex + 1;
            }
        }
    }
    return (maxScoreFileNum << 16) + MAX(maxCoinScore, 0);
}

#ifdef COMPLETE_SAVE_FILE
s32 save_file_get_course_star_count(UNUSED s32 fileIndex, UNUSED s32 courseIndex) {
    return 7;
}
#else
s32 save_file_get_course_star_count(s32 fileIndex, s32 courseIndex) {
    s32 i;
    s32 count = 0;
    u8 flag = 1;
    u8 starFlags = save_file_get_star_flags(fileIndex, courseIndex);

    for (i = 0; i < 7; i++, flag <<= 1) {
        if (starFlags & flag) {
            count++;
        }
    }
    return count;
}
#endif

s32 save_file_get_total_star_count(s32 fileIndex, s32 minCourse, s32 maxCourse) {
    s32 count = 0;

    // Get standard course star count.
    for (; minCourse <= maxCourse; minCourse++) {
        count += save_file_get_course_star_count(fileIndex, minCourse);
    }

    // Add castle secret star count.
    return save_file_get_course_star_count(fileIndex, COURSE_NUM_TO_INDEX(COURSE_NONE)) + count;
}

// Read saved star bits directly; debug overrides and ending-course metadata are not earned stars.
s32 save_file_get_saved_star_count(s32 fileIndex) {
    if (fileIndex < 0 || fileIndex >= NUM_SAVE_FILES) {
        return 0;
    }
    struct SaveFile *file = &gSaveBuffer.files[fileIndex][0];
    u32 stars = SAVE_FLAG_TO_STAR_FLAG(file->flags) & 0x1F; // Three Toad stars and two MIPS stars.
    s32 count = 0;
    for (s32 course = COURSE_NONE; course <= COURSE_MAX; course++) {
        if (course == COURSE_CAKE_END) {
            continue;
        }
        if (course != COURSE_NONE) {
            stars = file->courseStars[COURSE_NUM_TO_INDEX(course)] & 0x7F;
        }
        for (; stars != 0; stars >>= 1) {
            count += stars & 1;
        }
    }
    return count;
}

s32 save_file_get_castle_lobby_progress(s32 fileIndex) {
    if (fileIndex < 0 || fileIndex >= NUM_SAVE_FILES) {
        return 0;
    }
    s32 progress = gSaveBuffer.files[fileIndex][0].courseStars[COURSE_NUM_TO_INDEX(COURSE_CAKE_END)]
                   & SAVE_CAKE_CASTLE_LOBBY_PROGRESS_MASK;
    return progress <= CASTLE_LOBBY_CONVERSATION_COUNT ? progress : 0;
}

void save_file_set_castle_lobby_progress(s32 fileIndex, s32 progress) {
    if (fileIndex < 0 || fileIndex >= NUM_SAVE_FILES || gCurrDemoInput != NULL
        || gCurrCreditsEntry != NULL || progress <= save_file_get_castle_lobby_progress(fileIndex)
        || progress > CASTLE_LOBBY_CONVERSATION_COUNT) {
        return;
    }
    u8 *ending = &gSaveBuffer.files[fileIndex][0].courseStars[COURSE_NUM_TO_INDEX(COURSE_CAKE_END)];
    *ending = (*ending & ~SAVE_CAKE_CASTLE_LOBBY_PROGRESS_MASK) | progress;
    mark_save_file_modified(fileIndex);
    save_file_do_save(fileIndex);
}

void save_file_set_flags(u32 flags) {
    gSaveBuffer.files[gCurrSaveFileNum - 1][0].flags |= (flags | SAVE_FLAG_FILE_EXISTS);
    mark_save_file_modified(gCurrSaveFileNum - 1);
}

s32 save_file_has_castle_wing_cap(s32 fileIndex) {
    return fileIndex >= 0 && fileIndex < NUM_SAVE_FILES
           && (gSaveBuffer.files[fileIndex][0].courseStars[COURSE_NUM_TO_INDEX(COURSE_CAKE_END)]
               & SAVE_CAKE_CASTLE_WING_CAP_UNLOCKED)
                  != 0;
}

void save_file_unlock_castle_wing_cap(s32 fileIndex) {
    if (game_options_secrets_blocked()) {
        return;
    }
    if (fileIndex < 0 || fileIndex >= NUM_SAVE_FILES || gCurrDemoInput != NULL
        || gCurrCreditsEntry != NULL || save_file_has_castle_wing_cap(fileIndex)) {
        return;
    }
    save_file_set_star_flags(fileIndex, COURSE_NUM_TO_INDEX(COURSE_CAKE_END),
                             SAVE_CAKE_CASTLE_WING_CAP_UNLOCKED);
    sCastleSecretSavePending |= 1u << fileIndex;
}

struct CastleSecretSaveBit {
    s8 course;
    u8 mask;
};

// Cannons use the byte AFTER their course's star byte. Keep those bits untouched.
static const struct CastleSecretSaveBit sCastleSecretSaveBits[] = {
    { COURSE_CAKE_END, SAVE_CAKE_CASTLE_SECRET_MOVED },
    { COURSE_WMOTR, 0x80 },
    { COURSE_COTMC, 0x80 },
    { COURSE_VCUTM, 0x80 },
    { COURSE_TOTWC, 0x80 },
    { COURSE_PSS, 0x80 },
    { COURSE_BITFS, 0x80 },
    { COURSE_CAKE_END, SAVE_CAKE_CASTLE_LOBBY_SIGN_READ },
    { COURSE_BITS, 0x80 },
};

u32 save_file_get_castle_secret(s32 fileIndex) {
    u32 flags = 0;
    if (fileIndex < 0 || fileIndex >= NUM_SAVE_FILES) {
        return 0;
    }
    for (s32 bit = 0; bit < ARRAY_COUNT(sCastleSecretSaveBits); bit++) {
        if (gSaveBuffer.files[fileIndex][0]
                .courseStars[COURSE_NUM_TO_INDEX(sCastleSecretSaveBits[bit].course)]
            & sCastleSecretSaveBits[bit].mask) {
            flags |= 1 << bit;
        }
    }
    // Legacy cannon bits cannot prove a secret was completed. Preserve rooftop
    // progress using its independent Wing unlock or relocated-sign introduction.
    // The ambiguous old floor-sign bit is ignored, allowing that sign to be read.
    if (save_file_has_castle_wing_cap(fileIndex) || (flags & CASTLE_SECRET_INTRO_READ)) {
        flags |= CASTLE_SECRET_MOVED;
    }
    // Ignore the premature departure bit written by earlier encounter saves.
    if (!(flags & CASTLE_SECRET_INTRO_READ)) {
        flags &= ~CASTLE_SECRET_AWAY;
    }
    return flags;
}

void save_file_set_castle_secret(s32 fileIndex, u32 flags) {
    if (game_options_secrets_blocked()) {
        return;
    }
    if (fileIndex < 0 || fileIndex >= NUM_SAVE_FILES || gCurrDemoInput != NULL
        || gCurrCreditsEntry != NULL) {
        return;
    }
    u32 previous = save_file_get_castle_secret(fileIndex);
    if ((previous & flags) == flags) {
        return;
    }
    if ((flags & CASTLE_SECRET_INTRO_READ) && !(previous & CASTLE_SECRET_INTRO_READ)) {
        // Completing the introduction starts a fresh departure gate on older saves.
        gSaveBuffer.files[fileIndex][0]
            .courseStars[COURSE_NUM_TO_INDEX(sCastleSecretSaveBits[1].course)] &=
            ~sCastleSecretSaveBits[1].mask;
    }
    for (s32 bit = 0; bit < ARRAY_COUNT(sCastleSecretSaveBits); bit++) {
        if (flags & (1 << bit)) {
            save_file_set_star_flags(fileIndex, COURSE_NUM_TO_INDEX(sCastleSecretSaveBits[bit].course),
                                     sCastleSecretSaveBits[bit].mask);
        }
    }
    sCastleSecretSavePending |= 1u << fileIndex;
}

// Flush only at a level boundary or before Game Over restores the saved backup.
void save_file_flush_castle_secret(void) {
    for (s32 fileIndex = 0; fileIndex < NUM_SAVE_FILES; fileIndex++) {
        if (sCastleSecretSavePending & (1u << fileIndex)) {
            mark_save_file_modified(fileIndex);
            save_file_do_save(fileIndex);
        }
    }
}

void save_file_clear_flags(u32 flags) {
    gSaveBuffer.files[gCurrSaveFileNum - 1][0].flags &= ~flags;
    gSaveBuffer.files[gCurrSaveFileNum - 1][0].flags |= SAVE_FLAG_FILE_EXISTS;
    mark_save_file_modified(gCurrSaveFileNum - 1);
}

u32 save_file_get_flags(void) {
#ifdef COMPLETE_SAVE_FILE
    return (SAVE_FLAG_FILE_EXISTS | SAVE_FLAG_HAVE_WING_CAP | SAVE_FLAG_HAVE_METAL_CAP
            | SAVE_FLAG_HAVE_VANISH_CAP | SAVE_FLAG_UNLOCKED_BASEMENT_DOOR
            | SAVE_FLAG_UNLOCKED_UPSTAIRS_DOOR | SAVE_FLAG_DDD_MOVED_BACK | SAVE_FLAG_MOAT_DRAINED
            | SAVE_FLAG_UNLOCKED_PSS_DOOR | SAVE_FLAG_UNLOCKED_WF_DOOR | SAVE_FLAG_UNLOCKED_CCM_DOOR
            | SAVE_FLAG_UNLOCKED_JRB_DOOR | SAVE_FLAG_UNLOCKED_BITDW_DOOR
            | SAVE_FLAG_UNLOCKED_BITFS_DOOR | SAVE_FLAG_UNLOCKED_50_STAR_DOOR
            | SAVE_FLAG_COLLECTED_TOAD_STAR_1 | SAVE_FLAG_COLLECTED_TOAD_STAR_2
            | SAVE_FLAG_COLLECTED_TOAD_STAR_3 | SAVE_FLAG_COLLECTED_MIPS_STAR_1
            | SAVE_FLAG_COLLECTED_MIPS_STAR_2);
#else
    if (gCurrCreditsEntry != NULL || gCurrDemoInput != NULL) {
        return 0;
    }
    return gSaveBuffer.files[gCurrSaveFileNum - 1][0].flags;
#endif
}

/**
 * Return the bitset of obtained stars in the specified course.
 * If course is COURSE_NONE, return the bitset of obtained castle secret stars.
 */
#ifdef COMPLETE_SAVE_FILE
u32 save_file_get_star_flags(UNUSED s32 fileIndex, UNUSED s32 courseIndex) {
    return 0x7F;
}
#else
u32 save_file_get_star_flags(s32 fileIndex, s32 courseIndex) {
    u32 starFlags;

    if (courseIndex == COURSE_NUM_TO_INDEX(COURSE_CAKE_END)) {
        starFlags = 0; // This unused course byte holds encounter metadata, not stars.
    } else if (courseIndex == COURSE_NUM_TO_INDEX(COURSE_NONE)) {
        starFlags = SAVE_FLAG_TO_STAR_FLAG(gSaveBuffer.files[fileIndex][0].flags);
    } else {
        starFlags = gSaveBuffer.files[fileIndex][0].courseStars[courseIndex] & 0x7F;
    }

    return starFlags;
}
#endif

/**
 * Add to the bitset of obtained stars in the specified course.
 * If course is COURSE_NONE, add to the bitset of obtained castle secret stars.
 */
void save_file_set_star_flags(s32 fileIndex, s32 courseIndex, u32 starFlags) {
    if (courseIndex == COURSE_NUM_TO_INDEX(COURSE_NONE)) {
        gSaveBuffer.files[fileIndex][0].flags |= STAR_FLAG_TO_SAVE_FLAG(starFlags);
    } else {
        gSaveBuffer.files[fileIndex][0].courseStars[courseIndex] |= starFlags;
    }

    gSaveBuffer.files[fileIndex][0].flags |= SAVE_FLAG_FILE_EXISTS;
    mark_save_file_modified(fileIndex);
}

#ifdef COMPLETE_SAVE_FILE
s32 save_file_get_course_coin_score(UNUSED s32 fileIndex, UNUSED s32 courseIndex) {
    return MAX_NUM_COINS;
}
#else
s32 save_file_get_course_coin_score(s32 fileIndex, s32 courseIndex) {
    return gSaveBuffer.files[fileIndex][0].courseCoinScores[courseIndex];
}
#endif

/**
 * Return TRUE if the cannon is unlocked in the current course.
 */
s32 save_file_is_cannon_unlocked(void) {
#ifdef UNLOCK_ALL
    return TRUE;
#else
    return (gSaveBuffer.files[gCurrSaveFileNum - 1][0].courseStars[gCurrCourseNum] & COURSE_FLAG_CANNON_UNLOCKED) != 0;
#endif
}

/**
 * Sets the cannon status to unlocked in the current course.
 */
void save_file_set_cannon_unlocked(void) {
    gSaveBuffer.files[gCurrSaveFileNum - 1][0].courseStars[gCurrCourseNum] |=
        COURSE_FLAG_CANNON_UNLOCKED;
    gSaveBuffer.files[gCurrSaveFileNum - 1][0].flags |= SAVE_FLAG_FILE_EXISTS;
    mark_save_file_modified(gCurrSaveFileNum - 1);
}

void save_file_set_cap_pos(s16 x, s16 y, s16 z) {
    struct SaveFile *saveFile = &gSaveBuffer.files[gCurrSaveFileNum - 1][0];

    saveFile->capLevel =
        (saveFile->capLevel & SAVE_CAP_LEVEL_METADATA) | (gCurrLevelNum & SAVE_CAP_LEVEL_MASK);
    saveFile->capArea =
        (saveFile->capArea & ~SAVE_CAP_AREA_MASK) | (gCurrAreaIndex & SAVE_CAP_AREA_MASK);
    vec3s_set(saveFile->capPos, x, y, z);
    save_file_set_flags(SAVE_FLAG_CAP_ON_GROUND);
}

s32 save_file_get_cap_pos(Vec3s capPos) {
    struct SaveFile *saveFile = &gSaveBuffer.files[gCurrSaveFileNum - 1][0];
    s32 flags = save_file_get_flags();

    if ((saveFile->capLevel & SAVE_CAP_LEVEL_MASK) == gCurrLevelNum
        && (saveFile->capArea & SAVE_CAP_AREA_MASK) == gCurrAreaIndex
        && (flags & SAVE_FLAG_CAP_ON_GROUND)) {
        vec3s_copy(capPos, saveFile->capPos);
        return TRUE;
    }
    return FALSE;
}

void save_file_set_sound_mode(u16 mode) {
    set_sound_mode(mode);
    gSaveBuffer.menuData.soundMode = mode;

    gMainMenuDataModified = TRUE;
    save_main_menu_data();
}

#ifdef WIDE
u32 save_file_get_widescreen_mode(void) {
    return gSaveBuffer.menuData.wideMode;
}

void save_file_set_widescreen_mode(u8 mode) {
    gSaveBuffer.menuData.wideMode = mode;

    gMainMenuDataModified = TRUE;
    save_main_menu_data();
}
#endif

u32 save_file_get_sound_mode(void) {
    if (gSaveBuffer.menuData.soundMode >= SOUND_MODE_COUNT) {
        return 0;
    }

    return gSaveBuffer.menuData.soundMode;
}

#ifdef REONUCAM
void save_file_set_camera_speed(u8 speed) {
    s32 fileIndex = gCurrSaveFileNum - 1;
    if (fileIndex < 0 || fileIndex >= NUM_SAVE_FILES) {
        return;
    }
    if (speed >= REONUCAM_SPEED_COUNT) {
        speed = REONUCAM_DEFAULT_SPEED;
    }
    struct SaveFile *saveFile = &gSaveBuffer.files[fileIndex][0];
    // Zero means unset, so legacy and erased files use Medium.
    u8 encoded = (speed + 1) << SAVE_CAP_AREA_CAMERA_SPEED_SHIFT;
    game_options_set_saved_camera_speed(fileIndex, speed);
    if ((saveFile->capArea & SAVE_CAP_AREA_CAMERA_SPEED_MASK) == encoded) {
        game_options_save(fileIndex);
        return;
    }
    saveFile->capArea = (saveFile->capArea & ~SAVE_CAP_AREA_CAMERA_SPEED_MASK) | encoded;
    mark_save_file_modified(fileIndex);
    save_file_do_save(fileIndex);
}

u8 save_file_get_camera_speed(void) {
    s32 fileIndex = gCurrSaveFileNum - 1;
    if (fileIndex < 0 || fileIndex >= NUM_SAVE_FILES) {
        return REONUCAM_DEFAULT_SPEED;
    }
    u8 encoded = (gSaveBuffer.files[fileIndex][0].capArea & SAVE_CAP_AREA_CAMERA_SPEED_MASK)
                 >> SAVE_CAP_AREA_CAMERA_SPEED_SHIFT;
    return encoded > 0 && encoded <= REONUCAM_SPEED_COUNT ? encoded - 1 : REONUCAM_DEFAULT_SPEED;
}

#endif

#if MULTILANG
void eu_set_language(u16 language) {
    gSaveBuffer.menuData.language = language;
    gMainMenuDataModified = TRUE;
    save_main_menu_data();
}

u32 eu_get_language(void) {
    return gSaveBuffer.menuData.language;
}
#endif

void disable_warp_checkpoint(void) {
    // check_warp_checkpoint() checks to see if gWarpCheckpoint.courseNum != COURSE_NONE
    gWarpCheckpoint.courseNum = COURSE_NONE;
}

/**
 * Checks the upper bit of the WarpNode->destLevel byte to see if the
 * game should set a warp checkpoint.
 */
void check_if_should_set_warp_checkpoint(struct WarpNode *warpNode) {
    if (warpNode->destLevel & WARP_CHECKPOINT) {
        // Overwrite the warp checkpoint variables.
        gWarpCheckpoint.actNum = gCurrActNum;
        gWarpCheckpoint.courseNum = gCurrCourseNum;
        gWarpCheckpoint.levelID = warpNode->destLevel & 0x7F;
        gWarpCheckpoint.areaNum = warpNode->destArea;
        gWarpCheckpoint.warpNode = warpNode->destNode;
    }
}

/**
 * Checks to see if a checkpoint is properly active or not. This will
 * also update the level, area, and destination node of the input WarpNode.
 * returns TRUE if input WarpNode was updated, and FALSE if not.
 */
s32 check_warp_checkpoint(struct WarpNode *warpNode) {
    s16 warpCheckpointActive = FALSE;
    s16 currCourseNum = gLevelToCourseNumTable[(warpNode->destLevel & 0x7F) - 1];

    // gSavedCourseNum is only used in this function.
    if (gWarpCheckpoint.courseNum != COURSE_NONE && gSavedCourseNum == currCourseNum
        && gWarpCheckpoint.actNum == gCurrActNum) {
        warpNode->destLevel = gWarpCheckpoint.levelID;
        warpNode->destArea = gWarpCheckpoint.areaNum;
        warpNode->destNode = gWarpCheckpoint.warpNode;
        warpCheckpointActive = TRUE;
    } else {
        // Disable the warp checkpoint just in case the other 2 conditions failed?
        gWarpCheckpoint.courseNum = COURSE_NONE;
    }

    return warpCheckpointActive;
}
