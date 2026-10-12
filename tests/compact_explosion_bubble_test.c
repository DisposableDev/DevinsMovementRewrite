#include <stdint.h>
#include <stdio.h>
#include <math.h>

typedef int16_t s16;
typedef int32_t s32;
typedef uint8_t u8;
typedef float f32;
typedef f32 Vec3f[3];
#define TRUE 1
#define FALSE 0
#define FLOOR_LOWER_LIMIT_MISC -11000
#define MODEL_SMALL_WATER_SPLASH 1
#define PARTICLE_PRIORITY_DROPLET 5
#define COMPACT_EXPLOSION_BUBBLE_CAPACITY 128
#define CHECK(cond)                                                                                    \
    do {                                                                                               \
        if (!(cond)) {                                                                                 \
            fprintf(stderr, "FAIL %s:%d: %s\n", __func__, __LINE__, #cond);                            \
            return 1;                                                                                  \
        }                                                                                              \
    } while (0)

struct Object {
    struct {
        struct {
            s16 areaIndex;
        } gfx;
    } header;
    f32 oPosX, oPosY, oPosZ;
};
struct MarioState {
    f32 waterLevel;
};
struct Area {
    s16 index;
};
static struct Object mario;
static struct Object *gMarioObject = &mario;
static struct MarioState marioState;
static struct MarioState *gMarioState = &marioState;
static struct Area area;
static struct Area *gCurrentArea = &area;
static const s32 bhvObjectWaterSplash[] = { 1 };
static struct Object splashes[COMPACT_EXPLOSION_BUBBLE_CAPACITY];
static s32 splashCount;

static f32 random_float(void) {
    return 0.25f;
}
static f32 sins(s16 angle) {
    return sinf((f32) angle * 3.14159265358979323846f / 32768.0f);
}
static void vec3_copy(Vec3f dest, Vec3f src) {
    for (s32 i = 0; i < 3; i++) {
        dest[i] = src[i];
    }
}
static void vec3f_set(Vec3f dest, f32 x, f32 y, f32 z) {
    dest[0] = x;
    dest[1] = y;
    dest[2] = z;
}
static struct Object *try_spawn_particle(struct Object *parent, s32 model, const s32 *behavior,
                                         s32 priority) {
    if (parent != gMarioObject || model != MODEL_SMALL_WATER_SPLASH || behavior != bhvObjectWaterSplash
        || priority != PARTICLE_PRIORITY_DROPLET || splashCount == COMPACT_EXPLOSION_BUBBLE_CAPACITY) {
        return NULL;
    }
    return &splashes[splashCount++];
}

#include "compact_source.inc"

static void reset_fixture(void) {
    compact_explosion_bubbles_reset();
    splashCount = 0;
    area.index = 1;
    marioState.waterLevel = 1000.0f;
}

static int motion_and_surface_splash(void) {
    Vec3f pos, scale;
    s16 areaIndex;
    struct Object emitter = { .header.gfx.areaIndex = 1, .oPosX = 10, .oPosY = 100, .oPosZ = 20 };
    reset_fixture();
    CHECK(compact_explosion_bubble_spawn(&emitter));
    CHECK(compact_explosion_bubbles_count() == 1);
    CHECK(compact_explosion_bubble_view(0, pos, scale, &areaIndex));
    CHECK(areaIndex == 1 && pos[0] == -16 && pos[1] == 75 && pos[2] == -6);
    CHECK(scale[0] == 2 && scale[1] == 2 && scale[2] == 1);
    compact_explosion_bubbles_update(); // Script's initial one-update delay.
    CHECK(compact_explosion_bubble_view(0, pos, scale, &areaIndex) && pos[1] == 75);
    compact_explosion_bubbles_update();
    CHECK(compact_explosion_bubble_view(0, pos, scale, &areaIndex));
    CHECK(pos[0] == -17 && pos[1] == 80 && pos[2] == -7);
    marioState.waterLevel = 70;
    compact_explosion_bubbles_update();
    CHECK(compact_explosion_bubbles_count() == 0 && splashCount == 1);
    CHECK(splashes[0].oPosX == -18 && splashes[0].oPosY == 85 && splashes[0].oPosZ == -8);
    return 0;
}

static int capacity_reuse_and_area(void) {
    struct Object emitter = { .header.gfx.areaIndex = 1 };
    reset_fixture();
    for (s32 i = 0; i < COMPACT_EXPLOSION_BUBBLE_CAPACITY; i++) {
        CHECK(compact_explosion_bubble_spawn(&emitter));
    }
    CHECK(!compact_explosion_bubble_spawn(&emitter));
    CHECK(compact_explosion_bubbles_count() == COMPACT_EXPLOSION_BUBBLE_CAPACITY);
    area.index = 2;
    compact_explosion_bubbles_update();
    CHECK(compact_explosion_bubbles_count() == 0);
    area.index = 1;
    CHECK(compact_explosion_bubble_spawn(&emitter));
    CHECK(compact_explosion_bubbles_count() == 1);
    compact_explosion_bubbles_reset();
    CHECK(compact_explosion_bubbles_count() == 0);
    return 0;
}

static int natural_lifetime(void) {
    struct Object emitter = { .header.gfx.areaIndex = 1 };
    reset_fixture();
    CHECK(compact_explosion_bubble_spawn(&emitter));
    for (s32 frame = 0; frame < 40; frame++) {
        compact_explosion_bubbles_update();
    }
    CHECK(compact_explosion_bubbles_count() == 0 && splashCount == 0);
    return 0;
}

int main(void) {
    if (motion_and_surface_splash()) {
        return 1;
    }
    puts("PASS compact.motion_and_surface_splash");
    if (capacity_reuse_and_area()) {
        return 1;
    }
    puts("PASS compact.capacity_reuse_and_area");
    if (natural_lifetime()) {
        return 1;
    }
    puts("PASS compact.natural_lifetime");
    return 0;
}
