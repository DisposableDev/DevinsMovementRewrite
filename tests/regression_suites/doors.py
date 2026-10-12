"""Production extraction and deliberate defects for the doors suite."""
from .common import prepare_common_constants
from .mutation_support import MutationSource

def prepare_doors(root, source, definitions, functions):
    prepare_common_constants(source, definitions)
    definitions += [source("src/game/camera.h").declaration("enum", "Cutscenes")]
    definitions += [source("include/config/config_movement.h").macros("STANDARD_DOOR_.*"),
                    source("include/config/config_movement.h").macros("WALK_WALL_HUG_MAX_ANGLE"),
                    source("src/game/mario_actions_moving.h").declaration("enum", "ActionStatesPushOrSidleWall"),
                    source("include/object_constants.h").declaration("enum", "oActionsDoor"),
                    source("include/object_constants.h").declaration("enum", "animIDsDoor"),
                    source("include/mario_animation_ids.h").declaration("enum", "MarioAnimID"),
                    source("src/game/level_update.h").declaration("enum", "WarpOperation"),
                    source("src/game/object_list_processor.h").declaration("enum", "gTimeStopStateFlags"),
                    source("include/model_ids.h").macros("MODEL_HMC_METAL_DOOR")]
    definitions += [source("src/game/mario_he_thicc.h").macros(
        "HE_THICC_.*|ACT_ARG_WALL_KICK_HE_THICC|ACT_ARG_FORWARD_ROLLOUT_HE_THICC_BOUNCE")]
    definitions += [source("src/game/mario_actions_airborne.h").macros(
        "ACT_ARG_JUMP_BACKWARD_BUTT_SLIDE.*|ACT_ARG_JUMP_KICK_FOLLOWUP_QUEUED|ACT_ARG_JUMP_KICK_BACKWARD_BUTT_SLIDE.*|ACT_ARG_STEEP_JUMP_BACKWARD_BUTT_SLIDE_NO_UPHILL|ACT_ARG_DIVE_FROM_ROLLOUT_CONTACT")]
    definitions += [source("src/game/mario_actions_stationary.h").macros(
        "ACT_ARG_COUGHING_SHALLOW_WATER")]
    definitions += [source("include/config/config_movement.h").macros("BACKWARD_BUTT_SLIDE_JUMP_AIR_DRAG|BACKWARD_JUMP_KICK_UPHILL_DECEL")]
    sm64 = source("include/sm64.h")
    definitions += [sm64.declaration("enum", name) for name in (
        "MarioActionGroups", "MarioActionFlags", "MarioFlags", "MarioParticleFlags")]
    definitions += [sm64.macros("ACT_.*")]
    definitions += [source("src/game/interaction.h").declaration("enum", name) for name in (
        "ObjInteractTypes", "InteractSubtypes", "InteractStatus")]
    definitions += [source("src/game/save_file.h").declaration("enum", "SaveProgressFlags")]
    definitions += [source("src/game/level_update.h").declaration("enum", "WarpFlags")]
    definitions += [source("include/dialog_ids.h").declaration("enum", "DialogId").replace(
        '#include "simpleflips_messages.inc.h"', source("include/simpleflips_messages.inc.h").text)]
    interaction = source("src/game/interaction.c")
    definitions += [interaction.macros("DOOR_OPEN_.*")]
    definitions += [source("src/engine/math_util.h").macros("atans|DEGREES")]
    tables = source("include/trig_tables.inc.c")
    functions += [tables.mapped(0, len(tables.text))]
    functions += [source("src/engine/math_util.c").function(name) for name in (
        "abs_angle_diff", "atan2_lookup", "atan2s")]
    functions += [interaction.function(name) for name in (
        "mario_obj_angle_to_object", "mario_has_door_open_intent", "should_push_or_pull_door",
        "get_door_save_file_flag", "interact_door", "interact_warp_door")]
    functions += [source("src/game/mario_actions_moving.c").function("push_or_sidle_wall")]
    functions += [source("src/game/mario_actions_cutscene.c").function(name) for name in (
        "mario_door_passage_can_end", "act_going_through_door")]
    door = source("src/game/behaviors/door.inc.c")
    definitions += [door.region(r"^static s32 sDoorOpenSounds", r"^void door_animation_and_reset")]
    functions += [door.function(name) for name in (
        "door_animation_and_reset", "play_door_open_noise", "play_warp_door_open_noise")]
    functions += [source("src/game/camera.c").function(name) for name in (
        "cutscene_standard_door_speed", "cutscene_advance_standard_door_clock", "cutscene_door_fix_cam", "cutscene_door_follow_mario")]

def mutations_doors(root):
    path = "src/game/interaction.c"
    for name, function, old, new, case in (
        ("allow_coasting_door", "mario_has_door_open_intent",
         "m->controller->stickMag < DOOR_OPEN_MIN_STICK_MAG", "FALSE", "doors.boundaries"),
        ("use_scaled_door_stick", "mario_has_door_open_intent",
         "m->controller->stickMag < DOOR_OPEN_MIN_STICK_MAG",
         "m->intendedMag < DOOR_OPEN_MIN_STICK_MAG", "doors.boundaries"),
        ("ignore_door_facing", "mario_has_door_open_intent",
         "abs_angle_diff(m->faceAngle[1], doorYaw) <= DOOR_OPEN_MAX_ANGLE", "TRUE", "doors.boundaries"),
        ("ignore_door_stick_angle", "mario_has_door_open_intent",
         "abs_angle_diff(m->intendedYaw, doorYaw) <= DOOR_OPEN_MAX_ANGLE", "TRUE", "doors.boundaries"),
        ("omit_normal_door_intent", "interact_door",
         "if (!mario_has_door_open_intent(m, obj))", "if (FALSE)", "doors.locks_and_reapproach"),
        ("omit_warp_door_intent", "interact_warp_door",
         "if (!mario_has_door_open_intent(m, obj))", "if (FALSE)", "doors.locks_and_reapproach"),
        ("gate_committed_star_door", "interact_door",
         "m->action == ACT_IDLE && sDisplayingDoorText == TRUE && requiredNumStars == 70",
         "m->action == ACT_IDLE && sDisplayingDoorText == TRUE && requiredNumStars == 70"
         " && mario_has_door_open_intent(m, obj)", "doors.unlocks_and_continuation"),
    ):
        yield "doors", name, case, {path: MutationSource(root / path).replace_in_function(function, old, new)}
