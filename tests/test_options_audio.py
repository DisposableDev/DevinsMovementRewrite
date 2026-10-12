#!/usr/bin/env python3
"""Production output DSP and completed-buffer ownership, including AI retries."""
from pathlib import Path
import os
import subprocess
import tempfile
import unittest

from regression_support import Source

ROOT = Path(__file__).resolve().parents[1]
FIXTURE = r'''
#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define TRUE 1
#define FALSE 0
#include "game_options.c"
#include "game_options_audio.c"
#define AUDIO_IMMEDIATE_TASKS 1
#define ARRAY_COUNT(a) (sizeof(a) / sizeof((a)[0]))
s16 pcm[3][64];
s16 *gAiBuffers[3] = {pcm[0], pcm[1], pcm[2]};
s16 gAiBufferLengths[3] = {16,16,16};
s32 gAiFrequency = 32000;
static s32 invalidations, publications, submissions, busy;
static void osInvalDCache(void *data, s32 bytes) {
    assert(data == pcm[0] || data == pcm[1] || data == pcm[2]);
    assert(bytes == 64 && invalidations == publications);
    invalidations++;
}
static void osWritebackDCache(void *data, s32 bytes) {
    assert(data == pcm[0] || data == pcm[1] || data == pcm[2]);
    assert(bytes == 64 && invalidations == publications + 1);
    publications++;
}
#include "options_output.c"
struct SPTask { s32 msg; } gAudioTasks[2];
static u8 sAudioTaskInFlight[2];
static s32 osAiSetNextBuffer(void *data, s32 bytes) {
    assert(bytes == 64 && invalidations == publications);
    assert(data == pcm[0] || data == pcm[1] || data == pcm[2]);
    submissions++;
    return busy ? -1 : 0;
}
'''
CHECKS = r'''
static void near(f32 actual, f32 expected) { assert(fabsf(actual - expected) < 0.00001f); }
static void sliders(void) {
    game_options_init(NULL, NULL); game_options_select_file(0);
    assert(!game_options_audio_needs_output());
    s16 original[] = {-32768,32767,-1111,3333,0,0,2222,-4444};
    s16 samples[8]; memcpy(samples,original,sizeof(samples));
    assert(!game_options_audio_process(samples,4,32000));
    assert(memcmp(samples,original,sizeof(samples)) == 0);
    for (s32 volume = 0; volume <= 100; volume += 10) {
        game_options_set(GO_MASTER_VOLUME,volume);
        game_options_set(GO_MUSIC_VOLUME,volume);
        game_options_set(GO_SFX_VOLUME,volume);
        assert(!game_options_set(GO_VOICE_VOLUME,50));
        sValues[0][GO_VOICE_VOLUME] = 0; // An old hidden voice preference cannot mute Mario.
        near(game_options_music_gain(),volume * 0.01f);
        near(game_options_effects_gain(FALSE),volume * 0.01f);
        near(game_options_effects_gain(TRUE),volume * 0.01f);
        memcpy(samples,original,sizeof(samples));
        game_options_audio_process(samples,4,32000);
        for (s32 i = 0; i < 8; i++) { assert(samples[i] == original[i] * volume / 100); }
    }
    for (s32 balance = 0; balance <= 20; balance++) {
        game_options_set(GO_AUDIO_BALANCE,balance);
        memcpy(samples,original,sizeof(samples));
        game_options_audio_process(samples,4,32000);
        for (s32 i = 0; i < 4; i++) {
            assert(samples[i*2] == original[i*2] * (balance<=10 ? 100 : (20-balance)*10) / 100);
            assert(samples[i*2+1] == original[i*2+1] * (balance>=10 ? 100 : balance*10) / 100);
        }
    }
    game_options_set(GO_MASTER_VOLUME,50);
    game_options_set(GO_AUDIO_BALANCE,15);
    memcpy(samples,original,sizeof(samples));
    game_options_audio_process(samples,4,32000);
    assert(samples[0] == -8192 && samples[1] == 16383);
    assert(!game_options_audio_process(NULL,0,0));
}
static void dynamic_range(void) {
    static s16 samples[8192];
    game_options_init(NULL,NULL); game_options_select_file(0);
    game_options_set(GO_AUDIO_RANGE,1); game_options_audio_reset();
    for (s32 frequency = 30000; frequency <= 32000; frequency += 2000) {
        for (s32 block = 0; block < 3; block++) {
            for (s32 i = 0; i < 4096; i++) { samples[i*2]=1000; samples[i*2+1]=-500; }
            assert(game_options_audio_process(samples,4096,frequency));
            assert(samples[8190] > 1800 && samples[8190] <= 2000);
            assert(abs(samples[8190] + samples[8191]*2) <= 1);
        }
        for (s32 i=0;i<4096;i++) { samples[i*2]=30000; samples[i*2+1]=-15000; }
        game_options_audio_process(samples,4096,frequency);
        assert(samples[8190] >= 21000 && samples[8190] < 22000);
        assert(abs(samples[8190] + samples[8191]*2) <= 1);
        for (s32 i=0;i<4096;i++) { samples[i*2]=-32768; samples[i*2+1]=32767; }
        game_options_audio_process(samples,4096,frequency);
        for (s32 i=0;i<4096;i++) { assert(samples[i*2]<0 && samples[i*2+1]>0); }
    }
    game_options_set(GO_MASTER_VOLUME,0);
    assert(game_options_audio_process(samples,4096,32000));
    for (s32 i=0;i<8192;i++) { assert(samples[i]==0); }
    game_options_set(GO_AUDIO_RANGE,0); game_options_set(GO_MASTER_VOLUME,100);
    samples[0]=-32768; samples[1]=32767;
    assert(!game_options_audio_process(samples,1,32000));
    assert(samples[0]==-32768 && samples[1]==32767);
}
static void ownership(void) {
    game_options_init(NULL,NULL); game_options_select_file(0);
    game_options_set(GO_MASTER_VOLUME,50);
    audio_options_output_reset();
    sAudioBufferStates[1]=AUDIO_BUFFER_RENDERING;
    gAudioTasks[0].msg=2; sAudioTaskInFlight[0]=TRUE;
    for(s32 i=0;i<32;i++) { pcm[1][i]=i%2 ? -2000 : 1000; }
    audio_options_output_begin(1);
    audio_submit_frame_buffer();
    assert(!invalidations && !publications && !submissions);
    assert(audio_complete_frame_task(&gAudioTasks[0])==TRUE);
    busy=1; audio_submit_frame_buffer();
    assert(submissions==1 && invalidations==1 && publications==1);
    assert(pcm[1][0]==500 && pcm[1][1]==-1000);
    assert(sAudioBufferStates[1]==AUDIO_BUFFER_READY);
    busy=0; audio_submit_frame_buffer();
    assert(submissions==2 && invalidations==1 && publications==1);
    assert(pcm[1][0]==500 && pcm[1][1]==-1000);
    assert(sAudioBufferStates[1]==AUDIO_BUFFER_QUEUED);
    audio_submit_frame_buffer();
    assert(invalidations==1 && submissions==2); // The next slot is not ready.
    game_options_set(GO_MASTER_VOLUME,0);
    sAudioBufferStates[2]=AUDIO_BUFFER_RENDERING;
    audio_options_output_begin(2);
    for(s32 i=0;i<32;i++) { pcm[2][i]=i%2 ? -32768 : 32767; }
    gAudioTasks[1].msg=3; sAudioTaskInFlight[1]=TRUE;
    audio_complete_frame_task(&gAudioTasks[1]);
    audio_submit_frame_buffer();
    assert(invalidations==2 && publications==2 && submissions==3);
    for(s32 i=0;i<32;i++) { assert(pcm[2][i]==0); }
    // Full AI queue leaves an otherwise-ready buffer untouched.
    sAudioBufferStates[0]=AUDIO_BUFFER_READY;
    audio_options_output_begin(0);
    audio_submit_frame_buffer();
    assert(invalidations==2 && submissions==3);
    sAudioAiQueueCount=0;
    game_options_set(GO_MASTER_VOLUME,100);
    pcm[0][0]=1234;
    audio_submit_frame_buffer();
    assert(invalidations==2 && publications==2 && submissions==4 && pcm[0][0]==1234);
}
int main(void) { sliders(); dynamic_range(); ownership(); puts("options audio checks passed"); }
'''


