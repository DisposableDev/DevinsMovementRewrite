"""Production extraction and deliberate defects for the geometry suite."""
from .common import prepare_common_constants
import re
from regression_support import HarnessError, Source
from .mutation_support import MutationSource

def prepare_geometry(root, source, definitions, functions):
    prepare_common_constants(source, definitions, gameplay_options=True, cap_functions=functions)
    definitions += [source("src/game/mario_actions_airborne.h").macros("JUMP_KICK_(CANCEL|BUFFER)_FRAMES")]
    functions += [source("src/game/mario_actions_airborne.c").function("mario_jump_kick_can_cancel")]
    airborne = source("src/game/mario_actions_airborne.c")
    definitions += [source("src/game/mario_actions_airborne.h").macros("ROLLOUT_WATER_WALK_DELAY_FRAMES")]
    definitions += [source("src/game/mario_actions_airborne.h").declaration("enum", "ActionArgsFreefall")]
    definitions += [airborne.declaration("enum", name) for name in
                    ("HandstandFallState", "FallDamageSeverity")]
    definitions += [source("include/config/config_movement.h").macros("FALL_DAMAGE_HEIGHT_.*")]
    functions += [airborne.function(name) for name in
                  ("handstand_jump_protects_from_fall", "is_large_fall_at_height",
                   "fall_damage_severity_at_height", "fall_damage_severity",
                   "mario_would_take_fall_damage_on_water")]
    twirl = source("src/game/mario_air_twirl.c")
    definitions += [twirl.macros("AIR_TWIRL_.*")]
    definitions += [source("include/config/config_movement.h").macros(
        "WATER_JUMP_WALL_KICK_LOCK_FRAMES|BACKWARD_AIR_.*")]
    functions += [twirl.function(name) for name in (
        "mario_air_twirl_reset_allowances", "mario_air_twirl_init", "mario_air_twirl_reset_on_coin",
        "mario_air_twirl_is_active",
        "mario_air_twirl_transition", "mario_start_air_twirl", "mario_air_twirl_input_allowed",
        "mario_try_wall_air_twirl", "mario_air_twirl_action_allows_entry", "mario_try_air_twirl",
        "mario_air_twirl_gravity_scale",
        "mario_air_twirl_control_scale",
        "mario_air_twirl_limit_control_speed",
        "mario_air_twirl_bounce_off_wall", "mario_air_twirl_flying_wall_bounce",
        "mario_air_twirl_flying_action", "mario_air_twirl_action")]
    functions += [source("src/game/mario_actions_airborne.c").function(name) for name in (
        "check_horizontal_wind", "backward_air_extra_drag", "update_air_with_turn")]
    # Keep existing geometry action doubles; exercise real movement in Wing control cases.
    functions += [airborne.function("update_air_without_turn").replace(
        "void update_air_without_turn(", "void wing_update_air_without_turn(", 1)]
    definitions += [airborne.macros("WALL_KICK_.*")]
    definitions += [source("src/game/mario_actions_airborne.c").macros("AIR_HIGH_SPEED_.*")]
    definitions += [source("include/config/config_movement.h").macros(
        "FORWARD_DIVE_.*|SLIDE_KICK_.*|SIDE_FLIP_.*|CAPLESS_CAP_PICKUP_.*")]
    definitions += [source("src/game/mario_actions_airborne.h").declaration("enum", "ActionArgsPuttingOnCapAir")]
    definitions += [source("include/config/config_movement.h").macros("WALL_BONK_MOMENTUM_RETENTION")]
    functions += [source("src/game/mario_step.c").function("mario_set_wall_bonk_momentum")]
    definitions += [source("src/game/camera.h").declaration("enum", "CameraModes"),
                    source("src/game/camera.h").declaration("enum", "CameraEvent"),
                    source("include/config/config_camera.h").macros("FLYING_CAMERA_MODE")]
    functions += [source("src/engine/math_util.c").function("approach_s32"),
                  source("src/game/mario_step.c").function("set_vel_from_pitch_and_yaw")]
    functions += [source("src/game/mario_actions_airborne.c").function(name) for name in (
        "update_flying_yaw", "update_flying_pitch", "flying_speed_pitch_change", "update_flying",
        "cancel_wing_cap_flight", "act_flying", "act_flying_triple_jump", "act_shot_from_cannon")]
    definitions += [source("src/game/mario_actions_airborne.h").declaration(
        "enum", "ActionStatesFlyingTripleJump")]
    functions += [
        "void mario_he_thicc_reset(void) {}",
        "s32 mario_he_thicc_try_belly_landing(struct MarioState *m) { return FALSE; }",
        "s32 mario_he_thicc_try_pound_bounce(struct MarioState *m) { return FALSE; }",
        "void mario_he_thicc_wall_kick(struct MarioState *m) {}",
        "void mario_he_thicc_slide_kick_bounce(struct MarioState *m) {}",
    ]
    definitions += [source("src/game/mario_he_thicc.h").macros(
        "HE_THICC_.*|ACT_ARG_WALL_KICK_HE_THICC|ACT_ARG_FORWARD_ROLLOUT_HE_THICC_BOUNCE")]
    definitions += [source("include/config/config_movement.h").macros("METAL_WATER_.*|MARIO_MAX_FALL_SPEED|HE_THICC_(MAX_FALL_SPEED|WATER_GRAVITY_SCALE)")]
    definitions += [source("src/engine/surface_collision.h").macros("FLOOR_LOWER_LIMIT")]
    functions += [source("src/game/mario_step.c").function("mario_get_movement_time_scale")]
    functions += [source("src/game/mario_step.c").function(name) for name in (
        "mario_max_fall_speed", "limit_metal_water_fall_speed")]
    definitions += [source("src/game/mario_character.h").declaration("enum", "MarioPalette")]
    definitions += [source("src/game/mario_actions_airborne.h").macros(
        "ACT_ARG_JUMP_BACKWARD_BUTT_SLIDE.*|ACT_ARG_JUMP_KICK_FOLLOWUP_QUEUED|ACT_ARG_JUMP_KICK_BACKWARD_BUTT_SLIDE.*|ACT_ARG_STEEP_JUMP_BACKWARD_BUTT_SLIDE_NO_UPHILL|ACT_ARG_DIVE_FROM_ROLLOUT_CONTACT")]
    definitions += [source("src/game/mario_actions_stationary.h").macros(
        "ACT_ARG_COUGHING_SHALLOW_WATER")]
    definitions += [source("include/config/config_movement.h").macros("BACKWARD_BUTT_SLIDE_JUMP_AIR_DRAG|BACKWARD_JUMP_KICK_UPHILL_DECEL")]
    definitions += [source("src/game/interaction.h").declaration("enum", "Interactions")]
    air_header = source("src/game/mario_actions_airborne.h")
    definitions += [air_header.declaration("enum", "ActionArgsTripleJump"),
                    air_header.declaration("enum", "ActionStatesFlying"),
                    air_header.declaration("enum", "ActionArgsFlying"),
                    air_header.macros("BACKWARD_JUMP_WALL_KICK_DELAY_FRAMES")]
    functions += [source("src/game/mario_actions_airborne.c").function(
        "mario_can_wall_kick_backward_jump")]
    automatic = source("src/game/mario_actions_automatic.c")
    definitions += [automatic.macros("HANG_DISTANCE|HANG_GRAB_.*")]
    definitions += [source("include/config/config_movement.h").macros("HANGING_SPEED")]
    definitions += [source("src/game/mario_actions_automatic.h").declaration("enum", "HangingSteps")]
    definitions += [source("src/engine/math_util.h").macros("approach_s16_symmetric")]
    definitions += [source("src/engine/math_util.h").macros("NEAR_ZERO")]
    functions += ["s16 abs_angle_diff(s16 a0, s16 a1);"]
    functions += [source("src/engine/math_util.c").function("approach_s16")]
    # Match hanging controls without changing this fixture's existing air-grab branch.
    functions += [source("include/config/config_movement.h").macros("BETTER_HANGING")]
    functions += [automatic.function(name) for name in (
        "perform_hanging_step", "update_hang_moving", "update_hang_stationary",
        "update_hang_grab_momentum", "act_start_hanging")]
    functions += ["#undef BETTER_HANGING"]
    definitions += [source("include/object_constants.h").macros("BOWSER_MINE_LAUNCH_ARG")]
    functions.remove("void mario_he_thicc_slide_kick_bounce(struct MarioState *m) {}")
    definitions += [source("src/game/mario_actions_airborne.h").declaration("enum", name) for name in (
        "ActionStatesJumpKick", "ActionStatesSlideKick")]
    definitions += [source("include/types.h").declaration("enum", "PunchStateTypes")]
    definitions += [source("src/game/mario_actions_airborne.c").macros("JUMP_KICK_(ACTIVE|FOLLOWUP|BUFFER)_FRAMES")]
    definitions += [source("src/engine/surface_collision.h").macros("FIND_FLOOR_BUFFER")]
    definitions += [source("src/game/mario_actions_airborne.c").macros("SLIDE_KICK_.*"),
                    source("src/game/liquid_particles.h").declaration("enum", "LiquidParticleMaterial")]
    functions += [source("src/game/mario_actions_airborne.c").function(name) for name in (
        "act_jump_kick", "slide_kick_handle_wall", "slide_kick_continue_bounce", "act_slide_kick")]
    definitions += [source("src/game/camera.h").declaration("enum", "CameraShake")]
    functions += [source("src/game/mario.c").function("mario_he_thicc_contact_shake")]
    definitions += [source("src/game/mario_actions_airborne.h").declaration(
        "enum", "ActionStatesSpecialTripleJump")]
    sm64 = source("include/sm64.h")
    definitions += [sm64.declaration("enum", name) for name in (
        "MarioInput", "MarioFlags", "MarioActionFlags", "MarioActionGroups",
        "MarioParticleFlags", "AirStep", "AirStepCheck", "GroundStep")]
    definitions += [sm64.macros("ACT_.*|COS25|COS73")]
    definitions += [source("include/mario_animation_ids.h").declaration("enum", "MarioAnimID")]
    definitions += [source("src/game/mario.h").declaration("enum", "MarioSpawnFallProtection")]
    definitions += [source("include/surface_terrains.h").declaration("enum", name) for name in (
        "SurfaceTypes", "TerrainType")]
    definitions += [source("include/config/config_movement.h").macros(
        "WALL_KICK_DEGREES|WALL_KICK_GRACE_FRAMES|MARIO_GROUND_AIR_.*|MARIO_GLOBAL_SPEED_CAP|"
        "MARIO_MAX_FALL_SPEED|FALL_DRAG_.*|JUMP_RELEASE_UPWARD_DECEL|DONT_LEDGE_GRAB_STEEP_SLOPES|MARIO_STRONG_STICK_THRESHOLD")]
    definitions += [source("src/game/mario_actions_airborne.h").macros("WALL_KICK_MOMENTUM_RETENTION|WALL_KICK_MIN_FORWARD_SPEED|WALL_KICK_STICK_LOOKBACK_FRAMES")]
    air = source("src/game/mario_actions_airborne.c")
    definitions += [air.macros("WALL_KICK_VERTICAL_SPEED|WALL_KICK_CHAIN_SPEED_LOSS|WALL_KICK_CHAIN_ZERO_SPEED_COUNT")]
    step = source("src/game/mario_step.c")
    config = source("include/config/config_movement.h")
    if not re.search(r"^#define HANGING_FIX\b", config.mask, re.M):
        raise HarnessError("Geometry adapter expects enabled HANGING_FIX; review alternate branch")
    active, count = re.subn(r"#ifdef HANGING_FIX\n(.*?)#else\n.*?#endif", r"\1", step.text, flags=re.S)
    if count != 1:
        raise HarnessError("HANGING_FIX branch moved; update geometry extraction")
    step = Source(step.path, active)
    definitions += [step.macros("hdot_surf|WIND_.*|MARIO_GROUND_STEP_MAX_FLOOR_DELTA|MARIO_GROUND_LOW_STEP_MAX_HEIGHT|MARIO_WATER_WALK_.*")]
    definitions += [config.macros("WING_FLIGHT_.*")]
    definitions += [source("src/engine/math_util.h").macros("absf")]
    functions += [source("src/engine/math_util.c").function("abs_angle_diff")]
    functions += [step.function(name) for name in (
        "check_ledge_grab", "mario_is_inside_wind_column", "mario_update_wind_death_grace",
        "mario_is_in_vertical_wind", "wind_stopping_distance",
        "wind_safe_downward_speed", "constrain_wind_descent", "bonk_or_hit_lava_wall",
        "mario_water_walk_conditions_met", "mario_tick_water_walking_cooldown", "mario_update_water_walking_state", "mario_can_walk_on_water", "mario_is_on_floor", "mario_can_step_past_lower_walls", "perform_ground_quarter_step",
        "get_mario_ground_vertical_travel",
        "get_mario_ground_air_collision_steps", "get_mario_ground_remaining_steps",
        "perform_ground_step_internal", "perform_ground_step_with_fraction", "perform_ground_step",
        "perform_ground_step_stop_at_ledge",
        "perform_air_step_ceiling_collision", "perform_air_quarter_step",
        "gravity_with_fall_drag", "water_gravity_scale", "apply_twirl_gravity",
        "should_strengthen_gravity_for_jump_ascent", "apply_gravity", "apply_vertical_wind",
        "mario_find_air_twirl_entry_wall", "perform_air_step_internal", "perform_air_step_with_remainder", "perform_slide_kick_air_step", "perform_air_step_with_fraction", "mario_play_bonk_sound", "mario_play_wall_bonk_sound", "mario_play_air_twirl_sound", "mario_bonk_reflection")]
    functions += [air.function(name) for name in (
        "start_wall_kick_from_wall", "mario_update_wall_kick_slide_input", "mario_has_wall_kick_slide_input", "wall_kick_priority_step_arg", "begin_wall_kick_window", "check_wall_kick",
        "act_air_hit_wall", "common_air_knockback_step", "act_soft_bonk", "act_backward_air_kb",
        "common_air_action_step", "act_freefall", "act_long_jump",
        "act_triple_jump", "act_special_triple_jump", "act_backflip",
        "apply_backward_butt_slide_jump_drag", "backward_butt_slide_jump_blocks_uphill", "mario_restrict_air_uphill", "backward_butt_slide_jump_restrict_uphill",
        "act_steep_jump")]
    functions += [source("src/game/interaction.c").function("check_death_barrier")]
    functions += [source("src/game/mario.c").function("mario_is_low_land_pose")]
    functions += [source("src/game/mario.c").function("mario_has_land_movement_underwater")]
    definitions += [source("include/config/config_movement.h").macros("WATER_ENTRY_.*_DIVISOR")]
    definitions += [source("include/config/config_camera.h").macros("WATER_SURFACE_CAMERA_MODE")]
    definitions += [source("src/engine/math_util.h").macros("vec3_zero|vec3_same")]
    definitions += [source("src/game/mario_actions_submerged.h").declaration("enum", "ActionArgsWaterDownBurst")]
    functions += [source("src/game/mario.c").function("set_water_plunge_action")]
    moving = source("src/game/mario_actions_moving.c")
    definitions += [moving.macros("analog_stick_held_back")]
    functions += [moving.function(name) for name in (
        "check_common_moving_cancels", "check_ledge_climb_down_internal",
        "check_crouch_slide_ledge_grab", "common_slide_action", "common_slide_action_with_jump")]
    definitions += [source("src/game/mario_punch.h").declaration("enum", "ActionArgsPunchSequence")]
    mesh = source("levels/bitfs/tilting_square_platform/collision.inc.c")
    vertices = re.findall(r"COL_VERTEX\(\s*(-?\d+),\s*(-?\d+),\s*(-?\d+)\)", mesh.mask)
    if len(vertices) != 5:
        raise HarnessError("Tilting platform vertex layout changed")
    definitions += ["static Vec3f tiltVertices[] = {" + ",".join("{" + ",".join(v) + "}" for v in vertices) + "};"]
    rock = source("levels/jrb/rock/collision.inc.c")
    vertices = re.findall(r"COL_VERTEX\(\s*(-?\d+),\s*(-?\d+),\s*(-?\d+)\)", rock.mask)
    triangles = re.findall(r"COL_TRI\(\s*(\d+),\s*(\d+),\s*(\d+)\)", rock.mask)
    if (len(vertices) != 12 or len(triangles) != 20
            or any(int(i) >= len(vertices) for t in triangles for i in t)):
        raise HarnessError("JRB rock layout changed; review downhill-slide fixture")
    definitions += ["static Vec3s rockVertices[] = {" + ",".join("{" + ",".join(v) + "}" for v in vertices) + "};",
                    "static int rockTriangles[][3] = {" + ",".join("{" + ",".join(t) + "}" for t in triangles) + "};"]
    placements = re.findall(r"MODEL_JRB_ROCK,\s*(-?\d+),\s*(-?\d+),\s*(-?\d+),\s*0,\s*(\d+),\s*0",
                           source("levels/jrb/script.c").mask)
    by_position = {tuple(map(int, p[:3])): p for p in placements}
    requested = [(-5228, 1230, 2106), (-5332, 1434, 1023), (-3978, 1536, -85)]
    if any(p not in by_position for p in requested):
        raise HarnessError("Screenshot rock placements changed; review downhill-slide fixture")
    definitions += ["static const s32 rockTestPlacements[][4] = {" +
                    ",".join("{" + ",".join(by_position[p]) + "}" for p in requested) + "};"]

