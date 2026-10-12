// Shared Metal hand layouts. Palette-dependent bodies are instantiated below.

// 0x170018F8
const GeoLayout mario_metal_geo_face_and_wings[] = {
   GEO_ASM(0, geo_mario_head_rotation),
   GEO_ROTATION_NODE(0x00, 0, 0, 0),
   GEO_OPEN_NODE(),
      GEO_SWITCH_CASE(0, geo_switch_mario_cap_on_off),
      GEO_OPEN_NODE(),
         GEO_DISPLAY_LIST(LAYER_OPAQUE, mario_metal_cap_on_shared_dl),
         GEO_DISPLAY_LIST(LAYER_OPAQUE, mario_metal_cap_off_shared_dl),
      GEO_CLOSE_NODE(),
      GEO_TRANSLATE_ROTATE(0, 142, -51, -126, 22, -40, -135),
      GEO_OPEN_NODE(),
         GEO_ASM(0, geo_mario_rotate_wing_cap_wings),
         GEO_ROTATION_NODE(0x00, 0, 0, 0),
         GEO_OPEN_NODE(),
            GEO_DISPLAY_LIST(LAYER_ALPHA, mario_metal_cap_wings), // left
         GEO_CLOSE_NODE(),
      GEO_CLOSE_NODE(),
      GEO_TRANSLATE_ROTATE(0, 142, -51, 126, -22, 40, -135),
      GEO_OPEN_NODE(),
         GEO_ASM(1, geo_mario_rotate_wing_cap_wings),
         GEO_ROTATION_NODE(0x00, 0, 0, 0),
         GEO_OPEN_NODE(),
            GEO_DISPLAY_LIST(LAYER_ALPHA, mario_metal_cap_wings), // right
         GEO_CLOSE_NODE(),
      GEO_CLOSE_NODE(),
   GEO_CLOSE_NODE(),
   GEO_RETURN(),
};

// 0x170019A4
const GeoLayout mario_metal_geo_left_hand[] = {
   GEO_SWITCH_CASE(1, geo_switch_mario_hand),
   GEO_OPEN_NODE(),
      GEO_ANIMATED_PART(LAYER_OPAQUE, 60, 0, 0, NULL),
      GEO_OPEN_NODE(),
         GEO_ASM(1, geo_mario_hand_foot_scaler),
         GEO_SCALE(0x00, 65536),
         GEO_OPEN_NODE(),
            GEO_DISPLAY_LIST(LAYER_OPAQUE, mario_left_hand_closed_shared_dl),
         GEO_CLOSE_NODE(),
      GEO_CLOSE_NODE(),
      GEO_ANIMATED_PART(LAYER_OPAQUE, 60, 0, 0, mario_left_hand_open_shared_dl),
      GEO_ANIMATED_PART(LAYER_OPAQUE, 60, 0, 0, mario_left_hand_closed_shared_dl),
      GEO_ANIMATED_PART(LAYER_OPAQUE, 60, 0, 0, mario_left_hand_closed_shared_dl),
      GEO_ANIMATED_PART(LAYER_OPAQUE, 60, 0, 0, mario_left_hand_closed_shared_dl),
   GEO_CLOSE_NODE(),
   GEO_RETURN(),
};

// 0x17001C80
const GeoLayout mario_metal_geo_medium_poly_left_hand[] = {
   GEO_SWITCH_CASE(1, geo_switch_mario_hand),
   GEO_OPEN_NODE(),
      GEO_ANIMATED_PART(LAYER_OPAQUE, 60, 0, 0, NULL),
      GEO_OPEN_NODE(),
         GEO_ASM(1, geo_mario_hand_foot_scaler),
         GEO_SCALE(0x00, 65536),
         GEO_OPEN_NODE(),
            GEO_DISPLAY_LIST(LAYER_OPAQUE, mario_medium_poly_left_hand_closed_shared_dl),
         GEO_CLOSE_NODE(),
      GEO_CLOSE_NODE(),
      GEO_ANIMATED_PART(LAYER_OPAQUE, 60, 0, 0, mario_left_hand_open_shared_dl),
      GEO_ANIMATED_PART(LAYER_OPAQUE, 60, 0, 0, mario_medium_poly_left_hand_closed_shared_dl),
      GEO_ANIMATED_PART(LAYER_OPAQUE, 60, 0, 0, mario_medium_poly_left_hand_closed_shared_dl),
      GEO_ANIMATED_PART(LAYER_OPAQUE, 60, 0, 0, mario_medium_poly_left_hand_closed_shared_dl),
   GEO_CLOSE_NODE(),
   GEO_RETURN(),
};

