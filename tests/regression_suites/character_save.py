"""Production extraction and deliberate defects for the character_save suite."""
from .common import prepare_common_constants
import re
import json
from .mutation_support import MutationSource

def prepare_character_save(root, source, definitions, functions):
    prepare_common_constants(source, definitions)
    definitions += ["static u8 sMenuNavigationStick, sMenuNavigationDirection, gMenuHoldKeyIndex;"]
    definitions += [source("src/game/game_options.h").region(
        r"^#define GAME_OPTIONS_FILES", r"^#endif")]
    functions += [source("src/game/game_options.c").region(r"^#define OPTIONS_MAGIC", r"\Z")]
    definitions += [source("src/game/mario_he_thicc.h").macros(
        "HE_THICC_.*|ACT_ARG_WALL_KICK_HE_THICC|ACT_ARG_FORWARD_ROLLOUT_HE_THICC_BOUNCE")]
    definitions += [source("include/object_constants.h").macros("BOO_DEATH_STATUS_.*")]
    definitions += [source("src/game/object_list_processor.h").declaration("enum", "ObjectList")]
    definitions += [source("src/game/mario_actions_airborne.h").macros(
        "ACT_ARG_JUMP_BACKWARD_BUTT_SLIDE.*|ACT_ARG_JUMP_KICK_FOLLOWUP_QUEUED|ACT_ARG_JUMP_KICK_BACKWARD_BUTT_SLIDE.*|ACT_ARG_STEEP_JUMP_BACKWARD_BUTT_SLIDE_NO_UPHILL|ACT_ARG_DIVE_FROM_ROLLOUT_CONTACT")]
    definitions += [source("src/game/mario_actions_stationary.h").macros(
        "ACT_ARG_COUGHING_SHALLOW_WATER")]
    definitions += [source("include/config/config_movement.h").macros("BACKWARD_BUTT_SLIDE_JUMP_AIR_DRAG|BACKWARD_JUMP_KICK_UPHILL_DECEL")]
    definitions += ["#define TAS_MOVIE_DIRECTORY " + json.dumps(str(root / "docs/verification/tas-thicc"))]
    definitions += [source("include/sm64.h").declaration("enum", "MarioFlags")]
    definitions += [source("include/object_constants.h").declaration("enum", "ObjActiveFlags")]
    definitions += [source("src/game/camera.h").macros("REONUCAM_SPEED_COUNT|REONUCAM_DEFAULT_SPEED")]
    header = source("src/game/save_file.h")
    definitions += [header.macros("SAVE_CAP_AREA_.*|SAVE_CAP_LEVEL_.*")]
    course = source("include/course_table.h")
    course_text = course.text.replace('#include "levels/course_defines.h"',
                                      source("levels/course_defines.h").text)
    definitions += [course_text, header.macros("NUM_SAVE_FILES"),
                    header.declaration("enum", "SaveFileIndex"),
                    header.declaration("enum", "SaveProgressFlags")]
    definitions += [header.declaration("struct", name) for name in (
        "SaveBlockSignature", "SaveFile", "MainMenuSaveData", "SaveBuffer")]
    definitions += [header.region(r"^void save_file_do_save", r"^void save_file_load_all")]
    character = source("src/game/mario_character.h")
    definitions += [character.region(r"^struct MarioState;", r"^#endif")]
    definitions += [source("include/config/config_movement.h").macros(
        "WALK_TARGET_SPEED|JUMP_HEIGHT_VELOCITY_SCALE|JUMP_FORWARD_SPEED_RETENTION")]
    definitions += [source("include/sm64.h").macros("ACT_DAB|ACT_QUICKSAND_DEATH|ACT_IDLE|ACT_INTRO_CUTSCENE|ACT_UNINITIALIZED")]
    definitions += [source("src/game/mario_actions_cutscene.h").declaration("enum", "ActionArgsQuicksandDeath")]
    levels = re.findall(r"^(?:DEFINE_LEVEL|STUB_LEVEL)\([^,]+,\s*(LEVEL_\w+)",
                        source("levels/level_defines.h").mask, re.M)
    definitions += ["enum { LEVEL_NONE, " + ", ".join(levels) + " };"]
    save = source("src/game/save_file.c")
    definitions += [save.macros("SAVE_FILE_MAGIC|MENU_DATA_MAGIC")]
    functions += [save.function(name) for name in (
        "calc_checksum", "verify_save_block_signature", "add_save_block_signature",
        "save_main_menu_data", "wipe_main_menu_data", "get_coin_score_age",
        "set_coin_score_age", "touch_coin_score_age", "touch_high_score_ages",
        "restore_save_file_data", "mark_save_file_modified", "save_file_write_progress", "save_file_do_save", "save_file_erase", "save_file_try_erase", "save_file_copy",
        "save_file_is_he_thicc", "save_file_he_thicc_pending", "save_file_he_thicc_intro_pending",
        "save_file_arm_he_thicc", "save_file_start_he_thicc", "save_file_finish_he_thicc_intro",
        "save_file_is_tas_thicc", "save_file_unlock_tas_thicc",
        "save_file_load_all", "save_file_reload", "save_file_exists",
        "save_file_has_completed_game", "save_file_set_flags", "save_file_mark_game_completed",
        "save_file_set_camera_speed", "save_file_get_camera_speed",
        "save_file_get_flags", "save_file_set_star_flags", "save_file_set_cap_pos", "save_file_get_cap_pos")]
    definitions += [header.macros("SAVE_FLAG_TO_STAR_FLAG|STAR_FLAG_TO_SAVE_FLAG")]
    functions += [source("src/game/mario_character.c").region(
        r"^const struct MarioMovementProfile gMarioMovementProfiles", r"\Z")]
    functions += [source("src/game/level_update.c").function(name) for name in (
        "lvl_prepare_tas_thicc_start", "lvl_init_from_save_file", "lvl_should_skip_file_select",
        "set_play_mode", "save_reonucam_speed_on_unpause", "play_mode_paused")]
    definitions += [character.macros("MARIO_TAS_CODE_FRAMES")]
    definitions += [source("include/sm64.h").macros("C_BUTTONS")]
    functions += [source("src/menu/file_select.c").function("lvl_update_obj_and_load_file_selected")]
    functions += [source("src/game/ingame_menu.c").function(name)
                  for name in ("reset_menu_navigation", "set_menu_mode")]
    functions += [source("src/game/mario_actions_cutscene.c").function("intro_cutscene_set_mario_to_idle")]
    functions += [source("src/menu/file_select.c").region(r"^unsigned char textMarioC", r"^unsigned char textMarioD")]
    definitions += [source("src/menu/file_select.h").declaration("enum", "MenuButtonTypes")]
    definitions += [source("src/menu/file_select.h").declaration("enum", name)
                    for name in ("MainMenuButtonStates", "CopyMenuActionPhase")]
    definitions += [source("include/model_ids.h").macros(
        "MODEL_MAIN_MENU_MARIO_SAVE_BUTTON_FADE|MODEL_LEVEL_GEOMETRY_08")]
    definitions += [source("src/menu/file_select.c").macros("BUZZ_TIMER")]
    functions += [source("src/menu/file_select.c").function("copy_action_file_button")]
    functions += [source("src/menu/file_select.c").function("geo_switch_file_select_face")]

