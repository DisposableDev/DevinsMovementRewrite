#!/usr/bin/env python3
"""Native pause framebuffer commands and lifecycle; no RDP rasterization."""

import os
from pathlib import Path
import shlex
import subprocess
import tempfile
import unittest

from regression_support import Source

ROOT = Path(__file__).resolve().parents[1]


def prepare_pause_blend_fixture(directory):
    source = Source(ROOT / "src/game/game_init.c")
    actual = Source(ROOT / "src/game/level_update.h").declaration("enum", "PlayModes")
    actual += Source(ROOT / "src/game/emutest.h").region(r"^enum Emulator \{", r"^enum SystemCapabilities \{")
    actual += Source(ROOT / "src/game/game_init.h").macros("PAUSE_BLEND_FRAMES")
    actual += source.macros("PAUSE_BLEND_.*")
    actual += source.region(r"^static struct \{",
                            r"^static Gfx \*pause_blend_draw_image")
    actual += "\n".join(source.function(name) for name in (
        "pause_blend_draw_image", "pause_blend_draw_framebuffer", "pause_blend_end_display_list",
        "capture_input_display_background",
        "prepare_pause_screen_blend", "render_pause_screen_blend", "render_pause_world_native"))
    camera = Source(ROOT / "src/game/camera.c")
    actual += Source(ROOT / "src/game/camera.h").macros("ZOOMOUT_AREA_MASK")
    actual += camera.macros("PAUSE_CAMERA_MIN_MOVE_SPEED")
    actual += camera.region(r"^u8 sZoomOutAreaMasks\[\] =",
                            r"^STATIC_ASSERT\(ARRAY_COUNT\(sZoomOutAreaMasks\)")
    actual += camera.declaration("struct", "PausedCameraView")
    actual += "\nstatic struct PausedCameraView sPausedCameraView;\n"
    actual += "static s32 sPausedCameraUsesNormalView;\n"
    actual += camera.function("zoom_out_if_paused_and_outside")
    actual += camera.function("update_graph_node_camera")
    (directory / "pause_blend_actual.inc.c").write_text(actual)


class PauseScreenBlend(unittest.TestCase):
    def test_framebuffer_commands_and_repeated_transitions(self):
        with tempfile.TemporaryDirectory(prefix="pause-screen-blend-") as directory:
            directory = Path(directory)
            prepare_pause_blend_fixture(directory)
            command = shlex.split(os.environ.get("CC", "cc")) + [
                "-std=gnu99", "-D_LANGUAGE_C", "-Wall", "-Wextra", "-Werror",
                "-Wno-pointer-to-int-cast",
                "-I", str(ROOT / "include"), "-I", str(ROOT / "include/n64"),
                "-I", str(directory), str(ROOT / "tests/pause_screen_blend_test.c")]
            for name, defines in (("fast3d", []), ("f3dex2", ["-DF3DEX_GBI_2"])):
                binary = directory / name
                subprocess.run(command + defines + ["-o", str(binary)], check=True)
                subprocess.run([str(binary)], check=True)

    def test_compositing_surrounds_the_complete_scene(self):
        # Verify integration order separately from the extracted command fixture.
        render = Source(ROOT / "src/game/area.c").function("render_game")
        self.assertLess(render.index("prepare_pause_screen_blend("),
                        render.index("geo_process_root("))
        for drawing in ("render_hud();", "render_menus_and_dialogs();",
                        "render_screen_transition(", "s2d_handle_deferred();",
                        "prepare_debug_overlay();"):
            self.assertLess(render.index(drawing), render.index("render_pause_screen_blend();"))

        self.assertLess(render.index("geo_process_root("), render.index("render_pause_world_native("))
        self.assertLess(render.index("render_pause_world_native("), render.index("render_hud();"))
        self.assertLess(render.index("render_pause_world_native("),
                        render.index("render_menus_and_dialogs();"))

        master = Source(ROOT / "src/game/game_init.c").function("end_master_display_list")
        self.assertLess(master.index("capture_input_display_background();"),
                        master.index("render_input_display();"))
        self.assertLess(master.index("render_input_display();"),
                        master.index("sDebugOverlayDisplayList);"))


if __name__ == "__main__":
    unittest.main()
