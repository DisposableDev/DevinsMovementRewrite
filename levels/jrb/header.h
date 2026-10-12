#ifndef JRB_HEADER_H
#define JRB_HEADER_H

#include "types.h"
#include "game/moving_texture.h"

// geo
extern const GeoLayout jrb_geo_000900[];
extern const GeoLayout jrb_geo_000918[];
extern const GeoLayout jrb_geo_000930[];
extern const GeoLayout jrb_geo_000948[];
extern const GeoLayout jrb_geo_000960[];
extern const GeoLayout jrb_geo_000978[];
extern const GeoLayout jrb_geo_000990[];
extern const GeoLayout jrb_geo_0009B0[];
extern const GeoLayout jrb_geo_0009C8[];
extern const GeoLayout jrb_geo_0009E8[];
extern const GeoLayout jrb_geo_000A00[];
extern const GeoLayout jrb_geo_000A18[];
extern const GeoLayout jrb_geo_000AFC[];

// leveldata
extern const Gfx jrb_seg7_dl_07002FD0[];
extern const Gfx jrb_seg7_dl_07004940[];
extern const Gfx jrb_seg7_dl_07004C78[];
extern const Gfx jrb_seg7_dl_070058C8[];
extern const Gfx jrb_seg7_dl_070069B0[];
extern const Gfx jrb_seg7_dl_07007570[];
extern const Gfx jrb_seg7_dl_07004940_culled[];
extern const Gfx jrb_seg7_dl_07004C78_culled[];
extern const Gfx jrb_seg7_dl_070058C8_culled[];
extern const Gfx jrb_seg7_dl_070069B0_culled[];
extern const Gfx jrb_seg7_dl_07007570_culled[];
extern const Gfx jrb_seg7_dl_07004940_cave[];
extern const Gfx jrb_seg7_dl_07004940_tunnel[];
extern const Gfx jrb_seg7_dl_07004C78_cave[];
extern const Gfx jrb_seg7_dl_07004C78_tunnel[];
extern const Gfx jrb_seg7_dl_070069B0_cave[];
extern const Gfx jrb_seg7_dl_070069B0_tunnel[];
extern const Gfx jrb_seg7_dl_07007718[];
extern const Gfx jrb_seg7_dl_07007AC8[];
extern const Gfx jrb_seg7_dl_07007DC8[];
extern const Gfx jrb_seg7_dl_070080F8[];
extern const Gfx jrb_seg7_dl_07008FD8[];
extern const Gfx jrb_seg7_dl_070090B0[];
extern const Gfx jrb_seg7_dl_07009A58[];
extern const Gfx jrb_seg7_dl_07009B30[];
extern const Gfx jrb_seg7_dl_0700A608[];
extern const Gfx jrb_seg7_dl_0700AC68[];
extern const Gfx jrb_ship_medium_dl_07008FD8[];
extern const Gfx jrb_ship_medium_dl_070090B0[];
extern const Gfx jrb_ship_medium_dl_07009A58[];
extern const Gfx jrb_ship_medium_dl_07009B30[];
extern const Gfx jrb_ship_medium_dl_0700A608[];
extern const Gfx jrb_ship_medium_dl_0700AC68[];
extern const Gfx jrb_ship_very_far_dl_07008FD8[];
extern const Gfx jrb_ship_very_far_dl_070090B0[];
extern const Gfx jrb_ship_very_far_dl_07009A58[];
extern const Gfx jrb_ship_very_far_dl_07009B30[];
extern const Gfx jrb_ship_very_far_dl_0700A608[];
extern const Gfx jrb_ship_very_far_dl_0700AC68[];
extern const Gfx jrb_ship_far_dl_07008FD8[];
extern const Gfx jrb_ship_far_dl_070090B0[];
extern const Gfx jrb_ship_far_dl_07009A58[];
extern const Gfx jrb_ship_far_dl_07009B30[];
extern const Gfx jrb_ship_far_dl_0700A608[];
extern const Gfx jrb_ship_far_dl_0700AC68[];
extern const Gfx jrb_seg7_dl_0700AE48[];
extern const Gfx jrb_seg7_dl_0700AFB0[];
extern const Collision jrb_seg7_area_1_collision[];
extern const MacroObject jrb_seg7_area_1_macro_objs[];
extern const Collision jrb_seg7_collision_rock_solid[];
extern const Collision jrb_seg7_collision_floating_platform[];
extern const Collision jrb_seg7_collision_floating_box[];
extern const Collision jrb_seg7_collision_in_sunken_ship_3[];
extern const Collision jrb_seg7_collision_in_sunken_ship[];
extern const Collision jrb_seg7_collision_0700CEF0[];
extern const Collision jrb_seg7_collision_in_sunken_ship_2[];
extern const Collision jrb_seg7_collision_pillar_base[];
extern const Collision jrb_seg7_collision_0700D1DC[];
extern const Trajectory jrb_seg7_trajectory_unagi_1[];
extern const Trajectory jrb_seg7_trajectory_unagi_2[];
extern const struct MovtexQuadCollection jrb_movtex_water[];
extern const struct MovtexQuadCollection jrb_movtex_initial_mist[];
extern const Gfx jrb_seg7_dl_0700EF00[];
extern const Gfx jrb_seg7_dl_0700FE48[];
extern const Gfx jrb_seg7_dl_07010548[];
extern const Collision jrb_seg7_area_2_collision[];
extern const MacroObject jrb_seg7_area_2_macro_objs[];
extern const struct MovtexQuadCollection jrb_movtex_sunken_ship_water[];

