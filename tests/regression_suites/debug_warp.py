"""Production extraction and deliberate defects for the debug_warp suite."""
from .common import prepare_common_constants
import re
from regression_support import HarnessError, closing
from .mutation_support import MutationSource

def debug_warp_metadata(source, behaviors):
    """Read entrance metadata, following local sub-scripts and preserving act masks."""
    result = []
    entries = re.findall(r'^DEFINE_LEVEL\([^,]+,\s*(LEVEL_\w+),[^,]+,\s*(\w+),',
                         source("levels/level_defines.h").mask, re.M)
    for level, folder in entries:
        if level == "LEVEL_ENDING":
            continue  # The cake uses the separately tested end-screen script.
        script = source(f"levels/{folder}/script.c")
        arrays = {}
        for match in re.finditer(r'\bLevelScript\s+(\w+)\[\]\s*=\s*\{', script.mask):
            start = match.end() - 1
            end = closing(script.mask, start, "{", "}")
            arrays[match[1]] = script.mask[start + 1:end - 1]
        area = None
        default = None
        areas, objects, nodes = set(), [], []

        def visit(name, stack=()):
            nonlocal area, default
            if name in stack:
                raise HarnessError(f"{script.path}: recursive entrance sub-script {name}")
            if name not in arrays:
                if name.startswith("script_func_global_"):
                    return  # These shared scripts load models; they place no warp objects.
                raise HarnessError(f"{script.path}: unsupported entrance sub-script {name}")
            text = arrays[name]
            for match in re.finditer(r'\b(AREA|END_AREA|MARIO_POS|OBJECT|OBJECT_WITH_ACTS|'
                                     r'WARP_NODE|JUMP_LINK)\s*\(', text):
                start = match.end() - 1
                end = closing(text, start, "(", ")")
                args = [arg.strip() for arg in text[start + 1:end - 1].split(",")]
                command = match[1]
                if command == "AREA":
                    area = int(args[0], 0)
                    areas.add(area)
                elif command == "END_AREA":
                    area = None
                elif command == "MARIO_POS":
                    default = int(args[0], 0)
                elif command == "JUMP_LINK":
                    visit(args[0], stack + (name,))
                elif command == "WARP_NODE":
                    nodes.append((area, int(args[0], 0)))
                elif args[8] in behaviors:
                    mask = 63
                    if command == "OBJECT_WITH_ACTS" and args[9] != "ALL_ACTS":
                        acts = re.findall(r'ACT_([1-6])', args[9])
                        if not acts or re.sub(r'ACT_[1-6]|[()|\s]', '', args[9]):
                            raise HarnessError(f"{script.path}: unsupported warp act mask {args[9]}")
                        mask = sum(1 << (int(act) - 1) for act in set(acts))
                    objects.append((area, int(args[7], 0), args[8], mask))

        visit(f"level_{folder}_entry")
        if default not in areas:
            raise HarnessError(f"{script.path}: no valid authored MARIO_POS")
        result.append((level, default, sorted(areas), objects, nodes))
    return result

