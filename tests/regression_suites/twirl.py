"""Production extraction and deliberate defects for the twirl suite."""
from .common import prepare_common_constants
from .mutation_support import MutationSource

def prepare_twirl(root, source, definitions, functions):
    prepare_common_constants(source, definitions, gameplay_options=True, cap_functions=functions)
    functions += [source("src/game/mario.c").function(name) for name in
                  ("mario_keeps_shell", "mario_blocks_harmful_knockback")]
    definitions += ["void mario_ground_twirl_init(void);"]
    functions += [source("src/game/mario_ground_twirl.c").function("mario_ground_twirl_reset_rebound_state")]
    functions += [source("src/game/mario_ground_twirl.c").function("mario_twirl_advance_contact_generation")]
    functions += [source("src/game/mario_ground_twirl.c").function("mario_ground_twirl_init")]
    functions += [source("src/game/mario_air_twirl.c").function("mario_air_twirl_is_active")]
    definitions += [source("src/game/mario_he_thicc.h").macros(
        "HE_THICC_.*|ACT_ARG_WALL_KICK_HE_THICC|ACT_ARG_FORWARD_ROLLOUT_HE_THICC_BOUNCE")]
    definitions += [source("src/game/mario_actions_airborne.h").macros(
        "ACT_ARG_JUMP_BACKWARD_BUTT_SLIDE.*|ACT_ARG_JUMP_KICK_FOLLOWUP_QUEUED|ACT_ARG_JUMP_KICK_BACKWARD_BUTT_SLIDE.*|ACT_ARG_STEEP_JUMP_BACKWARD_BUTT_SLIDE_NO_UPHILL|ACT_ARG_DIVE_FROM_ROLLOUT_CONTACT")]
    definitions += [source("src/game/mario_actions_stationary.h").macros(
        "ACT_ARG_COUGHING_SHALLOW_WATER")]
    definitions += [source("include/config/config_movement.h").macros("BACKWARD_BUTT_SLIDE_JUMP_AIR_DRAG|BACKWARD_JUMP_KICK_UPHILL_DECEL")]
    definitions += [source("src/game/mario_character.h").declaration("enum", "MarioPalette")]
    definitions += [source("src/game/camera.h").declaration("enum", "CameraShake")]
    definitions += [source("include/object_constants.h").declaration("enum", "HeldState")]
    sm64 = source("include/sm64.h")
    definitions += [sm64.declaration("enum", name) for name in (
        "MarioInput", "MarioFlags", "MarioActionFlags", "MarioActionGroups", "GroundStep")]
    definitions += [sm64.macros("ACT_.*")]
    definitions += [source("include/mario_animation_ids.h").declaration("enum", "MarioAnimID")]
    interaction = source("src/game/interaction.c")
    definitions += [source("include/object_constants.h").declaration("enum", "oActionsBowser")]
    functions += [source("src/game/mario_punch.c").function("mario_is_first_punch")]
    functions += [interaction.function(name) for name in (
        "mario_bowser_tail_grab_action", "mario_can_grab_bowser_tail", "mario_overlaps_grab_hitbox",
        "mario_get_bowser_tail_contact")]
    definitions += [source("src/game/interaction.h").declaration("enum", name) for name in (
        "ObjInteractTypes", "Interactions", "InteractSubtypes", "InteractStatus", "AttackType")]
    definitions += [source("src/game/mario_punch.h").declaration("enum", "ActionArgsPunchSequence")]
    definitions += [interaction.declaration("enum", "DefensiveContactResult")]
    definitions += [interaction.macros("GROUND_TWIRL_.*|DIVE_HIT_PHASE|BULLY_CONTACT_HIT_PHASE|MAX_PUNCH_HIT_OBJECTS")]
    definitions += [source("src/game/mario_actions_moving.h").macros("GROUND_TWIRL_IMMUNE_REBOUND_STATE")]
    definitions += [source("include/object_constants.h").macros("GOOMBA_SIZE_.*|KOOPA_BP_.*|KOOPA_THE_QUICK_.*INDEX")]
    definitions += [source("include/object_constants.h").macros(
        "MR_I_PIRANHA_PARTICLE_ACT_.*|MR_BLIZZARD_SNOWBALL_ACT_.*|MONTY_MOLE_ROCK_ACT_.*|"
        "FLYING_BOOKEND_ACT_.*|WATER_BOMB_ACT_.*|BULLET_BILL_ACT_.*")]
    functions += [source("src/game/interaction.c").function("mario_ground_twirl_destroy_projectile")]
    twirl = source("src/game/mario_ground_twirl.c")
    definitions += [twirl.macros("GROUND_TWIRL_.*|HE_THICC_GROUND_POUND_.*")]
    functions += [interaction.function(name) for name in (
        "mario_set_punch_hit_phase", "he_thicc_attack_contact", "mario_is_breakdancing",
        "mario_breakdance_protects_from_damage",
        "punch_hit_object", "bully_attack_hit_phase", "clear_separated_bully_hits",
        "update_attack_hit_phase", "mario_ground_twirl_takes_hazard_damage", "damage_came_from_enemy",
        "mario_ground_twirl_contact_can_damage", "mario_ground_twirl_rebounds_from_object",
        "mario_tick_contact_immunities",
        "mario_start_heave_ho_recovery_immunity",
        "mario_is_knocked_back", "mario_ground_twirl_reset_contact_damage",
        "mario_ground_twirl_damage_capless_contact", "mario_ground_twirl_rebound_touches_enemy",
        "mario_rebound_from_twirl_object", "mario_ground_twirl_rebound_from_stun",
        "mario_ground_twirl_rebound_is_playing", "update_ground_twirl_rebound_invulnerability",
        "mario_vanish_ignores_hostile_contact", "mario_ground_twirl_outside_enemy_body",
        "mario_ground_twirl_reset_rebound_invulnerability",
        "mario_clear_projectile_contacts", "mario_air_twirl_recoil", "mario_air_twirl_rebound_from_stun",
        "mario_handle_air_twirl_contact", "mario_handle_defensive_contact", "mario_process_interactions")]
    functions += [twirl.function("mario_ground_twirl_finish").replace(
        "static void mario_ground_twirl_finish(", "static void real_mario_ground_twirl_finish(")]
    functions += [twirl.function(name) for name in (
        "mario_twirl_can_stun_enemy", "mario_ground_pound_impact",
        "mario_twirl_begin_contacts", "mario_start_ground_twirl",
        "mario_ground_twirl_is_active", "mario_ground_twirl_is_held_pose",
        "mario_ground_twirl_cancel_frames",
        "mario_ground_twirl_visual_frames",
        "mario_ground_twirl_update_cooldown", "mario_try_ground_twirl",
        "mario_twirl_push_enemies", "mario_ground_twirl_action")]

