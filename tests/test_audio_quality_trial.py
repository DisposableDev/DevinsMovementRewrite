"""Host checks for trial ABI parameters, released tails and matching PCM assets."""

import hashlib
import os
from pathlib import Path
import shlex
import subprocess
import tempfile
import unittest

from regression_support import Source

ROOT = Path(__file__).resolve().parents[1]

FIXTURE = r'''
#include <ultra64.h>
#include <assert.h>
#include <stdint.h>
#include "config/config_audio.h"
#define TRUE 1
#define FALSE 0
#define NOTE_PRIORITY_STOPPING 1
#define VOL_RAMPING_EXPONENT 9
#define VIRTUAL_TO_PHYSICAL2(p) ((uintptr_t) (p) & 0x1FFFFFFF)
struct NoteSynthesisBuffers { s16 mixEnvelopeState[40]; };
struct Note {
    u8 priority, needsInit, finished, enabled, initFullVelocity, envMixerNeedsInit, reverbVol;
    u16 targetVolLeft, targetVolRight;
    s16 curVolLeft, curVolRight;
    struct NoteSynthesisBuffers *synthesisBuffers;
};
static s32 gVolume = 0x7fff;
static struct { s32 useReverb; } gSynthesisReverb;
#if AUDIO_LINEAR_ENVELOPES
static f32 sLinearRampScale;
#else
static f32 table[512] = { [0 ... 511] = 256.0f };
static f32 *currentRampingTableLeft = table;
static f32 *currentRampingTableRight = table;
#endif
#include "trial_actual.inc"
int main(void) {
    struct NoteSynthesisBuffers buffers = { 0 };
    const s32 counts[] = { 16, 128, 256, 272, 288, 320 };
    Acmd commands[16];
    for (u32 i = 0; i < sizeof(counts) / sizeof(counts[0]); i++) {
        s32 count = counts[i];
        struct Note note = { .curVolLeft = 1, .curVolRight = 32736,
            .targetVolLeft = 32736, .targetVolRight = 1, .reverbVol = 64,
            .synthesisBuffers = &buffers };
#if AUDIO_LINEAR_ENVELOPES
        sLinearRampScale = 1048576.0f / (f32) count;
#endif
        gSynthesisReverb.useReverb = TRUE;
        Acmd *end = process_envelope(commands, &note, count, 0);
        assert(end - commands == 8);
        assert(note.curVolLeft == 32736 && note.curVolRight == 1);
        assert(commands[0].words.w0 >> 24 == A_SETBUFF);
        assert((commands[0].words.w1 & 65535) == (u32) count);
        assert(commands[4].words.w0 >> 24 == A_SETVOL);
        assert((commands[4].words.w0 & 65535) == 32736);
        assert((commands[5].words.w0 & 65535) == 1);
        assert(commands[7].words.w0 >> 24 == A_ENVMIXER);
        assert(((commands[7].words.w0 >> 16) & (A_INIT | A_AUX)) == (A_INIT | A_AUX));
#if AUDIO_LINEAR_ENVELOPES
        s32 expected = (s32) (32735.0f * (1048576.0f / count));
        assert((s32) commands[4].words.w1 == expected);
        assert((s32) commands[5].words.w1 == -expected);
        // Steady volume still emits fresh endpoints; no stale RSP state is read.
        note.curVolLeft = note.targetVolLeft = 12000;
        note.curVolRight = note.targetVolRight = 8000;
        gSynthesisReverb.useReverb = FALSE;
        end = process_envelope(commands, &note, count, 0);
        assert(end - commands == 8);
        assert(commands[4].words.w1 == 0 && commands[5].words.w1 == 0);
        assert(((commands[7].words.w0 >> 16) & A_AUX) == 0);
        note.initFullVelocity = TRUE;
        note.targetVolLeft = 4000;
        process_envelope(commands, &note, count, 0);
        assert(!note.initFullVelocity && commands[4].words.w1 == 0);
#else
        // HLE expects a positive Q16.16 multiplier, including fading channels.
        assert(commands[4].words.w1 == 65536 && commands[5].words.w1 == 65536);
        note.curVolLeft = note.targetVolLeft;
        note.curVolRight = note.targetVolRight;
        end = process_envelope(commands, &note, count, 0);
        assert(end - commands == 3);
#endif
    }
#if AUDIO_QUALITY_TRIAL || AUDIO_TRIAL == 8 || AUDIO_TRIAL == 10
    struct Note tail = { .priority = NOTE_PRIORITY_STOPPING,
        .targetVolLeft = AUDIO_QUIET_TAIL_THRESHOLD, .targetVolRight = AUDIO_QUIET_TAIL_THRESHOLD,
        .curVolLeft = AUDIO_QUIET_TAIL_THRESHOLD, .curVolRight = AUDIO_QUIET_TAIL_THRESHOLD };
    assert(note_has_inaudible_tail(&tail));
    tail.curVolLeft = AUDIO_QUIET_TAIL_THRESHOLD + 1;
    assert(!note_has_inaudible_tail(&tail));
    tail.curVolLeft = AUDIO_QUIET_TAIL_THRESHOLD;
    tail.targetVolRight = AUDIO_QUIET_TAIL_THRESHOLD + 1;
    assert(!note_has_inaudible_tail(&tail));
    tail.targetVolRight = AUDIO_QUIET_TAIL_THRESHOLD;
    tail.priority = 3;
    assert(!note_has_inaudible_tail(&tail));
    tail.priority = NOTE_PRIORITY_STOPPING;
    tail.needsInit = TRUE;
    assert(!note_has_inaudible_tail(&tail));
#if AUDIO_TRIAL == 8 || AUDIO_TRIAL == 10
    tail.needsInit = FALSE;
    tail.synthesisBuffers = &buffers;
    assert(!note_finish_quiet_tail(&tail));
    assert(tail.targetVolLeft == 0 && tail.targetVolRight == 0);
    assert(tail.envMixerNeedsInit);
    process_envelope(commands, &tail, 128, 0);
    assert(commands[4].words.w1 == 0 && commands[5].words.w1 == 0);
    assert(tail.curVolLeft == 0 && tail.curVolRight == 0);
    assert(note_finish_quiet_tail(&tail));
#endif
#else
#ifdef AUDIO_QUIET_TAIL_THRESHOLD
#error Normal synthesis must retain quiet tails
#endif
#endif
#if AUDIO_QUALITY_TRIAL || AUDIO_TRIAL == 9 || AUDIO_TRIAL == 10
    assert(AUDIO_OUTPUT_FREQUENCY == 30000);
#else
    assert(AUDIO_OUTPUT_FREQUENCY == 32000);
#endif
    assert(AUDIO_LOW_LATENCY == 1);
    return 0;
}
'''


