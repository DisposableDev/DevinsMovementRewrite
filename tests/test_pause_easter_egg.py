#!/usr/bin/env python3
"""Native pause timing, EEPROM persistence and PCM command checks; no emulator."""

import os
import json
from pathlib import Path
import shlex
import subprocess
import tempfile
import unittest

from regression_support import Source

ROOT = Path(__file__).resolve().parents[1]


def prepare_pause_easter_egg_fixture(directory):
    header = Source(ROOT / "src/game/save_file.h")
    definitions = header.macros("SAVE_CAP_AREA_.*|SAVE_CAP_LEVEL_.*|NUM_SAVE_FILES")
    definitions += Source(ROOT / "src/game/game_options.h").region(r"^#define GAME_OPTIONS_FILES", r"^#endif")
    definitions += Source(ROOT / "src/game/camera.h").macros("REONUCAM_SPEED_COUNT|REONUCAM_DEFAULT_SPEED")
    definitions += header.declaration("enum", "SaveProgressFlags")
    definitions += header.declaration("enum", "SaveFileIndex")
    level_header = Source(ROOT / "src/game/level_update.h")
    for name in ("PlayModes", "WarpTypes", "WarpOperation"):
        definitions += level_header.declaration("enum", name)
    mario_header = Source(ROOT / "include/sm64.h")
    definitions += mario_header.declaration("enum", "MarioActionFlags")
    definitions += mario_header.macros("ACT_END_PEACH_CUTSCENE")
    for name in ("SaveBlockSignature", "SaveFile", "MainMenuSaveData", "SaveBuffer"):
        definitions += header.declaration("struct", name)
    save = Source(ROOT / "src/game/save_file.c")
    definitions += save.macros("SAVE_FILE_MAGIC")
    definitions += "\nstatic u8 sFailedSaveFiles;\n"
    actual = Source(ROOT / "src/game/game_options.c").region(r"^#define OPTIONS_MAGIC", r"\Z")
    actual += Source(ROOT / "src/game/game_options_audio.c").region(r"^static f32 sRangeGain", r"\Z")
    actual += "\n".join(save.function(name) for name in (
        "calc_checksum", "verify_save_block_signature", "add_save_block_signature",
        "mark_save_file_modified", "save_file_write_progress", "save_file_do_save",
        "save_file_set_flags", "save_file_has_pause_easter_egg",
        "save_file_mark_pause_easter_egg", "save_file_erase", "save_file_copy",
        "save_file_has_simpleflips_encounter", "save_file_mark_simpleflips_encounter",
        "save_file_mark_simpleflips_game_over", "save_file_consume_simpleflips_game_over",
        "save_file_set_cap_pos", "save_file_get_cap_pos"))
    encounter = Source(ROOT / "src/game/jrb_simpleflips.c")
    actual += "\n" + encounter.function("lvl_simpleflips_boot")
    actual += "\n" + encounter.function("jrb_simpleflips_is_game_over")
    timer = Source(ROOT / "src/game/pause_easter_egg.c")
    actual += "\n" + timer.region(r"^#define PAUSE_EASTER_EGG_ODDS", r"\Z")
    pcm = Source(ROOT / "src/audio/pause_pcm.c")
    # Host ABI command words use uintptr_t; step by Acmd rather than target u64.
    actual += "\n" + "\n".join(line for line in pcm.text.splitlines()
                                 if not line.startswith("#include")).replace("u64 *", "Acmd *")
    actual += "\n" + Source(ROOT / "src/game/sound_init.c").function("pause_music_is_disabled")
    level = Source(ROOT / "src/game/level_update.c")
    actual += "\n" + "\n".join(level.function(name) for name in (
        "level_is_ending", "pressed_pause", "play_mode_paused"))
    # Normal update has mutually exclusive preprocessor branches with shared braces.
    actual += "\n" + level.region(r"^s32 play_mode_normal\(void\)",
                                  r"^static void secret_lobby_cake_fade_update\(s16 \*timer\)")
    (directory / "pause_egg_definitions.inc").write_text(definitions)
    (directory / "pause_egg_actual.inc").write_text(actual)