// SimpleFlips encounter (area 3 only)
extern const GeoLayout jrb_simpleflips_room_geo[];
extern const GeoLayout jrb_simpleflips_toad_geo[];
extern const GeoLayout jrb_simpleflips_punch_geo[];
extern const Gfx jrb_simpleflips_room_dl[];
extern const Gfx jrb_simpleflips_toad_dl_06007710[];
extern const Gfx jrb_simpleflips_toad_dl_06007A48[];
extern const Gfx jrb_simpleflips_toad_dl_06007EB0[];
extern const Gfx jrb_simpleflips_toad_dl_060081F8[];
extern const Gfx jrb_simpleflips_toad_dl_06008520[];
extern const Gfx jrb_simpleflips_toad_dl_06008890[];
extern const Gfx jrb_simpleflips_toad_dl_06008BD8[];
extern const Collision jrb_simpleflips_collision[];
extern const struct Animation *const jrb_simpleflips_anims[];

extern const GeoLayout jrb_simpleflips_mips_geo[];
extern const GeoLayout jrb_simpleflips_sign_geo[];
extern const struct Animation *const jrb_simpleflips_mips_anims[];
extern const Gfx jrb_simpleflips_sign_dl_0302DA48[];
extern const Gfx jrb_simpleflips_sign_dl_0302DD08[];
extern const Gfx jrb_simpleflips_mips_dl_060107B8[];
extern const Gfx jrb_simpleflips_mips_dl_06010C40[];
extern const Gfx jrb_simpleflips_mips_dl_06010E60[];
extern const Gfx jrb_simpleflips_mips_dl_060110B0[];
extern const Gfx jrb_simpleflips_mips_dl_06011310[];
extern const Gfx jrb_simpleflips_mips_dl_06011520[];
extern const Gfx jrb_simpleflips_mips_dl_06011780[];
extern const Gfx jrb_simpleflips_mips_dl_06011990[];
extern const Gfx jrb_simpleflips_mips_dl_06011BB0[];
extern const Gfx jrb_simpleflips_mips_dl_06011DB0[];
extern const Gfx jrb_simpleflips_mips_dl_06011ED8[];
extern const Gfx jrb_simpleflips_mips_dl_06011FC0[];

// script
#include "actors/motos/header.h"

extern const LevelScript level_jrb_entry[];

#endif
