"""Actual queued-health updates and predictor; host environment/effects are controlled."""
from pathlib import Path
import subprocess
import tempfile
import unittest

from run_regressions import prepare
from regression_support import Source

ROOT = Path(__file__).resolve().parents[1]


class AuditHealthFixTests(unittest.TestCase):
    def _check_shallow_probe(self, probe):
        with tempfile.TemporaryDirectory(prefix='audit-shallow-health-') as temporary:
            folder = Path(temporary)
            prepare(ROOT, 'fixes', folder, {})
            fixture = (ROOT / 'tests/bugfix_transitions_test.c').read_text()
            fixture = fixture.replace('int main(int argc, char **argv)',
                                      'int inherited_main(int argc, char **argv)')
            fixture = fixture.replace('#define game_options_secrets_blocked() 0',
                                      'static int secretsBlocked;\n'
                                      '#define game_options_secrets_blocked() secretsBlocked')
            # Execute the production pre-action branch after the actual health update.
            # Collision and sounds are controlled; secret blocking is a supplied boundary.
            mario = Source(ROOT / 'src/game/mario.c')
            pre_action = mario.region(
                r'^        if \(mario_should_drown_on_land\(gMarioState\)\)',
                r'^\s*mario_handle_special_floors\(gMarioState\);')
            (folder / 'probe.c').write_text(
                fixture + '\nstatic void check_land_death(void) {\n' + pre_action
                + '\n}\nint main(void) {\n' + probe + '\nreturn 0;\n}\n')
            result = subprocess.run(
                ['cc', '-std=c99', '-Wall', '-Wextra', '-Werror', '-Wno-unused-parameter',
                 '-I', str(folder), '-I', str(ROOT / 'tests'), str(folder / 'probe.c'),
                 '-o', str(folder / 'probe'), '-lm'],
                capture_output=True, text=True, timeout=30)
            self.assertEqual(result.returncode, 0, result.stderr)
            result = subprocess.run([str(folder / 'probe')],
                                    capture_output=True, text=True, timeout=10)
            self.assertEqual(result.returncode, 0, result.stderr)

    def test_shallow_sink_requires_fatal_water_drain(self):
        self._check_shallow_probe(r'''
    for (int blocked = 0; blocked < 2; blocked++) {
        secretsBlocked = blocked;
        for (int cause = 0; cause < 3; cause++) {
            begin(ACT_CROUCHING);
            m.health = 0x180;
            m.waterLevel = 80;
            m.flags = MARIO_NORMAL_CAP | MARIO_CAP_ON_HEAD;
            sWaterHealth.previousHealth = m.health;
            if (cause == 0) {
                m.input = INPUT_IN_POISON_GAS;
                sPoisonHealth.drainProgress = WATER_HEALTH_WEDGE - POISON_HEALTH_DRAIN_TICKS;
            } else if (cause == 1) {
                m.hurtCounter = 1;
            } else {
                sWaterHealth.drainProgress = WATER_HEALTH_WEDGE_INTERVAL - WATER_HEALTH_DRAIN_TICKS;
            }
            update_mario_health(&m);
            assert(m.health == 0xFF);
            assert(m.action == ACT_CROUCHING);
            check_land_death();
            if (cause == 2 && !blocked) {
                assert(m.action == ACT_QUICKSAND_DEATH);
                assert(m.actionArg == ACT_ARG_QUICKSAND_DEATH_SHALLOW_WATER);
            } else {
                assert(m.action == ACT_DROWNING);
            }
            // A reload/refill reset must not attribute a later death to this water loss.
            mario_reset_health_regeneration(&m);
            m.action = ACT_CROUCHING;
            check_land_death();
            assert(m.action == ACT_DROWNING);
        }
        // Ordinary deep water and survivable shallow loss keep their reactions.
        begin(ACT_CROUCHING);
        m.health = 0x180;
        m.waterLevel = 101;
        m.flags = MARIO_NORMAL_CAP | MARIO_CAP_ON_HEAD;
        sWaterHealth.previousHealth = m.health;
        sWaterHealth.drainProgress = WATER_HEALTH_WEDGE_INTERVAL - WATER_HEALTH_DRAIN_TICKS;
        update_mario_health(&m);
        check_land_death();
        assert(m.action == ACT_DROWNING);
        begin(ACT_DAB);
        m.waterLevel = 80;
        m.flags = MARIO_NORMAL_CAP | MARIO_CAP_ON_HEAD;
        sWaterHealth.drainProgress = WATER_HEALTH_WEDGE_INTERVAL - WATER_HEALTH_DRAIN_TICKS;
        update_mario_health(&m);
        assert(m.action == ACT_COUGHING);
        assert(m.actionArg == ACT_ARG_COUGHING_SHALLOW_WATER);
    }
''')

    def test_blocked_secrets_preserve_shallow_regeneration_delay(self):
        self._check_shallow_probe(r'''
    for (int blocked = 0; blocked < 2; blocked++) {
        secretsBlocked = blocked;
        for (int exposure = 89; exposure <= 90; exposure++) {
            begin(ACT_WALKING);
            m.health = 0x480;
            mario_reset_health_regeneration(&m);
            advance_health_updates(1349);
            m.action = ACT_CRAWLING;
            m.waterLevel = 80;
            advance_health_updates(exposure);
            check_land_death();
            m.action = ACT_WALKING;
            m.waterLevel = 0;
            update_mario_health(&m);
            assert(m.health == (exposure == 89 ? 0x580 : 0x480));
        }
    }
''')

    def test_prediction_matches_ticks_and_respawn_refill_is_not_restoration(self):
        with tempfile.TemporaryDirectory(prefix='audit-health-fixes-') as temporary:
            folder = Path(temporary)
            prepare(ROOT, 'fixes', folder, {})
            fixture = (ROOT / 'tests/bugfix_transitions_test.c').read_text()
            fixture = fixture.replace('int main(int argc, char **argv)', 'int inherited_main(int argc, char **argv)')
            accounting = r'''
#undef player_stats_health
#undef player_stats_health_non_restoring
static int restoreCalls,discardedRefillTicks;
static void record_health(int before,int after) {if(after>before && before<0x880) restoreCalls++;}
#define player_stats_health(before,after) record_health(before,after)
#define player_stats_health_non_restoring(before,after) (discardedRefillTicks++)
'''
            fixture = fixture.replace('#include "fixes_sources.inc"', accounting + '\n#include "fixes_sources.inc"')
            probe = r'''
#include <assert.h>
int main(void) {
    const int hurts[]={0,1,4,8,9,20,255};
    const int heals[]={0,1,4,20,31,255};
    const int healths[]={0xFF,0x100,0x140,0x180,0x280,0x840,0x880};
    for(unsigned h=0;h<sizeof(healths)/sizeof(*healths);h++)
    for(unsigned d=0;d<sizeof(hurts)/sizeof(*hurts);d++)
    for(unsigned c=0;c<sizeof(heals)/sizeof(*heals);c++) {
        begin(ACT_BACKWARD_AIR_KB);
        m.health=healths[h];m.hurtCounter=hurts[d];m.healCounter=heals[c];
        m.flags=MARIO_NORMAL_CAP|MARIO_CAP_ON_HEAD;m.waterLevel=-10000;m.pos[1]=500;
        sWaterHealth.previousHealth=m.health;
        int predicted=mario_health_after_pending_damage(&m);
        for(int frame=0;frame<512 && (m.hurtCounter || m.healCounter);frame++) update_mario_health(&m);
        assert((predicted>=0x100)==(m.health>=0x100));
        assert(predicted==m.health);
    }
    begin(ACT_BACKWARD_AIR_KB);
    m.health=0x100;m.flags=MARIO_NORMAL_CAP|MARIO_CAP_ON_HEAD;m.waterLevel=-10000;m.pos[1]=500;
    sWaterHealth.previousHealth=m.health;restoreCalls=discardedRefillTicks=0;
    mario_queue_respawn_healing(&m);
    for(int frame=0;frame<31;frame++) update_mario_health(&m);
    assert(m.health==0x880 && !m.healCounter && !m.respawnHealCounter);
    assert(restoreCalls==0 && discardedRefillTicks==31);
    // A genuine heal after the refill still counts normally.
    m.health=0x480;m.healCounter=4;sWaterHealth.previousHealth=m.health;
    for(int frame=0;frame<4;frame++) update_mario_health(&m);
    assert(restoreCalls==4 && m.health==0x580);
    return 0;
}
'''
            (folder / 'probe.c').write_text(fixture + probe)
            result = subprocess.run(['cc','-std=c99','-Wall','-Wextra','-Werror','-Wno-unused-parameter',
                '-I',str(folder),'-I',str(ROOT/'tests'),str(folder/'probe.c'),'-o',str(folder/'probe'),'-lm'],
                capture_output=True,text=True,timeout=30)
            self.assertEqual(result.returncode,0,result.stderr)
            result = subprocess.run([str(folder/'probe')],capture_output=True,text=True,timeout=10)
            self.assertEqual(result.returncode,0,result.stderr)


if __name__=='__main__':
    unittest.main()
