"""One airborne update before a buffered forward dive from a butt-slide jump."""
from pathlib import Path
import subprocess
import tempfile
import unittest

from regression_support import Source

ROOT = Path(__file__).resolve().parents[1]


class ButtSlideDiveBufferTests(unittest.TestCase):
    def test_delay_buffer_and_other_attack_paths(self):
        source = Source(ROOT / "src/game/mario_actions_airborne.c")
        functions = Source(ROOT / "src/game/mario_actions_airborne.h").macros("ACT_ARG_.*")
        functions += source.function("check_kick_or_dive_in_air")
        fixture = r'''
#include <assert.h>
#include <stdint.h>
typedef int32_t s32;
typedef uint32_t u32;
#define FALSE 0
#define ACT_JUMP 1
#define ACT_HOLD_JUMP 2
#define ACT_STEEP_JUMP 3
#define ACT_JUMP_KICK 4
#define ACT_DIVE 5
#define ACT_BUTT_SLIDE 6
#define ACT_WALKING 7
#define INPUT_B_PRESSED 1
struct MarioState {u32 action, prevAction, actionArg, input; int diveInput;};
static int transitions;
static int mario_has_dive_input(struct MarioState *m) {return m->diveInput;}
static int set_mario_action(struct MarioState *m, u32 action, u32 arg) {
    m->prevAction=m->action; m->action=action; m->actionArg=arg; transitions++; return 1;
}
/* FUNCTIONS */
int main(void) {
    const u32 actions[] = {ACT_JUMP, ACT_STEEP_JUMP};
    for (int i=0;i<2;i++) {
        struct MarioState m={.action=actions[i],.prevAction=ACT_BUTT_SLIDE,
                             .input=INPUT_B_PRESSED,.diveInput=1};
        transitions=0;
        assert(!check_kick_or_dive_in_air(&m));
        assert(m.action==actions[i] && transitions==0);
        // A short tap and subsequent stick release retain the requested dive.
        m.input=0; m.diveInput=0;
        assert(check_kick_or_dive_in_air(&m));
        assert(m.action==ACT_DIVE && m.actionArg==0 && transitions==1);
        // First update without B still completes the restriction.
        m=(struct MarioState){.action=actions[i],.prevAction=ACT_BUTT_SLIDE,.diveInput=1};
        assert(!check_kick_or_dive_in_air(&m));
        m.input=INPUT_B_PRESSED;
        assert(check_kick_or_dive_in_air(&m) && m.action==ACT_DIVE);
        // An intervening action discards the action-local queue.
        m=(struct MarioState){.action=actions[i],.prevAction=ACT_BUTT_SLIDE,
                             .input=INPUT_B_PRESSED,.diveInput=1};
        assert(!check_kick_or_dive_in_air(&m));
        set_mario_action(&m,ACT_WALKING,0); m.input=0;
        assert(!check_kick_or_dive_in_air(&m));
    }
    struct MarioState m={.action=ACT_JUMP,.prevAction=ACT_WALKING,
                         .input=INPUT_B_PRESSED,.diveInput=1};
    assert(check_kick_or_dive_in_air(&m) && m.action==ACT_DIVE);
    m=(struct MarioState){.action=ACT_JUMP,.prevAction=ACT_BUTT_SLIDE,.input=INPUT_B_PRESSED,
                         .actionArg=ACT_ARG_JUMP_BACKWARD_BUTT_SLIDE};
    assert(check_kick_or_dive_in_air(&m) && m.action==ACT_JUMP_KICK);
    assert(m.actionArg==ACT_ARG_JUMP_KICK_BACKWARD_BUTT_SLIDE);
    return 0;
}
'''.replace("/* FUNCTIONS */", functions)
        with tempfile.TemporaryDirectory(prefix="butt-slide-dive-") as name:
            path = Path(name)
            (path / "test.c").write_text(fixture)
            result = subprocess.run(
                ["cc", "-std=c11", "-Wall", "-Wextra", "-Werror", "-fsanitize=undefined",
                 str(path / "test.c"), "-o", str(path / "test")],
                capture_output=True, text=True, timeout=30)
            self.assertEqual(result.returncode, 0, result.stderr)
            result = subprocess.run([str(path / "test")], capture_output=True, text=True, timeout=10)
            self.assertEqual(result.returncode, 0, result.stderr)


if __name__ == "__main__":
    unittest.main()