// 0x17001F5C
const GeoLayout mario_metal_geo_low_poly_face_and_wings[] = {
   GEO_ASM(0, geo_mario_head_rotation),
   GEO_ROTATION_NODE(0x00, 0, 0, 0),
   GEO_OPEN_NODE(),
      GEO_SWITCH_CASE(0, geo_switch_mario_cap_on_off),
      GEO_OPEN_NODE(),
         GEO_DISPLAY_LIST(LAYER_OPAQUE, mario_metal_low_poly_cap_on),
         GEO_DISPLAY_LIST(LAYER_OPAQUE, mario_metal_low_poly_cap_off),
      GEO_CLOSE_NODE(),
      GEO_TRANSLATE_ROTATE(0, 142, -51, -126, 22, -40, -135),
      GEO_OPEN_NODE(),
         GEO_ASM(0, geo_mario_rotate_wing_cap_wings),
         GEO_ROTATION_NODE(0x00, 0, 0, 0),
         GEO_OPEN_NODE(),
            GEO_DISPLAY_LIST(LAYER_ALPHA, mario_metal_cap_wings),
         GEO_CLOSE_NODE(),
      GEO_CLOSE_NODE(),
      GEO_TRANSLATE_ROTATE(0, 142, -51, 126, -22, 40, -135),
      GEO_OPEN_NODE(),
         GEO_ASM(1, geo_mario_rotate_wing_cap_wings),
         GEO_ROTATION_NODE(0x00, 0, 0, 0),
         GEO_OPEN_NODE(),
            GEO_DISPLAY_LIST(LAYER_ALPHA, mario_metal_cap_wings),
         GEO_CLOSE_NODE(),
      GEO_CLOSE_NODE(),
   GEO_CLOSE_NODE(),
   GEO_RETURN(),
};

// 0x17002008
const GeoLayout mario_metal_geo_low_poly_left_hand[] = {
   GEO_SWITCH_CASE(1, geo_switch_mario_hand),
   GEO_OPEN_NODE(),
      GEO_ANIMATED_PART(LAYER_OPAQUE, 60, 0, 0, NULL),
      GEO_OPEN_NODE(),
         GEO_ASM(1, geo_mario_hand_foot_scaler),
         GEO_SCALE(0x00, 65536),
         GEO_OPEN_NODE(),
            GEO_DISPLAY_LIST(LAYER_OPAQUE, mario_low_poly_left_hand_closed_shared_dl),
         GEO_CLOSE_NODE(),
      GEO_CLOSE_NODE(),
      GEO_ANIMATED_PART(LAYER_OPAQUE, 60, 0, 0, mario_left_hand_open_shared_dl),
      GEO_ANIMATED_PART(LAYER_OPAQUE, 60, 0, 0, mario_low_poly_left_hand_closed_shared_dl),
      GEO_ANIMATED_PART(LAYER_OPAQUE, 60, 0, 0, mario_low_poly_left_hand_closed_shared_dl),
      GEO_ANIMATED_PART(LAYER_OPAQUE, 60, 0, 0, mario_low_poly_left_hand_closed_shared_dl),
   GEO_CLOSE_NODE(),
   GEO_RETURN(),
};

// 0x170022E4
const GeoLayout mario_metal_vanish_geo_face_and_wings[] = {
   GEO_ASM(0, geo_mario_head_rotation),
   GEO_ROTATION_NODE(0x00, 0, 0, 0),
   GEO_OPEN_NODE(),
      GEO_SWITCH_CASE(0, geo_switch_mario_cap_on_off),
      GEO_OPEN_NODE(),
         GEO_DISPLAY_LIST(LAYER_TRANSPARENT, mario_metal_cap_on_shared_dl),
         GEO_DISPLAY_LIST(LAYER_TRANSPARENT, mario_metal_cap_off_shared_dl),
      GEO_CLOSE_NODE(),
      GEO_TRANSLATE_ROTATE(0, 142, -51, -126, 22, -40, -135),
      GEO_OPEN_NODE(),
         GEO_ASM(0, geo_mario_rotate_wing_cap_wings),
         GEO_ROTATION_NODE(0x00, 0, 0, 0),
         GEO_OPEN_NODE(),
            GEO_DISPLAY_LIST(LAYER_TRANSPARENT, mario_metal_cap_wings_transparent),
         GEO_CLOSE_NODE(),
      GEO_CLOSE_NODE(),
      GEO_TRANSLATE_ROTATE(0, 142, -51, 126, -22, 40, -135),
      GEO_OPEN_NODE(),
         GEO_ASM(1, geo_mario_rotate_wing_cap_wings),
         GEO_ROTATION_NODE(0x00, 0, 0, 0),
         GEO_OPEN_NODE(),
            GEO_DISPLAY_LIST(LAYER_TRANSPARENT, mario_metal_cap_wings_transparent),
         GEO_CLOSE_NODE(),
      GEO_CLOSE_NODE(),
   GEO_CLOSE_NODE(),
   GEO_RETURN(),
};

