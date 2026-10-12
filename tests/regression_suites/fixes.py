"""Production extraction and deliberate defects for the fixes suite."""
from .common import prepare_common_constants
import re
from regression_support import animation_metadata
from .mutation_support import MutationSource

def prepare_fixes(root, source, definitions, functions):
    prepare_common_constants(source, definitions, gameplay_options=True, cap_functions=functions)
    definitions += [source("src/game/ingame_menu.h").declaration("enum", "MenuMode"),
                    "static s16 gMenuMode = MENU_MODE_NONE;"]
    functions += [source("src/game/mario.c").function(name) for name in
                  ("mario_keeps_shell", "mario_blocks_harmful_knockback")]
    definitions += [source("src/engine/math_util.h").macros("ABS")]
    definitions += ["#define sReducedFlashing sTestGameOptions[GO_FLASHING]"]
    definitions += ["void mario_dab_reset(void);", "void mario_ground_twirl_init(void);"]
    ground_twirl = source("src/game/mario_ground_twirl.c")
    definitions += [ground_twirl.region(r"^static struct \{", r"^void mario_ground_twirl_reset_cooldown\(")]
    functions += [ground_twirl.function("mario_ground_twirl_reset_rebound_state")]
    functions += [ground_twirl.function("mario_twirl_advance_contact_generation")]
    functions += [ground_twirl.function("mario_ground_twirl_init")]
    definitions += [source("src/game/mario_actions_airborne.h").macros("JUMP_KICK_(CANCEL|BUFFER)_FRAMES")]
    functions += [source("src/game/mario.c").function("mario_has_cap_for_damage")]
    functions += [source("src/game/mario_actions_airborne.c").function("mario_jump_kick_can_cancel")]
    definitions.append("s32 mario_is_knocked_back(u32 action);")
    definitions.append("void mario_apply_automatic_buffer(struct MarioState *m);")
    functions += [source("src/game/interaction.c").function("mario_is_knocked_back")]
    functions += [source("src/game/mario_actions_object.c").function("cancel_throw_recovery")]
    fall = source("src/game/mario_actions_airborne.c")
    definitions += [fall.declaration("enum", name) for name in ("HandstandFallState", "FallDamageSeverity")]
    definitions += [source("include/config/config_movement.h").macros("FALL_DAMAGE_HEIGHT_.*")]
    functions += [fall.function(name) for name in (
        "handstand_jump_protects_from_fall", "is_large_fall_at_height",
        "fall_damage_severity_at_height", "mario_would_take_fall_damage_on_water")]

    air_twirl = source("src/game/mario_air_twirl.c")
    definitions += [air_twirl.macros("AIR_TWIRL_.*")]
    functions += [air_twirl.function(name) for name in (
        "mario_air_twirl_reset_allowances", "mario_air_twirl_init", "mario_air_twirl_reset_on_coin",
        "mario_air_twirl_is_active",
        "mario_air_twirl_transition", "mario_air_twirl_gravity_scale")]
    definitions += [source("include/config/config_movement.h").macros("WALL_BONK_MOMENTUM_RETENTION")]
    functions += [source("src/game/mario_step.c").function("mario_set_wall_bonk_momentum")]
    functions += [
        "void mario_he_thicc_reset(void) {}",
        "s32 mario_he_thicc_try_belly_landing(struct MarioState *m) { return FALSE; }",
        "s32 mario_he_thicc_try_pound_bounce(struct MarioState *m) { return FALSE; }",
        "void mario_he_thicc_wall_kick(struct MarioState *m) {}",
        "void mario_he_thicc_slide_kick_bounce(struct MarioState *m) {}",
    ]
    definitions += [source("src/game/mario_he_thicc.h").macros(
        "HE_THICC_.*|ACT_ARG_WALL_KICK_HE_THICC|ACT_ARG_FORWARD_ROLLOUT_HE_THICC_BOUNCE")]
    definitions += [source("include/config/config_movement.h").macros("METAL_WATER_.*|MARIO_MAX_FALL_SPEED|HE_THICC_(MAX_FALL_SPEED|WATER_GRAVITY_SCALE)")]
    definitions += [source("src/engine/surface_collision.h").macros("FLOOR_LOWER_LIMIT")]
    functions += [source("src/game/mario_step.c").function("mario_get_movement_time_scale")]
    definitions += [source("src/game/mario_step.c").macros("MARIO_WATER_WALK_.*")]
    functions += [source("src/game/mario_step.c").function(name) for name in (
        "mario_water_walk_conditions_met", "mario_update_water_walking_state",
        "mario_tick_water_walking_cooldown", "mario_can_walk_on_water")]
    functions += [source("src/game/mario_step.c").function(name) for name in (
        "mario_max_fall_speed", "limit_metal_water_fall_speed")]
    definitions += [source("include/object_constants.h").macros("BOO_DEATH_STATUS_.*")]
    definitions += [source("src/game/object_list_processor.h").declaration("enum", "ObjectList")]
    definitions += [source("src/game/mario_actions_airborne.h").macros(
        "ACT_ARG_JUMP_BACKWARD_BUTT_SLIDE.*|ACT_ARG_BUTT_SLIDE_JUMP_DIVE_.*|ACT_ARG_JUMP_KICK_FOLLOWUP_QUEUED|ACT_ARG_JUMP_KICK_BACKWARD_BUTT_SLIDE.*|ACT_ARG_STEEP_JUMP_BACKWARD_BUTT_SLIDE_NO_UPHILL|ACT_ARG_DIVE_FROM_ROLLOUT_CONTACT")]
    definitions += [source("src/game/mario_actions_stationary.h").macros(
        "ACT_ARG_COUGHING_SHALLOW_WATER")]
    definitions += [source("include/config/config_movement.h").macros("BACKWARD_BUTT_SLIDE_JUMP_AIR_DRAG|BACKWARD_JUMP_KICK_UPHILL_DECEL")]
    header = source("src/game/mario_character.h")
    definitions += [header.region(r"^struct MarioState;", r"^#endif")]
    definitions += [source("include/config/config_movement.h").macros(
        "WALK_TARGET_SPEED|JUMP_HEIGHT_VELOCITY_SCALE|JUMP_FORWARD_SPEED_RETENTION")]
    character = source("src/game/mario_character.c")
    functions += [character.region(r"^const struct MarioMovementProfile gMarioMovementProfiles", r"^s32 mario_character_can_unlock")]
    functions += [character.function("mario_character_can_unlock")]
    functions += [character.region(r"^void mario_character_commit", r"\Z")]
    definitions += [source("src/game/mario_actions_airborne.h").declaration("enum", "ActionArgsLavaBoost")]
    functions += [source("src/game/mario_actions_airborne.c").region(
        r"^#ifdef NO_GETTING_BURIED", r"^void play_far_fall_sound")]
    definitions += [source("include/sounds.h").macros("SOUND_TERRAIN_.*")]
    definitions += [source("src/game/mario_dab.h").macros("DAB_ANALOG_.*")]
    air_header = source("src/game/mario_actions_airborne.h")
    definitions += [air_header.declaration("enum", "ActionArgsTripleJump"),
                    air_header.macros("BACKWARD_JUMP_WALL_KICK_DELAY_FRAMES")]
    stop_crouching_length = animation_metadata(source("assets/anims/anim_96.inc.c"))[3]
    definitions += [f"#define STOP_CROUCHING_ANIM_LENGTH {stop_crouching_length}"]
    for name, number in (("GROUND_BONK_ANIM_LENGTH", "8A"),
                         ("BACKWARD_BONK_ANIM_LENGTH", "7B"),
                         ("FORWARD_BONK_ANIM_LENGTH", "7C")):
        length = animation_metadata(source(f"assets/anims/anim_{number}.inc.c"))[3]
        definitions += [f"#define {name} {length}"]
    for name in ("is_anim_at_end", "is_anim_past_end"):
        functions += [source("src/game/mario.c").function(name).replace(
            f"s32 {name}(", f"s32 real_{name}(")]
    sm64 = source("include/sm64.h")
    definitions += [sm64.declaration("enum", name) for name in (
        "MarioInput", "MarioFlags", "MarioActionGroups", "MarioActionFlags",
        "MarioParticleFlags", "GroundStep", "AirStep", "AirStepCheck", "WaterStep", "ModelStates")]
    definitions += [source("include/mario_geo_switch_case_ids.h").declaration("enum", name)
                    for name in ("MarioHandGSCId", "MarioCapGSCId", "MarioCapBlinkState")]
    definitions += [sm64.macros("ACT_.*")]
    definitions += [source("include/object_constants.h").macros("KING_BOBOMB_ACT_ACTIVE")]
    definitions += [source("include/mario_animation_ids.h").declaration("enum", "MarioAnimID")]
    definitions += [source("include/surface_terrains.h").declaration("enum", "SurfaceTypes")]
    definitions += [source("include/surface_terrains.h").declaration("enum", "TerrainType")]
    definitions += [source("include/surface_terrains.h").declaration("enum", "SurfaceFlags")]
    definitions += [source("include/surface_terrains.h").macros("SURFACE_IS_NOT_HARD|TERRAIN_MASK")]
    definitions += [source("include/surface_terrains.h").declaration("enum", "SurfaceClass")]
    definitions += [source("include/surface_terrains.h").macros(
        "SURFACE_IS_UNSAFE|SURFACE_IS_QUICKSAND|SURFACE_IS_WARP|SURFACE_IS_PAINTING_WARP|"
        "SURFACE_IS_INSTANT_WARP|INSTANT_WARP_INDEX_STOP")]
    definitions += [sm64.macros("COS[0-9]+")]
    definitions += [source("src/game/mario.c").macros("WATER_BREATH_.*")]
    definitions += [source("src/game/mario_dab.c").macros("DAB_.*")]
    definitions += [source("src/game/mario_dab.c").declaration("enum", "DabState")]
    definitions += [source("src/game/mario_actions_automatic.h").declaration("enum", "PolePositions")]
    definitions += [source("src/game/mario_actions_automatic.h").declaration("enum", "ActionStatesInCannon")]
    definitions += [source("include/object_constants.h").macros("OPENED_CANNON_ACT_.*")]
    definitions += ["#undef CUTSCENE_NONE", source("src/game/camera.h").declaration("enum", "Cutscenes")]
    cannon = source("src/game/behaviors/cannon.inc.c")
    functions += [cannon.function(name) for name in (
        "opened_cannon_act_idle", "opened_cannon_act_ready",
        "opened_cannon_act_shoot", "opened_cannon_act_resetting")]
    automatic = source("src/game/mario_actions_automatic.c")
    functions += [source("src/engine/graph_node.c").function("retrieve_animation_index")]
    functions += [automatic.function(name) for name in (
        "set_pole_position", "try_cancel_pole_handstand", "pole_grab_can_cancel",
        "act_top_of_pole_transition", "pole_jump_anim_y_translation", "act_top_of_pole",
        "mario_apply_automatic_buffer", "act_in_cannon")]
    functions += [source("src/game/mario.c").function("update_pole_handstand_facing")]
    definitions += [source("src/game/interaction.c").macros("MARIO_CAP_PICKUP_LOCK_FRAMES")]
    definitions += [source("src/game/mario_actions_stationary.h").declaration("enum", "ActionStatesStartSleeping")]
    definitions += [source("src/game/mario_actions_stationary.h").declaration("enum", "ActionStatesSleeping")]
    definitions += [source("src/game/mario_actions_stationary.h").declaration("enum", "ActionStatesTwirlLand")]
    definitions += [source("src/game/mario_actions_submerged.h").declaration("enum", "ActionStatesWaterPlunge")]
    definitions += [source("src/game/mario_actions_submerged.h").declaration("enum", "WaterPlungeFlags")]
    definitions += [source("src/game/mario_actions_submerged.h").declaration("enum", "ActionArgsFlutterKick")]
    definitions += [source("include/config/config_movement.h").macros(
        "WATER_PLUNGE_DRAG_SCALE|WATER_PLUNGE_FORWARD_DRAG|FALL_DAMAGE_HEIGHT_SMALL")]
    definitions += [source("src/game/mario_ground_twirl.c").macros(
        "GROUND_TWIRL_FRAMES|GROUND_TWIRL_HAT_LOSS_FRAMES")]
    spin_dizzy = source("src/game/mario_spin_dizzy.c")
    definitions += [spin_dizzy.region(r"^#define SPIN_DIZZY_TURNS\b",
                                      r"^void mario_spin_dizzy_reset\(")]
    definitions += [source("include/object_constants.h").declaration("enum", "oActionsBobOmbBuddy")]
    definitions += [source("include/object_constants.h").macros("GOOMBA_SIZE_.*|KOOPA_BP_.*|KOOPA_THE_QUICK_.*INDEX")]
    definitions += [source("include/config/config_movement.h").macros(
        "MARIO_GLOBAL_SPEED_CAP|MARIO_STRONG_STICK_THRESHOLD|JUMP_HEIGHT_VELOCITY_SCALE|GROUND_DIVE_HEIGHT_RETENTION|"
        "JUMP_FORWARD_SPEED_RETENTION|WALL_KICK_MIN_FORWARD_SPEED|FORWARD_DIVE_SPEED_BOOST|"
        "LONG_JUMP_SPEED_MULTIPLIER|LONG_JUMP_BOOST_CAP|WATER_JUMP_WALL_KICK_LOCK_FRAMES|WATER_JUMP_RIM_ASSIST_.*|"
        "WATER_JUMP_DIVE_FORWARD_.*|WATER_JUMP_AIR_.*")]
    definitions += [source("include/config/config_movement.h").macros(
        "MARIO_MAX_FALL_SPEED|FALL_DRAG_START_SPEED|FALL_DRAG_GRAVITY_REDUCTION_PER_UNIT|JUMP_RELEASE_UPWARD_DECEL")]
    definitions += [source("include/config/config_movement.h").macros("GROUND_NEUTRAL_.*_DECEL")]
    definitions += [source("src/game/mario_actions_airborne.h").macros(
        "DIVE_STARTUP_VULNERABLE_FRAMES|DIVE_WALL_BONK_GRACE_FRAMES|ACT_ARG_DIVE_FROM_ROLLOUT_CONTACT|ACT_ARG_FORWARD_ROLLOUT_.*|FORWARD_ROLLOUT_WALL_KICK_DELAY_FRAMES|ROLLOUT_WATER_WALK_DELAY_FRAMES")]
    definitions += [source("src/game/mario_actions_airborne.h").declaration("enum", "ActionStatesJumpKick")]
    definitions += [source("src/game/mario_actions_airborne.c").macros("JUMP_KICK_(ACTIVE|FOLLOWUP|BUFFER)_FRAMES")]
    definitions += [source("include/types.h").declaration("enum", "PunchStateTypes")]
    definitions += [source("src/game/mario_actions_moving.h").macros("GROUND_TWIRL_IMMUNE_REBOUND_.*|ACT_ARG_GROUND_SPEED_FROM_ROLLOUT")]
    definitions += [source("src/game/mario_actions_moving.h").declaration("enum", "ActionStatesWalking")]
    definitions += [source("src/game/mario_actions_airborne.h").macros("WALL_KICK_MIN_FORWARD_SPEED")]
    definitions += [source("src/game/mario_actions_moving.c").macros("GROUND_TWIRL_IMMUNE_REBOUND_ANIM_SPEED")]
    definitions += [source("src/game/mario_punch.h").declaration("enum", "ActionArgsPunchSequence")]
    functions += [source("src/game/interaction.c").function(name) for name in (
        "mario_is_breakdancing", "mario_breakdance_protects_from_damage")]
    definitions += [source("src/game/interaction.h").declaration("enum", name) for name in (
        "ObjInteractTypes", "Interactions", "InteractSubtypes", "InteractStatus", "AttackType")]
    definitions += [source("include/object_constants.h").declaration("enum", name) for name in (
        "ObjActiveFlags", "ObjMoveFlags", "ObjFlags")]
    definitions += [source("include/object_constants.h").declaration("enum", "ActiveParticleFlags")]
    definitions += [source("include/object_constants.h").macros(
        "SNUFIT_ACT_.*|OBJ_MOVE_MASK_IN_WATER|BOWSER_MINE_LAUNCH_ARG")]
    definitions += [source("src/game/mario_actions_cutscene.h").declaration("enum", "ActionStatesSquished")]
    definitions += [source("src/game/mario_actions_cutscene.h").declaration("enum", "ActionStatesQuicksandDeath")]
    definitions += [source("src/game/mario_actions_cutscene.h").declaration("enum", "ActionArgsQuicksandDeath")]
    definitions += [source("src/engine/graph_node.h").declaration("enum", "GraphRenderFlags")]
    definitions += [source("src/game/mario.h").declaration("enum", "MarioSpawnFallProtection")]
    definitions += [source("src/game/obj_behaviors.h").declaration("enum", "ObjCollisionFlags")]
    definitions += [source("include/object_constants.h").declaration("enum", "ObjGeneralDeathActions")]
    definitions += [source("include/object_constants.h").macros("BULLY_STYPE_.*")]
    mario = source("src/game/mario.c")
    functions += [mario.region(r"^s8 sTerrainSounds", r"^u32 mario_get_terrain_sound_addend")]
    functions += [mario.function(name) for name in (
        "mario_add_cap_power", "mario_clear_cap_flags", "mario_natural_cap_powers",
        "mario_get_terrain_sound_addend", "play_sound_and_spawn_particles", "play_mario_action_sound")]
    functions += [mario.function("play_mario_landing_sound").replace(
        "void play_mario_landing_sound(", "void real_play_mario_landing_sound(")]
    functions += [mario.function("play_mario_landing_sound_once").replace(
        "void play_mario_landing_sound_once(", "void real_play_mario_landing_sound_once(")]
    definitions += [mario.macros("WATER_HEALTH_.*|POISON_HEALTH_.*|HEALTH_REGEN_.*|MARIO_SOAKED_TAIL_FRAMES")]
    definitions += [source("src/game/mario.h").macros("MARIO_BURN_.*")]
    functions += [mario.function("mario_apply_burning_damage")]
    definitions += [source("include/config/config_movement.h").macros("IDLE_SLEEP_DELAY_.*")]
    # Expand the level enumeration from the production list without pulling
    # platform headers into this host harness.
    levels = re.findall(r"^(?:DEFINE_LEVEL|STUB_LEVEL)\([^,]+,\s*(LEVEL_\w+)",
                        source("levels/level_defines.h").mask, re.M)
    definitions += ["enum { LEVEL_NONE, " + ", ".join(levels) + " };"]
    functions += [source("src/game/physics_lab.c").function("physics_lab_is_cold_pool")]
    functions += [mario.function(name) for name in (
        "play_sound_if_no_flag", "ground_dive_launch_velocity", "set_mario_y_vel_based_on_fspeed", "set_mario_action_airborne", "set_mario_action_submerged",
        "prepare_ground_takeoff", "reset_mario_action_state", "set_mario_action", "update_mario_button_inputs", "landing_jump_action", "check_landing_attack_or_crouch",
        "check_common_hold_action_exits",
        "mario_stop_underwater_shell", "transition_submerged_to_walking", "transition_submerged_to_airborne", "mario_has_land_movement_underwater",
        "mario_is_low_land_pose", "mario_is_head_underwater", "mario_should_drown_on_land",
        "mario_has_water_contact", "mario_is_soaked", "mario_apply_soaked_status", "mario_consume_soaked_status",
        "mario_dry_from_wind", "mario_extinguish_soaked_burning", "mario_update_soaked_status",
        "mario_refresh_soaked_after_movement", "mario_emit_soaked_drips",
        "mario_is_shallow_water_drowning", "mario_has_shallow_water_death_pending",
        "mario_action_locks_camera", "reset_environmental_health_progress", "mario_reset_health_regeneration", "mario_health_regeneration_sleep_due",
        "update_water_health_reentry", "update_poison_health_reentry", "finish_mario_health_update",
        "mario_update_infinite_health", "mario_health_after_pending_damage", "mario_queue_respawn_healing",
        "update_mario_health", "update_mario_breath",
        "mario_update_c_up_input", "update_mario_poison_input",
        "update_surface_breath_sound", "mario_get_floor_class", "mario_facing_downhill",
        "mario_floor_is_slippery", "mario_floor_is_slope", "mario_floor_is_steep",
        "sink_mario_in_quicksand", "mario_move_buffer_family", "mario_is_buffered_landing", "mario_update_move_buffers",
        "mario_is_knockback_recovery", "update_bully_knockback_buffer", "update_mario_submerged_effects")]
    functions += [source("src/game/mario_actions_airborne.c").function(name) for name in (
        "backward_butt_slide_jump_blocks_uphill", "mario_restrict_air_uphill")]
    definitions += [source("include/config/config_movement.h").macros("JUMP_KICK_FIX")]
    functions += [mario.function("set_steep_jump_action").replace(
        "void set_steep_jump_action(", "void real_set_steep_jump_action(")]
    functions += [mario.function("set_jumping_action").replace(
        "s32 set_jumping_action(", "s32 real_set_jumping_action(").replace(
        "set_steep_jump_action(m);", "real_set_steep_jump_action(m);")]
    functions += [mario.function("set_mario_action_cutscene").replace(
        "u32 set_mario_action_cutscene(", "u32 real_set_mario_action_cutscene(")]
    functions += [mario.function("set_mario_action_moving").replace(
        "u32 set_mario_action_moving(", "u32 real_set_mario_action_moving(")]
    functions += [mario.function("mario_set_forward_vel").replace(
        "void mario_set_forward_vel(", "void real_mario_set_forward_vel(")]
    functions += [mario.function("mario_tick_cap_pickup_lock")]
    definitions += [mario.macros("CAP_FLICKER_.*")]
    definitions += [mario.region(r"^u64 sCapFlickerFrames\b",
                                r"^u32 update_and_return_cap_flags\(")]
    functions += [mario.function("update_and_return_cap_flags").replace(
        "u32 update_and_return_cap_flags(", "u32 real_update_and_return_cap_flags(")]
    functions += [mario.function("mario_reset_bodystate").replace(
        "void mario_reset_bodystate(", "void real_mario_reset_bodystate(")]
    functions += [mario.function(name) for name in (
        "mario_set_cap_render_state", "mario_update_hitbox_and_cap_model")]
    definitions += [source("src/game/liquid_particles.h").declaration("enum", name)
                    for name in ("LiquidParticleMaterial", "WaterDropletAction")]
    definitions += [source("src/game/liquid_particles.h").macros("LIQUID_PARTICLE_LAVA_SPLASH_SCALE")]
    functions += [source("src/game/behaviors/water_splashes_and_waves.inc.c").function(name)
                  for name in ("slide_kick_liquid_droplet_land", "bhv_water_droplet_loop", "bhv_water_droplet_splash_init",
                               "bhv_water_droplet_splash_fade")]
    functions += [source("src/game/mario_actions_submerged.c").function("check_common_submerged_cancels")]
    submerged = source("src/game/mario_actions_submerged.c")
    definitions += [submerged.macros("SWIMMING_ROLL_FRAMES")]
    definitions += [source("src/game/mario_actions_submerged.h").declaration("enum", "ActionStatesWaterPunch")]
    definitions += [source("src/game/mario_actions_airborne.h").declaration("enum", name)
                    for name in ("ActionStatesFlying", "ActionArgsFlying")]
    definitions += [source("include/config/config_movement.h").macros(
        "A_SWIM_STROKE_TAP_FRAMES|AIR_ENTRY_SWIM_STROKE_TAP_FRAMES|B_SWIM_ATTACK_TAP_FRAMES|WATER_JUMP_.*")]
    functions += [submerged.function(name) for name in (
        "is_normal_swimming_action", "can_control_swimming", "check_water_jump", "try_swimming_surface_launch",
        "cancel_swimming_roll", "update_swimming_roll", "mario_swimming_roll_is_active",
        "b_swim_has_priority", "clear_air_entry_stroke",
        "common_water_knockback_step", "update_air_entry_stroke", "update_water_down_repeat_buffer", "update_swimming_b_tap", "mario_swimming_update", "check_initial_swim_stroke",
        "act_water_plunge")]
    step = source("src/game/mario_step.c")
    functions += [step.function(name) for name in (
        "mario_play_bonk_sound", "mario_play_wall_bonk_sound", "mario_play_air_twirl_sound", "mario_bonk_reflection", "mario_is_on_floor", "mario_update_quicksand", "mario_is_in_vertical_wind")]
    functions += [step.function(name) for name in (
        "gravity_with_fall_drag", "water_gravity_scale", "apply_twirl_gravity", "should_strengthen_gravity_for_jump_ascent", "apply_gravity")]
    moving = source("src/game/mario_actions_moving.c")
    functions += [moving.function("apply_slope_accel")]
    functions += [moving.function("apply_landing_accel").replace(
        "s32 apply_landing_accel(", "s32 real_apply_landing_accel(")]
    functions += [moving.function("slide_bonk").replace("void slide_bonk(", "void real_slide_bonk(")]
    definitions += [moving.declaration("struct", "LandingAction")]
    functions += [moving.region(r"^struct LandingAction\s+sJumpLandAction\b", r"^Mat4 sFloorAlignMatrix")]
    definitions += [source("include/config/config_movement.h").macros("TRIPLE_JUMP_SPEED_THRESHOLD")]
    functions += [mario.function("set_jump_from_landing").replace(
        "s32 set_jump_from_landing(", "s32 real_set_jump_from_landing(")]
    functions += [moving.function(name) for name in (
        "set_triple_jump_action", "common_landing_cancels", "common_ground_knockback_action",
        "act_jump_land", "act_long_jump_land")]
    functions += [moving.function("try_burning_coin_recovery_jump").replace(
        "static s32 try_burning_coin_recovery_jump(", "s32 try_burning_coin_recovery_jump(")]
    functions += [source("src/game/mario_actions_airborne.c").function("finish_water_jump_pose")]

    functions += [source("src/game/mario_actions_airborne.c").function(name) for name in (
        "mario_can_wall_kick_backward_jump", "mario_has_wall_kick_slide_input", "wall_kick_priority_step_arg",
        "common_air_knockback_step", "check_kick_or_dive_in_air", "check_common_airborne_cancels", "act_jump", "act_double_jump",
        "act_dive", "act_jump_kick", "act_burning_jump", "act_burning_fall",
        "finish_rollout_landing", "act_forward_rollout", "act_backward_rollout",
        "accelerate_steep_water_jump", "update_water_jump_air_control",
        "act_water_jump", "act_hold_water_jump")]
    functions += [source("src/game/mario_actions_airborne.c").function("water_jump_hit_wall").replace(
        "static void water_jump_hit_wall(", "static void real_water_jump_hit_wall(")]
    functions += [source("src/engine/math_util.c").function("approach_f32")]
    definitions += [source("src/engine/math_util.h").macros("approach_angle")]
    definitions += [source("src/game/mario_actions_moving.h").macros("DIVE_SLIDE_ATTACK_FRAMES")]
    functions += [moving.function(name) for name in (
        "belly_slide_forward_stick_held", "can_rollout_from_belly_slide", "stomach_slide_action", "act_dive_slide",
        "act_slide_kick_slide", "act_hold_heavy_walking", "act_crawling", "coasting_ground_step", "act_burning_ground", "act_backflip_land")]
    functions += [moving.function("should_begin_sliding").replace(
        "s32 should_begin_sliding(", "s32 real_should_begin_sliding(")]
    definitions += [moving.macros("BELLY_SLIDE_FORWARD_STICK_DEADZONE|analog_stick_held_back")]
    definitions += [moving.macros("GROUND_SLIDE_SPEED_CAP"),
                    source("include/config/config_movement.h").macros("HOLD_HEAVY_TARGET_SPEED|CRAWL_TARGET_SPEED")]
    functions += [moving.function("update_sliding_angle"), moving.function("update_sliding").replace(
        "s32 update_sliding(", "s32 real_update_sliding(")]
    functions += [moving.function("update_decelerating_speed"),
                  moving.function("begin_braking_action"),
                  moving.function("start_moving_crouch_attack"),
                  moving.function("check_ground_dive_or_punch")]
    functions += [source("src/game/mario_actions_stationary.c").function("check_common_idle_cancels")
                  .replace("s32 check_common_idle_cancels(", "s32 ground_dive_idle_cancels(")]
    functions += [source("src/game/mario_actions_stationary.c").function(name) for name in (
        "check_common_stationary_cancels", "check_common_landing_cancels", "landing_step",
        "stopping_step", "act_braking_stop", "act_butt_slide_stop", "act_slide_kick_slide_stop",
        "start_stationary_crouch_attack", "act_crouching", "act_start_crouching", "act_stop_crouching",
        "act_start_crawling", "act_stop_crawling", "act_hold_heavy_idle",
        "act_freefall_land_stop", "act_twirl_land", "act_air_throw_land",
        "act_hold_jump_land_stop", "act_hold_freefall_land_stop", "act_backflip_land_stop", "act_coughing")]
    functions += [source("src/game/mario_actions_object.c").function(name) for name in (
        "animated_stationary_ground_step", "act_stomach_slide_stop", "act_releasing_bowser")]
    functions += [source("src/game/mario_actions_cutscene.c").function(name)
                  for name in ("act_squished", "act_disappeared", "act_quicksand_death")]
    interaction = source("src/game/interaction.c")
    definitions += [source("src/game/interaction.h").macros("INT_SUBTYPE_YOSHI_GIFT_COIN")]
    definitions += [interaction.macros("SMALL_OBJECT_THROW_SPEED_SCALE")]
    functions += [interaction.function("mario_small_object_throw_speed")]
    definitions += [source("include/config/config_movement.h").macros("SHINDOU_POLES")]
    functions += [interaction.function("reset_mario_pitch"), interaction.function("interact_pole")]
    definitions += [interaction.declaration("enum", "DefensiveContactResult")]
    functions += [interaction.function(name) for name in (
        "does_mario_have_normal_cap_on_head", "mario_can_lose_cap_to_enemy", "mario_remove_cap_powers",
        "mario_blow_off_cap_in_direction", "mario_blow_off_cap_from_wind",
        "mario_lose_cap_on_lethal_damage_with_speed",
        "mario_retrieve_cap",
        "mario_cap_pickup_is_blocked")]
    functions += [interaction.region(r"^Bool32 mario_lose_cap_to_enemy\(u32 enemyType\)",
        r"^#endif\n\nvoid mario_blow_off_cap_from_wind")]
    functions += [interaction.function("mario_extinguish_fire"), interaction.function("interact_coin"),
                  interaction.function("interact_water_ring"),
                  interaction.function("mario_lose_cap_to_shallow_water_death")]
    functions += [interaction.function(name) for name in (
        "mario_vanish_ignores_hostile_contact", "mario_ground_twirl_outside_enemy_body",
        "mario_handle_defensive_contact")]
    camera = source("src/game/camera.c")
    definitions += [source("src/game/camera.h").declaration("enum", name)
                    for name in ("CameraSelection", "CameraAngle", "CameraModes",
                                 "CameraMovementFlags", "CameraStatus", "CameraSounds", "CameraEvent")]
    definitions += [source("src/game/camera.h").macros("CAM_MODE_.*")]
    definitions += [camera.macros("REONUCAM_R_.*FRAMES")]
    functions += [camera.function("play_c_up_rejection_sound")]
    functions += [camera.function(name) for name in ("dialogue_camera_input_mask", "reonucam_reset_r_tap", "reset_camera_mode", "update_camera_hud_status", "set_mode_c_up", "play_sound_cbutton_up", "play_c_up_entry_sound_if_zooming", "play_c_up_exit_sound_once")]
    behavior = source("src/engine/behavior_script.c")
    definitions += [behavior.macros("OBJ_OPACITY_(NEAR|LENGTH)")]
    definitions += [source("src/game/object_helpers.h").declaration("enum", "MarioRoomStates")]
    definitions += [source("include/object_constants.h").declaration("enum", "HeldState")]
    functions += [behavior.function("cur_obj_drawing_distance"),
                  behavior.function("cur_obj_update_distance_visibility")]
    definitions += [source("src/engine/behavior_script.h").macros("ENEMY_DRAW_DISTANCE_EXIT_SCALE")]
    functions += [behavior.function("object_has_silhouette"),
                  behavior.function("obj_set_opacity_from_cam_dist")]
    definitions += [source("src/game/mario_ground_twirl.c").macros("METAL_ENEMY_CONTACT_TIMEOUT")]
    definitions += [source("include/object_constants.h").macros("OBJ_MOVE_MASK_ON_GROUND")]
    functions += [source("src/game/mario_ground_twirl.c").function("mario_metal_stun_enemy_on_contact")]
    functions += [interaction.function("stunned_enemy_can_interact")]

    functions += [source("src/game/behaviors/water_splashes_and_waves.inc.c").function("bhv_wave_trail_shrink")]
    definitions += [source("include/object_constants.h").macros("WHITE_PUFF_EXPLODE_BP_.*")]
    functions += [source("src/game/behaviors/ground_particles.inc.c").function("bhv_water_down_mist_init")]
    functions += [source("src/game/behaviors/water_objs.inc.c").function(name)
                  for name in ("bhv_spawn_mario_breath_bubble", "bhv_small_water_wave_loop")]
    snufit = source("src/game/behaviors/snufit.inc.c")
    functions += [snufit.function(name) for name in ("snufit_act_idle", "snufit_act_shoot", "bhv_snufit_loop")]
    functions += [source("src/game/mario_dab.c").region(r"^static struct \{", r"\Z")]
    stationary = source("src/game/mario_actions_stationary.c")
    functions += [stationary.function(name) for name in (
        "play_anim_sound", "sleep_input_during_dab_lock", "act_start_sleeping", "act_sleeping", "act_waking_up")]
    functions += [source("src/game/mario_ground_twirl.c").function(name) for name in (
        "mario_ground_twirl_is_held_pose", "mario_ground_twirl_loses_cap_on_damage")]
    functions += [spin_dizzy.function(name) for name in (
        "mario_spin_dizzy_reset", "mario_spin_dizzy_can_track", "mario_spin_dizzy_update")]
    functions += [mario.function("mario_update_environment_cheats"),
                  source("src/game/mario_actions_cutscene.c").function("check_for_instant_quicksand"),
                  mario.function("mario_update_gameplay_options")]
    functions += [mario.function("mario_has_normal_cap_in_current_area"),
                  mario.function("init_mario")]
    functions += [source("src/game/obj_behaviors.c").function("obj_check_floor_death")]
    functions += [source("src/game/behaviors/bully.inc.c").function("bully_step")]
    functions += [source("src/game/behaviors/bobomb.inc.c").function("bobomb_buddy_act_turn_to_talk")]

