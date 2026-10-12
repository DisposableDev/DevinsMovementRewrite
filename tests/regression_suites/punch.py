"""Production extraction and deliberate defects for the punch suite."""
from .common import prepare_common_constants
from regression_support import HarnessError, animation_metadata
from .mutation_support import MutationSource

def prepare_punch(root, source, definitions, functions):
    prepare_common_constants(source, definitions, gameplay_options=True, cap_functions=functions)
    functions += [source("src/game/mario.c").function(name) for name in (
        "mario_move_buffer_family", "mario_is_buffered_landing", "mario_is_knockback_recovery")]
    functions += ["void mario_air_twirl_transition(struct MarioState *m, u32 action) {}"]
    definitions += [source("src/game/mario_he_thicc.h").macros(
        "HE_THICC_.*|ACT_ARG_WALL_KICK_HE_THICC|ACT_ARG_FORWARD_ROLLOUT_HE_THICC_BOUNCE")]
    definitions += [source("src/game/mario_actions_airborne.h").macros(
        "ACT_ARG_JUMP_BACKWARD_BUTT_SLIDE.*|ACT_ARG_JUMP_KICK_FOLLOWUP_QUEUED|ACT_ARG_JUMP_KICK_BACKWARD_BUTT_SLIDE.*|ACT_ARG_STEEP_JUMP_BACKWARD_BUTT_SLIDE_NO_UPHILL|ACT_ARG_DIVE_FROM_ROLLOUT_CONTACT")]
    definitions += [source("src/game/mario_actions_stationary.h").macros(
        "ACT_ARG_COUGHING_SHALLOW_WATER")]
    definitions += [source("include/config/config_movement.h").macros("BACKWARD_BUTT_SLIDE_JUMP_AIR_DRAG|BACKWARD_JUMP_KICK_UPHILL_DECEL")]
    definitions += [source("src/game/mario_actions_moving.h").macros("ACT_ARG_WALKING_TURN_FROM_PUNCH")]
    definitions += [source("src/game/interaction.h").declaration("enum", "Interactions")]
    definitions += [source("include/types.h").declaration("enum", "AnimFlags")]
    functions += [source("src/engine/graph_node.c").function("geo_update_animation_frame"),
                  source("src/game/mario.c").function("set_anim_to_frame")]
    definitions += [source("include/model_ids.h").macros("MODEL_MARIOS_.*CAP")]
    definitions += [source("include/n64/PR/os_cont.h").macros("CONT_L|CONT_G|L_TRIG|Z_TRIG")]
    sm64 = source("include/sm64.h")
    header = source("src/game/mario_punch.h")
    definitions += [sm64.declaration("enum", name) for name in (
        "MarioInput", "GroundStep", "MarioParticleFlags", "MarioFlags",
        "MarioActionGroups", "MarioActionFlags")]
    definitions += [sm64.macros("ACT_.*")]
    definitions += [source("include/config/config_movement.h").macros(
        "MARIO_STRONG_STICK_THRESHOLD|CRAWL_TARGET_SPEED|WATER_ENTRY_.*_DIVISOR")]
    definitions += [header.declaration("enum", name) for name in (
        "ActionStatesPunchSequence", "ActionArgsPunchSequence")]
    definitions += [source("src/game/mario_actions_submerged.h").declaration("enum", name)
                    for name in ("ActionArgsWaterDownBurst", "ActionStatesWaterPunch")]
    definitions += [source("include/mario_animation_ids.h").declaration("enum", "MarioAnimID")]
    animations = []
    for number in ("66", "67", "68", "69", "6A", "71", "8B", "96", "97", "9A", "9B"):
        values = animation_metadata(source(f"assets/anims/anim_{number}.inc.c"))
        animations.append(f"[0x{number}] = {{{', '.join(map(str, values))}}},")
    definitions.append("static struct Animation sAnimations[256] = {\n" + "\n".join(animations) + "\n};")
    crouch_asset = source("assets/anims/anim_97.inc.c")
    definitions += [crouch_asset.region(r"^static const u16 anim_97_indices", r"\Z")]
    punch = source("src/game/mario_punch.c")
    obj = source("src/game/mario_actions_object.c")
    definitions += [obj.macros("DIVE_PICKUP_CANCEL_EARLY_FRAMES")]
    mario = source("src/game/mario.c")
    functions += [mario.function(name) for name in ("mario_clear_cap_flags", "mario_natural_cap_powers")]
    interactions = source("src/game/interaction.c")
    functions += [interactions.region(r"^#define MAX_PUNCH_HIT_OBJECTS\b",
                                     r"^static struct Object \*sKnockbackSourceObj;")]
    functions += [interactions.function(name) for name in (
        "mario_set_punch_hit_phase", "he_thicc_attack_contact", "punch_hit_object", "remember_punch_hit_object")]
    functions += [mario.function(name) for name in ("is_anim_at_end", "is_anim_past_end")]
    functions += [source("src/engine/math_util.c").function("approach_s16")]
    functions += [mario.function(name) for name in (
        "mario_has_land_movement_underwater", "mario_is_low_land_pose",
        "mario_is_head_underwater", "set_water_plunge_action")]
    functions += [obj.function("check_common_object_cancels")]
    submerged = source("src/game/mario_actions_submerged.c")
    functions += [submerged.function(name) for name in (
        "is_normal_swimming_action", "can_control_swimming", "transition_cap_swimming_to_land")]
    functions += [source("src/game/mario_step.c").function("mario_is_on_floor")]
    start = punch.mask.find("s8 sPunchingForwardVelocities")
    if start < 0:
        raise HarnessError("Punch declarations moved; update the test adapter")
    functions += [source("src/game/mario_actions_stationary.c").function("check_common_hold_idle_cancels")]
    functions += [punch.mapped(start, len(punch.text)), obj.function("act_punching"),
                  obj.function("act_dive_picking_up"),
                  mario.function("prepare_ground_takeoff"), mario.function("reset_mario_action_state"),
                  mario.function("set_mario_action")]
    functions += [source("src/game/mario_actions_stationary.c").function("check_common_idle_cancels")]
    functions += [source("src/game/mario_actions_stationary.c").function(name) for name in (
        "check_common_stationary_cancels", "start_stationary_crouch_attack", "act_start_crouching", "act_stop_crouching", "act_crouching", "act_start_crawling", "act_stop_crawling")]
    functions += [source("src/game/behaviors/cap.inc.c").function("bhv_released_power_up_cap_loop")]
    moving = source("src/game/mario_actions_moving.c")
    definitions += [moving.macros("analog_stick_held_back")]
    definitions += [moving.macros("GROUND_SLIDE_SPEED_CAP"),
                    source("include/surface_terrains.h").declaration("enum", "SurfaceClass"),
                    source("include/surface_terrains.h").declaration("enum", "SurfaceTypes")]
    functions += [moving.function("update_sliding_angle"), moving.function("update_sliding").replace(
        "s32 update_sliding(", "s32 real_update_sliding(")]
    functions += [moving.function(name) for name in (
        "check_common_moving_cancels", "act_move_punching", "act_crawling", "start_moving_crouch_attack",
        "common_slide_action_with_jump", "act_crouch_slide", "act_butt_slide")]
    functions += [source("src/game/mario_actions_airborne.c").function("mario_has_dive_input")]
    definitions += [source("include/object_constants.h").declaration("enum", name)
                    for name in ("oActionsBowser", "HeldState")]
    functions += [interactions.function("mario_bowser_tail_grab_action"),
                  interactions.function("mario_can_grab_bowser_tail"),
                  interactions.function("able_to_grab_object")]

