/* Real sound queue/stop functions. Sound-bank allocation, synthesis and rumble
 * playback are boundary doubles. Active linked-list entries are explicit fixtures. */
#include <stdint.h>
#include <math.h>
#include "test_support.h"
typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef int16_t s16;
typedef int32_t s32;
typedef float f32;
#define assertf(condition, message)                                                                    \
    do {                                                                                               \
        if (!(condition)) {                                                                            \
            fprintf(stderr, "HARNESS ERROR: %s\n", message);                                           \
            exit(2);                                                                                   \
        }                                                                                              \
    } while (0)
#include "audio_definitions.inc"

static struct Sound sSoundRequests[256];
static struct SoundCharacteristics sSoundBanks[16][40];
static u8 sNumProcessedSoundRequests, sSoundRequestCount;
static struct Sound sDispatched[256];
static int sDispatchedCount, sBankStops, sRumbleStops;
static s32 sDamageVoiceMuted;
static s32 game_options_damage_voice_muted(void) {
    return sDamageVoiceMuted;
}
static void update_background_music_after_sound(u8 bank, u8 index) {
    sBankStops++;
}
static void cancel_sound_rumble(u32 bits, f32 *pos) {
    sRumbleStops++;
}
static void process_sound_request(s32 bits, f32 *pos, s16 time, s16 level) {
    if (sDispatchedCount >= 256) {
        fprintf(stderr, "HARNESS ERROR: dispatch log overflow\n");
        exit(2);
    }
    sDispatched[sDispatchedCount++] = (struct Sound){ bits, pos, time, level };
}
static struct {
    f32 audioFrequency;
} gConfig = { 1.0f };
static u32 gAudioRandom;
static u8 sPalette;
static u8 mario_character_palette(void) {
    return sPalette;
}
struct Object {
    struct {
        struct {
            f32 cameraToObject[3];
        } gfx;
    } header;
};
static struct Object sMarioObject;
static struct Object *gMarioObject;
static struct {
    f32 forwardVel;
} gMarioStates[1];
#define absf(x) fabsf(x)
static f32 sLevelAcousticReaches[] = { 10000.0f };
static s32 gCurrLevelNum;
#define sqr(x) ((x) * (x))
struct SequencePlayer {
    s32 unused;
};
static struct SequencePlayer gSequencePlayers[3];
struct SequenceChannel {
    u8 instOrWave;
    struct SequencePlayer *seqPlayer;
};
struct AudioBankSound {
    f32 tuning;
};
struct SequenceChannelLayer {
    struct SequenceChannel *seqChannel;
    struct AudioBankSound *sound;
    f32 noteVelocity, noteFreqScale;
    u8 instOrWave;
};
#include "audio_sources.inc"

static f32 sPositionA[3], sPositionB[3]; // Equal coordinates, different sources.
#define SOUND_A ((1u << SOUNDARGS_SHIFT_BANK) | (0x42u << SOUNDARGS_SHIFT_SOUNDID) | 0x8001)
#define SOUND_B ((1u << SOUNDARGS_SHIFT_BANK) | (0x43u << SOUNDARGS_SHIFT_SOUNDID) | 0x8001)
#define SOUND_C ((2u << SOUNDARGS_SHIFT_BANK) | (0x42u << SOUNDARGS_SHIFT_SOUNDID) | 0x8001)
static void begin(void) {
    memset(sSoundRequests, 0, sizeof(sSoundRequests));
    memset(sSoundBanks, 0, sizeof(sSoundBanks));
    memset(sDispatched, 0, sizeof(sDispatched));
    for (int bank = 0; bank < 16; bank++) {
        sSoundBanks[bank][0].next = 0xff;
    }
    sNumProcessedSoundRequests = sSoundRequestCount = 0;
    sDispatchedCount = sBankStops = sRumbleStops = 0;
}

