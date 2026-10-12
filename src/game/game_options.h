#ifndef GAME_OPTIONS_H
#define GAME_OPTIONS_H

#include <PR/ultratypes.h>

#define GAME_OPTIONS_FILES 4
#define GAME_OPTIONS_SRAM_OFFSET 0x800
#define GAME_OPTIONS_SAVE_STRIDE 0x100
#define GAME_OPTIONS_VERSION_TEXT "0.1.0"

// Append IDs only: saved payload bytes use this permanent order.
// Removed settings retain a reserved slot with a NULL label.
#define GAME_OPTION_CATALOG(X)                                                                         \
    X(PAUSE_DISCONNECT, CONTROLS, CHOICE, "Pause when controller disconnects", 0, 1, 1, 0)             \
    X(CAMERA_SPEED, CONTROLS, CHOICE, "Camera speed", 0, 5, 1, 2)                                      \
    X(CAMERA_INVERT_X, CONTROLS, CHOICE, "Horizontal camera direction", 0, 1, 1, 0)                    \
    X(CAMERA_INVERT_Y, CONTROLS, CHOICE, "Vertical camera direction", 0, 1, 1, 0)                      \
    X(RUMBLE, CONTROLS, CHOICE, "Rumble", 0, 2, 1, 2)                                                  \
    X(DEADZONE, CONTROLS, NUMBER, "Controller deadzone", 0, 63, 1, 6)                                  \
    X(CONTROLLER, CONTROLS, CHOICE, "Active controller", 0, 4, 1, 0)                                   \
    X(MASTER_VOLUME, PRESENTATION, NUMBER, "Master volume", 0, 100, 10, 100)                           \
    X(MUSIC_VOLUME, PRESENTATION, NUMBER, "Music volume", 0, 100, 10, 100)                             \
    X(SFX_VOLUME, PRESENTATION, NUMBER, "Sound effects volume", 0, 100, 10, 100)                       \
    X(VOICE_VOLUME, PRESENTATION, NUMBER, NULL, 100, 100, 1, 100)                                      \
    X(SOUND_OUTPUT, PRESENTATION, CHOICE, "Sound output", 0, 1, 1, 0)                                  \
    X(INPUT_DISPLAY, PRESENTATION, CHOICE, "Input display", 0, 1, 1, 0)                                \
    X(CONTROLLER_DISPLAY, PRESENTATION, CHOICE, "Controller display", 0, 2, 1, 0)                      \
    X(HUD, PRESENTATION, CHOICE, "HUD visibility", 0, 2, 1, 1)                                         \
    X(RESERVED_PROMPTS, PRESENTATION, CHOICE, NULL, 0, 0, 1, 0)                                        \
    X(PAUSE_MUSIC, PRESENTATION, CHOICE, "Pause music", 0, 2, 1, 1)                                    \
    X(AUDIO_RANGE, PRESENTATION, CHOICE, "Reduced audio dynamic range", 0, 1, 1, 0)                    \
    X(AUDIO_BALANCE, PRESENTATION, NUMBER, "Stereo balance", 0, 20, 1, 10)                             \
    X(ASPECT_RATIO, PRESENTATION, CHOICE, "Aspect ratio", 0, 1, 1, 0)                                  \
    X(HUD_WIDE, PRESENTATION, CHOICE, "HUD placement in widescreen", 0, 1, 1, 1)                       \
    X(COURSE_TIMER, PRESENTATION, CHOICE, "Course timer", 0, 1, 1, 0)                                  \
    X(SESSION_TIMER, PRESENTATION, CHOICE, "Session timer", 0, 1, 1, 0)                                \
    X(BEST_TIMES, PRESENTATION, ACTION, "Best times per star", 0, 0, 1, 0)                             \
    X(HEALTH_WARNING, ACCESSIBILITY, CHOICE, "Low-health warning sound", 0, 1, 1, 1)                   \
    X(DEBOUNCE, ACCESSIBILITY, NUMBER, "Button debounce", 0, 30, 1, 0)                                 \
    X(MIN_HOLD, ACCESSIBILITY, NUMBER, "Minimum hold duration", 0, 30, 1, 0)                           \
    X(BOWSER_HOLD_SPIN, CHEATS, CHOICE, "Bowser spin assistance", 0, 1, 1, 0)                          \
    X(MENU_NAVIGATION, ACCESSIBILITY, CHOICE, "Menu navigation", 0, 2, 1, 2)                           \
    X(MENU_STICK_RELEASE, ACCESSIBILITY, CHOICE, "Require release between menu selections", 0, 1, 1,   \
      0)                                                                                               \
    X(PAUSED_CAMERA, ACCESSIBILITY, CHOICE, "Allow adjusting camera when paused", 0, 1, 1, 0)          \
    X(CAMERA_SHAKE, ACCESSIBILITY, CHOICE, "Camera shake", 0, 1, 1, 0)                                 \
    X(FLASHING, ACCESSIBILITY, CHOICE, "Flashing effects", 0, 1, 1, 0)                                 \
    X(BOWSER_THROW, GAMEPLAY, CHOICE, "Bowser throw assistance", 0, 1, 1, 1)                           \
    X(TWIRLS, GAMEPLAY, CHOICE, "Twirls", 0, 1, 1, 1)                                                  \
    X(FALL_DAMAGE, GAMEPLAY, CHOICE, "Fall damage", 0, 1, 1, 1)                                        \
    X(HELD_TWIRL, GAMEPLAY, CHOICE, "Held twirl pose", 0, 1, 1, 1)                                     \
    X(DAB, GAMEPLAY, CHOICE, "Dab", 0, 1, 1, 1)                                                        \
    X(DIZZINESS, GAMEPLAY, CHOICE, "Circle dizziness", 0, 1, 1, 1)                                     \
    X(CAP_LOSS, GAMEPLAY, CHOICE, "Cap loss", 0, 1, 1, 1)                                              \
    X(PASSIVE_HEAL, GAMEPLAY, CHOICE, "Passive healing", 0, 1, 1, 1)                                   \
    X(SWITCHES, GAMEPLAY, CHOICE, NULL, 0, 0, 1, 0)                                                    \
    X(WING_CAP, GAMEPLAY, CHOICE, "Wing Cap", 0, 1, 1, 0)                                              \
    X(METAL_CAP, GAMEPLAY, CHOICE, "Metal Cap", 0, 1, 1, 0)                                            \
    X(VANISH_CAP, GAMEPLAY, CHOICE, "Vanish Cap", 0, 1, 1, 0)                                          \
    X(SOAKED, GAMEPLAY, CHOICE, "Soaked status", 0, 1, 1, 1)                                           \
    X(BOWSER_FIGHTS, GAMEPLAY, CHOICE, "Bowser fights", 0, 1, 1, 0)                                    \
    X(CHEATS_ENABLED, CHEATS, CHOICE, "Cheats enabled", 0, 1, 1, 0)                                    \
    X(DEBUG_MODE, CHEATS, CHOICE, "Debug mode", 0, 1, 1, 0)                                            \
    X(INFINITE_HEALTH, CHEATS, CHOICE, "Infinite health", 0, 1, 1, 0)                                  \
    X(KNOCKBACK, CHEATS, CHOICE, "Disable knockback", 0, 1, 1, 0)                                      \
    X(FORCE_WING_CAP, CHEATS, CHOICE, "Wing Cap", 0, 1, 1, 0)                                          \
    X(FORCE_METAL_CAP, CHEATS, CHOICE, "Metal Cap", 0, 1, 1, 0)                                        \
    X(FORCE_VANISH_CAP, CHEATS, CHOICE, "Vanish Cap", 0, 1, 1, 0)                                      \
    X(PERMANENT_SOAKED, CHEATS, CHOICE, "Permanent Soaked", 0, 1, 1, 0)                                \
    X(KEEP_SHELL, CHEATS, CHOICE, "Prevent shell loss", 0, 1, 1, 0)                                    \
    X(FREE_MOVE, CHEATS, CHOICE, "Debug free movement", 0, 1, 1, 0)                                    \
    X(NO_SPEED_LIMIT, CHEATS, CHOICE, "Remove engine speed caps", 0, 1, 1, 0)                          \
    X(IGNORE_SLIPPERY, CHEATS, CHOICE, "Ignore slippery surfaces", 0, 1, 1, 0)                         \
    X(IGNORE_SAND, CHEATS, CHOICE, "Ignore sand", 0, 1, 1, 0)                                          \
    X(IGNORE_WIND, CHEATS, CHOICE, "Ignore wind", 0, 1, 1, 0)                                          \
    X(IGNORE_CURRENTS, CHEATS, CHOICE, "Ignore water currents", 0, 1, 1, 0)                            \
    X(CAMERA_CUSTOM, CONTROLS, NUMBER, "Custom camera speed", 1, 20, 1, 8)                             \
    X(DEBOUNCE_FRAMES, ACCESSIBILITY, NUMBER, NULL, 0, 0, 1, 0)                                        \
    X(MIN_HOLD_FRAMES, ACCESSIBILITY, NUMBER, NULL, 0, 0, 1, 0)                                        \
    X(CANNON_ANGLES, GAMEPLAY, CHOICE, "Allowed cannon angles", 0, 1, 1, 0)                            \
    X(CONTROLLER_NUMBERS, PRESENTATION, CHOICE, "Analogue stick values", 0, 1, 1, 1)                   \
    X(MARIO_SILHOUETTE, GAMEPLAY, CHOICE, "Through-wall silhouette", 0, 1, 1, 1)                       \
    X(DISABLE_THICC_TEXT, ACCESSIBILITY, CHOICE, "Disable THICC text", 0, 1, 1, 0)