def mutations_character_save(root):
    for name, case, path, function, old, new in (
        ("completion_not_saved", "completed_start", "src/game/save_file.c", "save_file_mark_game_completed",
         "save_file_do_save(fileIndex);", "(void) fileIndex;"),
        ("completion_wrong_file", "completed_start", "src/game/save_file.c", "save_file_has_completed_game",
         "gSaveBuffer.files[fileIndex][0].flags", "gSaveBuffer.files[0][0].flags"),
        ("completion_overrides_debug", "completion_priority", "src/game/level_update.c", "lvl_init_from_save_file",
         "!gDebugLevelSelect && save_file_has_completed_game", "save_file_has_completed_game"),
    ):
        yield "character_save", name, "character_save." + case, {
            path: MutationSource(root / path).replace_in_function(function, old, new)}
    for name, case, path, function, old, new in (
        ("tas_short_code", "tas_sequence", "src/game/mario_character.c", "mario_character_update_tas_code",
         "== MARIO_TAS_CODE_FRAMES", "== MARIO_TAS_CODE_FRAMES - 1"),
        ("tas_extra_buttons", "tas_sequence", "src/game/mario_character.c", "mario_character_update_tas_code",
         "buttons != expected", "(buttons & C_BUTTONS) != expected"),
        ("tas_intro_not_cleared", "tas_file_select", "src/game/save_file.c", "save_file_unlock_tas_thicc",
         "~(SAVE_FLAG_HE_THICC_PENDING | SAVE_FLAG_HE_THICC_INTRO)", "~SAVE_FLAG_HE_THICC_PENDING"),
        ("tas_wrong_start_level", "tas_file_select", "src/game/level_update.c", "lvl_init_from_save_file",
         "levelNum = gCurrLevelNum = LEVEL_CASTLE_GROUNDS;", "levelNum = gCurrLevelNum = LEVEL_BOB;"),
        ("tas_stale_pause", "tas_pause", "src/game/level_update.c", "set_play_mode",
         "sPauseTasCodeProgress = 0;", "(void) sPauseTasCodeProgress;"),
        ("tas_menu_not_closed", "tas_pause", "src/game/ingame_menu.c", "set_menu_mode",
         "mode == MENU_MODE_NONE || ", ""),
        ("tas_wrong_name", "tas_save_isolation", "src/menu/file_select.c", "file_c_name",
         "return textTasThicc;", "return textHeThicc;"),
        ("tas_copy_flag_leak", "tas_save_isolation", "src/game/save_file.c", "save_file_copy",
         "| SAVE_FLAG_HE_THICC_INTRO | SAVE_FLAG_TAS_THICC);", "| SAVE_FLAG_HE_THICC_INTRO);"),
    ):
        yield "character_save", name, "character_save." + case, {
            path: MutationSource(root / path).replace_in_function(function, old, new)}
    path = "src/game/mario_character.c"
    for name, old, new in (
        ("ignore_living_boos", "&& obj->oBooDeathStatus == BOO_DEATH_STATUS_ALIVE\n            && obj->oAction != 3", "&& FALSE"),
        ("require_big_boo", "obj_has_behavior(obj, bhvMerryGoRoundBoo)", "obj_has_behavior(obj, bhvMerryGoRoundBoo) || obj_has_behavior(obj, bhvBooWithCage)"),
        ("count_defeated_boo", "&& obj->oAction != 3) { // Fatal hit", ") { // Fatal hit"),
        ("missing_cage_boo_allowed", "return courtyardBigBooPresent;", "return courtyardBigBooPresent || TRUE;"),
        ("ignore_cap_requirement", "&& !(m->flags & (MARIO_CAP_ON_HEAD | MARIO_CAP_IN_HAND))", ""),
        ("allow_invisible_big_boo", "obj->oOpacity > 180", "TRUE"),
        ("allow_fading_big_boo", "obj->oBooTargetOpacity == 255", "TRUE"),
    ):
        yield "character_save", name, "character_save.courtyard_boos", {
            path: MutationSource(root / path).replace_in_function("mario_character_can_unlock", old, new)}
    checks = [
        ("claim_occupied_c", "character_save.occupied", "save_file_start_he_thicc", "!(gSaveBuffer.files[SAVE_FILE_C][0].flags & SAVE_FLAG_FILE_EXISTS)", "TRUE"),
        ("lose_pending_on_erase", "character_save.occupied", "save_file_erase", "flags = pending;", "flags = pending & 0;"),
        ("copy_unlock_to_a", "character_save.erase_copy", "save_file_copy", "flags |= characterFlags;", "flags |= characterFlags | SAVE_FLAG_HE_THICC;"),
        ("skip_intro_completion", "character_save.intro", "save_file_finish_he_thicc_intro", "flags &= ~SAVE_FLAG_HE_THICC_INTRO;", "flags |= SAVE_FLAG_HE_THICC_INTRO;"),
    ]
    for name, case, function, old, new in checks:
        path = "src/game/save_file.c"
        yield "character_save", name, case, {path: MutationSource(root / path).replace_in_function(function, old, new)}
    menu_path = "src/menu/file_select.c"
    yield "character_save", "omit_he_thicc_file_face", "character_save.file_face_palette", {
        menu_path: MutationSource(root / menu_path).replace_in_function("geo_switch_file_select_face",
            "save_file_is_he_thicc(SAVE_FILE_C)", "FALSE")}
