"""Extraction primitives shared by the host suites."""

import re

from regression_support import HarnessError, Source
from .mutation_support import MutationSource

def prepare_common_constants(source, definitions, gameplay_options=False, cap_functions=None):
    """Append shared declarations to definitions, using catalog defaults for options.

    If cap_functions is supplied, append production cap bodies to that list too;
    these bodies require gameplay_options so they observe the same option defaults.
    """
    definitions += [source("include/config/config_movement.h").macros(
        "MARIO_SPEED_SAFETY_CAP|MARIO_ACTIVE_SPEED_CAP|MARIO_UNLIMITED_COLLISION_MAX_STEPS")]
    if gameplay_options:
        options = source("src/game/game_options.h")
        definitions += [options.macros("GAME_OPTION_CATALOG"),
                        options.declaration("enum", "GameOptionId"),
                        "static u8 sTestGameOptions[GO_COUNT] = {\n"
                        "#define TEST_OPTION(id, category, kind, label, lo, hi, step, value) [GO_##id] = value,\n"
                        "GAME_OPTION_CATALOG(TEST_OPTION)\n#undef TEST_OPTION\n};",
                        "static inline u8 game_options_get(enum GameOptionId id) { return sTestGameOptions[id]; }",
                        "static inline s32 game_options_cheat_active(enum GameOptionId id) { "
                        "return sTestGameOptions[GO_CHEATS_ENABLED] && sTestGameOptions[id]; }"]
        definitions += [source("src/game/game_options_runtime.c").function("game_options_damage_voice_muted")]
    if cap_functions is not None:
        assert gameplay_options, "Cap fixtures must use production option defaults"
        helpers = ("mario_has_current_metal_cap", "mario_has_current_vanish_cap",
                   "mario_has_water_damage_protection")
        definitions += ["struct MarioState;"]
        definitions += [f"s32 {name}(struct MarioState *m);" for name in helpers]
        cap_functions += [source("src/game/mario.c").function(name) for name in helpers]
    definitions.append("struct MarioState;")
    definitions.append("s32 mario_keeps_shell(struct MarioState *m);")
    definitions.append("s32 mario_blocks_harmful_knockback(struct MarioState *m);")
    definitions.append("s32 mario_is_knockback_recovery(u32 action);")
    definitions.append("s32 mario_move_buffer_family(u32 action);")
    definitions.append("s32 mario_is_buffered_landing(u32 action);")
    definitions.append("void mario_update_move_buffers(struct MarioState *m);")
    # These fixtures isolate existing gameplay; the statistics units have their own executable tests.
    stats = source("src/game/player_stats.h").text
    for name in re.findall(r"\b(player_stats_\w+)\s*\(", stats):
        definitions.append(f"#define {name}(...) ((void) 0)")
    definitions += [source("src/game/mario.h").macros("MARIO_TWIRL_COOLDOWN_FRAMES|MARIO_TWIRL_WALL_BOOST_MULTIPLIER|MARIO_WALL_TWIRL_SOUND_GAP_FRAMES")]
    definitions += [source("src/game/mario.h").macros(
        "MARIO_LANDING_JUMP_BUFFER_FRAMES|MARIO_BACKWARD_LONG_JUMP_CROUCH_FRAMES|MARIO_BACKFLIP_LANDING_BUFFER_FRAMES|MARIO_WALL_KICK_INPUT_BUFFER_FRAMES|MARIO_BURN_COIN_JUMP_BUFFER_FRAMES")]
    definitions += [source("include/object_constants.h").macros(
        "ACTIVE_FLAG_BLINKING|OBJECT_BLINK_MIN_ALPHA")]
    definitions += [source("src/game/mario_actions_automatic.h").macros(
        "ACT_ARG_LEDGE_GRAB_.*|LEDGE_GRAB_VOICE_MIN_FALL_SPEED")]


