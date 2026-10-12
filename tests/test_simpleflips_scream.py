#!/usr/bin/env python3
"""Heavy-fall selection, file lifetime and complete protected PCM stream checks."""
from pathlib import Path
import json
import os
import shlex
import subprocess
import tempfile
import unittest

from regression_support import Source
from test_pause_easter_egg import prepare_pause_easter_egg_fixture

ROOT = Path(__file__).resolve().parents[1]


def compile_and_run(directory, text, audio=False):
    path = directory / "test.c"
    path.write_text(text)
    command = shlex.split(os.environ.get("CC", "cc")) + [
        "-std=gnu99", "-Wall", "-Wextra", "-Werror", "-fno-pie", "-no-pie"]
    if audio:
        command += ["-D_LANGUAGE_C", "-DVERSION_US", "-Wno-pointer-to-int-cast",
                    "-I", str(ROOT / "include"), "-I", str(ROOT / "include/n64"),
                    "-I", str(directory)]
    else:
        command += ["-fsanitize=address,undefined"]
    subprocess.run(command + [str(path), "-o", str(directory / "test")], check=True)
    subprocess.run([str(directory / "test")], check=True,
                   env={**os.environ, "ASAN_OPTIONS": "detect_leaks=0"})


class SimpleFlipsScream(unittest.TestCase):
    def test_heavy_fall_selection_and_file_lifetime(self):
        air = Source(ROOT / "src/game/mario_actions_airborne.c")
        feature = Source(ROOT / "src/game/simpleflips_scream.c")
        header = Source(ROOT / "include/sm64.h")
        actions = ("JUMP|PUTTING_ON_CAP_AIR|DOUBLE_JUMP|TRIPLE_JUMP|BACKFLIP|FREEFALL|"
                   "HOLD_JUMP|HOLD_FREEFALL|SIDE_FLIP|WALL_KICK_AIR|LONG_JUMP|DIVE|AIR_THROW|"
                   "STEEP_JUMP|GROUND_POUND|JUMP_KICK|FLYING_TRIPLE_JUMP|TOP_OF_POLE_JUMP")
        definitions = header.macros("ACT_(?:" + actions + ")")
        definitions += header.declaration("enum", "MarioFlags")
        code = r'''
#include <assert.h>
#include <stddef.h>
#include <stdint.h>
typedef int32_t s32;
typedef uint16_t u16;
#define TRUE 1
#define FALSE 0
#define NUM_SAVE_FILES 4
#define DEGREES(x) (x)
#define SOUND_MARIO_WAAAOOOW 1
struct Object { struct { struct { float cameraToObject[3]; } gfx; } header; };
struct MarioState {
    uint32_t flags, action;
    float waterLevel, floorHeight;
    int faceAngle[3];
    struct Object *marioObj;
};
static void *gCurrDemoInput;
static int cheating;
#define game_options_secrets_blocked() cheating
static u16 sRoll;
static int sRandomCalls, sStarts, sStartAllowed = TRUE, sNormalSounds;
static int sLargeFall = TRUE, sBuried;
static u16 random_u16(void) { sRandomCalls++; return sRoll; }
static s32 simpleflips_scream_pcm_start(void) { sStarts++; return sStartAllowed; }
static int is_large_fall(struct MarioState *m) { (void)m; return sLargeFall; }
static int should_get_stuck_in_ground(struct MarioState *m) { (void)m; return sBuried; }
static void play_sound(int sound, float *source) {
    assert(sound == SOUND_MARIO_WAAAOOOW && source != NULL);
    sNormalSounds++;
}
/* ACTUAL */
int main(void) {
    struct Object marioObject = {0};
    struct MarioState m = { .action = ACT_FREEFALL, .marioObj = &marioObject };
    simpleflips_scream_load_file(1); cheating=1;
    assert(!simpleflips_scream_try_play() && sRandomCalls==0 && sStarts==0);
    cheating=0;
    // Exactly eight of the 65,536 possible roll values select the replacement.
    simpleflips_scream_load_file(2);
    for (unsigned roll = 0; roll < 65536; roll++) {
        simpleflips_scream_load_file((roll & 1) + 1);
        sRoll = roll;
        assert(simpleflips_scream_try_play() == ((roll & 8191) == 0));
    }
    assert(sStarts == 8 && sRandomCalls == 65536);
    simpleflips_scream_load_file(2);
    simpleflips_scream_load_file(1);
    sRoll = 0;
    sRandomCalls = sStarts = sNormalSounds = 0;
    for (int tick = 0; tick < 100; tick++) {
        play_far_fall_sound(&m);
    }
    assert(sStarts == 1 && sRandomCalls == 1 && sNormalSounds == 0);
    assert(m.flags & MARIO_FALL_SOUND_PLAYED);
    // Later falls and same-file reloads retain the spent latch and normal scream.
    for (int fall = 0; fall < 3; fall++) {
        simpleflips_scream_load_file(1);
        m.flags = 0;
        play_far_fall_sound(&m);
    }
    assert(sStarts == 1 && sRandomCalls == 1 && sNormalSounds == 3);
    simpleflips_scream_load_file(0);
    simpleflips_scream_load_file(5);
    assert(!simpleflips_scream_try_play());
    simpleflips_scream_load_file(2);
    m.flags = 0;
    play_far_fall_sound(&m);
    assert(sStarts == 2 && sRandomCalls == 2);
    simpleflips_scream_load_file(1);
    m.flags = 0;
    play_far_fall_sound(&m);
    assert(sStarts == 3 && sRandomCalls == 3);
    // Rejecting an existing warning/action/water/burial/short fall does not roll.
    simpleflips_scream_load_file(2);
    m.flags = MARIO_FALL_SOUND_PLAYED;
    play_far_fall_sound(&m);
    m.flags = 0;
    sLargeFall = FALSE;
    play_far_fall_sound(&m);
    sLargeFall = TRUE;
    m.action = 0;
    play_far_fall_sound(&m);
    m.action = ACT_FREEFALL;
    m.waterLevel = 101;
    play_far_fall_sound(&m);
    m.waterLevel = 0;
    sBuried = TRUE;
    play_far_fall_sound(&m);
    sBuried = FALSE;
    assert(sStarts == 3 && sRandomCalls == 3 && m.flags == 0);
    gCurrDemoInput = &m;
    play_far_fall_sound(&m);
    assert(sRandomCalls == 3 && sNormalSounds == 4);
    gCurrDemoInput = NULL;
    // Audio refusal is an ordinary scream and does not spend this file's chance.
    sStartAllowed = FALSE;
    m.flags = 0;
    play_far_fall_sound(&m);
    assert(sNormalSounds == 5 && !sSimpleFlipsScreamPlayed);
    sStartAllowed = TRUE;
    m.flags = 0;
    play_far_fall_sound(&m);
    assert(sSimpleFlipsScreamPlayed && sNormalSounds == 5);
    // A losing roll still emits the normal voice exactly once for this fall.
    simpleflips_scream_load_file(1);
    sRoll = 1;
    m.flags = 0;
    play_far_fall_sound(&m);
    assert(sNormalSounds == 6 && !sSimpleFlipsScreamPlayed);
    return 0;
}
'''
        actual = feature.region(r"^#define SIMPLEFLIPS_SCREAM_ODDS", r"\Z")
        actual += "\n" + air.function("play_far_fall_sound")
        with tempfile.TemporaryDirectory(prefix="simpleflips-selection-") as folder:
            compile_and_run(Path(folder), code.replace("/* ACTUAL */", definitions + actual))

    def test_complete_assets_survive_other_stream_stops(self):
        for asset in ("simpleflips_scream.s16be", "simpleflips_scream_30k.s16be"):
            with self.subTest(asset=asset), tempfile.TemporaryDirectory(prefix="simpleflips-pcm-") as folder:
                directory = Path(folder)
                prepare_pause_easter_egg_fixture(directory)
                fixture = (ROOT / "tests/pause_easter_egg_test.c").read_text()
                start = fixture.index("/* Independent protected scream fixture")
                end = fixture.index("static u16 random_u16", start)
                assembly = ('.balign 16\n.global gSimpleFlipsScreamPcmStart\ngSimpleFlipsScreamPcmStart:\n'
                            f'.incbin "{ROOT / "sound/pcm" / asset}"\n'
                            '.global gSimpleFlipsScreamPcmEnd\ngSimpleFlipsScreamPcmEnd:\n.space 1296,0\n')
                fixture = fixture[:start] + "__asm__(" + json.dumps(assembly) + ");\n" + fixture[end:]
                fixture = fixture.replace("return sDmaBuffer;",
                                          "memcpy(sDmaBuffer, (void *) address, size); return sDmaBuffer;")
                fixture = fixture.replace("int main(void)", "int short_fixture_main(void)")
                fixture += r'''
int main(void) {
    Acmd commands[16];
    const u32 sizes[] = {256, 272, 288};
    const u32 length = (uintptr_t) gSimpleFlipsScreamPcmEnd - (uintptr_t) gSimpleFlipsScreamPcmStart;
    assert(simpleflips_scream_pcm_start());
    assert(!simpleflips_scream_pcm_start());
    u32 offset = 0, chunks = 0;
    while (sSimpleFlipsScreamRequested) {
        assert(!simpleflips_scream_pcm_start());
        assert(pause_pcm_start());
        assert(yoshi_cake_pcm_start());
        // The game calls this on pause exits and warps; it cannot stop the scream.
        pause_pcm_stop();
        assert(!yoshi_cake_pcm_is_playing());
        u32 size = sizes[chunks % 3];
        assert(pause_pcm_mix(commands, size, 0, 0x4C0, 0x600) == commands + 4);
        assert(sDmaAddress == (uintptr_t) gSimpleFlipsScreamPcmStart + offset);
        assert(sDmaAddress + ALIGN32(size) <= (uintptr_t) gSimpleFlipsScreamPcmEnd + 1296);
        assert(memcmp(sDmaBuffer, gSimpleFlipsScreamPcmStart + offset, ALIGN32(size)) == 0);
        assert(commands[2].words.w0 >> 24 == A_MIXER && commands[2].words.w1 == 0x4C0);
        assert(commands[3].words.w0 >> 24 == A_MIXER && commands[3].words.w1 == 0x600);
        offset += size;
        chunks++;
    }
    assert(offset >= length && offset - length < 288 && chunks > 150);
    assert(pause_pcm_mix(commands, 272, 0, 0x4C0, 0x600) == commands);
    // Independent quotes can mix concurrently, without restarting either stream.
    assert(simpleflips_scream_pcm_start());
    assert(pause_pcm_start());
    assert(pause_pcm_mix(commands, 272, 0, 0x4C0, 0x600) == commands + 8);
    assert(sStreamByteOffset == 272 && sSimpleFlipsScreamPosition == 272);
    assert(yoshi_cake_pcm_start());
    assert(pause_pcm_mix(commands, 272, 0, 0x4C0, 0x600) == commands + 8);
    assert(sStreamByteOffset == 272 && sSimpleFlipsScreamPosition == 544);
    return 0;
}
'''
                compile_and_run(directory, fixture, audio=True)


if __name__ == "__main__":
    unittest.main()
