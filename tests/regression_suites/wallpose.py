"""Production extraction and deliberate defects for the wallpose suite."""
from .common import prepare_common_constants
from .mutation_support import MutationSource

def prepare_wallpose(root, source, definitions, functions):
    pose = source("src/game/mario_render_pose.h")
    definitions += [pose.declaration("enum", "MarioPoseRootSpace"),
                    pose.declaration("struct", "MarioRenderPose")]
    prepare_common_constants(source, definitions)
    functions += [
        "s32 mario_he_thicc_render_pose(struct AnimInfo *info, struct MarioRenderPose *pose) { return FALSE; }",
    ]
    definitions += [source("src/game/mario_he_thicc.h").macros(
        "HE_THICC_.*|ACT_ARG_WALL_KICK_HE_THICC|ACT_ARG_FORWARD_ROLLOUT_HE_THICC_BOUNCE")]
    definitions += [source("src/game/mario_actions_airborne.h").macros(
        "ACT_ARG_JUMP_BACKWARD_BUTT_SLIDE.*|ACT_ARG_JUMP_KICK_FOLLOWUP_QUEUED|ACT_ARG_JUMP_KICK_BACKWARD_BUTT_SLIDE.*|ACT_ARG_STEEP_JUMP_BACKWARD_BUTT_SLIDE_NO_UPHILL|ACT_ARG_DIVE_FROM_ROLLOUT_CONTACT")]
    definitions += [source("src/game/mario_actions_stationary.h").macros(
        "ACT_ARG_COUGHING_SHALLOW_WATER")]
    definitions += [source("include/config/config_movement.h").macros("BACKWARD_BUTT_SLIDE_JUMP_AIR_DRAG|BACKWARD_JUMP_KICK_UPHILL_DECEL")]
    air = source("src/game/mario_actions_airborne.c")
    definitions += [air.macros("HANDSTAND_FALL_GRACE_FRAMES"),
                    air.declaration("enum", "HandstandFallState")]
    sm64 = source("include/sm64.h")
    definitions += [sm64.declaration("enum", name) for name in (
        "MarioInput", "AirStep", "AirStepCheck")]
    definitions += [sm64.macros("ACT_.*")]
    definitions += [source("src/game/mario_actions_automatic.h").declaration("enum", "PolePositions")]
    definitions += [source("src/game/mario_dab.h").macros("DAB_ANALOG_.*")]
    types = source("include/types.h")
    definitions += [types.declaration("enum", "AnimFlags"),
                    types.declaration("struct", "Animation"),
                    types.declaration("struct", "AnimInfo")]
    definitions += [source("include/mario_animation_ids.h").declaration("enum", "MarioAnimID")]
    definitions += [source("src/game/rendering_graph_node.h").declaration("enum", "AnimType")]
    definitions += [types.macros("ANIMINDEX_NUMPARTS")]
    dab_asset = source("assets/anims/anim_D1.inc.c")
    dab_split = dab_asset.mask.index("static const u16 anim_D1_indices")
    definitions += [dab_asset.mapped(dab_split, len(dab_asset.text)),
                    '#pragma GCC diagnostic push\n#pragma GCC diagnostic ignored "-Wmissing-braces"',
                    dab_asset.mapped(0, dab_split), '#pragma GCC diagnostic pop']
    asset = source("assets/anims/anim_CB_CC.inc.c")
    split = asset.mask.index("static const u16 anim_CB_CC_indices")
    definitions += [asset.mapped(split, len(asset.text)),
                    '#pragma GCC diagnostic push\n#pragma GCC diagnostic ignored "-Wmissing-braces"',
                    asset.mapped(0, split), '#pragma GCC diagnostic pop']
    side_flip = source("assets/anims/anim_BF.inc.c")
    side_split = side_flip.mask.index("static const u16 anim_BF_indices")
    definitions += [side_flip.mapped(side_split, len(side_flip.text)),
                    '#pragma GCC diagnostic push\n#pragma GCC diagnostic ignored "-Wmissing-braces"',
                    side_flip.mapped(0, side_split), '#pragma GCC diagnostic pop']
    for name in ("04", "C1", "6F_70", "91", "09", "0A", "0B_0C", "56_57", "88_89", "3B", "3C_3D"):
        flip = source(f"assets/anims/anim_{name}.inc.c")
        split = flip.mask.index(f"static const u16 anim_{name}_indices")
        definitions += [flip.mapped(split, len(flip.text)),
                        '#pragma GCC diagnostic push\n#pragma GCC diagnostic ignored "-Wmissing-braces"',
                        flip.mapped(0, split), '#pragma GCC diagnostic pop']
    blend = source("src/game/mario_transition_pose.c")
    functions += [blend.region(r"^#define WALL_KICK_BLEND_FRAMES", r"\Z")]
    functions += [source("src/game/mario_ground_pound_animation.c").region(
        r"^#define GROUND_POUND_POSE_CHANNELS", r"\Z")]
    functions += [source("src/game/mario.c").function(name) for name in (
        "set_mario_animation", "set_mario_anim_with_accel", "set_anim_to_frame", "is_anim_at_end")]
    functions += [air.function("act_top_of_pole_jump")]
    automatic = source("src/game/mario_actions_automatic.c")
    functions += [automatic.function(name) for name in (
        "pole_jump_anim_y_translation", "act_top_of_pole")]
    functions += [source("src/engine/graph_node.c").function(name) for name in (
        "retrieve_animation_index", "geo_update_animation_frame")]
    functions += [source("src/game/rendering_graph_node.c").function("geo_set_animation_globals")]

