"""Check the common model-bank dependency behind Fight 3 encounter spawns."""
from pathlib import Path
import re
import unittest

ROOT = Path(__file__).resolve().parents[1]


class BowserArenaAssetsTests(unittest.TestCase):
    def test_common_models_have_area_local_geometry_and_resident_assets(self):
        script = (ROOT / 'levels/bowser_3/script.c').read_text()
        entry = script[script.index('const LevelScript level_bowser_3_entry'):]
        pool = entry.index('ALLOC_LEVEL_POOL()')
        area = entry.index('AREA(')
        for bank in ('_common0_yay0SegmentRomStart', '_common0_geoSegmentRomStart'):
            self.assertLess(entry.index(bank), pool)
        registration = entry.index('JUMP_LINK(script_func_global_1)')
        self.assertGreater(registration, pool)
        self.assertLess(registration, area)
        models = (ROOT / 'levels/scripts.c').read_text()
        common = models[models.index('const LevelScript script_func_global_1'):]
        common = common[:common.index('RETURN()')]
        for model in ('MODEL_GOOMBA', 'MODEL_BLACK_BOBOMB', 'MODEL_AMP'):
            self.assertRegex(common, rf'LOAD_MODEL_FROM_GEO\({model}\s*,')
        self.assertRegex(entry[:area], r'LOAD_MODEL_FROM_GEO\(MODEL_BOWLING_BALL,\s*bowling_ball_geo\)')


if __name__ == '__main__':
    unittest.main()
