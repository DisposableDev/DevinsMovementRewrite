#ifndef MARIO_CHARACTER_H
#define MARIO_CHARACTER_H

#include <PR/ultratypes.h>

struct MarioState;

enum MarioAppearance { MARIO_APPEARANCE_NORMAL, MARIO_APPEARANCE_SIMPLIFIED };
enum MarioPalette { MARIO_PALETTE_STANDARD, MARIO_PALETTE_HE_THICC };
enum MarioTextStyle { MARIO_TEXT_STANDARD, MARIO_TEXT_HE_THICC };

struct MarioMovementProfile {
    f32 walkTarget;       // World units/frame.
    f32 walkAcceleration; // World units/frame^2, before walking drag.
    // Dimensionless multipliers for vertical launch speed, retained speed and speed-derived lift.
    f32 jumpScale;
    f32 jumpRetention;
    f32 jumpSpeedContribution;
};

struct MarioCharacterSelection {
    u8 appearance;
    u8 palette;
};

#define MARIO_CHARACTER_COUNT 4

extern const struct MarioMovementProfile gMarioMovementProfiles[1];
#define MARIO_TAS_CODE_FRAMES 64
// Call once per menu update with held buttons; the menu owns and resets progress.
s32 mario_character_update_tas_code(u8 *progress, u32 buttons);
void mario_character_init(void);
// Stage the save palette with normal appearance; fileIndex is zero-based.
void mario_character_load_file(s32 fileIndex);
s32 mario_character_can_unlock(struct MarioState *m);
s32 mario_character_is_unlock_death(struct MarioState *m);
// Reject invalid indices or a different save palette; FALSE leaves the request unchanged.
s32 mario_character_request(s32 index);
// Commit selection and movement profile at Mario initialization.
// useDefault selects index 0 without replacing the request.
void mario_character_commit(struct MarioState *m, s32 useDefault);
// Display compatible appearance changes immediately; retain the active palette.
void mario_character_apply_appearance(s32 useDefault);
s32 mario_character_active_index(void);
s32 mario_character_requested_index(void);
u8 mario_character_palette(void);
s32 mario_character_is_simplified(void);
enum MarioTextStyle mario_character_text_style(void);
const struct MarioMovementProfile *mario_character_movement(void);
const char *mario_character_name(s32 index);
const char *mario_character_movement_name(void);

#endif
