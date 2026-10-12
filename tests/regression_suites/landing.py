"""Production extraction and deliberate defects for the landing suite."""
from .common import prepare_common_constants
from .mutation_support import MutationSource

def prepare_landing(root, source, definitions, functions):
    prepare_common_constants(source, definitions, gameplay_options=True, cap_functions=functions)
    definitions += [source("include/config/config_movement.h").macros("WALL_BONK_MOMENTUM_RETENTION")]
    definitions += [source("include/config/config_movement.h").macros("MARIO_STRONG_STICK_THRESHOLD")]
    functions += [source("src/game/mario_actions_airborne.c").function("mario_has_dive_input")]
    functions += [source("src/game/mario_step.c").function("mario_set_wall_bonk_momentum")]
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
                    air_header.macros("BACKWARD_JUMP_WALL_KICK_DELAY_FRAMES")]
    functions += [source("src/game/mario_actions_airborne.c").function(
        "mario_can_wall_kick_backward_jump")]
    air = source("src/game/mario_actions_airborne.c")
    definitions += [air.macros("HANDSTAND_FALL_GRACE_FRAMES|SLIDE_KICK_FALL_ANIM_ACCEL"),
                    air.declaration("enum", "HandstandFallState")]
    definitions += [source("src/engine/math_util.h").macros("absf")]
    sm64 = source("include/sm64.h")
    definitions += [sm64.declaration("enum", name) for name in (
        "GroundStep", "AirStep", "AirStepCheck", "MarioParticleFlags", "MarioActionGroups", "MarioActionFlags", "MarioFlags", "MarioInput")]
    definitions += [sm64.macros("ACT_.*|COS25")]
    definitions += [source("include/mario_animation_ids.h").declaration("enum", "MarioAnimID")]
    definitions += [source("src/game/mario.h").declaration("enum", "MarioSpawnFallProtection")]
    definitions += [source("include/config/config_movement.h").macros("MARIO_GROUND_AIR_COLLISION_.*")]
    step = source("src/game/mario_step.c")
    definitions += [step.macros("MARIO_GROUND_STEP_MAX_FLOOR_DELTA")]
    functions += [step.function(name) for name in (
        "get_mario_ground_vertical_travel", "get_mario_ground_air_collision_steps",
        "get_mario_ground_remaining_steps", "perform_ground_step_internal",
        "perform_ground_step_with_fraction", "perform_ground_step", "perform_ground_step_stop_at_ledge",
        "perform_air_step_internal", "perform_air_step_with_remainder", "perform_air_step_with_fraction", "perform_air_step")]
    functions += [air.function(name) for name in (
        "wall_kick_priority_step_arg", "common_air_action_step", "act_top_of_pole_jump", "act_air_throw")]
    definitions += [source("include/config/config_movement.h").macros("GROUND_NEUTRAL_.*_DECEL"),
                    source("src/game/mario_actions_moving.h").macros("ACT_ARG_GROUND_SPEED_FROM_ROLLOUT")]
    functions += [source("src/engine/math_util.c").function("approach_f32")]
    functions += [step.function("stationary_ground_step")]
    functions += [source("src/game/mario_actions_moving.c").function(name) for name in (
        "update_decelerating_speed", "coasting_ground_step")]
    functions += [source("src/game/mario_actions_stationary.c").function("act_air_throw_land")]


    definitions += [source("include/types.h").declaration("enum", "AnimFlags")]
    functions += [source("src/game/mario.c").function("set_anim_to_frame")]
    definitions += [source("src/game/interaction.h").declaration("enum", "InteractStatus")]
    functions += [source("src/game/mario.c").function("play_sound_if_no_flag")]
    functions += [source("src/game/mario.c").function(name) for name in (
        "play_sound_and_spawn_particles", "play_mario_action_sound", "play_mario_landing_sound_once")]
    functions += [source("src/game/mario_actions_automatic.c").function(name) for name in (
        "update_ledge_climb_camera", "update_ledge_climb", "act_ledge_grab", "act_ledge_climb_down",
        "act_ledge_climb_slow", "act_ledge_climb_fast")]

