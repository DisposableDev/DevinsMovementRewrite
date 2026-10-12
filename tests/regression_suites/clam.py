"""Production extraction and deliberate defects for the clam suite."""
from .common import prepare_common_constants
import re
from regression_support import HarnessError
from .mutation_support import MutationSource

def prepare_clam(root, source, definitions, functions):
    prepare_common_constants(source, definitions)
    definitions += [source("src/game/mario_he_thicc.h").macros(
        "HE_THICC_.*|ACT_ARG_WALL_KICK_HE_THICC|ACT_ARG_FORWARD_ROLLOUT_HE_THICC_BOUNCE")]
    definitions += [source("src/game/mario_actions_airborne.h").macros(
        "ACT_ARG_JUMP_BACKWARD_BUTT_SLIDE.*|ACT_ARG_JUMP_KICK_FOLLOWUP_QUEUED|ACT_ARG_JUMP_KICK_BACKWARD_BUTT_SLIDE.*|ACT_ARG_STEEP_JUMP_BACKWARD_BUTT_SLIDE_NO_UPHILL|ACT_ARG_DIVE_FROM_ROLLOUT_CONTACT")]
    definitions += [source("src/game/mario_actions_stationary.h").macros(
        "ACT_ARG_COUGHING_SHALLOW_WATER")]
    definitions += [source("include/config/config_movement.h").macros("BACKWARD_BUTT_SLIDE_JUMP_AIR_DRAG|BACKWARD_JUMP_KICK_UPHILL_DECEL")]
    definitions += [source("include/object_constants.h").macros("CLAM_.*")]
    definitions += [source("include/object_constants.h").declaration("enum", "ObjFlags")]
    definitions += [source("include/types.h").declaration("enum", "AnimFlags")]
    animations = []
    for number in ("05001654", "0500172C"):
        animation = source(f"actors/clam_shell/anims/anim_{number}.inc.c")
        header = re.search(r"static const struct Animation \w+\s*=\s*\{([^}]+)\}", animation.mask)
        if header is None:
            raise HarnessError(f"{animation.path}: clam animation header not found")
        flags, _, start, loop_start, loop_end = [int(value.strip(), 0)
            for value in header[1].split(",")[:5]]
        if flags != 1 or start != 0 or loop_start != 0 or loop_end < 3:
            raise HarnessError(f"{animation.path}: unsupported clam animation playback")
        animations.append((flags, start, loop_start, loop_end))
    definitions += ["static struct Animation sAnimations[] = {" +
                    ",".join("{" + ",".join(map(str, values)) + "}" for values in animations) + "};"]
    functions += [source("src/engine/graph_node.c").function("geo_obj_init_animation")]
    helpers = source("src/game/object_helpers.c")
    functions += [helpers.function(name) for name in (
        "cur_obj_init_animation_with_sound", "cur_obj_check_anim_frame",
        "cur_obj_become_tangible", "cur_obj_become_intangible", "obj_set_hitbox")]
    functions += [source("src/game/obj_behaviors_2.c").function("cur_obj_init_anim_check_frame")]
    functions += [source("src/game/behaviors/clam.inc.c").region(
        r"^struct ObjectHitbox sClamShellHitbox", r"\Z")]

def mutations_clam(root):
    path = "src/game/behaviors/clam.inc.c"
    clam = MutationSource(root / path)
    yield "clam", "keep_opening_dangerous_until_30", "clam.damage_boundaries", {
        path: clam.replace_in_function("clam_act_1", "CLAM_OPENING_SAFE_FRAME", "30")}
    yield "clam", "delay_closing_damage_until_28", "clam.damage_boundaries", {
        path: clam.replace_in_function("clam_act_0", "CLAM_CLOSING_DAMAGE_FRAME", "28")}
    yield "clam", "skip_clam_damage_when_frame_skipped", "clam.skipped_and_frozen_frames", {
        path: clam.replace_in_function("clam_act_0", ">= CLAM_CLOSING_DAMAGE_FRAME", "== CLAM_CLOSING_DAMAGE_FRAME")}
    yield "clam", "override_clam_spawn_safety", "clam.damage_boundaries", {
        path: clam.replace_in_function("bhv_clam_loop", "obj_set_hitbox(o, &sClamShellHitbox);", "")}

