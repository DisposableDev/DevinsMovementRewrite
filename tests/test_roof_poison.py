#!/usr/bin/env python3
"""Rooftop encounter host state checks; movement, audio and graphics are boundaries."""
from pathlib import Path
import ast
import re
import subprocess
import tempfile
import unittest

from PIL import Image
from regression_support import Source

ROOT = Path(__file__).resolve().parents[1]


def prepare_roof_poison_fixture(folder):
    yoshi = Source(ROOT / "src/game/behaviors/yoshi.inc.c")
    constants = Source(ROOT / "include/object_constants.h")
    production = constants.macros("CASTLE_ROOF_COMPLETION_STARS|YOSHI_CREDIT_SIGN_ACT_.*")
    production += constants.declaration("enum", "oBehParams2ndByte1UpMushroom")
    production += Source(ROOT / "src/game/save_file.h").macros("NUM_SAVE_FILES|SAVE_CAKE_.*|CASTLE_SECRET_.*")
    save = Source(ROOT / "src/game/save_file.c")
    production += save.function("mark_save_file_modified")
    production += save.function("save_file_set_star_flags")
    production += save.function("save_file_is_cannon_unlocked")
    production += save.function("save_file_set_cannon_unlocked")
    production += save.function("save_file_has_castle_wing_cap")
    production += save.function("save_file_unlock_castle_wing_cap")
    production += save.declaration("struct", "CastleSecretSaveBit")
    courses = re.search(r"static const struct CastleSecretSaveBit sCastleSecretSaveBits\[\] = \{.*?\};", save.text, re.S)
    production += save.mapped(courses.start(), courses.end())
    production += save.function("save_file_get_castle_secret")
    production += save.function("save_file_set_castle_secret")
    production += save.function("save_file_flush_castle_secret")
    production += save.function("save_file_reload")
    mushroom = Source(ROOT / "src/game/behaviors/mushroom_1up.inc.c")
    production += mushroom.function("bhv_1up_common_init")
    production += mushroom.function("bhv_1up_init")
    production += mushroom.function("bhv_1up_interact")
    production += mushroom.function("bhv_1up_loop")
    production += Source(ROOT / "src/game/behaviors/exclamation_box.inc.c").function(
        "exclamation_box_cap_is_unlocked")
    panel = Source(ROOT / "src/game/behaviors/message_panel.inc.c")
    production += panel.macros("SIGNPOST_.*")
    production += panel.function("bhv_message_panel_loop").replace(
        "void bhv_message_panel_loop(", "static void actual_message_panel_loop(")
    production += panel.macros("CASTLE_LOBBY_SIGN_REVEAL_DISTANCE")
    production += "static int castle_secret_lobby_rejection_dialog(void) {return -1;}\n"
    production += panel.function("bhv_castle_lobby_floor_sign_loop")
    production += yoshi.function("bhv_yoshi_init")
    production += Source(ROOT / "src/game/behaviors/yoshi_credit_sign.inc.c").text
    secret = Source(ROOT / "src/game/behaviors/castle_secret_room.inc.c")
    production += secret.mapped(0, len(secret.text))
    production += Source(ROOT / "src/game/behaviors/mushroom_1up.inc.c").function(
        "one_up_steer_toward_mario")
    production += Source(ROOT / "src/game/behaviors/mushroom_1up.inc.c").function(
        "bhv_poison_mushroom_loop")
    production += Source(ROOT / "src/game/mario_actions_cutscene.h").declaration(
        "enum", "ActionStatesReadingSign")
    production += Source(ROOT / "src/game/mario_actions_cutscene.c").function("act_reading_sign")
    (folder / "roof_poison_actual.inc").write_text(production)


