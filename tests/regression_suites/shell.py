"""Production extraction and deliberate defects for the shell suite."""
from .common import prepare_common_constants
from .mutation_support import MutationSource

def prepare_shell(root, source, definitions, functions):
    prepare_common_constants(source, definitions, gameplay_options=True, cap_functions=functions)
    functions += [source("src/game/mario.c").function(name) for name in
                  ("mario_keeps_shell", "mario_blocks_harmful_knockback")]
    definitions += [source("include/sm64.h").declaration("enum", "MarioFlags")]
    definitions += [source("src/game/mario_he_thicc.h").macros(
        "HE_THICC_.*|ACT_ARG_WALL_KICK_HE_THICC|ACT_ARG_FORWARD_ROLLOUT_HE_THICC_BOUNCE")]
    definitions += [source("src/game/mario_actions_airborne.h").macros(
        "ACT_ARG_JUMP_BACKWARD_BUTT_SLIDE.*|ACT_ARG_JUMP_KICK_FOLLOWUP_QUEUED|ACT_ARG_JUMP_KICK_BACKWARD_BUTT_SLIDE.*|ACT_ARG_STEEP_JUMP_BACKWARD_BUTT_SLIDE_NO_UPHILL|ACT_ARG_DIVE_FROM_ROLLOUT_CONTACT")]
    definitions += [source("src/game/mario_actions_stationary.h").macros(
        "ACT_ARG_COUGHING_SHALLOW_WATER")]
    definitions += [source("include/config/config_movement.h").macros("BACKWARD_BUTT_SLIDE_JUMP_AIR_DRAG|BACKWARD_JUMP_KICK_UPHILL_DECEL")]
    definitions += [source("include/sm64.h").macros("ACT_FLUTTER_KICK|ACT_WATER_THROW|ACT_WATER_JUMP|ACT_HOLD_WATER_JUMP")]
    definitions += [source("src/game/mario_actions_submerged.h").declaration("enum", "ActionArgsFlutterKick")]
    definitions += [source("include/config/config_movement.h").macros("SWIM_DRAG")]
    functions += [source("src/game/mario.c").function("mario_stop_underwater_shell"),
                  source("src/game/mario_actions_submerged.c").function("check_water_jump"),
                  source("src/game/mario_actions_submerged.c").function("act_water_shell_swimming")]
    definitions += [source("include/sm64.h").declaration("enum", "GroundStep")]
    definitions += [source("include/sm64.h").macros("ACT_RIDING_SHELL_JUMP|ACT_CROUCH_SLIDE|ACT_BACKWARD_GROUND_KB")]
    functions += [source("src/game/interaction.c").function("mario_stop_riding_object"),
                  source("src/game/mario_actions_moving.c").function("act_riding_shell_ground")]
    functions += [source("src/game/mario.c").function("mario_has_land_movement_underwater")]
    interactions = source("src/game/interaction.c")
    mario = source("src/game/mario.c")
    underwater_shell = source("src/game/behaviors/koopa_shell_underwater.inc.c")
    definitions += [underwater_shell.region(r"^struct ObjectHitbox sKoopaShellUnderwaterHitbox", r"^void set_koopa_shell_underwater_hitbox")]
    functions += [interactions.function("interact_grabbable"),
                  interactions.function("interact_koopa_shell")]
    functions += [source("src/game/behaviors/koopa_shell.inc.c").function("check_shell_despawn")]
    functions += [mario.function("mario_is_riding_underwater_shell"),
                  mario.function("mario_transition_underwater_shell_to_riding"),
                  mario.function("mario_transition_underwater_shell_to_swimming")]
    functions += [underwater_shell.function("set_koopa_shell_underwater_hitbox"),
                  underwater_shell.function("bhv_koopa_shell_underwater_loop")]

def mutations_shell(root):
    path = "src/game/interaction.c"
    yield "shell", "remount_thrown_shell", "shell.released_shell_cannot_remount", {
        path: MutationSource(root / path).replace_in_function("interact_grabbable",
            "&& obj->oHeldState == HELD_FREE", "")}
    path = "src/game/mario.c"
    yield "shell", "drop_shell_on_cap_expiry", "shell.cap_expiry_handoff", {
        path: MutationSource(root / path).replace_in_function("mario_transition_underwater_shell_to_swimming",
            "m->heldObj = shell;", "m->heldObj = NULL;")}
