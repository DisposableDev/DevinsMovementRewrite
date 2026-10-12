"""Production extraction and deliberate defects for the sideflip suite."""
from .common import prepare_common_constants
from .mutation_support import MutationSource

def prepare_sideflip(root, source, definitions, functions):
    prepare_common_constants(source, definitions)
    definitions += [source("include/sm64.h").macros("ACT_BUTT_SLIDE")]
    definitions += [source("src/game/mario_he_thicc.h").macros(
        "HE_THICC_.*|ACT_ARG_WALL_KICK_HE_THICC|ACT_ARG_FORWARD_ROLLOUT_HE_THICC_BOUNCE")]
    definitions += [source("src/game/mario_actions_airborne.h").macros(
        "ACT_ARG_JUMP_BACKWARD_BUTT_SLIDE.*|ACT_ARG_BUTT_SLIDE_JUMP_DIVE_.*|ACT_ARG_JUMP_KICK_FOLLOWUP_QUEUED|ACT_ARG_JUMP_KICK_BACKWARD_BUTT_SLIDE.*|ACT_ARG_STEEP_JUMP_BACKWARD_BUTT_SLIDE_NO_UPHILL|ACT_ARG_DIVE_FROM_ROLLOUT_CONTACT")]
    definitions += [source("src/game/mario_actions_stationary.h").macros(
        "ACT_ARG_COUGHING_SHALLOW_WATER")]
    definitions += [source("include/config/config_movement.h").macros("BACKWARD_BUTT_SLIDE_JUMP_AIR_DRAG|BACKWARD_JUMP_KICK_UPHILL_DECEL")]
    definitions += [source("src/game/mario_actions_moving.h").macros("ACT_ARG_WALKING_TURN_FROM_PUNCH")]
    moving = source("src/game/mario_actions_moving.c")
    punch = source("src/game/mario_punch.c")
    definitions += [moving.macros("SIDE_FLIP_.*|analog_stick_held_back")]
    definitions += [punch.macros("HELD_B_PUNCH_FOLLOWUP_FRAME|GROUND_KICK_RECOVERY_START_FRAME")]
    definitions += [source("src/game/mario_punch.h").declaration("enum", "ActionArgsPunchSequence")]
    functions += [moving.function(name) for name in (
        "start_moving_crouch_attack", "check_ground_dive_or_punch",
        "begin_walking_action", "turning_side_flip_window_open", "act_turning_around",
        "act_finish_turning_around")]
    definitions += [source("include/config/config_movement.h").macros("MARIO_STRONG_STICK_THRESHOLD")]
    air = source("src/game/mario_actions_airborne.c")
    definitions += [air.macros("SIDE_FLIP_JUMP_KICK_LOCK_FRAMES")]
    functions += [air.function(name) for name in (
        "mario_has_dive_input", "check_kick_or_dive_in_air", "act_side_flip")]
    functions += [punch.function(name) for name in (
        "punch_recovery_is_open", "cancel_punch_recovery")]

def mutations_sideflip(root):
    punch_path = "src/game/mario_punch.c"
    yield "sideflip", "allow_immediate_punch_side_flip", "sideflip.punch_entry", {
        punch_path: MutationSource(root / punch_path).replace_in_function("cancel_punch_recovery",
            "return set_jumping_action(m, ACT_JUMP, 0);",
            "return set_jumping_action(m, ACT_SIDE_FLIP, 0);")}
    moving_path = "src/game/mario_actions_moving.c"
    for function in ("act_turning_around", "act_finish_turning_around"):
        for label, old in (("attack", "if (check_ground_dive_or_punch(m))"),
                           ("crouch", "if (m->input & INPUT_Z_DOWN)")):
            yield "sideflip", function + "_omit_" + label, "sideflip.turn_controls", {
                moving_path: MutationSource(root / moving_path).replace_in_function(function, old, "if (FALSE)")}

