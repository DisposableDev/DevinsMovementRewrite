#pragma once

/* Keep these defaults aligned with Makefile and the assembled microcode. */
#ifndef AUDIO_QUALITY_TRIAL
#define AUDIO_QUALITY_TRIAL 0
#endif

/* US/JP default to the accepted combined trial; 0 restores normal audio. */
#ifndef AUDIO_TRIAL
#if defined(VERSION_US) || defined(VERSION_JP)
#define AUDIO_TRIAL 10
#else
#define AUDIO_TRIAL 0
#endif
#endif

/* HLE audio plugins require the original exponential command format. */
#ifndef AUDIO_LINEAR_ENVELOPES
#define AUDIO_LINEAR_ENVELOPES 0
#endif

/* US/JP process each task immediately, then queue completed output at VBlank. */
#ifndef AUDIO_LOW_LATENCY
#define AUDIO_LOW_LATENCY 1
#endif
#if AUDIO_LOW_LATENCY && (defined(VERSION_US) || defined(VERSION_JP))
#define AUDIO_IMMEDIATE_TASKS 1
#else
#define AUDIO_IMMEDIATE_TASKS 0
#endif

/* US/JP Mupen audio follows the playback sample rate rather than rounded AI timing. */
#ifndef AUDIO_MUPEN_SAMPLE_CLOCK
#define AUDIO_MUPEN_SAMPLE_CLOCK 1
#endif

#if AUDIO_QUALITY_TRIAL || AUDIO_TRIAL == 9 || AUDIO_TRIAL == 10
#define AUDIO_OUTPUT_FREQUENCY 30000
#else
#define AUDIO_OUTPUT_FREQUENCY 32000
#endif

#if AUDIO_QUALITY_TRIAL
/* Q1.15 channel gain: roughly -72 dB, only for already released tails. */
#define AUDIO_QUIET_TAIL_THRESHOLD 8
#elif AUDIO_TRIAL == 8 || AUDIO_TRIAL == 10
/* At most one Q1.15 gain step per released voice, after a final fade. */
#define AUDIO_QUIET_TAIL_THRESHOLD 1
#endif

/******************
 * AUDIO SETTINGS *
 ******************/

/**
 * Do not restart the music on cap grabs.
 */
#define PERSISTENT_CAP_MUSIC

/**
 * Red coins should use the unchanging JP sound effect.
 */
// #define JP_RED_COIN_SOUND

/**
 * In vanilla, Mario's sounds are pitch shifted in real time in order to maintain a sense of
 * consistency. This can be annoying when replacing Mario's sounds. This define will disable this
 * behavior, making sound replacements easier to manage. Use of this is not recommended unless you're
 * actually replacing Mario's sounds. When toggling this define, you will need to run `make clean` for
 * it to apply.
 */
// #define DISABLE_MARIO_PITCH_SHIFTING

/** Slightly deeper Mario voice for either HE THICC appearance. */
#define HE_THICC_VOICE_PITCH_SCALE 0.97f

/** Subtle bass-line emphasis, without changing music pitch or tempo. */
#define HE_THICC_MUSIC_BASS_GAIN 1.10f
// Note-frequency scale 1 is middle C; 0.5 is the octave below (about 128 Hz).
#define HE_THICC_MUSIC_BASS_CUTOFF 0.5f

/** Quieter footsteps for either Standard Mario appearance. */
#define MARIO_FOOTSTEP_VOLUME_SCALE 0.595f
#define MARIO_JUMP_LANDING_VOLUME_SCALE 0.85f

/**
 * Enables the additional spacial processing that takes place with sound effects in vanilla.
 * SM64's implementation of this isn't the best, which causes some sounds like the waterfall in castle
 * grounds to sound more unpleasant than necessary. This is not the same thing as enabling stereo audio,
 * but it does represent the sole differences between the Stereo and Headset sound options. Because of
 * this, the Headset sound option will also not appear in the file select menu with this disabled.
 * Leaving this disabled may also be slightly beneficial for CPU performance.
 */
// #define ENABLE_STEREO_HEADSET_EFFECTS

/**
 * Increase audio heap size to allow for larger/more custom sequences/banks/sfx to be imported without
 * causing issues (not supported for SH). Note that this is REQUIRED in order to use the extended 0C
 * audio bank. Do not disable this if you're planning on using it.
 */
#define EXPAND_AUDIO_HEAP

/**
 * The maximum number of notes (sfx inclusive) that can sound at any given time (not supported for SH).
 * Lower values may cause notes to get cut more easily but can potentially improve performance slightly.
 * Lower values may cause problems with streamed audio if a sequence used for it is missing channel priority data.
 * Vanilla by default only generally allocates 16 or 20 notes at once. Memory usage is always determined by the largest of the two values here (~7200 bytes per US/JP note).
 */
#define MAX_SIMULTANEOUS_NOTES_EMULATOR 40
#define MAX_SIMULTANEOUS_NOTES_CONSOLE 20

/**
 * Uses a much better implementation of reverb over vanilla's fake echo reverb. Great for caves or eerie
 * levels, as well as just a better audio experience in general. Reverb presets can be configured in
 * audio/data.c to meet desired aesthetic/performance needs. More detailed usage info can also be found
 * on the HackerSM64 Wiki page.
 */
// Preset 0 in audio/data.c is the lightweight, console-compatible default.
// #define BETTER_REVERB

// Horizontal radii from the camera; selected sounds have the same vertical range.
#define OBJECT_MOTOR_SOUND_DISTANCE 2500.0f
#define OBJECT_AMBIENT_SOUND_DISTANCE 4000.0f
#define ENEMY_LOOP_SOUND_DISTANCE 2500.0f
#define ENEMY_FOOTSTEP_SOUND_DISTANCE 1000.0f
#define OBJECT_SOUND_VERTICAL_DISTANCE_SCALE 1.0f

// Console-class source budget: essential feedback may exceed the selected-source cap.
#ifndef SFX_BUDGET_TRIAL
#define SFX_BUDGET_TRIAL 1
#endif
#define SFX_SOURCE_BUDGET 6
#define SFX_LOOP_PRIORITY_BONUS 512
