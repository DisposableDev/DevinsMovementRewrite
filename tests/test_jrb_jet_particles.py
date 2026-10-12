"""JRB jet density/lifetime and native particle-motion preservation."""
from pathlib import Path
import os
import subprocess
import tempfile
import unittest
from regression_support import Source

ROOT = Path(__file__).resolve().parents[1]


class JetParticles(unittest.TestCase):
    def test_count_lifetime_and_motion(self):
        bubbles = Source(ROOT / 'src/game/envfx_bubbles.c')
        definitions = Source(ROOT / 'src/game/envfx_bubbles.h')
        fixture = r'''

#include <assert.h>
#include <stdint.h>
#include <string.h>
#include <math.h>
typedef int8_t s8;
typedef uint8_t u8;
typedef int16_t s16;
typedef int32_t s32;
typedef float f32;
#define TRUE 1
#define FALSE 0
#define LEVEL_JRB 12
#define ENVFX_MODE_NONE 0
#define ENVFX_FLOWERS 11
#define ENVFX_LAVA_BUBBLES 12
#define ENVFX_WHIRLPOOL_BUBBLES 13
#define ENVFX_JETSTREAM_BUBBLES 14
#define SOUND_ENV_WATER 1
#define sqr(x) ((x) * (x))
#define bzero(p, n) memset(p, 0, n)
static int gCurrLevelNum = LEVEL_JRB, gEnvFxMode, soundRequests, requestedAllocationBytes;
static int sBubbleParticleCapacity, sBubbleActiveCount;
static void *gEffectsMemoryPool;
static uint32_t rng = 1;
static int randomDraws;
static uint16_t random_u16(void) {
    randomDraws++;
    rng = rng * 1664525u + 1013904223u;
    return rng >> 16;
}
static float random_float(void) {
    return random_u16() / 65536.0f;
}
static float sins(s32 angle) {
    return sinf((uint16_t) angle * (6.28318530718f / 65536.0f));
}
static float coss(s32 angle) {
    return cosf((uint16_t) angle * (6.28318530718f / 65536.0f));
}
struct Object {
    float oDistanceToMario, oPosX, oPosY, oPosZ;
} emitter, *o = &emitter;
static void cur_obj_play_sound_1(int sound) {
    soundRequests++;
}
'''
        fixture += Source(ROOT / 'src/game/envfx_snow.h').declaration('struct', 'EnvFxParticle')
        fixture += definitions.macros('JRB_JETSTREAM_BUBBLE_.*')
        fixture += definitions.declaration('enum', 'EnvfxBubblesState')
        fixture += r'''

static s16 gEnvFxBubbleConfig[10];
static struct EnvFxParticle storage[60], reference[60], actual[60], *gEnvFxBuffer = storage;
static void *mem_pool_alloc(void *pool, int bytes) {
    requestedAllocationBytes = bytes;
    assert(bytes <= sizeof(storage));
    return storage;
}
'''
        for function in ('envfx_init_bubble', 'envfx_set_max_bubble_particles', 'envfx_update_jetstream'):
            fixture += bubbles.function(function)
        fixture += Source(ROOT / 'src/game/behaviors/whirlpool.inc.c').function('bhv_jet_stream_loop')
        fixture += r'''

// Original arithmetic, truncation and RNG order for jets outside JRB.
static void reference_update(void) {
    for (int i = 0; i < sBubbleActiveCount; i++) {
        struct EnvFxParticle *p = &reference[i];
        int dx = p->xPos - gEnvFxBubbleConfig[ENVFX_STATE_SRC_X];
        int dz = p->zPos - gEnvFxBubbleConfig[ENVFX_STATE_SRC_Z];
        p->isAlive =
            (sqr(dx) + sqr(dz) <= sqr(1000) && p->yPos <= gEnvFxBubbleConfig[ENVFX_STATE_SRC_Y] + 1500);
        if (!p->isAlive) {
            p->angleAndDist[1] = random_float() * 300.0f;
            p->angleAndDist[0] = random_u16();
            p->xPos =
                gEnvFxBubbleConfig[ENVFX_STATE_SRC_X] + sins(p->angleAndDist[0]) * p->angleAndDist[1];
            p->zPos =
                gEnvFxBubbleConfig[ENVFX_STATE_SRC_Z] + coss(p->angleAndDist[0]) * p->angleAndDist[1];
            p->yPos = gEnvFxBubbleConfig[ENVFX_STATE_SRC_Y] + (random_float() * 400.0f - 200.0f);
        } else {
            p->angleAndDist[1] += 10;
            p->xPos += sins(p->angleAndDist[0]) * 10.0f;
            p->zPos += coss(p->angleAndDist[0]) * 10.0f;
            p->yPos -= (p->angleAndDist[1] / 30) - 50;
        }
    }
}
int main(void) {
    assert(sizeof(struct EnvFxParticle) == 56);
    // Observe complete visible lives and hidden gaps, including first appearance.
    assert(envfx_init_bubble(ENVFX_JETSTREAM_BUBBLES));
    assert(sBubbleParticleCapacity == 30 && requestedAllocationBytes == 30 * sizeof(storage[0]));
    emitter = (struct Object){ 4999, 4988, -5221, 2473 };
    bhv_jet_stream_loop();
    envfx_set_max_bubble_particles(ENVFX_JETSTREAM_BUBBLES);
    int birthCounts[30] = { 0 }, visibleFrames[30] = { 0 }, hiddenFrames[30] = { 0 };
    int expectedLifetimeFrames[30] = { 0 }, expectedGapFrames[30] = { 0 };
    unsigned lifeMask = 0, gapMask = 0, startMask = 0;
    for (int frame = 0; frame < 2000; frame++) {
        memcpy(actual, storage, sizeof(storage));
        randomDraws = 0;
        envfx_update_jetstream();
        int spawned = 0;
        for (int i = 0; i < 30; i++) {
            struct EnvFxParticle *p = &storage[i], *before = &actual[i];
            if (p->unusedBubbleVar > 0) {
                assert(p->animFrame >= 19 && p->animFrame <= 24);
                assert(p->bubbleY >= 0 && p->bubbleY <= 3);
                assert(p->unusedBubbleVar <= p->animFrame);
                if (p->unusedBubbleVar == 1) {
                    spawned++;
                    if (birthCounts[i] == 0) {
                        assert(frame <= 6);
                        startMask |= 1u << frame;
                    } else {
                        assert(visibleFrames[i] == expectedLifetimeFrames[i]
                               && hiddenFrames[i] == expectedGapFrames[i]);
                        gapMask |= 1u << hiddenFrames[i];
                    }
                    assert(p->angleAndDist[1] >= 0 && p->angleAndDist[1] < 300);
                    assert(p->yPos >= emitter.oPosY - 200 && p->yPos <= emitter.oPosY + 200);
                    expectedLifetimeFrames[i] = p->animFrame;
                    expectedGapFrames[i] = p->bubbleY;
                    lifeMask |= 1u << (p->animFrame - 19);
                    birthCounts[i]++;
                    visibleFrames[i] = 1;
                    hiddenFrames[i] = 0;
                } else {
                    assert(p->unusedBubbleVar == before->unusedBubbleVar + 1);
                    assert(p->angleAndDist[0] == before->angleAndDist[0]);
                    assert(p->angleAndDist[1] == before->angleAndDist[1] + 10);
                    assert(p->xPos == (s32) (before->xPos + sins(before->angleAndDist[0]) * 10.0f));
                    assert(p->zPos == (s32) (before->zPos + coss(before->angleAndDist[0]) * 10.0f));
                    assert(p->yPos == before->yPos - (p->angleAndDist[1] / 30) + 50);
                    visibleFrames[i]++;
                }
            } else {
                assert(!p->isAlive);
                assert(p->xPos == before->xPos && p->yPos == before->yPos && p->zPos == before->zPos);
                assert(++hiddenFrames[i] <= (birthCounts[i] ? 3 : 6));
            }
        }
        // Timing adds no random draws to motion or respawns.
        assert(randomDraws == spawned * 3);
    }
    assert(lifeMask == 63 && gapMask == 15 && startMask == 127);
    for (int i = 0; i < 30; i++) {
        assert(birthCounts[i] > 50);
    }
    emitter.oDistanceToMario = 5000;
    bhv_jet_stream_loop();
    envfx_set_max_bubble_particles(ENVFX_JETSTREAM_BUBBLES);
    memcpy(actual, storage, sizeof(storage));
    randomDraws = 0;
    envfx_update_jetstream();
    assert(randomDraws == 0 && !memcmp(actual, storage, sizeof(storage)));
    emitter.oDistanceToMario = 0;
    bhv_jet_stream_loop();
    envfx_set_max_bubble_particles(ENVFX_JETSTREAM_BUBBLES);
    assert(sBubbleActiveCount == 30);
    // A new area starts fresh delays; no previous particle state survives.
    assert(envfx_init_bubble(ENVFX_JETSTREAM_BUBBLES));
    for (int i = 0; i < 30; i++) {
        assert(storage[i].unusedBubbleVar >= -6 && storage[i].unusedBubbleVar <= 0);
        assert(storage[i].animFrame == 0 && storage[i].bubbleY == 0);
    }
    {
        gCurrLevelNum = LEVEL_JRB + 1;
        assert(envfx_init_bubble(ENVFX_JETSTREAM_BUBBLES));
        int count = 60;
        assert(sBubbleParticleCapacity == count && requestedAllocationBytes == count * sizeof(storage[0]));
        emitter = (struct Object){ 4999, 4988, -5221, 2473 };
        bhv_jet_stream_loop();
        envfx_set_max_bubble_particles(ENVFX_JETSTREAM_BUBBLES);
        assert(sBubbleActiveCount == count);
        memcpy(reference, storage, sizeof(reference));
        memcpy(actual, storage, sizeof(actual));
        for (int frame = 0; frame < 2000; frame++) {
            if (frame == 700) { // Moving source and out-of-range recycling retain their rules.
                gEnvFxBubbleConfig[ENVFX_STATE_SRC_X] += 500;
                gEnvFxBubbleConfig[ENVFX_STATE_SRC_Y] -= 300;
            }
            uint32_t before = rng;
            reference_update();
            uint32_t after = rng;
            rng = before;
            gEnvFxBuffer = actual;
            envfx_update_jetstream();
            assert(rng == after && !memcmp(actual, reference, sizeof(actual)));
        }
        emitter.oDistanceToMario = 5000;
        bhv_jet_stream_loop();
        envfx_set_max_bubble_particles(ENVFX_JETSTREAM_BUBBLES);
        assert(sBubbleActiveCount == 0);
        memcpy(reference, actual, sizeof(reference));
        envfx_update_jetstream();
        assert(!memcmp(actual, reference, sizeof(actual)));
        emitter.oDistanceToMario = 0;
        bhv_jet_stream_loop();
        envfx_set_max_bubble_particles(ENVFX_JETSTREAM_BUBBLES);
        assert(sBubbleActiveCount == count);
    }
    // Other environment effects keep their original allocation sizes.
    gCurrLevelNum = LEVEL_JRB;
    gEnvFxBuffer = storage;
    assert(envfx_init_bubble(ENVFX_WHIRLPOOL_BUBBLES));
    assert(sBubbleParticleCapacity == 60);
    assert(envfx_init_bubble(ENVFX_LAVA_BUBBLES));
    assert(sBubbleParticleCapacity == 15);
    assert(envfx_init_bubble(ENVFX_FLOWERS));
    assert(sBubbleParticleCapacity == 30);
    return 0;
}
'''
        with tempfile.TemporaryDirectory(prefix='jrb-jet-') as directory:
            source = Path(directory) / 'jet.c'
            binary = Path(directory) / 'jet'
            source.write_text(fixture)
            subprocess.run([os.environ.get('CC', 'cc'), '-std=gnu17', '-O1', '-fwrapv',
                            '-fsanitize=address,undefined', str(source), '-lm', '-o', str(binary)],
                           check=True)
            subprocess.run([str(binary)], check=True,
                           env={**os.environ, 'ASAN_OPTIONS': 'detect_leaks=0'})


if __name__ == '__main__':
    unittest.main()