class RoofPoisonTests(unittest.TestCase):
    def test_encounter_and_debug_star_gate(self):
        with tempfile.TemporaryDirectory(prefix="roof-poison-") as directory:
            folder = Path(directory)
            prepare_roof_poison_fixture(folder)
            for debug_define in (None, "UNLOCK_ALL", "COMPLETE_SAVE_FILE"):
                command = ["cc", "-std=c99", "-O2", "-Wall", "-Wextra",
                           "-Wno-unused-function", "-Werror", "-fsanitize=undefined", "-fno-sanitize-recover=all",
                           "-I", str(folder), str(ROOT / "tests/roof_poison_test.c"),
                           "-o", str(folder / "test"), "-lm"]
                if debug_define:
                    command.append(f"-D{debug_define}")
                subprocess.run(command, check=True, timeout=30)
                subprocess.run([str(folder / "test")], check=True, timeout=10)

    def test_each_phrase_gets_its_own_dialogue_box(self):
        text = (ROOT / "text/simpleflips_dialogs.h").read_text().split(
            "DEFINE_DIALOG(DIALOG_YOSHI_CREDIT,", 1)[1].split("DEFINE_DIALOG", 1)[0]
        self.assertRegex(text, r"^\s*1, 2, 30, 200,")
        lines = "".join(ast.literal_eval(s) for s in re.findall(r'"(?:\\.|[^"\\])*"', text)).split("\n")
        pages = [" ".join(lines[i:i + 2]).strip() for i in range(0, len(lines), 2)]
        self.assertEqual(pages, ["Sorry, Mario. I can't give credit.",
                                "Come back when you're a little...", "Mmmmm...", "Richer!"])

    def test_completion_dialogue_has_nine_separate_boxes(self):
        text = (ROOT / "text/simpleflips_dialogs.h").read_text().split(
            "DEFINE_DIALOG(DIALOG_CASTLE_SECRET_COMPLETE,", 1)[1].split("DEFINE_DIALOG", 1)[0]
        self.assertRegex(text, r"^\s*1, 4, 30, 200,")
        lines = "".join(ast.literal_eval(s) for s in re.findall(
            r'"(?:\\.|[^"\\])*"', text)).split("\n")
        pages = [" ".join(lines[i:i + 4]).strip() for i in range(0, len(lines), 4)]
        self.assertEqual(pages, [
            "Wow, I've been waiting here for a long time.",
            "You must be really good at this game.",
            "You might even be... a SUPER PLAYER!",
            "I'd give you a reward... if I had one!",
            "There are still more easter eggs to be found.",
            "(Unless you've found them all, of course.)",
            "Hop in the painting to access the secret level.",
            "As for your friend, Lil Board...",
            "See if you can find me in the next game!",
        ])
        self.assertIn('"- Ms. Board"', (ROOT / "text/simpleflips_dialogs.h").read_text())

    def test_portrait_route_uses_existing_physics_lab_entrance(self):
        grounds = (ROOT / "levels/castle_grounds/script.c").read_text()
        for node in range(3):
            self.assertIn(f"PAINTING_WARP_NODE(0x{node:02X}, LEVEL_PHYSICS_LAB, 1, 0x0A, WARP_NO_CHECKPOINT)", grounds)
        lab = (ROOT / "levels/physics_lab/script.c").read_text()
        self.assertRegex(lab, r"OBJECT\(MODEL_NONE,[^\n]*0x000A0000, bhvSpinAirborneWarp\)")

    def test_purple_cap_preserves_sprite_details(self):
        original = Image.open(ROOT / "actors/mushroom_1up/1-up_mushroom.rgba16.png").convert("RGBA")
        poison = Image.open(ROOT / "actors/mushroom_1up/poison_mushroom.rgba16.png").convert("RGBA")
        self.assertEqual(original.size, poison.size)
        changed = 0
        for before, after in zip(original.getdata(), poison.getdata()):
            self.assertEqual(before[3], after[3])
            if before[1] > before[0] and before[1] > before[2]:
                changed += 1
                self.assertGreater(after[0], after[1])
                self.assertGreater(after[2], after[1])
            else:
                self.assertEqual(before, after)
        self.assertGreater(changed, 0)


if __name__ == "__main__":
    unittest.main()
