"""Production extraction and deliberate defects for the animation suite."""
from .common import prepare_common_constants
from .mutation_support import MutationSource

def prepare_animation(root, source, definitions, functions):
    prepare_common_constants(source, definitions)
    functions += [
        "void object_platform_motion_before_step(struct Object *obj) {}",
        "s32 object_platform_motion_supported(struct Object *obj) { "
        "return obj->oPosY <= obj->oFloorHeight + 4.0f && obj->oVelY <= 0.0f; }",
    ]
    functions += ["f32 object_platform_motion_floor_rise(struct Object *obj) { return 0.0f; }"]
    definitions += [source("include/object_constants.h").macros("OBJ_FLAG_(ALLOW_ENEMY_SLEEP|HIDE_WHEN_SLEEPING)")]
    definitions += [source("src/game/object_platform_motion.h").declaration("enum", "EnemyPlatformUpdateStatus")]
    functions += [
        "s32 enemy_should_sleep(struct Object *obj) { return FALSE; }",
        "s32 bbh_merry_go_round_update_dormancy(struct Object *obj) { return FALSE; }",
        "s32 physics_lab_discard_distant_effect(struct Object *obj) { return FALSE; }",
        "s32 physics_lab_update_enemy_hidden_state(struct Object *obj) { return FALSE; }",
        "void cur_obj_update_sleeping_visibility(void) { unsupported(\"sleeping visibility\"); }",
        "void profiler_count_object(s32 sleeping) {}",
        "u8 *object_platform_motion_start_update(struct Object *obj, s32 *rider) { "
        "static u8 status; *rider = TRUE; return &status; }",
        "struct Object *object_platform_motion_provider(struct Object *obj) { return NULL; }",
        "void object_platform_motion_apply(struct Object *obj) {}",
        "void object_platform_motion_finish(struct Object *obj) {}",
    ]
    definitions += [source("src/game/mario_he_thicc.h").macros(
        "HE_THICC_.*|ACT_ARG_WALL_KICK_HE_THICC|ACT_ARG_FORWARD_ROLLOUT_HE_THICC_BOUNCE")]
    definitions += [source("src/game/mario_actions_airborne.h").macros(
        "ACT_ARG_JUMP_BACKWARD_BUTT_SLIDE.*|ACT_ARG_JUMP_KICK_FOLLOWUP_QUEUED|ACT_ARG_JUMP_KICK_BACKWARD_BUTT_SLIDE.*|ACT_ARG_STEEP_JUMP_BACKWARD_BUTT_SLIDE_NO_UPHILL|ACT_ARG_DIVE_FROM_ROLLOUT_CONTACT")]
    definitions += [source("src/game/mario_actions_stationary.h").macros(
        "ACT_ARG_COUGHING_SHALLOW_WATER")]
    definitions += [source("include/config/config_movement.h").macros("BACKWARD_BUTT_SLIDE_JUMP_AIR_DRAG|BACKWARD_JUMP_KICK_UPHILL_DECEL")]
    definitions += [source("include/surface_terrains.h").declaration("enum", "SurfaceTypes")]
    definitions += [source("src/game/object_list_processor.h").declaration("enum", "gTimeStopStateFlags")]
    definitions += [source("src/engine/graph_node.h").declaration("enum", "GraphRenderFlags")]
    constants = source("include/object_constants.h")
    definitions += [constants.declaration("enum", name) for name in ("ObjActiveFlags", "ObjMoveFlags")]
    definitions += [constants.macros("OBJ_MOVE_MASK_(ON_GROUND|IN_WATER)")]
    interactions = source("src/game/interaction.h")
    definitions += [interactions.declaration("enum", name) for name in ("ObjInteractTypes", "InteractStatus")]
    definitions += [source("src/engine/surface_collision.h").macros("FLOOR_LOWER_LIMIT(_MISC)?")]
    obj = source("src/game/object_list_processor.c")
    definitions += [obj.macros("GROUND_TWIRL_KNOCKBACK_GRAVITY")]
    functions += [obj.function(name) for name in (
        "stop_ground_twirl_stunned_enemy", "ground_twirl_support_raised_enemy",
        "update_ground_twirl_stun_motion", "update_ground_twirl_stunned_object",
        "update_object_with_ground_twirl_stun",
        "update_object_with_platform_motion", "update_objects_starting_at", "update_objects_during_time_stop")]

def mutations_animation(root):
    path = "src/game/object_list_processor.c"
    yield "animation", "classify_idle_objects", "animation.idle_fast_path", {
        path: MutationSource(root / path).replace_in_function("update_object_with_ground_twirl_stun",
            "if (!gCurrentObject->metalContactPendingDefeat", "if (FALSE && !gCurrentObject->metalContactPendingDefeat")}
    yield "animation", "omit_metal_timeout_tick", "animation.metal_timeout", {
        path: MutationSource(root / path).replace_in_function("update_ground_twirl_stunned_object",
            "gCurrentObject->metalContactCooldown--;", "(void)0;")}
    yield "animation", "expire_short_stun_before_metal_grace", "animation.metal_timeout", {
        path: MutationSource(root / path).replace_in_function('update_ground_twirl_stun_motion',
            "(s32) gCurrentObject->metalContactCooldown + 1", "0")}
    yield "animation", "skip_animation_restore", "animation.resume", {
        path: MutationSource(root / path).replace_in_function("update_object_with_platform_motion",
            "gCurrentObject->header.gfx.node.flags |= GRAPH_RENDER_HAS_ANIMATION;", "(void)0;")}
