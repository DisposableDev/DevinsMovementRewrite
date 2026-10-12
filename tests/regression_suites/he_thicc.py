"""Production extraction and deliberate defects for the he_thicc suite."""
from .common import prepare_common_constants
from regression_support import HarnessError
from .mutation_support import MutationSource

def prepare_he_thicc(root, source, definitions, functions):
    pose = source("src/game/mario_render_pose.h")
    definitions += [pose.declaration("enum", "MarioPoseRootSpace"),
                    pose.declaration("struct", "MarioRenderPose")]
    prepare_common_constants(source, definitions, gameplay_options=True)
    definitions.remove("#define player_stats_he_thicc_event(...) ((void) 0)")
    definitions += [source("src/game/player_stats.h").declaration("enum", "PlayerStatId")]
    definitions += [source("include/config/config_movement.h").macros("WALL_BONK_MOMENTUM_RETENTION")]
    functions += [source("src/game/mario_step.c").function("mario_set_wall_bonk_momentum")]
    functions += ["s32 mario_ground_pound_animation_render(struct AnimInfo *info, "
                  "struct MarioState *m, struct MarioRenderPose *pose) { return FALSE; }"]
    definitions += [source("src/game/mario_he_thicc.h").macros(
        "HE_THICC_.*|ACT_ARG_WALL_KICK_HE_THICC|ACT_ARG_FORWARD_ROLLOUT_HE_THICC_BOUNCE")]
    definitions += [source("src/game/mario_actions_airborne.h").macros(
        "ACT_ARG_JUMP_BACKWARD_BUTT_SLIDE.*|ACT_ARG_JUMP_KICK_FOLLOWUP_QUEUED|ACT_ARG_JUMP_KICK_BACKWARD_BUTT_SLIDE.*|ACT_ARG_STEEP_JUMP_BACKWARD_BUTT_SLIDE_NO_UPHILL|ACT_ARG_DIVE_FROM_ROLLOUT_CONTACT")]
    definitions += [source("src/game/mario_actions_stationary.h").macros(
        "ACT_ARG_COUGHING_SHALLOW_WATER")]
    definitions += [source("include/config/config_movement.h").macros("BACKWARD_BUTT_SLIDE_JUMP_AIR_DRAG|BACKWARD_JUMP_KICK_UPHILL_DECEL")]
    definitions += [source("include/model_ids.h").macros("MODEL_EXPLOSION")]
    definitions += [source("src/game/mario_dab.h").macros("DAB_ANALOG_.*")]
    definitions += [source("include/types.h").macros("ANIMINDEX_NUMPARTS")]
    definitions += [source("include/config/config_movement.h").macros("MARIO_STRONG_STICK_THRESHOLD")]
    definitions += [source("src/game/mario_actions_airborne.h").macros(
        "ROLLOUT_WATER_WALK_DELAY_FRAMES|FORWARD_ROLLOUT_WALL_KICK_DELAY_FRAMES|ACT_ARG_FORWARD_ROLLOUT_.*")]
    sounds = source("include/sounds.h")
    definitions += [sounds.declaration("enum", "SoundBank")]
    definitions += [sounds.macros(
        "SOUND_ARG_LOAD|SOUNDARGS_.*|SOUND_HE_THICC_.*|SOUND_BANK_DETAILS|"
        "SOUND_NO_PRIORITY_LOSS|SOUND_DISCRETE|SOUND_STATUS_WAITING")]
    table = source("sound/sequences/00_sound_player.s").region(
        r"^\.channel38_table:", r"^\.sound_general_activate_cap_switch:")
    references = [line.split()[1] for line in table.splitlines()
                  if line.startswith("sound_ref ")]
    if len(references) <= 0x86 or references[0x86] != ".sound_action_clap_hands_cold":
        raise HarnessError("HE THICC clap is missing from sound table entry 0x86")
    definitions += [source("include/sm64.h").macros("ACT_.*")]
    definitions += [source("include/sm64.h").declaration("enum", name) for name in (
        "MarioActionGroups", "MarioActionFlags", "MarioInput", "MarioParticleFlags",
        "AirStep", "AirStepCheck", "MarioFlags")]
    definitions += [source("src/game/mario_character.h").declaration("enum", "MarioPalette")]
    definitions += [source("include/surface_terrains.h").declaration("enum", "SurfaceTypes")]
    definitions += [source("include/surface_terrains.h").macros("SURFACE_IS_.*|INSTANT_WARP_INDEX_.*")]
    definitions += [source("include/mario_animation_ids.h").declaration("enum", "MarioAnimID")]
    definitions += [source("include/types.h").declaration("struct", "Animation")]
    definitions += [source("include/types.h").declaration("struct", "AnimInfo")]
    definitions += [source("include/types.h").declaration("enum", "AnimFlags")]
    dab_asset = source("assets/anims/anim_D1.inc.c")
    dab_split = dab_asset.mask.index("static const u16 anim_D1_indices")
    definitions += [dab_asset.mapped(dab_split, len(dab_asset.text)),
                    '#pragma GCC diagnostic push\n#pragma GCC diagnostic ignored "-Wmissing-braces"',
                    dab_asset.mapped(0, dab_split), '#pragma GCC diagnostic pop']
    definitions += [source("src/game/mario_actions_stationary.h").declaration(
        "enum", name) for name in ("ActionStatesGroundPoundLand", "ActionStatesIdle", "ActionStatesStartSleeping")]
    definitions += [source("include/surface_terrains.h").declaration("enum", name)
                    for name in ("SurfaceFlags", "TerrainType")]
    definitions += [source("include/config/config_movement.h").macros("IDLE_(SLEEP|SHIVER)_.*")]
    definitions += [source("src/game/mario_actions_submerged.h").declaration("enum", name)
                    for name in ("ActionStatesBreaststroke", "ActionArgsFlutterKick")]
    definitions += [source("include/config/config_movement.h").macros(
        "MIN_SWIM_SPEED|SWIM_SOFT_SPEED_LIMIT|BREASTSTROKE_.*|SWIM_DRAG")]
    definitions += [source("src/game/mario_actions_submerged.c").macros(
        "MIN_SWIM_STRENGTH|MAX_SWIM_STRENGTH|BREASTSTROKE_.*")]

    definitions += [source("src/game/rendering_graph_node.h").declaration("enum", "AnimType")]
    functions += [source("src/game/mario_he_thicc.c").region(
        r"^#define HE_THICC_WAVEDASH_HEIGHT", r"\Z")]
    functions += [source("src/engine/graph_node.c").function("retrieve_animation_index")]
    functions += [source("src/game/mario_actions_stationary.c").function("act_ground_pound_land")]
    functions += [source("src/game/mario_actions_stationary.c").function("act_idle")]
    functions += [source("src/game/mario_actions_submerged.c").function(name)
                  for name in ("act_breaststroke", "act_hold_breaststroke")]

    functions += [source("src/game/mario_actions_airborne.c").function("act_wall_kick_air")]
    functions += [source("src/game/mario_actions_airborne.c").function("check_fall_damage_or_get_stuck")]
    functions += [source("src/game/mario_actions_moving.c").function("play_step_sound")]
    functions += [source("src/game/mario_actions_airborne.c").function("act_forward_rollout")]
    functions += [source("src/game/rendering_graph_node.c").function("geo_set_animation_globals")]