def shared_mutations(root, suites):
    if "switches" in suites:
        path = "src/game/behaviors/breakable_box.inc.c"
        for name, old, new in (
            ("late_box_warning", "warningAge >= 0", "o->oTimer >= 360 && warningAge >= 0"),
            ("late_box_expiry", "o->oTimer >= PURPLE_SWITCH_HIDDEN_ACTIVE_FRAMES",
             "o->oTimer > PURPLE_SWITCH_HIDDEN_ACTIVE_FRAMES"),
        ):
            yield "switches", name, "switches.box_warning_timeline", {
                path: MutationSource(root / path).replace_in_function("hidden_box_update_lifetime", old, new)}
        path = "src/game/behaviors/purple_switch.inc.c"
        yield "switches", "short_switch_warning_sound", "switches.hidden_release", {
            path: MutationSource(root / path).replace_in_function("bhv_purple_switch_loop",
                "o->oPurpleSwitchEffectTimer < PURPLE_SWITCH_HIDDEN_BLINK_FRAMES",
                "o->oPurpleSwitchEffectTimer < 60")}
    if "bounce" in suites:
        path = "src/game/mario_actions_submerged.c"
        sound = "play_sound(SOUND_ACTION_WATER_PLUNGE, m->marioObj->header.gfx.cameraToObject);"
        windup_end = ("m->actionState = ACT_STATE_WATER_DOWN_BURST_DELAY;\n"
                      "                m->actionTimer = 0;")
        delayed = MutationSource(root / path).replace_in_function(
            "act_water_down_burst", windup_end + "\n                " + sound, windup_end)
        delayed = MutationSource(root / path, delayed).replace_in_function(
            "act_water_down_burst",
            "m->actionTimer++;\n            if (m->actionTimer >= WATER_DOWN_BURST_DELAY_FRAMES)",
            "m->actionTimer++;\n"
            "            if (m->actionTimer == 1) { " + sound + " }\n"
            "            if (m->actionTimer >= WATER_DOWN_BURST_DELAY_FRAMES)")
        yield "bounce", "late_water_down_splash", "bounce.water_down_splash_timing", {path: delayed}
    for suite, name, case, path, function, old, new in (
        ("twirl", "omit_generic_capless_hazard_damage", "twirl.capless_hazard_recoil", "src/game/interaction.c", "mario_ground_twirl_contact_can_damage",
         "|| (obj->oInteractType &", "|| (0 &"),
        ("twirl", "skip_immune_projectile_contacts", "twirl.projectile_immunity_contacts", "src/game/interaction.c", "mario_process_interactions",
         "mario_clear_projectile_contacts(m);", "if (FALSE) { mario_clear_projectile_contacts(m); }"),
        ("twirl", "ignore_ordinary_projectile_invincibility", "twirl.projectile_immunity_contacts", "src/game/interaction.c", "mario_clear_projectile_contacts",
         "sInvulnerable || m->invincTimer != 0", "FALSE"),
        ("twirl", "ignore_projectile_cap_immunity", "twirl.projectile_immunity_contacts", "src/game/interaction.c", "mario_clear_projectile_contacts",
         "(mario_has_current_metal_cap(m) || mario_has_current_vanish_cap(m))", "FALSE"),
        ("twirl", "destroy_room_separated_projectile", "twirl.projectile_immunity_boundaries", "src/game/interaction.c", "mario_clear_projectile_contacts",
         "&& !(obj->activeFlags & ACTIVE_FLAG_IN_DIFFERENT_ROOM)", ""),
        ("twirl", "destroy_unlaunched_snowball", "twirl.projectile_immunity_boundaries", "src/game/interaction.c", "mario_ground_twirl_destroy_projectile",
         "&& obj->oAction == MR_BLIZZARD_SNOWBALL_ACT_COLLISION", ""),
        ("twirl", "ignore_zero_damage_hazard", "twirl.capless_hazard_recoil", "src/game/interaction.c", "mario_ground_twirl_contact_can_damage",
         "|| (obj->oInteractType &", "|| (obj->oDamageOrCoinValue > 0 && obj->oInteractType &"),
        ("metal", "omit_capless_hazard_minimum", "metal.damage_values", "src/game/interaction.c", "take_damage_from_interact_object",
         "MAX(1, damage * 2)", "damage * 2"),
        ("twirl", "parry_bowser_during_twirl", "twirl.bowser_twirl_dispatch", "src/game/interaction.c", "mario_handle_defensive_contact",
         "if (object->oInteractionSubtype & INT_SUBTYPE_BOWSER_ATTACK)", "if (FALSE)"),
        ("twirl", "retain_twirl_immunity_to_bowser", "twirl.bowser_twirl_dispatch", "src/game/interaction.c", "mario_ground_twirl_takes_hazard_damage",
         "(obj->oInteractionSubtype & INT_SUBTYPE_BOWSER_ATTACK)", "FALSE"),
        ("fixes", "invisible_invincibility_blink", "fixes.invincibility_opacity", "src/game/mario.c", "mario_set_cap_render_state", "MIN(alpha, 64)", "MIN(alpha, 0)"),
        ("fixes", "brighten_invincibility_warp", "fixes.invincibility_opacity", "src/game/mario.c", "mario_set_cap_render_state", "MIN(alpha, 64)", "(alpha + 64 - alpha)"),
        ("metal", "skip_thicc_fall_shockwave", "metal.thicc_fall_shockwave", "src/game/mario_actions_airborne.c", "play_fall_damage_impact", "mario_ground_pound_impact(m);", "set_camera_shake_from_hit(SHAKE_FALL_DAMAGE);"),
        ("metal", "double_thicc_pound_shockwave", "metal.thicc_fall_shockwave", "src/game/mario_actions_airborne.c", "play_fall_damage_impact", "&& m->action != ACT_GROUND_POUND", ""),
        ("twirl", "thicc_pound_small_radius", "twirl.thicc_pound_range", "src/game/mario_ground_twirl.c", "mario_ground_pound_impact", "sqr(HE_THICC_GROUND_POUND_RADIUS)", "sqr(HE_THICC_GROUND_POUND_RADIUS * 0.5f)"),
        ("twirl", "thicc_pound_through_walls", "twirl.thicc_pound_eligibility", "src/game/mario_ground_twirl.c", "mario_ground_pound_impact", "|| mario_wall_between_object(m->pos, obj)", ""),
        ("twirl", "standard_pound_stuns", "twirl.thicc_pound_range", "src/game/mario_ground_twirl.c", "mario_ground_pound_impact", "mario_character_palette() != MARIO_PALETTE_HE_THICC", "mario_character_palette() == 255"),
        ("twirl", "thicc_pound_short_stun", "twirl.thicc_pound_range", "src/game/mario_ground_twirl.c", "mario_ground_pound_impact", "obj->oGroundTwirlStunDuration = HE_THICC_GROUND_POUND_STUN_FRAMES;", "obj->oGroundTwirlStunDuration = 6;"),
        ("geometry", "shake_standard_contacts", "geometry.thicc_contact_shakes", "src/game/mario.c", "mario_he_thicc_contact_shake", "mario_character_palette() == MARIO_PALETTE_HE_THICC", "TRUE"),
        ("metal", "skip_pound_floor_impact", "metal.ground_pound_lava", "src/game/mario_actions_airborne.c", "act_ground_pound", "mario_ground_pound_impact(m);", "(void)0;"),
        ("bounce", "skip_water_pound_impact", "bounce.underwater_ground_pound_impact", "src/game/mario_actions_submerged.c", "finish_water_down_burst", "mario_ground_pound_impact(m);", "(void)0;"),
        ("switches", "reject_swimming_pound", "switches.underwater_impact", "src/game/behaviors/purple_switch.inc.c", "bhv_purple_switch_loop", "(poundImpact || !(gMarioStates[0].action & MARIO_NO_PURPLE_SWITCH))", "!(gMarioStates[0].action & MARIO_NO_PURPLE_SWITCH)"),
        ("switches", "read_rebuilt_pound_floor", "switches.underwater_impact", "src/game/object_helpers.c", "cur_obj_is_mario_ground_pounding_platform", "gMarioStates[0].waterGroundPoundFloorObject == o", "gMarioStates[0].floor != NULL && gMarioStates[0].floor->object == o"),
        ("fixes", "poison_one_second_reset", "fixes.poison_contact_and_reentry", "src/game/mario.c", "update_poison_health_reentry", "sPoisonHealth.cleanAirFrames >= POISON_HEALTH_REENTRY_RESET_FRAMES", "sPoisonHealth.cleanAirFrames >= WATER_HEALTH_REENTRY_RESET_FRAMES"),
        ("fixes", "ignore_shallow_poison", "fixes.poison_contact_and_reentry", "src/game/mario.c", "update_mario_poison_input", "m->pos[1] < gasLevel", "m->pos[1] < gasLevel - 100.0f"),
        ("fixes", "retain_poison_on_protection", "fixes.poison_health_timing", "src/game/mario.c", "update_poison_health_reentry", "if (poisonProtected && !sPoisonHealth.protected)", "if (FALSE)"),
        ("bounce", "skip_water_cap_box_contact", "bounce.underwater_cap_boxes", "src/game/mario_actions_submerged.c", "finish_water_down_burst", "mario_attack_cap_box_surface(m, m->floor, INT_GROUND_POUND_OR_TWIRL);", "(void)0;"),
        ("bounce", "skip_swimming_box_punch", "bounce.underwater_cap_boxes", "src/game/interaction.c", "interact_breakable", "m->action == ACT_WATER_PUNCH && m->actionState == ACT_STATE_WATER_PUNCH_START", "FALSE"),
    ):
        if suite in suites:
            yield suite, name, case, {path: MutationSource(root / path).replace_in_function(function, old, new)}

