const GeoLayout castle_secret_window_geo[] = {
    GEO_NODE_START(), GEO_OPEN_NODE(), GEO_DISPLAY_LIST(LAYER_ALPHA, castle_secret_window_dl),
    GEO_CLOSE_NODE(), GEO_END(),
};

const GeoLayout castle_secret_room_geo[] = {
    // Enclose the room from its doorway origin, including views from inside it.
    GEO_CULLING_RADIUS(900),
    GEO_OPEN_NODE(),
    GEO_DISPLAY_LIST(LAYER_OPAQUE, castle_secret_room_dl),
    GEO_SWITCH_CASE(4, geo_switch_anim_state),
    GEO_OPEN_NODE(),
    GEO_DISPLAY_LIST(LAYER_OPAQUE, castle_secret_smug_bow_dl),
    GEO_NODE_START(),
    GEO_OPEN_NODE(),
    GEO_DISPLAY_LIST(LAYER_OPAQUE, castle_secret_physics_lab_dl),
    GEO_ASM(PAINTING_ID(0, 3), geo_castle_secret_painting_draw),
    GEO_CLOSE_NODE(),
    GEO_DISPLAY_LIST(LAYER_OPAQUE, castle_secret_bow_dl),
    GEO_DISPLAY_LIST(LAYER_OPAQUE, castle_secret_empty_frame_dl),
    GEO_CLOSE_NODE(),
    GEO_CLOSE_NODE(),
    GEO_END(),
};
