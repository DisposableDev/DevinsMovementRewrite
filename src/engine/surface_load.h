#ifndef SURFACE_LOAD_H
#define SURFACE_LOAD_H

#include <PR/ultratypes.h>

#include "surface_collision.h"
#include "types.h"

#define SURFACE_VERTICAL_BUFFER 5

#define NORMAL_FLOOR_THRESHOLD 0.01f
#define NORMAL_CEIL_THRESHOLD -NORMAL_FLOOR_THRESHOLD

/**
 * The size of the dynamic surface pool, in bytes.
 */
#define DYNAMIC_SURFACE_POOL_SIZE 0x8000

struct SurfaceNode {
    struct SurfaceNode *next;
    struct Surface *surface;
};

enum SpatialPartitions {
    SPATIAL_PARTITION_FLOORS,
    SPATIAL_PARTITION_CEILS,
    SPATIAL_PARTITION_WALLS,
    SPATIAL_PARTITION_WATER,
    NUM_SPATIAL_PARTITIONS
};

typedef struct SurfaceNode *SpatialPartitionCell[NUM_SPATIAL_PARTITIONS];

extern SpatialPartitionCell gStaticSurfacePartition[NUM_CELLS][NUM_CELLS];
extern SpatialPartitionCell gDynamicSurfacePartition[NUM_CELLS][NUM_CELLS];
// Changes whenever static collision is rebuilt or appended to.
extern u32 gStaticSurfaceGeneration;
extern void *gCurrStaticSurfacePool;
extern void *gDynamicSurfacePool;
extern void *gCurrStaticSurfacePoolEnd;
extern void *gDynamicSurfacePoolEnd;
extern u32 gTotalStaticSurfaceData;

// Drop pool-owned wall metadata before area reclamation or late static publication.
void invalidate_static_wall_height_index(void);
void alloc_surface_pools(void);
#ifdef NO_SEGMENTED_MEMORY
u32 get_area_terrain_size(TerrainData *data);
#endif
void load_area_terrain(s32 index, TerrainData *data, RoomData *surfaceRooms, MacroObject *macroObjects);
// A conservative per-area test; TRUE also covers positions outside the collision grid.
s32 water_cell_may_contain_water(s32 x, s32 z);
void clear_dynamic_surfaces(void);
void load_object_collision_model(void);
void load_object_static_model(void);

// Large persistent meshes share the native metadata, grid and profiling counters.
s32 surf_has_no_cam_collision(s32 surfaceType);
void publish_dynamic_surface_cell(s32 cellX, s32 cellZ, s32 partition, struct SurfaceNode *head);
void record_persistent_surface_counts(u32 surfaces, u32 nodes);

#endif // SURFACE_LOAD_H