static int damage_voice(void) {
    const s32 hurt[] = { SOUND_MARIO_ATTACKED, SOUND_MARIO_OOOF, SOUND_MARIO_OOOF2, SOUND_MARIO_ON_FIRE,
                         SOUND_MARIO_DOH };
    const s32 preserved[] = { SOUND_A, SOUND_MARIO_YAHOO, SOUND_MARIO_UH, SOUND_MARIO_DYING };
    for (s32 muted = 0; muted < 2; muted++) {
        begin();
        sDamageVoiceMuted = muted;
        for (unsigned i = 0; i < sizeof(hurt) / sizeof(*hurt); i++) {
            play_sound_with_rumble(hurt[i], sPositionA, 10, 20);
            play_sound(hurt[i], sPositionA);
        }
        EXPECT_EQ(sSoundRequestCount, muted ? 0 : 2 * sizeof(hurt) / sizeof(*hurt));
        u8 start = sSoundRequestCount;
        for (unsigned i = 0; i < sizeof(preserved) / sizeof(*preserved); i++) {
            play_sound(preserved[i], sPositionA);
        }
        EXPECT_EQ(sSoundRequestCount - start, sizeof(preserved) / sizeof(*preserved));
    }
    sDamageVoiceMuted = 0;
    return 1;
}

static int queued(void) {
    begin();
    play_sound(SOUND_A, sPositionA);
    stop_sound(SOUND_A, sPositionA);
    EXPECT_EQ(sSoundRequests[0].soundBits, NO_SOUND);
    process_all_sound_requests();
    EXPECT_EQ(sDispatchedCount, 0);
    EXPECT_EQ(sNumProcessedSoundRequests, sSoundRequestCount);
    process_all_sound_requests();
    EXPECT_EQ(sDispatchedCount, 0); // No delayed replay.
    return 1;
}

static int active(void) {
    begin();
    sSoundBanks[1][0].next = 1;
    sSoundBanks[1][1].soundBits = SOUND_A;
    sSoundBanks[1][1].x = sPositionA;
    sSoundBanks[1][1].next = 2;
    sSoundBanks[1][2].soundBits = SOUND_A;
    sSoundBanks[1][2].x = sPositionB;
    sSoundBanks[1][2].next = 0xff;
    play_sound(SOUND_A, sPositionA); // A queued refresh and the active sound must both stop.
    stop_sound(SOUND_A, sPositionA);
    process_all_sound_requests();
    EXPECT_EQ(sDispatchedCount, 0);
    EXPECT_EQ(sSoundBanks[1][1].soundBits, NO_SOUND);
    EXPECT_EQ(sSoundBanks[1][2].soundBits, SOUND_A);
    EXPECT_EQ(sBankStops, 1);
    EXPECT_EQ(sRumbleStops, 1);
    return 1;
}

static int source_isolation(void) {
    begin();
    play_sound(SOUND_A, sPositionA);
    play_sound(SOUND_A, sPositionB);
    play_sound(SOUND_B, sPositionA);
    play_sound(SOUND_C, sPositionA);
    // Priority differs; identity still matches the first request.
    stop_sound(SOUND_A ^ 0x1000, sPositionA);
    process_all_sound_requests();
    EXPECT_EQ(sDispatchedCount, 3);
    EXPECT_EQ(sDispatched[0].soundBits, SOUND_A);
    EXPECT_EQ(sDispatched[0].position == sPositionB, 1);
    EXPECT_EQ(sDispatched[1].soundBits, SOUND_B);
    EXPECT_EQ(sDispatched[2].soundBits, SOUND_C);
    return 1;
}

static int queue_wrap(void) {
    begin();
    sNumProcessedSoundRequests = sSoundRequestCount = 250;
    for (int i = 0; i < 12; i++) {
        play_sound(SOUND_A, i % 2 ? sPositionB : sPositionA);
    }
    EXPECT_EQ(sSoundRequestCount, 6);
    stop_sound(SOUND_A, sPositionA);
    process_all_sound_requests();
    EXPECT_EQ(sDispatchedCount, 6);
    for (int i = 0; i < 6; i++) {
        EXPECT_EQ(sDispatched[i].position == sPositionB, 1);
    }
    EXPECT_EQ(sNumProcessedSoundRequests, 6);
    return 1;
}