class AudioQualityTrial(unittest.TestCase):
    def test_native_envelope_parameters_and_tail_guards(self):
        synth = Source(ROOT / "src/audio/synthesis.c")
        actual = synth.macros("DMEM_ADDR_.*")
        actual += synth.declaration("struct", "VolumeChange")
        actual += "\n#if AUDIO_QUALITY_TRIAL || AUDIO_TRIAL == 8 || AUDIO_TRIAL == 10\n"
        actual += synth.function("note_has_inaudible_tail")
        actual += "\n#endif\n#if AUDIO_TRIAL == 8 || AUDIO_TRIAL == 10\n"
        actual += synth.function("note_finish_quiet_tail")
        actual += "\n#endif\n"
        # Both headset signatures open a brace; the native preprocessor selects one.
        actual += synth.region(
            r"^#ifdef ENABLE_STEREO_HEADSET_EFFECTS\nu64 \*process_envelope[^\n]*\{\n",
            r"^#ifdef ENABLE_STEREO_HEADSET_EFFECTS\nu64 \*note_apply_headset_pan_effects",
        ).replace("u64 *", "Acmd *")
        with tempfile.TemporaryDirectory(prefix="audio-quality-trial-") as name:
            directory = Path(name)
            (directory / "trial_actual.inc").write_text(actual)
            (directory / "fixture.c").write_text(FIXTURE)
            settings = [(None, None, None), (0, 0, 0), (1, 0, 0), (1, 1, 0)]
            settings.extend((0, 0, trial) for trial in range(1, 11))
            for quality, linear, trial in settings:
                defines = []
                if quality is not None:
                    defines = [f"-DAUDIO_QUALITY_TRIAL={quality}", f"-DAUDIO_LINEAR_ENVELOPES={linear}",
                               f"-DAUDIO_TRIAL={trial}"]
                command = shlex.split(os.environ.get("CC", "cc")) + [
                    "-std=gnu99", "-D_LANGUAGE_C", "-DVERSION_US",
                    *defines, "-Wall", "-Wextra", "-Werror",
                    "-Wno-pointer-to-int-cast", "-I", str(ROOT / "include"),
                    "-I", str(ROOT / "include/n64"), "-I", str(directory),
                    str(directory / "fixture.c"), "-o", str(directory / "test")]
                subprocess.run(command, check=True)
                subprocess.run([str(directory / "test")], check=True)

    def test_resampled_quote_duration_and_hash(self):
        original = (ROOT / "sound/pcm/dunkey_pause.s16be").read_bytes()
        trial = (ROOT / "sound/pcm/dunkey_pause_30k.s16be").read_bytes()
        self.assertEqual(hashlib.sha256(trial).hexdigest(),
                         "b4d83b5b6b520877b8c507251c1c837fb05d9785211082cf777963a86762517a")
        self.assertEqual(len(trial), 200966)
        self.assertLess(abs(len(original) / 2 / 32000 - len(trial) / 2 / 30000), 1 / 30000)
        assembly = (ROOT / "sound/sound_data.s").read_text()
        self.assertIn('#if AUDIO_OUTPUT_FREQUENCY == 30000\n.incbin "sound/pcm/dunkey_pause_30k.s16be"', assembly)


if __name__ == "__main__":
    unittest.main()