enum GameOptionCategory {
    GO_SETTINGS, // Reserved: the redundant informational page was removed.
    GO_CONTROLS,
    GO_PRESENTATION,
    GO_ACCESSIBILITY,
    GO_GAMEPLAY,
    GO_CHEATS,
    GO_CATEGORY_COUNT
};

enum GameOptionKind { GO_CHOICE, GO_NUMBER, GO_ACTION };
enum GameOptionId {
#define OPTION_ID(id, category, kind, label, minimum, maximum, step, defaultValue) GO_##id,
    GAME_OPTION_CATALOG(OPTION_ID)
#undef OPTION_ID
        GO_COUNT
};

struct GameOptionDefinition {
    const char *label;
    u8 category;
    u8 kind;
    u8 minimum;
    u8 maximum;
    u8 step;
    u8 defaultValue;
};

typedef s32 (*GameOptionsRead)(u32 offset, void *data, u32 size);
typedef s32 (*GameOptionsWrite)(u32 offset, const void *data, u32 size);
extern const struct GameOptionDefinition gGameOptionDefinitions[GO_COUNT];

void game_options_init(GameOptionsRead read, GameOptionsWrite write);
// Cheat preference changes and permission grants report enablement immediately,
// including while gameplay is paused. The callback receives the active file index.
void game_options_set_record_invalidator(void (*invalidate)(s32 file));
void game_options_select_file(s32 file);
s32 game_options_active_file(void);
// Changes on initialization or a different active file, including intervening file switches.
u32 game_options_file_selection_serial(void);
// Inset edge-anchored HUD groups by 40 logical pixels in widescreen's centre layout.
s32 game_options_hud_inset(void);
s32 game_options_has_record(s32 file);
void game_options_set_saved_camera_speed(s32 file, u8 speed);
// Stored selections change immediately; these gameplay rules apply on level reload.
s32 game_options_requires_restart(enum GameOptionId id);
// Snapshot level-entry preferences and clear the outgoing visit's cheating latch.
void game_options_restart(void);
// Latch cheating for the current visit without changing immediate cheat permissions.
s32 game_options_visit_cheating(s32 current);
u8 game_options_get(enum GameOptionId id);
u8 game_options_get_file(s32 file, enum GameOptionId id);
s32 game_options_set(enum GameOptionId id, s32 value);
// Update from completed physical polls, before virtual demo input replaces pads.
void game_options_set_connected_controllers(u8 bits);
// Choice 0 is Automatic; choices 1–4 require their physical port to be present.
s32 game_options_controller_available(s32 value);
s32 game_options_cheat(enum GameOptionId id);
s32 game_options_save(s32 file);
void game_options_erase(s32 file);
// Readable defaults qualify; unread/unsupported source records forbid copying.
s32 game_options_can_copy(s32 source);
void game_options_copy(s32 source, s32 destination);
s32 game_options_dirty(s32 file);
f32 game_options_camera_speed(void);
f32 game_options_shake_scale(void);
// Up=1, Down=2, Left=4, Right=8. Each menu owns its direction/neutral latch.
#define GO_MENU_WAIT_FOR_NEUTRAL 0x80
u8 game_options_menu_direction(u8 directions, u8 *held);
u8 game_options_menu_stick(u8 directions, u8 *held);
u16 game_options_menu_dpad(u16 buttons);
u16 game_options_swap_buttons(u16 buttons, u16 first, u16 second);
s32 game_options_rumble_motor(s32 requested);

// Menus must arm on release, then sample each active menu update. Duplicate frame
// samples do not advance the hold; changing files/cancelling resets it. Five
// seconds elapse after first observing the held button, rather than including
// that initial sample in the elapsed time.
s32 game_options_confirmation_begin(enum GameOptionId id);
s32 game_options_confirmation_update(u32 frame, s32 held, s32 cancel);
void game_options_confirmation_cancel(void);
u32 game_options_confirmation_progress(void);
u32 game_options_confirmation_duration(void);

#endif
