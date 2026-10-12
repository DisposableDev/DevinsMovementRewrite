#!/usr/bin/env python3
"""Execute complete production counter/storage/tracking units with controlled game services."""

from pathlib import Path
import re
import shutil
import subprocess
import tempfile
import unittest

from regression_support import Source

ROOT = Path(__file__).resolve().parents[1]


def prepare_fixture(directory):
    types = """#ifndef STATS_TEST_TYPES_H
#define STATS_TEST_TYPES_H
#include <stdint.h>
#include <stddef.h>
typedef int8_t s8; typedef uint8_t u8;
typedef int16_t s16; typedef uint16_t u16;
typedef int32_t s32; typedef uint32_t u32;
typedef int64_t s64; typedef uint64_t u64;
typedef float f32;
#endif
"""
    (directory / "PR").mkdir()
    (directory / "PR/ultratypes.h").write_text(types)
    tracking = ROOT / "src/game/player_stats_tracking.c"
    behavior_names = sorted(set(re.findall(r"\bbhv[A-Za-z0-9_]+", tracking.read_text()
                                                 + (ROOT / "tests/player_stats_test.c").read_text())))
    declarations = []
    sm64 = Source(ROOT / "include/sm64.h")
    for enum in ("MarioFlags", "MarioActionGroups", "MarioActionFlags"):
        declarations.append(sm64.declaration("enum", enum))
    declarations.append(sm64.macros("ACT_.*"))
    options = Source(ROOT / "src/game/game_options.h")
    declarations.append(options.macros("GAME_OPTION_CATALOG"))
    declarations.append(options.declaration("enum", "GameOptionId"))
    declarations.append("s32 game_options_cheat_active(enum GameOptionId id);")
    declarations.append("s32 game_options_is_cheating(void);")
    interaction = Source(ROOT / "src/game/interaction.h")
    for enum in ("AttackType", "InteractSubtypes"):
        declarations.append(interaction.declaration("enum", enum))
    declarations.append(Source(ROOT / "src/game/mario_punch.h").declaration(
        "enum", "ActionArgsPunchSequence"))
    declarations.append(Source(ROOT / "src/game/mario_actions_cutscene.h").declaration(
        "enum", "ActionArgsQuicksandDeath"))
    declarations.append(Source(ROOT / "src/game/mario_character.h").declaration("enum", "MarioPalette"))
    declarations.append("u8 mario_character_palette(void);")
    declarations.append("\n".join(f"extern const u32 {name}[];" for name in behavior_names))
    world = """#ifndef STATS_TEST_WORLD_H
#define STATS_TEST_WORLD_H
#include <PR/ultratypes.h>
#include <math.h>
#include "include/level_table.h"
#include "include/course_table.h"
#include "include/model_ids.h"
#include "include/object_constants.h"
#include "include/dialog_ids.h"
#define ABS(x) ((x) < 0 ? -(x) : (x))
#define TRUE 1
#define FALSE 0
#define MAX(a,b) ((a) > (b) ? (a) : (b))
#define MIN(a,b) ((a) < (b) ? (a) : (b))
#define OBJECT_POOL_CAPACITY 240
#define vec3f_copy(dst,src) do { (dst)[0]=(src)[0]; (dst)[1]=(src)[1]; (dst)[2]=(src)[2]; } while (0)
typedef f32 Vec3f[3];
struct Object {
const void *behavior;
struct Object *parentObj;
s32 oDamageOrCoinValue;
u32 oInteractionSubtype;
s32 oHealth;
s32 oAction;
s32 model;
};
struct Surface { s32 type; };
extern struct Surface gWaterSurfacePseudoFloor;
struct MarioState {
u32 action;
u32 actionArg;
u32 flags;
Vec3f pos;
s16 health;
u8 hurtCounter, healCounter;
struct Object *usedObj;
struct Object *heldObj;
struct Object *riddenObj;
const void *wall;
struct Surface *floor;
f32 floorHeight;
f32 waterLevel;
u8 soakedThisUpdate;
};
extern struct MarioState *gMarioState;
extern struct Object gObjectPool[];
extern s16 gCurrSaveFileNum, gCurrLevelNum, gCurrAreaIndex, gCurrCourseNum;
extern void *gCurrDemoInput, *gCurrCreditsEntry;
extern s8 gLevelToCourseNumTable[];
s32 mario_health_after_pending_damage(struct MarioState *m);
s32 get_dialog_id(void);
u32 save_file_get_star_flags(s32 file, s32 course);
s32 obj_has_model(struct Object *obj, s32 model);
s32 obj_has_behavior(struct Object *obj, const void *behavior);
s32 mario_is_soaked(struct MarioState *m);
s32 mario_has_water_contact(struct MarioState *m, f32 waterLevel);
s32 mario_ground_twirl_is_held_pose(struct MarioState *m);
s32 mario_ground_twirl_is_enemy(struct Object *obj);
s32 mario_ground_twirl_is_immune_enemy(struct Object *obj);
""" + "\n".join(declarations) + "\n#endif\n"
    (directory / "stats_test_world.h").write_text(world)
    (directory / "behaviors.c").write_text(
        '#include "stats_test_world.h"\n' + "\n".join(
            f"const u32 {name}[] = {{ {i + 1} }};" for i, name in enumerate(behavior_names))
        + '\ns32 game_options_cheat_active(enum GameOptionId id) { (void) id; return 0; }\n'
        + Source(ROOT / 'src/engine/surface_collision.h').macros('FLOOR_LOWER_LIMIT')
        + Source(ROOT / 'src/game/mario.c').function('mario_has_water_contact')
        + Source(ROOT / 'src/game/mario.c').function('mario_health_after_pending_damage'))
    for header in ("sm64.h", "area.h", "behavior_data.h", "game_init.h", "ingame_menu.h",
                   "interaction.h", "dialog_ids.h", "level_table.h", "level_update.h",
                   "game_options_runtime.h", "mario.h", "mario_punch.h", "mario_step.h", "object_constants.h", "object_helpers.h",
                   "object_list_processor.h", "save_file.h", "engine/math_util.h", "mario_character.h"):
        path = directory / header
        path.parent.mkdir(parents=True, exist_ok=True)
        path.write_text('#include "stats_test_world.h"\n')
    for source in ("player_stats.c", "player_stats.h", "player_stats_tracking.c"):
        shutil.copyfile(ROOT / "src/game" / source, directory / source)


