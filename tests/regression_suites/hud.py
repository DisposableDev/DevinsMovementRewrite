"""Production extraction and deliberate defects for the hud suite."""
from .common import prepare_common_constants
from .mutation_support import MutationSource

def prepare_hud(root, source, definitions, functions):
    prepare_common_constants(source, definitions)
    definitions += [source("src/game/mario_he_thicc.h").macros(
        "HE_THICC_.*|ACT_ARG_WALL_KICK_HE_THICC|ACT_ARG_FORWARD_ROLLOUT_HE_THICC_BOUNCE")]
    definitions += [source("src/game/mario_actions_airborne.h").macros(
        "ACT_ARG_JUMP_BACKWARD_BUTT_SLIDE.*|ACT_ARG_JUMP_KICK_FOLLOWUP_QUEUED|ACT_ARG_JUMP_KICK_BACKWARD_BUTT_SLIDE.*|ACT_ARG_STEEP_JUMP_BACKWARD_BUTT_SLIDE_NO_UPHILL|ACT_ARG_DIVE_FROM_ROLLOUT_CONTACT")]
    definitions += [source("src/game/mario_actions_stationary.h").macros(
        "ACT_ARG_COUGHING_SHALLOW_WATER")]
    definitions += [source("include/config/config_movement.h").macros("BACKWARD_BUTT_SLIDE_JUMP_AIR_DRAG|BACKWARD_JUMP_KICK_UPHILL_DECEL")]
    hud = source("src/game/hud.c")
    definitions += ["enum { GO_HUD, GO_ASPECT_RATIO, GO_HUD_WIDE, GO_FLASHING };",
                    "static s32 sReducedFlashing;",
                    "static s32 game_options_get(s32 id) { return id == GO_HUD ? 1 : id == GO_FLASHING ? sReducedFlashing : 0; }"]
    functions += [source("src/game/game_options.c").function("game_options_hud_inset")]
    definitions += [source("src/game/hud.h").macros("HUD_STARS_X|HUD_TOP_Y|HUD_POWER_METER_X")]
    definitions += [source("include/sm64.h").declaration("enum", "MarioFlags")]
    definitions += [hud.macros("HUD_IDLE_UPDATES|HUD_REVEAL_UPDATES|HUD_FADE_UPDATES|HUD_FADE_STEP|HUD_CAP_.*")]
    functions += [hud.function(name) for name in (
        "hud_update_cap_timer", "hud_approach_alpha", "hud_reset_auto_visibility", "hud_reveal_counters",
        "hud_reveal_cap_timer", "hud_reveal_after_pause", "hud_update_auto_visibility", "set_hud_camera_status",
        "render_hud_tex_lut", "render_hud_small_tex_lut",
        "render_hud_cap_timer", "render_hud_stars")]
    functions += [source("src/game/print.c").function("print_text_set_alpha")]
    functions += [source("src/game/print.c").function("render_textrect")]

def mutations_hud(root):
    path = "src/game/hud.c"
    yield "hud", "hide_flashing_star_counter", "hud.star_counter_blink", {
        path: MutationSource(root / path).replace_in_function("render_hud_stars",
            "blinkAlpha = MIN(alpha, OBJECT_BLINK_MIN_ALPHA);", "return;")}
    yield "hud", "leak_star_blink_alpha", "hud.star_counter_blink", {
        path: MutationSource(root / path).replace_in_function("render_hud_stars",
            "print_text_set_alpha(alpha);", "print_text_set_alpha(blinkAlpha);")}

