#!/usr/bin/env python3
"""Host checks of actual lab fixture motion and pause/exit routing.

Engine rendering, platform displacement and warp execution are observed stubs;
these tests do not establish Mario collision response or emulator behavior.
"""
import os
from pathlib import Path
import re
import shlex
import subprocess
import tempfile
import unittest

from regression_support import Source

ROOT = Path(__file__).resolve().parents[1]


def prepare_behavior_fixture(directory):
    """Share the production extraction with host tests and parsed source checks."""
    mover_enum = Source(ROOT / "levels/physics_lab/header.h").declaration("enum", "PhysicsLabMover")
    lab_behavior_source = Source(ROOT / "src/game/behaviors/physics_lab.inc.c")
    treasure_chest_source = Source(ROOT / "src/game/behaviors/treasure_chest.inc.c")
    menu_source = Source(ROOT / "src/game/ingame_menu.c")
    level_update_source = Source(ROOT / "src/game/level_update.c")
    runtime_source = (mover_enum + lab_behavior_source.function("bhv_physics_lab_mover_init")
                      + lab_behavior_source.function("bhv_physics_lab_mover_loop")
                      + lab_behavior_source.function("bhv_physics_lab_chests_init")
                      + lab_behavior_source.function("bhv_physics_lab_chests_loop")
                      + treasure_chest_source.function("bhv_treasure_chest_bottom_loop")
                      + menu_source.macros("PAUSE_MENU_UNPAUSE_DELAY_FRAMES")
                      + Source(ROOT / "src/game/game_options.h").macros("GO_MENU_WAIT_FOR_NEUTRAL")
                      + menu_source.function("reset_menu_navigation")
                      + menu_source.function("reset_pause_menu_navigation")
                      + menu_source.function("render_pause_courses_and_castle")
                      + level_update_source.function("play_mode_paused"))
    (directory / "lab_actual.inc.c").write_text(runtime_source)