// 0x17002390
const GeoLayout mario_metal_vanish_geo_left_hand[] = {
   GEO_SWITCH_CASE(0, geo_switch_mario_hand),
   GEO_OPEN_NODE(),
      GEO_ANIMATED_PART(LAYER_TRANSPARENT, 60, 0, 0, NULL),
      GEO_OPEN_NODE(),
         GEO_ASM(1, geo_mario_hand_foot_scaler),
         GEO_SCALE(0x00, 65536),
         GEO_OPEN_NODE(),
            GEO_DISPLAY_LIST(LAYER_TRANSPARENT, mario_left_hand_closed_shared_dl),
         GEO_CLOSE_NODE(),
      GEO_CLOSE_NODE(),
      GEO_ANIMATED_PART(LAYER_TRANSPARENT, 60, 0, 0, mario_left_hand_open_shared_dl),
      GEO_ANIMATED_PART(LAYER_TRANSPARENT, 60, 0, 0, mario_left_hand_closed_shared_dl),
      GEO_ANIMATED_PART(LAYER_TRANSPARENT, 60, 0, 0, mario_left_hand_closed_shared_dl),
      GEO_ANIMATED_PART(LAYER_TRANSPARENT, 60, 0, 0, mario_left_hand_closed_shared_dl),
   GEO_CLOSE_NODE(),
   GEO_RETURN(),
};

// 0x1700266C
const GeoLayout mario_metal_vanish_geo_medium_poly_left_hand[] = {
   GEO_SWITCH_CASE(0, geo_switch_mario_hand),
   GEO_OPEN_NODE(),
      GEO_ANIMATED_PART(LAYER_TRANSPARENT, 60, 0, 0, NULL),
      GEO_OPEN_NODE(),
         GEO_ASM(1, geo_mario_hand_foot_scaler),
         GEO_SCALE(0x00, 65536),
         GEO_OPEN_NODE(),
            GEO_DISPLAY_LIST(LAYER_TRANSPARENT, mario_medium_poly_left_hand_closed_shared_dl),
         GEO_CLOSE_NODE(),
      GEO_CLOSE_NODE(),
      GEO_ANIMATED_PART(LAYER_TRANSPARENT, 60, 0, 0, mario_left_hand_open_shared_dl),
      GEO_ANIMATED_PART(LAYER_TRANSPARENT, 60, 0, 0, mario_medium_poly_left_hand_closed_shared_dl),
      GEO_ANIMATED_PART(LAYER_TRANSPARENT, 60, 0, 0, mario_medium_poly_left_hand_closed_shared_dl),
      GEO_ANIMATED_PART(LAYER_TRANSPARENT, 60, 0, 0, mario_medium_poly_left_hand_closed_shared_dl),
   GEO_CLOSE_NODE(),
   GEO_RETURN(),
};

// 0x17002958
const GeoLayout mario_metal_vanish_geo_low_poly_face_and_wings[] = {
   GEO_ASM(0, geo_mario_head_rotation),
   GEO_ROTATION_NODE(0x00, 0, 0, 0),
   GEO_OPEN_NODE(),
      GEO_SWITCH_CASE(0, geo_switch_mario_cap_on_off),
      GEO_OPEN_NODE(),
         GEO_DISPLAY_LIST(LAYER_TRANSPARENT, mario_metal_low_poly_cap_on),
         GEO_DISPLAY_LIST(LAYER_TRANSPARENT, mario_metal_low_poly_cap_off),
      GEO_CLOSE_NODE(),
      GEO_TRANSLATE_ROTATE(0, 142, -51, -126, 22, -40, -135),
      GEO_OPEN_NODE(),
         GEO_ASM(0, geo_mario_rotate_wing_cap_wings),
         GEO_ROTATION_NODE(0x00, 0, 0, 0),
         GEO_OPEN_NODE(),
            GEO_DISPLAY_LIST(LAYER_TRANSPARENT, mario_metal_cap_wings_transparent),
         GEO_CLOSE_NODE(),
      GEO_CLOSE_NODE(),
      GEO_TRANSLATE_ROTATE(0, 142, -51, 126, -22, 40, -135),
      GEO_OPEN_NODE(),
         GEO_ASM(1, geo_mario_rotate_wing_cap_wings),
         GEO_ROTATION_NODE(0x00, 0, 0, 0),
         GEO_OPEN_NODE(),
            GEO_DISPLAY_LIST(LAYER_TRANSPARENT, mario_metal_cap_wings_transparent),
         GEO_CLOSE_NODE(),
      GEO_CLOSE_NODE(),
   GEO_CLOSE_NODE(),
   GEO_RETURN(),
};