def prepare_debug_warp(root, source, definitions, functions):
    prepare_common_constants(source, definitions)
    definitions += [source("src/game/game_options.h").region(r"^#define GAME_OPTIONS_FILES", r"^#endif"),
                    "static u8 sMenuNavigationStick, sMenuNavigationDirection, sNavigationPolicy = 2, sStickReleasePolicy;",
                    "u8 game_options_get(enum GameOptionId id) { return id == GO_MENU_NAVIGATION ? sNavigationPolicy : id == GO_MENU_STICK_RELEASE ? sStickReleasePolicy : 0; }"]
    functions += [source("src/game/game_options.c").function(name) for name in
                  ("game_options_menu_direction", "game_options_menu_stick", "game_options_menu_dpad")]
    definitions += [source("include/sm64.h").macros(
        "ACT_WATER_IDLE|ACT_BREASTSTROKE|ACT_FLUTTER_KICK|ACT_WATER_SHELL_SWIMMING|"
        "ACT_HOLD_WATER_IDLE|ACT_METAL_WATER_WALKING")]
    definitions += [source("include/sm64.h").declaration("enum", "MarioActionFlags")]
    definitions += [source("src/engine/math_util.h").macros("MIN|MAX")]
    definitions += [source("src/game/mario_he_thicc.h").macros(
        "HE_THICC_.*|ACT_ARG_WALL_KICK_HE_THICC|ACT_ARG_FORWARD_ROLLOUT_HE_THICC_BOUNCE")]
    definitions += [source("src/game/mario_actions_airborne.h").macros(
        "ACT_ARG_JUMP_BACKWARD_BUTT_SLIDE.*|ACT_ARG_JUMP_KICK_FOLLOWUP_QUEUED|ACT_ARG_JUMP_KICK_BACKWARD_BUTT_SLIDE.*|ACT_ARG_STEEP_JUMP_BACKWARD_BUTT_SLIDE_NO_UPHILL|ACT_ARG_DIVE_FROM_ROLLOUT_CONTACT")]
    definitions += [source("src/game/mario_actions_stationary.h").macros(
        "ACT_ARG_COUGHING_SHALLOW_WATER")]
    definitions += [source("include/config/config_movement.h").macros("BACKWARD_BUTT_SLIDE_JUMP_AIR_DRAG|BACKWARD_JUMP_KICK_UPHILL_DECEL")]
    header = source("src/game/level_update.h")
    definitions += [header.declaration("enum", name) for name in
                    ("WarpTypes", "WarpFlags", "MarioSpawnType", "WarpNodes", "WarpOperation",
                     "SpecialWarpDestinations", "PlayModes")]
    definitions += [source("include/config/config_game.h").macros("AREA_COUNT")]
    levels = re.findall(r"^(?:DEFINE_LEVEL|STUB_LEVEL)\([^,]+,\s*(LEVEL_\w+)",
                        source("levels/level_defines.h").mask, re.M)
    definitions += ["enum { LEVEL_NONE, " + ", ".join(levels) + ", LEVEL_COUNT };"]
    definitions += [source("include/level_select.h").region(r"^static const u8 sLevelSelectOrder", r"^#endif")]
    tables = source("src/game/area.c").region(r"^const BehaviorScript \*sWarpBhvSpawnTable", r"^Vp gViewport")
    behaviors = sorted(set(re.findall(r'\bbhv\w+', tables)))
    definitions += [f"static const BehaviorScript {behavior}[] = {{ 0 }};" for behavior in behaviors]
    definitions += [tables]
    functions += [source("src/game/area.c").function("get_mario_spawn_type_from_behavior")]
    functions += [source("src/game/puppyprint.c").region(
        r"^#define DEBUG_DPAD_REPEAT_DELAY_FRAMES", r"^static void puppyprint_update_level_select")]
    functions += [source("src/game/puppyprint.c").function("puppyprint_update_level_select")]
    definitions += [source("src/game/puppyprint.h").declaration("enum", "PPPages")]
    definitions += [source("src/game/mario_character.h").macros("MARIO_CHARACTER_COUNT")]
    definitions += [source("include/config/config_menu.h").macros("MENU_SCROLL_REPEAT_.*|DEBUG_DPAD_REPEAT_.*")]
    definitions += [source("src/game/ingame_menu.h").declaration("enum", "MenuScrollAxis")]
    definitions += [source("src/game/ingame_menu.h").declaration("enum", "MenuMode")]
    sounds = source("include/sounds.h")
    definitions += [sounds.declaration("enum", "SoundBank"), sounds.macros(
        "SOUND_BANK_COINS|SOUND_ARG_LOAD|SOUNDARGS_SHIFT_.*|SOUNDARGS_MASK_BANK|"
        "SOUND_MENU_STYLE|SOUND_DISCRETE|SOUND_STATUS_WAITING|SOUND_MENU_CHANGE_SELECT|"
        "SOUND_MENU_PAUSE_CHANGE_SELECT|SOUND_MENU_PAUSE_OPEN|SOUND_MENU_PAUSE_CLOSE")]
    functions += [source("src/game/ingame_menu.c").function(name) for name in
                  ("menu_scrolling_input", "handle_menu_scrolling")]
    functions += [source("src/game/puppyprint.c").function("puppyprint_profiler_process")]
    functions += [source("src/game/level_update.c").function(name) for name in
                  ("initiate_warp", "debug_warp_entry_node", "resolve_debug_warp_destination",
                   "warp_level", "init_level")]
    setup = ["static void load_shipped_metadata(s32 level, s32 act) {",
             "    clear_metadata();", "    switch (level) {"]
    for level, default, areas, objects, nodes in debug_warp_metadata(source, behaviors):
        setup += [f"        case {level}:", f"            gPlayerSpawnInfos[0].areaIndex = {default};"]
        setup += [f"            define_area({area});" for area in areas]
        setup += [f"            add_node({area}, {node});" for area, node in nodes]
        setup += [f"            if ((1 << (act - 1)) & {mask}) {{ "
                  f"add_spawn({area}, {arg}u, {behavior}); }}" for area, arg, behavior, mask in objects]
        setup += ["            break;"]
    setup += ["        default: abort();", "    }", "}"]
    functions += ["\n".join(setup)]

