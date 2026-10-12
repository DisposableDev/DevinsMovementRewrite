"""Production extraction and deliberate defects for the bounce suite."""
from .common import prepare_common_constants
from .mutation_support import MutationSource

def prepare_bounce(root, source, definitions, functions):
    prepare_common_constants(source, definitions, gameplay_options=True, cap_functions=functions)
    definitions += [source("src/game/mario_he_thicc.h").macros(
        "HE_THICC_.*|ACT_ARG_WALL_KICK_HE_THICC|ACT_ARG_FORWARD_ROLLOUT_HE_THICC_BOUNCE")]
    definitions += [source("src/game/mario_actions_airborne.h").macros(
        "ACT_ARG_JUMP_BACKWARD_BUTT_SLIDE.*|ACT_ARG_JUMP_KICK_FOLLOWUP_QUEUED|ACT_ARG_JUMP_KICK_BACKWARD_BUTT_SLIDE.*|ACT_ARG_STEEP_JUMP_BACKWARD_BUTT_SLIDE_NO_UPHILL|ACT_ARG_DIVE_FROM_ROLLOUT_CONTACT")]
    definitions += [source("src/game/mario_actions_stationary.h").macros(
        "ACT_ARG_COUGHING_SHALLOW_WATER")]
    definitions += [source("include/config/config_movement.h").macros("BACKWARD_BUTT_SLIDE_JUMP_AIR_DRAG|BACKWARD_JUMP_KICK_UPHILL_DECEL")]
    definitions += [source("include/object_constants.h").macros("GOOMBA_SIZE_.*|GOOMBA_ACT_.*")]
    definitions += [source("include/config/config_movement.h").macros("WATER_DOWN_.*")]
    definitions += [source("include/object_constants.h").declaration("enum", name) for name in (
        "oActionsExclamationBox", "ExclamationBoxContentsList")]
    definitions += [source("src/game/mario_actions_submerged.h").declaration(
        "enum", "ActionStatesWaterDownBurst"),
        source("src/game/mario_actions_submerged.h").declaration(
            "enum", "ActionArgsWaterDownBurst"),
        source("src/game/mario_actions_submerged.h").declaration(
            "enum", "ActionArgsFlutterKick"),
        source("src/game/mario_actions_submerged.h").declaration(
            "enum", "ActionArgsWaterActionEnd"),
        source("src/game/mario_actions_submerged.h").declaration(
            "enum", "ActionStatesWaterPunch")]
    sm64 = source("include/sm64.h")
    definitions += [sm64.declaration("enum", name) for name in (
        "MarioInput", "MarioFlags", "MarioActionGroups", "MarioActionFlags",
        "MarioParticleFlags", "AirStep", "WaterStep")]
    definitions += [sm64.macros("ACT_.*")]
    interactions = source("src/game/interaction.c")
    header = source("src/game/interaction.h")
    definitions += [header.declaration("enum", name) for name in (
        "ObjInteractTypes", "Interactions", "InteractSubtypes", "InteractStatus", "AttackType")]
    definitions += [source("src/game/mario_actions_airborne.h").macros(
        "SLIDE_KICK_STARTUP_VULNERABLE_FRAMES|DIVE_STARTUP_VULNERABLE_FRAMES")]
    definitions += [source("src/game/mario_actions_moving.h").macros("DIVE_SLIDE_ATTACK_FRAMES")]
    definitions += [source("include/config/config_movement.h").macros("Z_TWIRL")]
    definitions += [source("src/game/mario_actions_airborne.h").declaration("enum", "ActionStatesSlideKick")]
    definitions += [source("src/game/mario_punch.h").declaration("enum", "ActionArgsPunchSequence")]
    definitions += [source("include/mario_animation_ids.h").declaration("enum", "MarioAnimID")]
    functions += [interactions.region(r"^#define MAX_PUNCH_HIT_OBJECTS\b",
                                     r"^static struct Object \*sKnockbackSourceObj;")]
    functions += [interactions.function(name) for name in (
        "mario_set_punch_hit_phase", "bully_attack_hit_phase", "clear_separated_bully_hits",
        "update_attack_hit_phase", "punch_hit_object",
        "remember_punch_hit_object", "mario_forget_attack_object")]
    functions += [interactions.function(name) for name in (
        "attack_object", "determine_interaction", "dive_hit_target", "determine_enemy_interaction",
        "remember_enemy_dive_hit",
        "bounce_off_object", "bounce_off_enemy",
        "jump_after_ground_pound_enemy_hit", "interact_hit_from_below", "interact_bounce_top",
        "he_thicc_attack_contact", "mario_attack_cap_box_surface", "interact_breakable")]
    functions += [source("src/game/object_helpers.c").function(name) for name in (
        "mario_has_ground_pound_impact", "cur_obj_is_mario_ground_pounding_platform",
        "cur_obj_was_attacked_or_ground_pounded")]
    functions += [source("src/game/mario.c").function(name) for name in (
        "play_sound_and_spawn_particles", "play_mario_heavy_landing_sound")]
    functions += [source("src/game/mario_actions_submerged.c").function(name) for name in (
        "update_water_down_repeat_buffer", "finish_water_down_burst", "update_water_down_burst_particles",
        "act_water_down_burst", "water_ground_pound_landing")]
    functions += [source("src/game/mario_actions_airborne.c").function("act_twirling")]
    functions += [source("src/game/behaviors/goomba.inc.c").function("huge_goomba_weakly_attacked")]
    functions += [source("src/game/behaviors/exclamation_box.inc.c").function(name) for name in (
        "exclamation_box_act_active", "exclamation_box_act_scaling",
        "exclamation_box_act_explode", "exclamation_box_act_wait_for_respawn")]

