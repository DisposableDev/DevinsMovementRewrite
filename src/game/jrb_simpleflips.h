#ifndef JRB_SIMPLEFLIPS_H
#define JRB_SIMPLEFLIPS_H

#include "types.h"

struct GraphNode;
struct Object;
struct MarioState;

#define JRB_SIMPLEFLIPS_AREA 3
#define JRB_SIMPLEFLIPS_WINDOW_NODE 0x20
#define MODEL_JRB_SIMPLEFLIPS_TOAD MODEL_LEVEL_GEOMETRY_03
#define MODEL_JRB_SIMPLEFLIPS_PUNCH MODEL_LEVEL_GEOMETRY_04
#define MODEL_JRB_SIMPLEFLIPS_MIPS MODEL_LEVEL_GEOMETRY_05
#define MODEL_JRB_SIMPLEFLIPS_SIGN MODEL_LEVEL_GEOMETRY_06
#define MODEL_JRB_SIMPLEFLIPS_MOTOS MODEL_LEVEL_GEOMETRY_07

// Game-thread lifecycle: reset clears transient encounter/title state, not save markers.
void jrb_simpleflips_reset(void);
// Consume a saved one-shot game over. TRUE selects its 1-based file; FALSE finds none.
s32 lvl_simpleflips_boot(s16 arg, s32 unusedArg);
// Restore the normal cap and lost-cap flags/locks before the caller saves the visit.
// Temporary cap powers remain active.
void jrb_simpleflips_restore_cap(struct MarioState *m);

// State queries and Mario hooks; the separate area owns confinement and pause lock.
s32 jrb_simpleflips_room_active(void);
s32 jrb_simpleflips_is_game_over(void);
// Eligible encounters and Toad's game over keep their pause/exit lock.
// Blocked visits retain ordinary escape routes.
s32 jrb_simpleflips_exit_locked(void);
// Inside the room, SFX remain allowed; only the milestone-enabled room song may start.
s32 jrb_simpleflips_music_allowed(u8 player, u8 seqId);
void jrb_simpleflips_confine_mario(struct MarioState *m);
// Record a committed sign interaction; milestone effects wait until its reading action ends.
void jrb_simpleflips_begin_sign_read(struct Object *sign);

// Encounter object callbacks.
void bhv_jrb_simpleflips_window_loop(void);
void bhv_jrb_simpleflips_sign_loop(void);
void bhv_jrb_simpleflips_mips_loop(void);
void bhv_jrb_simpleflips_motos_loop(void);
void bhv_jrb_simpleflips_one_up_loop(void);

// Room, camera and game-over head geometry callbacks.
Gfx *geo_simpleflips_room(s32 callContext, struct GraphNode *node, void *context);
Gfx *geo_simpleflips_camera(s32 callContext, struct GraphNode *node, void *context);
Gfx *geo_switch_simpleflips_head(s32 callContext, struct GraphNode *node, void *context);

#endif
