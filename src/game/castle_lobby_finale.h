#ifndef CASTLE_LOBBY_FINALE_H
#define CASTLE_LOBBY_FINALE_H

#include "types.h"

struct Object;
struct GraphNode;

#define MODEL_CASTLE_LOBBY_PEACH MODEL_LEVEL_GEOMETRY_06
#define MODEL_CASTLE_LOBBY_BOBOMB MODEL_LEVEL_GEOMETRY_07
#define MODEL_CASTLE_LOBBY_KOOPA MODEL_LEVEL_GEOMETRY_08
#define MODEL_CASTLE_LOBBY_BOWSER MODEL_LEVEL_GEOMETRY_09
#define MODEL_CASTLE_LOBBY_PIRANHA MODEL_LEVEL_GEOMETRY_0A
#define MODEL_CASTLE_LOBBY_MOTOS MODEL_LEVEL_GEOMETRY_0B
#define MODEL_CASTLE_LOBBY_MIPS MODEL_LEVEL_GEOMETRY_0C

f32 castle_lobby_guest_focus_height(struct Object *guest);
s32 castle_lobby_finale_sun_ready(void);
s32 castle_lobby_finale_sun_alpha(void);
Gfx *geo_castle_lobby_sunbeam_alpha(s32 callContext, struct GraphNode *node, Mat4 mtx);
void castle_lobby_finale_sign_read(struct Object *sign);
Gfx *geo_castle_lobby_guest_alpha(s32 callContext, struct GraphNode *node, Mat4 mtx);
Gfx *geo_switch_castle_lobby_guest_opacity(s32 callContext, struct GraphNode *node, Mat4 mtx);
Gfx *geo_switch_castle_lobby_sunbeam(s32 callContext, struct GraphNode *node, Mat4 mtx);

#endif