class OptionsAudioTests(unittest.TestCase):
    def test_file_menu_sound_output(self):
        sound = Source(ROOT / "src/game/sound_init.c")
        saves = Source(ROOT / "src/game/save_file.c")
        fixture = r'''
#include <assert.h>
#define TRUE 1
#define FALSE 0
#define SOUND_MODE_COUNT 2
#define LEVEL_UNKNOWN_1 1
#define LEVEL_BOB 9
#include "game_options.c"
static s8 gSoundMode;
static s16 gCurrLevelNum;
static u8 sBackgroundNoiseReasons;
static f32 sPauseMusicFadeTargetScale;
static f32 game_options_pause_gain(void) { return 1; }
static void fade_pause_music_volume(f32 value) { (void)value; }
static void update_pause_music_volume(void) {}
static void audio_signal_game_loop_tick(void) {}
static struct { struct { u16 soundMode; } menuData; } gSaveBuffer;
static s32 gMainMenuDataModified;
static void save_main_menu_data(void) {}
'''
        fixture += sound.function("set_sound_mode")
        for name in ("save_file_get_sound_mode", "save_file_set_sound_mode"):
            fixture += saves.function(name)
        fixture += sound.function("audio_game_loop_tick")
        fixture += r'''
#line 1 "sound_output_checks"
int main(void) {
    game_options_init(NULL, NULL);
    // Cold boot and attract playback have no selected save.
    gCurrLevelNum = LEVEL_BOB;
    save_file_set_sound_mode(1);
    audio_game_loop_tick();
    assert(gSoundMode == 1);
    // Gameplay follows its file even when the global menu selection differs.
    game_options_select_file(0);
    assert(game_options_set(GO_SOUND_OUTPUT, 0));
    audio_game_loop_tick();
    assert(gSoundMode == 0);
    // Game over retains the active file; file-select has its own sound setting.
    gCurrLevelNum = LEVEL_UNKNOWN_1;
    audio_game_loop_tick();
    assert(gSoundMode == 1);
    save_file_set_sound_mode(0);
    audio_game_loop_tick();
    assert(gSoundMode == 0);
    // An opposite per-file selection also survives returning to the menu.
    assert(game_options_set(GO_SOUND_OUTPUT, 1));
    audio_game_loop_tick();
    assert(gSoundMode == 0);
    gCurrLevelNum = LEVEL_BOB;
    audio_game_loop_tick();
    assert(gSoundMode == 1);
    return 0;
}
'''
        with tempfile.TemporaryDirectory() as temporary:
            folder = Path(temporary)
            (folder / "PR").mkdir()
            (folder / "PR/ultratypes.h").write_text(
                "#pragma once\n#include <stdint.h>\n"
                "typedef uint8_t u8; typedef int8_t s8; typedef uint16_t u16; typedef int16_t s16;\n"
                "typedef uint32_t u32; typedef int32_t s32; typedef float f32;\n")
            (folder / "fixture.c").write_text(fixture)
            executable = folder / "test"
            subprocess.run([
                "cc", "-std=c11", "-Wall", "-Wextra", "-Werror",
                "-I", str(folder), "-I", str(ROOT / "src/game"),
                str(folder / "fixture.c"), "-o", str(executable),
            ], check=True, timeout=30)
            subprocess.run([str(executable)], check=True, timeout=10)

    def test_output_and_submission(self):
        with tempfile.TemporaryDirectory() as temporary:
            folder = Path(temporary)
            (folder / "PR").mkdir()
            (folder / "PR/ultratypes.h").write_text(
                "#pragma once\n#include <stdint.h>\n"
                "typedef uint8_t u8; typedef int8_t s8; typedef uint16_t u16; typedef int16_t s16;\n"
                "typedef uint32_t u32; typedef int32_t s32; typedef float f32;\n")
            (folder / "ultra64.h").write_text("#include <PR/ultratypes.h>\n")
            (folder / "data.h").write_text("#define NUMAIBUFFERS 3\n")
            (folder / "load.h").write_text("")
            for name in ("options_output.c", "options_output.h"):
                (folder / name).write_text((ROOT / "src/audio" / name).read_text())
            source = Source(ROOT / "src/audio/external.c")
            actual = source.region(r"^enum AudioBufferState", r"^static void audio_reclaim_played_buffers")
            actual += source.function("audio_submit_frame_buffer")
            actual += source.function("audio_complete_frame_task")
            fixture = FIXTURE + actual + CHECKS
            # The native wrapper's cross-directory quoted include resolves here.
            (folder / "game").mkdir()
            (folder / "game/game_options_audio.h").write_text(
                (ROOT / "src/game/game_options_audio.h").read_text())
            (folder / "fixture.c").write_text(fixture)
            executable = folder / "test"
            subprocess.run([
                "cc", "-std=c11", "-Wall", "-Wextra", "-Werror",
                "-fsanitize=address,undefined", "-fno-sanitize-recover=all", "-fno-omit-frame-pointer",
                "-I", str(folder), "-I", str(ROOT / "src/game"),
                str(folder / "fixture.c"), "-lm", "-o", str(executable),
            ], check=True, timeout=30)
            subprocess.run([str(executable)], check=True,
                           env={**os.environ, "ASAN_OPTIONS": "detect_leaks=0"}, timeout=10)


if __name__ == "__main__":
    unittest.main()
