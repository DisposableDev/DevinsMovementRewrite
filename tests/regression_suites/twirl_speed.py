"""Production extraction and deliberate defects for the twirl_speed suite."""
from .common import prepare_common_constants
from .mutation_support import MutationSource

def prepare_twirl_speed(root, source, definitions, functions):
    prepare_common_constants(source, definitions, gameplay_options=True, cap_functions=functions)
    definitions += [source("src/game/ingame_menu.h").declaration("enum", "MenuMode"),
                    "static s16 gMenuMode = MENU_MODE_NONE;"]
    definitions += [source("src/game/mario_he_thicc.h").macros(
        "HE_THICC_.*|ACT_ARG_WALL_KICK_HE_THICC|ACT_ARG_FORWARD_ROLLOUT_HE_THICC_BOUNCE")]
    definitions += [source("src/game/mario_actions_airborne.h").macros(
        "ACT_ARG_JUMP_BACKWARD_BUTT_SLIDE.*|ACT_ARG_JUMP_KICK_FOLLOWUP_QUEUED|ACT_ARG_JUMP_KICK_BACKWARD_BUTT_SLIDE.*|ACT_ARG_STEEP_JUMP_BACKWARD_BUTT_SLIDE_NO_UPHILL|ACT_ARG_DIVE_FROM_ROLLOUT_CONTACT")]
    definitions += [source("src/game/mario_actions_stationary.h").macros(
        "ACT_ARG_COUGHING_SHALLOW_WATER")]
    definitions += [source("include/config/config_movement.h").macros("BACKWARD_BUTT_SLIDE_JUMP_AIR_DRAG|BACKWARD_JUMP_KICK_UPHILL_DECEL")]
    header = source("src/game/mario_character.h")
    definitions += [header.region(r"^struct MarioState;", r"^#endif")]
    definitions += [source("include/config/config_movement.h").macros(
        "WALK_TARGET_SPEED|JUMP_HEIGHT_VELOCITY_SCALE|JUMP_FORWARD_SPEED_RETENTION")]
    character = source("src/game/mario_character.c")
    functions += [character.region(r"^const struct MarioMovementProfile gMarioMovementProfiles", r"^s32 mario_character_can_unlock")]
    functions += [character.region(r"^void mario_character_commit", r"\Z")]
    definitions += [source("include/sm64.h").declaration("enum", "MarioFlags")]
    definitions += [source("include/sm64.h").declaration("enum", "MarioParticleFlags")]
    definitions += [source("include/sm64.h").declaration("enum", "MarioActionFlags")]
    definitions += [source("include/sounds.h").macros("SOUND_TERRAIN_.*")]
    twirl = source("src/game/mario_ground_twirl.c")
    moving = source("src/game/mario_actions_moving.c")
    definitions += [twirl.macros("GROUND_TWIRL_.*")]
    definitions += [source("src/game/mario_actions_moving.h").macros(
        "GROUND_TWIRL_TARGET_SPEED|ACT_ARG_WALKING_TURN_FROM_PUNCH")]
    definitions += [source("include/config/config_movement.h").macros(
        "WALK_.*TARGET_SPEED|WALK_STICK_NEGATIVE_DECEL|VELOCITY_BASED_TURN_SPEED")]
    definitions += [source("src/engine/math_util.h").macros(
        "approach_angle_bool|approach_s16_symmetric_bool")]
    functions += [source("src/engine/math_util.c").function(name) for name in (
        "approach_s16", "approach_s16_bool")]
    functions += [moving.function("should_brake_negative_ground_speed")]
    functions += [moving.function("update_walking_speed_with_minimum")]
    functions += [twirl.function(name) for name in (
        "mario_ground_twirl_limit_speed", "mario_ground_twirl_drag_speed",
        "mario_ground_twirl_add_speed", "mario_ground_twirl_boost_for_age",
        "mario_ground_twirl_update_speed", "mario_ground_twirl_handle_wall")]

def mutations_twirl_speed(root):
    path = "src/game/mario_actions_moving.c"
    yield "twirl_speed", "snap_low_speed_punch_turn", "twirl_speed.punch_recovery_turn", {
        path: MutationSource(root / path).replace_in_function("update_walking_speed_with_minimum",
            "m->faceAngle[1] = approach_s16(m->faceAngle[1], m->intendedYaw, 0x800, 0x800);",
            "m->faceAngle[1] = m->intendedYaw;")}
    yield "twirl_speed", "retain_punch_turn_after_alignment", "twirl_speed.punch_recovery_turn", {
        path: MutationSource(root / path).replace_in_function("update_walking_speed_with_minimum",
            "m->actionArg = 0;", "m->actionArg = ACT_ARG_WALKING_TURN_FROM_PUNCH;")}
    yield "twirl_speed", "restore_vanilla_walk", "twirl_speed.character_profiles", {
        path: MutationSource(root / path).replace_in_function("update_walking_speed_with_minimum",
            "maxTargetSpeed = trialWalk ? WALK_DEBUG_TARGET_SPEED : walkTarget;",
            "maxTargetSpeed = trialWalk ? WALK_DEBUG_TARGET_SPEED : 32.0f;")}
    yield "twirl_speed", "bypass_debug_walk_consent", "twirl_speed.character_profiles", {
        path: MutationSource(root / path).replace_in_function("update_walking_speed_with_minimum",
            "normalWalk && gDebugFastWalk && game_options_debug_active()",
            "normalWalk && gDebugFastWalk")}
    twirl_path = "src/game/mario_ground_twirl.c"
    yield "twirl_speed", "lose_speed_as_wall_bonus_fades", "twirl_speed.wall_recovery", {
        twirl_path: MutationSource(root / twirl_path).replace_in_function("mario_ground_twirl_update_speed",
            "mario_ground_twirl_add_speed(m, minimumSpeed - m->forwardVel);", "(void) minimumSpeed;")}