def mutations_audit_repairs(root, suites):
    checks = [
        ("render", "late_pose_context", "render.pose_owner",
         "src/game/rendering_graph_node.c", "geo_set_animation_globals",
         "if (gCurGraphNodeHeldObject == NULL", "if (gCurGraphNodeObject != NULL && gCurGraphNodeHeldObject == NULL"),
        ("geometry", "redispatch_wall_expiry", "geometry.wall_expiry_dispatch",
         "src/game/mario_actions_airborne.c", "act_air_hit_wall",
         "        return FALSE;\n    }\n    return FALSE;", "        return TRUE;\n    }\n    return FALSE;"),
        ("platform", "skip_enemy_box_collision", "platform.box_support",
         "src/game/physics_lab_activity.c", "physics_lab_skip_box_update",
         "|| object_platform_motion_needs_collision(obj)", "|| FALSE"),
        ("jump_kick_wall", "overwrite_saved_provider", "jump_kick_wall.rebuilt_wall",
         "src/game/interaction.c", "same_punch_wall",
         "if (a->object != NULL && a->object == b->object)",
         "if (a->object != NULL && (a->object = b->object) != NULL)"),
        ("jump_kick_wall", "keep_unloaded_provider", "jump_kick_wall.forgotten_provider",
         "src/game/interaction.c", "mario_forget_attack_object",
         "if (sPunchContacts.hitWalls[i].object == obj)", "if (FALSE)"),
        ("twirl", "offset_pound_hitbox", "twirl.offset_hitbox",
         "src/game/mario_ground_twirl.c", "mario_ground_pound_impact",
         "obj->oPosY - obj->hitboxDownOffset", "obj->oPosY"),
        ("twirl", "offset_twirl_hitbox", "twirl.offset_hitbox",
         "src/game/mario_ground_twirl.c", "mario_twirl_push_enemies",
         "obj->oPosY - obj->hitboxDownOffset", "obj->oPosY"),
    ]
    for suite, name, case, path, function, old, new in checks:
        if suite in suites:
            yield suite, name, case, {path: MutationSource(root / path).replace_in_function(function, old, new)}
