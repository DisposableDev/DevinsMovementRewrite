// 0x16000FB4
const GeoLayout wooden_signpost_geo[] = {
   GEO_SHADOW(SHADOW_CIRCLE_4_VERTS, 0xC8, 60),
   GEO_OPEN_NODE(),
      GEO_SCALE(0x00, 16384),
      GEO_OPEN_NODE(),
         GEO_DISPLAY_LIST(LAYER_OPAQUE, wooden_signpost_seg3_dl_0302DA48),
         GEO_DISPLAY_LIST(LAYER_OPAQUE, wooden_signpost_seg3_dl_0302DD08),
      GEO_CLOSE_NODE(),
   GEO_CLOSE_NODE(),
   GEO_END(),
};

const GeoLayout yoshi_credit_sign_geo[] = {
    GEO_SHADOW(SHADOW_CIRCLE_4_VERTS, 0xC8, 60),
    GEO_OPEN_NODE(),
    GEO_SCALE(0x00, 16384),
    GEO_OPEN_NODE(),
    GEO_ASM(GEO_TRANSPARENCY_MODE_NO_DITHER, geo_update_layer_transparency),
    GEO_SWITCH_CASE(2, geo_switch_anim_state),
    GEO_OPEN_NODE(),
    GEO_DISPLAY_LIST(LAYER_OPAQUE, yoshi_credit_sign_dl),
    GEO_DISPLAY_LIST(LAYER_TRANSPARENT, yoshi_credit_sign_dl),
    GEO_CLOSE_NODE(),
    GEO_CLOSE_NODE(),
    GEO_CLOSE_NODE(),
    GEO_END(),
};
