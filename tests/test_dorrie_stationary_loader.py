"""Verify specialized mesh loading preserves eligibility, ownership and phase order."""
from pathlib import Path
import os
import subprocess
import tempfile
import unittest

from regression_support import Source

ROOT = Path(__file__).resolve().parents[1]


class DorrieStationaryLoaderTests(unittest.TestCase):
    def test_loading_gates_and_phase_order(self):
        source = Source(ROOT / "src/engine/surface_load.c")
        loader = "\n".join(source.function(name) for name in (
            "bowser_platform_only_needs_top", "load_bowser_platform_top", "load_object_collision_model"))
        code = r'''
#include <assert.h>
#include <stdint.h>
#include <math.h>
#include <string.h>
typedef int32_t s32;
typedef uint32_t u32;
typedef int16_t TerrainData;
typedef int8_t RoomData;
typedef float f32;
typedef f32 Vec3f[3];
typedef f32 Mat4[4][4];
#define ACT_FLAG_AIR 0x800
#define HELD_FREE 0
#define BOWSER_ACT_THROWN 2
#define BOWSER_ACT_HIT_MINE 3
#define BOWSER_ACT_JUMP_ONTO_STAGE 4
#define TRUE 1
#define FALSE 0
#define ALL_SURFACES_HAVE_FORCE
#define AUTO_COLLISION_DISTANCE
#define COLLISION_PROFILING 0
#define BOWSER_STATIONARY_SURFACES 1
#define BOWSER_BITS_PLAT_ACT_CHECK 1
#define TIME_STOP_ACTIVE 1
#define OBJ_FLAG_DONT_CALC_COLL_DIST 1
#define ACTIVE_FLAG_IN_DIFFERENT_ROOM 1
#define GRAPH_RENDER_ACTIVE 1
#define TERRAIN_LOAD_CONTINUE 255
#define O_POS_INDEX 0
#define O_FACE_ANGLE_INDEX 1
#define sqr(x) ((x) * (x))
#define absf(x) fabsf(x)
#define PUPPYPRINT_GET_COLLISION_SNAPSHOT() u32 first = 0
#define profiler_collision_update(x) ((void)(x))
#define COND_BIT(condition,field,bit) do { \
    if (condition) { (field) |= (bit); } else { (field) &= ~(bit); } \
} while (0)
struct Object {
    union { Vec3f oPosVec; struct { f32 oPosX, oPosY, oPosZ; }; };
    s32 oFaceAnglePitch, oFaceAngleYaw, oFaceAngleRoll;
    f32 oCollisionDistance, oDrawingDistance, oDistanceToMario;
    u32 oFlags, activeFlags, oAction, oHeldState;
    struct Object *oBitsPlatformBowser;
    const void *behavior;
    TerrainData *collisionData;
    Mat4 transform;
    struct { struct {
        struct { u32 flags; } node;
        Mat4 *throwMatrix;
        Vec3f scale;
    } gfx; } header;
};
struct Camera { Vec3f pos, focus; };
static struct Camera *gCamera;
struct MarioState { Vec3f vel; u32 action; } marioState;
static struct Object provider, mario;
static struct Object *gCurrentObject = &provider, *gMarioObject = &mario;
static struct MarioState *gMarioState = &marioState;
#define o gCurrentObject
static const u32 bhvDorrie[] = {1}, bhvTTCSpinner[] = {2}, bhvDddWarp[] = {3};
static const u32 bhvInSunkenShip[] = {4}, bhvInSunkenShip2[] = {5}, bhvInSunkenShip3[] = {6};
static const u32 bhvFallingBowserPlatform[] = {7};
static u32 gTimeStopState;
static TerrainData sVertexData[600], collisionData[] = {0, 78, 0, TERRAIN_LOAD_CONTINUE};
static int needed, cacheResult, refreshes, builds, transforms, publications;
static int bowserCalls, forgetCalls;
static int shipCalls, stationaryCalls, generalCalls, visibilityQueries, distanceCalculations;
static void *segmented_to_virtual(const void *pointer) { return (void *) pointer; }
static void vec3_diff(Vec3f result, const void *aa, const void *bb) {
    const f32 *a = aa, *b = bb;
    for (int i = 0; i < 3; i++) { result[i] = a[i] - b[i]; }
}
static void get_optimal_coll_dist(struct Object *object) {
    distanceCalculations++;
    object->oCollisionDistance = 1112;
    object->oFlags |= OBJ_FLAG_DONT_CALC_COLL_DIST;
}
static int object_platform_motion_needs_collision(struct Object *object) {
    assert(object == &provider);
    return needed;
}
static void object_platform_motion_refresh_transform(struct Object *object) {
    assert(object == &provider);
    assert(publications == 0);
    object->transform[3][1] = 27;
    refreshes++;
}
#if DORRIE_STATIONARY_SURFACES || PERSISTENT_DYNAMIC_SURFACES || JRB_STATIONARY_SURFACES || BOWSER_STATIONARY_SURFACES
static void obj_build_transform_from_pos_and_angle(struct Object *object, int pos, int angle) {
    assert(object == &provider && refreshes == 1 && pos == 0 && angle == 1);
    builds++;
}
#endif
#if DORRIE_STATIONARY_SURFACES
static int persistent_surfaces_publish_dorrie(struct Object *object, const TerrainData *data,
                                             Mat4 transform, Vec3f scale) {
    assert(object == &provider && object->behavior == bhvDorrie && data == collisionData);
    assert(refreshes == 1 && publications == 0 && transform[3][1] == 27);
    assert(scale[0] == 1 && object->header.gfx.throwMatrix == &object->transform);
    stationaryCalls++;
    return cacheResult;
}
#endif
static int persistent_surfaces_publish_jrb_ship(struct Object *object, const TerrainData *data,
                                               Mat4 transform, Vec3f scale) {
    assert(object == &provider && data == collisionData && refreshes == 1);
    assert(object->behavior == bhvInSunkenShip || object->behavior == bhvInSunkenShip2);
    assert(transform[3][1] == 27 && scale[0] == 1 && publications == 0);
    shipCalls++;
    return cacheResult;
}
static int persistent_surfaces_publish_bowser_platform(struct Object *object, const TerrainData *data,
                                                      Mat4 transform, Vec3f scale) {
    assert(object == &provider && data == collisionData && refreshes == 1);
    assert(object->oAction == BOWSER_BITS_PLAT_ACT_CHECK && publications == 0);
    assert(transform[3][1] == 27 && scale[0] == 1);
    bowserCalls++;
    return cacheResult;
}
static void persistent_surfaces_forget(struct Object *object) {
    assert(object == &provider && publications == 0);
    forgetCalls++;
}
#if PERSISTENT_DYNAMIC_SURFACES
static void mtxf_scale_vec3f(Mat4 target, Mat4 source, Vec3f scale) {
    assert(scale[0] == 1);
    memcpy(target, source, sizeof(Mat4));
}
static int persistent_surfaces_publish(struct Object *object, const TerrainData *data,
                                      Mat4 transform, RoomData room) {
    assert(object == &provider && refreshes == 1 && publications == 0);
    assert(data == collisionData && transform[3][1] == 27);
    assert(room == (object->behavior == bhvDddWarp ? 5 : 0));
    generalCalls++;
    // The real shared publisher rejects a second publication in this frame.
    if (cacheResult && shipCalls != 0) {
        return FALSE;
    }
    return cacheResult;
}
#endif
static void transform_object_vertices(TerrainData **data, TerrainData *vertices) {
    assert(vertices == sVertexData && refreshes == 1 && publications == 0);
    transforms++;
    *data = collisionData + 2;
}
static void load_object_surfaces(TerrainData **data, TerrainData *vertices, u32 dynamic) {
    assert(vertices == sVertexData && dynamic == TRUE);
    *data = collisionData + 3;
}
static void object_platform_motion_publish(struct Object *object) {
    assert(object == &provider && refreshes == 1);
    publications++;
}
static f32 dist_between_objects(struct Object *a, struct Object *b) {
    return sqrtf(sqr(a->oPosX - b->oPosX) + sqr(a->oPosY - b->oPosY)
                 + sqr(a->oPosZ - b->oPosZ));
}
static f32 cur_obj_drawing_distance(f32 distance) {
    (void) distance;
    visibilityQueries++;
    return o->oDrawingDistance;
}
/* LOADER */
static void reset(void) {
    provider = (struct Object) {0};
    mario = (struct Object) {0};
    provider.behavior = bhvDorrie;
    provider.collisionData = collisionData;
    provider.oCollisionDistance = 1112;
    provider.oDrawingDistance = 4000;
    provider.oDistanceToMario = 19000;
    provider.oFlags = OBJ_FLAG_DONT_CALC_COLL_DIST;
    provider.header.gfx.scale[0] = 1;
    provider.header.gfx.throwMatrix = &provider.transform;
    refreshes = builds = transforms = publications = shipCalls = stationaryCalls = generalCalls = 0;
    needed = visibilityQueries = distanceCalculations = bowserCalls = forgetCalls = 0;
    cacheResult = TRUE;
    gTimeStopState = 0;
}
static void check_skipped(void) {
    load_object_collision_model();
    assert(!refreshes && !transforms && !publications && !shipCalls && !stationaryCalls && !generalCalls);
    assert(visibilityQueries == 1);
}
static void check_bowser_top_boundaries(void) {
    struct Object bowser = {.oPosY = 307};
    struct Camera camera = {.pos = {0, 307, 0}, .focus = {0, 307, 0}};
    reset();
    gCamera = &camera;
    provider.behavior = bhvFallingBowserPlatform;
    provider.oAction = BOWSER_BITS_PLAT_ACT_CHECK;
    for (int axis = 0; axis < 3; axis++) { provider.header.gfx.scale[axis] = 1; }
    mario.oPosY = 307;
    provider.oBitsPlatformBowser = &bowser;
    assert(bowser_platform_only_needs_top());
    mario.oPosY = 306.99f; assert(!bowser_platform_only_needs_top()); mario.oPosY = 307;
    marioState.action = ACT_FLAG_AIR; assert(!bowser_platform_only_needs_top()); marioState.action = 0;
    marioState.vel[1] = -0.01f; assert(!bowser_platform_only_needs_top()); marioState.vel[1] = 0;
    camera.pos[1] = 306.99f; assert(!bowser_platform_only_needs_top()); camera.pos[1] = 307;
    camera.focus[1] = 306.99f; assert(!bowser_platform_only_needs_top()); camera.focus[1] = 307;
    for (int axis = 0; axis < 3; axis++) {
        provider.oPosVec[axis] = 0.01f; assert(!bowser_platform_only_needs_top()); provider.oPosVec[axis] = 0;
        provider.header.gfx.scale[axis] = 1.01f; assert(!bowser_platform_only_needs_top());
        provider.header.gfx.scale[axis] = 1;
    }
    provider.oFaceAnglePitch = 1; assert(!bowser_platform_only_needs_top()); provider.oFaceAnglePitch = 0;
    provider.oFaceAngleYaw = 1; assert(!bowser_platform_only_needs_top()); provider.oFaceAngleYaw = 0;
    provider.oFaceAngleRoll = 1; assert(!bowser_platform_only_needs_top()); provider.oFaceAngleRoll = 0;
    bowser.oPosY = 306.99f; assert(!bowser_platform_only_needs_top()); bowser.oPosY = 307;
    bowser.oHeldState = 1; assert(!bowser_platform_only_needs_top()); bowser.oHeldState = HELD_FREE;
    const u32 flights[] = {BOWSER_ACT_THROWN, BOWSER_ACT_HIT_MINE, BOWSER_ACT_JUMP_ONTO_STAGE};
    for (int i = 0; i < 3; i++) {
        bowser.oAction = flights[i]; assert(!bowser_platform_only_needs_top());
    }
    bowser.oAction = 0;
    provider.oBitsPlatformBowser = NULL; assert(!bowser_platform_only_needs_top());
    provider.oBitsPlatformBowser = &bowser;
    provider.oAction++; assert(!bowser_platform_only_needs_top());
    provider.oAction = BOWSER_BITS_PLAT_ACT_CHECK;
    provider.behavior = bhvDorrie; assert(!bowser_platform_only_needs_top());
    gCamera = NULL;
}
int main(void) {
    check_bowser_top_boundaries();
    for (int action = 0; action < 3; action++) {
        for (int cached = 0; cached < 2; cached++) {
            reset();
            provider.behavior = bhvFallingBowserPlatform;
            provider.oAction = action;
            collisionData[1] = 8;
            cacheResult = cached;
            provider.header.gfx.throwMatrix = 0;
            load_object_collision_model();
            assert(bowserCalls == (action == BOWSER_BITS_PLAT_ACT_CHECK));
            assert(forgetCalls == (action != BOWSER_BITS_PLAT_ACT_CHECK));
            assert(transforms == !(cached && action == BOWSER_BITS_PLAT_ACT_CHECK));
            assert(publications == 1 && visibilityQueries == 1);
        }
    }
    reset();
    provider.behavior = bhvFallingBowserPlatform;
    provider.oAction = BOWSER_BITS_PLAT_ACT_CHECK;
    gTimeStopState = TIME_STOP_ACTIVE;
    check_skipped();
    assert(!bowserCalls && !forgetCalls);
    collisionData[1] = 78;
    reset();
    load_object_collision_model();
    assert(refreshes == 1 && publications == 1 && visibilityQueries == 1);
    assert(stationaryCalls == DORRIE_STATIONARY_SURFACES);
    assert(generalCalls == (!DORRIE_STATIONARY_SURFACES && PERSISTENT_DYNAMIC_SURFACES));
    assert(transforms == !(DORRIE_STATIONARY_SURFACES || PERSISTENT_DYNAMIC_SURFACES));
    assert(provider.header.gfx.node.flags & GRAPH_RENDER_ACTIVE);
    reset();
    cacheResult = FALSE;
    provider.header.gfx.throwMatrix = 0;
    load_object_collision_model();
    assert(transforms == 1 && publications == 1);
    assert(builds == (DORRIE_STATIONARY_SURFACES || PERSISTENT_DYNAMIC_SURFACES));
    assert(generalCalls == (!DORRIE_STATIONARY_SURFACES && PERSISTENT_DYNAMIC_SURFACES));
    reset();
    provider.behavior = bhvDddWarp;
    load_object_collision_model();
    assert(!stationaryCalls && generalCalls == PERSISTENT_DYNAMIC_SURFACES);
    assert(transforms == !PERSISTENT_DYNAMIC_SURFACES && publications == 1);
    reset();
    gTimeStopState = TIME_STOP_ACTIVE;
    check_skipped();
    reset();
    provider.activeFlags = ACTIVE_FLAG_IN_DIFFERENT_ROOM;
    check_skipped();
    reset();
    mario.oPosX = 2000;
    check_skipped();
    reset();
    mario.oPosY = 4000;
    check_skipped();
    reset();
    mario.oPosY = -1112;
    check_skipped();
    reset();
    mario.oPosX = 2000;
    needed = 1;
    load_object_collision_model();
    assert(refreshes == 1 && publications == 1 && visibilityQueries == 1);
    reset();
    provider.oFlags = 0;
    provider.oCollisionDistance = provider.oDrawingDistance = 1;
    mario.oPosX = 2000;
    check_skipped();
    assert(distanceCalculations == 1 && provider.oDrawingDistance == 1112);
    assert(!(provider.header.gfx.node.flags & GRAPH_RENDER_ACTIVE));
    const void *shipBehaviors[] = {bhvInSunkenShip, bhvInSunkenShip2};
    for (int i = 0; i < 2; i++) {
        // Both the small mesh and the 79-vertex ship must publish only once,
        // even when the general persistent-mesh setting is enabled as well.
        const int vertexCounts[] = {10, 79};
        for (int mesh = 0; mesh < 2; mesh++) {
            collisionData[1] = vertexCounts[mesh];
            reset(); provider.behavior = shipBehaviors[i];
            load_object_collision_model();
            assert(shipCalls == 1 && !transforms && publications == 1 && !generalCalls);
            reset(); provider.behavior = shipBehaviors[i]; cacheResult = FALSE;
            load_object_collision_model();
            assert(shipCalls == 1 && transforms == 1 && publications == 1);
            assert(generalCalls == (PERSISTENT_DYNAMIC_SURFACES && vertexCounts[mesh] >= 64));
        }
        collisionData[1] = 10;
        reset(); provider.behavior = shipBehaviors[i]; gTimeStopState = TIME_STOP_ACTIVE;
        check_skipped();
        reset(); provider.behavior = shipBehaviors[i];
        provider.activeFlags = ACTIVE_FLAG_IN_DIFFERENT_ROOM; check_skipped();
        reset(); provider.behavior = shipBehaviors[i]; mario.oPosX = 2000;
        check_skipped();
    }
    reset(); provider.behavior = bhvInSunkenShip3;
    load_object_collision_model();
    assert(!shipCalls && transforms == 1 && publications == 1);
    return 0;
}
'''.replace("/* LOADER */", loader)
        with tempfile.TemporaryDirectory(prefix="dorrie-loader-") as directory:
            temporary = Path(directory)
            source = temporary / "test.c"
            source.write_text(code)
            for stationary, general in [(1, 0), (1, 1), (0, 1), (0, 0)]:
                with self.subTest(stationary=stationary, general=general):
                    binary = temporary / f"test-{stationary}-{general}"
                    subprocess.run([
                        os.environ.get("CC", "cc"), "-std=gnu17", "-O1", "-Wall", "-Wextra",
                        "-Werror", "-fsanitize=address,undefined", "-fno-omit-frame-pointer",
                        "-no-pie", f"-DDORRIE_STATIONARY_SURFACES={stationary}",
                        f"-DPERSISTENT_DYNAMIC_SURFACES={general}", "-DJRB_STATIONARY_SURFACES=1", str(source), "-lm", "-o",
                        str(binary),
                    ], check=True)
                    subprocess.run([str(binary)], check=True,
                                   env={**os.environ, "ASAN_OPTIONS": "detect_leaks=0"})


if __name__ == "__main__":
    unittest.main()
