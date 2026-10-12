// 0x1700009C
const GeoLayout small_water_splash_geo[] = {
    GEO_NODE_START(),
    GEO_OPEN_NODE(),
    GEO_ASM(0, geo_update_small_water_splash_opacity),
    GEO_SWITCH_CASE(6, geo_switch_anim_state),
    GEO_OPEN_NODE(),
    GEO_DISPLAY_LIST(LAYER_TRANSPARENT, small_water_splash_seg4_dl_0),
    GEO_DISPLAY_LIST(LAYER_TRANSPARENT, small_water_splash_seg4_dl_1),
    GEO_DISPLAY_LIST(LAYER_TRANSPARENT, small_water_splash_seg4_dl_2),
    GEO_DISPLAY_LIST(LAYER_TRANSPARENT, small_water_splash_seg4_dl_3),
    GEO_DISPLAY_LIST(LAYER_TRANSPARENT, small_water_splash_seg4_dl_4),
    GEO_DISPLAY_LIST(LAYER_TRANSPARENT, small_water_splash_seg4_dl_5),
    GEO_CLOSE_NODE(),
    GEO_CLOSE_NODE(),
    GEO_END(),
};