def prepare_boulder_fixture(directory):
    """Share the real boulder and lane extraction with tests and parsed checks."""
    from test_physics_lab_geometry import TRIANGLES

    def source(path):
        return Source(ROOT / path)

    type_definitions = []
    for path, kind, names in [
        ("include/sm64.h", "enum", ["MarioFlags"]),
        ("src/game/object_list_processor.h", "enum", ["CollisionFlags"]),
        ("include/surface_terrains.h", "enum", ["SurfaceTypes", "SurfaceFlags"]),
        ("include/object_constants.h", "enum", ["ObjActiveFlags"]),
        ("src/engine/surface_collision.h", "struct",
         ["WallCollisionData", "WallCollisionCacheCell", "WallCollisionCache"]),
        ("src/engine/surface_load.h", "enum", ["SpatialPartitions"]),
        ("include/types.h", "struct", ["ObjectHitbox"]),
        ("src/game/obj_behaviors.h", "enum", ["ObjCollisionFlags"])
    ]:
        type_definitions += [source(path).declaration(kind, name) for name in names]
    type_definitions += [
        source("src/engine/surface_load.c").declaration("struct", "DynamicWallTail"),
        "static struct DynamicWallTail sDynamicWallTails[16];"]
    surface_query_source = source("src/engine/surface_query.h")
    type_definitions += [
        "#define STATIC_SURFACE_INDEX 0\n#define DYNAMIC_SURFACE_INDEX 0",
        surface_query_source.declaration("enum", "SurfaceQueryStorage"),
        surface_query_source.declaration("struct", "SurfaceQueryIterator")]
    type_definitions += [source("src/engine/surface_collision.h").macros(
        "FIND_FLOOR_BUFFER|CELL_HEIGHT_LIMIT|FLOOR_LOWER_LIMIT(_MISC)?|is_outside_level_bounds|get_surface_height_at_location")]
    type_definitions += [source("include/surface_terrains.h").macros("SURFACE_IS_NEW_WATER")]
    runtime_fragments = [
        'void object_platform_motion_before_step(struct Object *obj) {}',
        '#include "trig_tables.inc.c"',
        source("src/engine/math_util.h").macros("gCosineTable|sins|coss|atans"),
        source("src/engine/math_util.c").macros("min_max_3_func")]
    runtime_fragments += [source("src/engine/bhaskara_trig.inc.c").text]
    # The lane fixture uses ordinary cursors and no result caches.
    runtime_fragments += ["#define ALWAYS_INLINE inline\n"
                         "#define STATIC_WALL_HEIGHT_GROUPS 0\n"
                         "#define COLLISION_RESULT_CACHES 0\n"
                         "#define PUPPYPRINT_GET_COLLISION_SNAPSHOT PUPPYPRINT_GET_SNAPSHOT"]
    runtime_fragments += [surface_query_source.function(name).replace("ALWAYS_INLINE", "inline")
                         for name in ("surface_query_list", "surface_query_is_empty",
                                      "surface_query_has_multiple", "surface_query_identity",
                                      "surface_query_cell", "surface_query_next")]
    for path, names in [
        ("src/engine/math_util.c", ["atan2_lookup", "atan2s", "min_max_3i"]),
        ("src/engine/surface_load.c",
         ["add_surface_to_cell", "lower_cell_index", "upper_cell_index", "add_surface"]),
        ("src/engine/surface_collision.c",
         ["check_wall_vw", "check_wall_edge", "surface_is_passable_vanish",
          "find_wall_collisions_in_cells", "dispatch_wall_collisions_in_cells", "find_wall_collisions",
          "check_within_floor_triangle_bounds", "floor_height_cutoff",
          "find_floor_from_list", "find_static_floor_from_list", "find_floor"]),
        ("src/game/object_helpers.c", ["obj_set_hitbox"]),
        ("src/game/obj_behaviors.c",
         ["turn_obj_away_from_surface", "obj_find_wall", "turn_obj_away_from_steep_floor",
          "calc_obj_friction", "calc_new_obj_vel_and_pos_y", "calc_new_obj_vel_and_pos_y_underwater",
          "obj_update_pos_vel_xz", "object_step", "object_step_without_floor_orient"])
    ]:
        extracted = [source(path).function(name) for name in names]
        if path == "src/engine/surface_collision.c":
            wall = source("src/engine/surface_wall.inc.c")
            extracted.insert(3, "#define WALL_HEIGHT_GROUP_SCAN 0\n"
                             + wall.mapped(0, len(wall.text))
                             + "\n#undef WALL_HEIGHT_GROUP_SCAN\n")
        runtime_fragments += extracted
    snowman_source = source("src/game/behaviors/snowman.inc.c")
    runtime_fragments += [
        snowman_source.region(r"^static struct ObjectHitbox sRollingSphereHitbox",
                              r"^static const Vec3f sSnowmanHeadPos"),
        snowman_source.function("set_rolling_sphere_hitbox"),
        snowman_source.function("adjust_rolling_face_pitch")]
    runtime_fragments += [source("src/game/behaviors/boulder.inc.c").function(name)
                         for name in ("bhv_big_boulder_init", "boulder_act_1", "bhv_big_boulder_loop")]
    lane_triangles = []
    for tri, normal, surface, force in TRIANGLES:
        if (surface != "SURFACE_DEATH_PLANE"
            and max(p[0] for p in tri) >= 13300 and min(p[0] for p in tri) <= 15100
            and max(p[2] for p in tri) >= 11000 and min(p[2] for p in tri) <= 14600):
            if (surface, force) != ("SURFACE_DEFAULT", 0):
                raise AssertionError("Boulder lane must use ordinary unforced collision")
            lane_triangles.append("{" + ",".join("{" + ",".join(map(str, p)) + "}" for p in tri) + "}")
    if not 0 < len(lane_triangles) < 600:
        raise AssertionError("Boulder lane mesh is missing or unexpectedly large")
    enemy_placements = (ROOT / "levels/physics_lab/enemies.inc.c").read_text()
    x, y, z, yaw = re.search(
        r"OBJECT\(LAB_MODEL_HMC_ROLLING_ROCK, (-?\d+), (-?\d+), (-?\d+), 0, (\d+), 0, 0, bhvBigBoulder",
        enemy_placements).groups()
    mesh_source = "static Vec3s mesh[][3]={" + ",".join(lane_triangles) + "};\n"
    mesh_source += f"static Vec3f spawn={{{x},{y},{z}}}; static s16 spawnYaw={int(yaw)*65536//360};\n"
    (directory / "lab_boulder_definitions.inc.c").write_text("\n".join(type_definitions))
    (directory / "lab_boulder_actual.inc.c").write_text("\n".join(runtime_fragments))
    (directory / "lab_boulder_mesh.inc.c").write_text(mesh_source)


