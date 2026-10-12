"""Production extraction and deliberate defects for the switches suite."""
from .common import prepare_common_constants
from .mutation_support import MutationSource


def prepare_switches(root, source, definitions, functions):
    definitions += [source("include/sounds.h").macros("SOUNDARGS_MASK_PRIORITY|SOUNDARGS_SHIFT_PRIORITY")]
    prepare_common_constants(source, definitions, gameplay_options=True)
    definitions += [source("src/game/mario_he_thicc.h").macros(
        "HE_THICC_.*|ACT_ARG_WALL_KICK_HE_THICC|ACT_ARG_FORWARD_ROLLOUT_HE_THICC_BOUNCE")]
    definitions += [source("src/game/mario_actions_airborne.h").macros(
        "ACT_ARG_JUMP_BACKWARD_BUTT_SLIDE.*|ACT_ARG_JUMP_KICK_FOLLOWUP_QUEUED|ACT_ARG_JUMP_KICK_BACKWARD_BUTT_SLIDE.*|ACT_ARG_STEEP_JUMP_BACKWARD_BUTT_SLIDE_NO_UPHILL|ACT_ARG_DIVE_FROM_ROLLOUT_CONTACT")]
    definitions += [source("src/game/mario_actions_stationary.h").macros(
        "ACT_ARG_COUGHING_SHALLOW_WATER")]
    definitions += [source("include/config/config_movement.h").macros("BACKWARD_BUTT_SLIDE_JUMP_AIR_DRAG|BACKWARD_JUMP_KICK_UPHILL_DECEL")]
    functions += [source("src/game/object_helpers.c").function("obj_set_blink_phase")]
    definitions += [source("src/engine/graph_node.h").declaration("enum", "GraphRenderFlags")]
    definitions += [source("src/game/interaction.h").declaration("enum", "Interactions")]
    definitions += [source("include/object_constants.h").macros("PURPLE_SWITCH_.*")]
    definitions += [source("include/object_constants.h").declaration("enum", name)
                    for name in ("oActionsCapSwitch", "oActionsBlueCoinSwitch")]
    definitions += [source("src/game/save_file.h").declaration("enum", "SaveProgressFlags")]
    definitions += [source("src/game/behaviors/capswitch.inc.c").region(
        r"^static s32 sCapSaveFlags", r"^void cap_switch_act_init")]
    sm64 = source("include/sm64.h")
    definitions += [sm64.declaration("enum", name) for name in (
        "MarioInput", "MarioParticleFlags", "MarioFlags")]
    definitions += [sm64.macros("ACT_GROUND_POUND_LAND|ACT_WATER_DOWN_BURST|ACT_WATER_ACTION_END|ACT_FLUTTER_KICK|ACT_FORWARD_WATER_KB")]
    definitions += [source("src/game/mario_actions_submerged.h").declaration(
        "enum", "ActionStatesWaterDownBurst"),
        source("src/game/mario_actions_submerged.h").declaration("enum", "ActionArgsWaterActionEnd"),
        source("src/game/mario_actions_submerged.h").declaration("enum", "ActionArgsFlutterKick")]
    functions += [source("src/game/object_helpers.c").function(name) for name in (
        "cur_obj_enable_rendering", "cur_obj_disable_rendering", "cur_obj_unhide",
        "cur_obj_become_tangible", "cur_obj_become_intangible", "cur_obj_wait_then_blink",
        "mario_has_ground_pound_impact", "cur_obj_is_mario_ground_pounding_platform")]
    definitions += [source("include/config/config_movement.h").macros("WATER_DOWN_IMPACT_COOLDOWN_FRAMES")]
    functions += [source("src/game/mario_actions_submerged.c").function("finish_water_down_burst")]
    functions += [source("src/game/behaviors/purple_switch.inc.c").function("bhv_purple_switch_loop")]
    functions += [source("src/game/behaviors/capswitch.inc.c").function("cap_switch_act_idle_unpressed")]
    # Alternative preprocessor branches share closing braces; compile the complete
    # final function rather than asking the lexical extractor to balance both.
    functions += [source("src/game/behaviors/blue_coin.inc.c").region(
        r"^void bhv_blue_coin_switch_loop", r"\Z")]
    functions += [source("src/game/behaviors/animated_floor_switch.inc.c").function("bhv_animates_on_floor_switch_press_loop")]
    boxes = source("src/game/behaviors/breakable_box.inc.c")
    functions += [boxes.function(name) for name in (
        "breakable_box_init", "hidden_box_switch_is_valid", "find_hidden_box_switch",
        "hidden_box_new_switch_press", "hidden_box_update_lifetime",
        "hidden_breakable_box_actions", "hidden_unbreakable_box_actions")]

    signs = source("src/game/behaviors/message_panel.inc.c")
    definitions += [signs.macros("SIGNPOST_.*")]
    functions += [signs.function("bhv_message_panel_loop")]


def mutations_switches(root):
    path = "src/game/behaviors/purple_switch.inc.c"
    yield "switches", "original_accepts_extended_impact", "switches.presets", {
        path: MutationSource(root / path).replace_in_function("bhv_purple_switch_loop",
            "game_options_get(GO_SWITCHES) == 0", "TRUE")}
