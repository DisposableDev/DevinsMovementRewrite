#!/usr/bin/env python3
"""Opt-in Fire Spitter physics trial; host checks do not measure N64 performance."""
from pathlib import Path
import subprocess
import tempfile
import unittest
from regression_support import Source

ROOT = Path(__file__).resolve().parents[1]


class FireSpitterSleepTests(unittest.TestCase):
    def test_native_actions_and_physics_boundaries(self):
        source = Source(ROOT / 'src/game/behaviors/fire_spitter.inc.c')
        names = ('fire_spitter_act_idle', 'fire_spitter_act_spit_fire',
                 'bhv_fire_spitter_update')
        functions = '\n'.join(source.function(name) for name in names)
        helper = source.function('fire_spitter_physics_can_sleep')
        functions = '#if FIRE_SPITTER_SLEEP_TRIAL\n' + helper + '\n#endif\n' + functions
        fixture = (ROOT / 'tests/fire_spitter_sleep_test.c').read_text()
        fixture = fixture.replace('../include/object_constants.h', 'object_constants.h')
        with tempfile.TemporaryDirectory(prefix='fire-spitter-sleep-') as directory:
            path = Path(directory)
            (path / 'test.c').write_text(fixture.replace('/* SOURCES */', functions))
            for enabled in (0, 1):
                subprocess.run(['cc', '-std=c99', '-O2', '-Wall', '-Wextra', '-Werror',
                                '-I', str(ROOT / 'include'),
                                f'-DFIRE_SPITTER_SLEEP_TRIAL={enabled}',
                                str(path / 'test.c'), '-lm', '-o', str(path / 'test')], check=True)
                subprocess.run([str(path / 'test')], check=True)


if __name__ == '__main__':
    unittest.main()