def mutations_debug_warp(root):
    path = "src/game/game_options.c"
    yield "debug_warp", "ignore_menu_navigation_policy", "debug_warp.menu_scroll_timing", {
        path: MutationSource(root / path).replace_in_function("game_options_menu_dpad",
            "return game_options_get(GO_MENU_NAVIGATION) == 0 ? 0 : buttons;", "return buttons;")}
    for name, case, path, function, old, new in (
        ("debug_no_held_repeat", "selector_held_navigation", "src/game/puppyprint.c",
         "puppyprint_debug_dpad_buttons", "pressed |= direction;", "pressed |= 0;"),
        ("debug_slow_held_repeat", "selector_held_navigation", "src/game/puppyprint.c",
         "puppyprint_debug_dpad_buttons",
         "sDebugRepeatTimer = DEBUG_DPAD_REPEAT_INTERVAL_FRAMES;",
         "sDebugRepeatTimer = DEBUG_DPAD_REPEAT_INTERVAL_FRAMES + 1;"),
        ("debug_four_destinations", "selector_destinations", "include/level_select.h",
         "level_select_warp_count", "return 1;", "return 4;"),
        ("debug_ttm_empty_area", "selector_destinations", "include/level_select.h",
         "level_select_warp_area", "return 4;", "return 3;"),
        ("debug_keep_invalid_choice", "selector_navigation", "src/game/puppyprint.c",
         "puppyprint_update_level_select", "sLevelSelectOptionWarp = 0;",
         "sLevelSelectOptionWarp = sLevelSelectOptionWarp;"),
    ):
        yield "debug_warp", name, "debug_warp." + case, {
            path: MutationSource(root / path).replace_in_function(function, old, new)}
    path = "src/game/level_update.c"
    for name, case, function, old, new in (
        ("debug_missing_init_hook", "shipped_entrances", "init_level",
         "resolve_debug_warp_destination();", "(void) resolve_debug_warp_destination;"),
        ("debug_forced_main_node", "shipped_entrances", "resolve_debug_warp_destination",
         "sWarpDest.nodeId = node;", "sWarpDest.nodeId = WARP_NODE_MAIN_ENTRY;"),
        ("debug_ignore_other_entrances", "boundaries", "debug_warp_entry_node",
         "fallbackNode = id;", "fallbackNode = -1;"),
        ("debug_death_entrance", "boundaries", "debug_warp_entry_node",
         "type == MARIO_SPAWN_DEATH", "FALSE"),
        ("debug_same_area_reload", "shipped_entrances", "initiate_warp",
         "// Reload even the current level so debug entrance validation always runs.\n"
         "        sWarpDest.type = WARP_TYPE_CHANGE_LEVEL;",
         "// Incorrectly bypass the destination initialization hook.\n"
         "        sWarpDest.type = WARP_TYPE_SAME_AREA;"),
    ):
        yield "debug_warp", name, "debug_warp." + case, {
            path: MutationSource(root / path).replace_in_function(function, old, new)}