def prepare_native_paths_fixture(directory):
    """Prepare the same native movement/mesh inputs for host and parsed checks."""
    from test_physics_lab_geometry import collision_vertices

    checkerboard_source = Source(ROOT / "src/game/behaviors/checkerboard_platform.inc.c")
    spindel_source = Source(ROOT / "src/game/behaviors/spindel.inc.c")
    math_source = Source(ROOT / "src/engine/math_util.c")
    runtime_source = "typedef uint16_t u16;\ntypedef uint32_t u32;\n"
    runtime_source += Source(ROOT / "src/engine/bhaskara_trig.inc.c").text
    runtime_source += (checkerboard_source.mapped(0, len(checkerboard_source.text))
                       + spindel_source.mapped(0, len(spindel_source.text)))
    runtime_source += math_source.function("mtxf_rotate_zxy_and_translate")
    mesh_source = ""
    for label, name, path in (
        ("checker", "checkerboard_platform_seg8_collision_platform",
         "actors/checkerboard_platform/collision.inc.c"),
        ("spindel", "ssl_seg7_collision_spindel", "levels/ssl/spindel/collision.inc.c"),
    ):
        vertices = collision_vertices(name, path)
        rows = ",".join("{" + ",".join(map(str, point)) + "}" for point in vertices)
        mesh_source += "const float " + label + "_vertices[][3]={" + rows + "};\n"
    level_script = (ROOT / "levels/physics_lab/script.c").read_text()
    checkerboard_params = re.search(r", (0x[0-9A-Fa-f]+), bhvCheckerboardElevatorGroup", level_script)[1]
    enemy_placements = (ROOT / "levels/physics_lab/enemies.inc.c").read_text()
    spindel_position = re.search(
        r"OBJECT\(LAB_MODEL_SSL_SPINDEL, (-?\d+), (-?\d+), (-?\d+), 0, 0, 0, 0, bhvSpindel",
        enemy_placements,
    ).groups()
    (directory / "lab_native_actual.inc.c").write_text(runtime_source)
    (directory / "lab_native_vertices.inc.c").write_text(mesh_source)
    return checkerboard_params, spindel_position


