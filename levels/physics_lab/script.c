#include <ultra64.h>
#include "sm64.h"
#include "behavior_data.h"
#include "model_ids.h"
#include "object_constants.h"
#include "seq_ids.h"
#include "segment_symbols.h"
#include "level_commands.h"
#include "game/level_update.h"
#include "levels/scripts.h"
#include "actors/common1.h"
#include "make_const_nonconst.h"
#include "levels/physics_lab/header.h"
#include "levels/physics_lab/enemy_assets.h"

// BP1=1 makes cap boxes available even on a fresh save, without setting save flags.
#define LAB_CAP(kind, x, y, z)                                                                         \
    OBJECT(MODEL_EXCLAMATION_BOX, x, y, z, 0, 0, 0, (1 << 24) | ((kind) << 16), bhvExclamationBox)
#define LAB_CAP_ROW(x, y, z)                                                                           \
    LAB_CAP(EXCLAMATION_BOX_BP_WING_CAP, (x) - 400, y, z),                                             \
        LAB_CAP(EXCLAMATION_BOX_BP_METAL_CAP, x, y, z),                                                \
        LAB_CAP(EXCLAMATION_BOX_BP_VANISH_CAP, (x) + 400, y, z)

#define LAB_CAP_COLUMN(x, y, z)                                                                        \
    LAB_CAP(EXCLAMATION_BOX_BP_WING_CAP, x, y, (z) - 400),                                             \
        LAB_CAP(EXCLAMATION_BOX_BP_METAL_CAP, x, y, z),                                                \
        LAB_CAP(EXCLAMATION_BOX_BP_VANISH_CAP, x, y, (z) + 400)

