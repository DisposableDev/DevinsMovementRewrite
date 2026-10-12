"""Production extraction and deliberate defects for the metal suite."""
from .common import prepare_common_constants
import re
from regression_support import HarnessError, animation_metadata
from .mutation_support import MutationSource

def prepare_metal(root, source, definitions, functions):
    prepare_common_constants(source, definitions, gameplay_options=True, cap_functions=functions)
    functions += [source("src/game/mario.c").function(name) for name in
                  ("mario_keeps_shell", "mario_blocks_harmful_knockback")]
    definitions += ["s32 hurt_and_set_mario_action(struct MarioState *, u32, u32, s16);"]
    functions += [source("src/game/mario.c").function("mario_has_cap_for_damage")]
    functions += [source("src/game/mario_air_twirl.c").function("mario_air_twirl_is_active")]
    functions += [
        "void object_platform_motion_before_step(struct Object *obj) {}",
        "s32 object_platform_motion_supported(struct Object *obj) { "
        "return obj->oPosY <= obj->oFloorHeight + 4.0f && obj->oVelY <= 0.0f; }",
    ]
    functions += ["f32 object_platform_motion_floor_rise(struct Object *obj) { return 0.0f; }"]
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
    definitions += [source("src/game/mario_character.h").declaration("enum", "MarioPalette")]
    definitions += [source("src/game/mario_actions_airborne.h").macros(
        "ACT_ARG_JUMP_BACKWARD_BUTT_SLIDE.*|ACT_ARG_JUMP_KICK_FOLLOWUP_QUEUED|ACT_ARG_JUMP_KICK_BACKWARD_BUTT_SLIDE.*|ACT_ARG_STEEP_JUMP_BACKWARD_BUTT_SLIDE_NO_UPHILL|ACT_ARG_DIVE_FROM_ROLLOUT_CONTACT")]
    definitions += [source("src/game/mario_actions_stationary.h").macros(
        "ACT_ARG_COUGHING_SHALLOW_WATER")]
    definitions += [source("include/config/config_movement.h").macros("BACKWARD_BUTT_SLIDE_JUMP_AIR_DRAG|BACKWARD_JUMP_KICK_UPHILL_DECEL")]
    definitions += [source("src/game/mario_actions_airborne.h").declaration("enum", "ActionArgsLavaBoost")]
    definitions += [source("src/game/mario_punch.h").declaration("enum", "ActionArgsPunchSequence")]
    breakdance_length = animation_metadata(source("assets/anims/anim_71.inc.c"))[3]
    definitions += [f"#define NATIVE_BREAKDANCE_FRAMES {breakdance_length}\n"]
    functions.remove("void mario_he_thicc_slide_kick_bounce(struct MarioState *m) {}")
    functions += ["static s32 he_thicc_is_active(void) { return mario_character_palette() == MARIO_PALETTE_HE_THICC; }"]
    functions += ["static void mario_he_thicc_clap(struct MarioState *m, f32 strength) {}"]
    functions += [source("src/game/mario_he_thicc.c").function("mario_he_thicc_slide_kick_bounce")]

    definitions += [source("include/n64/PR/os_cont.h").macros("A_BUTTON|CONT_A")]
    sm64 = source("include/sm64.h")
    constants = source("include/object_constants.h")
    interactions = source("src/game/interaction.h")
    definitions += [sm64.declaration("enum", name) for name in (
        "MarioInput", "MarioFlags", "MarioActionFlags", "MarioActionGroups", "MarioParticleFlags", "AirStep", "AirStepCheck")]
    definitions += [sm64.macros("ACT_.*|COS25|COS30")]
    definitions += [constants.declaration("enum", name) for name in (
        "ObjActiveFlags", "ObjMoveFlags", "ObjFlags", "HeldState", "oActionsBobOmb", "oActionsBowserBomb", "oActionsBowser", "oActionsBowserTail", "oBowserStatuses", "animIDsBowser", "RespawnInfoType")]
    definitions += [constants.macros("BOWSER_.*|OBJ_MOVE_MASK_.*|KOOPA_BP_.*|KOOPA_THE_QUICK_.*INDEX|"
        "GOOMBA_.*|KOOPA_UNSHELLED_ACT_.*|KLEPTO_ANIM_STATE_.*|MONTY_MOLE_ACT_.*|MONEYBAG_ACT_.*|BULLY_BP_SIZE_SMALL")]
    definitions += [source("src/game/camera.h").declaration("enum", "CameraShake")]
    definitions += [source("include/config/config_movement.h").macros("FALL_DAMAGE_HEIGHT_(SMALL|LARGE)")]
    definitions += [constants.declaration("enum", "ObjGeneralKnockbackActions"),
                    constants.declaration("enum", "ObjGeneralDeathActions"),
                    source("src/game/obj_behaviors.h").declaration("enum", "ObjCollisionFlags")]

    definitions += [interactions.declaration("enum", name) for name in (
        "ObjInteractTypes", "Interactions", "InteractSubtypes", "InteractStatus", "AttackType")]
    # Read actual Bowser behavior defaults, so a missing child-flame tag is
    # exercised through the production interaction handlers too.
    scripts = source("data/behavior_data.c")
    bowser_subtypes = []
    for name in ("bhvBowserBodyAnchor", "bhvBlueBowserFlame", "bhvFlameFloatingLanding",
                 "bhvBlueFlamesGroup", "bhvFlameBouncing", "bhvFlameMovingForwardGrowing",
                 "bhvFlameBowser", "bhvFlameLargeBurningOut", "bhvBowserBomb"):
        matches = re.findall(r"^const BehaviorScript " + name + r"\[\] = \{(.*?)^\};",
                             scripts.mask, re.M | re.S)
        if len(matches) != 1:
            raise HarnessError(f"{scripts.path}: expected one behavior definition for {name}")
        script = matches[0]
        expression = "0"
        for command, value in re.findall(r"(SET_INT|OR_INT)\(oInteractionSubtype, ([^\n]+)\),", script):
            expression = value if command == "SET_INT" else "(" + expression + " | " + value + ")"
        bowser_subtypes.append(expression)
    definitions += ["static const u32 bowserAttackSubtypes[] = {" + ",".join(bowser_subtypes) + "};"]
    definitions += [source("include/types.h").declaration("struct", "ObjectHitbox")]
    definitions += [source("src/engine/graph_node.h").declaration("enum", "GraphRenderFlags")]
    definitions += [source("src/engine/surface_collision.h").macros("FLOOR_LOWER_LIMIT(_MISC)?")]
    definitions += [source("src/game/obj_behaviors_2.h").declaration("enum", "AttackHandler")]
    twirl = source("src/game/mario_ground_twirl.c")
    definitions += [twirl.macros("GROUND_TWIRL_.*|METAL_ENEMY_CONTACT_TIMEOUT")]
    definitions += [source("src/game/mario_actions_moving.h").macros("GROUND_TWIRL_IMMUNE_REBOUND_STATE")]
    definitions += [source("src/game/interaction.c").macros("GROUND_TWIRL_LARGE_ENEMY_KNOCKBACK_FACTOR|GROUND_TWIRL_SMALL_BULLY_KNOCKBACK_CAP")]
    functions += [twirl.function(name) for name in (
        "mario_twirl_advance_contact_generation",
        "mario_ground_twirl_is_boo", "mario_ground_twirl_is_enemy", "mario_ground_twirl_push_speed",
        "mario_launch_stunned_enemy", "mario_metal_stun_enemy_on_contact")]
    def initializer(path, name):
        src = source(path)
        matches = list(re.finditer(r"^(?:static )?(?:struct ObjectHitbox|u8|u32|s16) " + name + r"\b[^=]*=\s*\{", src.mask, re.M))
        if len(matches) != 1:
            raise HarnessError(f"{path}: expected one initializer for {name}")
        end = src.mask.index(";", matches[0].end()) + 1
        return src.mapped(matches[0].start(), end)
    enemies = {"mad_piano": "sMadPianoHitbox", "bobomb": "sBobombHitbox", "goomba": "sGoombaHitbox", "koopa": "sKoopaHitbox", "spiny": "sSpinyHitbox",
        "snufit": "sSnufitHitbox", "fly_guy": "sFlyGuyHitbox", "skeeter": "sSkeeterHitbox",
        "swoop": "sSwoopHitbox", "enemy_lakitu": "sEnemyLakituHitbox", "monty_mole": "sMontyMoleHitbox",
        "klepto": "sKleptoHitbox", "scuttlebug": "sScuttlebugHitbox", "spindrift": "sSpindriftHitbox"}
    for enemy, hitbox in enemies.items():
        definitions += [initializer(f"src/game/behaviors/{enemy}.inc.c", hitbox)]
    definitions += [initializer("src/game/behaviors/goomba.inc.c", "sGoombaAttackHandlers")]
    definitions += [initializer("src/game/interaction.c", name) for name in (
        "sForwardKnockbackActions", "sBackwardKnockbackActions")]

    standard = source("src/game/obj_behaviors_2.c")
    functions += [standard.function(name) for name in (
        "obj_die_if_health_non_positive", "obj_set_knockback_action", "obj_set_squished_action",
        "obj_die_if_above_lava_and_health_non_positive", "obj_handle_attacks", "obj_check_attacks",
        "obj_act_knockback", "obj_act_squished")]
    functions += [source("src/game/object_helpers.c").function("cur_obj_set_hitbox_and_die_if_attacked"),
                  source("src/game/object_helpers.c").function("obj_set_hitbox")]
    for enemy, names in {"goomba": ("mark_goomba_as_dead",), "koopa": ("shelled_koopa_attack_handler",),
        "monty_mole": ("monty_mole_hide_in_hole", "monty_mole_defeat"),
        "klepto": ("klepto_release_held_object",), "chuckya": ("chuckya_die", "common_anchor_mario_behavior"),
        "heave_ho": ("bhv_heave_ho_throw_mario_loop",),
        "bobomb": ("bobomb_spawn_coin", "bobomb_act_explode", "bobomb_act_launched",
            "bobomb_check_interactions", "generic_bobomb_free_loop", "stationary_bobomb_free_loop"),
        "moneybag": ("moneybag_act_death",)}.items():
        functions += [source(f"src/game/behaviors/{enemy}.inc.c").function(name) for name in names]
    definitions += [source("include/object_constants.h").declaration("enum", "oBehParams2ndByteBowser")]
    definitions += [source("include/model_ids.h").macros("MODEL_BOWSER_WAVE")]
    functions += [source("src/game/bowser_throw.c").function("bowser_throw_nearest_mine")]
    functions += [source("src/game/behaviors/bowser_bomb.inc.c").function("bhv_bowser_bomb_init"),
                  source("src/game/behaviors/bowser_bomb.inc.c").function("bhv_bowser_bomb_loop"),
                  source("src/game/behaviors/explosion.inc.c").function("bhv_explosion_init"),
                  source("src/game/behaviors/bowser.inc.c").function("bowser_check_hit_mine"),
                  source("src/game/behaviors/bowser.inc.c").function("bowser_finish_throw"),
                  source("src/game/behaviors/bowser.inc.c").function("bowser_act_thrown")]
    functions += [source("src/game/behaviors/shock_wave.inc.c").function("bhv_bowser_shock_wave_loop")]
    functions += [source("src/game/behavior_actions.c").function("cur_obj_defeat_metal_contact_enemy_legacy")]
    mario = source("src/game/mario.c")
    definitions += [mario.macros("MARIO_SOAKED_TAIL_FRAMES")]
    definitions += [source("src/game/mario.h").macros("MARIO_BURN_.*")]
    functions += [mario.function("mario_apply_burning_damage")]
    functions += [source("src/game/interaction.c").function("mario_extinguish_fire")]
    functions += [mario.function(name) for name in (
        "mario_add_cap_power", "mario_clear_cap_flags", "mario_natural_cap_powers",
        "mario_has_water_contact", "mario_is_soaked", "mario_apply_soaked_status",
        "mario_consume_soaked_status", "mario_dry_from_wind", "mario_extinguish_soaked_burning",
        "mario_update_soaked_status")]
    functions += [standard.function("cur_obj_defeat_metal_contact_enemy")]
    definitions += [constants.macros("BULLY_BP_SIZE_BIG|BULLY_STYPE_CHILL")]
    functions += [source("src/game/interaction.c").region(
        r"^#define MAX_PUNCH_HIT_OBJECTS\b", r"^static struct Object \*sKnockbackSourceObj;")]
    functions += [source("src/game/interaction.c").function(name) for name in (
        "mario_set_punch_hit_phase", "bully_attack_hit_phase", "clear_separated_bully_hits",
        "update_attack_hit_phase", "punch_hit_object",
        "remember_punch_hit_object", "mario_forget_attack_object", "dive_hit_target",
        "determine_enemy_interaction", "remember_enemy_dive_hit")]
    functions += [source("src/game/mario_actions_automatic.c").function(name)
                  for name in ("act_grabbed", "act_ledge_grab")]
    definitions += [source("src/game/interaction.c").macros("MARIO_CAP_PICKUP_LOCK_FRAMES")]
    functions += [source("src/game/interaction.c").function(name) for name in (
        "mario_is_breakdancing", "mario_protects_from_knockback", "mario_breakdance_protects_from_damage",
        "does_mario_have_normal_cap_on_head", "mario_blow_off_cap_in_direction",
        "mario_remove_cap_powers", "mario_tick_contact_immunities", "mario_start_heave_ho_recovery_immunity",
        "mario_is_knocked_back", "interact_bully", "check_object_grab_mario",
        "mario_queue_bowser_impact_with_strength", "mario_queue_bowser_impact", "mario_bowser_impact_knockback", "mario_bowser_blast_knockback", "determine_knockback_action", "take_damage_from_interact_object",
        "take_damage_and_knock_back", "interact_damage", "interact_mr_blizzard", "interact_snufit_bullet",
        "interact_tornado", "interact_strong_wind", "interact_whirlpool", "check_lava_boost", "interact_flame", "interact_shock",
        "mario_vanish_ignores_hostile_contact")]
    functions += [source("src/game/interaction.c").region(
        r"^void mario_blow_off_cap\(", r"^Bool32 mario_lose_cap_to_enemy\(u32")]
    functions += [source("src/game/mario_actions_cutscene.c").function("act_shocked"),
                  source("src/game/mario.c").function("hurt_and_set_mario_action"),
                  source("src/game/mario_actions_stationary.c").function("act_shockwave_bounce")]

    # Exercise the queued-impact block before native action cancels, without
    # supplying the unrelated object-contact dispatcher to this fixture.
    dispatcher = source("src/game/interaction.c").function("mario_process_interactions")
    queued_impacts = dispatcher.split(
        "    update_ground_twirl_rebound_invulnerability(m);", 1)[1].split(
        "    sDelayInvincTimer = FALSE;", 1)[0]
    functions += ["static void process_queued_bowser_impacts(struct MarioState *m) {"
                  + queued_impacts + "}"]

    definitions += [source("src/game/level_update.h").declaration("enum", "WarpOperation")]
    definitions += [source("include/mario_geo_switch_case_ids.h").declaration("enum", "MarioEyesGSCId")]
    surfaces = source("include/surface_terrains.h")
    definitions += [surfaces.declaration("enum", name) for name in ("SurfaceTypes", "SurfaceFlags", "TerrainType")]
    definitions += [surfaces.macros("SURFACE_IS_NOT_HARD|SURFACE_IS_QUICKSAND|TERRAIN_MASK")]
    definitions += [source("include/mario_animation_ids.h").declaration("enum", "MarioAnimID")]
    definitions += [source("src/game/mario_actions_airborne.h").declaration("enum", name) for name in (
        "ActionArgsFreefall", "ActionStatesSlideKick", "ActionStatesGroundPound", "ActionArgsGroundPound", "ActionStatesLavaBoost")]
    air = source("src/game/mario_actions_airborne.c")
    definitions += [air.declaration("enum", "HandstandFallState"),
                    air.declaration("enum", "FallDamageSeverity")]
    definitions += [air.macros("SLIDE_KICK_(AIR_TIMEOUT_FRAMES|WALL_BONK_GRACE_FRAMES|FULL_VISUAL_PITCH_SPEED|NORMAL_BOUNCE_FACTOR|HELD_A_LIQUID_BOUNCE_FACTOR)")]
    functions += [air.function(name) for name in (
        "handstand_jump_protects_from_fall", "is_large_fall_at_height", "is_large_fall",
        "lose_cap_from_small_fall_damage", "play_fall_damage_impact",
        "fall_damage_severity_at_height", "fall_damage_severity", "play_wet_fall_impact", "check_fall_damage",
        "check_fall_damage_or_get_stuck")]
    mario = source("src/game/mario.c")
    definitions += [mario.region(r"^u8 sSquishScaleOverTime", r"^void squish_mario_model")]
    functions += [mario.function("squish_mario_model")]
    definitions += [source("src/game/liquid_particles.h").declaration("enum", "LiquidParticleMaterial")]
    levels = re.findall(r"^(?:DEFINE_LEVEL|STUB_LEVEL)\([^,]+,\s*(LEVEL_\w+)",
                        source("levels/level_defines.h").mask, re.M)
    definitions += ["enum { LEVEL_NONE, " + ", ".join(levels) + " };"]
    functions += [source("src/game/physics_lab.c").function("physics_lab_is_cold_pool")]
    definitions += [source("src/game/mario_actions_airborne.h").declaration("enum", "ActionArgsPuttingOnCapAir")]
    functions += [air.function(name) for name in ("lava_boost_on_wall", "update_ground_pound_horizontal_speed", "act_ground_pound", "act_lava_boost", "slide_kick_handle_wall", "slide_kick_continue_bounce", "act_slide_kick", "check_horizontal_wind", "finish_air_cap_pickup", "act_putting_on_cap_air", "check_common_airborne_cancels", "act_burning_jump", "act_burning_fall")]
    functions += [air.region(r"^#ifdef NO_GETTING_BURIED", r"^void play_far_fall_sound")]
    functions += [air.function("play_far_fall_sound")]
    functions += [source("src/game/mario.c").function("mario_has_land_movement_underwater")]
    functions += [source("src/game/mario_actions_automatic.c").function("check_common_automatic_cancels")]
    step = source("src/game/mario_step.c")
    definitions += [step.macros("WIND_CATCH_ACCEL|WIND_DEATH_CLEARANCE|WIND_CATCH_MARGIN")]
    definitions += [initializer("src/game/mario_step.c", "sMovingSandSpeeds")]
    functions += [step.function(name) for name in ("mario_is_on_floor", "mario_update_quicksand",
        "mario_update_moving_sand", "mario_update_windy_ground", "mario_is_in_vertical_wind",
        "wind_stopping_distance", "wind_safe_downward_speed", "constrain_wind_descent", "apply_vertical_wind")]

    objects = source("src/game/object_list_processor.c")
    definitions += [objects.macros("GROUND_TWIRL_KNOCKBACK_GRAVITY")]
    functions += [objects.function(name) for name in (
        "ground_twirl_enemy_needs_water_update", "stop_ground_twirl_stunned_enemy", "ground_twirl_support_raised_enemy",
        "update_ground_twirl_stun_motion", "update_ground_twirl_stunned_object",
        "update_object_with_ground_twirl_stun")]

