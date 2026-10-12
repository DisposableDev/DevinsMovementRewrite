"""Production extraction and deliberate defects for the air_backward suite."""
from .common import prepare_common_constants
from .mutation_support import MutationSource

def prepare_air_backward(root, source, definitions, functions):
    prepare_common_constants(source, definitions)
    definitions += [source("src/game/mario_he_thicc.h").macros(
        "HE_THICC_.*|ACT_ARG_WALL_KICK_HE_THICC|ACT_ARG_FORWARD_ROLLOUT_HE_THICC_BOUNCE")]
    definitions += [source("src/game/mario_actions_airborne.h").macros(
        "ACT_ARG_JUMP_BACKWARD_BUTT_SLIDE.*|ACT_ARG_BUTT_SLIDE_JUMP_DIVE_.*|ACT_ARG_JUMP_KICK_FOLLOWUP_QUEUED|ACT_ARG_JUMP_KICK_BACKWARD_BUTT_SLIDE.*|ACT_ARG_STEEP_JUMP_BACKWARD_BUTT_SLIDE_NO_UPHILL|ACT_ARG_DIVE_FROM_ROLLOUT_CONTACT")]
    definitions += [source("src/game/mario_actions_stationary.h").macros(
        "ACT_ARG_COUGHING_SHALLOW_WATER")]
    definitions += [source("include/config/config_movement.h").macros("BACKWARD_BUTT_SLIDE_JUMP_AIR_DRAG|BACKWARD_JUMP_KICK_UPHILL_DECEL")]
    air = source("src/game/mario_actions_airborne.c")
    definitions += [source("src/game/mario_actions_airborne.c").macros("AIR_HIGH_SPEED_.*")]
    definitions += [source("include/config/config_movement.h").macros(
        "BACKWARD_AIR_.*|FORWARD_DIVE_.*|SLIDE_KICK_.*|SIDE_FLIP_.*|CAPLESS_CAP_PICKUP_AIR_DRAG|WALL_KICK_CHAIN_EXTRA_DRAG")]
    definitions += [air.macros("WALL_KICK_(DRAG_START_SPEED|BASE_AIR_DRAG|DRAG_ABOVE_40|SOFT_CAP|DRAG_ABOVE_SOFT_CAP|CHAIN_EXTRA_DRAG)")]
    functions += [source("src/engine/math_util.c").function("approach_f32")]
    functions += [source("src/game/mario.c").function("set_jumping_action")]
    functions += [source("src/game/mario_actions_moving.c").function("common_slide_action_with_jump")]
    functions += [air.function(name) for name in (
        "check_kick_or_dive_in_air", "backward_air_extra_drag",
        "apply_backward_butt_slide_jump_drag", "backward_butt_slide_jump_blocks_uphill",
        "mario_restrict_air_uphill", "backward_butt_slide_jump_restrict_uphill",
        "update_air_with_turn", "update_air_without_turn")]

def mutations_air_backward(root):
    moving_path = "src/game/mario_actions_moving.c"
    yield "air_backward", "remove_extra_butt_slide_delay", "air_backward.slope_rejump_delay", {
        moving_path: MutationSource(root / moving_path).replace_in_function("common_slide_action_with_jump",
            "jumpDelay = m->backwardButtSlideJumpCount >= 2\n"
            "            ? 9 : 3 * (m->backwardButtSlideJumpCount + 1);",
            "jumpDelay = 3;")}
    air_path = "src/game/mario_actions_airborne.c"
    yield "air_backward", "require_butt_slide_for_uphill_block", "air_backward.unmarked_jump_kick", {
        air_path: MutationSource(root / air_path).replace_in_function("backward_butt_slide_jump_blocks_uphill",
            "|| m->forwardVel < 0.0f", "|| FALSE").replace(
            "|| m->vel[0] * sins(m->faceAngle[1]) + m->vel[2] * coss(m->faceAngle[1]) < 0.0f", "|| FALSE")}
    yield "air_backward", "remove_held_speed_drag", "air_backward.held_back", {
        air_path: MutationSource(root / air_path).replace_in_function("backward_air_extra_drag",
            "extraDrag = MAX(extraDrag, previousForwardVel - maxGain - forwardVel);",
            "(void) maxGain;")}
    yield "air_backward", "butt_slide_drag_before_steering", "air_backward.butt_slide_takeoff_sequence", {
        air_path: MutationSource(root / air_path).replace_in_function("update_air_without_turn",
            "        apply_backward_butt_slide_jump_drag(m);", "").replace(
            "        f32 airDrag = 0.35f / momentumRetention;",
            "        apply_backward_butt_slide_jump_drag(m);\n        f32 airDrag = 0.35f / momentumRetention;")}