def mutations_he_thicc(root):
    path = "src/game/mario_he_thicc.c"
    for name, case, function, old, new in (
        ("belly_any_air_move", "belly_landings", "mario_he_thicc_try_belly_landing",
         "m->action != ACT_DIVE", "FALSE"),
        ("belly_backward", "belly_landings", "mario_he_thicc_try_belly_landing",
         "m->forwardVel <= 0.0f", "FALSE"),
        ("belly_too_low", "belly_landings", "mario_he_thicc_try_belly_landing",
         "m->peakHeight - m->pos[1] < HE_THICC_BELLY_SLAM_HEIGHT", "FALSE"),
        ("fixed_movement_dab", "movement_dabs", "mario_he_thicc_render_pose",
         "he_thicc_animate_dab(info, frame);", "(void) he_thicc_animate_dab;"),
        ("hide_tired_pose", "movement_dabs", "mario_he_thicc_render_pose",
         "if (!he_thicc_uses_native_upper_body(info->animID)",
         "if ((void) he_thicc_uses_native_upper_body, TRUE"),
        ("dab_running", "native_movement_pose", "he_thicc_uses_native_upper_body",
         "        case MARIO_ANIM_RUNNING:\n", ""),
        ("dab_idle", "native_movement_pose", "he_thicc_uses_native_upper_body",
         "        case MARIO_ANIM_IDLE_HEAD_LEFT:\n", ""),
        ("dab_standard_jump", "native_movement_pose", "he_thicc_uses_native_upper_body",
         "        case MARIO_ANIM_SINGLE_JUMP:\n", ""),
        ("no_running_thump", "running_audio", "mario_he_thicc_running_step",
         "play_sound(SOUND_HE_THICC_RUNNING_STEP, m->marioObj->header.gfx.cameraToObject);", "(void) m;"),
        ("punch_blast_tangible", "move_feedback", "mario_he_thicc_attack_impact",
         "explosion->oIntangibleTimer = -1;", "explosion->oIntangibleTimer = 0;"),
        ("no_bounce_wave", "move_feedback", "mario_he_thicc_slide_kick_bounce",
         "mario_ground_pound_impact(m);", "(void) m;"),
        ("thicc_standard_leak", "wavedash_chain", "he_thicc_is_active",
         "== MARIO_PALETTE_HE_THICC", "== MARIO_PALETTE_STANDARD"),
        ("wave_upward", "wavedash_chain", "mario_he_thicc_try_wavedash",
         "m->vel[1] > 0.0f", "FALSE"),
        ("belly_held_jump", "belly_landings", "mario_he_thicc_belly_slide",
         "if (m->input & INPUT_A_PRESSED)", "if (m->input & INPUT_A_DOWN)"),
        ("late_belly_boost", "belly_landings", "mario_he_thicc_belly_slide",
         "m->actionTimer < HE_THICC_BELLY_JUMP_FRAMES", "TRUE"),
        ("late_pound_bounce", "pound_and_wall", "mario_he_thicc_try_pound_bounce",
         "m->actionTimer >= HE_THICC_BELLY_JUMP_FRAMES", "FALSE"),
        ("ignore_inverse_dab", "inverse_dab_pose", "mario_he_thicc_render_pose",
         "info->animID != MARIO_ANIM_DAB", "TRUE"),
        ("dab_missing", "dab_pose", "mario_he_thicc_render_pose",
         "sHeThicc.pose[15 + i] = dabArms[i];", "(void) dabArms;"),
        ("spring_never_decays", "jiggle_and_clap", "mario_he_thicc_update",
         "* 0.78f", "* 1.0f"),
        ("squash_scale_accumulates", "jiggle_and_clap", "mario_he_thicc_update",
         "m->squishTimer == 0 && m->action != ACT_SQUISHED", "TRUE"),
    ):
        yield "he_thicc", name, "he_thicc." + case, {
            path: MutationSource(root / path).replace_in_function(function, old, new)}


    for name, case, path, function, old, new in (
        ("early_tired", "idle_tired", "src/game/mario_actions_stationary.c", "act_idle",
         "sleepDelay = HE_THICC_IDLE_SLEEP_FRAMES;", "sleepDelay = 90;"),
        ("late_tired", "idle_tired", "src/game/mario_actions_stationary.c", "act_idle",
         "sleepDelay = HE_THICC_IDLE_SLEEP_FRAMES;", "sleepDelay = 92;"),
        ("no_free_stroke_ring", "breaststroke_rings", "src/game/mario_actions_submerged.c", "act_breaststroke",
         "mario_he_thicc_breaststroke(m);", "(void) m;"),
        ("no_held_stroke_ring", "breaststroke_rings", "src/game/mario_actions_submerged.c", "act_hold_breaststroke",
         "mario_he_thicc_breaststroke(m);", "(void) m;"),
    ):
        yield "he_thicc", name, "he_thicc." + case, {
            path: MutationSource(root / path).replace_in_function(function, old, new)}

