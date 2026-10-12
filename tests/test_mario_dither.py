"""Check dither coverage for inherited Mario materials at every model detail."""
from pathlib import Path
import re
import sys
import subprocess
import tempfile
import unittest

ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT / 'tools'))
from generate_mario_silhouette import ARRAY, palette_source

try:
    from .source_asset import read_text as read_source
except ImportError:
    from source_asset import read_text as read_source


class MarioDitherCoverage(unittest.TestCase):
    def test_every_drawn_part_has_a_threshold_wrapper(self):
        base = ROOT / 'actors/mario'
        geo = read_source(base / 'geo.inc.c')
        material = read_source(base / 'model.inc.c')
        for palette, body in (('', ''), ('he_thicc_', 'he_thicc_body_')):
            geo += palette_source(read_source(base / 'palette_geo.inc.c'), palette, body)
            material += palette_source(read_source(base / 'palette_materials.inc.c'), palette, body)
        material += read_source(base / 'he_thicc_body.inc.c')
        ordinary = {match[1] for match in ARRAY.finditer(material)}
        roots = set(re.findall(
            r'GEO_(?:DISPLAY_LIST|ANIMATED_PART)\([^\n]*,\s*(\w+)\)', geo)) & ordinary
        mapping = dict(re.findall(r'\{ (\w+), (\w+), \w+ \}',
                                  read_source(base / 'dither_map.inc.c')))
        wrappers = {match[1]: match[2] for match in ARRAY.finditer(
            read_source(base / 'dither.inc.c'))}
        self.assertGreater(len(roots), 100)
        for root in roots:
            with self.subTest(part=root):
                self.assertIn(root, mapping)
                wrapper = wrappers[mapping[root]]
                self.assertIn('gsDPSetCycleType(G_CYC_2CYCLE)', wrapper)
                self.assertIn('gsSPSetOtherMode(G_SETOTHERMODE_L, 3, 1, 0)', wrapper)
                self.assertIn('gsDPSetAlphaCompare(G_AC_THRESHOLD)', wrapper)
                self.assertIn('gsDPSetAlphaCompare(G_AC_NONE)', wrapper)
                self.assertIn('gsDPSetCycleType(G_CYC_1CYCLE)', wrapper)

    def test_compatibility_changes_only_alpha_comparison(self):
        base = ROOT / 'actors/mario'
        wrappers = {match[1]: match[2] for match in ARRAY.finditer(
            read_source(base / 'dither.inc.c'))}
        for ordinary, ordered, noise in re.findall(
                r'\{ (\w+), (\w+), (\w+) \}', read_source(base / 'dither_map.inc.c')):
            with self.subTest(part=ordinary):
                self.assertEqual(wrappers[noise], wrappers[ordered].replace(
                    'G_AC_THRESHOLD', 'G_AC_DITHER'))

    def test_opacity_range_and_native_preservation(self):
        source = r"""
#define EMUTEST_H
#define EMU_MUPEN 16
#include <assert.h>
typedef int s32;
static unsigned gEmulator;
#include "mario_dither.h"
int main(void) {
    for (unsigned emulator = 1; emulator <= 64; emulator <<= 1) {
        gEmulator = emulator;
        for (int alpha = 0; alpha <= 255; alpha++) {
            int cutout = mario_dither_alpha(alpha);
            if (emulator == EMU_MUPEN) assert(cutout == alpha);
            else assert(cutout == (alpha + 16) / 32);
        }
    }
}
"""
        with tempfile.TemporaryDirectory() as folder:
            path = Path(folder)
            (path / 'check.c').write_text(source)
            subprocess.run(['cc', '-Wall', '-Wextra', '-Werror', '-I',
                            str(ROOT / 'src/game'), str(path / 'check.c'),
                            '-o', str(path / 'check')], check=True)
            subprocess.run([str(path / 'check')], check=True)


if __name__ == '__main__':
    unittest.main()