def mutations_twirl(root):
    path = "src/game/interaction.c"
    interaction = MutationSource(root / path)
    yield "twirl", "blast_rejected_contact", "twirl.thicc_attack_contact", {
        path: interaction.replace_in_function("mario_process_interactions",
            "if (object->oInteractStatus & INT_STATUS_WAS_ATTACKED) {", "if (TRUE) {")}
    yield "twirl", "reset_air_kick_blast_each_frame", "twirl.thicc_attack_contact", {
        path: interaction.replace_in_function("mario_set_punch_hit_phase",
            "if (phase != sPunchContacts.phase)", "if (TRUE)")}
    yield "twirl", "damage_displaced_enemy_overlap", "twirl.nondamaging_contacts", {
        path: interaction.replace_in_function("mario_handle_defensive_contact",
            "if (m->action == ACT_GROUND_TWIRL && isEnemy && displaced) {",
            "if (m->action == ACT_GROUND_TWIRL && isEnemy && displaced) {\n        m->hurtCounter += 4;")}
    unconditional_contact = interaction.replace_in_function("mario_process_interactions",
        "m->action != actionBeforeInteraction && mario_is_knocked_back(m->action)", "TRUE")
    yield "twirl", "ignore_native_knockback_requirement", "twirl.native_knockback", {
        path: MutationSource(root / path, unconditional_contact).replace_in_function(
            "mario_ground_twirl_damage_capless_contact",
            "mario_is_knocked_back(m->action)\n        && ", "")}
    yield "twirl", "block_bully_damage_after_grace_setup", "twirl.native_knockback", {
        path: interaction.replace_in_function("mario_ground_twirl_damage_capless_contact",
            "&& m->health >= 0x100", "&& m->health >= 0x100 && m->invincTimer == 0")}
    yield "twirl", "carry_running_speed_into_stun_recoil", "twirl.stun_recoil", {
        path: interaction.replace_in_function("mario_ground_twirl_rebound_from_stun",
            "obj, 16.0f", "obj, MAX(16.0f, m->forwardVel)")}
    twirl_path = "src/game/mario_ground_twirl.c"
    yield "twirl", "block_twirl_on_slide", "twirl.startup_priority", {
        twirl_path: MutationSource(root / twirl_path).replace_in_function("mario_try_ground_twirl",
            "INPUT_OFF_FLOOR | INPUT_STOMPED", "INPUT_OFF_FLOOR | INPUT_ABOVE_SLIDE | INPUT_STOMPED")}
    yield "twirl", "continue_twirl_after_recoil", "twirl.stun_recoil", {
        twirl_path: MutationSource(root / twirl_path).replace_in_function("mario_ground_twirl_action",
            "if (m->action != ACT_GROUND_TWIRL)", "if (FALSE)")}
    yield "twirl", "interrupt_enemy_targeting_before_recoil", "twirl.stun_recoil", {
        twirl_path: MutationSource(root / twirl_path).replace_in_function("mario_twirl_push_enemies",
            "mario_launch_stunned_enemy(m, obj, marioSpeed, sGroundTwirl.id);",
            "mario_launch_stunned_enemy(m, obj, marioSpeed, sGroundTwirl.id);\n            mario_ground_twirl_rebound_from_stun(m, obj);\n            return;")}
