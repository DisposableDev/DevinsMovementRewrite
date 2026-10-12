"""Production extraction and deliberate defects for the platform suite."""
from .common import prepare_common_constants
import re
from regression_support import HarnessError
from .mutation_support import MutationSource

def prepare_platform(root, source, definitions, functions):
    prepare_common_constants(source, definitions)
    # Retained carry cases explicitly exercise the optional implementation.
    definitions += ["#ifndef ENABLE_ENEMY_PLATFORM_MOTION\n#define ENABLE_ENEMY_PLATFORM_MOTION 1\n#endif"]
    definitions += [source("include/macros.h").region(r"^#ifdef __GNUC__\n(?=#define ALWAYS_INLINE)", r"^#ifdef __GNUC__\n(?=#define FALL_THROUGH)")]
    definitions += [source("src/game/mario_he_thicc.h").macros(
        "HE_THICC_.*|ACT_ARG_WALL_KICK_HE_THICC|ACT_ARG_FORWARD_ROLLOUT_HE_THICC_BOUNCE")]
    definitions += [source("src/game/mario_actions_airborne.h").macros(
        "ACT_ARG_JUMP_BACKWARD_BUTT_SLIDE.*|ACT_ARG_JUMP_KICK_FOLLOWUP_QUEUED|ACT_ARG_JUMP_KICK_BACKWARD_BUTT_SLIDE.*|ACT_ARG_STEEP_JUMP_BACKWARD_BUTT_SLIDE_NO_UPHILL|ACT_ARG_DIVE_FROM_ROLLOUT_CONTACT")]
    definitions += [source("src/game/mario_actions_stationary.h").macros(
        "ACT_ARG_COUGHING_SHALLOW_WATER")]
    definitions += [source("include/config/config_movement.h").macros("BACKWARD_BUTT_SLIDE_JUMP_AIR_DRAG|BACKWARD_JUMP_KICK_UPHILL_DECEL")]
    world = source("include/config/config_world.h")
    definitions += [world.macros("EXTENDED_BOUNDS_MODE|NUM_CELLS|GET_CELL_COORD"),
                    world.region(r"^#if EXTENDED_BOUNDS_MODE == 0", r"^STATIC_ASSERT")]
    definitions += [source("src/engine/surface_collision.h").macros("FIND_FLOOR_BUFFER|is_outside_level_bounds"),
                    source("src/engine/surface_load.h").macros("NORMAL_FLOOR_THRESHOLD"),
                    source("src/engine/surface_load.h").declaration("enum", "SpatialPartitions"),
                    source("src/engine/surface_query.h").declaration("enum", "SurfaceQueryStorage")]
    definitions += [source("include/surface_terrains.h").declaration("enum", "TerrainLoadCmd"),
                    source("include/surface_terrains.h").macros("COL_VERTEX")]
    constants = source("include/object_constants.h")
    definitions += [constants.declaration("enum", name) for name in (
        "ObjActiveFlags", "ObjFlags", "HeldState", "ObjMoveFlags", "ObjGeneralDeathActions", "oActionsBowser", "oActionsExclamationBox")]
    definitions += [constants.macros(
        "OBJ_FLAG_(ALLOW_ENEMY_SLEEP|HIDE_WHEN_SLEEPING)|ACTIVE_FLAG_BLINKING|OBJ_MOVE_MASK_.*|KING_BOBOMB_(ACT|SUB_ACT)_.*|CHAIN_CHOMP_.*|WIGGLER_ACT_.*|POKEY_ACT_.*|WHOMP_ACT_.*")]
    definitions += [source("include/config/config_objects.h").macros("WIGGLER_NUM_SEGMENTS|CHAIN_CHOMP_NUM_SEGMENTS")]
    definitions += [source("src/game/object_list_processor.h").declaration("enum", "gTimeStopStateFlags"),
                    source("src/game/object_list_processor.h").macros("OBJECT_POOL_CAPACITY"),
                    source("src/engine/graph_node.h").declaration("enum", "GraphRenderFlags"),
                    source("src/game/platform_displacement.h").declaration("struct", "PlatformDisplacementInfo"),
                    source("src/game/object_helpers.h").declaration("struct", "ChainSegment")]
    math = source("src/engine/math_util.h")
    definitions += [math.macros("vec2_dot|vec3_.*|vec3f_.*|linear_mtxf_.*|gCosineTable|sins|coss|atans")]
    motion = source("src/game/object_platform_motion.c")
    # Unique script identities are supplied; policy selection executes production code.
    behaviors = sorted(set(re.findall(r"\bbhv[A-Za-z0-9_]+", motion.text)))
    definitions += ["static const BehaviorScript " + name + "[] = {0};" for name in behaviors]
    definitions += [source("src/game/object_platform_motion.h").region(
        r"^void object_platform_motion_reset", r"^#endif")]
    definitions += [source("src/game/object_platform_motion.h").declaration("enum", "EnemyPlatformUpdateStatus")]
    arena = source("levels/bowser_2/tilting_platform/collision.inc.c").mask
    vertices = re.findall(r"COL_VERTEX\((-?\d+),\s*(-?\d+),\s*(-?\d+)\)", arena)
    if len(vertices) != 18 or "COL_TRI(16, 17, 6)" not in arena:
        raise HarnessError("Bowser arena layout changed; review quantized tilt coverage")
    definitions += ["static Vec3s bowserPlaneVertices[3] = {"
                    + ",".join("{" + ",".join(vertices[i]) + "}" for i in (16, 17, 6)) + "};"]
    functions += [source("include/trig_tables.inc.c").mapped(0, len(source("include/trig_tables.inc.c").text))]
    functions += [source("src/engine/math_util.c").declaration("struct", "CopyMat4")]
    functions += [source("src/engine/math_util.c").function(name) for name in (
        "mtxf_copy", "mtxf_scale_vec3f", "mtxf_rotate_zxy_and_translate", "atan2_lookup", "atan2s")]
    functions += [source("src/game/platform_displacement.c").function(name) for name in (
        "update_platform_displacement_info", "apply_platform_displacement")]
    functions += [motion.region(r"^#define ENEMY_PLATFORM_SUPPORT_TOLERANCE", r"\Z")]
    functions += [source("src/game/obj_behaviors.c").function(name) for name in (
        "calc_obj_friction", "calc_new_obj_vel_and_pos_y")]
    functions += [source("src/game/object_helpers.c").function("cur_obj_move_using_vel_and_gravity")]
    functions += [source("src/game/object_list_processor.c").function(name) for name in (
        "update_object_with_platform_motion", "update_objects_starting_at")]
    functions += [source("src/game/physics_lab_activity.c").function("physics_lab_skip_box_update"),
                  source("src/game/behaviors/exclamation_box.inc.c").function("bhv_exclamation_box_loop")]

