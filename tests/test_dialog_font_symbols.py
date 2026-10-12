#!/usr/bin/env python3
"""Compare custom US/EU glyph pixels using production assets and the EU unpacker.

The host allocator always succeeds; this does not exercise rendering or allocation
failure. Only the plus and slash payloads are retained in the native font table.
"""
from pathlib import Path
import re
import subprocess
import tempfile
import unittest

from regression_support import Source

ROOT = Path(__file__).resolve().parents[1]


class DialogFontSymbols(unittest.TestCase):
    def test_visible_symbols_match_across_regions(self):
        font_source = Source(ROOT / "bin/segment2.c")
        menu_source = Source(ROOT / "src/game/ingame_menu.c")
        dialog_definitions = Source(ROOT / "src/game/ingame_menu.h")
        glyph_start = font_source.text.index("// Hand-drawn plus")
        table_start = font_source.text.index("const Texture *const main_font_lut[]", glyph_start)
        table_end = font_source.text.index("\n};", table_start) + 4
        glyph_definitions = font_source.text[glyph_start:table_start]
        font_table = font_source.text[table_start:table_end]
        # Other glyph payloads are irrelevant; keep their slots and conditionals.
        font_table = re.sub(
            r"\btexture_font_char_\w+\b",
            lambda match: match[0] if match[0] in (
                "texture_font_char_plus", "texture_font_char_forward_slash"
            ) else "NULL",
            font_table,
        )
        fixture_source = r'''
#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
typedef int32_t s32;
typedef int16_t s16;
typedef uint32_t u32;
typedef uint8_t u8;
typedef u8 Texture;
typedef u32 Texture32;
#define ALIGNED8
static u8 unpacked_glyph_pixels[64];
static void *alloc_display_list(u32 size) {
    assert(size == sizeof(unpacked_glyph_pixels));
    return unpacked_glyph_pixels;
}
'''
        fixture_source += glyph_definitions + font_table
        fixture_source += dialog_definitions.macros("DIALOG_CHAR_PLUS|DIALOG_CHAR_FORWARD_SLASH")
        fixture_source += menu_source.function("alloc_ia4_tex_from_i1")
        fixture_source += r'''
int main(void) {
    assert(main_font_lut[DIALOG_CHAR_PLUS] == texture_font_char_plus);
    assert(main_font_lut[DIALOG_CHAR_FORWARD_SLASH] == texture_font_char_forward_slash);
    const u8 glyph_ids[] = { DIALOG_CHAR_PLUS, DIALOG_CHAR_FORWARD_SLASH };
    for (u32 glyph_index = 0; glyph_index < sizeof(glyph_ids); glyph_index++) {
        const Texture *glyph_pixels = main_font_lut[glyph_ids[glyph_index]];
#ifdef VERSION_EU
        glyph_pixels = (Texture *)alloc_ia4_tex_from_i1((Texture *)glyph_pixels, 8, 8);
#endif
        for (s32 pixel_index = 0; pixel_index < 64; pixel_index++) {
            printf("%02x", glyph_pixels[pixel_index]);
        }
        puts("");
    }
    return 0;
}
'''
        region_outputs = {}
        with tempfile.TemporaryDirectory(prefix="sm64-dialog-symbols-") as temporary_directory:
            fixture_directory = Path(temporary_directory)
            fixture_path = fixture_directory / "symbols.c"
            fixture_path.write_text(fixture_source)
            for region in ("us", "eu"):
                binary_path = fixture_directory / region
                command = [
                    "cc", "-DVERSION_" + region.upper(), "-std=c11", "-Wall", "-Wextra", "-Werror",
                    "-fsanitize=undefined", str(fixture_path), "-o", str(binary_path),
                ]
                subprocess.run(command, check=True)
                region_outputs[region] = subprocess.check_output([str(binary_path)]).decode().splitlines()
        glyph_names = ("plus", "slash")
        # Validate completeness before zip can silently omit a missing glyph.
        for region, glyph_lines in region_outputs.items():
            with self.subTest(region=region):
                self.assertEqual(len(glyph_lines), len(glyph_names))
                for pixel_hex in glyph_lines:
                    self.assertEqual(len(bytes.fromhex(pixel_hex)), 64)
        for glyph, us_pixels, eu_pixels in zip(glyph_names, region_outputs["us"], region_outputs["eu"]):
            with self.subTest(glyph=glyph):
                self.assertTrue(any(bytes.fromhex(us_pixels)))
                self.assertEqual(eu_pixels, us_pixels)


if __name__ == "__main__":
    unittest.main()