static int repeated_stop(void) {
    begin();
    stop_sound(SOUND_A, sPositionA); // Empty queue.
    play_sound(SOUND_A, sPositionA);
    stop_sound(SOUND_A, sPositionA);
    stop_sound(SOUND_A, sPositionA);
    process_all_sound_requests();
    EXPECT_EQ(sDispatchedCount, 0);
    return 1;
}

static int queue_overflow(void) {
    for (int start = 0; start <= 250; start += 250) {
        begin();
        sNumProcessedSoundRequests = sSoundRequestCount = start;
        for (int i = 0; i < 512; i++) {
            play_sound_with_rumble(SOUND_A, sPositionA, i, 2);
        }
        process_all_sound_requests();
        EXPECT_EQ(sDispatchedCount, 255);
        for (int i = 0; i < 255; i++) {
            EXPECT_EQ(sDispatched[i].rumbleTime, i);
        }
        // Space becomes usable again immediately after consumption.
        play_sound_with_rumble(SOUND_B, sPositionB, 600, 3);
        process_all_sound_requests();
        EXPECT_EQ(sDispatchedCount, 256);
        EXPECT_EQ(sDispatched[255].soundBits, SOUND_B);
        EXPECT_EQ(sDispatched[255].rumbleTime, 600);
    }
    return 1;
}

static int new_request_after_stop(void) {
    begin();
    play_sound(SOUND_A, sPositionA);
    stop_sound(SOUND_A, sPositionA);
    play_sound_with_rumble(SOUND_A, sPositionA, 7, 2);
    process_all_sound_requests();
    EXPECT_EQ(sDispatchedCount, 1);
    EXPECT_EQ(sDispatched[0].rumbleTime, 7);
    EXPECT_EQ(sDispatched[0].rumbleLevel, 2);
    return 1;
}

static int mario_footstep_volume(void) {
    f32 (*volumePolicies[])(u8, u8, f32) = { get_sound_volume, get_sound_volume_jp };
    const f32 distances[] = { 0.0f, 4000.0f, 12000.0f };
    const u32 flags[] = { 0, SOUND_VIBRATO, SOUND_NO_VOLUME_LOSS };
    begin();
    gMarioObject = &sMarioObject;
    gAudioRandom = 7;
    gMarioStates[0].forwardVel = 24;

    for (unsigned region = 0; region < sizeof(volumePolicies) / sizeof(volumePolicies[0]); region++) {
        for (int cycle = 0; cycle < 3; cycle++) {
            for (int bank = 0; bank < 16; bank++) {
                for (int id = 0; id < 256; id++) {
                    int isStep = bank == SOUND_BANK_ACTION
                                 && ((id >= 0x10 && id <= 0x17) || (id >= 0x20 && id <= 0x27)
                                     || id == 0x2a || id == 0x2e || id == 0x2f || id == 0x52);
                    int isJumpLanding = (bank == SOUND_BANK_ACTION
                                         && (id <= 0x0f || (id >= 0x60 && id <= 0x67) || id == 0x28
                                             || id == 0x29 || id == 0x2b || id == 0x50 || id == 0x51))
                                        || (bank == SOUND_BANK_DETAILS && id == 0x84);
                    f32 gain = 1.0f;
                    if (isStep) {
                        gain = 0.595f;
                    } else if (isJumpLanding) {
                        gain = 0.85f;
                    }
                    for (unsigned mode = 0; mode < 3; mode++) {
                        sSoundBanks[bank][1].soundBits =
                            (u32) id << SOUNDARGS_SHIFT_SOUNDID | flags[mode];
                        sSoundBanks[bank][1].distance = distances[mode];
                        sSoundBanks[bank][1].x = sPositionB;
                        sPalette = MARIO_PALETTE_HE_THICC;
                        f32 original = volumePolicies[region](bank, 1, 0.75f);
                        sSoundBanks[bank][1].x = sMarioObject.header.gfx.cameraToObject;
                        sPalette = MARIO_PALETTE_STANDARD;
                        f32 quieter = volumePolicies[region](bank, 1, 0.75f);
                        EXPECT_EQ(fabsf(quieter - original * gain) < 0.00001f, 1);
                        // Every update recomputes gain; raw sound data and other sources stay intact.
                        EXPECT_EQ(fabsf(volumePolicies[region](bank, 1, 0.75f) - quieter) < 0.00001f,
                                  1);
                        sSoundBanks[bank][1].x = sPositionB;
                        EXPECT_EQ(fabsf(volumePolicies[region](bank, 1, 0.75f) - original) < 0.00001f,
                                  1);
                        sSoundBanks[bank][1].x = sMarioObject.header.gfx.cameraToObject;
                        gMarioObject = NULL;
                        EXPECT_EQ(fabsf(volumePolicies[region](bank, 1, 0.75f) - original) < 0.00001f,
                                  1);
                        gMarioObject = &sMarioObject;
                        sPalette = MARIO_PALETTE_HE_THICC;
                        f32 thiccGain = bank == SOUND_BANK_DETAILS && id == 0x87 ? .25f : 1.0f;
                        EXPECT_EQ(fabsf(volumePolicies[region](bank, 1, 0.75f) - original * thiccGain)
                                      < 0.00001f,
                                  1);
                    }
                }
            }
        }
    }
    return 1;
}