def mutations_punch(root):
    yield "punch", "original_caps_keep_land_actions", "punch.standing.cap_presets_underwater", {
        "src/game/mario.c": MutationSource(root / "src/game/mario.c").replace_in_function(
            "mario_has_land_movement_underwater",
            "mario_has_current_metal_cap(m) || mario_has_current_vanish_cap(m)",
            "(m->flags & (MARIO_METAL_CAP | MARIO_VANISH_CAP)) != 0")}

    path = "src/game/interaction.c"
    for name, old, new in (
        ("punch_explosion_every_frame", "!sPunchContacts.attackImpactDone", "TRUE"),
        ("punch_explosion_missing", "mario_he_thicc_attack_impact(m);", "(void) mario_he_thicc_attack_impact;"),
    ):
        yield "punch", name, "punch.standing.punch_explosion_windows", {
            path: MutationSource(root / path).replace_in_function("he_thicc_attack_contact", old, new)}

    path = "src/game/mario_punch.c"
    yield "punch", "blast_missed_punch", "punch.standing.punch_explosion_windows", {
        path: MutationSource(root / path).replace_in_function("mario_update_punch_sequence",
            "    return FALSE;\n}",
            "    if (m->flags & MARIO_PUNCHING) { mario_he_thicc_attack_impact(m); }\n"
            "    return FALSE;\n}")}

    moving_path = "src/game/mario_actions_moving.c"
    moving = MutationSource(root / moving_path)
    yield "punch", "ignore_crouch_slide_z_release", "punch.standing.crouch_slide_cancel", {
        moving_path: moving.replace_in_function("act_crouch_slide",
            "if (!(m->input & INPUT_Z_DOWN)) {", "if (FALSE) {")}
    yield "punch", "cancel_crouch_slide_while_z_held", "punch.standing.crouch_slide_cancel", {
        moving_path: moving.replace_in_function("act_crouch_slide",
            "if (!(m->input & INPUT_Z_DOWN)) {", "if (TRUE) {")}
    yield "punch", "restore_manual_butt_slide", "punch.standing.crouch_slide_a", {
        moving_path: moving.replace_in_function("act_crouch_slide",
            "return set_jumping_action(m, jumpAction, 0);",
            "(void) jumpAction; return set_mario_action(m, ACT_BUTT_SLIDE, 0);")}
    yield "punch", "delay_crouch_slide_backflip", "punch.standing.crouch_slide_backflip_buffer", {
        moving_path: moving.replace_in_function("act_crouch_slide",
            "return set_jumping_action(m, ACT_BACKFLIP, 0);",
            "if (m->crouchSlideFrames < 2) { return FALSE; } "
            "return set_jumping_action(m, ACT_BACKFLIP, 0);")}
    yield "punch", "lose_crouch_slide_backflip", "punch.standing.crouch_slide_a", {
        moving_path: moving.replace_in_function("act_crouch_slide",
            "return set_jumping_action(m, ACT_BACKFLIP, 0);",
            "return set_mario_action(m, ACT_BUTT_SLIDE, 0);")}
    stationary_path = "src/game/mario_actions_stationary.c"
    stationary = MutationSource(root / stationary_path)
    yield "punch", "skip_short_crouch_entry", "punch.standing.crouch_start_animation", {
        moving_path: moving.replace_in_function("act_crouch_slide",
            "is_anim_at_end(m) ? ACT_CROUCHING : ACT_START_CROUCHING", "ACT_CROUCHING")}
    for path, src, function in ((stationary_path, stationary, "act_start_crouching"),
                                (moving_path, moving, "act_crouch_slide")):
        yield "punch", "stutter_" + function, "punch.standing.crouch_lowering_playback", {
            path: src.replace_in_function(function,
                "set_mario_animation(m, MARIO_ANIM_START_CROUCHING);",
                "set_mario_anim_with_accel(m, MARIO_ANIM_START_CROUCHING, 0x8000);")}
        yield "punch", "truncate_" + function, "punch.standing.crouch_start_animation", {
            path: src.replace_in_function(function, "is_anim_at_end(m)", "is_anim_past_end(m)")}
    yield "punch", "skip_crouch_entry_on_stick", "punch.standing.crouch_start_animation", {
        stationary_path: stationary.replace_in_function("act_start_crouching",
            "if (coasting_ground_step(m) == GROUND_STEP_LEFT_GROUND)",
            "if (m->input & INPUT_NONZERO_ANALOG) return set_mario_action(m, ACT_START_CRAWLING, 0);\n"
            "    if (coasting_ground_step(m) == GROUND_STEP_LEFT_GROUND)")}
    yield "punch", "delay_uncrouch_redispatch", "punch.standing.crouch_entry_exit", {
        stationary_path: stationary.replace_in_function("act_stop_crouching",
            "return set_mario_action(m, m->forwardVel != 0.0f ? ACT_DECELERATING : ACT_IDLE, 0);",
            "set_mario_action(m, m->forwardVel != 0.0f ? ACT_DECELERATING : ACT_IDLE, 0); return FALSE;")}
    yield "punch", "restore_frame_six_uncrouch_window", "punch.standing.crouch_entry_exit", {
        stationary_path: stationary.replace_in_function("act_stop_crouching",
            "animInfo->curAnim->loopEnd - 7", "animInfo->curAnim->loopEnd - 5")}
    punch_path = "src/game/mario_punch.c"
    yield "punch", "forced_cap_release_ownership", "punch.standing.forced_cap_release", {
        punch_path: MutationSource(root / punch_path).replace_in_function(
            "release_breakdance_power_up_cap",
            "mario_natural_cap_powers(m)",
            "m->flags & MARIO_SPECIAL_CAPS")}

    punch = MutationSource(root / punch_path)
    yield "punch", "forget_pair_mode", "punch.standing.pairs", {punch_path: punch.replace_in_function(
        "mario_update_punch_sequence", "case ACT_ARG_PUNCH_SEQUENCE_YAH:",
        "case ACT_ARG_PUNCH_SEQUENCE_YAH:\n            mario_reset_punch_sequence();")}
    mario_path = "src/game/mario.c"
    mario = MutationSource(root / mario_path)
    yield "punch", "missing_action_reset", "punch.standing.interruptions", {mario_path: mario.replace_in_function(
        'reset_mario_action_state', "mario_reset_punch_sequence();", "(void)0;")}
    yield "punch", "skip_breakdance_cap_release", "punch.standing.breakdance_cap_release", {
        punch_path: punch.replace_in_function('update_breakdance',
            "release_breakdance_power_up_cap(m);", "(void)release_breakdance_power_up_cap;")}
    object_path = "src/game/mario_actions_object.c"
    obj = MutationSource(root / object_path)
    yield "punch", "loop_vanish_underwater_punch", "punch.standing.breakdance_cap_underwater", {
        object_path: obj.replace_in_function("check_common_object_cancels",
            "m->pos[1] < waterSurface && !mario_has_land_movement_underwater(m)",
            "m->pos[1] < waterSurface")}
    yield "punch", "delay_underwater_cap_release_swimming", "punch.moving.breakdance_cap_underwater", {
        punch_path: punch.replace_in_function('update_breakdance',
            "m->pos[1] < m->waterLevel - 100 && !mario_has_land_movement_underwater(m)", "FALSE")}
    yield "punch", "remove_attack_steering", "punch.standing.attack_steering", {
        punch_path: punch.replace_in_function("steer_ground_attack",
            "s16 maxTurn = PUNCH_MAX_TURN;", "s16 maxTurn = 0;")}
    yield "punch", "snap_punch_recovery_facing", "punch.moving.recovery_turn", {
        punch_path: punch.replace_in_function("cancel_punch_recovery",
            "m->actionArg != ACT_ARG_PUNCH_SEQUENCE_BREAKDANCE || m->action == ACT_MOVE_PUNCHING",
            "m->actionArg == ACT_ARG_PUNCH_SEQUENCE_BREAKDANCE && m->action == ACT_MOVE_PUNCHING")}
    yield "punch", "lose_completed_punch_turn", "punch.moving.recovery_turn", {
        punch_path: punch.replace_in_function("finish_punch_sequence",
            "endAction == ACT_WALKING ? ACT_ARG_WALKING_TURN_FROM_PUNCH : 0", "0")}