// Preserve the area hierarchy in this declarative level-command array.
// clang-format off
const LevelScript level_physics_lab_entry[] = {
    INIT_LEVEL(),
    LOAD_YAY0(0x07, _physics_lab_segment_7SegmentRomStart, _physics_lab_segment_7SegmentRomEnd),
    LOAD_YAY0_TEXTURE(0x09, _generic_yay0SegmentRomStart, _generic_yay0SegmentRomEnd),
    LOAD_YAY0(0x0A, _water_skybox_yay0SegmentRomStart, _water_skybox_yay0SegmentRomEnd),
    LOAD_YAY0(0x08, _common0_yay0SegmentRomStart, _common0_yay0SegmentRomEnd),
    LOAD_RAW(0x0F, _common0_geoSegmentRomStart, _common0_geoSegmentRomEnd),
    ALLOC_LEVEL_POOL(),
    MARIO(MODEL_MARIO, 0x00000001, bhvMario),
    JUMP_LINK(script_func_global_1),
    LOAD_MODEL_FROM_GEO(MODEL_BOB_BUBBLY_TREE, bubbly_tree_geo),
    LOAD_MODEL_FROM_GEO(MODEL_LEVEL_GEOMETRY_03, physics_lab_pole_short_geo),
    LOAD_MODEL_FROM_GEO(MODEL_LEVEL_GEOMETRY_04, physics_lab_pole_tall_geo),
    LOAD_MODEL_FROM_GEO(MODEL_LEVEL_GEOMETRY_05, physics_lab_tilter_geo),
    LOAD_MODEL_FROM_GEO(MODEL_LEVEL_GEOMETRY_06, physics_lab_shuttle_geo),
    LOAD_MODEL_FROM_GEO(MODEL_LEVEL_GEOMETRY_07, physics_lab_disk_geo),
    LOAD_MODEL_FROM_GEO(MODEL_LEVEL_GEOMETRY_08, physics_lab_bridge_geo),
    LOAD_MODEL_FROM_GEO(MODEL_LEVEL_GEOMETRY_09, physics_lab_closing_wall_geo),
    LOAD_MODEL_FROM_GEO(MODEL_LEVEL_GEOMETRY_0A, physics_lab_pole_impact_geo),
    LOAD_MODEL_FROM_GEO(MODEL_LEVEL_GEOMETRY_0B, physics_lab_pole_launch_geo),
    #include "levels/physics_lab/enemy_models.inc.c"

    AREA(1, physics_lab_geo),
        OBJECT(MODEL_NONE, 0, 20, 400, 0, 180, 0, 0x000A0000, bhvSpinAirborneWarp),
        WARP_NODE(0x0A, LEVEL_PHYSICS_LAB, 1, 0x0A, WARP_NO_CHECKPOINT),
        WARP_NODE(0xF0, LEVEL_PHYSICS_LAB, 1, 0x0A, WARP_NO_CHECKPOINT),
        WARP_NODE(0xF1, LEVEL_PHYSICS_LAB, 1, 0x0A, WARP_NO_CHECKPOINT),

        OBJECT(MODEL_WOODEN_SIGNPOST, 250, 0, 250, 0, 0, 0, 0, bhvPhysicsLabWelcomeSign),
        // Tucked into the rear corner, with the reading camera inside the shaft.
        OBJECT(MODEL_WOODEN_SIGNPOST, 10640, -2000, 5640, 0, -135, 0, 0, bhvPhysicsLabHiddenSign),

        // Central supply, with duplicates at the launch deck and pool edge.
        LAB_CAP_ROW(0, 220, -850),
        LAB_CAP_ROW(2100, 3220, -2100),
        LAB_CAP_ROW(1800, 220, 800),
        LAB_CAP(EXCLAMATION_BOX_BP_KOOPA_SHELL, -2200, 220, 500),
        LAB_CAP(EXCLAMATION_BOX_BP_KOOPA_SHELL, 5200, 220, 800),
        OBJECT(MODEL_KOOPA_SHELL, 5400, -1500, 5300, 0, 0, 0, 0, bhvKoopaShellUnderwater),

        // Movement objects and climbable fixtures.
        OBJECT(MODEL_BREAKABLE_BOX, -2600, 50, 500, 0, 0, 0, 0, bhvBreakableBoxSmall),
        OBJECT(MODEL_BREAKABLE_BOX, 1900, 50, 1100, 0, 0, 0, 0, bhvBreakableBoxSmall),
        OBJECT(MODEL_METAL_BOX, -3200, 0, 500, 0, 0, 0, 0, bhvPushableMetalBox),
        OBJECT(MODEL_BOB_BUBBLY_TREE, 400, 0, 3000, 0, 0, 0, 0, bhvTree),
        OBJECT(MODEL_BOB_BUBBLY_TREE, 400, 0, 4000, 0, 0, 0, 0, bhvTree),
        OBJECT(MODEL_LEVEL_GEOMETRY_03, -300, 0, 3000, 0, 0, 0, 120 << 16, bhvPoleGrabbing),
        OBJECT(MODEL_LEVEL_GEOMETRY_04, -300, 0, 4000, 0, 0, 0, 200 << 16, bhvPoleGrabbing),
        OBJECT(MODEL_LEVEL_GEOMETRY_0A, 7150, 0, 8600, 0, 0, 0, 380 << 16, bhvPoleGrabbing),
        // Direct launch-deck access, outside its columns and walking ramps.
        OBJECT(MODEL_LEVEL_GEOMETRY_0B, 1020, 0, -2100, 0, 0, 0, 320 << 16, bhvPoleGrabbing),
        OBJECT(MODEL_CHECKERBOARD_PLATFORM, 800, 500, -5500, 0, 90, 0, 0x01280000, bhvCheckerboardElevatorGroup),
        OBJECT(MODEL_LEVEL_GEOMETRY_05, -600, 700, -5650, 0, 0, 0, 0, bhvPhysicsLabTiltingPlatform),
        OBJECT(MODEL_CANNON_BASE, -6400, 0, 0, 0, 90, 0, 0x00400000, bhvCannon),

        // Supplies beside the new circuit, flight landing, and water terraces.
        LAB_CAP(EXCLAMATION_BOX_BP_KOOPA_SHELL, -12100, 220, -5400),
        LAB_CAP_ROW(8800, 920, -9700),
        LAB_CAP_ROW(8000, 220, 800),
        LAB_CAP(EXCLAMATION_BOX_BP_KOOPA_SHELL, 9000, 220, 800),
        LAB_CAP_ROW(-7600, 220, 5600),
        LAB_CAP_ROW(0, 220, 7650),
        LAB_CAP_ROW(4500, 220, 10400),
        LAB_CAP_ROW(3000, 220, -10500),
        LAB_CAP_ROW(-12500, 1420, -14200),
        LAB_CAP_COLUMN(13600, 220, 1500),
        LAB_CAP_ROW(10500, 220, 7450),
        OBJECT(MODEL_KOOPA_SHELL, 7000, -1700, 5400, 0, 0, 0, 0, bhvKoopaShellUnderwater),

        // Shuttle, rotating disk, rocking bridge, low-roof lift, and closing walls.
        OBJECT(MODEL_LEVEL_GEOMETRY_06, 8900, 250, -4800, 0, 0, 0, LAB_MOVER_SHUTTLE << 16, bhvPhysicsLabMover),
        OBJECT(MODEL_LEVEL_GEOMETRY_07, 11800, 300, -4800, 0, 0, 0, LAB_MOVER_DISK << 16, bhvPhysicsLabMover),
        OBJECT(MODEL_LEVEL_GEOMETRY_08, 9300, 350, -2400, 0, 0, 0, LAB_MOVER_BRIDGE << 16, bhvPhysicsLabMover),
        OBJECT(MODEL_LEVEL_GEOMETRY_06, 8300, 140, 0, 0, 0, 0, LAB_MOVER_LIFT << 16, bhvPhysicsLabMover),
        OBJECT(MODEL_LEVEL_GEOMETRY_09, 10500, 0, 0, 0, 0, 0, LAB_MOVER_WALL_LEFT << 16, bhvPhysicsLabMover),
        OBJECT(MODEL_LEVEL_GEOMETRY_09, 10500, 0, 0, 0, 0, 0, LAB_MOVER_WALL_RIGHT << 16, bhvPhysicsLabMover),

        // Static sculptures are batched scenery; collision is in the terrain.
        #include "levels/physics_lab/enemies.inc.c"

        TERRAIN(physics_lab_collision),
        TERRAIN_TYPE(TERRAIN_STONE),
        SET_BACKGROUND_MUSIC(0, SEQ_LEVEL_PHYSICS_LAB),
    END_AREA(),
    FREE_LEVEL_POOL(),
    MARIO_POS(1, 180, 0, 20, 400),
    CALL(0, lvl_init_or_update),
    CALL_LOOP(1, lvl_init_or_update),
    CLEAR_LEVEL(),
    SLEEP_BEFORE_EXIT(1),
    EXIT(),
};
// clang-format on

#undef LAB_CAP

#undef LAB_CAP_ROW

#undef LAB_CAP_COLUMN
