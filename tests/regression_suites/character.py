"""Production extraction and deliberate defects for the character suite."""
from .common import prepare_common_constants
from .mutation_support import MutationSource

def prepare_character(root, source, definitions, functions):
    prepare_common_constants(source, definitions)
    options = source("src/game/game_options.h")
    definitions += [options.macros("GAME_OPTION_CATALOG"),
                    options.declaration("enum", "GameOptionId")]
    definitions += ["static u8 sMenuNavigationStick, sMenuNavigationDirection, gMenuHoldKeyIndex;"]
    definitions += [source("src/game/mario_he_thicc.h").macros(
        "HE_THICC_.*|ACT_ARG_WALL_KICK_HE_THICC|ACT_ARG_FORWARD_ROLLOUT_HE_THICC_BOUNCE")]
    definitions += [source("src/game/mario_actions_airborne.h").macros(
        "ACT_ARG_JUMP_BACKWARD_BUTT_SLIDE.*|ACT_ARG_JUMP_KICK_FOLLOWUP_QUEUED|ACT_ARG_JUMP_KICK_BACKWARD_BUTT_SLIDE.*|ACT_ARG_STEEP_JUMP_BACKWARD_BUTT_SLIDE_NO_UPHILL|ACT_ARG_DIVE_FROM_ROLLOUT_CONTACT")]
    definitions += [source("src/game/mario_actions_stationary.h").macros(
        "ACT_ARG_COUGHING_SHALLOW_WATER")]
    definitions += [source("include/config/config_movement.h").macros("BACKWARD_BUTT_SLIDE_JUMP_AIR_DRAG|BACKWARD_JUMP_KICK_UPHILL_DECEL")]
    header = source("src/game/mario_character.h")
    definitions += [header.region(r"^struct MarioState;", r"^#endif")]
    definitions += [source("include/config/config_movement.h").macros(
        "WALK_TARGET_SPEED|JUMP_HEIGHT_VELOCITY_SCALE|JUMP_FORWARD_SPEED_RETENTION")]
    character = source("src/game/mario_character.c")
    functions += [character.region(r"^const struct MarioMovementProfile gMarioMovementProfiles", r"^s32 mario_character_can_unlock")]
    functions += [character.region(r"^void mario_character_commit", r"\Z")]
    menu = source("src/game/ingame_menu.c")
    header = source("src/game/ingame_menu.h")
    definitions += [header.declaration("enum", name) for name in (
        "MenuMode", "MenuMtxPushOp", "HUDLUTs", "SpecialFontChars", "DialogMark",
        "DialogSpecialChars", "DialogResponseDefines")]
    definitions += [header.macros("DIALOG_CHAR_FORWARD_SLASH")]
    definitions += [header.declaration("struct", "DialogEntry")]
    definitions += [menu.declaration("enum", name) for name in (
        "DialogBoxState", "DialogBoxPageState", "DialogBoxType", "MultiStringIDs")]
    definitions += [menu.declaration("struct", "MultiTextEntry"),
                    menu.macros("MAX_STRING_WIDTH|CHAR_WIDTH_SPACE|TEXT_THE_RAW|TEXT_YOU_RAW|X_VAL3|Y_VAL3"),
                    source("src/game/print.h").declaration("enum", "PrintfGlyphs"),
                    source("src/game/print.c").declaration("struct", "TextLabel"),
                    source("src/game/mario_character_text.h").declaration("enum", "MarioTextEncoding")]
    definitions += [menu.region(r"^u8 gDialogCharWidths", r"^s8 gDialogBoxState")]
    definitions += [source("src/game/mario_character_text.h").macros("MARIO_TEXT_SKIP")]
    functions += [source("src/game/mario_character_text.c").function(name) for name in (
        "character_next_glyph", "character_word_continues", "mario_character_text_glyph",
        "character_name_letter", "character_name_matches", "character_named_text",
        "mario_character_text_name_size", "mario_character_text_replace_name")]
    functions += [menu.function(name) for name in (
        "render_multi_text_string", "print_generic_string_styled", "print_generic_string",
        "print_hud_lut_string", "print_menu_generic_string", "print_credits_string",
        "get_string_width", "get_str_x_pos_from_center", "reset_menu_navigation", "create_dialog_box",
        "create_dialog_box_with_var", "create_dialog_inverted_box", "create_dialog_box_with_response",
        "reset_dialog_render_state", "handle_dialog_scroll_page_state",
        "render_star_count_dialog_text", "render_multi_text_string_lines", "handle_dialog_text_and_pages")]
    functions += [source("src/game/print.c").function(name) for name in (
        "char_to_glyph_index", "print_text_set_alpha", "print_text", "print_text_centered", "render_text_labels")]
    functions += [source("src/game/mario_misc.c").function(name) for name in (
        "geo_switch_mario_character", "geo_switch_mario_palette")]

def mutations_character(root):
    checks = [
        ("defer_appearance", "character.live_appearance", "src/game/mario_character.c", "mario_character_apply_appearance", "sActive = sRequested;", "sActive = sLastOrdinary;"),
        ("preview_changes_palette", "character.live_appearance", "src/game/mario_character.c", "mario_character_apply_appearance", "useDefault || sSelections[sRequested].palette != sSelections[sActive].palette", "useDefault"),
        ("keep_final_d", "character.final_d_words", "src/game/mario_character_text.c", "mario_character_text_glyph", "result = MARIO_TEXT_SKIP;", "result = original;"),
        ("drop_medial_d", "character.final_d_words", "src/game/mario_character_text.c", "mario_character_text_glyph", "!character_word_continues(following, encoding)", "(character_word_continues(following, encoding) || TRUE)"),
        ("drop_se_spelling", "character.spelling_cases", "src/game/mario_character_text.c", "mario_character_text_glyph", "if (lowercase && original == lowerE)", "if (FALSE && lowercase && original == lowerE)"),
        ("cascade_spelling", "character.spelling_cases", "src/game/mario_character_text.c", "mario_character_text_glyph", "*previous = original;", "*previous = result;"),
        ("drop_generic_spelling", "character.encoded_renderers", "src/game/ingame_menu.c", "print_generic_string_styled", "render_generic_char(glyph);", "render_generic_char(str[strPos]);"),
        ("lose_dialog_snapshot", "character.dialogue_snapshot_and_pages", "src/game/ingame_menu.c", "create_dialog_box", "sDialogTextStyle = mario_character_text_style();", "sDialogTextStyle = MARIO_TEXT_STANDARD;"),
        ("drop_ascii_spelling", "character.ascii_labels_and_geo", "src/game/print.c", "render_text_labels", "style, MARIO_TEXT_ASCII", "style, MARIO_TEXT_LITERAL"),
    ]
    for name, case, path, function, old, new in checks:
        yield "character", name, case, {path: MutationSource(root / path).replace_in_function(function, old, new)}

