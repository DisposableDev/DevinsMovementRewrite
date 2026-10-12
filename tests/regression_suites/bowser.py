"""Production extraction and deliberate defects for the bowser suite."""
from .common import prepare_common_constants
import re
from regression_support import HarnessError
from .mutation_support import MutationSource

def prepare_bowser(root, source, definitions, functions):
    prepare_common_constants(source, definitions, gameplay_options=True)
    definitions += [source("src/game/mario_he_thicc.h").macros(
        "HE_THICC_.*|ACT_ARG_WALL_KICK_HE_THICC|ACT_ARG_FORWARD_ROLLOUT_HE_THICC_BOUNCE")]
    definitions += [source("src/game/mario_actions_airborne.h").macros(
        "ACT_ARG_JUMP_BACKWARD_BUTT_SLIDE.*|ACT_ARG_JUMP_KICK_FOLLOWUP_QUEUED|ACT_ARG_JUMP_KICK_BACKWARD_BUTT_SLIDE.*|ACT_ARG_STEEP_JUMP_BACKWARD_BUTT_SLIDE_NO_UPHILL|ACT_ARG_DIVE_FROM_ROLLOUT_CONTACT")]
    definitions += [source("src/game/mario_actions_stationary.h").macros(
        "ACT_ARG_COUGHING_SHALLOW_WATER")]
    definitions += [source("include/config/config_movement.h").macros("BACKWARD_BUTT_SLIDE_JUMP_AIR_DRAG|BACKWARD_JUMP_KICK_UPHILL_DECEL")]
    constants = source("include/object_constants.h")
    definitions += [constants.macros("BOWSER_.*|BULLY_ACT_KNOCKBACK|AMP_BP_ROT_RADIUS_.*|OBJ_MOVE_MASK_.*|BOO_DEATH_STATUS_.*")]
    definitions += [source("src/engine/surface_collision.h").macros("FLOOR_LOWER_LIMIT.*")]
    definitions += [constants.declaration("enum", name) for name in (
        "ObjFlags", "ObjMoveFlags", "ObjActiveFlags", "HeldState", "ObjGeneralDeathActions", "oBehParam1stByteCoinInsideBoo", "oActionsBowser",
        "oActionsBowserTail", "oActionsBobOmb", "oSubActionBowserActDead", "oSubActionBowserActJumpOntoStage", "oSubActionBowserActTeleport", "oSubActionBowserActChargeMario", "oBowserGrabbedStatuses", "animIDsBowser", "oBehParams2ndByteBowser", "oBowserStatuses")]
    definitions += [source("src/game/camera.h").declaration("enum", "CameraEvent")]
    definitions += [source("include/sm64.h").macros("ACT_.*")]
    definitions += [source("include/sm64.h").declaration("enum", name) for name in (
        "MarioActionGroups", "MarioActionFlags", "MarioInput")]
    definitions += [source("src/game/mario_punch.h").declaration("enum", "ActionArgsPunchSequence")]
    definitions += [source("src/game/mario_punch.h").declaration("enum", "ActionStatesPunchSequence")]
    definitions += [source("include/segment_names.h").macros("SEGMENT_BEHAVIOR_DATA")]
    definitions += [source("src/game/interaction.h").declaration("enum", name) for name in (
        "ObjInteractTypes", "InteractStatus")]
    definitions += [source("include/types.h").declaration("struct", "ObjectHitbox")]
    definitions += [source("include/surface_terrains.h").declaration("enum", "SurfaceTypes")]
    definitions += [source("include/model_ids.h").macros("MODEL_(NONE|RED_FLAME|BLUE_FLAME|BLUE_COIN|YELLOW_COIN|BOWSER_WAVE|MR_I_BODY|BOO|SNUFIT)")]
    definitions += [source("src/engine/math_util.h").macros("DEGREES")]
    definitions += [source("src/engine/math_util.h").macros("approach_s16_symmetric")]
    functions += [source("src/engine/math_util.c").function("approach_s16")]
    functions += [source("src/game/object_helpers.c").function(name) for name in (
        "cur_obj_rotate_yaw_toward", "obj_set_hitbox", "cur_obj_compute_vel_xz",
        "apply_drag_to_value", "cur_obj_apply_drag_xz",
        "cur_obj_reset_timer_and_subaction", "cur_obj_change_action")]
    functions += [source("src/engine/math_util.c").function("approach_f32_signed")]
    functions += [source("src/game/behaviors/bobomb.inc.c").function("bhv_bobomb_loop"),
                  source("src/game/behaviors/spiny.inc.c").function("spiny_check_active")]
    flame = source("src/game/behaviors/bowser_flame.inc.c")
    functions += [flame.region(r"\A", r"\Z").replace(
        '#include "bowser_flame_original.inc.c"', source("src/game/behaviors/bowser_flame_original.inc.c").text)]
    definitions += [source("src/game/interaction.h").declaration("enum", "InteractSubtypes")]
    definitions += [constants.macros("MOVING_FLAME_BP_MOVE")]
    definitions += ["struct Object;", "f32 lateral_dist_between_objects(struct Object *, struct Object *);"]
    functions += [source("src/game/object_helpers.c").function("lateral_dist_between_objects")]
    functions += [source("src/game/obj_behaviors_2.c").function("obj_spit_fire")]
    functions += [source("src/game/mario_punch.c").function("mario_is_first_punch")]
    interaction = source("src/game/interaction.c")
    functions += [interaction.function(name) for name in (
        "mario_bowser_tail_grab_action", "mario_can_grab_bowser_tail", "mario_overlaps_grab_hitbox",
        "mario_get_bowser_tail_contact", "able_to_grab_object", "mario_check_object_grab")]
    definitions += [source("include/model_ids.h").macros("MODEL_(GOOMBA|BLACK_BOBOMB|ENEMY_LAKITU|BULLY|AMP|BOWLING_BALL)")]
    definitions += [source("src/game/bowser_arena.h").region(r"^#ifdef VERSION_EU", r"^#include")]
    definitions += [source("src/game/player_stats.h").declaration("enum", "PlayerStatId")]
    functions += [source("src/game/bowser_arena.c").region(r"^static const Vec3s sArenaPoints", r"^void \*bowser_arena_actor_asset")]
    functions += [source("src/game/behaviors/bowser_falling_platform.inc.c").function("falling_bowser_plat_act_check")]
    functions += [source("src/game/bowser_throw.c").function("bowser_throw_velocity")]
    bowser = source("src/game/behaviors/bowser.inc.c")
    definitions += ["void bowser_reset_fallen_off_stage(void);", "static void bowser_center_jump_update(void);"]
    functions += [bowser.function(name) for name in (
        "bowser_walk_turn_speed", "bowser_set_anim_look_up_and_walk", "bowser_set_anim_slow_gait", "bowser_set_anim_look_down_stop_walk", "bowser_act_walk_to_mario", "bowser_act_hit_edge", "bowser_turn_on_timer", "bowser_act_turn_from_edge", "bowser_spawn_shockwave", "bowser_bounce_effects", "bowser_tail_anchor_act_default", "bowser_tail_anchor_thrown", "bowser_update_grab_cooldown",
        "bowser_check_fallen_off_stage", "bowser_update_opacity", "bowser_teleport_to_mario", "bowser_act_teleport", "bowser_bitdw_actions", "bowser_bitfs_actions", "bowser_bits_action_list", "bowser_act_spit_fire_onto_floor", "bowser_spawn_center_return_flames", "bowser_set_anim_jump", "bowser_land", "bowser_short_second_hop", "bowser_act_big_jump", "bowser_act_quick_jump", "bowser_act_charge_mario", "bowser_center_jump_update", "bowser_start_center_jump", "bowser_action_is_attack",
        "bowser_update_post_attack_facing", "bowser_finish_throw", "bowser_act_thrown", "bowser_held_update", "bowser_thrown_dropped_update", "bowser_act_breath_fire", "bowser_act_spit_fire_into_sky",
        "bowser_recover_defeated", "bowser_dead_bounce", "bowser_dead_wait_for_mario", "bowser_set_goal_invisible", "bowser_reset_fallen_off_stage", "bowser_act_jump_onto_stage",
        "bhv_bowser_flame_spawn_loop")]
    definitions += [re.search(r"^s8 sBowserRainbowLight\[\].*?;", bowser.text, re.M)[0],
                    re.search(r"^s8 sBowserHealth\[\].*?;", bowser.text, re.M)[0]]
    functions += [bowser.function("bhv_bowser_init")]
    original = source("src/game/behaviors/bowser_original.inc.c")
    routed = set(re.findall(r"\boriginal_(\w+)\(", "\n".join(functions)))
    definitions += [f"static {match[1]} original_{match[2]}({match[3]});"
                    for match in re.finditer(r"^static (\w+) original_(\w+)\(([^)]*)\)", original.text, re.M)
                    if match[2] in routed]
    functions += [original.function("original_" + name) for name in sorted(routed)
                  if "original_" + name in original.text]
    definitions += [source("src/game/obj_behaviors.h").declaration("enum", "ObjCollisionFlags")]
    functions += [source("src/game/behaviors/cap.inc.c").function(name) for name in (
        "normal_cap_return_to_bowser_arena", "normal_cap_act_0")]
    definitions += [constants.declaration("enum", "oActionsCoinInsideBoo")]
    functions += [source("src/game/behaviors/coin.inc.c").function("coin_inside_boo_act_carried")]
    # Ensure behavior setup does not overwrite the initializer's varied gravity.
    behavior = source("data/behavior_data.c").text
    for name in ("bhvFlameBouncing", "bhvFlameBowser", "bhvFlameLargeBurningOut"):
        match = re.search(r"const BehaviorScript " + name + r"\[\] = \{(.*?)\n\};", behavior, re.S)
        if not match or match[1].find("SET_OBJ_PHYSICS") > match[1].find("CALL_NATIVE"):
            raise HarnessError(f"{name}: gravity setup runs after the flame initializer")

