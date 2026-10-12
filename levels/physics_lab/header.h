#ifndef PHYSICS_LAB_HEADER_H
#define PHYSICS_LAB_HEADER_H

#include "types.h"
#include "small_data.h"
#include "game/physics_lab.h"

enum PhysicsLabMover {
    LAB_MOVER_SHUTTLE,
    LAB_MOVER_DISK,
    LAB_MOVER_BRIDGE,
    LAB_MOVER_LIFT,
    LAB_MOVER_WALL_LEFT,
    LAB_MOVER_WALL_RIGHT,
};

extern const LevelScript level_physics_lab_entry[];
extern const GeoLayout physics_lab_geo[];
extern const GeoLayout physics_lab_pole_short_geo[];
extern const GeoLayout physics_lab_pole_tall_geo[];
extern const GeoLayout physics_lab_pole_impact_geo[];
extern const GeoLayout physics_lab_pole_launch_geo[];
extern const GeoLayout physics_lab_tilter_geo[];
extern const Gfx physics_lab_terrain_dl[];
extern const Gfx physics_lab_terrain_begin[];
extern const Gfx physics_lab_terrain_end[];
extern const Gfx physics_lab_ground_begin[];
extern const Gfx physics_lab_ground_end[];
extern const Gfx physics_lab_ground_dl[];
extern const Gfx physics_lab_ground_wide_dl[];
extern const struct PhysicsLabRenderSection physics_lab_ground_clip_bounds SEGMENTED_DATA;
extern const struct PhysicsLabTerrainBatch physics_lab_terrain_batches[];
extern const u32 physics_lab_terrain_batch_count SEGMENTED_DATA;
extern const struct PhysicsLabTerrainBatch physics_lab_terrain_north_batches[];
extern const f32 physics_lab_north_view_z SEGMENTED_DATA;
extern const struct PhysicsLabTerrainBatch physics_lab_terrain_aerial_batches[];
extern const f32 physics_lab_aerial_view_y SEGMENTED_DATA;
extern const f32 physics_lab_aerial_view_half_width SEGMENTED_DATA;
extern const struct PhysicsLabRenderSection physics_lab_terrain_lod_bounds[];
extern const u32 physics_lab_terrain_lod_count SEGMENTED_DATA;
extern const struct PhysicsLabSculpture physics_lab_sculptures[];
extern const u32 physics_lab_sculpture_count SEGMENTED_DATA;
extern const Gfx physics_lab_sculptures_dl[];
extern const Gfx physics_lab_water_dl[];
extern const Gfx physics_lab_pole_short_dl[];
extern const Gfx physics_lab_pole_tall_dl[];
extern const Gfx physics_lab_pole_impact_dl[];
extern const Gfx physics_lab_pole_launch_dl[];
extern const Gfx physics_lab_tilter_dl[];
extern const Collision physics_lab_collision[];
extern const Collision physics_lab_tilter_collision[];
extern const GeoLayout physics_lab_shuttle_geo[];
extern const GeoLayout physics_lab_disk_geo[];
extern const GeoLayout physics_lab_bridge_geo[];
extern const GeoLayout physics_lab_closing_wall_geo[];
extern const Gfx physics_lab_shuttle_dl[];
extern const Gfx physics_lab_disk_dl[];
extern const Gfx physics_lab_bridge_dl[];
extern const Gfx physics_lab_closing_wall_dl[];
extern const Collision physics_lab_shuttle_collision[];
extern const Collision physics_lab_disk_collision[];
extern const Collision physics_lab_bridge_collision[];
extern const Collision physics_lab_closing_wall_collision[];

#endif