def mutations_geometry(root):
    path = "src/game/mario_actions_airborne.c"
    yield "geometry", "original_keeps_cruise_physics", "geometry.wing_preset", {
        path: MutationSource(root / path).replace_in_function("update_flying",
            "game_options_get(GO_WING_CAP) != 0", "FALSE")}
    automatic_path = "src/game/mario_actions_automatic.c"
    automatic = MutationSource(root / automatic_path)
    for name, case, function, before, after in (
        ("erase_ceiling_grab_momentum", "hanging_grab_momentum", "act_start_hanging",
         "m->forwardVel * HANG_GRAB_MOMENTUM_RETENTION", "0.0f"),
        ("repeat_ceiling_grab_retention", "hanging_grab_momentum", "act_start_hanging",
         "if (m->actionTimer == 1)", "if (TRUE)"),
        ("skip_ceiling_grab_braking", "hanging_grab_momentum", "update_hang_grab_momentum",
         "approach_f32(m->forwardVel, 0.0f, HANG_GRAB_COAST_DECEL, HANG_GRAB_COAST_DECEL)",
         "m->forwardVel"),
        ("clamp_ceiling_grab_handoff", "hanging_grab_momentum", "update_hang_moving",
         "MAX(maxSpeed, m->forwardVel - HANG_GRAB_COAST_DECEL)", "maxSpeed"),
        ("retain_ceiling_grab_past_edge", "hanging_grab_boundaries", "update_hang_grab_momentum",
         "if (step == HANG_LEFT_CEIL)", "if (FALSE)"),
    ):
        yield "geometry", name, "geometry." + case, {
            automatic_path: automatic.replace_in_function(function, before, after)}
    wind_path = "src/game/mario_step.c"
    wind = MutationSource(root / wind_path)
    yield "geometry", "refresh_wind_death_grace", "geometry.wind_death_grace", {
        wind_path: wind.replace_in_function("mario_update_wind_death_grace",
            "!m->windColumnActive", "TRUE")}
    yield "geometry", "skip_wind_death_grace_countdown", "geometry.wind_death_grace", {
        wind_path: wind.replace_in_function("mario_update_wind_death_grace",
            "m->windDeathGraceTimer--;", "m->windDeathGraceTimer = m->windDeathGraceTimer;")}
    yield "geometry", "ignore_wind_death_grace", "geometry.wind_death_grace", {
        "src/game/interaction.c": MutationSource(root / "src/game/interaction.c").replace_in_function(
            "check_death_barrier", "m->windDeathGraceTimer != 0", "FALSE && m->windDeathGraceTimer != 0")}
    yield "geometry", "keep_wind_grace_outside_column", "geometry.wind_grace_exit", {
        "src/game/interaction.c": MutationSource(root / "src/game/interaction.c").replace_in_function(
            "check_death_barrier", "&& mario_is_inside_wind_column(m)", "")}
    yield "geometry", "keep_wind_grace_on_exit", "geometry.wind_grace_exit", {
        wind_path: wind.replace_in_function("mario_update_wind_death_grace",
            "m->windDeathGraceTimer = 0;", "(void) m->windDeathGraceTimer;")}
    yield "fixes", "keep_wind_grace_on_respawn", "fixes.crush_respawn_visibility", {
        "src/game/mario.c": MutationSource(root / "src/game/mario.c").replace_in_function(
            "init_mario", "gMarioState->windDeathGraceTimer = 0;", "(void) gMarioState->windDeathGraceTimer;")}
    edge_path = "src/game/mario_step.c"
    yield "geometry", "miss_swept_air_landing", "geometry.swept_air_floor_landing", {
        edge_path: MutationSource(root / edge_path).replace_in_function("perform_air_quarter_step",
            "if (find_air_step_floor_contact(", "if (FALSE && find_air_step_floor_contact(")}
    yield "geometry", "restore_fence_grinding", "geometry.fence_edge_grinding", {
        edge_path: MutationSource(root / edge_path).replace_in_function("perform_air_quarter_step",
            "f32 floorHeight = find_floor_for_air_step(floorStart, nextPos, &floor);",
            "f32 floorHeight = find_floor(nextPos[0], nextPos[1], nextPos[2], &floor);")}
    yield "geometry", "reuse_missing_floor_above_feet", "geometry.fence_edge_grinding", {
        edge_path: MutationSource(root / edge_path).replace_in_function("perform_air_quarter_step",
            "nextPos[1] <= m->floorHeight && m->pos[1] >= m->floorHeight",
            "nextPos[1] <= m->floorHeight")}
    # Alternate HANGING_FIX branches contain unmatched opening braces before extraction.
    step_text = MutationSource(root / edge_path)
    ceiling_guard = (
        "// A remembered floor only counts when this descent crosses it from above.\n"
        "    if (nextPos[1] <= m->floorHeight && m->pos[1] >= m->floorHeight)")
    yield "geometry", "reuse_ceiling_floor_above_feet", "geometry.fence_edge_grinding", {
        edge_path: step_text.replace_unique(ceiling_guard,
            ceiling_guard.replace(" && m->pos[1] >= m->floorHeight", ""))}
    step_path = "src/game/mario_step.c"
    step = MutationSource(root / step_path)
    priority_case = "geometry.buffered_wall_kick_ledge_priority"
    yield "geometry", "let_ledge_swallow_buffered_kick", priority_case, {
        step_path: step.replace_in_function("perform_air_quarter_step",
            "if (stepArg & AIR_STEP_CHECK_WALL_KICK)", "if (FALSE)")}
    yield "geometry", "overwrite_buffered_kick_in_later_substep", priority_case, {
        step_path: step.replace_in_function("perform_air_step_internal",
            "if (quarterStepResult == AIR_STEP_HIT_WALL_STOP_QSTEPS)", "if (FALSE)")}
    priority_air_path = "src/game/mario_actions_airborne.c"
    priority_air = MutationSource(root / priority_air_path)
    yield "geometry", "prioritize_expired_wall_kick", priority_case, {
        priority_air_path: priority_air.replace_in_function("wall_kick_priority_step_arg",
            "m->wallKickInputFrames != 0", "TRUE")}
    yield "geometry", "prioritize_kick_without_strong_stick", priority_case, {
        priority_air_path: priority_air.replace_in_function("wall_kick_priority_step_arg",
            "mario_has_wall_kick_slide_input(m)", "TRUE")}
    yield "geometry", "prioritize_kick_below_speed_gate", priority_case, {
        priority_air_path: priority_air.replace_in_function("wall_kick_priority_step_arg",
            "(m->forwardVel > 10.0f || mario_can_wall_kick_backward_jump(m))", "TRUE")}
    yield "geometry", "skip_no_floor_ceiling", "geometry.jump_geometry_preservation", {
        step_path: MutationSource(root / step_path).replace_in_function("perform_air_quarter_step",
            "if (blockedPos[1] + 160.0f > ceilHeight)", "if (FALSE)")}
    yield "geometry", "query_ceiling_beyond_blocked_footprint", "geometry.jump_geometry_preservation", {
        step_path: MutationSource(root / step_path).replace_in_function("perform_air_quarter_step",
            "find_mario_ceil(blockedPos, m->floorHeight, &ceil)",
            "find_mario_ceil(nextPos, m->floorHeight, &ceil)")}
    air_path = "src/game/mario_actions_airborne.c"
    air = MutationSource(root / air_path)
    no_wall = "if (m->wall == NULL) {"
    yield "geometry", "cut_no_floor_jump_ascent", "geometry.missing_floor_jump_height", {
        air_path: air.replace_in_function("common_air_action_step", no_wall,
            no_wall + "\n                if (m->forwardVel > 16.0f && m->vel[1] > 0.0f) m->vel[1] = 0.0f;")}
    yield "geometry", "lose_no_floor_short_hop_action", "geometry.missing_floor_jump_height", {
        air_path: air.replace_in_function("common_air_action_step", no_wall,
            no_wall + "\n                set_mario_action(m, ACT_SOFT_BONK, 0);")}
    action_path = "src/game/mario_actions_airborne.c"
    yield "geometry", "voice_on_level_act_air_hit_wall_catch", "geometry.wall_slide_ledge_voice", {
        action_path: MutationSource(root / action_path).replace_in_function("act_air_hit_wall",
            "m->vel[1] <= -LEDGE_GRAB_VOICE_MIN_FALL_SPEED ? 0 : ACT_ARG_LEDGE_GRAB_GENTLE",
            "0")}
    yield "geometry", "voice_on_slight_wall_slide_descent", "geometry.wall_slide_ledge_voice", {
        action_path: MutationSource(root / action_path).replace_in_function("act_air_hit_wall",
            "m->vel[1] <= -LEDGE_GRAB_VOICE_MIN_FALL_SPEED", "m->vel[1] < 0.0f")}
    path = "src/game/mario_step.c"
    step = MutationSource(root / path)
    old = "mario_attack_cap_box_surface(m, ceil, INT_HIT_FROM_BELOW);"
    # HANGING_FIX contains alternate opening braces; keep both branches
    # intact for the geometry adapter's existing preprocessing.
    yield "geometry", "skip_upward_cap_box_contact", "geometry.underwater_box_surface_contacts", {
        path: step.replace_unique(old, "(void)ceil;")}
    path = "src/game/mario_step.c"
    yield "geometry", "remove_metal_water_resistance", "geometry.metal_water_movement", {
        path: MutationSource(root / path).replace_in_function("mario_get_movement_time_scale",
            "if (!(m->flags & MARIO_METAL_CAP) || m->waterLevel <= FLOOR_LOWER_LIMIT) {", "if (TRUE) {")}
    yield "geometry", "retain_dry_metal_water_gravity", "geometry.metal_water_gravity", {
        path: MutationSource(root / path).replace_in_function("apply_gravity",
            "(m->vel[1] - beforeGravity) * timeScale", "(m->vel[1] - beforeGravity) * 1.0f")}
    yield "geometry", "delay_metal_water_descent_limit", "geometry.metal_water_air_and_boundaries", {
        path: MutationSource(root / path).replace_in_function("perform_air_step_internal",
            "limit_metal_water_fall_speed(m, timeScale);", "(void) timeScale;")}
    yield "geometry", "discard_slide_kick_rebound_travel", "geometry.slide_kick_bounce_travel", {
        air_path: MutationSource(root / air_path).replace_in_function("slide_kick_continue_bounce",
            "perform_air_step_with_fraction(m, 0, remaining)",
            "perform_air_step_with_fraction(m, 0, 0.0f)")}
    yield "geometry", "repeat_slide_kick_rebound_gravity", "geometry.slide_kick_bounce_travel", {
        path: MutationSource(root / path).replace_in_function("perform_air_step_with_fraction",
            "MIN(fraction, 1.0f), FALSE", "MIN(fraction, 1.0f), TRUE")}
    yield "geometry", "retain_dry_metal_water_ground_travel", "geometry.metal_water_movement", {
        path: MutationSource(root / path).replace_in_function("perform_ground_step_internal",
            "oldX + oldFloor->normal.y * m->vel[0] * stepFraction * timeScale",
            "oldX + oldFloor->normal.y * m->vel[0] * stepFraction")}
    path = "src/game/mario_actions_airborne.c"
    yield "geometry", "ignore_wall_slide_stick", "geometry.wall_slide_stick_gate", {
        path: MutationSource(root / path).replace_in_function("mario_has_wall_kick_slide_input",
            "m->wallKickBackwardStickFrames != 0 : m->wallKickForwardStickFrames != 0;", "TRUE : TRUE;")}
    yield "geometry", "forget_recent_wall_slide_stick", "geometry.wall_slide_stick_gate", {
        path: MutationSource(root / path).replace_in_function("mario_update_wall_kick_slide_input",
            "m->wallKickForwardStickFrames = WALL_KICK_STICK_LOOKBACK_FRAMES + 1;",
            "m->wallKickForwardStickFrames = 1;")}
    yield "geometry", "disable_backward_triple_wall_kick", "geometry.backward_triple_wall_kick", {
        path: MutationSource(root / path).replace_in_function("mario_can_wall_kick_backward_jump",
            "m->actionArg == ACT_ARG_TRIPLE_JUMP_BACKWARD && m->forwardVel < 0.0f", "FALSE")}
    yield "geometry", "reject_slow_backward_triple_wall_kick", "geometry.backward_triple_wall_kick", {
        path: MutationSource(root / path).replace_in_function("mario_can_wall_kick_backward_jump",
            "m->actionArg == ACT_ARG_TRIPLE_JUMP_BACKWARD && m->forwardVel < 0.0f",
            "m->actionArg == ACT_ARG_TRIPLE_JUMP_BACKWARD && m->forwardVel < -16.0f")}
    yield "geometry", "early_backward_triple_wall_kick", "geometry.backward_triple_wall_kick", {
        path: MutationSource(root / path).replace_in_function("mario_can_wall_kick_backward_jump",
            "m->actionTimer <= BACKWARD_JUMP_WALL_KICK_DELAY_FRAMES", "m->actionTimer < BACKWARD_JUMP_WALL_KICK_DELAY_FRAMES")}
    yield "geometry", "disable_backflip_wall_kick", "geometry.backward_triple_wall_kick", {
        path: MutationSource(root / path).replace_in_function("mario_can_wall_kick_backward_jump",
            "return m->forwardVel < 0.0f;", "return FALSE;")}
    yield "geometry", "omit_wall_kick_sound", "geometry.wall_window", {
        path: MutationSource(root / path).replace_in_function("begin_wall_kick_window",
            "mario_play_wall_bonk_sound(m);", "(void)0;")}
    yield "geometry", "duplicate_wall_kick_hit", "geometry.wall_window", {
        path: MutationSource(root / path).replace_in_function("start_wall_kick_from_wall",
            "queue_rumble_data(5, 40);",
            "play_sound(SOUND_ACTION_HIT, m->marioObj->header.gfx.cameraToObject);\n"
            "    queue_rumble_data(5, 40);")}
    path = "src/game/mario_step.c"
    yield "geometry", "crawl_up_vertical_floor", "geometry.crawl_platform_side", {
        path: MutationSource(root / path).replace_in_function("perform_ground_quarter_step",
            "mario_is_low_land_pose(m) && floor->normal.y < COS73", "FALSE && mario_is_low_land_pose(m)")}
    yield "geometry", "freeze_downhill_belly_slide", "geometry.rock_downhill", {
        path: MutationSource(root / path).replace_in_function("perform_ground_quarter_step",
            "\n        && (!mario_is_on_floor(m) || floorHeight > m->pos[1])", "")}
    yield "geometry", "attach_unsupported_steep_slide", "geometry.steep_slide_protection", {
        path: MutationSource(root / path).replace_in_function("perform_ground_quarter_step",
            "!mario_is_on_floor(m) || floorHeight > m->pos[1]", "floorHeight > m->pos[1]")}
    yield "geometry", "ignore_underwater_shell_ceiling", "geometry.underwater_shell_ascent", {
        path: MutationSource(root / path).replace_in_function("perform_ground_quarter_step",
            "waterLevel = MIN(waterLevel, ceilHeight - 161.0f);", "(void)ceilHeight;")}
    step_path = "src/game/mario_step.c"
    step_text = MutationSource(root / step_path)
    contact_brake = "            mario_restrict_air_uphill(m, floor);"
    yield "geometry", "omit_slope_contact_brake", "geometry.backward_jump_kick_slope_entry", {
        step_path: step_text.replace_unique(contact_brake, "            (void)0;")}