class PlayerStatsTests(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.directory = tempfile.TemporaryDirectory(prefix="sm64-player-stats-")
        cls.temp = Path(cls.directory.name)
        prepare_fixture(cls.temp)
        cls.executable = cls.temp / "test"
        command = ["cc", "-std=c11", "-Wall", "-Wextra", "-Werror", "-fsanitize=undefined", "-fno-sanitize-recover=all",
                   "-I", str(cls.temp), "-I", str(ROOT),
                   str(cls.temp / "player_stats.c"), str(cls.temp / "player_stats_tracking.c"),
                   str(cls.temp / "behaviors.c"), str(ROOT / "tests/player_stats_test.c"),
                   "-lm", "-o", str(cls.executable)]
        result = subprocess.run(command, capture_output=True, text=True, timeout=30)
        if result.returncode:
            cls.directory.cleanup()
            raise RuntimeError(result.stdout + result.stderr)

    @classmethod
    def tearDownClass(cls):
        cls.directory.cleanup()

    def run_case(self, name):
        result = subprocess.run([str(self.executable), name], capture_output=True, text=True,
                                timeout=10)
        self.assertEqual(result.returncode, 0, result.stdout + result.stderr)

    def test_cheating_excludes_counters_records_and_movement(self):
        self.run_case("cheating")

    def test_cheating_preserves_real_course_visits(self):
        self.run_case("cheating_visits")

    def test_counters_and_slot_isolation(self):
        self.run_case("counters")

    def test_newest_valid_backup(self):
        self.run_case("journal")

    def test_interrupted_write(self):
        self.run_case("torn_write")

    def test_write_retry(self):
        self.run_case("retry_write")

    def test_readback_failure(self):
        self.run_case("verify_failure")

    def test_initial_read_failure_preserves_persisted_totals(self):
        self.run_case("initial_read_failure")

    def test_copy_rejects_unread_source(self):
        self.run_case("copy_read_failure")

    def test_copy_and_erase_backups(self):
        self.run_case("copy_erase")

    def test_slot_identity(self):
        self.run_case("slot_identity")

    def test_bowser_defeats_per_fight(self):
        self.run_case("bowser_defeats")

    def test_version_three_migration(self):
        self.run_case("migration_v3")

    def test_damage_free_visits(self):
        self.run_case("damage_free_visits")

    def test_activity_timers(self):
        self.run_case("activity_timers")

    def test_new_defeat_records(self):
        self.run_case("new_defeat_records")

    def test_cannon_flight_record(self):
        self.run_case("cannon_flight_record")

    def test_migration_v6(self):
        self.run_case("migration_v6")

    def test_enemy_bounce_streak(self):
        self.run_case("enemy_bounce_streak")

    def test_seconds_soaked(self):
        self.run_case("seconds_soaked")

    def test_version_five_migration(self):
        self.run_case("migration_v5")

    def test_version_four_migration(self):
        self.run_case("migration_v4")

    def test_newer_version_four_clears_rejected_backup_fields(self):
        self.run_case("migration_mixed_v4")

    def test_clock_and_death_visit(self):
        self.run_case("clock_and_visits")

    def test_physics_lab_course_time(self):
        self.run_case("lab_time")

    def test_separate_distances_and_warp(self):
        self.run_case("movement")

    def test_star_exit_movement_is_excluded(self):
        self.run_case("warp_exit_actions")

    def test_entry_dialogue_timing(self):
        level = Source(ROOT / "src/game/level_update.c")
        driver = """#define main stats_fixture_main
#include "tests/player_stats_test.c"
#undef main
#define typeof __typeof__
"""
        driver += Source(ROOT / "src/game/save_file.h").declaration("enum", "SaveProgressFlags")
        driver += Source(ROOT / "src/game/level_update.h").declaration("enum", "PlayModes")
        driver += """
static s16 sTransitionTimer;
static void (*sTransitionUpdate)(s16 *);
static s32 sPlayMode = PLAY_MODE_CHANGE_AREA;
static struct {
    u8 dialog[1];
    void *camera;
} sArea = { { DIALOG_000 }, NULL }, *gCurrentArea = &sArea;

static u32 save_file_get_flags(void) {
    return 0;
}

static void create_dialog_box(u32 id) {
    sDialog = id;
}

static void update_camera(void *camera) {
    (void) camera;
}

static void set_play_mode(s32 mode) {
    sPlayMode = mode;
}
"""
        for name in ("level_set_transition", "load_level_init_text", "play_mode_change_area"):
            driver += level.function(name)
        driver += """
static void assert_clocks(u64 playtime, u64 dialogue) {
    assert(value(PS_TOTAL_PLAY_TIME) == playtime);
    assert(value(PS_TIME_SPENT_IN_COURSES) == playtime);
    assert(value(PS_TIME_SPENT_IN_DIALOGUE) == dialogue);
}

int main(void) {
    s32 frame;
    setup();
    load_level_init_text(0);
    assert(sDialog == DIALOG_000 && sTransitionTimer == -1);
    assert_clocks(1, 0);
    for (frame = 0; frame < 60; frame++) {
        play_mode_change_area();
    }
    assert_clocks(61, 60);
    assert(value(PS_COURSE_ENTRIES) == 1);

    // Closing the message resumes normal play without an extra clock tick.
    sDialog = DIALOG_NONE;
    level_set_transition(0, NULL);
    play_mode_change_area();
    assert_clocks(61, 60);
    assert(sTransitionTimer == 0 && sPlayMode == PLAY_MODE_NORMAL);

    // A timed warp stays excluded, even with a lingering dialogue ID.
    sDialog = DIALOG_000;
    level_set_transition(30, NULL);
    for (frame = 0; frame < 31; frame++) {
        play_mode_change_area();
    }
    assert_clocks(61, 60);

    // An indefinite transition alone is not an active dialogue.
    sDialog = DIALOG_NONE;
    level_set_transition(-1, NULL);
    for (frame = 0; frame < 3; frame++) {
        play_mode_change_area();
    }
    assert_clocks(61, 60);

    load_level_init_text(0);
    gCurrDemoInput = &sMario;
    play_mode_change_area();
    assert_clocks(61, 60);
    gCurrDemoInput = NULL;
    gCurrCreditsEntry = &sMario;
    play_mode_change_area();
    assert_clocks(61, 60);
    gCurrCreditsEntry = NULL;
    play_mode_change_area();
    assert_clocks(62, 61);
    return 0;
}
"""
        source = self.temp / "entry_dialog_test.c"
        source.write_text(driver)
        executable = self.temp / "entry_dialog_test"
        command = ["cc", "-std=c11", "-Wall", "-Wextra", "-Werror", "-fsanitize=undefined", "-fno-sanitize-recover=all",
                   "-I", str(self.temp), "-I", str(ROOT),
                   str(self.temp / "player_stats.c"), str(self.temp / "player_stats_tracking.c"),
                   str(self.temp / "behaviors.c"), str(source), "-lm", "-o", str(executable)]
        result = subprocess.run(command, capture_output=True, text=True, timeout=30)
        self.assertEqual(result.returncode, 0, result.stdout + result.stderr)
        result = subprocess.run([str(executable)], capture_output=True, text=True, timeout=10)
        self.assertEqual(result.returncode, 0, result.stdout + result.stderr)

    def test_twirl_preserves_long_jump(self):
        self.run_case("twirl_long_jump")

    def test_wing_cap_invalidates_airtime(self):
        self.run_case("assisted_airtime")

    def test_fall_bounce_water_and_lethal_landing(self):
        self.run_case("survived_falls")

    def test_coin_values_star_identity_and_non_exit(self):
        self.run_case("coins_stars")

    def test_defeat_credit_stun_and_object_reuse(self):
        self.run_case("enemy_credit")

    def test_health_amounts_and_damage_deduplication(self):
        self.run_case("health_and_hits")

    def test_completed_slide_and_shell_records(self):
        self.run_case("completed_sequences")

    def test_thrown_object_defeat_credit(self):
        self.run_case("indirect_defeat")

    def test_version_one_migration_and_crc_fallback(self):
        self.run_case("migration")

    def test_version_two_all_slots_and_retired_histories(self):
        self.run_case("migration_v2")

    def test_he_thicc_active_character_events_and_time(self):
        self.run_case("he_thicc_stats")

    def test_migration_write_failure_preserves_legacy_copy(self):
        self.run_case("migration_failure")

    def test_legacy_retirement_before_record_reuse(self):
        self.run_case("legacy_retirement")

    def test_dabs_sleep_origin_and_pose_timers(self):
        self.run_case("dabs_pose")

    def test_twirl_rebound_continuation_and_reset(self):
        self.run_case("twirl_chains")

    def test_water_support_distances_sequence_and_shell_exclusion(self):
        self.run_case("water_walk")

    def test_passive_metal_vs_wing_twirl_defeat_origin(self):
        self.run_case("metal_origin")

    def test_cap_power_combinations_and_normal_cap_exclusion(self):
        self.run_case("cap_powers")

    def test_periodic_checkpoint_frequency(self):
        self.run_case("checkpoints")


if __name__ == "__main__":
    unittest.main()
