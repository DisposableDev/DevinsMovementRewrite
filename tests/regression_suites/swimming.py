"""Production extraction and deliberate defects for the swimming suite."""
from .common import prepare_common_constants
import re
import hashlib
from regression_support import HarnessError, Source
from .mutation_support import MutationSource

def prepare_swimming(root, source, definitions, functions):
    prepare_common_constants(source, definitions, gameplay_options=True, cap_functions=functions)
    definitions += [source("include/n64/PR/os_cont.h").macros("CONT_L|L_TRIG")]
    fall = source("src/game/mario_actions_airborne.c")
    definitions += [fall.declaration("enum", name) for name in ("HandstandFallState", "FallDamageSeverity")]
    definitions += [source("include/config/config_movement.h").macros("FALL_DAMAGE_HEIGHT_.*")]
    definitions += [source("src/game/mario.h").declaration("enum", "MarioSpawnFallProtection")]
    functions += [fall.function(name) for name in (
        "handstand_jump_protects_from_fall", "is_large_fall_at_height",
        "fall_damage_severity_at_height", "mario_would_take_fall_damage_on_water")]

    definitions += ["#ifndef COLLISION_RESULT_CACHES\n#define COLLISION_RESULT_CACHES 1\n#endif",
                    "#ifndef MARIO_WALL_CANDIDATE_CACHE\n#define MARIO_WALL_CANDIDATE_CACHE 1\n#endif"]
    definitions += ["#ifndef STATIC_WALL_HEIGHT_GROUPS\n"
                    + source("include/config/config_collision.h").macros("STATIC_WALL_HEIGHT_GROUPS")
                    + "\n#endif"]
    query = source("src/engine/surface_query.h")
    query = Source(query.path, query.replace_in_function(
        "surface_query_next_in_bounds", "while (iterator->remaining >= STATIC_SURFACE_GROUP_SIZE) {",
        "while (!bypassStaticGroups && iterator->remaining >= STATIC_SURFACE_GROUP_SIZE) {"))
    # This fixture exercises the retained opt-in caches and packed index.
    definitions += ["#ifndef STATIC_SURFACE_INDEX\n#define STATIC_SURFACE_INDEX 1\n#endif", "#ifndef DYNAMIC_SURFACE_INDEX\n"
                    + source("include/config/config_collision.h").macros("DYNAMIC_SURFACE_INDEX")
                    + "\n#endif", query.macros("DYNAMIC_SURFACE_INDEX_CELLS|DYNAMIC_SURFACE_INDEX_BYTES"),
                    query.declaration("struct", "SurfaceQueryBounds"),
                    query.declaration("struct", "DynamicSurfaceIndexCell"),
                    query.declaration("struct", "DynamicSurfaceIndex"),
                    query.declaration("enum", "SurfaceQueryStorage"),
                    query.macros("STATIC_SURFACE_GROUP_SIZE"),
                    query.declaration("struct", "StaticSurfaceIndex"),
                    query.declaration("struct", "SurfaceQueryIterator"),
                    source("include/macros.h").macros("ALIGN")]
    definitions += [query.macros("STATIC_WALL_HEIGHT_.*"),
                    query.declaration("struct", "StaticWallHeightGroup"),
                    query.declaration("struct", "StaticWallHeightCell"),
                    query.declaration("struct", "StaticWallHeightIndex")]
    loading = source("src/engine/surface_load.c")
    functions += [loading.function("invalidate_static_wall_height_index"),
                  "#if STATIC_WALL_HEIGHT_GROUPS\n"
                  + loading.function("static_wall_height_list_group_count") + "\n"
                  + loading.function("build_static_wall_height_index") + "\n#endif"]
    functions += ["#if STATIC_SURFACE_INDEX || DYNAMIC_SURFACE_INDEX\n"
                  + loading.function("extend_surface_query_bounds").replace(
                      "static ALWAYS_INLINE", "ALWAYS_INLINE", 1) + "\n#endif"]
    loading = Source(loading.path, loading.replace_in_function(
        "build_dynamic_surface_index_cell", "gDynamicSurfaceIndex.used = start + bytes;",
        "gDynamicSurfaceIndex.used = start + bytes;\n    dynamicIndexBuildCount++;\n"
        "    dynamicIndexReferenceCount += count;"))
    functions += ["#if DYNAMIC_SURFACE_INDEX\n"
                  + loading.function("invalidate_dynamic_surface_index") + "\n"
                  + loading.function("build_dynamic_surface_index_cell") + "\n#endif"]
    query = Source(query.path, query.replace_in_function(
        "surface_query_cell", "if (iterator.next.node != NULL && iterator.next.node->next != NULL) {",
        "if (!bypassDynamicIndex && iterator.next.node != NULL && iterator.next.node->next != NULL) {"))
    functions += ["#if STATIC_SURFACE_INDEX\n"
                  + source("src/engine/surface_load.c").function("build_static_surface_index")
                  + "\n#endif"]
    # This fixture's ALWAYS_INLINE already includes static linkage.
    functions += [query.function(name).replace("static ALWAYS_INLINE", "ALWAYS_INLINE", 1)
                  for name in ("surface_query_list", "surface_query_is_empty", "surface_query_has_multiple",
                               "surface_query_identity", "surface_query_cell_is_empty", "surface_query_cell", "surface_query_next",
                               "surface_query_next_in_bounds", "static_wall_height_groups",
                               "surface_query_next_wall")]
    functions += [
        "void object_platform_motion_before_step(struct Object *obj) {}",
        "s32 object_platform_motion_supported(struct Object *obj) { "
        "return obj->oPosY <= obj->oFloorHeight + 4.0f && obj->oVelY <= 0.0f; }",
    ]
    definitions += [source("src/game/mario_he_thicc.h").macros(
        "HE_THICC_.*|ACT_ARG_WALL_KICK_HE_THICC|ACT_ARG_FORWARD_ROLLOUT_HE_THICC_BOUNCE")]
    definitions += [source("src/game/mario_actions_airborne.h").macros(
        "ROLLOUT_WATER_WALK_DELAY_FRAMES|ACT_ARG_JUMP_BACKWARD_BUTT_SLIDE.*|ACT_ARG_JUMP_KICK_FOLLOWUP_QUEUED|ACT_ARG_JUMP_KICK_BACKWARD_BUTT_SLIDE.*|ACT_ARG_STEEP_JUMP_BACKWARD_BUTT_SLIDE_NO_UPHILL|ACT_ARG_DIVE_FROM_ROLLOUT_CONTACT")]
    definitions += [source("src/game/mario_actions_stationary.h").macros(
        "ACT_ARG_COUGHING_SHALLOW_WATER")]
    definitions += [source("include/config/config_movement.h").macros("BACKWARD_BUTT_SLIDE_JUMP_AIR_DRAG|BACKWARD_JUMP_KICK_UPHILL_DECEL")]
    lift = source("levels/bitfs/elevator/collision.inc.c").text
    lift_vertices = re.findall(r"COL_VERTEX\((-?\d+),\s*(-?\d+),\s*(-?\d+)\)", lift)
    lift_triangles = re.findall(r"COL_TRI\((\d+),\s*(\d+),\s*(\d+)\)", lift)
    if len(lift_vertices) != 8 or len(lift_triangles) != 14:
        raise HarnessError("Fire Sea elevator mesh changed; review its rising-floor fixture")
    definitions += ["static Vec3s bitfsLiftTriangles[][3] = {" + ",".join(
        "{" + ",".join("{" + ",".join(lift_vertices[int(index)]) + "}"
                        for index in face) + "}" for face in lift_triangles) + "};"]
    for name, path, expected in (
            ("bowserArena", "levels/bowser_2/tilting_platform/collision.inc.c", 32),
            ("ttcPlatform", "levels/ttc/elevator_platform/collision.inc.c", 12)):
        mesh = source(path).mask
        vertices = re.findall(r"COL_VERTEX\(\s*(-?\d+),\s*(-?\d+),\s*(-?\d+)\)", mesh)
        faces = re.findall(r"COL_TRI\((\d+),\s*(\d+),\s*(\d+)\)", mesh)
        if len(faces) != expected:
            raise HarnessError(f"{name} mesh changed; review platform entry fixtures")
        definitions += [f"static Vec3s {name}Triangles[][3] = {{" + ",".join(
            "{" + ",".join("{" + ",".join(vertices[int(index)]) + "}"
                            for index in face) + "}" for face in faces) + "};"]
    definitions += [source("src/game/mario_character.h").declaration("enum", "MarioPalette")]
    definitions += [source("src/game/mario_actions_submerged.c").macros("HE_THICC_WATER_SINK_SPEED")]
    definitions += [source("src/engine/math_util.h").macros("vec3_copy|vec3f_copy|vec3_copy_y_off")]
    world = source("include/config/config_world.h")
    definitions += [world.region(r"^#define EXTENDED_BOUNDS_MODE", r"^STATIC_ASSERT"),
                    world.macros("NUM_CELLS|GET_CELL_COORD")]
    definitions += [source("include/object_constants.h").macros("MONTY_MOLE_ACT_.*|MONTY_MOLE_BP_.*")]
    definitions += [source("include/sm64.h").macros("COS78")]
    definitions += [source("src/game/mario_moving_collision_math.h").declaration("struct", name)
                    for name in ("MovingBodyProfile", "MovingContactPlane")]
    definitions += [source("src/game/mario_moving_collision_math.h").declaration("enum", "MovingCastResult")]
    definitions += [source("include/object_constants.h").declaration("enum", "ObjMoveFlags")]
    definitions += [source("include/object_constants.h").macros("OBJ_MOVE_MASK_.*")]
    definitions += [source("src/engine/graph_node.h").declaration("enum", "GraphRenderFlags")]
    definitions += [source("src/game/interaction.h").declaration("enum", "InteractStatus")]
    definitions += [source("src/game/object_list_processor.c").macros("GROUND_TWIRL_KNOCKBACK_GRAVITY")]
    helpers = source("src/game/object_helpers.c")
    # The fixture sizes a collision-independent test array in both cache modes.
    definitions += [helpers.macros("INACTIVE_OBJECT_FLOOR_CACHE_SIZE")]
    definitions += [helpers.region(r"^#if COLLISION_RESULT_CACHES\n#define INACTIVE_OBJECT_FLOOR_CACHE_SIZE",
                                  r"^static void cur_obj_update_floor\(void\)")]
    math_path = "src/game/mario_moving_collision_math.c"
    math_source = source(math_path)
    # Observe actual elimination work inside the production algorithm.
    math_source = Source(root / math_path, math_source.replace_in_function("moving_solve",
        "for (s32 column = 0; column < size; column++) {",
        "stunnedLinearSolveCount++;\n    for (s32 column = 0; column < size; column++) {"))
    math_source = Source(root / math_path, math_source.replace_in_function("moving_triangle_normal",
        "Vec3f a, b;", "Vec3f a, b;\n    stunnedTriangleNormalCount++;"))
    functions += [math_source.region(r"^#define MOVING_CONTACT_SKIN", r"\Z")]
    functions += [source("src/engine/math_util.c").function("abs_angle_diff")]
    # Count expensive casts without replacing their collision algorithm.
    functions += ["#define moving_triangle_cast counted_stunned_triangle_cast\n"]
    functions += [helpers.function(name) for name in (
        "cur_obj_update_floor_height_and_get_floor", "cur_obj_update_floor", "apply_drag_to_value",
        "cur_obj_detect_steep_floor", "cur_obj_resolve_wall_collisions",
        "cur_obj_update_floor_and_resolve_wall_collisions", "cur_obj_update_floor_and_walls",
        "cur_obj_apply_drag_xz", "cur_obj_compute_vel_xz", "clear_move_flag",
        "cur_obj_move_update_ground_air_flags", "cur_obj_move_update_underwater_flags",
        "sweep_object_motion", "object_resolve_platform_motion", "cur_obj_move_stunned")]
    functions += ["#undef moving_triangle_cast\n"]
    functions += [helpers.function(name) for name in (
        "cur_obj_move_xz", "cur_obj_move_y_and_get_water_level", "cur_obj_move_y",
        "cur_obj_move_standard")]
    functions += [source("src/game/behaviors/bowser_flame.inc.c").function(name) for name in (
        "bowser_flame_has_fixed_arena_support", "bowser_flame_has_fixed_arena_floor",
        "bowser_flame_update_floor", "bowser_flame_move_on_fixed_arena_floor",
        "bowser_flame_update_movement")]
    functions += [source("src/game/behaviors/monty_mole.inc.c").function(name) for name in (
        "monty_mole_act_rise_from_hole", "monty_mole_act_spawn_rock",
        "monty_mole_act_begin_jump_into_hole", "monty_mole_act_throw_rock",
        "monty_mole_act_jump_into_hole", "monty_mole_hide_in_hole",
        "monty_mole_act_hide", "monty_mole_act_jump_out_of_hole", "bhv_monty_mole_update")]
    functions += [source("src/game/object_list_processor.c").function(name) for name in (
        "stop_ground_twirl_stunned_enemy", "ground_twirl_support_raised_enemy",
        "update_ground_twirl_stun_motion", "update_ground_twirl_stunned_object",
        "update_object_with_ground_twirl_stun")]
    sm64 = source("include/sm64.h")
    collision = source("src/engine/surface_collision.c")
    collision = Source(collision.path, collision.replace_in_function("find_vertical_surface_precise",
        "surface = surface_query_next_in_bounds(&lists[i], minimum, maximum)) {",
        "surface = surface_query_next_in_bounds(&lists[i], minimum, maximum)) {\n            waterVerticalNodeCount++;"))
    collision = Source(collision.path, collision.replace_in_function("find_vertical_surface_cached",
        "if (cache == NULL || cache->count < 0) {",
        "if (cache == NULL || cache->count < 0 || bypassWaterQueries) {"))
    collision = Source(collision.path, collision.replace_in_function("find_floor_from_list",
        "while ((surf = surface_query_next(&candidates)) != NULL) {",
        "while ((surf = surface_query_next(&candidates)) != NULL) {\n        floorNodeVisits++;"))
    definitions += [collision.region(r"^#if COLLISION_RESULT_CACHES\n#define STATIC_FLOOR_CACHE_SIZE", r"^static struct Surface \*find_static_floor_from_list")]
    header = source("src/engine/surface_collision.h")
    math = source("src/engine/math_util.c")
    water = source("src/game/mario_water_collision.c")
    # Target stack placement does not change the extracted host semantics.
    water = Source(water.path, water.text.replace(
        "static __attribute__((noinline)) u32 perform_cached_water_collision_step(",
        "static u32 perform_cached_water_collision_step(", 1))
    water = Source(water.path, water.replace_in_function(
        "find_water_crossed_surface",
        "surface = surface_query_next_in_bounds(&lists[list], minimum, maximum)) {",
        "surface = surface_query_next_in_bounds(&lists[list], minimum, maximum)) {\n"
        "                            waterEdgeNodeCount++;"))
    water = Source(water.path, water.replace_in_function(
        "reuse_water_contacts", "if (contacts == NULL) {",
        "if (contacts == NULL || bypassWaterContacts) {\n"
        "        if (contacts != NULL) { contacts->count = 0; }"))
    math = Source(root / "src/engine/math_util.c", math.replace_in_function(
        "find_surface_on_ray", "Vec3f normalized_dir;",
        "Vec3f normalized_dir;\n    waterRaycastCount++;"))
    math = Source(math.path, math.replace_in_function(
        "ray_surface_intersect", "// Convert the vertices to Vec3f.",
        "waterRayTriangleCount++;\n    // Convert the vertices to Vec3f."))
    math = Source(math.path, math.replace_in_function(
        "find_surface_on_ray_candidates_internal", "// Reject surface if out of vertical bounds",
        "waterRayNodeCount++;\n        // Reject surface if out of vertical bounds"))
    # Bypass only the new broad phase to compare the same ray and movement
    # implementations, including hit identity and floating-point results.
    math = Source(math.path, math.replace_in_function(
        "find_surface_on_ray_candidates_internal",
        "if (flags & RAYCAST_MARIO_WALL) {\n            s32 outside",
        "if (!bypassWaterRayBounds && (flags & RAYCAST_MARIO_WALL)) {\n            s32 outside"))
    definitions += [sm64.declaration("enum", name) for name in (
        "WaterStep", "AirStep", "AirStepCheck", "MarioFlags", "MarioActionFlags", "MarioActionGroups", "MarioInput", "MarioParticleFlags")]
    definitions += [source("src/game/interaction.h").declaration("enum", "Interactions")]
    definitions += [source("include/config/config_movement.h").macros("WALL_KICK_DEGREES")]
    definitions += [sm64.macros("ACT_.*")]
    definitions += [sm64.declaration("enum", "GroundStep"), sm64.macros("COS73")]
    definitions += [source("src/game/object_list_processor.h").declaration("enum", "CollisionFlags")]
    definitions += [source("include/surface_terrains.h").declaration("enum", name)
                    for name in ("SurfaceTypes", "SurfaceFlags")]
    definitions += [source("include/object_constants.h").declaration("enum", "ObjActiveFlags")]
    definitions += [header.declaration("enum", "RaycastFlags"),
                    header.declaration("struct", "WallCollisionData"),
                    header.declaration("struct", "WallCollisionCacheCell"),
                    header.declaration("struct", "WallCollisionCache")]
    definitions += [header.macros("SURFACE_QUERY_CACHE_SIZE"),
                    header.declaration("struct", "SurfaceQueryCacheEntry"),
                    header.declaration("struct", "SurfaceQueryCache")]
    definitions += [header.macros("FIND_FLOOR_BUFFER|CELL_HEIGHT_LIMIT|FLOOR_LOWER_LIMIT(_MISC)?|is_outside_level_bounds|get_surface_height_at_location")]
    definitions += [source("src/engine/surface_load.h").declaration("enum", "SpatialPartitions"),
                    source("src/engine/surface_load.h").macros("NORMAL_FLOOR_THRESHOLD")]
    definitions += [water.macros("WATER_COLLISION_.*"),
                    water.declaration("struct", "WaterContactState"),
                    water.declaration("struct", "WaterPreparationGeometry"),
                    water.declaration("struct", "WaterWallCandidates")]
    definitions += [source("include/config/config_movement.h").macros(
        "MARIO_GLOBAL_SPEED_CAP|MAX_SWIM_SPEED|SWIM_SOFT_SPEED_LIMIT|SWIM_MAX_PITCH_DEGREES|"
        "SWIM_PITCH_SPEED_DEGREES|A_SWIM_SPEED|A_SWIM_ACCEL|B_SWIM_YAW_SPEED|SWIM_YAW_SPEED|WATER_DOWN_BURST_VERTICAL_BRAKE")]
    definitions += [source("include/config/config_collision.h").macros("FAST_FLOOR_ALIGN")]
    # Use the current JRB static triangles, with checked counts rather than a
    # copied level mesh. Object collision and the level loader stay outside
    # this host fixture.
    level = source("levels/jrb/areas/1/collision.inc.c")
    vertices = re.findall(r"COL_VERTEX\(\s*(-?\d+),\s*(-?\d+),\s*(-?\d+)\)", level.mask)
    vertex_count = re.search(r"COL_VERTEX_INIT\((0x[0-9A-Fa-f]+|\d+)\)", level.mask)
    triangles, expected_triangles = [], 0
    kind = None
    for line in level.mask.splitlines():
        group = re.search(r"COL_TRI_INIT\((\w+),\s*(\d+)\)", line)
        if group:
            kind = group[1]
            expected_triangles += int(group[2])
        triangle = re.search(r"COL_TRI\((\d+),\s*(\d+),\s*(\d+)\)", line)
        if triangle:
            if kind is None or any(int(v) >= len(vertices) for v in triangle.groups()):
                raise HarnessError("JRB collision fixture has invalid triangle data")
            triangles.append((*triangle.groups(), kind))
    if (vertex_count is None or len(vertices) != int(vertex_count[1], 0)
            or len(triangles) != expected_triangles or not triangles or len(triangles) > 600):
        raise HarnessError("JRB collision layout changed; update the swimming fixture")
    definitions += ["static Vec3s jrbVertices[] = {" +
                    ",".join("{" + ",".join(v) + "}" for v in vertices) + "};",
                    "static int jrbTriangles[][4] = {" +
                    ",".join("{" + ",".join(t) + "}" for t in triangles) + "};"]
    rock = source("levels/jrb/rock/collision.inc.c")
    rock_vertices = re.findall(r"COL_VERTEX\(\s*(-?\d+),\s*(-?\d+),\s*(-?\d+)\)", rock.mask)
    rock_triangles = re.findall(r"COL_TRI\(\s*(\d+),\s*(\d+),\s*(\d+)\)", rock.mask)
    if (len(rock_vertices) != 12 or len(rock_triangles) != 20
            or len(triangles) + len(rock_triangles) > 600
            or any(int(i) >= len(rock_vertices) for t in rock_triangles for i in t)):
        raise HarnessError("JRB object rock layout changed; review swimming escape fixture")
    placements = re.findall(r"MODEL_JRB_ROCK,\s*(-?\d+),\s*(-?\d+),\s*(-?\d+),\s*0,\s*(\d+),\s*0",
                           source("levels/jrb/script.c").mask)
    if not placements:
        raise HarnessError("JRB object rock placements are missing")
    definitions += ["static Vec3s swimmingRockVertices[] = {" +
                    ",".join("{" + ",".join(v) + "}" for v in rock_vertices) + "};",
                    "static int swimmingRockTriangles[][3] = {" +
                    ",".join("{" + ",".join(t) + "}" for t in rock_triangles) + "};",
                    "static const s32 swimmingRockPlacements[][4] = {" +
                    ",".join("{" + ",".join(p) + "}" for p in placements) + "};"]
    # Both collision objects of the act-1 sunken ship, including its warp
    # opening. Read mesh, placement and rotation from the live assets.
    ship = source("levels/jrb/wooden_ship/collision.inc.c")
    ship_vertices, ship_triangles = [], []
    for name in ("jrb_seg7_collision_in_sunken_ship", "jrb_seg7_collision_in_sunken_ship_2"):
        match = re.search(r"const Collision " + name + r"\[\]\s*=\s*\{(.*?)\};", ship.mask, re.S)
        if match is None:
            raise HarnessError("Sunken ship collision array missing")
        body = match[1]
        verts = re.findall(r"COL_VERTEX\(\s*(-?\d+),\s*(-?\d+),\s*(-?\d+)\)", body)
        count = re.search(r"COL_VERTEX_INIT\((0x[\da-fA-F]+|\d+)\)", body)
        offset, expected, added, kind = len(ship_vertices), 0, 0, None
        for line in body.splitlines():
            group = re.search(r"COL_TRI_INIT\((\w+),\s*(\d+)\)", line)
            if group:
                kind = group[1]
                expected += int(group[2])
            tri = re.search(r"COL_TRI\((\d+),\s*(\d+),\s*(\d+)\)", line)
            if tri:
                if kind is None or any(int(v) >= len(verts) for v in tri.groups()):
                    raise HarnessError("Invalid sunken ship triangle")
                ship_triangles.append((*[str(int(v) + offset) for v in tri.groups()], kind))
                added += 1
        if count is None or len(verts) != int(count[1], 0) or added != expected:
            raise HarnessError("Sunken ship collision layout changed")
        ship_vertices += verts
    script = source("levels/jrb/script.c").mask
    ship_positions = re.findall(r"OBJECT_WITH_ACTS\(\s*MODEL_NONE,\s*(-?\d+),\s*(-?\d+),\s*(-?\d+),"
        r"\s*0,\s*0,\s*0,\s*0x00000000,\s*bhvInSunkenShip2?,\s*ACT_1\)", script)
    rotation = re.search(r"bhvSunkenShipSetRotation\[\]\s*=\s*\{(.*?)\};",
                         source("data/behavior_data.c").mask, re.S)
    angles = re.findall(r"SET_INT\(oFaceAngle(?:Pitch|Yaw|Roll),\s*(0x[\da-fA-F]+)\)", rotation[1]) if rotation else []
    if (len(ship_positions) != 2 or ship_positions[0] != ship_positions[1] or len(angles) != 3
            or len(triangles) + len(ship_triangles) > 700):
        raise HarnessError("Sunken ship placement changed; review swimming fixture")
    definitions += ["static Vec3s swimmingShipVertices[] = {" +
                    ",".join("{" + ",".join(v) + "}" for v in ship_vertices) + "};",
                    "static int swimmingShipTriangles[][4] = {" +
                    ",".join("{" + ",".join(t) + "}" for t in ship_triangles) + "};",
                    "static Vec3f swimmingShipPosition = {" + ",".join(ship_positions[0]) + "};",
                    "static Vec3s swimmingShipRotation = {" + ",".join(angles) + "};"]
    definitions += [source("src/engine/math_util.h").macros(
        "linear_mtxf_mul_vec3|linear_mtxf_mul_vec3_and_translate")]
    functions += [math.function("mtxf_rotate_zxy_and_translate")]
    # Recorded ship contact points used the fixture's libm rotation. Preserve
    # their horizontal position relative to the live, approximated ship mesh.
    reference_matrix = math.function("mtxf_rotate_zxy_and_translate")
    reference_matrix = reference_matrix.replace("mtxf_rotate_zxy_and_translate", "ship_fixture_reference_matrix")
    for axis, sine, cosine in ((0, "sx", "cx"), (1, "sy", "cy"), (2, "sz", "cz")):
        pair = f"sincos_bhaskara(rot[{axis}], &{sine}, &{cosine});"
        if pair not in reference_matrix:
            raise HarnessError("Matrix trig changed; review ship contact fixture")
        reference_matrix = reference_matrix.replace(
            pair, f"{sine} = sins(rot[{axis}]);\n    {cosine} = coss(rot[{axis}]);")
    functions += [reference_matrix]
    # Actual full-size normal sculpture, translated to the fixture origin.
    # The former Vanish specimen has been removed. Tests explicitly assign
    # Vanish surfaces to this identical template and exercise all three sizes.
    lab = source("levels/physics_lab/collision.inc.c").text.split("};", 1)[0]
    lab_vertices = [tuple(map(int, p)) for p in re.findall(
        r"COL_VERTEX\((-?\d+), (-?\d+), (-?\d+)\)", lab)]
    lab_group = lab.split("COL_TRI_INIT(SURFACE_DEFAULT,", 1)[1].split("COL_TRI_INIT", 1)[0]
    uneven_triangles = []
    for indices in re.findall(r"COL_TRI\((\d+), (\d+), (\d+)\)", lab_group):
        points = [lab_vertices[int(i)] for i in indices]
        if all(-9500 <= x <= -7900 and -4300 <= z <= 500 and 0 <= y <= 310
               for x, y, z in points):
            uneven_triangles.append("{" + ",".join(f"{{{x},{y},{z}}}" for x,y,z in points) + "}")
    if len(uneven_triangles) != 192:
        raise HarnessError("Uneven Lab strip changed; review airborne landing fixtures")
    definitions += ["static Vec3s labUnevenTriangles[][3] = {" + ",".join(uneven_triangles) + "};"]
    pool_triangles = []
    for indices in re.findall(r"COL_TRI\((\d+), (\d+), (\d+)\)", lab_group):
        points = [lab_vertices[int(i)] for i in indices]
        if all(3150 <= x <= 5850 and 7250 <= z <= 10250 and 0 <= y <= 240
               for x, y, z in points):
            pool_triangles.append("{" + ",".join(f"{{{x},{y},{z}}}" for x,y,z in points) + "}")
    if not pool_triangles:
        raise HarnessError("Shallow Lab pool fixture has no triangles")
    definitions += ["static Vec3s labPoolTriangles[][3] = {" + ",".join(pool_triangles) + "};"]
    stair_triangles = []
    for indices in re.findall(r"COL_TRI\((\d+), (\d+), (\d+)\)", lab_group):
        points = [lab_vertices[int(i)] for i in indices]
        staircase = all(13800 <= x <= 15000 and -1560 <= z <= 2200 and 0 <= y <= 1280
                        for x, y, z in points)
        underlying_floor = (all(y == 0 for x, y, z in points)
                            and max(x for x, y, z in points) >= 13800
                            and min(x for x, y, z in points) <= 15000
                            and max(z for x, y, z in points) >= -1560
                            and min(z for x, y, z in points) <= 2200)
        if staircase or underlying_floor:
            stair_triangles.append("{" + ",".join(f"{{{x},{y},{z}}}" for x, y, z in points) + "}")
    if not stair_triangles:
        raise HarnessError("Lab stairs fixture has no triangles")
    definitions += ["static Vec3s labStairTriangles[][3] = {" + ",".join(stair_triangles) + "};"]
    ttm = source("levels/ttm/areas/1/collision.inc.c")
    ttm_vertices = [tuple(map(int, p)) for p in re.findall(
        r"COL_VERTEX\(\s*(-?\d+),\s*(-?\d+),\s*(-?\d+)\)", ttm.mask)]
    cave_triangles = []
    kind = None
    for line in ttm.mask.splitlines():
        group = re.search(r"COL_TRI_INIT\((\w+),", line)
        if group:
            kind = group[1]
        face = re.search(r"COL_TRI\((\d+),\s*(\d+),\s*(\d+)\)", line)
        if face:
            points = [ttm_vertices[int(i)] for i in face.groups()]
            if (max(p[0] for p in points) >= 1200 and min(p[0] for p in points) <= 2350
                    and max(p[2] for p in points) >= 500 and min(p[2] for p in points) <= 1650):
                points_c = "{" + ",".join(f"{{{x},{y},{z}}}" for x, y, z in points) + "}"
                cave_triangles.append("{" + points_c + "," + kind + "}")
    if len(cave_triangles) != 83:
        raise HarnessError("TTM cave fixture changed; review the box departure regression")
    definitions += ["static struct { Vec3s vertices[3]; s16 type; } ttmCaveTriangles[] = {"
                    + ",".join(cave_triangles) + "};"]
    box = source("actors/breakable_box/collision.inc.c")
    box_vertices = re.findall(r"COL_VERTEX\(\s*(-?\d+),\s*(-?\d+),\s*(-?\d+)\)", box.mask)
    box_faces = re.findall(r"COL_TRI\((\d+),\s*(\d+),\s*(\d+)\)", box.mask)
    definitions += ["static Vec3s caveBoxVertices[] = {"
                    + ",".join("{" + ",".join(p) + "}" for p in box_vertices) + "};",
                    "static s16 caveBoxFaces[][3] = {"
                    + ",".join("{" + ",".join(p) + "}" for p in box_faces) + "};"]
    castle = source("levels/castle_grounds/areas/1/collision.inc.c").text
    castle_vertices = [tuple(map(int, p)) for p in re.findall(
        r"COL_VERTEX\(\s*(-?\d+),\s*(-?\d+),\s*(-?\d+)\)", castle)]
    castle_roof = []
    castle_cliff = []
    castle_kind = None
    for line in castle.splitlines():
        group = re.search(r"COL_TRI_INIT\((\w+),", line)
        if group:
            castle_kind = group[1]
        face = re.search(r"COL_TRI\((\d+),\s*(\d+),\s*(\d+)\)", line)
        if face:
            points = [castle_vertices[int(i)] for i in face.groups()]
            if (min(p[1] for p in points) >= 2339 and max(p[1] for p in points) <= 3174
                    and max(abs(p[0]) for p in points) < 4500):
                points_c = "{" + ",".join(f"{{{x},{y},{z}}}" for x, y, z in points) + "}"
                castle_roof.append("{" + points_c + "," + castle_kind + "}")
            if ((max(p[0] for p in points) <= -5000 and max(p[2] for p in points) <= -4000
                 or min(p[0] for p in points) < -5000 and min(p[2] for p in points) < -4000
                 and max(p[1] for p in points) < 2000)
                    and castle_kind in ("SURFACE_VERY_SLIPPERY", "SURFACE_NOISE_SLIPPERY")):
                points_c = "{" + ",".join(f"{{{x},{y},{z}}}" for x, y, z in points) + "}"
                castle_cliff.append("{" + points_c + "," + castle_kind + "}")
    definitions += ["static struct { Vec3s vertices[3]; s16 type; } castleOuterRoofTriangles[] = {"
                    + ",".join(castle_roof) + "};"]
    definitions += ["static struct { Vec3s vertices[3]; s16 type; } castleCliffTriangles[] = {"
                    + ",".join(castle_cliff) + "};"]
    ramp_triangles = []
    upper_ramp_triangles = []
    kind = None
    for line in lab.splitlines():
        group = re.search(r"COL_TRI_INIT\((\w+),", line)
        if group:
            kind = group[1]
        face = re.search(r"COL_TRI\((\d+),\s*(\d+),\s*(\d+)\)", line)
        if face:
            points = [lab_vertices[int(i)] for i in face.groups()]
            if (max(p[0] for p in points) >= -13400 and min(p[0] for p in points) <= -13000
                    and max(p[2] for p in points) >= -12200
                    and min(p[2] for p in points) <= -11800):
                points_c = "{" + ",".join(f"{{{x},{y},{z}}}" for x, y, z in points) + "}"
                upper_ramp_triangles.append("{" + points_c + "," + kind + "}")
            if (max(p[0] for p in points) >= 11000 and min(p[0] for p in points) <= 12500
                    and max(p[2] for p in points) >= -4500
                    and min(p[2] for p in points) <= -3000):
                points_c = "{" + ",".join(f"{{{x},{y},{z}}}" for x, y, z in points) + "}"
                ramp_triangles.append("{" + points_c + "," + kind + "}")
    # The original 32 faces are unchanged; the finishing galleries add 61
    # nearby slab, return-ramp and support-column faces. Retain these solids
    # in the live-mesh fixture, and guard coordinates, winding and types too.
    ramp_digest = hashlib.sha256(",".join(ramp_triangles).encode()).hexdigest()
    if (len(ramp_triangles) != 93
            or ramp_digest != "78786c867c215b3b108fe88e23df96911aa655693c0ab85fe07908705749e921"):
        raise HarnessError("Pictured Lab ramp changed; review the side-entry regression")
    definitions += ["static struct { Vec3s vertices[3]; s16 type; } labSideRampTriangles[] = {"
                    + ",".join(ramp_triangles) + "};"]
    if hashlib.sha256(",".join(upper_ramp_triangles).encode()).hexdigest() != "ded0bca1b5424280dcd5187b92df2462147b7d23a3c95176d9d8a58a3421de22":
        raise HarnessError("Upper Lab ramp changed; review diagonal falling contacts")
    definitions += ["static struct { Vec3s vertices[3]; s16 type; } labUpperRampTriangles[] = {"
                    + ",".join(upper_ramp_triangles) + "};"]
    bridge = source("levels/physics_lab/collision.inc.c").text.split(
        "const Collision physics_lab_bridge_collision[] = {", 1)[1].split("COL_TRI_STOP()", 1)[0]
    bridge_vertices = re.findall(r"COL_VERTEX\((-?\d+),\s*(-?\d+),\s*(-?\d+)\)", bridge)
    bridge_faces = re.findall(r"COL_TRI\((\d+),\s*(\d+),\s*(\d+)\)", bridge)
    definitions += ["static Vec3s labBridgeTriangles[][3] = {" + ",".join(
        "{" + ",".join("{" + ",".join(bridge_vertices[int(i)]) + "}" for i in face) + "}"
        for face in bridge_faces) + "};"]
    bridge_roll = re.search(r"oFaceAngleRoll = (0x[0-9a-fA-F]+) \* sins\(phase\)",
                            source("src/game/behaviors/physics_lab.inc.c").text)
    if bridge_roll is None:
        raise HarnessError("Lab bridge roll changed; review its landing regression")
    definitions += ["#define LAB_BRIDGE_ROLL_AMPLITUDE " + bridge_roll[1]]
    definitions += [source("include/config/config_movement.h").macros(
        "MARIO_MAX_FALL_SPEED|HE_THICC_MAX_FALL_SPEED")]
    bridge_matrix = source("src/engine/math_util.c").function("mtxf_rotate_zxy_and_translate")
    bridge_matrix = bridge_matrix.replace("mtxf_rotate_zxy_and_translate", "lab_bridge_matrix")
    functions += [bridge_matrix]
    slope_triangles = []
    kind = None
    for line in lab.splitlines():
        group = re.search(r"COL_TRI_INIT\((\w+),", line)
        if group:
            kind = group[1]
        face = re.search(r"COL_TRI\((\d+), (\d+), (\d+)\)", line)
        if face:
            points = [lab_vertices[int(i)] for i in face.groups()]
            if (max(p[0] for p in points) >= -6300 and min(p[0] for p in points) <= -5200
                    and max(p[2] for p in points) >= -4100 and min(p[2] for p in points) <= -1300):
                points_c = "{" + ",".join(f"{{{x},{y},{z}}}" for x,y,z in points) + "}"
                slope_triangles.append("{" + points_c + "," + kind + "}")
    if len(slope_triangles) != 66:
        raise HarnessError("Standard Lab slope changed; review grounded-dive fixtures")
    definitions += ["static struct { Vec3s vertices[3]; s16 type; } labSlopeTriangles[] = {"
                    + ",".join(slope_triangles) + "};"]
    bob = source("levels/bob/areas/1/collision.inc.c")
    bob_vertices = re.findall(r"COL_VERTEX\(\s*(-?\d+),\s*(-?\d+),\s*(-?\d+)\)", bob.mask)
    bob_triangles = []
    kind = None
    for line in bob.mask.splitlines():
        group = re.search(r"COL_TRI_INIT\((\w+),", line)
        if group:
            kind = group[1]
        face = re.search(r"COL_TRI\((\d+),\s*(\d+),\s*(\d+)\)", line)
        if face:
            if kind is None or any(int(i) >= len(bob_vertices) for i in face.groups()):
                raise HarnessError("Invalid Bob-omb Battlefield triangle")
            points = "{" + ",".join("{" + ",".join(bob_vertices[int(i)]) + "}"
                                     for i in face.groups()) + "}"
            bob_triangles.append("{" + points + "," + kind + "}")
    if len(bob_vertices) != 570 or len(bob_triangles) != 1060:
        raise HarnessError("Bob-omb Battlefield mesh changed; review grounded-dive fixtures")
    definitions += ["static struct { Vec3s vertices[3]; s16 type; } bobTriangles[] = {"
                    + ",".join(bob_triangles) + "};"]
    ddd = source("levels/ddd/areas/2/collision.inc.c")
    cage_vertices = re.findall(r"COL_VERTEX\(\s*(-?\d+),\s*(-?\d+),\s*(-?\d+)\)", ddd.mask)
    cage_triangles = []
    kind = None
    for line in ddd.mask.splitlines():
        group = re.search(r"COL_TRI_INIT\((\w+),", line)
        if group:
            kind = group[1]
        face = re.search(r"COL_TRI\((\d+),\s*(\d+),\s*(\d+)\)", line)
        if face:
            vertices = [cage_vertices[int(i)] for i in face.groups()]
            if all(1300 < int(x) < 2700 and -4200 < int(y) < -3100
                   and -3450 < int(z) < -2000 for x, y, z in vertices):
                points = "{" + ",".join("{" + ",".join(v) + "}" for v in vertices) + "}"
                cage_triangles.append("{" + points + "," + kind + "}")
    definitions += ["static struct { Vec3s vertices[3]; s16 type; } dddCageTriangles[] = {"
                    + ",".join(cage_triangles) + "};"]
    lab_triangles = []
    for indices in re.findall(r"COL_TRI\((\d+), (\d+), (\d+)\)", lab_group):
        points = [lab_vertices[int(i)] for i in indices]
        if any(y>0 for x,y,z in points) and all(abs(x+11100)<=930 and abs(z-7900)<=950 for x,y,z in points):
            lab_triangles.append("{" + ",".join(f"{{{x+11100},{y},{z-7900}}}" for x,y,z in points) + "}")
    if len(lab_triangles) != 126:
        raise HarnessError("Sculpture fixture changed; review the collision tests")
    definitions += ["static Vec3s vanishSculpture[][3] = {" + ",".join(lab_triangles) + "};"]
    # Keep the court at its real world coordinates: short wall ends and
    # long faces here exposed persistent airborne stun stalls.
    court_triangles = []
    kind = None
    for line in lab.splitlines():
        group = re.search(r"COL_TRI_INIT\((\w+),", line)
        if group:
            kind = group[1]
        triangle = re.search(r"COL_TRI\((\d+), (\d+), (\d+)\)", line)
        if triangle:
            points = [lab_vertices[int(i)] for i in triangle.groups()]
            if (max(p[0] for p in points) >= 800 and min(p[0] for p in points) <= 7000
                    and max(p[2] for p in points) >= -13300 and min(p[2] for p in points) <= -10200):
                if kind is None:
                    raise HarnessError("Enemy court fixture has no surface type")
                points_c = "{" + ",".join(f"{{{x},{y},{z}}}" for x,y,z in points) + "}"
                court_triangles.append("{" + points_c + "," + kind + "}")
    # The accepted enemy layout retains the original walls and adds equal
    # Goomba cells plus shared flush approach lines. Keep this guard exact.
    if len(court_triangles) != 353:
        raise HarnessError("Enemy court fixture changed; review the stun lifecycle tests")
    definitions += ["static struct { Vec3s vertices[3]; s16 type; } enemyCourtTriangles[] = {"
                    + ",".join(court_triangles) + "};"]
    functions += [collision.function(name).replace("static ALWAYS_INLINE", "ALWAYS_INLINE", 1)
                  for name in ("check_wall_vw", "check_wall_edge", "surface_is_passable_vanish")]
    wall_solver = source("src/engine/surface_wall.inc.c").text
    functions += ["#define WALL_HEIGHT_GROUP_SCAN 0\n" + wall_solver
                  + "\n#undef WALL_HEIGHT_GROUP_SCAN\n#if STATIC_WALL_HEIGHT_GROUPS\n"
                  + "#define WALL_HEIGHT_GROUP_SCAN 1\n" + wall_solver
                  + "\n#undef WALL_HEIGHT_GROUP_SCAN\n#endif"]
    functions += [collision.function("find_wall_collisions_in_cells")]
    # The source extractor accepts plain function prefixes; noinline affects
    # target code placement, not this host fixture's collision semantics.
    grouped_collision = Source(collision.path, collision.text.replace(
        "static __attribute__((noinline)) s32 find_wall_collisions_in_cells_with_height_groups(",
        "static s32 find_wall_collisions_in_cells_with_height_groups(", 1))
    functions += ["#if STATIC_WALL_HEIGHT_GROUPS\n"
                  + grouped_collision.function("find_wall_collisions_in_cells_with_height_groups")
                  + "\n#endif"]
    functions += [collision.function(name).replace("static ALWAYS_INLINE", "ALWAYS_INLINE", 1) for name in (
        "dispatch_wall_collisions_in_cells", "find_wall_collisions", "f32_find_wall_collision",
        "resolve_and_return_wall_collisions", "find_wall_collisions_with_cache",
        "resolve_and_return_wall_collisions_cached", "add_ceil_margin",
        "check_within_ceil_triangle_bounds", "find_ceil_from_list", "find_ceil",
        "check_within_floor_triangle_bounds", "floor_height_cutoff", "find_floor_from_list", "find_static_floor_from_list")]
    # Match this fixture's ALWAYS_INLINE linkage while keeping the bounds helper
    # after the floor/ceiling tests and before its water callers.
    functions += [collision.function("check_within_bounds_y_norm").replace(
        "ALWAYS_INLINE static", "ALWAYS_INLINE", 1)]
    functions += [collision.function(name).replace("static ALWAYS_INLINE", "ALWAYS_INLINE", 1)
                  for name in (
        "find_water_floor_from_list", "find_water_floor",
        "find_floor", "find_floor_excluding_objects",
        "check_within_surface_triangle_bounds_f32", "floor_height_at_xz", "get_air_step_floor_start",
        "find_vertical_surface_precise", "find_vertical_surface_cache_entry",
        "find_vertical_surface_cached",
        "find_floor_precise_cached", "find_floor_precise", "find_floor_for_air_step_with_obstacle",
        "find_floor_for_air_step",
        "find_air_step_floor_contact", "find_ceil_precise_cached", "find_ceil_precise")]
    # The same production query, with only the static cache bypassed, supplies
    # the reference result and operation count for cache equivalence checks.
    functions += [collision.function("find_floor").replace("f32 find_floor(", "static f32 uncached_find_floor(", 1)
                  .replace("find_static_floor_from_list(surfaceList,", "find_floor_from_list(surfaceList,")]
    functions += [source("src/engine/surface_load.c").function(name)
                  for name in ("lower_cell_index", "upper_cell_index")]
    functions += [header.function("find_mario_ceil")]
    functions += [math.function(name).replace("static ALWAYS_INLINE", "ALWAYS_INLINE", 1)
                  for name in (
        "ray_surface_intersect", "find_surface_on_ray_candidates_internal",
        "find_surface_on_ray_candidates", "find_surface_on_ray_static_walls",
        "find_surface_on_ray_cell", "find_surface_on_ray")]
    functions += [source("src/game/mario_collision.c").region(
        r"^static s32 mario_wall_overlaps_xz_box", r"\Z")]
    functions += [water.function(name) for name in (
        "project_water_velocity", "project_water_contact_velocity", "merge_water_step_result",
        "find_water_surface_edge", "find_water_crossed_surface",
        "water_contact_contains", "remember_water_contact", "reuse_water_contacts",
        "sweep_water_movement", "water_position_has_clearance",
        "water_movement_reduces_overlap",
        "recover_water_overlap", "water_wall_contact_contains", "project_water_wall_increment",
        "filter_water_wall_list", "prepare_water_wall_candidates",
        "resolve_water_wall_push", "prepare_water_geometry",
        "mario_water_collision_prepare", "mario_water_collision_prepare_geometry",
        "perform_water_substep", "perform_water_collision_step",
        "perform_bbh_water_collision_step", "perform_cached_water_collision_step",
        "mario_water_collision_step_with_floor", "mario_water_collision_step")]
    functions += [source("src/game/mario.c").function(name) for name in (
        "mario_limit_velocity_vector", "mario_limit_velocity", "mario_set_forward_vel", "find_floor_slope",
        "resolve_mario_geometry_walls")]
    step = source("src/game/mario_step.c")
    active, count = re.subn(r"#ifdef HANGING_FIX\n(.*?)#else\n.*?#endif", r"\1", step.text, flags=re.S)
    if count != 1 or not re.search(r"^#define HANGING_FIX\b", source("include/config/config_movement.h").mask, re.M):
        raise HarnessError("Slope adapter expects enabled HANGING_FIX; review alternate branch")
    step = Source(step.path, active)
    definitions += [step.macros("MARIO_GROUND_STEP_MAX_FLOOR_DELTA|MARIO_GROUND_LOW_STEP_MAX_HEIGHT|MARIO_WATER_WALK_.*")]
    functions += [step.function(name) for name in (
        "mario_water_walk_conditions_met", "mario_tick_water_walking_cooldown", "mario_update_water_walking_state", "mario_can_walk_on_water", "mario_is_on_floor", "mario_can_step_past_lower_walls", "perform_ground_quarter_step",
        "bonk_or_hit_lava_wall", "perform_air_step_ceiling_collision", "mario_can_step_onto_air_floor",
        "perform_air_quarter_step")]
    definitions += [source("src/game/mario_actions_submerged.c").macros("SWIMMING_ROLL_.*")]
    definitions += [source("include/config/config_movement.h").macros("WALL_KICK_DEGREES")]
    functions += [source("src/game/mario_actions_submerged.c").function(name) for name in (
        "bounce_swimming_roll", "perform_water_step", "is_normal_swimming_action",
        "swimming_near_surface", "get_buoyancy", "update_swimming_vertical_velocity",
        "update_swimming_speed", "update_swimming_yaw", "update_swimming_pitch",
        "adjust_swimming_render_pose", "surface_swim_bob", "common_swimming_step")]

