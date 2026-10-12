"""Production extraction and deliberate defects for the end_screen suite."""
from .common import prepare_common_constants
import re
from .mutation_support import MutationSource

def prepare_end_screen(root, source, definitions, functions):
    prepare_common_constants(source, definitions)
    definitions += [source("include/n64/PR/gu.h").macros("MAX")]
    definitions += [source("include/n64/PR/os_cont.h").macros("CONT_.*|B_BUTTON|U_JPAD|D_JPAD|L_JPAD|R_JPAD|L_TRIG|R_TRIG|Z_TRIG")]
    definitions += [source("src/game/game_options.h").region(r"^#define GAME_OPTIONS_FILES", r"^#endif")]
    definitions += [source("src/game/game_options_menu.h").region(r"^enum OptionsMenuButton", r"^#endif")]
    definitions += [source("src/game/game_options_menu.c").macros("OPTIONS_MENU_DIRECTIONS"),
                    source("include/config/config_menu.h").macros("MENU_SCROLL_REPEAT_.*")]
    definitions += [source("src/game/game_timing.h").declaration("struct", "GameTimingState")]
    functions += [
        "static struct GameTimingState sTimingService;",
        "const struct GameTimingState *game_timing_state(void) { return &sTimingService; }",
        "s32 game_timing_checkpoint(s32 file) { (void) file; return TRUE; }",
        "void game_timing_end_course(void) {}",
        "void game_timing_runtime_star(struct Object *obj, u32 star) { (void) obj; (void) star; }",
    ]
    functions += [source("src/game/game_options.c").region(r"^#define OPTIONS_MAGIC", r"\Z")]
    functions += [source("src/game/game_options_runtime.c").function(name) for name in
                  ("game_options_is_cheating", "game_options_secrets_blocked",
                   "game_options_runtime_restart")]
    functions += [source("src/game/game_options_menu.c").region(r"^struct GameOptionsMenuState gOptionsMenu", r"\Z")]
    functions += [source("src/game/game_options_menu_render.c").function(name) for name in
                  ("game_options_menu_update_controller",)]
    definitions += [source("src/game/mario_he_thicc.h").macros(
        "HE_THICC_.*|ACT_ARG_WALL_KICK_HE_THICC|ACT_ARG_FORWARD_ROLLOUT_HE_THICC_BOUNCE")]
    definitions += [source("src/game/mario_actions_airborne.h").macros(
        "ACT_ARG_JUMP_BACKWARD_BUTT_SLIDE.*|ACT_ARG_JUMP_KICK_FOLLOWUP_QUEUED|ACT_ARG_JUMP_KICK_BACKWARD_BUTT_SLIDE.*|ACT_ARG_STEEP_JUMP_BACKWARD_BUTT_SLIDE_NO_UPHILL|ACT_ARG_DIVE_FROM_ROLLOUT_CONTACT")]
    definitions += [source("src/game/mario_actions_stationary.h").macros(
        "ACT_ARG_COUGHING_SHALLOW_WATER")]
    definitions += [source("include/config/config_movement.h").macros("BACKWARD_BUTT_SLIDE_JUMP_AIR_DRAG|BACKWARD_JUMP_KICK_UPHILL_DECEL")]
    header = source("src/game/level_update.h")
    definitions += [header.macros("WARP_FLAG_YOSHI_COIN_CAKE|WARP_FLAG_CASTLE_BOARD_CAKE|WARP_FLAG_SECRET_LOBBY_CAKE")]
    definitions += ["static s32 sYoshiCoinCakeActive, sCastleBoardCakeActive, sSecretLobbyCakeActive;"]
    definitions += [header.declaration("enum", name) for name in
                    ("WarpTypes", "WarpOperation", "WarpFlags", "PlayModes", "WarpNodes",
                     "SpecialWarpDestinations")]
    levels = re.findall(r"^(?:DEFINE_LEVEL|STUB_LEVEL)\([^,]+,\s*(LEVEL_\w+)",
                        source("levels/level_defines.h").mask, re.M)
    definitions += ["enum { LEVEL_NONE, " + ", ".join(levels) + " };"]
    sm64 = source("include/sm64.h")
    definitions += [sm64.declaration("enum", name) for name in
                    ("MarioActionGroups", "MarioActionFlags")]
    definitions += [sm64.macros("ACT_.*")]
    definitions += [source("src/game/mario_actions_cutscene.h").declaration(
        "enum", "ActionStatesEndPeachCutsceneFadeOut")]
    definitions += [sm64.declaration("enum", "MarioFlags")]
    definitions += [source("src/game/interaction.h").declaration("enum", name) for name in
                    ("InteractSubtypes", "InteractStatus")]
    menu = source("src/game/ingame_menu.c")
    definitions += [menu.macros("PAUSE_MENU_UNPAUSE_DELAY_FRAMES|X_VAL8|Y_VAL8")]
    functions += [menu.function("clear_cutscene_message")]
    functions += [source("src/game/level_update.c").function(name) for name in (
        "level_is_ending", "pressed_pause", "warp_special", "fade_into_special_warp",
        "level_warp_from_ending", "play_mode_paused", "play_mode_change_level",
        "initiate_delayed_warp", "lvl_end_screen", "lvl_play_the_end_screen_sound")]
    definitions += [source("src/game/level_update.c").macros("END_SCREEN_IDLE_FRAMES")]
    functions += [source("src/game/mario_actions_cutscene.c").function("end_peach_cutscene_fade_out")]
    functions += [source("src/game/interaction.c").region(
        r"^u32 interact_star_or_key\(struct MarioState \*m, UNUSED", r"^u32 interact_bbh_entrance\(struct MarioState \*m, UNUSED")]
    functions += [menu.function(name) for name in (
        "reset_menu_navigation", "reset_pause_menu_navigation", "do_cutscene_handler",
        "render_pause_course_options", "render_pause_courses_and_castle")]
    functions += [source("levels/ending/script.c").region(
        r"^static const LevelScript level_ending_display", r"\Z")]