static int he_thicc_voice_pitch(void) {
    begin();
    const u8 marioIds[] = { 0, 1, 2, 0x24, 0x2B, 0x2C, 0x2F, 0x30, 0x37 };
    const u8 otherIds[] = { 0x25, 0x28, 0x38, 0x3F };
    const u32 flags[] = { 0, SOUND_CONSTANT_FREQUENCY, SOUND_VIBRATO };
    for (int cycle = 0; cycle < 3; cycle++) {
        for (unsigned flag = 0; flag < 3; flag++) {
            for (int far = 0; far < 2; far++) {
                for (unsigned id = 0; id < sizeof(marioIds); id++) {
                    sSoundBanks[SOUND_BANK_VOICE][1].soundBits =
                        ((u32) marioIds[id] << SOUNDARGS_SHIFT_SOUNDID) | flags[flag];
                    sSoundBanks[SOUND_BANK_VOICE][1].distance = far ? AUDIO_MAX_DISTANCE : 0;
                    gAudioRandom = 127;
                    gConfig.audioFrequency = 1.1f;
                    sPalette = MARIO_PALETTE_STANDARD;
                    f32 original = get_sound_freq_scale(SOUND_BANK_VOICE, 1);
                    sPalette = MARIO_PALETTE_HE_THICC;
                    f32 lower = get_sound_freq_scale(SOUND_BANK_VOICE, 1);
                    EXPECT_EQ(fabsf(lower - original * .97f) < .00001f, 1);
                    EXPECT_EQ(fabsf(get_sound_freq_scale(SOUND_BANK_VOICE, 1) - lower) < .00001f, 1);
                }
            }
        }
    }
    // The shared Peach bank, other effects and returning to Standard retain their pitch.
    for (unsigned id = 0; id < sizeof(otherIds); id++) {
        sSoundBanks[SOUND_BANK_VOICE][1].soundBits = (u32) otherIds[id] << SOUNDARGS_SHIFT_SOUNDID;
        sPalette = MARIO_PALETTE_STANDARD;
        f32 original = get_sound_freq_scale(SOUND_BANK_VOICE, 1);
        sPalette = MARIO_PALETTE_HE_THICC;
        EXPECT_EQ(get_sound_freq_scale(SOUND_BANK_VOICE, 1), original);
    }
    for (int bank = 0; bank < SOUND_BANK_COUNT; bank++) {
        if (bank != SOUND_BANK_VOICE) {
            sPalette = MARIO_PALETTE_STANDARD;
            f32 original = get_sound_freq_scale(bank, 1);
            sPalette = MARIO_PALETTE_HE_THICC;
            EXPECT_EQ(get_sound_freq_scale(bank, 1), original);
        }
    }
    sPalette = MARIO_PALETTE_STANDARD;
    gConfig.audioFrequency = 1.0f;
    gAudioRandom = 0;
    return 1;
}

