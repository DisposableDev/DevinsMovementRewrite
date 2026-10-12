#!/usr/bin/env python3
"""Production request gates; host checks do not measure audio timing or audibility."""
from pathlib import Path
import subprocess
import tempfile
import unittest
from regression_support import Source

ROOT = Path(__file__).resolve().parents[1]


class DistantObjectSoundsTests(unittest.TestCase):
    def test_request_gates(self):
        source = Source(ROOT / 'src/game/spawn_sound.c')
        code = r'''
#include <stdint.h>
#include <assert.h>
typedef int32_t s32;
typedef uint32_t u32;
typedef float f32;
#include "sounds.h"
#include "level_table.h"
#include "object_constants.h"
#define TRUE 1
#define FALSE 0
#define GRAPH_RENDER_ACTIVE 1
struct Object {
    int oRoom;
    float oPosX, oPosY, oPosZ;
    struct {
        struct {
            struct {
                s32 flags;
            } node;
            f32 cameraToObject[3], pos[3];
        } gfx;
    } header;
};
static struct Object object;
static struct Object *gCurrentObject = &object;
static struct Object *gMarioObject = &object;
static int gMarioCurrentRoom, staticRoom;
static struct {
    int forwardRoom, backwardRoom;
} gDoorAdjacentRooms[20];
static int get_room_at_pos(float x, float y, float z) {
    (void) x;
    (void) y;
    (void) z;
    return staticRoom;
}
static struct {
    f32 pos[3];
} gLakituState = { { 137, 900, -321 } };
static int requests, rumbles;
static int16_t gCurrLevelNum = LEVEL_BOB;
static void play_sound(s32 sound, f32 *pos) {
    (void) sound;
    assert(pos == object.header.gfx.cameraToObject);
    requests++;
}
static void queue_rumble_data(int time, int strength) {
    (void) time;
    (void) strength;
    rumbles++;
}
/* PRODUCTION */
static void set_source_offset(f32 x, f32 y, f32 z) {
    object.header.gfx.pos[0] = gLakituState.pos[0] + x;
    object.header.gfx.pos[1] = gLakituState.pos[1] + y;
    object.header.gfx.pos[2] = gLakituState.pos[2] + z;
    // A pitched camera can give these very different axes. Gates use world height.
    object.header.gfx.cameraToObject[0] = x;
    object.header.gfx.cameraToObject[1] = z;
    object.header.gfx.cameraToObject[2] = -y;
}
static void check_range_boundaries(s32 sound, f32 range) {
    for (int path = 0; path < 2; path++) {
        void (*play)(s32) = path ? cur_obj_play_sound_2 : cur_obj_play_sound_1;
        for (int axis = 0; axis < 3; axis++) {
            for (int sign = -1; sign <= 1; sign += 2) {
                f32 offset[3] = { 0, 0, 0 };
                f32 cutoff = range;
                offset[axis] = sign * cutoff;
                set_source_offset(offset[0], offset[1], offset[2]);
                requests = 0;
                // In-range loops must keep refreshing every update.
                for (int frame = 0; frame < 8; frame++) {
                    play(sound);
                }
                assert(requests == 8);
                offset[axis] = sign * (cutoff + 1);
                set_source_offset(offset[0], offset[1], offset[2]);
                play(sound);
                assert(requests == 8);
                set_source_offset(0, 0, 0);
                play(sound);
                assert(requests == 9);
            }
        }
        set_source_offset(range * 0.8f, 0, range * 0.8f);
        requests = 0;
        play(sound);
        assert(requests == 0);
        set_source_offset(range, range, 0);
        play(sound);
        assert(requests == 1);
        set_source_offset(range, range + 1, 0);
        play(sound);
        assert(requests == 1);
    }
}
int main(void) {
    object.header.gfx.node.flags = GRAPH_RENDER_ACTIVE;
    /* RANGE_CHECKS */
    // The grounds waterfall bypasses distance gating; other distant ambience does not.
    set_source_offset(100000, 100000, 100000);
    gCurrLevelNum = LEVEL_CASTLE_GROUNDS;
    requests = 0;
    for (int frame = 0; frame < 8; frame++) {
        cur_obj_play_sound_1(SOUND_ENV_WATERFALL2);
    }
    assert(requests == 8);
    cur_obj_play_sound_1(SOUND_ENV_WATERFALL1);
    cur_obj_play_sound_1(SOUND_ENV_MOVINGSAND);
    assert(requests == 8);
    gCurrLevelNum = LEVEL_BOB;
    cur_obj_play_sound_1(SOUND_ENV_WATERFALL2);
    assert(requests == 8);
    set_source_offset(100000, 100000, 100000);
    // Important cues bypass these distance gates, and _2 retains impact rumble.
    requests = rumbles = 0;
    cur_obj_play_sound_2(SOUND_OBJ_GOOMBA_ALERT);
    cur_obj_play_sound_2(SOUND_OBJ_BOWSER_WALK);
    cur_obj_play_sound_2(SOUND_OBJ_WHOMP);
    cur_obj_play_sound_1(SOUND_ENV_STAR);
    assert(requests == 4 && rumbles == 2);
    // Exercise doorway adjacency, dynamic-room lookup and unknown-room allowance.
    gCurrLevelNum = LEVEL_BBH;
    set_source_offset(0, 0, 0);
    requests = 0;
    object.oRoom = 12;
    gMarioCurrentRoom = 10;
    cur_obj_play_sound_1(SOUND_ENV_ELEVATOR2);
    assert(requests == 0);
    gDoorAdjacentRooms[10].forwardRoom = 12;
    cur_obj_play_sound_1(SOUND_ENV_ELEVATOR2);
    assert(requests == 1);
    gMarioCurrentRoom = 0;
    staticRoom = 10;
    cur_obj_play_sound_1(SOUND_ENV_MERRY_GO_ROUND_CREAKING);
    cur_obj_play_sound_1(SOUND_AIR_BLOW_FIRE);
    assert(requests == 3);
    staticRoom = 13;
    cur_obj_play_sound_1(SOUND_ENV_MERRY_GO_ROUND_CREAKING);
    cur_obj_play_sound_1(SOUND_AIR_BLOW_FIRE);
    cur_obj_play_sound_1(SOUND_ENV_ELEVATOR2);
    assert(requests == 3);
    staticRoom = -1;
    cur_obj_play_sound_1(SOUND_AIR_BLOW_FIRE);
    assert(requests == 4);
    // Other levels and important cues retain their original paths.
    cur_obj_play_sound_1(SOUND_OBJ_BOO_LAUGH_LONG);
    assert(requests == 5);
    gCurrLevelNum = LEVEL_BOB;
    cur_obj_play_sound_1(SOUND_AIR_BLOW_FIRE);
    assert(requests == 6);
    // Inactive rendering suppresses the playback request and impact rumble together.
    requests = 4;
    object.header.gfx.node.flags = 0;
    cur_obj_play_sound_2(SOUND_OBJ_WHOMP);
    assert(requests == 4 && rumbles == 2);
    return 0;
}
'''
        production = Source(ROOT / 'include/config/config_audio.h').macros(
            'OBJECT_.*_SOUND_DISTANCE|ENEMY_.*_SOUND_DISTANCE|OBJECT_SOUND_VERTICAL_DISTANCE_SCALE')
        production += '\n'.join(source.function(name) for name in (
            'cur_obj_bbh_sound_is_in_room',
            'cur_obj_sound_is_in_range',
            'cur_obj_play_sound_1',
            'cur_obj_play_sound_2',
        ))
        sounds_by_range = {
            'OBJECT_MOTOR_SOUND_DISTANCE': [
                'ENV_ELEVATOR1',
                'ENV_ELEVATOR2',
                'ENV_ELEVATOR3',
                'ENV_ELEVATOR4',
            ],
            'OBJECT_AMBIENT_SOUND_DISTANCE': [
                'ENV_WATERFALL1',
                'ENV_WATERFALL2',
                'ENV_MOVINGSAND',
                'ENV_DRONING1',
                'ENV_DRONING2',
                'ENV_MERRY_GO_ROUND_CREAKING',
            ],
            'ENEMY_LOOP_SOUND_DISTANCE': [
                'AIR_AMP_BUZZ',
                'AIR_BLOW_FIRE',
                'ENV_WIND1',
            ],
            'ENEMY_FOOTSTEP_SOUND_DISTANCE': [
                'OBJ_GOOMBA_WALK',
                'OBJ_BOBOMB_WALK',
                'OBJ_KOOPA_WALK',
                'OBJ_KOOPA_THE_QUICK_WALK',
                'OBJ_BULLY_WALK_SMALL',
                'OBJ_BULLY_WALK_LARGE',
                'OBJ_SKEETER_WALK',
            ],
        }
        range_checks = '\n'.join(
            f'check_range_boundaries(SOUND_{sound}, {range_macro});'
            for range_macro, sounds in sounds_by_range.items() for sound in sounds
        )
        code = code.replace('/* PRODUCTION */', production).replace('/* RANGE_CHECKS */', range_checks)
        with tempfile.TemporaryDirectory(prefix='distant-object-sounds-') as directory:
            path = Path(directory)
            (path / 'test.c').write_text(code)
            subprocess.run(['cc', '-std=c99', '-O2', '-Wall', '-Wextra', '-Werror',
                            '-I', str(ROOT / 'include'), '-I', str(ROOT), str(path / 'test.c'),
                            '-o', str(path / 'test')], check=True)
            subprocess.run([str(path / 'test')], check=True)


if __name__ == '__main__':
    unittest.main()
