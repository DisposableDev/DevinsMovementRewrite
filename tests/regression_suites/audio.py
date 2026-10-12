"""Production extraction and deliberate defects for the audio suite."""
from .common import prepare_common_constants
from .mutation_support import MutationSource

def prepare_audio(root, source, definitions, functions):
    prepare_common_constants(source, definitions)
    definitions += [source("src/game/mario_he_thicc.h").macros(
        "HE_THICC_.*|ACT_ARG_WALL_KICK_HE_THICC|ACT_ARG_FORWARD_ROLLOUT_HE_THICC_BOUNCE")]
    definitions += [source("src/game/mario_actions_airborne.h").macros(
        "ACT_ARG_JUMP_BACKWARD_BUTT_SLIDE.*|ACT_ARG_JUMP_KICK_FOLLOWUP_QUEUED|ACT_ARG_JUMP_KICK_BACKWARD_BUTT_SLIDE.*|ACT_ARG_STEEP_JUMP_BACKWARD_BUTT_SLIDE_NO_UPHILL|ACT_ARG_DIVE_FROM_ROLLOUT_CONTACT")]
    definitions += [source("src/game/mario_actions_stationary.h").macros(
        "ACT_ARG_COUGHING_SHALLOW_WATER")]
    definitions += [source("include/config/config_movement.h").macros("BACKWARD_BUTT_SLIDE_JUMP_AIR_DRAG|BACKWARD_JUMP_KICK_UPHILL_DECEL")]
    definitions += [source("src/audio/external.h").declaration("enum", "SequencePlayers")]
    playback = source("src/audio/playback.c")
    functions += [playback.function("music_note_velocity")]
    for version in ("EU", "SH"):
        functions += ["#define VERSION_" + version,
                      playback.function("music_note_velocity").replace(
                          "music_note_velocity(", "music_note_velocity_" + version.lower() + "("),
                      "#undef VERSION_" + version]
    audio = source("src/audio/external.c")
    definitions += [audio.declaration("struct", name) for name in ("Sound", "SoundCharacteristics")]
    definitions += [source("include/sounds.h").macros("SOUNDARGS_(MASK_.*|SHIFT_.*)|NO_SOUND")]
    definitions += [source("include/sounds.h").macros(
        "SOUND_MARIO_(ATTACKED|OOOF|OOOF2|ON_FIRE|DOH|UH|YAHOO|DYING)")]
    definitions += [source("include/sounds.h").declaration("enum", "SoundBank"),
                    source("include/sounds.h").macros("SOUND_CONSTANT_FREQUENCY|SOUND_VIBRATO|SOUND_NO_VOLUME_LOSS"),
                    source("src/game/mario_character.h").declaration("enum", "MarioPalette"),
                    source("include/config/config_audio.h").macros("HE_THICC_VOICE_PITCH_SCALE|HE_THICC_MUSIC_BASS_.*|MARIO_FOOTSTEP_VOLUME_SCALE|MARIO_JUMP_LANDING_VOLUME_SCALE"),
                    source("include/sounds.h").macros("SOUND_BANK_DETAILS|SOUND_HE_THICC_CLAP|SOUND_HE_THICC_RUNNING_STEP|SOUND_ARG_LOAD|SOUND_NO_PRIORITY_LOSS|SOUND_DISCRETE|SOUND_STATUS_WAITING"),
                    audio.macros("AUDIO_MAX_DISTANCE")]
    functions += [audio.function(name) for name in (
        "play_sound_with_rumble", "play_sound", "process_all_sound_requests", "stop_sound",
        "get_he_thicc_step_weight", "get_sound_freq_scale", "get_mario_movement_volume_scale", "get_sound_volume")]
    functions += ["#define VERSION_JP", audio.function("get_sound_volume").replace(
        "get_sound_volume(", "get_sound_volume_jp("), "#undef VERSION_JP"]

def mutations_audio(root):
    audio_path = "src/audio/external.c"
    audio = MutationSource(root / audio_path)
    yield "audio", "leave_queued_sound", "audio.queued", {audio_path: audio.replace_in_function(
        "stop_sound", "request->soundBits = NO_SOUND;", "(void)request;")}
    for name, function, before, after in (
        ("fixed_running_bass_pitch", "get_sound_freq_scale",
         "stepPitch = 1.35f - 0.25f * stepWeight;", "stepPitch = 1.10f;"),
        ("full_running_bass_volume", "get_mario_movement_volume_scale",
         "return 0.25f + 0.50f * stepWeight;", "return 1.0f;"),
    ):
        yield "audio", name, "audio.he_thicc_running_bass", {
            audio_path: audio.replace_in_function(function, before, after)}
    config_path = "include/config/config_audio.h"
    yield "audio", "old_music_bass_gain", "audio.he_thicc_music_bass", {
        config_path: MutationSource(root / config_path).replace_unique(
            "HE_THICC_MUSIC_BASS_GAIN 1.10f", "HE_THICC_MUSIC_BASS_GAIN 1.04f")}