def mutations_fixes(root):
    yield "fixes", "original_metal_allows_water_damage", "fixes.cap_presets_health", {
        "src/game/mario.c": MutationSource(root / "src/game/mario.c").replace_in_function(
            "mario_has_water_damage_protection", "game_options_get(GO_METAL_CAP) == 1", "FALSE")}

    path = "src/game/mario.c"
    for name, case, function, before, after in (
        ("ignore_gameplay_disable_cleanup", "fixes.gameplay_option_transitions",
         "mario_update_gameplay_options", "if (!game_options_get(GO_TWIRLS))", "if (FALSE)"),
        ("ignore_passive_healing_option", "fixes.gameplay_status_gates", "update_mario_health",
         "!game_options_get(GO_PASSIVE_HEAL) || ", ""),
    ):
        yield "fixes", name, case, {
            path: MutationSource(root / path).replace_in_function(function, before, after)}
    soaked_path = "src/game/mario.c"
    for name, function, before, after in (
        ("retain_soaked_in_wind", "mario_dry_from_wind",
         "mario_consume_soaked_status(m);", "(void) m;"),
        ("rewet_after_wind_movement", "mario_refresh_soaked_after_movement",
         "mario_dry_from_wind(m);", "(void) m;"),
    ):
        yield "fixes", name, "fixes.soaked_wind_drying", {
            soaked_path: MutationSource(root / soaked_path).replace_in_function(function, before, after)}
    for name, case, path, function, old, new in (
        ("old_ground_burn_damage", "burning_episode_damage", "src/game/mario_actions_moving.c", "act_burning_ground",
         "mario_apply_burning_damage(m, 2);\n    if (m->health", "m->health -= 10;\n    if (m->health"),
        ("old_jump_burn_damage", "burning_episode_damage", "src/game/mario_actions_airborne.c", "act_burning_jump",
         "mario_apply_burning_damage(m, 3);", "m->health -= 10;"),
        ("old_fall_burn_damage", "burning_episode_damage", "src/game/mario_actions_airborne.c", "act_burning_fall",
         "mario_apply_burning_damage(m, 3);", "m->health -= 10;"),
        ("unbounded_burn_damage", "burning_episode_damage", "src/game/mario.c", "mario_apply_burning_damage",
         "s32 current = CLAMP(timer, 0, MARIO_BURN_TIMER_LIMIT);", "s32 current = MAX(timer, 0);"),
        ("skip_final_burn_remainder", "burning_extinguish_and_death", "src/game/mario_actions_moving.c", "act_burning_ground",
         "mario_apply_burning_damage(m, 2);\n        return", "(void) m;\n        return"),
        ("ignore_final_burn_death", "burning_extinguish_and_death", "src/game/mario_actions_moving.c", "act_burning_ground",
         "m->health < 0x100 ? ACT_STANDING_DEATH : ACT_WALKING", "ACT_WALKING"),
    ):
        yield "fixes", name, "fixes." + case, {
            path: MutationSource(root / path).replace_in_function(function, old, new)}
    cap_path = "src/game/mario.c"
    yield "fixes", "short_cap_warning", "fixes.cap_warning_timing", {
        cap_path: MutationSource(root / cap_path).replace_in_function("update_and_return_cap_flags",
            "m->capTimer < CAP_FLICKER_FRAMES", "m->capTimer < 64")}
    yield "fixes", "instant_cap_blink", "fixes.cap_warning_blend", {
        cap_path: MutationSource(root / cap_path).replace_in_function("mario_update_hitbox_and_cap_model",
            "if (blend)", "if (FALSE && blend)")}
    yield "fixes", "linger_cap_blend", "fixes.cap_warning_blend", {
        cap_path: MutationSource(root / cap_path).replace_in_function("mario_update_hitbox_and_cap_model",
            "bodyState->capBlinkBlend = MARIO_CAP_BLINK_NONE;",
            "bodyState->capBlinkBlend = MARIO_CAP_BLINK_GEOMETRY;")}
    path = "src/game/mario_actions_stationary.c"
    yield "fixes", "lock_throw_landing_controls", "fixes.air_throw_landing_controls", {
        path: MutationSource(root / path).replace_in_function("act_air_throw_land",
            "m->heldObj == NULL && check_common_landing_cancels(m, ACT_JUMP)",
            "FALSE && check_common_landing_cancels(m, ACT_JUMP)")}
    path = "src/game/interaction.c"
    text = MutationSource(root / path)
    old = "#define SMALL_OBJECT_THROW_SPEED_SCALE 1.1025f"
    yield "fixes", "old_throw_launch_speed", "fixes.small_object_throw_speed", {
        path: text.replace_unique(old, "#define SMALL_OBJECT_THROW_SPEED_SCALE 1.05f")}
    priority_path = "src/game/mario_actions_airborne.c"
    yield "fixes", "prioritize_water_kick_during_startup_lock", "fixes.water_jump_wall_kick_priority", {
        priority_path: MutationSource(root / priority_path).replace_in_function("act_water_jump",
            "if (m->actionTimer > WATER_JUMP_WALL_KICK_LOCK_FRAMES)", "if (TRUE)")}
    path = "src/game/mario_step.c"
    yield "fixes", "missing_thicc_cap_water_gravity", "fixes.thicc_submerged_gravity", {
        path: MutationSource(root / path).replace_in_function("water_gravity_scale",
            "return HE_THICC_WATER_GRAVITY_SCALE;", "return 1.0f;")}

    mario_path = "src/game/mario.c"
    mario = MutationSource(root / mario_path)
    yield "fixes", "omit_early_steep_kick_reduction", "fixes.slope_jump_kick_height", {
        mario_path: mario.replace_in_function("set_mario_action_airborne",
            "|| m->action == ACT_STEEP_JUMP)) {", ")) {")}
    yield "fixes", "reduce_late_jump_kick", "fixes.early_jump_kick_boundaries", {
        mario_path: mario.replace_in_function("set_mario_action_airborne",
            "m->framesSinceA <= 3", "m->framesSinceA <= 4")}
    path = "src/game/mario_actions_airborne.c"
    air = MutationSource(root / path)
    condition = "((m->flags & MARIO_WING_CAP) && (m->controller->buttonDown & A_BUTTON))"
    yield "fixes", "remove_held_wing_dive_protection", "fixes.wing_dive_burial", {
        path: air.replace_in_region("#ifdef NO_GETTING_BURIED", "\nvoid play_far_fall_sound",
            condition, "FALSE")}
    action_path = "src/game/mario_actions_airborne.c"
    yield "fixes", "voice_on_level_act_water_jump_catch", "fixes.water_jump_ledge_voice", {
        action_path: MutationSource(root / action_path).replace_in_function("act_water_jump",
            "m->vel[1] <= -LEDGE_GRAB_VOICE_MIN_FALL_SPEED ? 0 : ACT_ARG_LEDGE_GRAB_GENTLE",
            "0")}
    yield "fixes", "voice_on_slight_water_jump_descent", "fixes.water_jump_ledge_voice", {
        action_path: MutationSource(root / action_path).replace_in_function("act_water_jump",
            "m->vel[1] <= -LEDGE_GRAB_VOICE_MIN_FALL_SPEED", "m->vel[1] < 0.0f")}
    for name, case, function, old, new in (
        ("ignore_held_handstand_z", "fixes.pole_handstand_z_cancel", "act_top_of_pole",
         "if (try_cancel_pole_handstand(m))", "if (FALSE && try_cancel_pole_handstand(m))"),
        ("ignore_transition_handstand_z", "fixes.pole_handstand_z_cancel", "act_top_of_pole_transition",
         "if (try_cancel_pole_handstand(m))", "if (FALSE && try_cancel_pole_handstand(m))"),
        ("lose_handstand_drop_facing", "fixes.pole_handstand_z_cancel", "try_cancel_pole_handstand",
         "s16 yaw = m->marioObj->header.gfx.angle[1];", "s16 yaw = m->faceAngle[1];"),
        ("lose_handstand_drop_root", "fixes.pole_handstand_z_cancel", "try_cancel_pole_handstand",
         "return_mario_anim_y_translation(m)", "0.0f"),
        ("cancel_handstand_with_held_z", "fixes.pole_handstand_preserved_controls", "try_cancel_pole_handstand",
         "INPUT_Z_PRESSED", "INPUT_Z_DOWN"),
    ):
        path = "src/game/mario_actions_automatic.c"
        yield "fixes", name, case, {
            path: MutationSource(root / path).replace_in_function(function, old, new)}

    step_path = "src/game/mario_step.c"
    step = MutationSource(root / step_path)
    yield "fixes", "full_speed_wall_bonk_recoil", "fixes.wall_bonk_momentum", {
        step_path: step.replace_in_function("mario_set_wall_bonk_momentum",
            "absf(recoilSpeed) * WALL_BONK_MOMENTUM_RETENTION", "absf(recoilSpeed)")}
    yield "fixes", "fixed_wall_bonk_recoil", "fixes.wall_bonk_momentum", {
        step_path: step.replace_in_function("mario_set_wall_bonk_momentum",
            "MAX(absf(recoilSpeed) * WALL_BONK_MOMENTUM_RETENTION, minimumSpeed)", "minimumSpeed")}
    yield "fixes", "remove_wall_bonk_minimum", "fixes.wall_bonk_momentum", {
        step_path: step.replace_in_function("mario_set_wall_bonk_momentum",
            "MAX(absf(recoilSpeed) * WALL_BONK_MOMENTUM_RETENTION, minimumSpeed)",
            "absf(recoilSpeed) * WALL_BONK_MOMENTUM_RETENTION")}
    air_path = "src/game/mario_actions_airborne.c"
    air = MutationSource(root / air_path)
    yield "fixes", "fixed_wall_bonk_air_recovery", "fixes.wall_bonk_momentum", {
        air_path: air.replace_in_function("common_air_knockback_step",
            "if (wallBonkMomentum || bowserMineLaunch)", "if (bowserMineLaunch)")}
    moving_path = "src/game/mario_actions_moving.c"
    moving = MutationSource(root / moving_path)
    yield "fixes", "clamp_wall_bonk_ground_recovery", "fixes.wall_bonk_recovery", {
        moving_path: moving.replace_in_function("common_ground_knockback_action",
            "if (!m->wallBonkMomentum)", "if (TRUE)")}
    yield "fixes", "snap_wall_bonk_late_recovery", "fixes.wall_bonk_recovery", {
        moving_path: moving.replace_in_function("common_ground_knockback_action",
            "animFrame < checkFrame || m->wallBonkMomentum", "animFrame < checkFrame")}
    yield "fixes", "delay_bonk_recovery", "fixes.bonk_recovery_boundary", {
        moving_path: moving.replace_in_function("common_ground_knockback_action",
            "quickTwirlRebound || earlyBonkRecovery", "quickTwirlRebound || (earlyBonkRecovery && FALSE)")}
    yield "fixes", "shorten_damage_recovery", "fixes.bonk_recovery_boundary", {
        moving_path: moving.replace_in_function("common_ground_knockback_action",
            "m->action == ACT_GROUND_BONK || m->wallBonkMomentum", "TRUE")}
    yield "fixes", "shorten_fatal_bonk_recovery", "fixes.bonk_recovery_boundary", {
        moving_path: moving.replace_in_function("common_ground_knockback_action",
            "m->health >= 0x100", "TRUE")}
    path = "src/game/mario.c"
    mario = MutationSource(root / path)
    for name, before, after in (
        ("restore_thirty_second_regeneration", "#define HEALTH_REGEN_START_FRAMES (45 * 30)",
         "#define HEALTH_REGEN_START_FRAMES 900"),
        ("restore_two_second_regeneration_wedges", "#define HEALTH_REGEN_WEDGE_FRAMES (3 * 30)",
         "#define HEALTH_REGEN_WEDGE_FRAMES 60")):
        yield "fixes", name, "fixes.health_regeneration_timing", {path: mario.replace_unique(before, after)}
    yield "fixes", "regenerate_while_falling_asleep_or_sleeping", "fixes.health_regeneration_idle_reset", {
        path: mario.replace_in_function("update_mario_health", " || sleepDue", " || (sleepDue && FALSE)")}
    yield "fixes", "ignore_equivalent_sleep_in_other_states", "fixes.health_regeneration_equivalent_sleep", {
        path: mario.replace_in_function("mario_health_regeneration_sleep_due",
            "return sHealthRegenInactiveFrames >= sleepDelay;",
            "return FALSE;")}
    path = "src/game/mario.c"
    yield "fixes", "lose_fountain_unlock", "fixes.fountain_unlock_drowning", {
        path: MutationSource(root / path).replace_in_function("finish_mario_health_update",
            "&& mario_character_can_unlock(m)", "&& mario_character_can_unlock(m) && FALSE")}
    yield "fixes", "skip_spawn_character", "fixes.character_spawn_lifecycle", {
        path: MutationSource(root / path).replace_in_function("init_mario",
            "mario_character_commit(gMarioState, gCurrDemoInput != NULL || gCurrCreditsEntry != NULL);", "mario_character_commit(gMarioState, TRUE);")}
    checks = [
        ("interrupt_held_blj", "fixes.blj_controls", "src/game/mario.c", "landing_jump_action",
         "m->controller->stickMag > 0.0f", "m->controller->stickMag >= MARIO_STRONG_STICK_THRESHOLD"),
        ("keep_respawn_mario_mode", "fixes.death_camera_mode", "src/game/camera.c", "reset_camera_mode",
         "c->mode = c->defMode;", "c->mode = c->mode;"),
        ("show_unattended_shell", "fixes.shell_silhouette", "src/engine/behavior_script.c", "object_has_silhouette",
         "return gMarioState->riddenObj == obj || gMarioState->heldObj == obj;", "return TRUE;"),
        ("remove_metal_contact_grace", "fixes.metal_stunned_contact", "src/game/mario_ground_twirl.c", "mario_metal_stun_enemy_on_contact",
         "obj->metalContactCooldown = METAL_ENEMY_CONTACT_TIMEOUT;", "obj->metalContactCooldown = 0;"),
        ("leave_respawn_hidden", "fixes.crush_respawn_visibility", "src/game/mario.c", "init_mario",
         "gMarioState->marioObj->header.gfx.node.flags |= GRAPH_RENDER_ACTIVE;", "(void)0;"),
        ("duplicate_cap_on_same_area_warp", "fixes.normal_cap_same_area_warp", "src/game/mario.c", "init_mario",
         "!mario_has_normal_cap_in_current_area()",
         "(mario_has_normal_cap_in_current_area() || !mario_has_normal_cap_in_current_area())"),
        ("delay_dab_cap_vulnerability", "fixes.cap_pose_five_frame_windows", "src/game/mario_dab.c",
         "mario_dab_loses_cap_on_damage", ">= DAB_CAP_DAMAGE_FRAMES", ">= 10"),
        ("delay_twirl_pose_cap_vulnerability", "fixes.cap_pose_five_frame_windows", "src/game/mario_ground_twirl.c",
         "mario_ground_twirl_loses_cap_on_damage", ">= GROUND_TWIRL_HAT_LOSS_FRAMES", ">= 10"),
        ("omit_dizzy_cap_loss", "fixes.spin_dizzy_cap_loss", "src/game/mario_spin_dizzy.c",
         "mario_spin_dizzy_update", "mario_blow_off_cap(m, 15.0f);",
         "if (FALSE) mario_blow_off_cap(m, 15.0f);"),
        ("apply_sl_death_height_in_lab", "fixes.lab_chill_bully_spawn", "src/game/behaviors/bully.inc.c", "bully_step",
         " && gCurrLevelNum != LEVEL_PHYSICS_LAB", ""),
        ("keep_takeoff_z_edge", "fixes.ground_chords", "src/game/mario.c", "prepare_ground_takeoff",
         "m->input &= ~INPUT_Z_PRESSED;", "(void)0;"),
        ("keep_ground_dive_height", "fixes.ground_dive_height", "src/game/mario.c", "ground_dive_launch_velocity",
         "height = GROUND_DIVE_HEIGHT_RETENTION *", "height = 1.0f *"),
        ("flatten_ground_dive", "fixes.ground_dive_sequence", "src/game/mario.c", "set_mario_action_airborne",
         "m->vel[1] = ground_dive_launch_velocity(m->vel[1]);", "m->vel[1] = 0.0f * ground_dive_launch_velocity(m->vel[1]);"),
        ("keep_dialogue_velocity", "fixes.dialogue_stop", "src/game/mario.c", "set_mario_action",
         "mario_set_forward_vel(m, 0.0f);", "mario_set_forward_vel(m, m->forwardVel);"),
        ("use_pitched_shore_speed", "fixes.shoreline", "src/game/mario.c", "transition_submerged_to_walking",
         "sqrtf(sqr(m->vel[0]) + sqr(m->vel[2]))", "m->forwardVel"),
        ("keep_water_jump_pitch", "fixes.water_dive_pose", "src/game/mario_actions_airborne.c", "finish_water_jump_pose",
         "m->faceAngle[0] = m->faceAngle[2] = 0;", "m->faceAngle[2] = 0;"),
        ("keep_expired_knockback_a", "fixes.bully_release", "src/game/mario.c", "update_bully_knockback_buffer",
         "if (m->framesSinceA >= 6)", "if (FALSE)"),
        ("block_landing_attack", "fixes.landing_controls", "src/game/mario_actions_moving.c", "common_landing_cancels",
         "if (check_landing_attack_or_crouch(m))", "if (FALSE && check_landing_attack_or_crouch(m))"),
        ("lose_double_jump_wall_kick_origin", "fixes.double_jump_wall_kick_followup", "src/game/mario.c", "reset_mario_action_state",
         "m->action == ACT_DOUBLE_JUMP\n                ||", "FALSE\n                ||"),
        ("ordinary_jump_after_double_jump_wall_kick", "fixes.double_jump_wall_kick_followup", "src/game/mario_actions_moving.c", "act_jump_land",
         "m->wallKickFromDoubleJump ?", "FALSE ?"),
        ("lose_stopped_wall_kick_triple", "fixes.double_jump_wall_kick_followup", "src/game/mario.c", "set_jump_from_landing",
         "m->wallKickFromDoubleJump ?", "FALSE ?"),
        ("retain_wall_kick_chain_on_interrupt", "fixes.double_jump_wall_kick_interruptions", "src/game/mario.c", "reset_mario_action_state",
         "default:\n            m->wallKickFromDoubleJump = FALSE;", "default:\n            break;"),
        ("skip_jump_kick_touchdown_pose", "fixes.jump_kick_grounded_chain", "src/game/mario_actions_airborne.c", "act_jump_kick",
         "set_mario_animation(m, MARIO_ANIM_GENERAL_LAND);", "(void)0;"),
        ("sink_above_quicksand", "fixes.airborne_sand", "src/game/mario_step.c", "mario_update_quicksand",
         "if (!mario_is_on_floor(m))", "if (FALSE)"),
        ("use_standing_breath_height", "fixes.prone_water", "src/game/mario.c", "mario_is_head_underwater",
         "50.0f : 140.0f", "140.0f : 140.0f"),
        ("skip_prone_drowning", "fixes.drowning_recovery", "src/game/mario.c", "mario_should_drown_on_land",
         "mario_is_low_land_pose(m) &&", "FALSE &&"),
        ("sink_deep_water_as_shallow", "fixes.shallow_water_sink_death", "src/game/mario.c", "mario_is_shallow_water_drowning",
         "m->pos[1] >= m->waterLevel - 100", "TRUE"),
        ("skip_shallow_cap_loss", "fixes.shallow_water_sink_death", "src/game/mario_actions_cutscene.c", "act_quicksand_death",
         "mario_lose_cap_to_shallow_water_death(m);", "if (FALSE) mario_lose_cap_to_shallow_water_death(m);"),
        ("ignore_vanish_defense", "fixes.vanish_contacts", "src/game/interaction.c", "mario_handle_defensive_contact",
         "if (mario_vanish_ignores_hostile_contact(m, object, interactType))",
         "if (FALSE && mario_vanish_ignores_hostile_contact(m, object, interactType))"),
        ("restore_giant_goomba_twirl_radius", "fixes.giant_goomba_twirl_contact", "src/game/interaction.c", "mario_handle_defensive_contact",
         "if (twirlActive && mario_ground_twirl_outside_enemy_body(m, object))", "if (FALSE)"),
        ("resume_held_camera_buttons", "fixes.dialogue_camera", "src/game/camera.c", "dialogue_camera_input_mask",
         "return sDialogueCameraBlockedButtons;", "return 0;"),
        ("delete_odd_submerged_trails", "fixes.wave_trails", "src/game/behaviors/water_splashes_and_waves.inc.c", "bhv_wave_trail_shrink",
         "o->oSubAction == 0 &&", "TRUE &&"),
        ("restore_old_snufit_orbit", "fixes.snufit_resume", "src/game/behaviors/snufit.inc.c", "bhv_snufit_loop",
         "if (o->oGroundTwirlStunDuration != 0)", "if (FALSE)"),
        ("repeat_buddy_talk_sound", "fixes.buddy_talk_sound", "src/game/behaviors/bobomb.inc.c", "bobomb_buddy_act_turn_to_talk",
         "if (o->oTimer == 0)", "if (TRUE)"),
        ("reverse_inverse_dab", "fixes.dab_inverse_analog", "src/game/mario_dab.c", "update_dab_analog_pose",
         "amount * (DAB_ANALOG_FRAME_COUNT - 1)", "(1.0f - amount) * (DAB_ANALOG_FRAME_COUNT - 1)"),
        ("cancel_forward_dab", "fixes.dab_inverse_analog", "src/game/mario_dab.c", "mario_dab_action",
         "m->input &= ~(INPUT_NONZERO_ANALOG | INPUT_A_PRESSED | INPUT_B_PRESSED | INPUT_FIRST_PERSON);",
         "(void)0;"),
        ("cancel_dab_with_buttons", "fixes.dab_analog_cancel_lock", "src/game/mario_dab.c", "mario_dab_action",
         "m->input &= ~(INPUT_NONZERO_ANALOG | INPUT_A_PRESSED | INPUT_B_PRESSED | INPUT_FIRST_PERSON);",
         "m->input &= ~INPUT_NONZERO_ANALOG;"),
        ("cancel_nonforward_dab", "fixes.dab_analog_cancel_lock", "src/game/mario_dab.c", "dab_analog_stick_amount",
         "return CLAMP(forward / 64.0f, 0.0f, 1.0f);",
         "if (forward <= 0.0f) return -1.0f;\n    return CLAMP(forward / 64.0f, 0.0f, 1.0f);"),
        ("restore_dab_controls_before_eligibility", "fixes.dab_analog_cancel_lock", "src/game/mario_dab.c", "mario_dab_action",
         "canContinue = !cancel && dab_can_start(m);", "m->input = savedInput;\n    canContinue = !cancel && dab_can_start(m);"),
        ("control_dab_without_z", "fixes.dab_inverse_analog", "src/game/mario_dab.c", "dab_analog_stick_amount",
         "(L_TRIG | Z_TRIG)) != (L_TRIG | Z_TRIG)", "L_TRIG) != L_TRIG"),
        ("omit_initial_dab_voice", "fixes.dab_input_sound", "src/game/mario_dab.c", "start_dab",
         "    set_mario_action(m, ACT_DAB, neutralAction);\n    play_sound_if_no_flag(m, SOUND_MARIO_PUNCH_HOO, MARIO_MARIO_SOUND_PLAYED);",
         "    set_mario_action(m, ACT_DAB, neutralAction);"),
        ("omit_repeat_dab_voice", "fixes.dab_repeat_sound", "src/game/mario_dab.c", "start_dab",
         "        play_sound_if_no_flag(m, SOUND_MARIO_PUNCH_HOO, MARIO_MARIO_SOUND_PLAYED);\n        return TRUE;",
         "        return TRUE;"),
        ("restore_dense_underwater_trails", "fixes.wave_spacing", "src/game/mario.c", "update_mario_submerged_effects",
         "100.0f * 100.0f", "85.0f * 85.0f"),
        ("keep_ground_pound_lava_drift", "fixes.metal_lava_launch", "src/game/mario.c", "set_mario_action_airborne",
         "\n                || actionArg == ACT_ARG_LAVA_BOOST_METAL_GROUND_POUND", ""),
        ("protect_metal_water_health", "fixes.cap_environment_vulnerabilities", "src/game/mario.c", "update_mario_health",
         "&& !mario_has_water_damage_protection(m) && !(m->action & ACT_FLAG_INTANGIBLE)",
         "&& !(m->flags & (MARIO_METAL_CAP | MARIO_VANISH_CAP))\n                    && !(m->action & ACT_FLAG_INTANGIBLE)"),
        ("protect_powered_hat_from_theft", "fixes.powered_cap_theft_and_wind", "src/game/interaction.c", "mario_can_lose_cap_to_enemy",
         "(MARIO_NORMAL_CAP | MARIO_CAP_ON_HEAD))", "(MARIO_CAPS | MARIO_CAP_ON_HEAD))"),
        ("protect_vanish_wind_hat", "fixes.powered_cap_theft_and_wind", "src/game/interaction.c", "mario_blow_off_cap_from_wind",
         "MARIO_WING_CAP | MARIO_METAL_CAP", "MARIO_WING_CAP | MARIO_METAL_CAP | MARIO_VANISH_CAP"),
        ("protect_vanish_poison_health", "fixes.cap_environment_vulnerabilities", "src/game/mario.c", "update_mario_health",
         "if (!(m->flags & MARIO_METAL_CAP) && !gDebugLevelSelect)",
         "if (!(m->flags & (MARIO_METAL_CAP | MARIO_VANISH_CAP)) && !gDebugLevelSelect)"),
        ("drain_vanish_water_health", "fixes.vanish_environment", "src/game/mario.c", "update_mario_health",
         "&& !mario_has_water_damage_protection(m) && !(m->action & ACT_FLAG_INTANGIBLE)",
         "&& !(m->flags & MARIO_METAL_CAP)\n                    && !(m->action & ACT_FLAG_INTANGIBLE)"),
        ("drown_vanish_land_actions", "fixes.vanish_environment", "src/game/mario.c", "mario_should_drown_on_land",
         "!mario_has_water_damage_protection(m)", "!(m->flags & MARIO_METAL_CAP)"),
        ("drown_vanish_from_lethal_hit", "fixes.vanish_environment", "src/game/mario_actions_submerged.c", "check_common_submerged_cancels",
         "mario_has_current_vanish_cap(m) ? ACT_WATER_DEATH", "FALSE ? ACT_WATER_DEATH"),
        ("drain_vanish_breath", "fixes.vanish_breath", "src/game/mario.c", "update_mario_breath",
         "!mario_has_water_damage_protection(m)", "!(m->flags & MARIO_METAL_CAP)"),
        ("suppress_vanish_land_bubbles", "fixes.vanish_bubbles", "src/game/mario.c", "update_mario_submerged_effects",
         "vanishUnderwater || ((m->action & ACT_GROUP_MASK)", "FALSE || ((m->action & ACT_GROUP_MASK)"),
        ("emit_dry_vanish_bubbles", "fixes.vanish_bubbles", "src/game/mario.c", "update_mario_submerged_effects",
         "&& mario_is_head_underwater(m);", "&& TRUE;"),
        ("suppress_shallow_vanish_bubbles", "fixes.vanish_bubbles", "src/game/mario.c", "update_mario_submerged_effects",
         "if (vanishUnderwater || (m->pos[1]", "if (FALSE || (m->pos[1]"),
        ("rewet_without_new_contact", "fixes.soaked_status", "src/game/mario.c", "mario_update_soaked_status",
         "else if (m->soakedTimer != 0)", "else if (TRUE)"),
        ("lose_dry_drip_splash", "fixes.soaked_status", "src/game/behaviors/water_splashes_and_waves.inc.c", "bhv_water_droplet_loop",
         "if (o->oAction == 1)", "if (FALSE)"),
        ("leave_drops_behind_mario", "fixes.soaked_status", "src/game/behaviors/water_splashes_and_waves.inc.c", "bhv_water_droplet_loop",
         "o->oPosX += (mario->oPosX - o->oHomeX) * 0.8f;", "o->oPosX += (mario->oPosX - o->oHomeX) * 0.0f;"),
        ("keep_burning_after_soaking", "fixes.soaked_status", "src/game/mario.c", "mario_extinguish_soaked_burning",
         "mario_extinguish_fire(m);", "(void)m;"),
        ("keep_full_soaked_splash", "fixes.soaked_status", "src/game/behaviors/water_splashes_and_waves.inc.c", "bhv_water_droplet_splash_init",
         "cur_obj_scale(0.9f);", "cur_obj_scale(1.5f);"),
        ("stop_soaked_splash_fade", "fixes.soaked_status", "src/game/behaviors/water_splashes_and_waves.inc.c", "bhv_water_droplet_splash_fade",
         "o->oOpacity = MAX(o->oOpacity - 30, 0);", "o->oOpacity = MAX(o->oOpacity - 0, 0);"),
    ]
    for name, case, path, function, before, after in checks:
        yield "fixes", name, case, {path: MutationSource(root / path).replace_in_function(function, before, after)}
    path = "src/game/mario_actions_submerged.c"
    submerged = MutationSource(root / path)
    yield "fixes", "drop_air_entry_stroke_update", "fixes.air_entry_stroke_buffer", {
        path: submerged.replace_in_function("mario_swimming_update",
            "update_air_entry_stroke(m);", "if (FALSE) update_air_entry_stroke(m);")}
    yield "fixes", "shorten_air_entry_stroke_window", "fixes.air_entry_stroke_buffer", {
        path: submerged.replace_in_function("update_air_entry_stroke",
            "++sSwimming.entryTapFrames > AIR_ENTRY_SWIM_STROKE_TAP_FRAMES",
            "++sSwimming.entryTapFrames >= AIR_ENTRY_SWIM_STROKE_TAP_FRAMES")}
    yield "fixes", "ignore_air_entry_stroke_on_plunge", "fixes.air_entry_stroke_buffer", {
        path: submerged.replace_in_function("act_water_plunge",
            "|| sSwimming.entryStrokeReady", "")}
    moving_path = "src/game/mario_actions_moving.c"
    for name, function, old, replacement in (
            ("snap_neutral_recovery", "coasting_ground_step", "update_decelerating_speed(m);", "mario_set_forward_vel(m, 0.0f);"),
            ("erase_belly_recovery_speed", "stomach_slide_action", "update_sliding(m, 0.0f);", "update_sliding(m, 4.0f);"),
            ("erase_dive_recovery_speed", "act_dive_slide", "update_sliding(m, 0.0f);", "update_sliding(m, 8.0f);"),
            ("stop_backward_slide_kick", "act_slide_kick_slide", "absf(m->forwardVel) < 1.0f", "m->forwardVel < 1.0f")):
        yield "fixes", name, "fixes.neutral_recovery_momentum", {
            moving_path: MutationSource(root / moving_path).replace_in_function(function, old, replacement)}
    step_path = "src/game/mario_step.c"
    yield "fixes", "restore_standard_thicc_fall_cap", "fixes.character_fall_speed", {
        step_path: MutationSource(root / step_path).replace_in_function("mario_max_fall_speed",
            "HE_THICC_MAX_FALL_SPEED", "MARIO_MAX_FALL_SPEED")}
    air_path = "src/game/mario_actions_airborne.c"
    yield "fixes", "defer_rollout_contact_inputs", "fixes.rollout_recovery_inputs", {
        air_path: MutationSource(root / air_path).replace_in_function("finish_rollout_landing",
            "act_freefall_land_stop(m);", "(void) m;")}
    yield "fixes", "reuse_rollout_initiating_press", "fixes.rollout_recovery_inputs", {
        air_path: MutationSource(root / air_path).replace_in_function("finish_rollout_landing",
            "if (starting)", "if (FALSE && starting)")}
    yield "fixes", "double_rollout_contact_movement", "fixes.rollout_recovery_momentum", {
        air_path: MutationSource(root / air_path).replace_in_function("finish_rollout_landing",
            "act_freefall_land_stop(m);", "act_freefall_land_stop(m); perform_ground_step(m);")}
    air_path = "src/game/mario_actions_airborne.c"
    yield "fixes", "lose_rollout_first_dive_press", "fixes.belly_slide_input_chain", {
        air_path: MutationSource(root / air_path).replace_in_function("finish_rollout_landing",
            "m->actionArg |= ACT_ARG_DIVE_FROM_ROLLOUT_CONTACT;", "(void) m;")}
    moving_path = "src/game/mario_actions_moving.c"
    yield "fixes", "lock_belly_slide_startup", "fixes.belly_slide_input_chain", {
        moving_path: MutationSource(root / moving_path).replace_in_function("stomach_slide_action",
            "&& can_rollout_from_belly_slide(m)", "&& m->actionTimer >= 5 && can_rollout_from_belly_slide(m)")}
    for label, condition in (
            ("allow_uphill_belly_rollout", "downhillTravel > 0.0f"),
            ("allow_backward_belly_rollout", "forwardTravel > 0.0f"),
            ("launch_belly_rollout_facing_uphill", "downhillFacing > 0.0f")):
        yield "fixes", label, "fixes.belly_slide_slope_rollout", {
            moving_path: MutationSource(root / moving_path).replace_in_function("can_rollout_from_belly_slide",
                condition, "((void) " + condition.split()[0] + ", TRUE)")}
    object_path = "src/game/mario_actions_object.c"
    yield "fixes", "lock_belly_slide_standup", "fixes.belly_slide_input_chain", {
        object_path: MutationSource(root / object_path).replace_in_function("act_stomach_slide_stop",
            "if (check_common_landing_cancels(m, 0))", "if (FALSE)")}
    mario_path = "src/game/mario.c"
    yield "fixes", "lose_prebrake_impact_speed", "fixes.jump_kick_impact_slide_sequence", {
        "src/game/mario_actions_airborne.c": MutationSource(root / "src/game/mario_actions_airborne.c").replace_in_function(
            "mario_restrict_air_uphill", "m->slopeJumpKickImpactSpeed = m->forwardVel;",
            "m->slopeJumpKickImpactSpeed = 0.0f;")}
    yield "fixes", "leak_impact_speed_after_landing", "fixes.jump_kick_impact_slide_sequence", {
        mario_path: MutationSource(root / mario_path).replace_in_function('reset_mario_action_state',
            "m->slopeJumpKickImpactSpeed = 0.0f;", "(void)0;")}
    for label, response in (("flip_first_slope_attempt", "-slideEntrySpeed"),
                            ("never_flip_repeated_slope_attempt", "0.0f")):
        yield "fixes", label, "fixes.jump_kick_impact_slide_sequence", {
            mario_path: MutationSource(root / mario_path).replace_in_function("set_mario_action_moving",
                "m->backwardSlopeSlideCount == 0 ? 0.0f\n"
                "                                 : m->backwardSlopeSlideCount == 1 ? 10.0f\n"
                "                                 : MAX(-slideEntrySpeed, 20.0f)", response)}
    yield "fixes", "omit_third_slope_minimum", "fixes.jump_kick_impact_slide_sequence", {
        mario_path: MutationSource(root / mario_path).replace_in_function("set_mario_action_moving",
            "MAX(-slideEntrySpeed, 20.0f)", "-slideEntrySpeed")}
    yield "fixes", "keep_fast_slope_butt_slide_entry", "fixes.slope_butt_slide_entry", {
        mario_path: MutationSource(root / mario_path).replace_in_function("set_mario_action_moving",
            "slideEntrySpeed < -16.0f", "FALSE")}
    yield "fixes", "clear_exact_sixteen_slope_entry", "fixes.slope_butt_slide_entry", {
        mario_path: MutationSource(root / mario_path).replace_in_function("set_mario_action_moving",
            "slideEntrySpeed < -16.0f", "slideEntrySpeed <= -16.0f")}