def mutations_metal(root):
    path = "src/game/behaviors/shock_wave.inc.c"
    yield "metal", "original_bowser_current_wave", "metal.original_bowser_wave", {
        path: MutationSource(root / path).replace_in_function("bhv_bowser_shock_wave_loop", "if (o->oAnimState)", "if (FALSE)")}
    yield "metal", "original_metal_keeps_light_reactions", "metal.cap_preset_knockback", {
        "src/game/interaction.c": MutationSource(root / "src/game/interaction.c").replace_in_function(
            "determine_knockback_action", "else if (mario_has_current_metal_cap(m))",
            "else if (m->flags & MARIO_METAL_CAP)")}

    yield "metal", "original_vanish_loses_flame_immunity", "metal.cap_preset_damage", {
        "src/game/interaction.c": MutationSource(root / "src/game/interaction.c").replace_in_function(
            "interact_flame", "game_options_get(GO_VANISH_CAP) == 1", "FALSE")}
    yield "metal", "original_metal_keeps_electric_damage", "metal.cap_preset_damage", {
        "src/game/interaction.c": MutationSource(root / "src/game/interaction.c").replace_in_function(
            "take_damage_from_interact_object", "game_options_get(GO_METAL_CAP) == 1", "FALSE")}

    blast_path = "src/game/interaction.c"
    yield "metal", "retain_soaked_after_blast", "metal.soaked_blast_drying", {
        blast_path: MutationSource(root / blast_path).replace_in_function("interact_damage",
            "mario_consume_soaked_status(m);", "(void) m;")}
    dive_path = "src/game/interaction.c"
    yield "metal", "repeat_bully_other_attacks", "metal.bully_all_attacks_once", {
        dive_path: MutationSource(root / dive_path).replace_in_function("determine_enemy_interaction",
            "obj->oInteractType == INTERACT_BULLY && sPunchContacts.phase != 0",
            "FALSE")}
    yield "metal", "repeat_bully_dive", "metal.bully_dive_once", {
        dive_path: MutationSource(root / dive_path).replace_in_function("determine_enemy_interaction",
            "&& punch_hit_object(target)", "&& FALSE")}
    yield "metal", "forget_bully_dive_hit", "metal.bully_dive_once", {
        dive_path: MutationSource(root / dive_path).replace_in_function("interact_bully",
            "remember_enemy_dive_hit(m, obj, interaction);",
            "remember_enemy_dive_hit(m, obj, INT_NONE);")}
    breakdance_path = "src/game/interaction.c"
    breakdance = MutationSource(root / breakdance_path)
    breakdance_case = "metal.breakdance_damage_windows"
    yield "metal", "breakdance_wrong_damage_window", breakdance_case, {
        breakdance_path: breakdance.replace_in_function("mario_breakdance_protects_from_damage",
            "animInfo->animFrame != 12", "animInfo->animFrame != 13")}
    yield "metal", "breakdance_tail_knockback", breakdance_case, {
        breakdance_path: breakdance.replace_in_function("mario_protects_from_knockback",
            "return game_options_cheat_active(GO_KNOCKBACK) || mario_is_breakdancing(m);",
            "return game_options_cheat_active(GO_KNOCKBACK) || (mario_is_breakdancing(m) && m->marioObj->header.gfx.animInfo.animFrame < 22);")}
    yield "metal", "inherit_previous_spin_vulnerability", breakdance_case, {
        breakdance_path: breakdance.replace_in_function("mario_breakdance_protects_from_damage",
            "if (m->actionTimer == 0)", "if (FALSE)")}
    path = "src/game/behaviors/explosion.inc.c"
    yield "metal", "unsafe_punch_blast", "metal.thicc_punch_explosion_safe", {
        path: MutationSource(root / path).replace_in_function("bhv_explosion_init",
            "o->oBehParams2ndByte == HE_THICC_PUNCH_EXPLOSION", "FALSE")}
    path = "src/game/mario_he_thicc.c"
    yield "metal", "missing_slide_bounce_wave", "metal.thicc_slide_bounce_wave", {
        path: MutationSource(root / path).replace_in_function("mario_he_thicc_slide_kick_bounce",
            "mario_ground_pound_impact(m);", "(void) m;")}

    path = "src/game/mario_actions_airborne.c"
    for name, old, new in (
        ("remove_held_wing_fall_protection", "m->flags & MARIO_WING_CAP", "FALSE"),
        ("use_masked_a_for_wing_fall", "m->controller->buttonDown & A_BUTTON", "m->input & INPUT_A_DOWN"),
    ):
        yield "metal", name, "metal.wing_fall_damage", {
            path: MutationSource(root / path).replace_in_function("check_fall_damage", old, new)}
    path = "src/game/mario_actions_airborne.c"
    air = MutationSource(root / path)
    condition = "m->controller->buttonDown & A_BUTTON"
    for case in ("metal.wing_burial_boundaries", "metal.wing_burial_landings"):
        yield "metal", "remove_held_wing_protection_" + case.rsplit(".", 1)[1], case, {
            path: air.replace_in_region("#ifdef NO_GETTING_BURIED", "\nvoid play_far_fall_sound",
                condition, "FALSE")}
    yield "metal", "use_masked_a_for_wing_burial", "metal.wing_burial_boundaries", {
        path: air.replace_in_region("#ifdef NO_GETTING_BURIED", "\nvoid play_far_fall_sound",
            condition, "m->input & INPUT_A_DOWN")}
    checks = [
        ("ignore_saved_wet_fall", "metal.wet_fall_feedback", "src/game/mario_actions_airborne.c", "check_fall_damage",
         "play_wet_fall_impact(m, fall_damage_severity(m));", "play_wet_fall_impact(m, FALL_DAMAGE_NONE);"),
        ("large_wet_fall_uses_small_splash", "metal.wet_fall_feedback", "src/game/mario_actions_airborne.c", "play_wet_fall_impact",
         "m->particleFlags |= PARTICLE_WATER_SPLASH;", "m->particleFlags |= PARTICLE_SHALLOW_WATER_SPLASH;"),
        ("small_wet_fall_uses_large_rumble", "metal.wet_fall_feedback", "src/game/mario_actions_airborne.c", "play_wet_fall_impact",
         "queue_rumble_data(3, 40);", "queue_rumble_data(5, 80);"),
        ("keep_wet_fall_scream", "metal.wet_fall_scream", "src/game/mario_actions_airborne.c", "play_wet_fall_impact",
         "stop_sound(SOUND_MARIO_WAAAOOOW, m->marioObj->header.gfx.cameraToObject);", "(void)stop_sound;"),
        ("keep_wet_fall_warning_flag", "metal.wet_fall_feedback", "src/game/mario_actions_airborne.c", "play_wet_fall_impact",
         "m->flags &= ~MARIO_FALL_SOUND_PLAYED;", "(void)0;"),
        ("wet_fall_at_exact_ten", "metal.wet_fall_boundaries", "src/game/mario_actions_airborne.c", "check_fall_damage",
         "m->pos[1] < find_water_level", "m->pos[1] <= find_water_level"),
        ("leave_mario_grabbed", "metal.grab_cleanup", "src/game/behavior_actions.c", "cur_obj_defeat_metal_contact_enemy_legacy",
         "INT_STATUS_MARIO_THROWN_BY_OBJ | INT_STATUS_MARIO_DROPPED_BY_OBJ", "INT_STATUS_MARIO_DROPPED_BY_OBJ"),
        ("keep_heave_ho_anchor", "metal.grab_cleanup", "src/game/behaviors/heave_ho.inc.c", "bhv_heave_ho_throw_mario_loop",
         "if (o->parentObj->activeFlags == ACTIVE_FLAG_DEACTIVATED)", "if (FALSE && o->parentObj->activeFlags == ACTIVE_FLAG_DEACTIVATED)"),
        ("skip_klepto_item_release", "metal.enemy_roster", "src/game/obj_behaviors_2.c", "cur_obj_defeat_metal_contact_enemy",
         "klepto_release_held_object();", "if (FALSE) klepto_release_held_object();"),
        ("ignore_metal_grace", "metal.timeout_and_contacts", "src/game/mario_ground_twirl.c", "mario_metal_stun_enemy_on_contact",
         "obj->metalContactCooldown == 0 && obj->oGroundTwirlContactRebound == 0", "TRUE && obj->oGroundTwirlContactRebound == 0"),
        ("ignore_metal_wall_contact", "metal.timeout_and_contacts", "src/game/mario_ground_twirl.c", "mario_metal_stun_enemy_on_contact",
         "(grounded || atWall)", "(grounded || (atWall && FALSE))"),
        ("kill_metal_overlap_at_rest", "metal.eligibility", "src/game/mario_ground_twirl.c", "mario_metal_stun_enemy_on_contact",
         "m->vel[0] * dx + m->vel[2] * dz > 0.0f", "m->vel[0] * dx + m->vel[2] * dz >= 0.0f"),
        ("keep_huge_goomba_weak_hit", "metal.enemy_roster", "src/game/obj_behaviors_2.c", "cur_obj_defeat_metal_contact_enemy",
         "| ATTACK_GROUND_POUND_OR_TWIRL;", "| ATTACK_FAST_ATTACK;"),
        ("keep_klepto_alive", "metal.enemy_roster", "src/game/obj_behaviors_2.c", "cur_obj_defeat_metal_contact_enemy",
         "o->oHealth = 0;", "o->oHealth = 1;"),
        ("skip_metal_native_defeat", "metal.enemy_roster", "src/game/object_list_processor.c", "update_ground_twirl_stunned_object",
         "cur_obj_defeat_metal_contact_enemy();", "gCurrentObject->oInteractStatus = INT_STATUS_NONE;"),
        ("explode_bobomb_in_place", "metal.bobomb_launch_before_explosion", "src/game/behavior_actions.c", "cur_obj_defeat_metal_contact_enemy_legacy",
         "o->oAction = BOBOMB_ACT_LAUNCHED;", "o->oAction = BOBOMB_ACT_EXPLODE;"),
        ("retain_bobomb_expired_fuse", "metal.bobomb_launch_before_explosion", "src/game/behavior_actions.c", "cur_obj_defeat_metal_contact_enemy_legacy",
         "o->oBobombFuseTimer = 0;", "(void)0;"),
        ("allow_metal_explosion_contact", "metal.explosion_immunity", "src/game/interaction.c", "interact_damage",
         "if ((m->flags & MARIO_METAL_CAP)\n        && obj_has_behavior(obj, bhvExplosion)",
         "if (FALSE && obj_has_behavior(obj, bhvExplosion)"),
        ("retain_metal_heavy_reaction", "metal.light_enemy_knockback", "src/game/interaction.c", "determine_knockback_action",
         "else if (mario_has_current_metal_cap(m))", "else if (FALSE)"),
        ("retain_metal_big_knockback", "metal.light_enemy_knockback", "src/game/interaction.c", "take_damage_and_knock_back",
         "&& !mario_has_current_metal_cap(m)", "&& TRUE"),
        ("allow_metal_tornado_capture", "metal.wind_immunity", "src/game/interaction.c", "interact_tornado",
         "if (m->flags & MARIO_METAL_CAP)", "if (FALSE)"),
        ("allow_metal_gust_capture", "metal.wind_immunity", "src/game/interaction.c", "interact_strong_wind",
         "if (m->flags & MARIO_METAL_CAP)", "if (FALSE)"),
        ("allow_metal_ground_wind", "metal.wind_immunity", "src/game/mario_step.c", "mario_update_windy_ground",
         "!mario_has_current_metal_cap(m) &&", "TRUE &&"),
        ("allow_metal_air_wind", "metal.wind_immunity", "src/game/mario_actions_airborne.c", "check_horizontal_wind",
         "!mario_has_current_metal_cap(m) &&", "TRUE &&"),
        ("allow_metal_vertical_wind", "metal.wind_immunity", "src/game/mario_step.c", "mario_is_in_vertical_wind",
         "!mario_has_current_metal_cap(m) &&", "TRUE &&"),
        ("allow_metal_wind_catch", "metal.wind_immunity", "src/game/mario_step.c", "constrain_wind_descent",
         "mario_has_current_metal_cap(m) ||", "FALSE ||"),
        ("retain_metal_wind_action", "metal.wind_immunity", "src/game/mario_actions_airborne.c", "check_common_airborne_cancels",
         "if (mario_has_current_metal_cap(m)", "if (FALSE"),
        ("allow_metal_lava_boost", "metal.solid_lava", "src/game/interaction.c", "check_lava_boost",
         "if (m->flags & MARIO_METAL_CAP)", "if (FALSE)"),
        ("allow_capped_water_skip", "metal.capped_water_skip", "src/game/mario_actions_airborne.c", "act_slide_kick",
         "&& !mario_has_land_movement_underwater(m)",
         "&& TRUE"),
        ("retain_metal_lava_skip_bonus", "metal.capped_water_skip", "src/game/mario_actions_airborne.c", "act_slide_kick",
         "&& !(m->flags & MARIO_METAL_CAP)", "&& TRUE"),
        ("cold_skip_uses_white_splash", "metal.slide_kick_liquid_effects", "src/game/mario_actions_airborne.c", "act_slide_kick",
         "if (coldWater)", "if (!coldWater)"),
        ("repeat_lava_splash_on_recovery", "metal.slide_kick_liquid_effects", "src/game/mario_actions_airborne.c", "act_slide_kick",
         "if (lavaBounce)", "if (!lavaBounce)"),
        ("weak_bowser_throw_triggers_mine", "metal.bowser_throw_velocity", "src/game/behaviors/bowser.inc.c", "bowser_check_hit_mine",
         "!(o->oBowserStatus & BOWSER_STATUS_VALID_THROW)", "FALSE"),
        ("bowser_mine_after_velocity_reset", "metal.bowser_throw_velocity", "src/game/behaviors/bowser.inc.c", "bowser_act_thrown",
         "if (hitMine)", "if (hitMine && bowser_check_hit_mine())"),
        ("bowser_point_blank_no_launch", "metal.bowser_body_reactions", "src/game/interaction.c", "take_damage_and_knock_back",
         "dist_between_objects(m->marioObj, bowser) < BOWSER_POINT_BLANK_RADIUS", "FALSE"),
        ("bowser_near_old_damage", "metal.bowser_impact_queue", "src/game/interaction.c", "mario_bowser_impact_knockback",
         "heavy ? 2 : 1", "heavy ? 4 : 0"),
        ("bowser_mine_wrong_direction", "metal.bowser_mine_impulse", "src/game/interaction.c", "take_damage_and_knock_back",
         "atan2s(m->pos[2], m->pos[0])", "atan2s(-m->pos[2], -m->pos[0])"),
        ("wave_old_electric_reaction", "metal.bowser_wave_reactions", "src/game/behaviors/shock_wave.inc.c", "bhv_bowser_shock_wave_loop",
         "mario_queue_bowser_impact_with_strength(o, TRUE);", "gMarioObject->oInteractStatus |= INT_STATUS_MARIO_SHOCKWAVE;"),
        ("first_bowser_wave_heavy", "metal.bowser_wave_reactions", "src/game/behaviors/shock_wave.inc.c", "bhv_bowser_shock_wave_loop",
         "mario_queue_bowser_impact_with_strength(o, FALSE);", "mario_queue_bowser_impact_with_strength(o, TRUE);"),
        ("second_bowser_wave_light", "metal.bowser_wave_reactions", "src/game/behaviors/shock_wave.inc.c", "bhv_bowser_shock_wave_loop",
         "mario_queue_bowser_impact_with_strength(o, TRUE);", "mario_queue_bowser_impact_with_strength(o, FALSE);"),
        ("retain_cap_on_bowser_impact", "metal.bowser_impact_queue", "src/game/interaction.c", "mario_bowser_impact_knockback",
         "if (heavy) {", "if (FALSE) {"),
        ("retain_cap_on_bowser_launch", "metal.bowser_body_reactions", "src/game/interaction.c", "mario_bowser_blast_knockback",
         "mario_blow_off_cap(m, MAX(15.0f, ABS(m->forwardVel) + 5.0f));", "(void) 0;"),
        ("retain_cap_on_bowser_charge", "metal.bowser_body_reactions", "src/game/interaction.c", "determine_knockback_action",
         "strengthIndex == 2 &&", "FALSE && strengthIndex == 2 &&"),
        ("retain_power_on_bowser_body", "metal.bowser_cap_attacks", "src/game/interaction.c", "take_damage_and_knock_back",
         "mario_remove_cap_powers(m);", "if (FALSE) mario_remove_cap_powers(m);"),
        ("retain_power_on_bowser_flame", "metal.bowser_cap_attacks", "src/game/interaction.c", "interact_flame",
         "mario_remove_cap_powers(m);", "if (FALSE) mario_remove_cap_powers(m);"),
        ("retain_power_on_bowser_wave", "metal.bowser_cap_attacks", "src/game/interaction.c", "mario_bowser_impact_knockback",
         "mario_remove_cap_powers(m);", "if (FALSE) mario_remove_cap_powers(m);"),
        ("ignore_metal_shock_damage", "metal.capped_shocks", "src/game/interaction.c", "take_damage_from_interact_object",
         "&& (game_options_get(GO_METAL_CAP) == 1 || m->interactObj->oInteractType != INTERACT_SHOCK)", "&& TRUE"),
        ("ignore_vanish_shock_contact", "metal.capped_shocks", "src/game/interaction.c", "interact_shock",
         "if (!sInvulnerable", "if (!sInvulnerable && !(m->flags & MARIO_VANISH_CAP)"),
        ("ignore_vanish_shock_prepass", "metal.capped_shocks", "src/game/interaction.c", "mario_vanish_ignores_hostile_contact",
         "(INTERACT_DAMAGE", "(INTERACT_SHOCK | INTERACT_DAMAGE"),
        ("retain_shocked_power", "metal.capped_shocks", "src/game/interaction.c", "interact_shock",
         "m->flags &= ~MARIO_SPECIAL_CAPS;", "m->flags &= ~0;"),
        ("ignore_vanish_fire_prepass", "metal.vanish_fire", "src/game/interaction.c", "mario_vanish_ignores_hostile_contact",
         "(INTERACT_DAMAGE", "(INTERACT_FLAME | INTERACT_DAMAGE"),
        ("ignore_vanish_fire_handler", "metal.vanish_fire", "src/game/interaction.c", "interact_flame",
         "if (!sInvulnerable && !(obj->oInteractionSubtype & INT_SUBTYPE_DELAY_INVINCIBILITY))",
         "if (!sInvulnerable && !(m->flags & MARIO_VANISH_CAP) && !(obj->oInteractionSubtype & INT_SUBTYPE_DELAY_INVINCIBILITY))"),
        ("ignite_soaked_mario", "metal.vanish_fire", "src/game/interaction.c", "interact_flame",
         "if (mario_is_soaked(m)\n            || (game_options_get(GO_SOAKED) && m->soakedFlameProtection\n                && !mario_has_current_vanish_cap(m)))",
         "if (FALSE)"),
        ("reignite_continuous_soaked_fire", "metal.soaked_fire_contact", "src/game/interaction.c", "interact_flame",
         "game_options_get(GO_SOAKED) && m->soakedFlameProtection", "FALSE"),
        ("keep_soaked_fire_after_leaving", "metal.soaked_fire_contact", "src/game/mario.c", "mario_update_soaked_status",
         "m->soakedFlameProtection = FALSE;", "(void)m;"),
        ("reignite_after_midburn_soaking", "metal.soaked_fire_contact", "src/game/mario.c", "mario_extinguish_soaked_burning",
         "m->soakedFlameProtection = (m->collidedObjInteractTypes & INTERACT_FLAME) != 0;",
         "m->soakedFlameProtection = FALSE;"),
        ("skip_soaked_shock_multiplier", "metal.capped_shocks", "src/game/interaction.c", "interact_shock",
         "if (wetShock) {", "if (FALSE) {"),
        ("ignore_water_after_soaked_shock", "metal.repeated_water_shocks", "src/game/interaction.c", "interact_shock",
         "game_options_get(GO_SOAKED) && mario_has_water_contact(m, m->waterLevel)", "FALSE"),
        ("damage_soaked_lava_contact", "metal.soaked_lava", "src/game/interaction.c", "check_lava_boost",
         "if (!soaked) {", "if (TRUE) {"),
        ("damage_soaked_lava_rebounce", "metal.soaked_lava", "src/game/mario_actions_airborne.c", "act_lava_boost",
         "&& !soaked) {", "&& TRUE) {"),
        ("ignore_later_lava_bounces", "metal.soaked_lava", "src/game/mario_actions_airborne.c", "act_lava_boost",
         "if (!(m->flags & MARIO_METAL_CAP) && !soaked) {", "if (FALSE) {"),
        ("suppress_metal_ground_pound_lava", "metal.ground_pound_lava", "src/game/mario_actions_airborne.c", "act_ground_pound",
         "if (mario_has_current_metal_cap(m) && m->floor != NULL", "if (FALSE && m->floor != NULL"),
        ("stop_held_metal_lava_rebounce", "metal.held_a_lava_bounce", "src/game/mario_actions_airborne.c", "act_lava_boost",
         "|| (m->input & INPUT_A_DOWN)", "|| FALSE"),
        ("ignore_release_metal_lava_rebounce", "metal.held_a_lava_bounce", "src/game/mario_actions_airborne.c", "act_lava_boost",
         "|| (m->input & INPUT_A_DOWN)", "|| (m->flags & MARIO_METAL_CAP)"),
        ("damage_metal_lava_rebounce", "metal.held_a_lava_bounce", "src/game/mario_actions_airborne.c", "act_lava_boost",
         "if (!(m->flags & MARIO_METAL_CAP) && !soaked) {", "if (TRUE) {"),
        ("cancel_intentional_metal_lava_bounce", "metal.ground_pound_lava", "src/game/mario_actions_airborne.c", "check_common_airborne_cancels",
         "m->actionArg == ACT_ARG_LAVA_BOOST_FLOOR", "m->actionArg != ACT_ARG_LAVA_BOOST_WALL"),
    ]
    for name, case, path, function, before, after in checks:
        yield "metal", name, case, {path: MutationSource(root / path).replace_in_function(function, before, after)}
    path = "src/game/mario_actions_airborne.c"
    src = MutationSource(root / path)
    before = "if (mario_has_current_vanish_cap(m) && terrainType == TERRAIN_SAND"
    yield "metal", "retain_vanish_sand_burial", "metal.vanish_sand", {
        path: src.replace_unique(before, "if (FALSE && terrainType == TERRAIN_SAND")}
