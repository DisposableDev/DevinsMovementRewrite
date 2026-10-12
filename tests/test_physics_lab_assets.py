#!/usr/bin/env python3
"""Check the lab's private bank and real asset/model adapter on the host.

These checks do not simulate enemy AI, object spawning, animation rendering or
N64 performance. The optional ELF check examines the last locally built ROM.
"""
from pathlib import Path
import re
import subprocess
import tempfile
import unittest

from regression_support import Source

ROOT=Path(__file__).resolve().parents[1]
TABLE=(ROOT/"levels/physics_lab/enemy_remap.inc.c").read_text()
ASSETS=re.findall(r"\{ (bhv\w+), (\w+), (physics_lab_enemy_\w+) \}",TABLE)
MODELS=re.findall(r"\{ (bhv\w+), (MODEL_\w+), (LAB_MODEL_\w+) \}",TABLE)


class PhysicsLabAssets(unittest.TestCase):
    def test_real_model_and_asset_adapter(self):
        lab_source = Source(ROOT / "src/game/physics_lab.c")
        object_helpers_source = Source(ROOT / "src/game/object_helpers.c")
        model_ids_source = (ROOT / "include/model_ids.h").read_text()
        # Placeholder payloads serve only as distinct symbol addresses.
        stub_symbols = {value for row in ASSETS for value in row}
        stub_symbols |= {behavior for behavior, _, _ in MODELS}
        stub_declarations = "\n".join(
            f"static const int {name}[]={{0}};" for name in sorted(stub_symbols)
        )
        model_definitions = []
        for name in {old for _, old, _ in MODELS}:
            definition = re.search(
                r"^#define " + name + r"\s+(0x[0-9A-Fa-f]+|\d+)\b", model_ids_source, re.M
            )
            self.assertIsNotNone(definition, name)
            model_definitions.append(definition[0])
        code = """
#include <stdint.h>
#include <stddef.h>
#include <assert.h>
typedef uint32_t u32;
typedef int BehaviorScript;
typedef int ModelID32;
typedef int ModelID16;
typedef int s32;
#define LEVEL_PHYSICS_LAB 32
#define ARRAY_COUNT(a) (sizeof(a) / sizeof((a)[0]))
static int gCurrLevelNum;
// Bias host pointers to distinguish raw and virtual identities; no N64 mapping runs.
static void *segmented_to_virtual(const void *p) {
    return (void *) ((uintptr_t) p + 0x10000000);
}
// This fixture isolates Lab remapping; the separate Bowser suite owns its private bank.
static void *bowser_arena_actor_asset(const BehaviorScript *behavior, const void *asset) {
    (void) behavior;
    return (void *) asset;
}
static ModelID32 bowser_arena_actor_model(const BehaviorScript *behavior, ModelID32 model) {
    (void) behavior;
    return model;
}
struct Object {
    const BehaviorScript *behavior;
    struct {
        struct {
            void *sharedChild;
        } gfx;
    } header;
};
static struct Object object, *o = &object;
static void *gLoadedGraphNodes[256];
""" + stub_declarations + "\n" + "\n".join(model_definitions) + "\n"
        code += Source(ROOT / "levels/physics_lab/enemy_assets.h").declaration(
            "enum", "PhysicsLabEnemyModel"
        )
        code += (
            lab_source.declaration("struct", "LabAssetRemap")
            + lab_source.declaration("struct", "LabModelRemap")
        )
        code += TABLE[TABLE.index("static const struct LabAssetRemap"):]
        code += (
            lab_source.function("physics_lab_actor_asset")
            + lab_source.function("physics_lab_actor_model")
        )
        code += "\n".join(
            object_helpers_source.function(name)
            for name in ("obj_set_model", "cur_obj_set_model", "obj_has_model", "cur_obj_has_model")
        )
        code += """
int main(void) {
    for (int i = 0; i < 256; i++) {
        gLoadedGraphNodes[i] = (void *) (uintptr_t) (i + 1);
    }
    for (int level = 1; level <= 32; level++) {
        gCurrLevelNum = level;
        for (u32 i = 0; i < ARRAY_COUNT(sLabAssets); i++) {
            const struct LabAssetRemap *remap = &sLabAssets[i];
            assert(physics_lab_actor_asset(segmented_to_virtual(remap->behavior), remap->original)
                   == (level == 32 ? remap->replacement : remap->original));
            assert(physics_lab_actor_asset(remap->behavior, remap->original) == remap->original);
            assert(physics_lab_actor_asset(NULL, remap->original) == remap->original);
        }
        for (u32 i = 0; i < ARRAY_COUNT(sLabModels); i++) {
            const struct LabModelRemap *remap = &sLabModels[i];
            object.behavior = segmented_to_virtual(remap->behavior);
            int expectedModel = level == 32 ? remap->replacement : remap->original;
            assert(physics_lab_actor_model(object.behavior, remap->original) == expectedModel);
            cur_obj_set_model(remap->original);
            assert(object.header.gfx.sharedChild == gLoadedGraphNodes[expectedModel]);
            assert(cur_obj_has_model(remap->original));
            object.header.gfx.sharedChild = NULL;
            assert(!obj_has_model(&object, remap->original));
        }
    }
    return 0;
}
"""
        with tempfile.TemporaryDirectory(prefix="lab-assets-") as directory:
            path = Path(directory)
            (path / 'test.c').write_text(code)
            subprocess.run(
                ["cc", "-std=c99", "-Wall", "-Wextra", "-Werror",
                 str(path / "test.c"), "-o", str(path / "test")], check=True
            )
            subprocess.run([str(path / "test")], check=True)

    def test_built_asset_addresses_are_unambiguous(self):
        # Addresses describe this existing build, which may predate checkout changes.
        built_elf = ROOT / "build/us_n64/sm64.elf"
        if not built_elf.exists():
            self.skipTest("No local ROM build; source/host checks still run")
        nm_output = subprocess.check_output(["nm", str(built_elf)], text=True)
        symbol_addresses = {
            symbol_name: int(address, 16)
            for address, symbol_name in re.findall(r"^(\w+) \w (\w+)$", nm_output, re.M)
        }
        remap_keys = set()
        for behavior, original_symbol, replacement_symbol in ASSETS:
            # Runtime lookup combines behavior identity with the original asset address.
            remap_key = (symbol_addresses[behavior], symbol_addresses[original_symbol])
            self.assertNotIn(remap_key, remap_keys, (behavior, original_symbol))
            remap_keys.add(remap_key)
            self.assertEqual(symbol_addresses[replacement_symbol] >> 24, 7, replacement_symbol)
        for object_file in ("leveldata.o", "geo.o"):
            undefined_symbols_output = subprocess.check_output(
                ["nm", "-u", str(built_elf.parent / "levels/physics_lab" / object_file)], text=True
            )
            # No private enemy reference may still point into an unloaded
            # original actor bank (segments 5/6 or geo segments 12/13).
            for symbol in re.findall(r"U (\w+)", undefined_symbols_output):
                self.assertNotIn(symbol_addresses[symbol] >> 24, (5, 6, 12, 13), symbol)

    def test_cap_supplies_are_complete_trios(self):
        script_source = (ROOT / "levels/physics_lab/script.c").read_text()
        for orientation, next_definition in (
            ("ROW", "#define LAB_CAP_COLUMN"), ("COLUMN", "const LevelScript")
        ):
            cap_macro = script_source.split(
                "#define LAB_CAP_" + orientation, 1
            )[1].split(next_definition, 1)[0]
            for cap in ("WING", "METAL", "VANISH"):
                self.assertEqual(cap_macro.count(f"EXCLAMATION_BOX_BP_{cap}_CAP"), 1)
        area_script = script_source.split("AREA(1,", 1)[1]
        # Independent shell boxes are allowed; cap boxes must use the trio.
        self.assertEqual(
            re.findall(r"LAB_CAP\((EXCLAMATION_BOX_BP_\w+)", area_script),
            ["EXCLAMATION_BOX_BP_KOOPA_SHELL"] * 4
        )
        self.assertEqual(area_script.count("LAB_CAP_ROW("), 11)
        self.assertEqual(area_script.count("LAB_CAP_COLUMN("), 1)

    def test_roster_excludes_bosses_and_has_needed_children(self):
        roster_source = (ROOT / "levels/physics_lab/enemies.inc.c").read_text()
        for boss_name in (
            "Bowser", "KingBobomb", "WhompKingBoss", "BigBully", "BigChillBully",
            "GhostHuntBigBoo", "Eyerok", "Wiggler"
        ):
            self.assertNotIn("bhv" + boss_name, roster_source)
        for behavior, model in (
            ("Spiny", "SPINY_BALL"), ("Spiny", "SPINY"),
            ("MrIIris", "MR_I_IRIS"), ("PokeyBodyPart", "POKEY_BODY_PART"),
            ("CloudPart", "FWOOSH"), ("ChainChompChainPart", "METALLIC_BALL"),
            ("WoodenPost", "WOODEN_POST"), ("Bub", "BUB"),
            ("FlyingBookend", "BOOKEND"), ("WaterBomb", "WATER_BOMB")
        ):
            self.assertIn(("bhv" + behavior, "MODEL_" + model, "LAB_MODEL_" + model), MODELS)
        self.assertIn("bhvBubSpawner", roster_source)
        self.assertRegex(
            roster_source, r"OBJECT\(MODEL_YELLOW_COIN, -?\d+, 0, -?\d+, 0, 0, 0, 0, bhvMoneybagHidden\)"
        )
        self.assertEqual(roster_source.count("bhvPhysicsLabChests"), 2)
        for behavior, model in (
            ("TreasureChestBottom", "TREASURE_CHEST_BASE"), ("TreasureChestTop", "TREASURE_CHEST_LID")
        ):
            self.assertIn(("bhv" + behavior, "MODEL_" + model, "LAB_MODEL_" + model), MODELS)
        self.assertEqual(roster_source.count("bhvMontyMoleHole"), 3)


if __name__ == "__main__":
    unittest.main()
