#include <ultra64.h>
#include "sm64.h"
#include "geo_commands.h"
#include "game/level_geo.h"
#include "game/geo_misc.h"
#include "game/behavior_actions.h"
#include "actors/common1.h"
#include "game/camera.h"
#include "game/screen_transition.h"
#include "make_const_nonconst.h"
#include "levels/physics_lab/header.h"
#include "levels/physics_lab/enemy_geo.inc.c"

// Preserve the visible node hierarchy in these declarative geo-command arrays.
// clang-format off
const GeoLayout physics_lab_pole_short_geo[] = {
    GEO_CULLING_RADIUS(1500),
    GEO_OPEN_NODE(),
        GEO_DISPLAY_LIST(LAYER_OPAQUE, physics_lab_pole_short_dl),
    GEO_CLOSE_NODE(),
    GEO_END(),
};

const GeoLayout physics_lab_pole_tall_geo[] = {
    GEO_CULLING_RADIUS(2300),
    GEO_OPEN_NODE(),
        GEO_DISPLAY_LIST(LAYER_OPAQUE, physics_lab_pole_tall_dl),
    GEO_CLOSE_NODE(),
    GEO_END(),
};

const GeoLayout physics_lab_pole_impact_geo[] = {
    GEO_CULLING_RADIUS(4100),
    GEO_OPEN_NODE(),
        GEO_DISPLAY_LIST(LAYER_OPAQUE, physics_lab_pole_impact_dl),
    GEO_CLOSE_NODE(),
    GEO_END(),
};

const GeoLayout physics_lab_pole_launch_geo[] = {
    GEO_CULLING_RADIUS(3500),
    GEO_OPEN_NODE(),
        GEO_DISPLAY_LIST(LAYER_OPAQUE, physics_lab_pole_launch_dl),
    GEO_CLOSE_NODE(),
    GEO_END(),
};

const GeoLayout physics_lab_tilter_geo[] = {
    GEO_CULLING_RADIUS(1000),
    GEO_OPEN_NODE(),
        GEO_DISPLAY_LIST(LAYER_OPAQUE, physics_lab_tilter_dl),
    GEO_CLOSE_NODE(),
    GEO_END(),
};

#define LAB_MOVER_GEO(name, radius) \
const GeoLayout physics_lab_##name##_geo[] = { \
    GEO_CULLING_RADIUS(radius), \
    GEO_OPEN_NODE(), \
        GEO_DISPLAY_LIST(LAYER_OPAQUE, physics_lab_##name##_dl), \
    GEO_CLOSE_NODE(), \
    GEO_END(), \
};

LAB_MOVER_GEO(shuttle, 650)
LAB_MOVER_GEO(disk, 750)
LAB_MOVER_GEO(bridge, 1200)
LAB_MOVER_GEO(closing_wall, 1000)

#undef LAB_MOVER_GEO

const GeoLayout physics_lab_geo[] = {
    GEO_NODE_SCREEN_AREA(10, SCREEN_CENTER_X, SCREEN_CENTER_Y, SCREEN_CENTER_X, SCREEN_CENTER_Y),
    GEO_OPEN_NODE(),
        GEO_ZBUFFER(0),
        GEO_OPEN_NODE(),
            GEO_NODE_ORTHO(100),
            GEO_OPEN_NODE(),
                GEO_BACKGROUND(BACKGROUND_OCEAN_SKY, geo_skybox_main),
            GEO_CLOSE_NODE(),
        GEO_CLOSE_NODE(),
        GEO_ZBUFFER(1),
        GEO_OPEN_NODE(),
            GEO_CAMERA_FRUSTUM_WITH_FUNC(45, 100, 30000, geo_camera_fov),
            GEO_OPEN_NODE(),
                GEO_CAMERA(CAMERA_MODE_8_DIRECTIONS, 0, 2000, 6000, 0, 0, 0, geo_camera_main),
                GEO_OPEN_NODE(),
                    GEO_ASM(0, geo_physics_lab_terrain),
                    GEO_DISPLAY_LIST(LAYER_TRANSPARENT, physics_lab_water_dl),
                    GEO_TRANSLATE_NODE(LAYER_OPAQUE, LAB_SCULPTURE_ORIGIN_X, 0, LAB_SCULPTURE_ORIGIN_Z),
                    GEO_OPEN_NODE(),
                        GEO_SCALE(LAYER_OPAQUE, 0x4000),
                        GEO_OPEN_NODE(),
                            GEO_ASM(0, geo_physics_lab_sculptures),
                        GEO_CLOSE_NODE(),
                    GEO_CLOSE_NODE(),
                    GEO_RENDER_OBJ(),
                    GEO_ASM(ENVFX_MODE_NONE, geo_envfx_main),
                GEO_CLOSE_NODE(),
            GEO_CLOSE_NODE(),
        GEO_CLOSE_NODE(),
        GEO_ZBUFFER(0),
        GEO_OPEN_NODE(),
            GEO_ASM(0, geo_cannon_circle_base),
        GEO_CLOSE_NODE(),
    GEO_CLOSE_NODE(),
    GEO_END(),
};
// clang-format on
