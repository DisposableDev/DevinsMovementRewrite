"""Production extraction and deliberate defects for the render suite."""
from .common import prepare_common_constants
import re
import runpy
from regression_support import HarnessError
from .mutation_support import MutationSource

def prepare_render(root, source, definitions, functions):
    pose = source("src/game/mario_render_pose.h")
    definitions += [pose.declaration("enum", "MarioPoseRootSpace"),
                    pose.declaration("struct", "MarioRenderPose")]
    prepare_common_constants(source, definitions, gameplay_options=True)
    definitions += [source("src/engine/surface_collision.h").declaration("enum", "RaycastFlags")]
    definitions += [source("include/sm64.h").declaration("enum", "MarioActionFlags"),
                    source("include/sm64.h").macros("ACT_WATER_IDLE|ACT_BREASTSTROKE|ACT_FLYING|ACT_IDLE|ACT_TWIRLING|ACT_DROWNING")]
    definitions += [source("include/model_ids.h").macros("MODEL_SEAWEED|MODEL_CLAM_SHELL|MODEL_JRB_ROCK")]
    # Batch cases exercise the retained opt-in ordinary-object renderer.
    definitions += ["#ifndef ORDINARY_SPRITE_BATCHING\n#define ORDINARY_SPRITE_BATCHING 1\n#endif"]
    definitions += [source("src/game/object_helpers.h").declaration("enum", "GeoUpdateLayerTransparencyModes"),
                    source("include/object_constants.h").declaration("enum", "AnimStateLayerTransparency"),
                    source("include/object_constants.h").declaration("enum", "oAnimStatesBowser"),
                    source("include/object_constants.h").declaration("enum", "oActionsBowser"),
                    source("include/object_constants.h").declaration("enum", "animIDsDoor"),
                    source("include/object_constants.h").declaration("enum", "oPlatformOnTrackTypes"),
                    source("include/mario_animation_ids.h").declaration("enum", "MarioAnimID"),
                    source("include/surface_terrains.h").declaration("enum", "SurfaceTypes"),
                    source("src/engine/surface_collision.h").macros("FLOOR_LOWER_LIMIT|FLOOR_LOWER_LIMIT_MISC")]

    functions += ["s32 mario_ground_pound_animation_render(struct AnimInfo *info, "
                  "struct MarioState *m, struct MarioRenderPose *pose) { return FALSE; }"]
    definitions += [source("src/game/mario_he_thicc.h").macros(
        "HE_THICC_.*|ACT_ARG_WALL_KICK_HE_THICC|ACT_ARG_FORWARD_ROLLOUT_HE_THICC_BOUNCE")]
    definitions += [source("src/game/mario_actions_airborne.h").macros(
        "ACT_ARG_JUMP_BACKWARD_BUTT_SLIDE.*|ACT_ARG_JUMP_KICK_FOLLOWUP_QUEUED|ACT_ARG_JUMP_KICK_BACKWARD_BUTT_SLIDE.*|ACT_ARG_STEEP_JUMP_BACKWARD_BUTT_SLIDE_NO_UPHILL|ACT_ARG_DIVE_FROM_ROLLOUT_CONTACT")]
    definitions += [source("src/game/mario_actions_stationary.h").macros(
        "ACT_ARG_COUGHING_SHALLOW_WATER")]
    definitions += [source("include/config/config_movement.h").macros("BACKWARD_BUTT_SLIDE_JUMP_AIR_DRAG|BACKWARD_JUMP_KICK_UPHILL_DECEL")]
    definitions += [source("src/game/emutest.h").declaration("enum", "Emulator")]
    definitions += [source("src/game/mario_character.h").declaration("enum", "MarioPalette")]
    simple_geo = source("actors/mario/palette_geo.inc.c").region(
        r"^const GeoLayout MARIO_PALETTE_NAME\(mario_geo_simple_body\)",
        r"\Z")
    definitions += ["\n".join((
        "typedef const void *GeoLayout;",
        "static const GeoLayout mario_geo_load_low_poly_body[] = { NULL };",
        "static const GeoLayout he_thicc_mario_geo_load_low_poly_body[] = { NULL };",
        "static const GeoLayout mario_geo_load_medium_poly_body[] = { NULL };",
        "static const GeoLayout he_thicc_mario_geo_load_medium_poly_body[] = { NULL };",
        "#define GEO_NODE_START() NULL",
        "#define GEO_OPEN_NODE() NULL",
        "#define GEO_CLOSE_NODE() NULL",
        "#define GEO_RETURN() NULL",
        "#define GEO_RENDER_RANGE(a,b) ((const void *) 2)",
        "#define GEO_BRANCH(type,target) (target)",
        "#define MARIO_PALETTE_NAME(name) name",
        simple_geo,
        "#undef MARIO_PALETTE_NAME",
        "#define MARIO_PALETTE_NAME(name) he_thicc_##name",
        simple_geo,
        "#undef MARIO_PALETTE_NAME"))]
    graph = source("src/engine/graph_node.h")
    types = source("include/types.h")
    render = source("src/game/rendering_graph_node.c")
    silhouette_map = source("actors/mario/silhouette_map.inc.c").text
    silhouette_symbols = sorted(set(re.findall(r"\b(?:mario|he_thicc)\w+", silhouette_map)))
    definitions += ["static const Gfx " + name + "[1];" for name in silhouette_symbols]
    definitions += [render.declaration("struct", "MarioSilhouetteMap"), silhouette_map]
    definitions += [render.declaration("struct", "MarioDitherMap"),
                    "static const struct MarioDitherMap sMarioDitherMap[] = {"
                    "{ (const Gfx *) 0x04012340, (const Gfx *) 0x80200000, (const Gfx *) 0x80200010 } }; "]
    definitions += [render.region(r"^static struct \{\n    const void \*displayList;\n    u8 geometry;",
                                  r"^static u8 geo_mario_silhouette_geometry")]
    math = source("src/engine/math_util.c")
    math_header = source("src/engine/math_util.h")
    definitions += [math_header.macros("ABS|CLAMP")]
    definitions += [graph.declaration("enum", name) for name in ("GraphRenderFlags", "GraphNodeTypes")]
    definitions += [source("include/sm64.h").declaration("enum", "RenderLayers")]
    definitions += [graph.region(r"^#define GRAPH_RENDER_FLAGS_SIZE", r"^enum GraphNodeTypes")]
    definitions += [types.declaration("struct", name) for name in ("GraphNode", "AnimInfo", "GraphNodeObject")]
    definitions += [types.declaration("enum", "AnimFlags"), types.declaration("struct", "Animation")]
    definitions += [types.macros("ANIMINDEX_NUMPARTS|PUNCH_STATE_TIMER_MASK"),
                    source("include/sm64.h").macros(
                        "COS15|COS10|ACT_WALKING|ACT_CROUCHING|ACT_START_CROUCHING|ACT_STOP_CROUCHING|"
                        "ACT_START_CRAWLING|ACT_STOP_CRAWLING|ACT_CRAWLING|ACT_STANDING_AGAINST_WALL|"
                        "ACT_FIRST_PERSON|ACT_JUMP"),
                    source("src/game/mario_floor_lift.inc.c").text,
                    "static f32 sMarioFloorLift;"]
    for actor, animations in (("seaweed", ("0600A4BC",)),
                              ("clam_shell", ("05001654", "0500172C"))):
        definitions += [source(f"actors/{actor}/anims/anim_{name}.inc.c").text
                        for name in animations]
        definitions += [source(f"actors/{actor}/anims/table.inc.c").text]
    flame = source("actors/flame/geo.inc.c")
    for name in ("red_flame_geo", "blue_flame_geo"):
        layout = re.search(rf"const GeoLayout {name}\[\] = \{{(.*?)\}};", flame.text, re.S)[1]
        expected = ["GEO_NODE_START", "GEO_OPEN_NODE", "GEO_SWITCH_CASE", "GEO_OPEN_NODE"]
        expected += ["GEO_DISPLAY_LIST"] * 8 + ["GEO_CLOSE_NODE", "GEO_CLOSE_NODE", "GEO_END"]
        if re.findall(r"\bGEO_\w+(?=\()", layout) != expected:
            raise HarnessError(f"{name}: flame layout changed; review the direct renderer")
    render_header = source("src/game/rendering_graph_node.h")
    definitions += [render_header.declaration("enum", "AnimType"),
                    render_header.region(r"^enum RenderModeTypes", r"^#define RENDER_PHASE_FIRST"),
                    render_header.macros("RENDER_PHASE_FIRST")]
    definitions += [source("include/sm64.h").region(r"^#define LAYER_FIRST", r"^enum MarioInput")]
    definitions += [render.declaration("struct", "RenderPhase"),
                    render.region(r"^static struct RenderPhase sRenderPhases\[\]", r"^extern const Gfx init_rsp")]
    definitions += [graph.declaration("struct", name) for name in (
        "FnGraphNode", "GraphNodeRoot", "GraphNodePerspective", "DisplayListNode", "GraphNodeLevelOfDetail",
        "GraphNodeMasterList", "GraphNodeCamera", "GraphNodeDisplayList",
        "GraphNodeCullingRadius", "GraphNodeObjectParent", "GraphNodeScale", "GraphNodeAnimatedPart",
        "GraphNodeShadow", "GraphNodeHeldObject", "GraphNodeSwitchCase", "GraphNodeGenerated")]
    # Native solid materials can carry zero shade alpha, ignored by their normal layer.
    # Lakitu's representative is unused geometry; the other five are reachable parts.
    for actor, vertex in (("goomba", "sGoombaVertexGroup4"),
                          ("bobomb", "bobomb_seg8_vertex_08022E30"),
                          ("bully", "bully_seg5_vertex_05002C68"),
                          ("lakitu_enemy", "lakitu_enemy_seg5_vertex_05013790"),
                          ("spiny", "spiny_seg5_vertex_05015888"),
                          ("spiny_egg", "spiny_egg_seg5_vertex_050149A8")):
        model = source(f"actors/{actor}/model.inc.c")
        match = re.search(rf"static const Vtx {vertex}\[\] = \{{.*?\n\}};", model.text, re.S)
        if match is None:
            raise HarnessError(f"{actor}: native fade vertices changed; review the fixture")
        definitions += [model.mapped(*match.span())]
    definitions += [source("include/config/config_graphics.h").macros("DEFAULT_CULLING_RADIUS|VERTICAL_CULLING|HD_SHADOWS|SHADOW_TEXTURE_LOD|DOOR_SCREEN_LOD")]
    particle_vertices = []
    for path in ("white_particle_small", "white_particle", "sand", "pebble"):
        model = source(f"actors/{path}/model.inc.c").text
        vertices = re.findall(r"\{\{\{\s*(-?\d+),\s*(-?\d+),\s*(-?\d+)\}", model)
        if not vertices:
            raise HarnessError(f"No particle vertices found in {path}; review vertical bounds")
        particle_vertices.extend("{" + ",".join(vertex) + "}" for vertex in vertices)
    definitions += ["static const Vec3f verticalParticleVertices[] = {" +
                    ",".join(particle_vertices) + "};"]
    definitions += [source("include/config/config_world.h").region(
        r"^#define EXTENDED_BOUNDS_MODE", r"^#define NUM_CELLS")]
    definitions += [source("include/model_ids.h").macros(
        "MODEL_(RED_FLAME|BLUE_FLAME|MIST|WHITE_PUFF|SMALL_WATER_SPLASH|SILVER_COIN|SILVER_COIN_NO_SHADOW|WATER_SPLASH|YELLOW_COIN|YELLOW_COIN_NO_SHADOW|FISH|WHITE_PARTICLE_SMALL|RED_COIN|RED_COIN_NO_SHADOW|BLUE_COIN|BLUE_COIN_NO_SHADOW|1UP|STAR|BUBBLE|PURPLE_MARBLE|SPARKLES|EXPLOSION)")]
    definitions += [source("include/macros.h").macros("ARRAY_COUNT")]
    definitions += [source("src/game/liquid_particles.h").declaration("enum", name) for name in
                    ("LiquidParticleMaterial", "WaterDropletAction")]
    definitions += ["static u8 sTextureReuseEnvironment;"]
    materials = render.function("geo_texture_reuse_display_list")
    material_symbols = sorted(set(re.findall(r"\b(?:coin_seg3_dl|water_splash_seg4_dl|mist_seg3_dl|sparkles_seg4_dl|small_water_splash_seg4_dl)_\w+", materials)))
    definitions += ["static const Gfx " + name + "[1];" for name in material_symbols]
    pairs = re.findall(r"\{\s*((?:coin_seg3_dl|water_splash_seg4_dl|mist_seg3_dl|sparkles_seg4_dl|small_water_splash_seg4_dl)_\w+),\s*(\w+)\s*\}", materials)
    definitions += ["static const struct { const Gfx *full; const Gfx *reuse; } textureReuseMaterials[] = {" +
                    ",".join("{" + full + "," + reuse + "}" for full, reuse in pairs) + "};"]
    bounds = runpy.run_path(str(root / "tools/generate_physics_lab_bounds.py"))["Bounds"]()
    tree_radii = []
    for name in ("bubbly_tree_geo", "spiky_tree_geo", "snow_tree_geo", "palm_tree_geo"):
        body = bounds.arrays[name][1]
        expected = ["GEO_CULLING_RADIUS", "GEO_OPEN_NODE", "GEO_ASM", "GEO_SWITCH_CASE",
                    "GEO_OPEN_NODE", "GEO_DISPLAY_LIST", "GEO_DISPLAY_LIST",
                    "GEO_CLOSE_NODE", "GEO_CLOSE_NODE", "GEO_END"]
        if (re.findall(r"\bGEO_\w+(?=\()", body) != expected
                or not re.search(r"GEO_CULLING_RADIUS\(800\)", body)
                or not re.search(r"GEO_ASM\(GEO_TRANSPARENCY_MODE_INTER,\s*geo_update_layer_transparency\)", body)
                or not re.search(r"GEO_SWITCH_CASE\(2,\s*geo_switch_anim_state\)", body)):
            raise HarnessError(f"{name}: tree layout changed; review early culling")
        lists = re.findall(r"GEO_DISPLAY_LIST\([^,]+,\s*(\w+)\)", body)
        tree_radii.append(max(bounds.display(display) for display in lists))
    definitions += ["static const f32 verticalTreeRadii[] = {" +
                    ",".join(f"{radius:.9f}f" for radius in tree_radii) + "};"]
    sprite_layouts = (("bubble_geo", 1), ("purple_marble_geo", 1),
                      ("sparkles_geo", 12), ("explosion_geo", 9))
    switch_radii = []
    for name, frames in sprite_layouts:
        body = bounds.arrays[name][1]
        ops = re.findall(r"\bGEO_\w+(?=\()", body)
        expected = ["GEO_SWITCH_CASE", "GEO_OPEN_NODE"] + ["GEO_DISPLAY_LIST"] * frames
        expected += ["GEO_CLOSE_NODE", "GEO_END"]
        if name == "explosion_geo":
            expected = ["GEO_NODE_START", "GEO_OPEN_NODE"] + expected[:-1] + ["GEO_CLOSE_NODE", "GEO_END"]
        if ops != expected or not re.search(
                rf"GEO_SWITCH_CASE\(\s*{frames}\s*,\s*geo_switch_anim_state\s*\)", body):
            raise HarnessError(f"{name}: sprite layout changed; review early culling")
        lists = re.findall(r"GEO_DISPLAY_LIST\([^,]+,\s*(\w+)\)", body)
        switch_radii.append(max(bounds.display(display) for display in lists))
    definitions += ["static const f32 verticalSwitchRadii[] = {" +
                    ",".join(f"{radius:.9f}f" for radius in switch_radii) + "};"]
    definitions += [source("src/game/compact_explosion_bubbles.h").macros(
        "COMPACT_EXPLOSION_BUBBLE_CAPACITY")]
    definitions += [math_header.macros("vec[234].*|linear_mtxf_.*|MTXF_END|NEAR_ZERO|FLOAT_ONE|mtxf_billboard|sqr")]
    definitions += [render.region(r"^#define BUBBLE_BATCH_CAPACITY\b",
                                  r"^static void geo_flush_bubble_batch")]
    definitions += [render.region(r"^#if defined\(SHADOW_TEXTURE_REUSE\) && !defined\(DISABLE_SHADOWS\)\nstatic struct",
                                  r"^struct RenderModeContainer renderModeTable_1Cycle").replace(
        "#if defined(SHADOW_TEXTURE_REUSE) && !defined(DISABLE_SHADOWS)\n", "").replace("#endif", "")]
    shadow = source("src/game/shadow.c")
    definitions += [source("src/game/shadow.h").declaration("enum", "ShadowType"),
                    source("src/game/shadow.h").declaration("enum", "ShadowSolidity"),
                    source("src/game/shadow.h").declaration("struct", "Shadow"),
                    source("src/game/shadow.h").region(r"^typedef struct \{", r"^extern struct Shadow"),
                    shadow.region(r"^static ShadowRectangle sShadowRectangles", r"^struct Shadow gCurrShadow"),
                    shadow.macros("MARIO_SHADOW_.*OFFSET.*"),
                    shadow.macros("MARIO_WALL_SHADOW_.*")]
    shadow_vertices = re.findall(r"\{\{\{\s*(-?\d+),\s*(-?\d+),\s*(-?\d+)",
                                source("bin/segment2.c").region(r"^static const Vtx vertex_shadow\[", r"^const Gfx dl_shadow_end"))
    if not shadow_vertices:
        raise HarnessError("Missing shadow vertices; review shadow footprint culling")
    definitions += ["static const Vec3f shadowFootprintVertices[] = {" +
                    ",".join("{" + ",".join(v) + "}" for v in shadow_vertices) + "};"]

    definitions += [render.macros("COLLECTIBLE_SILHOUETTE_.*|MARIO_SILHOUETTE_TINT_GRAY|SIL_CVG_THRESHOLD")]
    definitions += [render.region(r"^static const Mtx sCollectibleSilhouetteInset",
                                  r"^#define SIL_CVG_THRESHOLD")]
    definitions += [source("include/n64/PR/gbi.h").macros(
        "AA_EN|IM_RD|CLR_ON_CVG|CVG_DST_WRAP|CVG_X_ALPHA|FORCE_BL|ZMODE_XLU|Z_CMP|Z_UPD|"
        "GBL_c[12]|G_BL_CLR_FOG|G_BL_CLR_IN|G_BL_CLR_MEM|G_BL_A_FOG|G_BL_1MA|G_BL_0|G_BL_1|"
        "CVG_DST_SAVE|CVG_DST_CLAMP|CVG_DST_FULL|ZMODE_OPA|ZMODE_DEC|ALPHA_CVG_SEL|G_BL_A_IN|G_BL_A_MEM|"
        "RM_OPA_SURF|G_RM_OPA_SURF2?|RM_ZB_OPA_SURF|G_RM_ZB_OPA_SURF2?|"
        "RM_(AA_)?(ZB_)?XLU_(SURF|DECAL)")]
    # Preserve the header's conditional AA aliases instead of flattening both branches.
    definitions += [source("include/n64/PR/gbi.h").region(
        r"^#ifdef DISABLE_AA\n#define AA_DEF", r"^#define\s+GBL_c1"),
        source("include/n64/PR/gbi.h").region(
        r"^#ifdef DISABLE_AA\n\n#define\s+G_RM_AA_ZB_OPA_SURF", r"^#define\s+G_RM_RA_OPA_SURF\b")]
    functions += [source("src/game/object_helpers.c").function(name) for name in (
        "geo_update_layer_transparency", "geo_update_small_water_splash_opacity", "geo_switch_anim_state", "obj_set_blink_phase", "cur_obj_hide", "cur_obj_unhide", "cur_obj_wait_then_blink")]
    functions += [source("src/game/obj_behaviors.c").function(name) for name in (
        "obj_flicker_and_disappear", "set_object_visibility")]
    functions += [source("src/game/behaviors/breakable_box_small.inc.c").function(
        "breakable_box_small_released_loop")]
    # Compare generated sprite vertices with the actual model, not a copied oracle.
    actor = source("actors/white_particle_small/model.inc.c")
    definitions += [source("src/game/liquid_particles.h").macros("LIQUID_PARTICLE_(COLD|LAVA)_[RGB]")]
    definitions += [actor.region(r"^static const Vtx white_particle_small_vertex\[",
                                r"^static const Vtx white_particle_small_unused_vertex")]
    definitions += [actor.region(r"^static const Vtx lava_droplet_vertex\[",
                                r"^const Gfx lava_droplet_dl")]
    fish = source("actors/blue_fish/anims/anim_0301C298.inc.c")
    definitions += [fish.mapped(0, len(fish.text))]
    layout = source("actors/blue_fish/geo.inc.c").text.split("const GeoLayout fish_geo[] = {", 1)[1]
    scales = re.findall(r"GEO_SCALE\(0x00,\s*(\d+)\)", layout)
    parts = re.findall(r"GEO_ANIMATED_PART\((\w+),\s*(-?\d+),\s*(-?\d+),\s*(-?\d+),\s*(\w+)\)", layout)
    if (len(scales) != 1 or len(parts) != 3
            or [p[4] for p in parts] != ["NULL", "blue_fish_seg3_dl_0301BFB8", "blue_fish_seg3_dl_0301C150"]
            or any(p[0] != "LAYER_OCCLUDE_SILHOUETTE_OPAQUE" for p in parts)
            or re.findall(r"\bGEO_\w+", layout) != ["GEO_SCALE", "GEO_OPEN_NODE",
                "GEO_ANIMATED_PART", "GEO_OPEN_NODE", "GEO_ANIMATED_PART", "GEO_OPEN_NODE",
                "GEO_ANIMATED_PART", "GEO_CLOSE_NODE", "GEO_CLOSE_NODE", "GEO_CLOSE_NODE", "GEO_END"]):
        raise HarnessError("Aquarium fish hierarchy changed; review the renderer fixture")
    definitions += [f"static const f32 fishModelScale = {scales[0]}.0f / 65536.0f;",
                    "static const Vec3s fishModelOffsets[] = {" +
                    ",".join("{" + ",".join(p[1:4]) + "}" for p in parts) + "};"]
    functions += [math.function(name) for name in (
        "mtxf_identity", "mtxf_translate", "mtxf_rotate_zxy_and_translate",
        "mtxf_scale_vec3f", "mtxf_billboard_generic", "mtxf_to_mtx_fast",
        "mtxf_rotate_xyz_and_translate_and_mul")]
    functions += [math_header.function("mtxf_to_mtx")]
    functions += [source("src/engine/graph_node.c").function(name) for name in (
        "retrieve_animation_index", "geo_update_animation_frame")]
    functions += [render.function("geo_set_animation_globals").replace(
        "void geo_set_animation_globals(", "static void real_geo_set_animation_globals(")]
    definitions += [render.region(r"^static s32 sJrbBayVisible", r"^static void geo_jrb_prepare_occluders"),
                    render.macros("SEAWEED_LOD_MAX_PIXELS|SEAWEED_LOD_MIN_DEPTH|JRB_SHIP_LOD_.*")]
    definitions += ["static s32 sJrbCaveVisible = TRUE;"]
    definitions += [source("src/game/bowser_arena.h").region(r"^#ifdef VERSION_EU", r"^#include")]
    functions += [render.function(name) for name in (
        "geo_jrb_object_outside_vertical_view", "geo_jrb_near_cave",
        "geo_jrb_prepare_occluders", "geo_jrb_object_behind_terrain",
        "geo_jrb_tunnel_exit_in_view", "geo_jrb_bay_terrain_region",
        "geo_switch_jrb_bay", "geo_switch_jrb_bay_terrain",
        "geo_switch_jrb_cave", "geo_switch_jrb_ship_lod", "geo_switch_seaweed_lod")]
    functions += [render.function(name) for name in (
        "geo_flush_bubble_batch", "geo_collectible_silhouette_alpha", "geo_mario_silhouette_geometry",
        "geo_mario_dither_material",
        "geo_texture_reuse_display_list", "geo_draw_texture_reuse_display_list",
        "geo_object_allows_texture_reuse", "geo_object_blend_alpha", "geo_is_1up_sprite_display_list",
        "geo_draw_1up_cutout_display_list",
        "geo_draw_1up_silhouette_display_list",
        "geo_draw_blinking_display_list",
        "geo_append_display_list", "geo_process_generated_list", "geo_door_uses_far_mesh", "geo_process_level_of_detail", "inc_mat_stack",
        "append_dl_and_return", "geo_mario_floor_lift", "geo_process_scale", "geo_process_animated_part",
        "geo_process_master_list", "geo_is_simple_small_sprite", "geo_is_aquarium_decoration",
        "geo_aquarium_decoration_in_view", "geo_bubble_batch_positions",
        "geo_try_batch_bubble", "geo_render_compact_explosion_bubbles",
        "geo_shadow_uses_small_texture", "geo_shadow_outside_vertical_view", "geo_tree_transparency_node", "geo_particle_outside_vertical_view", "geo_camera_box_in_view",
        "geo_advance_offscreen_animation", "obj_is_in_view", "geo_try_draw_bowser_flame", "geo_process_object",
        "geo_process_object_parent", "geo_append_shadow_display_list", "geo_shadow_keeps_distance",
        "geo_shadow_solidity", "geo_process_shadow")]
    functions[0:0] = [source("src/game/mario_dither.h").function(name) for name in (
        "mario_dither_uses_noise", "mario_dither_alpha")]
    functions += [shadow.function(name) for name in (
        "scale_shadow_with_distance", "dim_shadow_with_distance", "init_shadow",
        "linearly_interpolate_solidity_positive", "linearly_interpolate_solidity_negative",
        "correct_shadow_solidity_for_animations", "correct_lava_shadow_height", "add_shadow_to_display_list",
        "create_mario_wall_shadow")]
    functions += [shadow.function("create_shadow_below_xyz").replace(
        "Gfx *create_shadow_below_xyz(", "static Gfx *real_create_shadow_below_xyz(", 1)]
    definitions += [types.declaration("struct", "MarioBodyState"),
                    source("include/sm64.h").declaration("enum", "ModelStates"),
                    render.declaration("struct", "GeoAnimState"), math.declaration("struct", "CopyMat4")]
    definitions += [source("include/mario_geo_switch_case_ids.h").declaration("enum", "MarioCapBlinkState")]
    functions += [source("src/game/mario_misc.c").function(name) for name in (
        "make_gfx_mario_alpha", "geo_mirror_mario_set_alpha", "geo_switch_mario_cap_effect")]

    functions += [render.region(r"^static s32 sMarioCapBlinkOverlay;", r"^Mat4 gCameraTransform;")]
    functions += [math.function("mtxf_copy"), math.function("mtxf_mul"),
                  render.function("geo_process_held_object")]
    functions += [render.region(r"^#define SIL_CVG_THRESHOLD", r"^#undef SCHWA")]
    functions += [render.region(r"^static const Gfx dl_mario_silhouette_begin\[", r"^#endif\n\nstruct RenderPhase")]
    functions += [render.function("geo_process_master_list_sub").replace(
        "void geo_process_master_list_sub(", "static void real_geo_process_master_list_sub(", 1)]
    definitions += [render.macros("NO_CULLING_EMULATOR_WHITELIST|SHADOW_TEXTURE_LOD_MAX_PIXELS|DOOR_SCREEN_LOD_.*")]