def mutations_swimming(root):
    yield "swimming", "erase_slope_top_with_wall_push", "swimming.air_thin_slope_top", {
        "src/game/mario_step.c": MutationSource(root / "src/game/mario_step.c").replace_in_function(
            "perform_air_quarter_step", "&& !mario_air_step_crosses_wall(m, topContact)",
            "&& FALSE")}
    flame_path = "src/game/behaviors/bowser_flame.inc.c"
    flame = MutationSource(root / flame_path)
    yield "swimming", "settled_fire_keeps_duplicate_search", "swimming.settled_flame_work", {
        flame_path: flame.replace_in_function("bowser_flame_update_movement",
            "if (ordinaryQuery &&", "if (FALSE && ordinaryQuery &&")}
    yield "swimming", "settled_fire_skips_vertical_step", "swimming.settled_flame_floor_changes", {
        flame_path: flame.replace_in_function("bowser_flame_update_movement",
            "cur_obj_move_y(o->oGravity, o->oBounciness, o->oBuoyancy);", "(void) o;")}
    helper_path = "src/game/object_helpers.c"
    helper = MutationSource(root / helper_path)
    for name, case, before, after in (
        ("repeat_inactive_floor_queries", "inactive_object_floor_work",
         "&& ordinaryQuery", "&& FALSE && ordinaryQuery"),
        ("reuse_inactive_floor_after_move", "inactive_object_floor_refresh",
         "entry->pos[0] == o->oPosX", "TRUE"),
        ("reuse_inactive_floor_after_reload", "inactive_object_floor_refresh",
         "entry->generation == gStaticSurfaceGeneration", "TRUE"),
        ("reuse_special_inactive_floor", "inactive_object_floor_refresh",
         "entry->object = ordinaryQuery ? o : NULL;", "entry->object = o;"),
        ("miss_new_inactive_dynamic_support", "inactive_object_dynamic_floor",
         "&& surface_query_cell_is_empty(GET_CELL_COORD(o->oPosX), GET_CELL_COORD(o->oPosZ),\n"
         "                                       SPATIAL_PARTITION_FLOORS, SURFACE_QUERY_DYNAMIC)", "&& TRUE"),
    ):
        yield "swimming", name, "swimming." + case, {
            helper_path: helper.replace_in_function("cur_obj_update_floor", before, after)}
    collision_path = "src/engine/surface_collision.c"
    collision = MutationSource(root / collision_path)
    for name, case, before, after in (
        ("repeat_static_floor_scans", "static_floor_cache_work", "entry->list == list", "FALSE && entry->list == list"),
        ("reuse_floor_previous_frame", "static_floor_cache_lifecycle", "entry->frame == gGlobalTimer", "TRUE"),
        ("reuse_floor_previous_terrain", "static_floor_cache_lifecycle",
         "entry->generation == gStaticSurfaceGeneration", "TRUE"),
        ("reuse_floor_other_eligibility", "static_floor_cache_lifecycle", "entry->ignoreGrates == ignoreGrates", "TRUE"),
        ("reuse_floor_other_dynamic_height", "static_floor_cache_lifecycle", "entry->minimumHeight == *height", "TRUE"),
        ("cache_special_floor_queries", "static_floor_cache_equivalence", "gCollisionFlags != 0", "FALSE"),
    ):
        yield "swimming", name, "swimming." + case, {
            collision_path: collision.replace_in_function("find_static_floor_from_list", before, after)}
    query_path = "src/engine/surface_query.h"
    yield "swimming", "use_stale_static_index", "swimming.static_index_lifecycle", {
        query_path: MutationSource(root / query_path).replace_in_function("surface_query_cell",
            "gStaticSurfaceIndex.generation == gStaticSurfaceGeneration", "TRUE")}
    load_path = "src/engine/surface_load.c"
    yield "swimming", "repeat_first_static_reference", "swimming.indexed_collision_equivalence", {
        load_path: MutationSource(root / load_path).replace_in_function("build_static_surface_index",
            "surfaces[count++] = surface;",
            "surfaces[count++] = gStaticSurfacePartition[z][x][partition]->surface;")}
    for name, condition in (("scan_disjoint_static_groups", "TRUE ||"),
                            ("discard_reachable_static_groups", "FALSE &&")):
        yield "swimming", name, "swimming.jrb_grouped_travel", {
            query_path: MutationSource(root / query_path).replace_in_function(
                "surface_query_next_in_bounds", "if (!(bounds->minimum[0] > maximum[0]",
                "if (" + condition + " !(bounds->minimum[0] > maximum[0]")}
    water_path = "src/game/mario_water_collision.c"
    yield "swimming", "consume_water_top_scan", "swimming.water_top_bottom_scan", {
        collision_path: collision.replace_in_function("find_water_floor_from_list",
            "    // SURFACE_NEW_WATER\n", "    // SURFACE_NEW_WATER\n    tops = bottoms;\n")}
    yield "swimming", "hide_support_floor_checks", "swimming.platform_support_query", {
        collision_path: collision.replace_in_function("find_floor_excluding_objects",
            "PUPPYPRINT_ADD_COUNTER(gPuppyCallCounter.collision_floor);", "(void) 0;")}
    air_path = "src/game/mario_step.c"
    rounded_start = MutationSource(root / air_path).replace_in_function("perform_air_quarter_step",
        "get_air_step_floor_start(m->pos, m->floor, m->floorHeight, floorStart);",
        "vec3f_copy(floorStart, m->pos);")
    yield "swimming", "restore_grounded_dive_penetration", "swimming.air_floor_grounded_dives", {
        air_path: rounded_start}
    yield "swimming", "restore_bob_false_bonks", "swimming.air_floor_bob_takeoffs", {
        air_path: rounded_start}
    yield "swimming", "treat_falling_as_grounded", "swimming.air_floor_support_boundaries", {
        collision_path: collision.replace_in_function("get_air_step_floor_start",
            "pos[1] != floorHeight", "(FALSE && pos[1] != floorHeight)")}
    yield "swimming", "extend_stale_support_plane", "swimming.air_floor_support_boundaries", {
        collision_path: collision.replace_in_function("get_air_step_floor_start",
            "!check_within_surface_triangle_bounds_f32((s32) pos[0], (s32) pos[2], floor)",
            "(FALSE && !check_within_surface_triangle_bounds_f32((s32) pos[0], (s32) pos[2], floor))")}
    yield "swimming", "lose_rounded_seam_support", "swimming.air_floor_grounded_seam", {
        collision_path: collision.replace_in_function("get_air_step_floor_start",
            "(s32) pos[0], (s32) pos[2], floor", "pos[0], pos[2], floor")}
    for name, case, before, after in (
        ("miss_lab_ridge_contact", "swimming.air_floor_lab_dives", "*floor != NULL", "FALSE"),
        ("catch_floor_from_below", "swimming.air_floor_sweep_boundaries",
         "from < 0.0f || ", ""),
        ("extend_floor_past_triangle", "swimming.air_floor_sweep_boundaries",
         "!check_within_surface_triangle_bounds_f32(candidate[0], candidate[2],\n"
         "                                                                     surface)", "FALSE"),
        ("ignore_swept_dynamic_filter", "swimming.air_floor_sweep_boundaries",
         "queryFlags & COLLISION_FLAG_EXCLUDE_DYNAMIC", "FALSE"),
    ):
        yield "swimming", name, case, {
            collision_path: collision.replace_in_function("find_air_step_floor_contact", before, after)}
    wall_path = "src/game/mario_collision.c"
    walls = MutationSource(root / wall_path)
    for function in ("mario_resolve_air_step_walls", "mario_resolve_ground_step_walls"):
        yield "swimming", "skip_seam_check_" + function, "swimming.wall_seam_crossing", {
            wall_path: walls.replace_in_function(function, "mario_wall_step_may_cross_plane(m, nextPos,",
                "lowerWall->numWalls == 0 && upperWall->numWalls == 0 && mario_wall_step_may_cross_plane(m, nextPos,")}
    yield "swimming", "allow_secondary_seam_push", "swimming.wall_seam_crossing", {
        wall_path: walls.replace_in_function("mario_reject_crossed_wall_push",
            "nextPos[0] = m->pos[0];", "return;\n    nextPos[0] = m->pos[0];")}
    water_path = "src/game/mario_water_collision.c"
    ray_path = "src/engine/math_util.c"
    rays = MutationSource(root / ray_path)
    yield "swimming", "scan_distant_movement_ray_faces", "swimming.jrb_ship_ray_bounds", {
        ray_path: rays.replace_in_function("find_surface_on_ray_candidates",
            "if (outside) {", "if (outside && FALSE) {")}
    yield "swimming", "reject_nearby_movement_ray_faces", "swimming.movement_ray_bounds_equivalence", {
        ray_path: rays.replace_in_function("find_surface_on_ray_candidates",
            "if (low > maximum[axis] || high < minimum[axis]) {",
            "if (low > maximum[axis] || high < minimum[axis] || TRUE) {")}
    water = MutationSource(root / water_path)
    yield "swimming", "repeat_known_water_contacts", "swimming.jrb_ship_contact_reuse", {
        water_path: water.replace_in_function("sweep_water_movement",
            "reuse_water_contacts(contacts, position, remaining, velocity, normals, &result)",
            "reuse_water_contacts(NULL, position, remaining, velocity, normals, &result)")}
    yield "swimming", "extend_water_contacts_past_edges", "swimming.contact_reuse_finite_edges", {
        water_path: water.replace_in_function("reuse_water_contacts",
            "if (!water_contact_contains(contacts->surfaces[i], target)) {",
            "if (FALSE && !water_contact_contains(contacts->surfaces[i], target)) {")}
    yield "swimming", "release_closed_swimming_corner", "swimming.closed_corner", {
        water_path: water.replace_in_function("project_water_contact_velocity",
            "previous < normalCount", "previous < 0")}
    yield "swimming", "repeat_ship_contact_planes", "swimming.jrb_ship_contact_work", {
        water_path: water.replace_in_function("sweep_water_movement",
            "project_water_contact_velocity(remaining, surface, normals, normalCount);",
            "project_water_velocity(remaining, surface);")}
    yield "swimming", "lose_ship_contact_velocity", "swimming.jrb_ship_contact_work", {
        water_path: water.replace_in_function("sweep_water_movement",
            "project_water_contact_velocity(velocity, surface, normals, normalCount);",
            "project_water_velocity(velocity, surface);").replace_in_function("reuse_water_contacts",
            "project_water_contact_velocity(velocity, contacts->surfaces[i], normals, i);",
            "project_water_velocity(velocity, contacts->surfaces[i]);")}
    edge_path = "src/engine/surface_collision.c"
    yield "swimming", "allow_edge_side_upwarp", "swimming.air_floor_edge_crossings", {
        edge_path: MutationSource(root / edge_path).replace_in_function("find_vertical_surface_precise",
            "airStart[1] < startPlaneY", "airStart[1] < startPlaneY && FALSE")}
    yield "swimming", "lose_edge_query_flags", "swimming.air_floor_edge_flags", {
        edge_path: MutationSource(root / edge_path).replace_in_function("find_floor_for_air_step_with_obstacle",
            "gCollisionFlags = collisionFlags;", "(void) collisionFlags;")}
    yield "swimming", "skip_air_riser_contact", "swimming.ttc_stopped_platform_entry", {
        "src/game/mario_step.c": MutationSource(root / "src/game/mario_step.c").replace_in_function(
            "mario_can_step_onto_air_floor", "if (floor == NULL || floor->normal.y < COS73) {",
            "if (TRUE || floor == NULL || floor->normal.y < COS73) {")}
    yield "swimming", "accept_thin_air_riser", "swimming.air_riser_entry_boundaries", {
        "src/game/mario_step.c": MutationSource(root / "src/game/mario_step.c").replace_in_function(
            "mario_can_step_onto_air_floor", "wall->normal.x * 50.0f", "wall->normal.x * 0.5f")
            .replace_in_function("mario_can_step_onto_air_floor",
                                 "wall->normal.z * 50.0f", "wall->normal.z * 0.5f")}
    path = "src/game/mario_actions_submerged.c"
    yield "swimming", "thicc_weight_missing", "swimming.thicc_water_weight", {
        path: MutationSource(root / path).replace_in_function("get_buoyancy",
            "buoyancy -= HE_THICC_WATER_SINK_SPEED;", "buoyancy -= 0.0f;")}

    helper_path = "src/game/object_helpers.c"
    helper = MutationSource(root / helper_path)
    yield "swimming", "block_stunned_separating_contacts", "swimming.stunned_contact_release", {
        helper_path: helper.replace_in_function("sweep_object_motion",
            "if (hit == MOVING_CAST_HIT && fraction == 0.0f",
            "if (FALSE && hit == MOVING_CAST_HIT && fraction == 0.0f")}
    math_path = "src/game/mario_moving_collision_math.c"
    math_source = MutationSource(root / math_path)
    yield "swimming", "extrapolate_stunned_simplex", "swimming.stunned_simplex_progress", {
        math_path: math_source.replace_in_function("moving_closest_simplex",
            "weights[i] < 0.0", "weights[i] < -MOVING_MATH_EPSILON")}
    yield "swimming", "round_stunned_simplex_candidates", "swimming.stunned_simplex_progress", {
        math_path: math_source.replace_in_function("moving_closest_simplex",
            "if (valid && distance < best)", "if (valid && (f32) distance < (f32) best)")}
    yield "swimming", "discard_skinny_stunned_faces", "swimming.stunned_simplex_progress", {
        math_path: math_source.replace_in_function("moving_closest_simplex",
            "determinant <= 0.0",
            "determinant <= 1e-12 * (edge0[0]*edge0[0] + edge0[1]*edge0[1] + edge0[2]*edge0[2])"
            " * (edge1[0]*edge1[0] + edge1[1]*edge1[1] + edge1[2]*edge1[2])")}
    yield "swimming", "repeat_stunned_triangle_geometry", "swimming.stunned_fixed_triangle_cast", {
        math_path: MutationSource(root / math_path).replace_in_function("moving_triangle_cast",
            "s32 fixedTriangle = (before == after);", "s32 fixedTriangle = FALSE;")}
    yield "swimming", "reverse_stunned_segment_projection", "swimming.stunned_simplex_distance", {
        math_path: MutationSource(root / math_path).replace_in_function("moving_closest_simplex",
            "weights[1] = a0 / d00;", "weights[1] = -a0 / d00;")}
    yield "swimming", "repeat_stunned_surface_casts", "swimming.stunned_candidate_work", {
        helper_path: helper.replace_in_function("sweep_object_motion",
            "if (duplicate) {", "if (FALSE && duplicate) {")}
    yield "swimming", "drop_stunned_candidates_on_overflow", "swimming.stunned_candidate_overflow", {
        helper_path: helper.replace_in_function("sweep_object_motion",
            "s32 duplicate = FALSE;",
            "if (testedCount == (s32) ARRAY_COUNT(tested)) { continue; }\n"
            "                    s32 duplicate = FALSE;")}
    yield "swimming", "skip_stunned_travel_sweep", "swimming.stunned_thin_geometry", {
        helper_path: MutationSource(root / helper_path).replace_in_function("cur_obj_move_stunned",
            "u32 contacts = sweep_object_motion(o, start, target, NULL, o->oWallHitboxRadius);",
            "u32 contacts = 0; (void)sweep_object_motion;")}
    path = "src/game/mario_water_collision.c"
    water = MutationSource(root / path)
    no_rays = water.replace_in_function("sweep_water_movement",
        "surface = find_water_crossed_surface(m, position, nextPos, &fraction, &hitResult, wallOffset,\n                                             &edgeContact);",
        "surface = NULL; (void)find_water_crossed_surface;")
    yield "swimming", "skip_swimming_body_edges", "swimming.body_edge_crossing", {
        path: water.replace_in_function("find_water_crossed_surface",
            "if (*fraction > 0.0f", "if (FALSE && *fraction > 0.0f")}
    yield "swimming", "skip_swimming_path_check", "swimming.steep_slope", {path: no_rays}
    old_step = MutationSource(root / path, no_rays).replace_in_function("mario_water_collision_step",
        "steps < WATER_COLLISION_MAX_STEPS", "steps < 1")
    yield "swimming", "single_destination_swimming", "swimming.fast_floor", {path: old_step}
    yield "swimming", "forget_water_floor_contact", "swimming.floor_contact_retained", {
        path: water.replace_in_function("mario_water_collision_step",
            "result = merge_water_step_result(result, stepResult);", "result = stepResult;")}
    yield "swimming", "reuse_spent_swimming_movement", "swimming.contact_remainder", {
        path: water.replace_in_function("sweep_water_movement",
            "remaining[axis] *= 1.0f - fraction;", "remaining[axis] *= 1.0f;")}
    yield "swimming", "use_rounded_floor_height", "swimming.downward_slope_contact", {
        path: water.replace_in_function("water_position_has_clearance",
            "*floorHeight = find_floor_precise(pos[0], pos[1], pos[2], floor);",
            "*floorHeight = find_floor(pos[0], pos[1], pos[2], floor);")}
    yield "swimming", "ignore_unreported_wall_push", "swimming.jrb_underside_travel", {
        path: water.replace_in_function("resolve_water_wall_push",
            "wallData.numWalls != 0 || pos[0] != beforeWall[0] || pos[2] != beforeWall[2]",
            "wallData.numWalls != 0")}
    swim_path = "src/game/mario_actions_submerged.c"
    yield "swimming", "overbrake_wall_glance", "swimming.turning_wall_speed", {
        swim_path: MutationSource(root / swim_path).replace_in_function("perform_water_step",
            "vec3f_copy(m->vel, velocity);", "vec3f_copy(m->vel, velocity); m->forwardVel *= 0.9f;")}
    yield "swimming", "allow_blocked_swim_velocity", "swimming.blocked_momentum", {
        swim_path: MutationSource(root / swim_path).replace_in_function("perform_water_step",
            "mario_water_collision_step(m, step, velocity)", "mario_water_collision_step(m, step, NULL)")}
    collision_path = "src/engine/surface_collision.c"
    yield "swimming", "round_swimming_triangle_selection", "swimming.downhill_seam", {
        collision_path: MutationSource(root / collision_path).replace_in_function(
            "find_vertical_surface_precise",
            "check_within_surface_triangle_bounds_f32(x, z, surface)",
            "check_within_surface_triangle_bounds_f32((s32)x, (s32)z, surface)")}
    yield "swimming", "omit_overlap_recovery", "swimming.overlap_escape", {
        path: water.replace_in_function("mario_water_collision_step",
            "u32 result = recover_water_overlap(m, &overlapping);",
            "u32 result = WATER_STEP_NONE; overlapping = FALSE;")}
    yield "swimming", "reject_partial_overlap_escape", "swimming.jrb_object_rock_escape", {
        path: water.replace_in_function("perform_water_substep",
            "if ((!hasClearance || *overlapping)\n"
            "        && !water_movement_reduces_overlap(m->pos, nextPos, floor, floorHeight))",
            "(void)water_movement_reduces_overlap; if (!hasClearance)")}
    yield "swimming", "forget_initial_overlap", "swimming.overlap_escape_progress", {
        path: water.replace_in_function("perform_water_substep",
            "(!hasClearance || *overlapping)", "(!hasClearance)")}
    yield "swimming", "allow_floor_backside_entry", "swimming.overlap_escape_progress", {
        path: water.replace_in_function("water_movement_reduces_overlap",
            "if (floorOverlap > WATER_COLLISION_SKIN", "if (FALSE && floorOverlap > WATER_COLLISION_SKIN")}
    yield "swimming", "allow_ceiling_backside_entry", "swimming.overlap_escape_progress", {
        path: water.replace_in_function("water_movement_reduces_overlap",
            "if (ceilOverlap > WATER_COLLISION_SKIN", "if (FALSE && ceilOverlap > WATER_COLLISION_SKIN")}
    action_path = "src/game/mario_actions_submerged.c"
    yield "swimming", "amplify_sideways_swim_speed", "swimming.wall_slide_controls", {
        action_path: MutationSource(root / action_path).replace_in_function("update_swimming_speed",
            "m->vel[2] = m->forwardVel * coss(m->faceAngle[0]) * coss(m->faceAngle[1]);",
            "m->vel[2] = m->forwardVel;")}
    yield "swimming", "restore_wall_pitch", "swimming.jrb_wall_pitch", {
        action_path: MutationSource(root / action_path).replace_in_function("common_swimming_step",
            "case WATER_STEP_HIT_WALL:", "case WATER_STEP_HIT_WALL: m->faceAngle[0] -= 0x200;")}
    yield "swimming", "override_downward_swim_input", "swimming.downward_controls", {
        action_path: MutationSource(root / action_path).replace_in_function("common_swimming_step",
            "case WATER_STEP_HIT_FLOOR:",
            "case WATER_STEP_HIT_FLOOR:\n"
            "            m->faceAngle[0] = MAX(m->faceAngle[0], -find_floor_slope(m, -0x8000));")}
    yield "swimming", "remove_floor_pitch_assist", "swimming.slope_floor_assist", {
        action_path: MutationSource(root / action_path).replace_in_function("common_swimming_step",
            "m->controller->stickY <= 0.0f", "FALSE")}
    yield "swimming", "snap_floor_pitch", "swimming.slope_floor_assist", {
        action_path: MutationSource(root / action_path).replace_in_function("common_swimming_step",
            "MIN(previousPitch + 0x200, floorPitch)", "((void) previousPitch, floorPitch)")}