def mutations_platform(root):
    path = "src/game/object_platform_motion.c"
    for name, case, function, old, new in (
        ("enemy_no_carry", "translation", "enemy_platform_apply_motion",
         "apply_platform_displacement(&state->displacement, target, &yaw, provider);", "(void) provider;"),
        ("enemy_no_drag", "departure", "enemy_platform_apply_motion",
         "state->velocity[0] *= ENEMY_PLATFORM_INERTIA_DRAG;", "state->velocity[0] *= 1.0f;"),
        ("enemy_repeat_upward_credit", "departure", "object_platform_motion_before_step",
         "state->upwardPending = FALSE;", "state->upwardPending = TRUE;"),
        ("enemy_stale_support", "lifecycle", "object_platform_motion_provider",
         "providerState->generation != state->providerGeneration", "FALSE"),
        ("enemy_bank_blocked_motion", "conveyor_and_clip", "enemy_platform_apply_motion",
         "state->velocity[axis] -= rejectedVelocityScale * rejected[axis];", "state->velocity[axis] -= 0.0f * rejected[axis];"),
    ):
        yield "platform", name, "platform." + case, {
            path: MutationSource(root / path).replace_in_function(function, old, new)}
    path = "src/game/object_list_processor.c"
    yield "swimming", "falling_stun_countdown", "swimming.stunned_lost_support", {
        path: MutationSource(root / path).replace_in_function("update_ground_twirl_stun_motion",
            "gCurrentObject->oGroundTwirlStunTimer--;", "gCurrentObject->oGroundTwirlStunTimer -= 0;")}