// 0x17002A04
const GeoLayout mario_metal_vanish_geo_low_poly_left_hand[] = {
   GEO_SWITCH_CASE(0, geo_switch_mario_hand),
   GEO_OPEN_NODE(),
      GEO_ANIMATED_PART(LAYER_TRANSPARENT, 60, 0, 0, NULL),
      GEO_OPEN_NODE(),
         GEO_ASM(1, geo_mario_hand_foot_scaler),
         GEO_SCALE(0x00, 65536),
         GEO_OPEN_NODE(),
            GEO_DISPLAY_LIST(LAYER_TRANSPARENT, mario_low_poly_left_hand_closed_shared_dl),
         GEO_CLOSE_NODE(),
      GEO_CLOSE_NODE(),
      GEO_ANIMATED_PART(LAYER_TRANSPARENT, 60, 0, 0, mario_left_hand_open_shared_dl),
      GEO_ANIMATED_PART(LAYER_TRANSPARENT, 60, 0, 0, mario_low_poly_left_hand_closed_shared_dl),
      GEO_ANIMATED_PART(LAYER_TRANSPARENT, 60, 0, 0, mario_low_poly_left_hand_closed_shared_dl),
      GEO_ANIMATED_PART(LAYER_TRANSPARENT, 60, 0, 0, mario_low_poly_left_hand_closed_shared_dl),
   GEO_CLOSE_NODE(),
   GEO_RETURN(),
};

// 0x17002DD4

#define MARIO_BODY_NAME(name) name
#define MARIO_PALETTE_NAME(name) name
#include "actors/mario/palette_geo.inc.c"
#undef MARIO_PALETTE_NAME
#undef MARIO_BODY_NAME

#define MARIO_BODY_NAME(name) he_thicc_body_##name
#define MARIO_PALETTE_NAME(name) he_thicc_##name
#include "actors/mario/palette_geo.inc.c"
#undef MARIO_PALETTE_NAME
#undef MARIO_BODY_NAME

const GeoLayout mario_geo[] = {
   GEO_SHADOW(SHADOW_CIRCLE_PLAYER, 0xB4, 100),
   GEO_OPEN_NODE(),
      GEO_SCALE(0x00, 16384),
      GEO_OPEN_NODE(),
         GEO_ASM(0, geo_mirror_mario_backface_culling),
         GEO_ASM(0, geo_mirror_mario_set_alpha),
         GEO_SWITCH_CASE(4, geo_switch_mario_character),
         GEO_OPEN_NODE(),
            GEO_NODE_START(),
            GEO_OPEN_NODE(),
               GEO_SWITCH_CASE(0, geo_switch_mario_stand_run),
               GEO_OPEN_NODE(),
                  GEO_BRANCH(1, mario_geo_load_body),
                  GEO_BRANCH(1, mario_geo_render_body),
               GEO_CLOSE_NODE(),
            GEO_CLOSE_NODE(),
            GEO_NODE_START(),
            GEO_OPEN_NODE(),
               GEO_SWITCH_CASE(0, geo_switch_mario_stand_run),
               GEO_OPEN_NODE(),
                  GEO_BRANCH(1, he_thicc_mario_geo_load_body),
                  GEO_BRANCH(1, he_thicc_mario_geo_render_body),
               GEO_CLOSE_NODE(),
            GEO_CLOSE_NODE(),
            GEO_BRANCH(1, mario_geo_simple_body),
            GEO_BRANCH(1, he_thicc_mario_geo_simple_body),
         GEO_CLOSE_NODE(),
         GEO_ASM(1, geo_mirror_mario_backface_culling),
      GEO_CLOSE_NODE(),
   GEO_CLOSE_NODE(),
   GEO_END(),
};