def mutations_landing(root):
    for name, path, function, old, new in (
        ("throw_landing_stop", "src/game/mario_actions_stationary.c", "act_air_throw_land",
         "coasting_ground_step(m)", "stationary_ground_step(m)"),
        ("discard_throw_landing_travel", "src/game/mario_actions_airborne.c", "act_air_throw",
         "if (remaining > 0.0f)", "if (FALSE && remaining > 0.0f)"),
        ("reset_throw_landing_clock", "src/game/mario_actions_airborne.c", "act_air_throw",
         "m->action = ACT_AIR_THROW_LAND;", "set_mario_action(m, ACT_AIR_THROW_LAND, 0);"),
        ("stop_after_throw_recovery", "src/game/mario_actions_stationary.c", "act_air_throw_land",
         "absf(m->forwardVel) >= 1.0f ? ACT_DECELERATING : ACT_IDLE", "ACT_IDLE"),
    ):
        yield "landing", name, "landing.air_throw_touchdown", {
            path: MutationSource(root / path).replace_in_function(function, old, new)}
    action_path = "src/game/mario_actions_airborne.c"
    yield "landing", "late_handstand_landing_pose", "landing.handstand_touchdown", {
        action_path: MutationSource(root / action_path).replace_in_function("act_top_of_pole_jump",
            "set_mario_animation(m, MARIO_ANIM_GENERAL_LAND);", "(void)0;")}
    yield "landing", "voice_on_level_ledge_catch", "landing.ledge_voice", {
        action_path: MutationSource(root / action_path).replace_in_function("common_air_action_step",
            "m->vel[1] <= -LEDGE_GRAB_VOICE_MIN_FALL_SPEED ? 0 : ACT_ARG_LEDGE_GRAB_GENTLE",
            "0")}
    yield "landing", "voice_on_slight_ledge_descent", "landing.ledge_voice", {
        action_path: MutationSource(root / action_path).replace_in_function("common_air_action_step",
            "m->vel[1] <= -LEDGE_GRAB_VOICE_MIN_FALL_SPEED", "m->vel[1] < 0.0f")}
    automatic_path = "src/game/mario_actions_automatic.c"
    yield "landing", "silent_intentional_climb_down", "landing.ledge_climb_down_voice", {
        automatic_path: MutationSource(root / automatic_path).replace_in_function("act_ledge_climb_down",
            "play_sound_if_no_flag(m, SOUND_MARIO_HRMM, MARIO_MARIO_SOUND_PLAYED);", "(void)0;")}
    yield "landing", "repeat_climb_down_voice", "landing.ledge_climb_down_voice", {
        automatic_path: MutationSource(root / automatic_path).replace_in_function("act_ledge_climb_down",
            "play_sound_if_no_flag(m, SOUND_MARIO_HRMM, MARIO_MARIO_SOUND_PLAYED);",
            "play_sound(SOUND_MARIO_HRMM, m->marioObj->header.gfx.cameraToObject);")}
    yield "landing", "whoa_on_intentional_climb_down", "landing.ledge_climb_down_voice", {
        automatic_path: MutationSource(root / automatic_path).replace_in_function("act_ledge_climb_down",
            "SOUND_MARIO_HRMM", "SOUND_MARIO_WHOA")}
    yield "landing", "old_uh_on_intentional_climb_down", "landing.ledge_climb_down_voice", {
        automatic_path: MutationSource(root / automatic_path).replace_in_function("act_ledge_climb_down",
            "SOUND_MARIO_HRMM", "SOUND_MARIO_UH")}
    yield "landing", "whoa_after_climb_down_handoff", "landing.ledge_climb_down_voice", {
        automatic_path: MutationSource(root / automatic_path).replace_in_function("act_ledge_climb_down",
            "m->actionArg = ACT_ARG_LEDGE_GRAB_SILENT;", "m->actionArg = 0;")}
    path = "src/game/mario_step.c"
    yield "landing", "discard_landing_movement", "landing.travel", {
        path: MutationSource(root / path).replace_in_function("perform_air_step_internal",
            "*remaining = (f32)(numSteps - i - 1) / (f32) numSteps;", "*remaining = 0.0f;")}