static int he_thicc_running_bass(void) {
    begin();
    sPalette = MARIO_PALETTE_HE_THICC;
    gMarioObject = &sMarioObject;
    struct SoundCharacteristics *sound = &sSoundBanks[SOUND_BANK_DETAILS][1];
    sound->soundBits = SOUND_HE_THICC_RUNNING_STEP | SOUND_CONSTANT_FREQUENCY;
    sound->x = sMarioObject.header.gfx.cameraToObject;
    gConfig.audioFrequency = 1.0f;
    const f32 speeds[] = { 0, 24, 30, 36, 42, 48, 100, -36 };
    const f32 weights[] = { 0, 0, .25f, .5f, .75f, 1, 1, .5f };
    for (unsigned i = 0; i < sizeof(speeds) / sizeof(*speeds); i++) {
        gMarioStates[0].forwardVel = speeds[i];
        for (int repeat = 0; repeat < 3; repeat++) {
            EXPECT_EQ(fabsf(get_mario_movement_volume_scale(SOUND_BANK_DETAILS, 1)
                            - (.25f + .5f * weights[i]))
                          < .0001f,
                      1);
            EXPECT_EQ(fabsf(get_sound_freq_scale(SOUND_BANK_DETAILS, 1) - (1.35f - .25f * weights[i]))
                          < .0001f,
                      1);
        }
    }
    sound->x = sPositionA;
    EXPECT_EQ(get_mario_movement_volume_scale(SOUND_BANK_DETAILS, 1), 1);
    EXPECT_EQ(get_sound_freq_scale(SOUND_BANK_DETAILS, 1), 1);
    sound->x = sMarioObject.header.gfx.cameraToObject;
    sound->soundBits = SOUND_HE_THICC_CLAP | SOUND_CONSTANT_FREQUENCY;
    EXPECT_EQ(get_mario_movement_volume_scale(SOUND_BANK_DETAILS, 1), 1);
    EXPECT_EQ(get_sound_freq_scale(SOUND_BANK_DETAILS, 1), 1);
    EXPECT_EQ((SOUND_HE_THICC_CLAP & SOUNDARGS_MASK_PRIORITY) >> SOUNDARGS_SHIFT_PRIORITY, 0xA0);
    EXPECT_EQ((SOUND_HE_THICC_CLAP & SOUNDARGS_MASK_PRIORITY)
                  > (SOUND_HE_THICC_RUNNING_STEP & SOUNDARGS_MASK_PRIORITY),
              1);
    sound->soundBits = SOUND_HE_THICC_RUNNING_STEP | SOUND_CONSTANT_FREQUENCY;
    sPalette = MARIO_PALETTE_STANDARD;
    EXPECT_EQ(get_mario_movement_volume_scale(SOUND_BANK_DETAILS, 1), 1);
    EXPECT_EQ(get_sound_freq_scale(SOUND_BANK_DETAILS, 1), 1);
    return 1;
}