def mutations_bowser(root):
    for name, case, path, function, old, new in (
        ("original_bowser_current_decisions", "original_preset_decisions", "src/game/behaviors/bowser.inc.c", "bowser_bitdw_actions",
         "if (game_options_get(GO_BOWSER_FIGHTS) == 1)", "if (FALSE)"),
        ("original_bowser_held_timeout", "original_preset_hold", "src/game/behaviors/bowser.inc.c", "bowser_held_update",
         "game_options_get(GO_BOWSER_FIGHTS) == 0", "TRUE"),
        ("original_bowser_arena_enemies", "original_preset_timing_and_arena", "src/game/bowser_arena.c", "bowser_arena_landed",
         "if (game_options_get(GO_BOWSER_FIGHTS) == 1)", "if (FALSE)"),
    ):
        yield "bowser", name, "bowser." + case, {path: MutationSource(root / path).replace_in_function(function, old, new)}
    for name, case, path, function, old, new in (
        ("return_waits_for_animation_frame", "quick_jump_offstage_return", "src/game/behaviors/bowser.inc.c", "bowser_act_jump_onto_stage",
         "++o->oBowserTimer >= 12", "cur_obj_check_anim_frame(11)"),
        ("air_fire_intangible", "airborne_fire", "src/game/behaviors/bowser_flame.inc.c", "bhv_flame_bowser_loop",
         "cur_obj_become_tangible();\n        bowser_flame_move();", "o->oIntangibleTimer = -1;\n        bowser_flame_move();"),
        ("floating_fire_no_hitbox", "airborne_fire", "src/game/behaviors/bowser_flame.inc.c", "bhv_flame_floating_landing_loop",
         "obj_set_hitbox(o, &sBowserFlameHitbox);", "(void) o;"),
        ("fire_expires_above_death_floor", "airborne_fire", "src/game/behaviors/bowser_flame.inc.c", "bowser_flame_should_despawn",
         "(o->oMoveFlags & OBJ_MOVE_MASK_ON_GROUND) && o->oFloorType == SURFACE_DEATH_PLANE",
         "o->oFloorType == SURFACE_DEATH_PLANE"),
        ("floating_fire_old_timeout", "airborne_fire", "src/game/behaviors/bowser_flame.inc.c", "bhv_flame_floating_landing_loop",
         "bowser_flame_should_despawn(BOWSER_FLAME_AIR_TIMEOUT_FRAMES)", "bowser_flame_should_despawn(900)"),
        ("blue_fire_no_homing", "blue_fire_homing", "src/game/behaviors/bowser_flame.inc.c", "bhv_flame_bouncing_loop",
         "BOWSER_BLUE_FLAME_TURN_SPEED", "0"),
        ("sky_fire_duplicate_held_frame", "staggered_fire_attacks", "src/game/behaviors/bowser.inc.c", "bowser_act_spit_fire_into_sky",
         "o->oBowserTimer != animFrame", "TRUE"),
        ("breath_fire_duplicate_held_frame", "staggered_fire_attacks", "src/game/behaviors/bowser.inc.c", "bhv_bowser_flame_spawn_loop",
         "!(animFrame & 1) && o->oSubAction != animFrame", "!(animFrame & 1)"),
        ("blue_group_wrong_count", "staggered_blue_groups", "src/game/behaviors/bowser_flame.inc.c", "bhv_blue_flames_group_loop",
         "flameIndex >= flameCount", "flameIndex >= flameCount - 3"),
        ("blue_fire_random_gravity", "blue_fire_gravity", "src/game/behaviors/bowser_flame.inc.c", "bhv_flame_bouncing_init",
         "o->oGravity = -1.0f;", "o->oGravity = -0.8f - random_float() * 0.4f;"),
        ("red_fire_uniform_fall_speed", "varied_fire_gravity", "src/game/behaviors/bowser_flame.inc.c", "bhv_flame_floating_landing_loop",
         "fallSpeedLimit *= -o->oGravity;", "fallSpeedLimit = -8.0f;"),
        ("landing_first_arena_only", "landing_impacts", "src/game/behaviors/bowser.inc.c", "bowser_land",
         "mario_queue_bowser_impact(o);", "if (o->oBehParams2ndByte == BOWSER_BP_BITDW) { mario_queue_bowser_impact(o); }"),
        ("fire_uniform_gravity", "varied_fire_gravity", "src/game/behaviors/bowser_flame.inc.c", "bhv_flame_floating_landing_init",
         "-0.8f - random_float() * 0.4f", "-1.0f"),
        ("tail_grab_without_punch", "punch_tail_grabs", "src/game/interaction.c", "mario_bowser_tail_grab_action",
         "mario_is_first_punch(m)", "TRUE"),
        ("front_tail_grab", "punch_tail_grabs", "src/game/interaction.c", "mario_can_grab_bowser_tail",
         "behindDYaw >= -0x2AAA && behindDYaw <= 0x2AAA", "(behindDYaw >= -0x2AAA || behindDYaw <= 0x2AAA)"),
        ("grab_before_facing", "tail_recovery", "src/game/behaviors/bowser.inc.c", "bowser_update_grab_cooldown",
         "o->oBowserGrabCooldown > 1", "o->oBowserGrabCooldown > 0"),
    ):
        yield "bowser", name, "bowser." + case, {
            path: MutationSource(root / path).replace_in_function(function, old, new)}
