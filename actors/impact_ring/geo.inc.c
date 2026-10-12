// 0x0D000090
const GeoLayout invisible_bowser_accessory_geo[] = {
    GEO_CULLING_RADIUS(10000),
    GEO_OPEN_NODE(),
    GEO_ASM(GEO_TRANSPARENCY_MODE_DECAL, geo_update_layer_transparency),
    GEO_SWITCH_CASE(2, geo_switch_bowser_wave),
    GEO_OPEN_NODE(),
    GEO_DISPLAY_LIST(LAYER_TRANSPARENT_DECAL, bowser_wave_ring_dl),
    GEO_DISPLAY_LIST(LAYER_TRANSPARENT_DECAL, impact_ring_seg6_dl_0601EAC0),
    GEO_CLOSE_NODE(),
    GEO_CLOSE_NODE(),
    GEO_END(),
};
