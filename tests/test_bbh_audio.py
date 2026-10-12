#!/usr/bin/env python3
"""BBH audio lifecycle host tests; no emulator timing or listening claims."""
from pathlib import Path
import subprocess
import tempfile
import unittest
from regression_support import Source

ROOT = Path(__file__).resolve().parents[1]


def compile_run(code):
    with tempfile.TemporaryDirectory(prefix='bbh-audio-') as directory:
        path = Path(directory)
        (path / 'test.c').write_text(code)
        subprocess.run(['cc', '-std=c99', '-O2', '-Wall', '-Wextra', '-Werror',
                        '-I', str(ROOT / 'include'), '-I', str(ROOT),
                        str(path / 'test.c'), '-o', str(path / 'test')], check=True)
        subprocess.run([str(path / 'test')], check=True)


class BbhAudioTests(unittest.TestCase):
    def test_music_stop_restore_and_interruptions(self):
        source = Source(ROOT / 'src/audio/external.c')
        code = r'''
#include <assert.h>
#include <stdint.h>
#include <string.h>
#include "seq_ids.h"
#include "level_table.h"
typedef uint8_t u8;
typedef uint16_t u16;
typedef int32_t s32;
typedef float f32;
typedef s32 FadeT;
#define TRUE 1
#define FALSE 0
#define VERSION_US
#define PERSISTENT_CAP_MUSIC
#define MAX_BACKGROUND_MUSIC_QUEUE_SIZE 6
#define SEQ_PLAYER_LEVEL 0
#define SEQ_PLAYER_ENV 1
#define SEQ_PLAYER_SFX 2
static int simpleFlipsRoom;
static int jrb_simpleflips_music_allowed(u8 player, u8 seq) {
    (void) seq;
    return !simpleFlipsRoom || player == 2;
}
#define CHANNELS_MAX 1
#define SEQ_BASE_ID 0x7f
#define SEQ_VARIATION 0x80
#define TARGET_VOLUME_UNSET 0xff
#define TARGET_VOLUME_VALUE_MASK 0x7f
#define TARGET_VOLUME_IS_PRESENT_FLAG 0x80
#define SEQUENCE_PLAYER_STATE_0 0
#define SEQUENCE_PLAYER_STATE_FADE_OUT 1
#define SEQUENCE_PLAYER_STATE_2 2
#define SEQUENCE_PLAYER_STATE_3 3
#define SEQUENCE_PLAYER_STATE_4 4
#define FLOAT_CAST(x) ((f32) (x))
#define CLAMP(x, lo, hi) ((x) < (lo) ? (lo) : ((x) > (hi) ? (hi) : (x)))
#define IS_SEQUENCE_CHANNEL_VALID(x) ((x) != NULL)
struct Channel {
    int enabled;
};
struct SequencePlayer {
    int enabled, state, seqVariation;
    u16 fadeRemainingFrames;
    f32 fadeVolume, fadeVelocity, volume, volumeDefault;
    struct Channel *channels[CHANNELS_MAX];
};
static struct SequencePlayer gSequencePlayers[2];
static struct {
    int remainingFrames;
} D_80360928[2][CHANNELS_MAX];
static u8 sCurrentBackgroundMusicSeqId, sBackgroundMusicForDynamics;
static u8 sCurrentMusicDynamic, sMusicDynamicDelay, sBackgroundMusicTargetVolume;
static u8 sBackgroundMusicMaxTargetVolume, sLowerBackgroundMusicVolume;
static u8 sSoundBanksThatLowerBackgroundMusic, D_80332120, D_80332124;
static u8 sBbhBackgroundMusicStopped;
// Observe sequence loading/disabling service calls; no native audio assets are loaded.
static int gCurrLevelNum = LEVEL_BBH, sequenceLoadCalls[2], sequenceDisableCalls;
static struct {
    u8 seqId, priority;
} sBackgroundMusicQueue[6];
static u8 sBackgroundMusicQueueSize;
static void append_puppyprint_log(const char *message) {
    (void) message;
}
static void stop_background_music(u16 seqId);
static u8 begin_background_music_fade(u16 fadeDuration);
static void sequence_player_disable(struct SequencePlayer *player) {
    player->enabled = FALSE;
    sequenceDisableCalls++;
}
static void sequence_channel_process_sound(struct Channel *channel) {
    (void) channel;
}
static void load_sequence(int player, int seq, int async) {
    (void) seq;
    (void) async;
    sequenceLoadCalls[player]++;
    memset(&gSequencePlayers[player], 0, sizeof(gSequencePlayers[player]));
    gSequencePlayers[player].enabled = TRUE;
    gSequencePlayers[player].volume = 1;
    gSequencePlayers[player].volumeDefault = 1;
    gSequencePlayers[player].fadeVolume = 1;
}
/* PRODUCTION */
// Each iteration processes enabled sequence players, not a full audio or game frame.
static void advance_sequence_players(int sequenceUpdates) {
    for (int i = 0; i < sequenceUpdates; i++) {
        for (int p = 0; p < 2; p++) {
            if (gSequencePlayers[p].enabled) {
                sequence_player_process_sound(&gSequencePlayers[p]);
            }
        }
    }
}
// Partial setup: reset observations and background policy/queue, then start main music.
// Environment-player storage is not reset here.
static void start_spooky_background(void) {
    memset(sequenceLoadCalls, 0, sizeof(sequenceLoadCalls));
    sequenceDisableCalls = 0;
    sBackgroundMusicTargetVolume = sBackgroundMusicMaxTargetVolume = TARGET_VOLUME_UNSET;
    sLowerBackgroundMusicVolume = sSoundBanksThatLowerBackgroundMusic = 0;
    D_80332120 = 0;
    sBbhBackgroundMusicStopped = 0;
    sBackgroundMusicQueueSize = 0;
    memset(sBackgroundMusicQueue, 0, sizeof(sBackgroundMusicQueue));
    play_music(SEQ_PLAYER_LEVEL, (5 << 8) | SEQ_LEVEL_SPOOKY, 0);
}
int main(void) {
    // Music cannot start in the isolated room; leaving restores normal requests.
    simpleFlipsRoom = 1;
    seq_player_play_sequence(SEQ_PLAYER_LEVEL, SEQ_LEVEL_WATER, 0);
    seq_player_play_sequence(SEQ_PLAYER_ENV, SEQ_EVENT_POWERUP, 0);
    assert(sequenceLoadCalls[0] == 0 && sequenceLoadCalls[1] == 0);
    simpleFlipsRoom = 0;
    start_spooky_background();
    // The main sequence remains enabled outside the enclosure.
    play_secondary_music(SEQ_EVENT_MERRY_GO_ROUND, 45, 20, 8);
    advance_sequence_players(8);
    assert(gSequencePlayers[0].enabled && sequenceLoadCalls[0] == 1);
    // Repeated refresh must not restart the fade and prevent actual cleanup.
    for (int cycle = 0; cycle < 3; cycle++) {
        for (int frame = 0; frame < 8; frame++) {
            play_secondary_music(SEQ_EVENT_MERRY_GO_ROUND, 0, 78, 8);
            advance_sequence_players(1);
        }
        assert(!gSequencePlayers[0].enabled && gSequencePlayers[1].enabled);
        assert(sCurrentBackgroundMusicSeqId == SEQ_LEVEL_SPOOKY);
        play_secondary_music(SEQ_EVENT_MERRY_GO_ROUND, 45, 20, 8);
        assert(gSequencePlayers[0].enabled && gSequencePlayers[0].fadeVolume == 0);
        advance_sequence_players(8);
        assert(gSequencePlayers[0].fadeVolume > .35f);
    }
    assert(sequenceLoadCalls[0] == 4 && sequenceDisableCalls == 3);
    // Quick exit cancels cleanup without restarting or disabling the track.
    play_secondary_music(SEQ_EVENT_MERRY_GO_ROUND, 0, 78, 8);
    advance_sequence_players(2);
    float volume = gSequencePlayers[0].fadeVolume;
    play_secondary_music(SEQ_EVENT_MERRY_GO_ROUND, 45, 20, 8);
    advance_sequence_players(8);
    assert(gSequencePlayers[0].enabled && sequenceLoadCalls[0] == 4);
    assert(gSequencePlayers[0].fadeVolume > volume);
    // Leaving the basement/stopping the carousel restores main and stops secondary.
    play_secondary_music(SEQ_EVENT_MERRY_GO_ROUND, 0, 78, 8);
    advance_sequence_players(8);
    stop_secondary_music(8);
    advance_sequence_players(8);
    assert(gSequencePlayers[0].enabled && !gSequencePlayers[1].enabled);
    assert(gSequencePlayers[0].fadeVolume == 1);
    // Cap/other priority music is never stopped by the carousel policy.
    start_spooky_background();
    seq_player_play_sequence(SEQ_PLAYER_LEVEL, SEQ_EVENT_POWERUP, 0);
    play_secondary_music(SEQ_EVENT_MERRY_GO_ROUND, 0, 78, 8);
    advance_sequence_players(8);
    assert(gSequencePlayers[0].enabled && !sBbhBackgroundMusicStopped);
    start_spooky_background();
    play_secondary_music(SEQ_EVENT_PIRANHA_PLANT, 0, 78, 8);
    advance_sequence_players(8);
    assert(gSequencePlayers[0].enabled && !sBbhBackgroundMusicStopped);
    // A temporary fanfare keeps the suspended main stopped, then restores the carousel.
    start_spooky_background();
    play_secondary_music(SEQ_EVENT_MERRY_GO_ROUND, 0, 78, 8);
    advance_sequence_players(8);
    play_star_fanfare();
    advance_sequence_players(8);
    assert(!gSequencePlayers[0].enabled && gSequencePlayers[1].enabled);
    gSequencePlayers[1].enabled = FALSE; // The fanfare script ends.
    func_80320ED8();
    assert(!gSequencePlayers[0].enabled && gSequencePlayers[1].enabled);
    play_secondary_music(SEQ_EVENT_MERRY_GO_ROUND, 45, 20, 8);
    advance_sequence_players(8);
    assert(gSequencePlayers[0].enabled);
    play_secondary_music(SEQ_EVENT_MERRY_GO_ROUND, 0, 78, 8);
    advance_sequence_players(8);
    assert(!gSequencePlayers[0].enabled);
    // Returning from cap music must still stop the newly loaded silent main track.
    seq_player_play_sequence(SEQ_PLAYER_LEVEL, SEQ_EVENT_POWERUP, 0);
    seq_player_play_sequence(SEQ_PLAYER_LEVEL, SEQ_LEVEL_SPOOKY, 0);
    advance_sequence_players(2);
    assert(!gSequencePlayers[0].enabled && sBbhBackgroundMusicStopped);
    // A queued lower-priority request must not mistake deliberate suspension for completion.
    sBackgroundMusicQueueSize = 2;
    sBackgroundMusicQueue[1].seqId = SEQ_EVENT_PIRANHA_PLANT;
    sBackgroundMusicQueue[1].priority = 1;
    play_music(SEQ_PLAYER_LEVEL, (1 << 8) | SEQ_EVENT_PIRANHA_PLANT, 0);
    assert(sBackgroundMusicQueueSize == 2 && sBackgroundMusicQueue[0].seqId == SEQ_LEVEL_SPOOKY);
    // A global music stop must not resurrect a track on secondary cleanup.
    seq_player_fade_out(SEQ_PLAYER_LEVEL, 8);
    stop_secondary_music(8);
    advance_sequence_players(8);
    assert(!gSequencePlayers[0].enabled && sCurrentBackgroundMusicSeqId == SEQUENCE_NONE);
    return 0;
}
'''
        names = ('seq_player_fade_to_zero_volume', 'func_8031D690',
                 'seq_player_fade_to_normal_volume', 'seq_player_fade_to_target_volume',
                 'seq_player_play_sequence', 'seq_player_fade_out',
                 'update_bbh_background_music', 'begin_background_music_fade',
                 'play_secondary_music', 'stop_secondary_music',
                 'play_star_fanfare', 'play_music', 'stop_background_music')
        production = '\n'.join(source.function(name) for name in names)
        production += source.region(r'^void func_80320ED8\(void\) \{', r'^void play_secondary_music\(')
        production += Source(ROOT / 'src/audio/effects.c').function('sequence_player_process_sound')
        compile_run(code.replace('/* PRODUCTION */', production))

    def test_wind_outdoor_platform_and_indoor_entry(self):
        source = Source(ROOT / 'src/game/behaviors/bbh_merry_go_round.inc.c')
        code = r'''
#include <assert.h>
#include <stdint.h>
#include "sounds.h"
#include "seq_ids.h"
#include <stddef.h>
#include "object_constants.h"
#include "surface_terrains.h"
typedef uint16_t u16;
typedef int32_t s32;
typedef uint32_t u32;
#define FALSE 0
#define TRUE 1
struct Object {
    int oMerryGoRoundMarioIsOutside, oMerryGoRoundStopped, oMerryGoRoundMusicShouldPlay;
    int oAngleVelYaw, oMoveAngleYaw, oFaceAngleYaw;
};
static struct Object object, *o = &object;
struct Surface {
    u16 type;
};
static struct Surface floor;
static struct {
    float pos[3];
    struct Surface *floor;
} mario, *gMarioState = &mario;
// Room lookup uses a supplied result; this fixture does not query real geometry.
// Counters observe service requests to sound/music/collision doubles.
static int gMarioCurrentRoom, resolvedStaticRoom, windRequests, secondaryMusicRequests, creakRequests,
    secondaryMusicStops;
static int carouselActive = 1, collisionLoadCalls;
static int bbh_merry_go_round_is_active(void) {
    return carouselActive;
}
static void load_object_collision_model(void) {
    collisionLoadCalls++;
}
static int requestedBackgroundVolume, gMarioOnMerryGoRound;
static float gGlobalSoundSource[3];
static int get_room_at_pos(float x, float y, float z) {
    (void) x;
    (void) y;
    (void) z;
    return resolvedStaticRoom;
}
static void play_sound(int sound, float *pos) {
    assert(sound == SOUND_AIR_HOWLING_WIND && pos == gGlobalSoundSource);
    windRequests++;
}
static int cur_obj_is_mario_on_platform(void) {
    return 0;
}
static void cur_obj_play_sound_1(int sound) {
    assert((u32) sound == SOUND_ENV_MERRY_GO_ROUND_CREAKING);
    creakRequests++;
}
static void play_secondary_music(int seq, int bg, int volume, int fade) {
    (void) seq;
    (void) volume;
    (void) fade;
    requestedBackgroundVolume = bg;
    secondaryMusicRequests++;
}
static void stop_secondary_music(int fade) {
    assert(fade == 300);
    secondaryMusicStops++;
}
/* PRODUCTION */
int main(void) {
    // Direct and dynamic-floor outdoor room IDs both request wind.
    gMarioCurrentRoom = BBH_OUTSIDE_ROOM;
    bhv_merry_go_round_loop();
    assert(windRequests == 1);
    gMarioCurrentRoom = BBH_DYNAMIC_SURFACE_ROOM;
    resolvedStaticRoom = BBH_OUTSIDE_ROOM;
    bhv_merry_go_round_loop();
    assert(windRequests == 2);
    resolvedStaticRoom = BBH_NEAR_MERRY_GO_ROUND_ROOM;
    bhv_merry_go_round_loop();
    assert(windRequests == 2 && !o->oMerryGoRoundMarioIsOutside);
    assert(secondaryMusicRequests == 1 && requestedBackgroundVolume == 45);
    // Supply the music-floor type while the direct platform probe remains false.
    floor.type = SURFACE_MGR_MUSIC;
    mario.floor = &floor;
    bhv_merry_go_round_loop();
    assert(gMarioOnMerryGoRound && requestedBackgroundVolume == 0 && creakRequests == 1);
    // An unrelated indoor dynamic platform must not retain carousel music.
    resolvedStaticRoom = 1;
    mario.floor = NULL;
    bhv_merry_go_round_loop();
    assert(!gMarioOnMerryGoRound && secondaryMusicStops == 1 && creakRequests == 1);
    assert(!o->oMerryGoRoundMusicShouldPlay && windRequests == 2);
    gMarioCurrentRoom = BBH_OUTSIDE_ROOM;
    bhv_merry_go_round_loop();
    assert(windRequests == 3);
    gMarioCurrentRoom = 1;
    bhv_merry_go_round_loop();
    assert(windRequests == 3);
    assert(secondaryMusicRequests == 3 && o->oMoveAngleYaw == 7 * 0x80);
    assert(collisionLoadCalls == 7);
    // Dormancy suspends rotation and collision submissions.
    carouselActive = 0;
    bhv_merry_go_round_loop();
    assert(o->oMoveAngleYaw == 7 * 0x80 && o->oAngleVelYaw == 0 && collisionLoadCalls == 7);
    carouselActive = 1;
    // A stopped carousel does not start another secondary-music request.
    o->oMerryGoRoundStopped = 1;
    bhv_merry_go_round_loop();
    assert(secondaryMusicRequests == 3 && o->oAngleVelYaw == 0);
    return 0;
}
'''
        production = source.function('handle_merry_go_round_music') + source.function('bhv_merry_go_round_loop')
        compile_run(code.replace('/* PRODUCTION */', production))


if __name__ == '__main__':
    unittest.main()