static int he_thicc_music_bass(void) {
    f32 (*helpers[])(struct SequenceChannelLayer *) = { music_note_velocity, music_note_velocity_eu,
                                                        music_note_velocity_sh };
    const f32 pitches[] = { 0.0f, 0.25f, 0.5f, 0.5001f, 1.0f };
    const f32 tunings[] = { 0.5f, 1.0f, 2.0f };
    for (unsigned region = 0; region < sizeof(helpers) / sizeof(*helpers); region++) {
        for (int cycle = 0; cycle < 3; cycle++) {
            for (int player = SEQ_PLAYER_LEVEL; player <= SEQ_PLAYER_SFX; player++) {
                for (unsigned pitch = 0; pitch < sizeof(pitches) / sizeof(*pitches); pitch++) {
                    for (unsigned tuning = 0; tuning < sizeof(tunings) / sizeof(*tunings); tuning++) {
                        struct SequenceChannel channel = { 1, &gSequencePlayers[player] };
                        struct AudioBankSound sound = { tunings[tuning] };
                        struct SequenceChannelLayer layer = { &channel, &sound, 100.0f,
                                                              pitches[pitch] * sound.tuning, 0xff };
                        sPalette = MARIO_PALETTE_STANDARD;
                        EXPECT_EQ(helpers[region](&layer), 100);
                        sPalette = MARIO_PALETTE_HE_THICC;
                        f32 expected =
                            player != SEQ_PLAYER_SFX && pitches[pitch] > 0.0f && pitches[pitch] <= 0.5f
                                ? 110.0f
                                : 100.0f;
                        for (int update = 0; update < 30; update++) {
                            EXPECT_EQ(fabsf(helpers[region](&layer) - expected) < 0.00001f, 1);
                        }
                        EXPECT_EQ(layer.noteVelocity, 100);
                        EXPECT_EQ(layer.noteFreqScale, pitches[pitch] * sound.tuning);
                        sPalette = MARIO_PALETTE_STANDARD;
                        EXPECT_EQ(helpers[region](&layer), 100);
                    }
                }
            }
        }
        struct SequenceChannel channel = { 0, &gSequencePlayers[SEQ_PLAYER_LEVEL] };
        struct AudioBankSound sound = { 1.0f };
        struct SequenceChannelLayer layer = { &channel, &sound, 100.0f, 0.25f, 0xff };
        sPalette = MARIO_PALETTE_HE_THICC;
        EXPECT_EQ(helpers[region](&layer), 100); // Percussion is unchanged.
        channel.instOrWave = 0x80;
        layer.sound = NULL;
        EXPECT_EQ(helpers[region](&layer), 100);
        channel.instOrWave = 1;
        EXPECT_EQ(helpers[region](&layer), 100); // Missing or invalid sample tuning.
        layer.sound = &sound;
        sound.tuning = 0;
        EXPECT_EQ(helpers[region](&layer), 100);
        sound.tuning = 1;
        layer.seqChannel = NULL;
        EXPECT_EQ(helpers[region](&layer), 100);
        layer.seqChannel = &channel;
        channel.seqPlayer = NULL;
        EXPECT_EQ(helpers[region](&layer), 100);
        channel.seqPlayer = &gSequencePlayers[SEQ_PLAYER_LEVEL];
        if (region != 0) {
            layer.instOrWave = 0;
            EXPECT_EQ(helpers[region](&layer), 100); // Layer-level drum override.
            channel.instOrWave = 0;
            layer.instOrWave = 1;
            EXPECT_EQ(fabsf(helpers[region](&layer) - 110.0f) < 0.00001f, 1);
        }
    }
    sPalette = MARIO_PALETTE_STANDARD;
    return 1;
}

int main(int argc, char **argv) {
    const struct {
        const char *name;
        int (*run)(void);
    } tests[] = {
        { "audio.damage_voice", damage_voice },
        { "audio.he_thicc_voice_pitch", he_thicc_voice_pitch },
        { "audio.mario_footstep_volume", mario_footstep_volume },
        { "audio.he_thicc_music_bass", he_thicc_music_bass },
        { "audio.he_thicc_running_bass", he_thicc_running_bass },
        { "audio.queued", queued },
        { "audio.active", active },
        { "audio.source_isolation", source_isolation },
        { "audio.queue_wrap", queue_wrap },
        { "audio.queue_overflow", queue_overflow },
        { "audio.repeated_stop", repeated_stop },
        { "audio.new_request_after_stop", new_request_after_stop },
    };
    test_init(argc, argv);
    for (unsigned int i = 0; i < sizeof(tests) / sizeof(tests[0]); i++) {
        if (test_selected(tests[i].name)) {
            test_report(tests[i].name, tests[i].run());
        }
    }
    return test_finish();
}