def mutations_end_screen(root):
    for name, case, path, function, old, new in (
        ("cake_keep_incoming_request", "debug_departure", "src/game/level_update.c", "lvl_end_screen",
         "gPuppyWarp = 0;", "gPuppyWarp = gPuppyWarp;"),
        ("cake_ignore_next_selection", "debug_departure", "src/game/level_update.c", "lvl_end_screen",
         "else if (gPuppyWarp)", "else if (FALSE)"),
        ("cake_in_area_reload", "debug_departure", "src/game/level_update.c", "lvl_end_screen",
         "sWarpDest.type = WARP_TYPE_CHANGE_LEVEL;", "sWarpDest.type = WARP_TYPE_SAME_AREA;"),
        ("cake_endless", "automatic_departure", "src/game/level_update.c", "lvl_end_screen",
         "++sEndScreenTimer >= END_SCREEN_IDLE_FRAMES", "++sEndScreenTimer < 0"),
        ("cake_early_end", "automatic_departure", "src/game/level_update.c", "lvl_end_screen",
         "++sEndScreenTimer >= END_SCREEN_IDLE_FRAMES", "++sEndScreenTimer >= END_SCREEN_IDLE_FRAMES - 1"),
        ("cake_timer_not_reset", "pause_countdown", "src/game/level_update.c", "lvl_end_screen",
         "sEndScreenTimer = 0;", "sEndScreenTimer = sEndScreenTimer;"),
        ("cake_volume_not_restored", "automatic_departure", "src/game/level_update.c", "lvl_end_screen",
         "if (changeLevel) {", "if (FALSE) {"),
        ("ending_pause_locked", "pause_policy", "src/game/level_update.c", "pressed_pause",
         "|| level_is_ending()", ""),
        ("ending_flight_pause", "pause_policy", "src/game/level_update.c", "level_is_ending",
         "gMarioState->action == ACT_END_PEACH_CUTSCENE", "(gMarioState->action & ACT_GROUP_MASK) == ACT_GROUP_CUTSCENE"),
        ("ending_debug_exit", "peach_departure", "src/game/level_update.c", "play_mode_paused",
         "if (level_is_ending()) {", "if (FALSE) {"),
        ("ending_credits_state_leak", "peach_departure", "src/game/level_update.c", "level_warp_from_ending",
         "gCurrCreditsEntry = NULL;", "gCurrCreditsEntry = gCurrCreditsEntry;"),
        ("ending_wrong_lab_area", "peach_departure", "src/game/level_update.c", "level_warp_from_ending",
         "initiate_warp(LEVEL_PHYSICS_LAB, 1,", "initiate_warp(LEVEL_PHYSICS_LAB, 2,"),
        ("credits_terminal_route", "credits_cake_route", "src/game/level_update.c", "initiate_delayed_warp",
         "initiate_warp(LEVEL_ENDING, 1, WARP_NODE_MAIN_ENTRY, WARP_FLAGS_NONE);", "warp_special(WARP_SPECIAL_ENDING);"),
        ("ending_caption_advances_paused", "menu_captions", "src/game/ingame_menu.c", "do_cutscene_handler",
         " || (level_is_ending() && sCurrPlayMode == PLAY_MODE_PAUSED)", ""),
        ("ending_menu_camera", "menu_captions", "src/game/ingame_menu.c", "render_pause_courses_and_castle",
         "if (!ending) {\n                change_dialog_camera_angle();", "if (TRUE) {\n                change_dialog_camera_angle();"),
        ("ending_menu_camera_option", "menu_captions", "src/game/ingame_menu.c", "render_pause_course_options",
         "ending ? MENU_OPT_EXIT_COURSE : MENU_OPT_CAMERA_ANGLE_R", "MENU_OPT_CAMERA_ANGLE_R"),
    ):
        yield "end_screen", name, "end_screen." + case, {
            path: MutationSource(root / path).replace_in_function(function, old, new)}
    path = "levels/ending/script.c"
    text = MutationSource(root / path)
    old = "CLEAR_LEVEL(),"
    yield "end_screen", "cake_skip_cleanup", "end_screen.automatic_departure", {
        path: text.replace_unique(old, "SLEEP(1),")}
