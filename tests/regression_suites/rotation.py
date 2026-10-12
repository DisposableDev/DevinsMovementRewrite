"""Production extraction and deliberate defects for the rotation suite."""
from .common import prepare_common_constants
from .mutation_support import MutationSource

def prepare_rotation(root, source, definitions, functions):
    prepare_common_constants(source, definitions, gameplay_options=True)
    definitions += [source("src/game/mario_he_thicc.h").macros(
        "HE_THICC_.*|ACT_ARG_WALL_KICK_HE_THICC|ACT_ARG_FORWARD_ROLLOUT_HE_THICC_BOUNCE")]
    definitions += [source("src/game/mario_actions_airborne.h").macros(
        "ACT_ARG_JUMP_BACKWARD_BUTT_SLIDE.*|ACT_ARG_JUMP_KICK_FOLLOWUP_QUEUED|ACT_ARG_JUMP_KICK_BACKWARD_BUTT_SLIDE.*|ACT_ARG_STEEP_JUMP_BACKWARD_BUTT_SLIDE_NO_UPHILL|ACT_ARG_DIVE_FROM_ROLLOUT_CONTACT")]
    definitions += [source("src/game/mario_actions_stationary.h").macros(
        "ACT_ARG_COUGHING_SHALLOW_WATER")]
    definitions += [source("include/config/config_movement.h").macros("BACKWARD_BUTT_SLIDE_JUMP_AIR_DRAG|BACKWARD_JUMP_KICK_UPHILL_DECEL")]
    sm64 = source("include/sm64.h")
    definitions += [sm64.declaration("enum", name) for name in (
        "MarioInput", "MarioFlags", "MarioParticleFlags", "AirStep")]
    definitions += [sm64.macros("ACT_.*")]
    definitions += [source("include/mario_animation_ids.h").declaration("enum", "MarioAnimID")]
    definitions += [source("src/game/mario_actions_airborne.h").declaration("enum", "ActionArgsPuttingOnCapAir")]
    definitions += [source("src/engine/math_util.h").macros("gCosineTable|sins|coss|atans")]
    definitions += [source("include/config/config_collision.h").macros("FAST_FLOOR_ALIGN")]
    definitions += [source("include/config/config_movement.h").macros(
        "MARIO_GLOBAL_SPEED_CAP|MARIO_MAX_FALL_SPEED|FALL_DRAG_.*|BACKWARD_AIR_.*|"
        "FORWARD_DIVE_.*|SLIDE_KICK_.*|SIDE_FLIP_.*|CAPLESS_CAP_PICKUP_AIR_DRAG")]
    moving = source("src/game/mario_actions_moving.c")
    definitions += [moving.macros("RUNNING_MAX_VISUAL_PITCH")]
    air = source("src/game/mario_actions_airborne.c")
    definitions += [source("src/game/mario_actions_airborne.c").macros("AIR_HIGH_SPEED_.*")]
    definitions += [air.macros("WALL_KICK_.*")]
    tables = source("include/trig_tables.inc.c")
    functions += [tables.mapped(0, len(tables.text))]
    functions += [source("src/engine/math_util.c").function(name) for name in (
        "atan2_lookup", "atan2s")]
    functions += [source("src/game/mario.c").function(name) for name in (
        "find_floor_slope", "mario_set_forward_vel")]
    functions += [moving.function("tilt_body_running")]
    functions += [air.function(name) for name in (
        "backward_air_extra_drag", "update_air_without_turn", "act_crazy_box_bounce")]
    functions += [source("src/game/mario_step.c").function(name) for name in (
        "gravity_with_fall_drag", "mario_play_wall_bonk_sound", "mario_bonk_reflection")]

def mutations_rotation(root):
    path = "src/game/mario_actions_moving.c"
    yield "rotation", "unbounded_running_lean", "rotation.running_lean", {
        path: MutationSource(root / path).replace_in_function("tilt_body_running",
            "CLAMP(-pitch, -RUNNING_MAX_VISUAL_PITCH, RUNNING_MAX_VISUAL_PITCH)", "-pitch")}
    yield "rotation", "wrap_running_lean_before_limit", "rotation.running_lean", {
        path: MutationSource(root / path).replace_in_function("tilt_body_running",
            "f32 pitch =", "s16 pitch =")}
    path = "src/game/mario_actions_airborne.c"
    yield "rotation", "signed_box_pitch", "rotation.box_reverse", {
        path: MutationSource(root / path).replace_in_function("act_crazy_box_bounce",
            "atan2s(absf(m->forwardVel), -m->vel[1])", "atan2s(m->forwardVel, -m->vel[1])")}
    yield "rotation", "force_box_travel_forward", "rotation.box_reverse", {
        path: MutationSource(root / path).replace_in_function("act_crazy_box_bounce",
            "m->marioObj->header.gfx.angle[0] = atan2s(absf(m->forwardVel), -m->vel[1]);",
            "m->forwardVel = absf(m->forwardVel);\n"
            "    m->marioObj->header.gfx.angle[0] = atan2s(absf(m->forwardVel), -m->vel[1]);")}

