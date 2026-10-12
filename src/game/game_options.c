#include "game_options.h"

#include <stddef.h>
#include <string.h>

#define OPTIONS_MAGIC 0x4F505431u
#define OPTIONS_VERSION 3
#define OPTIONS_VALUE_CAPACITY 96
#ifdef VERSION_EU
#define OPTIONS_CONFIRM_FRAMES 125
#else
#define OPTIONS_CONFIRM_FRAMES 150
#endif

const struct GameOptionDefinition gGameOptionDefinitions[GO_COUNT] = {
#define OPTION_DEF(id, category, kind, label, minimum, maximum, step, defaultValue)                    \
    { label, GO_##category, GO_##kind, minimum, maximum, step, defaultValue },
    GAME_OPTION_CATALOG(OPTION_DEF)
#undef OPTION_DEF
};

struct OptionsRecord {
    u32 magic;
    u32 version;
    u32 generation;
    u32 file;
    u32 count;
    u8 values[OPTIONS_VALUE_CAPACITY];
    u32 checksum;
    u32 reserved[2];
} __attribute__((aligned(16)));

_Static_assert(sizeof(struct OptionsRecord) <= GAME_OPTIONS_SAVE_STRIDE,
               "Options exceed reserved SRAM bank");
_Static_assert(GO_COUNT <= OPTIONS_VALUE_CAPACITY, "Options exceed stable payload capacity");
_Static_assert(sizeof(struct OptionsRecord) % 16 == 0, "Options DMA size must be aligned");
_Static_assert(GAME_OPTIONS_SRAM_OFFSET + GAME_OPTIONS_FILES * 2 * GAME_OPTIONS_SAVE_STRIDE <= 0x1000,
               "Options overlap legacy statistics");

static u8 sValues[GAME_OPTIONS_FILES][GO_COUNT];
static u8 sDirty[GAME_OPTIONS_FILES];
// Each set bit identifies a bank still needing replacement before the file is clean.
static u8 sReplaceBanks[GAME_OPTIONS_FILES];
// Unread or newer preferences block writes until reload or explicit copy/erase.
static u8 sLoadFailed[GAME_OPTIONS_FILES];
static void (*sRecordInvalidator)(s32 file);
static s8 sBank[GAME_OPTIONS_FILES];
static u32 sGeneration[GAME_OPTIONS_FILES];
static s32 sActiveFile = -1;
static u32 sFileSelectionSerial;
// Restart-required preferences stay frozen for the current level visit.
static s32 sLevelSnapshotFile = -1;
static u8 sLevelSnapshotValues[GO_COUNT];
static u8 sVisitCheated;
static u8 sConnectedControllers;
static u8 sLowRumblePhase;
static GameOptionsRead sRead;
static GameOptionsWrite sWrite;
static struct OptionsRecord sScratch[2] __attribute__((aligned(16)));
static struct {
    s32 active;
    s32 file;
    enum GameOptionId id;
    u32 frame;
    u32 heldFrames;
    s32 armed;
    s32 sampled;
} sConfirmation;

u8 game_options_menu_direction(u8 directions, u8 *held) {
    // Conflicting holds cannot become a fresh choice until all directions are released.
    if (!directions) {
        *held = 0;
        return 0;
    }
    if ((*held & GO_MENU_WAIT_FOR_NEUTRAL) || (directions & (directions - 1))
        || (game_options_get(GO_MENU_STICK_RELEASE) && *held)) {
        *held = GO_MENU_WAIT_FOR_NEUTRAL | directions;
        return 0;
    }
    *held = directions;
    return directions;
}

u8 game_options_menu_stick(u8 directions, u8 *held) {
    u8 previous = *held;
    directions = game_options_menu_direction(directions, held);
    if (game_options_get(GO_MENU_NAVIGATION) == 1
        || (game_options_get(GO_MENU_STICK_RELEASE) && previous)) {
        return 0;
    }
    return directions;
}

u16 game_options_menu_dpad(u16 buttons) {
    return game_options_get(GO_MENU_NAVIGATION) == 0 ? 0 : buttons;
}

f32 game_options_shake_scale(void) {
    return game_options_get(GO_CAMERA_SHAKE) ? 0.0f : 1.0f;
}

s32 game_options_hud_inset(void) {
    return game_options_get(GO_ASPECT_RATIO) && !game_options_get(GO_HUD_WIDE) ? 40 : 0;
}

static s32 valid_file(s32 file) {
    return file >= 0 && file < GAME_OPTIONS_FILES;
}

static s32 valid_id(enum GameOptionId id) {
    return id >= 0 && id < GO_COUNT;
}

static u32 record_offset(s32 file, s32 bank) {
    return GAME_OPTIONS_SRAM_OFFSET + (file * 2 + bank) * GAME_OPTIONS_SAVE_STRIDE;
}

static u32 record_checksum(const struct OptionsRecord *record) {
    const u8 *bytes = (const u8 *) record;
    u32 checksum = 2166136261u;
    for (u32 i = 0; i < offsetof(struct OptionsRecord, checksum); i++) {
        checksum = (checksum ^ bytes[i]) * 16777619u;
    }
    return checksum;
}

// Version 1 stored a separate enable toggle and frame count for each filter.
static u8 legacy_filter_duration(const struct OptionsRecord *record, enum GameOptionId toggle,
                                 enum GameOptionId duration, u8 fallback) {
    if (record->count <= (u32) toggle || !record->values[toggle]) {
        return 0;
    }
    if (record->count <= (u32) duration) {
        return fallback;
    }
    return record->values[duration] ? record->values[duration] : 1;
}

static s32 valid_record(const struct OptionsRecord *record, s32 file) {
    return record->magic == OPTIONS_MAGIC
           && (record->version >= 1 && record->version <= OPTIONS_VERSION) && record->file == (u32) file
           && record->count > 0 && record->count <= GO_COUNT
           && record->checksum == record_checksum(record);
}

static void default_values(u8 *values) {
    for (s32 id = 0; id < GO_COUNT; id++) {
        values[id] = gGameOptionDefinitions[id].defaultValue;
    }
}

// Restore a validated payload, including defaults for IDs absent from older catalogs.
// Range normalization happens after selecting the newest valid bank.
static void restore_record_values(u8 *values, const struct OptionsRecord *record) {
    default_values(values);
    memcpy(values, record->values, record->count);
    if (record->version == 1) {
        values[GO_CAMERA_SHAKE] =
            record->count > GO_CAMERA_SHAKE && record->values[GO_CAMERA_SHAKE] == 2;
        values[GO_DEBOUNCE] = legacy_filter_duration(record, GO_DEBOUNCE, GO_DEBOUNCE_FRAMES, 2);
        values[GO_MIN_HOLD] = legacy_filter_duration(record, GO_MIN_HOLD, GO_MIN_HOLD_FRAMES, 1);
    }
    if (record->version < OPTIONS_VERSION && record->count > GO_KNOCKBACK) {
        // Older saves stored normal knockback On instead of disabling it.
        values[GO_KNOCKBACK] = record->values[GO_KNOCKBACK] == 0;
    }
}

static void normalize_values(u8 *values) {
    for (s32 id = 0; id < GO_COUNT; id++) {
        const struct GameOptionDefinition *definition = &gGameOptionDefinitions[id];
        if (values[id] < definition->minimum || values[id] > definition->maximum
            || (values[id] - definition->minimum) % definition->step != 0) {
            values[id] = definition->defaultValue;
        }
    }
    // Cheats consent is session-owned; debug and individual preferences remain saved.
    values[GO_CHEATS_ENABLED] = 0;
}

void game_options_confirmation_cancel(void) {
    memset(&sConfirmation, 0, sizeof(sConfirmation));
}

u32 game_options_confirmation_progress(void) {
    return sConfirmation.heldFrames ? sConfirmation.heldFrames - 1 : 0;
}

u32 game_options_confirmation_duration(void) {
    return OPTIONS_CONFIRM_FRAMES;
}

void game_options_set_record_invalidator(void (*invalidate)(s32 file)) {
    sRecordInvalidator = invalidate;
}

void game_options_init(GameOptionsRead read, GameOptionsWrite write) {
    sRead = read;
    sWrite = write;
    sActiveFile = -1;
    sFileSelectionSerial++;
    sRecordInvalidator = NULL;
    sLevelSnapshotFile = -1;
    sVisitCheated = 0;
    sConnectedControllers = 0;
    sLowRumblePhase = 0;
    game_options_confirmation_cancel();
    for (s32 file = 0; file < GAME_OPTIONS_FILES; file++) {
        s32 migrated = FALSE;
        default_values(sValues[file]);
        sBank[file] = -1;
        sGeneration[file] = 0;
        sReplaceBanks[file] = 3;
        sLoadFailed[file] = 0;
        for (s32 bank = 0; bank < 2; bank++) {
            struct OptionsRecord *record = &sScratch[bank];
            memset(record, 0, sizeof(*record));
            if (sRead == NULL) {
                continue;
            }
            if (sRead(record_offset(file, bank), record, sizeof(*record)) != 0) {
                sLoadFailed[file] = 1;
                continue;
            }
            if (record->magic == OPTIONS_MAGIC && record->file == (u32) file
                && (record->version > OPTIONS_VERSION || record->count > GO_COUNT)) {
                sLoadFailed[file] = 1;
                continue;
            }
            if (!valid_record(record, file)) {
                continue;
            }
            sReplaceBanks[file] &= ~(1 << bank);
            if (sBank[file] < 0 || (s32) (record->generation - sGeneration[file]) > 0) {
                restore_record_values(sValues[file], record);
                migrated = record->version < OPTIONS_VERSION;
                sBank[file] = bank;
                sGeneration[file] = record->generation;
            }
        }
        u8 loaded[GO_COUNT];
        memcpy(loaded, sValues[file], GO_COUNT);
        normalize_values(sValues[file]);
        sDirty[file] =
            migrated || sReplaceBanks[file] != 0 || memcmp(loaded, sValues[file], GO_COUNT) != 0;
    }
}

void game_options_select_file(s32 file) {
    file = valid_file(file) ? file : -1;
    if (file == sActiveFile) {
        return;
    }
    if (valid_file(sActiveFile)) {
        sValues[sActiveFile][GO_CHEATS_ENABLED] = 0;
    }
    sActiveFile = file;
    sFileSelectionSerial++;
    sLevelSnapshotFile = -1;
    sVisitCheated = 0;
    sLowRumblePhase = 0;
    game_options_confirmation_cancel();
}

s32 game_options_active_file(void) {
    return sActiveFile;
}

u32 game_options_file_selection_serial(void) {
    return sFileSelectionSerial;
}

s32 game_options_has_record(s32 file) {
    return valid_file(file) && sBank[file] >= 0;
}

// Bridge the existing per-file camera save API, including before a file is active.
void game_options_set_saved_camera_speed(s32 file, u8 speed) {
    if (valid_file(file) && speed < 5 && sValues[file][GO_CAMERA_SPEED] != speed) {
        sValues[file][GO_CAMERA_SPEED] = speed;
        sDirty[file] = 1;
    }
}

f32 game_options_camera_speed(void) {
    static const u8 halfSteps[] = { 4, 6, 8, 12, 16 };
    u8 preset = game_options_get(GO_CAMERA_SPEED);
    return (preset < 5 ? halfSteps[preset] : game_options_get(GO_CAMERA_CUSTOM)) * 0.5f;
}

u16 game_options_swap_buttons(u16 buttons, u16 first, u16 second) {
    u16 result = buttons & ~(first | second);
    if (buttons & first) {
        result |= second;
    }
    if (buttons & second) {
        result |= first;
    }
    return result;
}

s32 game_options_rumble_motor(s32 requested) {
    u8 setting = game_options_get(GO_RUMBLE);
    if (setting != 1) {
        sLowRumblePhase = 0;
    }
    if (!requested || setting == 0) {
        return 0;
    }
    // Count requested pulses, rather than even/odd VIs: otherwise sparse pulses
    // aligned to one VI parity could remain full strength or disappear entirely.
    if (setting == 1) {
        sLowRumblePhase ^= 1;
        return sLowRumblePhase;
    }
    return 1;
}

u8 game_options_get_file(s32 file, enum GameOptionId id) {
    if (!valid_id(id)) {
        return 0;
    }
    return valid_file(file) ? sValues[file][id] : gGameOptionDefinitions[id].defaultValue;
}

s32 game_options_requires_restart(enum GameOptionId id) {
    return id == GO_BOWSER_FIGHTS || id == GO_SWITCHES || id == GO_WING_CAP || id == GO_METAL_CAP
           || id == GO_VANISH_CAP;
}

void game_options_restart(void) {
    sLevelSnapshotFile = sActiveFile;
    sVisitCheated = 0;
    if (valid_file(sActiveFile)) {
        memcpy(sLevelSnapshotValues, sValues[sActiveFile], GO_COUNT);
    }
}

s32 game_options_visit_cheating(s32 current) {
    if (valid_file(sActiveFile) && sLevelSnapshotFile == sActiveFile) {
        sVisitCheated |= current != 0;
        return sVisitCheated;
    }
    return current;
}

// Both dormant cheat preferences and permission grants disqualify the visit immediately.
static void mark_active_visit_cheated(void) {
    game_options_visit_cheating(1);
    if (sRecordInvalidator) {
        sRecordInvalidator(sActiveFile);
    }
}

u8 game_options_get(enum GameOptionId id) {
    if (valid_file(sActiveFile) && sLevelSnapshotFile == sActiveFile
        && game_options_requires_restart(id)) {
        return sLevelSnapshotValues[id];
    }
    return game_options_get_file(sActiveFile, id);
}

void game_options_set_connected_controllers(u8 bits) {
    sConnectedControllers = bits & 0x0F;
}

s32 game_options_controller_available(s32 value) {
    return value == 0 || (value >= 1 && value <= 4 && (sConnectedControllers & (1 << (value - 1))));
}

s32 game_options_set(enum GameOptionId id, s32 value) {
    if (!valid_file(sActiveFile) || !valid_id(id)) {
        return 0;
    }
    const struct GameOptionDefinition *definition = &gGameOptionDefinitions[id];
    if (definition->kind == GO_ACTION || value < definition->minimum || value > definition->maximum
        || (value - definition->minimum) % definition->step != 0
        || (id == GO_CONTROLLER && !game_options_controller_available(value))
        || ((id == GO_CHEATS_ENABLED || id == GO_DEBUG_MODE) && value != 0)) {
        return 0;
    }
    u8 *values = sValues[sActiveFile];
    if (values[id] == value) {
        return 1;
    }
    values[id] = value;
    if (definition->category == GO_CHEATS && value != 0) {
        mark_active_visit_cheated();
    }
    if (id == GO_INPUT_DISPLAY && value && values[GO_CONTROLLER_DISPLAY] == 0) {
        values[GO_CONTROLLER_DISPLAY] = 1;
    }
    sDirty[sActiveFile] = 1;
    return 1;
}

s32 game_options_cheat(enum GameOptionId id) {
    return valid_file(sActiveFile) && game_options_get(GO_CHEATS_ENABLED) && valid_id(id)
           && gGameOptionDefinitions[id].category == GO_CHEATS && id != GO_CHEATS_ENABLED
           && id != GO_DEBUG_MODE && game_options_get(id);
}

s32 game_options_save(s32 file) {
    struct OptionsRecord *record = &sScratch[0];
    struct OptionsRecord *verify = &sScratch[1];
    if (!valid_file(file)) {
        return 0;
    }
    if (!sDirty[file]) {
        return 1;
    }
    // Failed reads and newer formats must not become implicit default writes.
    // Only a successful supported reload or explicit copy/erase permits replacement.
    if (sRead == NULL || sWrite == NULL || sLoadFailed[file]) {
        return 0;
    }
    do {
        s32 bank = sBank[file] == 0 ? 1 : 0;
        memset(record, 0, sizeof(*record));
        record->magic = OPTIONS_MAGIC;
        record->version = OPTIONS_VERSION;
        record->generation = sGeneration[file] + 1;
        record->file = file;
        record->count = GO_COUNT;
        memcpy(record->values, sValues[file], GO_COUNT);
        normalize_values(record->values);
        record->checksum = record_checksum(record);
        if (sWrite(record_offset(file, bank), record, sizeof(*record)) != 0
            || sRead(record_offset(file, bank), verify, sizeof(*verify)) != 0
            || memcmp(record, verify, sizeof(*record)) != 0) {
            return 0;
        }
        sBank[file] = bank;
        sGeneration[file] = record->generation;
        sReplaceBanks[file] &= ~(1 << bank);
    } while (sReplaceBanks[file] != 0);
    sDirty[file] = 0;
    return 1;
}

void game_options_erase(s32 file) {
    if (!valid_file(file)) {
        return;
    }
    default_values(sValues[file]);
    sDirty[file] = 1;
    sReplaceBanks[file] = 3;
    sLoadFailed[file] = 0;
    if (file == sActiveFile) {
        game_options_confirmation_cancel();
    }
}

s32 game_options_can_copy(s32 source) {
    return valid_file(source) && sRead != NULL && !sLoadFailed[source];
}

void game_options_copy(s32 source, s32 destination) {
    if (!game_options_can_copy(source) || !valid_file(destination) || source == destination) {
        return;
    }
    memcpy(sValues[destination], sValues[source], GO_COUNT);
    normalize_values(sValues[destination]);
    sDirty[destination] = 1;
    sReplaceBanks[destination] = 3;
    sLoadFailed[destination] = 0;
    if (destination == sActiveFile) {
        game_options_confirmation_cancel();
    }
}

s32 game_options_dirty(s32 file) {
    return valid_file(file) && sDirty[file];
}

s32 game_options_confirmation_begin(enum GameOptionId id) {
    game_options_confirmation_cancel();
    if (!valid_file(sActiveFile) || (id != GO_CHEATS_ENABLED && id != GO_DEBUG_MODE)) {
        return 0;
    }
    sConfirmation.active = 1;
    sConfirmation.file = sActiveFile;
    sConfirmation.id = id;
    return 1;
}

s32 game_options_confirmation_update(u32 frame, s32 held, s32 cancel) {
    if (!sConfirmation.active) {
        return 0;
    }
    if (cancel || sConfirmation.file != sActiveFile) {
        game_options_confirmation_cancel();
        return 0;
    }
    if (sConfirmation.sampled && frame == sConfirmation.frame) {
        return 0;
    }
    if (sConfirmation.sampled && (u32) (frame - sConfirmation.frame) != 1) {
        sConfirmation.heldFrames = 0;
        sConfirmation.armed = 0;
    }
    sConfirmation.frame = frame;
    sConfirmation.sampled = 1;
    if (!held) {
        sConfirmation.heldFrames = 0;
        sConfirmation.armed = 1;
    } else if (sConfirmation.armed && ++sConfirmation.heldFrames > OPTIONS_CONFIRM_FRAMES) {
        sValues[sActiveFile][sConfirmation.id] = 1;
        mark_active_visit_cheated();
        sDirty[sActiveFile] = 1;
        game_options_confirmation_cancel();
        return 1;
    }
    return 0;
}