class PhysicsLabBehaviors(unittest.TestCase):
    def test_boulder_keeps_rolling_and_stays_in_its_lane(self):
        with tempfile.TemporaryDirectory(prefix="lab-boulder-") as directory:
            fixture_directory = Path(directory)
            binary = fixture_directory / 'boulder'
            prepare_boulder_fixture(fixture_directory)
            subprocess.run(shlex.split(os.environ.get("CC", "cc")) + [
                "-std=c99", "-O1", "-Wall", "-Wextra", "-Werror", "-Wno-unused-parameter",
                "-I", str(fixture_directory), "-I", str(ROOT / "include"),
                str(ROOT / "tests/physics_lab_boulder_test.c"),
                "-lm", "-o", str(binary)], check=True, timeout=30)
            subprocess.run([str(binary)], check=True, timeout=30)

    def test_native_platform_and_spindel_paths_clear_terrain(self):
        from test_physics_lab_geometry import fixture_placement, floor, static_intrusions
        with tempfile.TemporaryDirectory(prefix="lab-native-paths-") as directory:
            fixture_directory = Path(directory)
            binary = fixture_directory / 'paths'
            checkerboard_params, spindel_position = prepare_native_paths_fixture(fixture_directory)
            subprocess.run(shlex.split(os.environ.get("CC", "cc")) + [
                "-std=c99", "-Wall", "-Wextra", "-Werror", "-Wno-unused-parameter",
                "-I", str(fixture_directory),
                str(ROOT / "tests/physics_lab_native_paths_test.c"),
                "-lm", "-o", str(binary)], check=True, timeout=30)
            bounds_output = subprocess.check_output([
                str(binary), *map(str, fixture_placement("bhvCheckerboardElevatorGroup")),
                checkerboard_params, *spindel_position], text=True, timeout=10)
        self.assertEqual(len(bounds_output.splitlines()), 3)  # both platforms and Spindel
        for path_index, bounds_line in enumerate(bounds_output.splitlines()):
            path_bounds = list(map(float, bounds_line.split()))
            bounds_min = path_bounds[:3]
            bounds_max = path_bounds[3:]
            if path_index < 2:
                self.assertGreater(bounds_min[1], 40, (path_index, bounds_min, bounds_max))
            else:
                # Spindel deliberately rolls on the floor; check obstructions
                # above it and the full path's ground coverage instead.
                bounds_min[1] = 1
            self.assertEqual(static_intrusions(bounds_min, bounds_max), [],
                             (path_index, bounds_min, bounds_max))
            for x in range(int(bounds_min[0]), int(bounds_max[0]) + 1, 100):
                for z in range(int(bounds_min[2]), int(bounds_max[2]) + 1, 100):
                    self.assertAlmostEqual(floor(x, z)[0], 0)

    def test_motion_and_pause_exit_with_both_exit_configurations(self):
        with tempfile.TemporaryDirectory(prefix="physics-lab-tests-") as directory:
            fixture_directory = Path(directory)
            prepare_behavior_fixture(fixture_directory)
            for death_on_exit in (False, True):
                with self.subTest(death_on_exit=death_on_exit):
                    binary = fixture_directory / ("death" if death_on_exit else "normal")
                    compile_command = shlex.split(os.environ.get("CC", "cc")) + [
                        "-std=c99", "-Wall", "-Wextra", "-Werror", "-Wno-unused-parameter",
                        "-I", str(fixture_directory),
                        str(ROOT / "tests/physics_lab_behaviors_test.c"),
                        "-lm", "-o", str(binary)]
                    if death_on_exit:
                        compile_command.insert(1, "-DDEATH_ON_EXIT_COURSE")
                    subprocess.run(compile_command, check=True, timeout=30)
                    subprocess.run([str(binary)], check=True, timeout=10)

    def test_tornado_is_intangible_until_visible(self):
        behavior = Source(ROOT / "src/game/behaviors/tweester.inc.c")
        helpers = Source(ROOT / "src/game/object_helpers.c")
        constants = Source(ROOT / "include/object_constants.h")
        runtime_source = constants.macros("TWEESTER_.*")
        runtime_source += constants.declaration("enum", "ObjFlags")
        runtime_source += helpers.function("cur_obj_become_intangible")
        runtime_source += helpers.function("cur_obj_become_tangible")
        runtime_source += helpers.function("obj_set_hitbox")
        runtime_source += behavior.function("tweester_scale_and_move")
        runtime_source += behavior.function("tweester_act_idle")
        runtime_source += behavior.function("tweester_act_hide")
        runtime_source += behavior.region(r"^struct ObjectHitbox sTweesterHitbox", r"^void tweester_scale_and_move")
        runtime_source += "void tweester_act_chase(void) {}\n"
        runtime_source += behavior.region(r"^ObjActionFunc sTweesterActions", r"^void bhv_tweester_loop")
        runtime_source += behavior.function("bhv_tweester_loop")
        fixture = r"""

#include <assert.h>
#include <math.h>
#include <stdint.h>
#include <string.h>
typedef float f32;
typedef int16_t s16;
#define INTERACT_TORNADO 1
#define INT_STATUS_NONE 0
#define SOUND_ENV_WIND1 0
struct Object {
    struct {
        struct {
            f32 scale[3];
        } gfx;
    } header;
    int oFlags, oInteractType, oDamageOrCoinValue, oHealth, oNumLootCoins;
    int oIntangibleTimer, oSubAction, oTweesterUnused, oTimer, oAction;
    int oTweesterScaleTimer, oFaceAngleYaw, oInteractStatus;
    f32 oDistanceToMario, oForwardVel;
    f32 hitboxRadius, hitboxHeight, hurtboxRadius, hurtboxHeight, hitboxDownOffset;
};
struct ObjectHitbox {
    int interactType, downOffset, damageOrCoinValue, health, numLootCoins;
    f32 radius, height, hurtboxRadius, hurtboxHeight;
};
typedef void (*ObjActionFunc)(void);
static struct Object object;
static struct Object *o = &object;
static f32 coss(s16 angle) {
    return cosf(angle * 6.28318530718f / 65536);
}
static void cur_obj_scale(f32 scale) {
    for (int axis = 0; axis < 3; axis++) {
        o->header.gfx.scale[axis] = scale;
    }
}
static void cur_obj_set_pos_to_home(void) {
}
static void cur_obj_play_sound_1(int sound) {
}
static f32 cur_obj_lateral_dist_from_mario_to_home(void) {
    return o->oDistanceToMario;
}
static void cur_obj_call_action_function(ObjActionFunc *actions) {
    actions[o->oAction]();
}
#include "tornado_actual.inc.c"
int main(void) {
    memset(o, 0, sizeof(*o));
    cur_obj_scale(1);
    for (int cycle = 0; cycle < 3; cycle++) {
        o->oAction = TWEESTER_ACT_IDLE;
        o->oSubAction = TWEESTER_SUB_ACT_WAIT;
        o->oDistanceToMario = 2000;
        for (int tick = 0; tick < 3; tick++) {
            bhv_tweester_loop();
            assert(o->oIntangibleTimer == -1 && o->header.gfx.scale[0] == 0);
        }
        // Entering range and the zero-size growth pose are still harmless.
        o->oDistanceToMario = 100;
        bhv_tweester_loop();
        assert(o->oSubAction == TWEESTER_SUB_ACT_GROW && o->oIntangibleTimer == -1);
        bhv_tweester_loop();
        assert(o->oIntangibleTimer == -1 && o->header.gfx.scale[0] == 0);
        for (o->oTimer = 1; o->oTimer <= 60; o->oTimer++) {
            bhv_tweester_loop();
            assert(o->oIntangibleTimer == 0 && o->header.gfx.scale[0] > 0);
        }
        assert(o->oAction == TWEESTER_ACT_CHASE);
        o->oAction = TWEESTER_ACT_HIDE;
        for (o->oTimer = 0; o->oTimer < 60; o->oTimer++) {
            bhv_tweester_loop();
            assert(o->oIntangibleTimer == 0 && o->header.gfx.scale[0] > 0);
        }
        bhv_tweester_loop();
        assert(o->oIntangibleTimer == -1 && o->header.gfx.scale[0] == 0);
        o->oTimer = 361;
        bhv_tweester_loop();
        assert(o->oAction == TWEESTER_ACT_IDLE && o->oIntangibleTimer == -1);
    }
}
"""
        with tempfile.TemporaryDirectory(prefix="tornado-visibility-") as directory:
            fixture_directory = Path(directory)
            (fixture_directory / "tornado_actual.inc.c").write_text(runtime_source)
            (fixture_directory / "test.c").write_text(fixture)
            binary = fixture_directory / "test"
            subprocess.run(shlex.split(os.environ.get("CC", "cc")) + [
                "-std=c99", "-Wall", "-Wextra", "-Werror", "-Wno-unused-parameter",
                "-I", str(fixture_directory), str(fixture_directory / "test.c"), "-lm", "-o", str(binary)],
                check=True, timeout=30)
            subprocess.run([str(binary)], check=True, timeout=10)

    def test_castle_exit_destination_exists(self):
        castle_script = (ROOT / "levels/castle_grounds/script.c").read_text()
        self.assertIn("0x00320000, bhvAirborneWarp", castle_script)
        self.assertIn("WARP_NODE(0x32, LEVEL_CASTLE_GROUNDS, 1, 0x32, WARP_NO_CHECKPOINT)", castle_script)


if __name__ == "__main__":
    unittest.main()