class PauseEasterEgg(unittest.TestCase):
    def test_stream_volume_and_muted_progress(self):
        with tempfile.TemporaryDirectory(prefix="pcm-options-") as tmp:
            directory = Path(tmp)
            prepare_pause_easter_egg_fixture(directory)
            fixture = (ROOT / "tests/pause_easter_egg_test.c").read_text()
            fixture = fixture.replace("int main(void)", "int original_main(void)")
            fixture += r'''
int main(void) {
    Acmd commands[16];
    const s32 effects[] = {100,50,50,0};
    const s32 voices[] = {100,100,50,100};
    const u32 gains[] = {32767,16383,8191,0};
    game_options_init(NULL,NULL);
    game_options_select_file(0);
    for (s32 clip = 0; clip < 3; clip++) {
        for (s32 setting = 0; setting < 4; setting++) {
            pause_pcm_stop();
            sSimpleFlipsScreamRequested = FALSE;
            assert(pause_pcm_mix(commands,256,0,0x4C0,0x600) == commands);
            game_options_set(GO_SFX_VOLUME,effects[setting]);
            game_options_set(GO_VOICE_VOLUME,voices[setting]);
            if (clip == 0) { assert(pause_pcm_start()); }
            if (clip == 1) { assert(yoshi_cake_pcm_start()); }
            if (clip == 2) { assert(simpleflips_scream_pcm_start()); }
            assert(pause_pcm_mix(commands,256,0,0x4C0,0x600) == commands+4);
            assert((commands[2].words.w0 & 0xFFFF) == gains[setting]);
            assert((commands[3].words.w0 & 0xFFFF) == gains[setting]);
            assert((clip == 2 ? sSimpleFlipsScreamPosition : sStreamByteOffset) == 256);
        }
    }
    return 0;
}
'''
            path = directory / "fixture.c"
            path.write_text(fixture)
            command = shlex.split(os.environ.get("CC", "cc")) + [
                "-std=gnu99", "-D_LANGUAGE_C", "-DVERSION_US", "-Wall", "-Wextra", "-Werror",
                "-Wno-pointer-to-int-cast", "-I", str(ROOT / "include"),
                "-I", str(ROOT / "include/n64"), "-I", str(directory),
                str(path), "-o", str(directory / "test")]
            subprocess.run(command, check=True)
            subprocess.run([str(directory / "test")], check=True)

    def test_yoshi_cake_full_assets(self):
        for asset in ("yoshi_coin_cake.s16be", "yoshi_coin_cake_30k.s16be"):
            with self.subTest(asset=asset), tempfile.TemporaryDirectory(prefix="cake-quote-") as tmp:
                directory = Path(tmp)
                prepare_pause_easter_egg_fixture(directory)
                fixture = (ROOT / "tests/pause_easter_egg_test.c").read_text()
                start = fixture.index('/* Linker symbols delimit')
                end = fixture.index('static u8 sDmaBuffer', start)
                assembly = ('.balign 16\n.global gDunkeyPausePcmStart\ngDunkeyPausePcmStart:\n'
                            '.space 600,0\n.global gDunkeyPausePcmEnd\ngDunkeyPausePcmEnd:\n.space 1296,0\n'
                            '.balign 16\n.global gYoshiCakePcmStart\ngYoshiCakePcmStart:\n'
                            f'.incbin "{ROOT / "sound/pcm" / asset}"\n'
                            '.global gYoshiCakePcmEnd\ngYoshiCakePcmEnd:\n.space 1296,0\n')
                fixture = fixture[:start] + '__asm__(' + json.dumps(assembly) + ');\n' + fixture[end:]
                fixture = fixture.replace('return sDmaBuffer;',
                                          'memcpy(sDmaBuffer, (void *) address, size); return sDmaBuffer;')
                fixture = fixture.replace('int main(void)', 'int short_fixture_main(void)')
                fixture += r'''
int main(void) {
    Acmd commands[16];
    const u32 sizes[] = {256, 272, 288};
    const u32 length = (uintptr_t) gYoshiCakePcmEnd - (uintptr_t) gYoshiCakePcmStart;
    // Switching from an already running pause quote restarts at the cake clip's beginning.
    pause_pcm_start();
    pause_pcm_mix(commands, 272, 0, 0x4C0, 0x600);
    assert(yoshi_cake_pcm_start() && yoshi_cake_pcm_is_playing());
    u32 offset = 0, chunks = 0;
    while (yoshi_cake_pcm_is_playing()) {
        u32 size = sizes[chunks % 3];
        assert(pause_pcm_mix(commands, size, 0, 0x4C0, 0x600) == commands + 4);
        assert(sDmaAddress == (uintptr_t) gYoshiCakePcmStart + offset);
        assert(sDmaAddress + ALIGN32(size) <= (uintptr_t) gYoshiCakePcmEnd + 1296);
        assert(memcmp(sDmaBuffer, gYoshiCakePcmStart + offset, ALIGN32(size)) == 0);
        assert(commands[1].words.w0 >> 24 == A_LOADBUFF);
        assert(commands[2].words.w0 >> 24 == A_MIXER && commands[2].words.w1 == 0x4C0);
        assert(commands[3].words.w0 >> 24 == A_MIXER && commands[3].words.w1 == 0x600);
        offset += size;
        chunks++;
    }
    assert(offset >= length && offset - length < 288 && chunks > 600);
    assert(pause_pcm_mix(commands, 272, 0, 0x4C0, 0x600) == commands);
    yoshi_cake_pcm_start();
    pause_pcm_stop();
    assert(!yoshi_cake_pcm_is_playing());
    return 0;
}
'''
                path = directory / "cake_quote.c"
                path.write_text(fixture)
                command = shlex.split(os.environ.get("CC", "cc")) + [
                    "-std=gnu99", "-D_LANGUAGE_C", "-DVERSION_US", "-Wall", "-Wextra", "-Werror",
                    "-Wno-pointer-to-int-cast", "-I", str(ROOT / "include"),
                    "-I", str(ROOT / "include/n64"), "-I", str(directory),
                    str(path), "-o", str(directory / "test")]
                subprocess.run(command, check=True)
                subprocess.run([str(directory / "test")], check=True)

    def test_full_assets_continue_through_paused_updates(self):
        for asset in ("dunkey_pause_30k.s16be", "dunkey_pause.s16be"):
            with self.subTest(asset=asset), tempfile.TemporaryDirectory(prefix="pause-full-quote-") as tmp:
                directory = Path(tmp)
                prepare_pause_easter_egg_fixture(directory)
                fixture = (ROOT / "tests/pause_easter_egg_test.c").read_text()
                start = fixture.index('/* Linker symbols delimit')
                end = fixture.index('static u8 sDmaBuffer', start)
                assembly = ('.balign 16\n.global gDunkeyPausePcmStart\ngDunkeyPausePcmStart:\n'
                            f'.incbin "{ROOT / "sound/pcm" / asset}"\n'
                            '.global gDunkeyPausePcmEnd\ngDunkeyPausePcmEnd:\n.space 1296,0\n'
                            '.global gYoshiCakePcmStart\ngYoshiCakePcmStart:\n.space 800,0\n'
                            '.global gYoshiCakePcmEnd\ngYoshiCakePcmEnd:\n.space 1296,0\n')
                fixture = (fixture[:start] + 'extern u8 gDunkeyPausePcmStart[], gDunkeyPausePcmEnd[];\n'
                           + '__asm__(' + json.dumps(assembly) + ');\n' + fixture[end:])
                fixture = fixture.replace('return sDmaBuffer;',
                                          'assert(address + size <= (uintptr_t) gDunkeyPausePcmEnd + 1296);'
                                          '\n    memcpy(sDmaBuffer, (void *) address, size);\n    return sDmaBuffer;')
                fixture = fixture.replace('int main(void)', 'int short_fixture_main(void)')
                fixture += r'''
int main(void) {
    const u32 sizes[] = { 256, 272, 288 };
    const u32 length = (uintptr_t) gDunkeyPausePcmEnd - (uintptr_t) gDunkeyPausePcmStart;
    Acmd commands[16];
    for (u32 scenario = 0; scenario < 5; scenario++) {
        reset();
        set_play_mode(PLAY_MODE_PAUSED);
        play_mode_paused();
        sClock += sPauseEasterEggIdleDelayCycles;
        play_mode_paused();
        assert(sRequestedClip && sWrites == 1);
        u32 offset = 0, chunks = 0;
        while (sRequestedClip) {
            if (scenario == 1) {
                sController.buttonDown = B_BUTTON;
                sController.stickMag = 32.0f;
                gMuteMusicPlayers = chunks & 1;
            }
            play_mode_paused();
            assert(sRequestedClip); /* Paused updates cannot cancel a started quote. */
            u32 size = sizes[chunks % 3];
            assert(pause_pcm_mix(commands, size, 0, 0x4C0, 0x600) == commands + 4);
            assert(sDmaAddress == (uintptr_t) gDunkeyPausePcmStart + offset);
            assert(sDmaBytes == ALIGN32(size));
            assert(commands[0].words.w0 >> 24 == A_SETBUFF);
            assert((commands[0].words.w1 & 0xFFFF) == ALIGN32(size));
            assert(commands[1].words.w0 >> 24 == A_LOADBUFF);
            assert(commands[2].words.w0 >> 24 == A_MIXER && commands[2].words.w1 == 0x4C0);
            assert(commands[3].words.w0 >> 24 == A_MIXER && commands[3].words.w1 == 0x600);
            assert((commands[2].words.w0 & 0xFFFF) == 0x7FFF);
            assert((commands[3].words.w0 & 0xFFFF) == 0x7FFF);
            u32 remaining = length - offset;
            u32 valid = remaining < size ? remaining : size;
            assert(memcmp(sDmaBuffer, gDunkeyPausePcmStart + offset, valid) == 0);
            offset += size;
            chunks++;
            if (scenario >= 2 && chunks == 10) {
                assert(sRequestedClip && offset < length);
                if (scenario == 2) {
                    set_play_mode(PLAY_MODE_NORMAL);
                } else if (scenario == 3) {
                    gMenuOptSelectIndex = MENU_OPT_DEFAULT;
                    play_mode_paused();
                } else {
                    set_play_mode(PLAY_MODE_CHANGE_AREA);
                }
                assert(!sRequestedClip);
            }
        }
        if (scenario < 2) {
            assert(offset >= length && offset - length < 288);
            assert(chunks > 700);
        }
        for (u32 i = 0; i < 100; i++) {
            play_mode_paused();
            assert(pause_pcm_mix(commands, 272, 0, 0x4C0, 0x600) == commands);
        }
        assert(sWrites == 1 && sRandomIndex == 2);
    }
    puts("PASS complete quote: both channels, full asset, paused input, resume, exit and warp");
    return 0;
}
'''
                source = directory / "full_quote.c"
                source.write_text(fixture)
                command = shlex.split(os.environ.get("CC", "cc")) + [
                    "-std=gnu99", "-D_LANGUAGE_C", "-DVERSION_US", "-Wall", "-Wextra", "-Werror",
                    "-Wno-pointer-to-int-cast", "-I", str(ROOT / "include"),
                    "-I", str(ROOT / "include/n64"), "-I", str(directory),
                    str(source), "-o", str(directory / "test")]
                subprocess.run(command, check=True)
                subprocess.run([str(directory / "test")], check=True)

    def test_native_timing_save_and_pcm(self):
        with tempfile.TemporaryDirectory(prefix="pause-easter-egg-") as directory:
            directory = Path(directory)
            prepare_pause_easter_egg_fixture(directory)
            command = shlex.split(os.environ.get("CC", "cc")) + [
                "-std=gnu99", "-D_LANGUAGE_C", "-DVERSION_US", "-Wall", "-Wextra", "-Werror",
                "-Wno-pointer-to-int-cast", "-I", str(ROOT / "include"),
                "-I", str(ROOT / "include/n64"), "-I", str(directory),
                str(ROOT / "tests/pause_easter_egg_test.c"), "-o", str(directory / "test")]
            subprocess.run(command, check=True)
            subprocess.run([str(directory / "test")], check=True)

    def test_asset_and_integration(self):
        import hashlib
        pcm = (ROOT / "sound/pcm/dunkey_pause.s16be").read_bytes()
        self.assertEqual(len(pcm), 214364)
        self.assertEqual(hashlib.sha256(pcm).hexdigest(),
                         "aa83f5bdf66225f461f4cdbee53f388c195057746dbbbc284bbe8c0be08118a1")
        level = Source(ROOT / "src/game/level_update.c")
        mode = level.function("set_play_mode")
        self.assertIn("pause_easter_egg_begin();", mode)
        self.assertIn("pause_easter_egg_end();", mode)
        self.assertIn("pause_easter_egg_update();", level.function("play_mode_paused"))
        synth = Source(ROOT / "src/audio/synthesis.c").region(
            r"^u64 \*synthesis_process_notes[^\n]+\{", r"^u64 \*load_wave_samples[^\n]+\{")
        self.assertLess(synth.index("pause_pcm_mix("), synth.index("aInterleave("))
        self.assertLess(synth.index("aInterleave("), synth.index("aSaveBuffer("))
        assembly = (ROOT / "sound/sound_data.s").read_text()
        self.assertIn('.incbin "sound/pcm/dunkey_pause.s16be"', assembly)
        self.assertIn(".space (144 * 9), 0", assembly)


if __name__ == "__main__":
    unittest.main()
