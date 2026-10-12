#include <ultra64.h>
#include "sm64.h"
#include "segment_symbols.h"
#include "level_commands.h"

#include "levels/intro/header.h"
#include "levels/menu/header.h"
#include "game/level_update.h"
#include "game/jrb_simpleflips.h"

#include "make_const_nonconst.h"

#include "config.h"

extern const LevelScript level_main_scripts_entry[];
extern const LevelScript level_script_entry[];
static const LevelScript entry_simpleflips_game_over[] = {
    EXECUTE(SEGMENT_MENU_INTRO, _introSegmentRomStart, _introSegmentRomEnd,
            level_intro_mario_head_dizzy),
    JUMP(level_script_entry),
};
#ifdef TEST_LEVEL
static const LevelScript entry_unlocked_file_select[] = {
    EXECUTE(SEGMENT_MENU_INTRO, _menuSegmentRomStart, _menuSegmentRomEnd,
            level_main_menu_entry_file_select),
    JUMP(level_script_entry),
};
#endif

const LevelScript level_script_entry[] = {
    INIT_LEVEL(),
    SLEEP(/*frames*/ 2),
    BLACKOUT(/*active*/ FALSE),
    // Save data is loaded before this script, including after a reset.
    CALL(0, lvl_simpleflips_boot),
    JUMP_IF(OP_EQ, TRUE, entry_simpleflips_game_over),
#ifdef TEST_LEVEL
    CALL(0, lvl_should_skip_file_select),
    JUMP_IF(OP_EQ, FALSE, entry_unlocked_file_select),
    SET_REG(/*value*/ TEST_LEVEL),
    EXECUTE(/*seg*/ SEGMENT_GLOBAL_LEVEL_SCRIPT, /*script*/ _scriptsSegmentRomStart,
            /*scriptEnd*/ _scriptsSegmentRomEnd, /*entry*/ level_main_scripts_entry),
#else
    SET_REG(/*value*/ 0),
    EXECUTE(/*seg*/ SEGMENT_MENU_INTRO, /*script*/ _introSegmentRomStart,
            /*scriptEnd*/ _introSegmentRomEnd, /*entry*/ level_intro_splash_screen),
#endif
    JUMP(/*target*/ level_script_entry),
};
