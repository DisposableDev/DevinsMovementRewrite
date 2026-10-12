#!/usr/bin/env python3
"""Production lab activity gates, object update bookkeeping and wave emission.

Engine collision, camera visibility and native AI are observed doubles. This
checks conservative and aggressive sleeping, immediate wake-up, transient cleanup
and avoided cosmetic/setup calls,
not target performance or feel.
"""
from pathlib import Path
import re
import subprocess
import tempfile
import unittest
from regression_support import Source

ROOT = Path(__file__).resolve().parents[1]


def prepare_activity_fixture(directory):
    def emit(label, text):
        (directory / ('activity_' + label.lower() + '.inc')).write_text(text + '\n')

    activity_source = Source(ROOT / 'src/game/physics_lab_activity.c')
    fixture_source = (ROOT / 'tests/physics_lab_activity_test.c').read_text()
    activity_body = activity_source.region(r'^#ifndef PHYSICS_LAB_AGGRESSIVE_SLEEP', r'\Z')
    behavior_symbols = sorted(
        set(re.findall(r'\bbhv\w+', activity_body + fixture_source))
        - {'bhvSkeeterWave', 'bhv_exclamation_box_loop'})
    emit('SEGMENT_COUNTS', Source(ROOT / 'include/config/config_objects.h').macros(
        'POKEY_NUM_SEGMENTS|CHAIN_CHOMP_NUM_SEGMENTS'))
    emit('FLOOR_LIMIT', Source(ROOT / 'src/engine/surface_collision.h').macros(
        'FLOOR_LOWER_LIMIT|FLOOR_LOWER_LIMIT_MISC'))
    emit('TIME_STOP', Source(ROOT / 'src/game/object_list_processor.h').declaration(
        'enum', 'gTimeStopStateFlags'))
    emit('BEHAVIORS', '\n'.join(
        (f'#ifdef FLOOMBAS\nstatic const BehaviorScript {behavior_symbol}[] = {{{i+1}}};\n#endif'
         if behavior_symbol == 'bhvFloomba' else f'static const BehaviorScript {behavior_symbol}[] = {{{i+1}}};')
        for i, behavior_symbol in enumerate(behavior_symbols)))
    emit('MOLE_WAKE', Source(ROOT / 'src/game/behaviors/monty_mole.inc.c').function(
        'physics_lab_monty_mole_can_sleep'))
    behavior_data = (ROOT / 'data/behavior_data.c').read_text()
    active_from_afar_symbols = [
        behavior_symbol for behavior_symbol in behavior_symbols
        if (script := re.search(
            r'const BehaviorScript ' + behavior_symbol + r'\[\] = \{(.*?)\n\};',
            behavior_data, re.S))
        and 'OBJ_FLAG_ACTIVE_FROM_AFAR' in script[1]]
    emit('AFAR_SCRIPTS', 'static s32 native_afar_script(const BehaviorScript *behavior) { return '
        + ' || '.join('behavior == ' + behavior_symbol
                      for behavior_symbol in active_from_afar_symbols) + '; }')
    emit('ACTIVITY', activity_body)
    behavior_engine = Source(ROOT / 'src/engine/behavior_script.c')
    emit('UPDATE',
        Source(ROOT / 'src/game/object_platform_motion.h').declaration(
            'enum', 'EnemyPlatformUpdateStatus')
        + behavior_engine.function('cur_obj_update_distance_visibility')
        + behavior_engine.function('cur_obj_update_sleeping_visibility')
        + behavior_engine.function('cur_obj_update'))
    emit('OBJECT_WRAPPER', Source(ROOT / 'src/game/object_list_processor.c').function(
        'update_object_with_platform_motion'))
    emit('WAVES', Source(ROOT / 'src/game/behaviors/skeeter.inc.c').function('skeeter_spawn_waves'))
    emit('BOX', Source(ROOT / 'src/game/behaviors/exclamation_box.inc.c').function('bhv_exclamation_box_loop'))
    emit('BOX_RESPAWN', Source(ROOT / 'src/game/behaviors/exclamation_box.inc.c').function(
        'exclamation_box_act_wait_for_respawn'))


class ActivityTests(unittest.TestCase):
    def test_activity_lifecycle(self):
        for legacy_field in ('labActivityBehavior', 'labActivityKind', 'labSleepDelay'):
            self.assertNotIn(legacy_field, (ROOT / 'include/types.h').read_text())
            self.assertNotIn(legacy_field, (ROOT / 'src/game/spawn_object.c').read_text())
        with tempfile.TemporaryDirectory(prefix='lab-activity-') as directory:
            fixture_directory = Path(directory)
            prepare_activity_fixture(fixture_directory)
            for aggressive, floombas, auto_collision_distance, skeeter_trial, patrol_trial in (
                    (0, False, False, 0, 0), (0, False, True, 0, 0),
                    (0, True, False, 0, 0), (0, True, True, 0, 0),
                    (1, False, False, 0, 0), (1, True, True, 0, 0),
                    (0, False, False, 1, 0),
                    (0, False, False, 0, 2)):
                subprocess.run(['cc', '-std=c99', '-O2', '-Wall', '-Wextra', '-Werror',
                                '-I', str(ROOT / 'include'), '-I', str(fixture_directory),
                                '-DPHYSICS_LAB_AGGRESSIVE_SLEEP=' + str(aggressive),
                                '-DSKEETER_WATER_SLEEP_TRIAL=' + str(skeeter_trial),
                                '-DENEMY_PATROL_SLEEP_TRIAL=' + str(patrol_trial),
                                *(['-DFLOOMBAS'] if floombas else []),
                                *(['-DAUTO_COLLISION_DISTANCE'] if auto_collision_distance else []),
                                str(ROOT / 'tests/physics_lab_activity_test.c'),
                                '-lm', '-o', str(fixture_directory / 'test')], check=True)
                subprocess.run([str(fixture_directory / 'test')], check=True)


if __name__ == '__main__':
    unittest.main()
