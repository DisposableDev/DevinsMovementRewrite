#include <assert.h>
#include <math.h>
#include <stdint.h>
#include <string.h>

typedef int16_t s16;
typedef float f32;
struct Object {
    int oTimer, oMoveAnglePitch, oAngleVelPitch, intangible;
    f32 oGravity, oFriction, oBuoyancy;
    f32 oPosX, oPosY, oPosZ, oForwardVel, oVelY;
};

static struct Object mushroom, reference;
static struct Object *o = &mushroom;
static int homingCalls, spawnSounds, sparkles;
static struct Object sparkle;
static f32 gGlobalSoundSource[3];
static const int sSparkleBehavior;
#define bhvSparkleShort (&sSparkleBehavior)
#define MODEL_SPARKLES_ANIMATION 1
#define PARTICLE_PRIORITY_FEEDBACK 2
#define SOUND_GENERAL2_1UP_APPEAR 3

static void play_sound(int sound, const f32 *source) {
    assert(sound == SOUND_GENERAL2_1UP_APPEAR && source == gGlobalSoundSource);
    spawnSounds++;
}

static struct Object *try_spawn_particle(struct Object *parent, int model, const void *behavior,
                                         int priority) {
    assert(parent == &mushroom && model == MODEL_SPARKLES_ANIMATION);
    assert(behavior == bhvSparkleShort && priority == PARTICLE_PRIORITY_FEEDBACK);
    sparkles++;
    return &sparkle;
}

static void obj_translate_xyz_random(struct Object *obj, f32 range) {
    assert(obj == &sparkle && range == 90.0f);
}

static void obj_scale_random(struct Object *obj, f32 range, f32 minimum) {
    assert(obj == &sparkle && range == 1.0f && minimum == 0.0f);
}

static float coss(s16 angle) {
    return cosf(angle * 3.14159265358979323846f / 32768.0f);
}

static float sins(s16 angle) {
    return sinf(angle * 3.14159265358979323846f / 32768.0f);
}

static void cur_obj_become_tangible(void) {
    o->intangible = 0;
}

static void pole_1up_move_towards_mario(void) {
    assert(!o->intangible);
    homingCalls++;
    o->oForwardVel = 3.0f;
    o->oVelY = -5.0f;
}

static void cur_obj_move_xz_using_fvel_and_yaw(void) {
    o->oPosZ += o->oForwardVel;
}

#include "one_up_actual.inc"

int main(void) {
    // The encounter's behavior runs common init and starts intangible.
    mushroom.intangible = 1;
    mushroom.oPosY = 200.0f;
    bhv_1up_common_init();
    reference = mushroom;
    for (int frame = 0; frame < 38; frame++) {
        mushroom.oTimer = frame;
        bhv_jrb_simpleflips_one_up_loop();
        assert(homingCalls == 0);
        assert(spawnSounds == 1);
        if (frame <= 17) {
            assert(sparkles == 0);
        }
        assert(mushroom.intangible == (frame < 37));

        // Compare the full spawn flight to the existing 1-Up's loop motion.
        o = &reference;
        reference.oTimer = frame;
        one_up_loop_in_air();
        cur_obj_move_xz_using_fvel_and_yaw();
        reference.oPosY += reference.oVelY;
        o = &mushroom;
        assert(mushroom.oPosY == reference.oPosY);
        assert(mushroom.oPosZ == reference.oPosZ);
        assert(mushroom.oMoveAnglePitch == reference.oMoveAnglePitch);
    }
    assert(sparkles == 8);
    float startY = mushroom.oPosY;
    float startZ = mushroom.oPosZ;
    for (int frame = 38; frame < 58; frame++) {
        mushroom.oTimer = frame;
        bhv_jrb_simpleflips_one_up_loop();
    }
    assert(homingCalls == 20 && spawnSounds == 1 && sparkles == 16);
    assert(fabsf(mushroom.oPosY - (startY - 100.0f)) < 0.001f);
    assert(fabsf(mushroom.oPosZ - (startZ + 60.0f)) < 0.001f);
    return 0;
}