def mutations_bounce(root):
    path = "src/game/mario_actions_submerged.c"
    yield "bounce", "underwater_surface_splash", "bounce.underwater_ground_pound_impact", {
        path: MutationSource(root / path).replace_in_function("finish_water_down_burst",
            "u32 sound = SOUND_ACTION_TERRAIN_HEAVY_LANDING + m->terrainSoundAddend;\n"
            "        if (m->flags & MARIO_METAL_CAP) {\n"
            "            sound = SOUND_ACTION_METAL_HEAVY_LANDING;\n"
            "        }\n"
            "        play_sound(sound, m->marioObj->header.gfx.cameraToObject);",
            "play_mario_heavy_landing_sound(m, SOUND_ACTION_TERRAIN_HEAVY_LANDING);")}
    path = "src/game/interaction.c"
    yield "bounce", "repeat_huge_goomba_dive", "bounce.huge_goomba_dive_once", {
        path: MutationSource(root / path).replace_in_function("determine_enemy_interaction",
            "&& punch_hit_object(target)", "&& FALSE")}
    yield "bounce", "split_segment_dive_records", "bounce.segmented_enemy_dive_once", {
        path: MutationSource(root / path).replace_in_function("dive_hit_target",
            "return obj->parentObj != NULL ? obj->parentObj : obj;", "return obj;")}
    yield "bounce", "repeat_segmented_dive", "bounce.segmented_enemy_dive_once", {
        path: MutationSource(root / path).replace_in_function("determine_enemy_interaction",
            "&& punch_hit_object(target)", "&& FALSE")}
    yield "bounce", "forget_dive_hit_on_landing", "bounce.huge_goomba_dive_once", {
        path: MutationSource(root / path).replace_in_function("update_attack_hit_phase",
            "m->action == ACT_DIVE || m->action == ACT_DIVE_SLIDE",
            "m->action == ACT_DIVE")}
    yield "bounce", "repeat_breakdance_contact", "bounce.breakdance_contacts", {
        path: MutationSource(root / path).replace_in_function("determine_interaction",
            "ACT_ARG_PUNCH_SEQUENCE_BREAKDANCE && interaction == INT_TRIP",
            "ACT_ARG_PUNCH_SEQUENCE_BREAKDANCE && interaction == INT_KICK")}
    path = "src/game/interaction.c"
    yield "bounce", "replace_twirl_with_jump", "bounce.ground_pound_twirl", {
        path: MutationSource(root / path).replace_in_function("jump_after_ground_pound_enemy_hit",
            "drop_and_set_mario_action(m, ACT_TWIRLING, 0);",
            "drop_and_set_mario_action(m, ACT_JUMP, 0);")}
