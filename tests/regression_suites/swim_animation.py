"""Production extraction and deliberate defects for the swim_animation suite."""
from .common import prepare_common_constants
from .mutation_support import MutationSource

def prepare_swim_animation(root, source, definitions, functions):
    prepare_common_constants(source, definitions, gameplay_options=True)
    definitions += [source("src/game/mario_he_thicc.h").macros(
        "HE_THICC_.*|ACT_ARG_WALL_KICK_HE_THICC|ACT_ARG_FORWARD_ROLLOUT_HE_THICC_BOUNCE")]
    definitions += [source("src/game/mario_actions_airborne.h").macros(
        "ACT_ARG_JUMP_BACKWARD_BUTT_SLIDE.*|ACT_ARG_JUMP_KICK_FOLLOWUP_QUEUED|ACT_ARG_JUMP_KICK_BACKWARD_BUTT_SLIDE.*|ACT_ARG_STEEP_JUMP_BACKWARD_BUTT_SLIDE_NO_UPHILL|ACT_ARG_DIVE_FROM_ROLLOUT_CONTACT")]
    definitions += [source("src/game/mario_actions_stationary.h").macros(
        "ACT_ARG_COUGHING_SHALLOW_WATER")]
    definitions += [source("include/config/config_movement.h").macros("BACKWARD_BUTT_SLIDE_JUMP_AIR_DRAG|BACKWARD_JUMP_KICK_UPHILL_DECEL")]
    sm64 = source("include/sm64.h")
    definitions += [sm64.macros("ACT_.*")]
    definitions += [sm64.declaration("enum", name) for name in (
        "MarioActionFlags", "MarioInput", "MarioFlags")]
    types = source("include/types.h")
    definitions += [types.declaration("enum", "AnimFlags"),
                    types.declaration("struct", "Animation"),
                    types.declaration("struct", "AnimInfo"), types.macros("ANIMINDEX_NUMPARTS")]
    definitions += [source("include/mario_animation_ids.h").declaration("enum", "MarioAnimID")]
    definitions += [source("src/game/mario_actions_submerged.h").declaration("enum", name)
                    for name in ("ActionStatesBreaststroke", "ActionArgsFlutterKick", "ActionStatesWaterPunch")]
    definitions += [source("include/config/config_movement.h").macros(
        "MIN_SWIM_SPEED|SWIM_SOFT_SPEED_LIMIT|SWIM_DRAG|SWIM_RECOVERY_DECEL|"
        "A_SWIM_.*|AIR_ENTRY_SWIM_STROKE_TAP_FRAMES|B_SWIM_.*|BREASTSTROKE_.*|WATER_JUMP_MIN_UPWARD_SPEED|WATER_DOWN_.*")]
    submerged = source("src/game/mario_actions_submerged.c")
    definitions += [submerged.macros("MIN_SWIM_STRENGTH|MAX_SWIM_STRENGTH|BREASTSTROKE_.*|SWIMMING_ROLL_FRAMES")]
    for name in ("AA", "AB", "AC", "B2", "9F", "A0", "A1", "A4", "3C_3D"):
        asset = source(f"assets/anims/anim_{name}.inc.c")
        split = asset.mask.index(f"static const u16 anim_{name}_indices")
        definitions += [asset.mapped(split, len(asset.text)),
                        '#pragma GCC diagnostic push\n#pragma GCC diagnostic ignored "-Wmissing-braces"',
                        asset.mapped(0, split), '#pragma GCC diagnostic pop']
    functions += [submerged.region(r"^static struct \{", r"^static s16 sWaterCurrentSpeeds")]
    functions += [submerged.function(name) for name in (
        "is_normal_swimming_action", "cancel_swimming_roll", "update_swimming_roll",
        "center_swimming_roll", "apply_swimming_roll", "is_a_swimming_action", "can_control_swimming",
        "b_swim_has_priority", "update_swim_button_priority", "clear_initial_a_tap",
        "update_initial_a_tap", "clear_air_entry_stroke", "update_air_entry_stroke",
        "clear_interrupted_swimming_controls", "update_swimming_b_tap", "update_water_down_repeat_buffer", "mario_swimming_update", "check_swimming_b_input",
        "check_water_jump", "check_initial_swim_stroke", "start_a_swimming", "act_water_idle",
        "act_hold_water_idle", "act_breaststroke", "act_hold_breaststroke", "act_swimming_end",
        "act_hold_swimming_end", "act_flutter_kick", "act_hold_flutter_kick")]
    functions += [source("src/game/mario.c").function(name) for name in (
        "set_mario_animation", "set_mario_anim_with_accel", "set_anim_to_frame", "is_anim_at_end")]
    functions += [source("src/engine/graph_node.c").function(name) for name in ("geo_update_animation_frame", "retrieve_animation_index")]

def mutations_swim_animation(root):
    path = "src/game/mario_actions_submerged.c"
    for function, name in (("act_flutter_kick", "flash_free_flutter_pose"),
                           ("act_hold_flutter_kick", "flash_held_flutter_pose")):
        yield "swim_animation", name, "swim_animation.initial_tap", {
            path: MutationSource(root / path).replace_in_function(function,
                "!sSwimming.aTapPending", "(!sSwimming.aTapPending || sSwimming.aTapFrames >= 2)")}

