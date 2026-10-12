#ifndef GAME_OPTIONS_RUNTIME_H
#define GAME_OPTIONS_RUNTIME_H

#include "game_options.h"

// Runtime permissions require the selected playing file, with no demo or credits.
s32 game_options_runtime_allowed(void);
// Shared record/secret eligibility, including enabled but currently dormant cheats.
s32 game_options_is_cheating(void);
// Secrets stay blocked until a fresh level load after all cheating is disabled.
s32 game_options_secrets_blocked(void);
void game_options_runtime_restart(void);
s32 game_options_debug_active(void);
s32 game_options_free_move_active(void);
// Effect requests include Disable knockback On; free movement belongs to Debug Mode.
s32 game_options_cheat_active(enum GameOptionId id);
s32 game_options_damage_voice_muted(void);
void game_options_runtime_update(void);

#endif