def mutations_render(root):
    path = "src/game/mario_misc.c"
    yield "render", "cap_skip_opacity_blend", "render.cap_blink_opacity", {
        path: MutationSource(root / path).replace_in_function("geo_mirror_mario_set_alpha",
            "if (bodyState->capBlinkBlend == MARIO_CAP_BLINK_OPACITY)", "if (FALSE)")}
    path = "src/game/rendering_graph_node.c"
    checks = [
        ("instant_cap_geometry", "render.cap_blink_rendering", "geo_process_switch",
         "&& gBodyStates[node->numCases].capBlinkBlend == MARIO_CAP_BLINK_GEOMETRY", "&& FALSE"),
        ("cap_double_held_object", "render.cap_blink_rendering", "geo_process_held_object",
         "if (sMarioCapBlinkOverlay)", "if (FALSE && sMarioCapBlinkOverlay)"),
        ("cap_skip_pose_rewind", "render.cap_blink_rendering", "geo_process_cap_blink",
         "gCurrAnimAttribute = animAttribute;", "(void) animAttribute;"),
        ("cap_brighten_fades", "render.cap_blink_rendering", "geo_process_cap_blink",
         "overlayAlpha = (poweredAlpha + 1) / 2;", "overlayAlpha = (poweredAlpha + 1) / 3;"),
        ("zero_blink_alpha", "render.object_blink_rendering", "geo_draw_blinking_display_list",
         "0, 0, 0, list->blinkAlpha", "0, 0, 0, 0"),
        ("hide_dim_blink", "render.object_blink_rendering", "geo_process_object",
         "&& !(node->activeFlags & ACTIVE_FLAG_BLINKING)", "&& TRUE"),
        ("batch_dim_blink", "render.object_blink_rendering", "geo_try_batch_bubble",
         "if (node->activeFlags & ACTIVE_FLAG_BLINKING)", "if (FALSE)"),
        ("leak_blink_render_mode", "render.object_blink_rendering", "geo_draw_blinking_display_list",
         "gDPSetRenderMode(head++, mode1, mode2);", "gDPSetRenderMode(head++, mode1 | Z_UPD, mode2);"),
        ("lose_blink_texture_edges", "render.object_blink_rendering", "geo_draw_blinking_display_list",
         "CVG_DST_WRAP | CVG_X_ALPHA | FORCE_BL", "CVG_DST_WRAP | FORCE_BL"),
        ("bright_blink_silhouette", "render.object_blink_rendering", "geo_append_display_list",
         "blinkAlpha ? MIN(alpha, blinkAlpha) : alpha", "alpha"),
        ("convert_unused_node_matrices", "render.visible_fish_matrices", "inc_mat_stack",
         "gMatStackFixed[gMatStackIndex] = NULL;",
         "Mtx *mtx = alloc_display_list(sizeof(*mtx));\n"
         "    mtxf_to_mtx(mtx, gMatStack[gMatStackIndex]);\n"
         "    gMatStackFixed[gMatStackIndex] = mtx;"),
        ("reuse_sibling_matrix", "render.matrix_stack_reuse", "inc_mat_stack",
         "gMatStackFixed[gMatStackIndex] = NULL;", "(void)0;"),
        ("skip_bubble_batching", "render.batch_sizes", "geo_process_object",
         "if (geo_try_batch_bubble(node, position))", "if (FALSE && geo_try_batch_bubble(node, position))"),
        ("reorder_bubble_draws", "render.draw_order", "geo_append_display_list",
         "geo_flush_bubble_batch();", "(void)0;"),
        ("load_unused_bubble_vertices", "render.batch_sizes", "geo_flush_bubble_batch",
         "remainder * 4, 0", "16, 0"),
        ("drop_vertical_decoration_culling", "render.culling_edges", "geo_aquarium_decoration_in_view",
         "absf(node->cameraToObject[1])", "absf(0.0f)"),
        ("cull_intersecting_decoration_spheres", "render.culling_edges", "geo_aquarium_decoration_in_view",
         "radius * (1.0f + absf(horizontal))", "radius"),
    ]
    for name, case, function, before, after in checks:
        yield "render", name, case, {path: MutationSource(root / path).replace_in_function(function, before, after)}
    helper_path = "src/game/object_helpers.c"
    yield "render", "restore_binary_blink", "render.object_blink_phases", {
        helper_path: MutationSource(root / helper_path).replace_in_function("obj_set_blink_phase",
            "dim && obj->activeFlags != ACTIVE_FLAG_DEACTIVATED", "FALSE")}
