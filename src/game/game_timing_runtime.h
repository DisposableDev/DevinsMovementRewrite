#ifndef GAME_TIMING_RUNTIME_H
#define GAME_TIMING_RUNTIME_H

#include <PR/ultratypes.h>

struct Object;
void game_timing_runtime_entered(void);
// Called before world updates; running is the play-mode gate, refined by world state.
void game_timing_runtime_update(s32 running);
void game_timing_runtime_star(struct Object *obj, u32 star);
void game_timing_render_hud(u8 alpha);

#endif
