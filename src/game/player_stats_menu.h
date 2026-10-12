#ifndef PLAYER_STATS_MENU_H
#define PLAYER_STATS_MENU_H

#include <PR/ultratypes.h>

// Open General and quarantine a held stick until it returns to neutral.
void player_stats_menu_open(void);
/* Returns TRUE on B; repeated renders within one game frame do not repeat input. */
s32 player_stats_menu_update(void);
void player_stats_menu_render(void);
// Draw ASCII in the caller's active IA text pipeline; unsupported bytes become spaces.
// Text is capped at 95 glyphs; centered text returns its left edge for cursor placement.
void player_stats_menu_text(s16 x, s16 y, const char *text);
s16 player_stats_menu_text_centered(s16 centerX, s16 y, const char *text);
const char *player_stats_menu_course_name(s32 course);

#endif