def mutations_wallpose(root):
    pose_path = "src/game/mario_ground_pound_animation.c"
    for name, before, after in (
        ("ground_pound_frozen_windup", "position = MAX(0, m->actionTimer - 1) * (anim->loopEnd - 1.0f) / (anim->loopEnd + 3.0f);", "position = info->animFrame;"),
        ("ground_pound_integer_water_pose", "fraction = position - frame;", "fraction = 0.0f;"),
    ):
        yield "wallpose", name, "wallpose.ground_pound_sampling", {
            pose_path: MutationSource(root / pose_path).replace_in_function(
                "mario_ground_pound_animation_render", before, after)}
    path = "src/game/mario.c"
    yield "wallpose", "snap_wall_contact", "wallpose.entry", {
        path: MutationSource(root / path).replace_in_function("set_mario_animation",
            "mario_transition_pose_begin(m, targetAnimID);", "(void)0;")}
    path = "src/game/mario_transition_pose.c"
    yield "wallpose", "snap_handstand_dive_pose", "wallpose.handstand_dive_blend", {
        path: MutationSource(root / path).replace_in_function("mario_transition_pose_begin",
            "&& !handstandTransition", "&& (handstandTransition || !handstandTransition)")}
    yield "wallpose", "snap_handstand_jump_edges", "wallpose.handstand_jump_edges", {
        "src/game/mario_actions_automatic.c": MutationSource(root / "src/game/mario_actions_automatic.c")
            .replace_in_function("act_top_of_pole",
                "m->pos[1] -= pole_jump_anim_y_translation(m);", "(void) m;")}
    for name, function, before, after in (
        ("slow_late_wall_kick_turn", "mario_transition_pose_update_gfx",
         "turnAmount : amount", "MIN(turnAmount, amount) : amount"),
        ("rush_wall_kick_pitch_roll", "mario_transition_pose_update_gfx",
         "i == 1 ? turnAmount : amount", "turnAmount"),
        ("rush_early_wall_kick_turn", "mario_transition_pose_begin",
         "sTransitionPose.owner != animInfo || wall_kick_blend_amount() >= 1.0f", "TRUE"),
        ("lose_seated_wall_kick_turn", "mario_transition_pose_begin",
         "targetAnimID == MARIO_ANIM_SLIDE && sTransitionPose.owner == animInfo", "FALSE"),
    ):
        yield "wallpose", name, "wallpose.late_kick_turn", {
            path: MutationSource(root / path).replace_in_function(function, before, after)}
    yield "wallpose", "turn_over_long_arc", "wallpose.facing", {
        path: MutationSource(root / path).replace_in_function("wall_kick_blend_angle",
            "(s16) (to - from)", "(s32) (to - from)")}
    yield "wallpose", "discard_visible_interrupted_pose", "wallpose.interruption", {
        path: MutationSource(root / path).replace_in_function("mario_transition_pose_begin",
            "if (sTransitionPose.owner != animInfo) {", "if (TRUE) {")}
    yield "wallpose", "carry_side_flip_spin_into_wall", "wallpose.side_flip_entry", {
        path: MutationSource(root / path).replace_in_function("mario_transition_pose_render",
            "if (sTransitionPose.straightenFlipBody && i >= MARIO_BODY_ROTATION_START",
            "if (FALSE && sTransitionPose.straightenFlipBody && i >= MARIO_BODY_ROTATION_START")}
    for animation in ("BACKFLIP", "TRIPLE_JUMP", "BACKWARD_SPINNING"):
        yield "wallpose", "carry_" + animation.lower() + "_into_wall", "wallpose.flip_body_entry", {
            path: MutationSource(root / path).replace_in_function("mario_transition_pose_begin",
                "|| animInfo->animID == MARIO_ANIM_" + animation, "")}
