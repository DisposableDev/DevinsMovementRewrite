"""Native instant-warp/area teardown/slot reuse with controlled world services.

Object positions, ownership and allocator lifetime are production behavior. The
fixture supplies terrain loading, platform-history reset, rendering and audio;
this is not a ROM or emulator test.
"""
from pathlib import Path
import subprocess
import tempfile
import unittest

from regression_support import Source

ROOT = Path(__file__).resolve().parents[1]

DRIVER = r'''
#include <assert.h>
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
typedef int16_t s16;
typedef int32_t s32;
typedef float f32;
typedef int BehaviorScript;
#define UNUSED __attribute__((unused))
#define FALSE 0
#define NUM_OBJ_LISTS 1
#define OBJ_LIST_GENACTOR 0
#define SURFACE_INSTANT_WARP_1B 27
#define INSTANT_WARP_INDEX_START 0
#define INSTANT_WARP_INDEX_STOP 4
#define AREA_FLAG_LOAD 1
#define AREA_FLAG_UNLOAD 0
#define ACTIVE_PARTICLE_NONE 0
#define ACTIVE_FLAG_DEACTIVATED 0
#define GRAPH_RENDER_BILLBOARD 1
#define GRAPH_RENDER_ACTIVE 2
#define GEO_CONTEXT_AREA_UNLOAD 0
#define FLOOR_LOWER_LIMIT_MISC (-10000)
#define SURFACE_DEFAULT 0
#define segmented_to_virtual(p) (p)
#define vec3f_add(dst, src) do { for (s32 i = 0; i < 3; i++) (dst)[i] += (src)[i]; } while (0)
#define oPosVec position[0]
#define oPosX position[0]
#define oPosY position[1]
#define oPosZ position[2]
#define oHomeVec home[0]
struct ObjectNode {
    struct ObjectNode *next, *prev;
    struct {
        s32 activeAreaIndex, areaIndex;
        struct { s32 flags; } node;
        f32 pos[3], cameraToObject[3];
        void *throwMatrix;
    } gfx;
};
struct Surface { s32 type; };
struct Object {
    struct ObjectNode header;
    f32 position[3], home[3], velocity[3], oFloorHeight;
    s32 oActiveParticleFlags, activeFlags, metalContactCooldown, metalContactPendingDefeat;
    s32 bowserArenaBooTimer, bowserArenaSpinyAge, bowserThrowAirFrames, bowserArenaCoinRolled;
    s32 bowserArenaFade, bowserArenaCollapse, bowserArenaFireMask, bowserArenaStage;
    s32 bowserArenaFallAllowed, oFloorType, oMoveFlags, resetCount, yaw;
    struct Object *parentObj, *prevObj, *platform, *bowserArenaOwner;
    struct Surface *oFloor;
    const BehaviorScript *behavior;
};
struct Camera { s16 yaw; };
struct InstantWarp { s32 id, area, displacement[3]; };
struct GraphNode { struct ObjectNode node; };
struct Area {
    s32 flags, index;
    struct InstantWarp *instantWarps;
    struct Camera *camera;
    struct GraphNode *graphNode;
};
struct MarioBodyState { f32 heldObjLastPosition[3]; };
struct MarioState {
    f32 pos[3], velocity[3], forwardVel;
    s32 action, yaw;
    struct Object *marioObj, *heldObj, *riddenObj;
    struct Surface *floor;
    struct Area *area;
    struct MarioBodyState *marioBodyState;
};
static const BehaviorScript bhvKoopaShell[1], bhvKoopaShellUnderwater[1];
static const BehaviorScript bhvChuckya[1], bhvChuckyaAnchorMario[1], bhvBobomb[1], sOther[1];
static struct ObjectNode gObjectListArray[1], *gObjectLists, gFreeObjectList, gObjParentGraphNode;
static struct Object sMarioObject, sOwned, sAnchor, sOtherObject, sSourceParent;
static struct Object *gMarioObject = &sMarioObject;
static struct MarioState sMario, *gMarioState = &sMario;
static struct MarioBodyState sBody;
static struct Camera sCameras[3];
static struct GraphNode sGraph;
static struct Area sAreas[3], *gCurrentArea;
static struct InstantWarp sWarps[4];
static struct Surface sFloor = { SURFACE_INSTANT_WARP_1B + 2 };
static s32 gCurrAreaIndex, sReused, sFreed, sShellSoundStops, sMusicActive;
static struct { s32 areaIndex; } sSpawn, *gMarioSpawnInfo = &sSpawn;
static struct { s32 isActive; } gWarpTransition;
static void no_object_service(struct Object *obj) { (void) obj; }
#define mario_forget_attack_object no_object_service
#define mario_forget_pickup_object no_object_service
#define player_stats_object_reset no_object_service
#define persistent_surfaces_forget no_object_service
#define object_platform_motion_freed no_object_service
#define particle_budget_allocated no_object_service
#define object_platform_motion_allocated no_object_service
static void object_platform_motion_reset(struct Object *obj) { obj->resetCount++; }
static void particle_budget_freed(struct Object *obj) { (void) obj; sFreed++; }
static void player_stats_reset_movement(void) {}
static void yoshi_credit_sign_area_exit(void) {}
static void invalidate_static_wall_height_index(void) {}
static void geo_call_global_function_nodes(struct ObjectNode *node, s32 context) {
    (void) node; (void) context;
}
static void warp_camera(s32 x, s32 y, s32 z) { (void) x; (void) y; (void) z; }
static void stop_sounds_from_source(f32 *position) {
    if (position == sOwned.header.gfx.cameraToObject) sShellSoundStops++;
}
static void geo_remove_child(void *node) { (void) node; }
static void geo_add_child(void *parent, void *node) { (void) parent; (void) node; }
static void load_area(s32 index);
#include "instant_warp_sources.inc"

static void load_area(s32 index) {
    gCurrAreaIndex = index;
    gCurrentArea = &sAreas[index];
    // Destination loading really allocates from the newly freed source slots.
    struct Object *replacement = try_allocate_object(&gObjectListArray[0], &gFreeObjectList);
    assert(replacement != NULL && replacement != &sOwned && replacement != &sAnchor);
    replacement->activeFlags = 1;
    replacement->behavior = sOther;
    sReused++;
}
static void append(struct Object *obj, const BehaviorScript *behavior) {
    *obj = (struct Object) { 0 };
    obj->activeFlags = 1;
    obj->behavior = behavior;
    obj->header.gfx.areaIndex = obj->header.gfx.activeAreaIndex = 2;
    obj->header.prev = gObjectListArray[0].prev;
    obj->header.next = &gObjectListArray[0];
    gObjectListArray[0].prev->next = &obj->header;
    gObjectListArray[0].prev = &obj->header;
}
static void setup(const BehaviorScript *behavior, s32 destination) {
    memset(&sMarioObject, 0, sizeof(sMarioObject));
    sMarioObject.header.gfx.activeAreaIndex = -1;
    gObjectLists = gObjectListArray;
    gObjectListArray[0].next = gObjectListArray[0].prev = &gObjectListArray[0];
    gFreeObjectList.next = NULL;
    append(&sOwned, behavior);
    append(&sOtherObject, sOther);
    append(&sSourceParent, sOther);
    sOwned.parentObj = gMarioObject;
    sOwned.oFloor = &sFloor;
    sOwned.oFloorHeight = 123;
    sOwned.oFloorType = 5;
    sOwned.oMoveFlags = 31;
    sOwned.platform = &sSourceParent;
    sOwned.yaw = 222;
    for (s32 i = 0; i < 3; i++) {
        sOwned.position[i] = 100 + i;
        sOwned.home[i] = 200 + i;
        sOwned.header.gfx.pos[i] = 300 + i;
        sOwned.velocity[i] = 400 + i;
        sBody.heldObjLastPosition[i] = 500 + i;
    }
    sWarps[2] = (struct InstantWarp) { 1, destination, { 8192, 64, -32 } };
    sAreas[2] = (struct Area) { AREA_FLAG_LOAD, 2, sWarps, &sCameras[2], &sGraph };
    sAreas[1] = (struct Area) { AREA_FLAG_UNLOAD, 1, NULL, &sCameras[1], &sGraph };
    gCurrentArea = &sAreas[2];
    gCurrAreaIndex = 2;
    sCameras[2].yaw = 321;
    sCameras[1].yaw = -123;
    sMario = (struct MarioState) {
        .marioObj = gMarioObject, .floor = &sFloor, .area = gCurrentArea,
        .marioBodyState = &sBody, .pos = { 10, 20, 30 }, .velocity = { 3, 4, 5 },
        .forwardVel = 37, .action = 101, .yaw = 1234
    };
    sFreed = sReused = sShellSoundStops = 0;
    sMusicActive = 1;
}
static void check_positions(struct Object *obj, s32 bodyBase, s32 homeBase, s32 gfxBase) {
    for (s32 i = 0; i < 3; i++) {
        assert(obj->position[i] == bodyBase + i + sWarps[2].displacement[i]);
        assert(obj->home[i] == homeBase + i + sWarps[2].displacement[i]);
        assert(obj->header.gfx.pos[i] == gfxBase + i + sWarps[2].displacement[i]);
    }
}
static void check_player(void) {
    assert(sMario.action == 101 && sMario.yaw == 1234 && sMario.forwardVel == 37);
    assert(sMario.velocity[0] == 3 && sMario.velocity[1] == 4 && sMario.velocity[2] == 5);
    assert(sMario.area == gCurrentArea && gCurrentArea->camera->yaw == 321);
    assert(sShellSoundStops == 0 && sMusicActive == 1);
    assert(sOwned.velocity[0] == 400 && sOwned.velocity[1] == 401 && sOwned.velocity[2] == 402);
    assert(sOwned.yaw == 222 && sOwned.activeFlags == 1);
}
static void check_cross_area(void) {
    assert(gCurrAreaIndex == 1 && gCurrentArea->flags == AREA_FLAG_LOAD);
    assert(sSpawn.areaIndex == 1 && sOwned.header.gfx.areaIndex == 1);
    assert(sOwned.header.gfx.activeAreaIndex == 1 && sReused == 1);
    assert(sOwned.oFloor == NULL && sOwned.platform == NULL && sOwned.resetCount == 1);
    assert(sOwned.oFloorHeight == FLOOR_LOWER_LIMIT_MISC && sOwned.oFloorType == SURFACE_DEFAULT);
    assert(sOwned.oMoveFlags == 0 && sFreed == 2);
}
int main(void) {
    // Both swimming and ridden variants survive real source teardown and slot reuse.
    const BehaviorScript *shells[] = { bhvKoopaShellUnderwater, bhvKoopaShellUnderwater, bhvKoopaShell };
    for (s32 kind = 0; kind < 3; kind++) {
        setup(shells[kind], 1);
        if (kind == 0) sMario.heldObj = &sOwned;
        else { sMario.riddenObj = &sOwned; sOwned.parentObj = &sSourceParent; }
        check_instant_warp();
        check_positions(&sOwned, 100, 200, 300);
        check_cross_area();
        check_player();
        assert((kind == 0 ? sMario.heldObj : sMario.riddenObj) == &sOwned);
        assert(sOwned.parentObj == gMarioObject);
        for (s32 i = 0; i < 3; i++)
            assert(sBody.heldObjLastPosition[i] == 500 + i + (kind == 0 ? sWarps[2].displacement[i] : 0));
    }
    // Duplicate ownership must not double-translate an object.
    setup(bhvKoopaShellUnderwater, 1);
    sMario.heldObj = sMario.riddenObj = &sOwned;
    check_instant_warp();
    check_positions(&sOwned, 100, 200, 300);
    check_cross_area();
    assert(sMario.heldObj == &sOwned && sMario.riddenObj == &sOwned);

    // A seamless warp within one area neither frees support nor changes the box parent.
    setup(bhvKoopaShell, 2);
    sMario.riddenObj = &sOwned;
    sOwned.parentObj = &sSourceParent;
    check_instant_warp();
    check_positions(&sOwned, 100, 200, 300);
    check_player();
    assert(sFreed == 0 && sReused == 0 && sOwned.resetCount == 0);
    assert(sOwned.parentObj == &sSourceParent && sOwned.oFloor == &sFloor);
    assert(sOwned.platform == &sSourceParent && sOwned.header.gfx.activeAreaIndex == 2);

    setup(bhvBobomb, 1);
    sMario.heldObj = &sOwned;
    check_instant_warp();
    check_positions(&sOwned, 100, 200, 300);
    check_cross_area();
    assert(sOwned.parentObj == gMarioObject && sMario.heldObj == &sOwned);

    // Ordinary carried enemies retain their native home and the Chuckya grab anchor.
    setup(bhvChuckya, 1);
    sMario.heldObj = &sOwned;
    append(&sAnchor, bhvChuckyaAnchorMario);
    sAnchor.parentObj = &sOwned;
    sAnchor.oFloor = &sFloor;
    sOtherObject.parentObj = &sOwned; // A particle/other child is not its grab anchor.
    check_instant_warp();
    check_positions(&sOwned, 100, 200, 300);
    assert(sAnchor.parentObj == &sOwned && sAnchor.activeFlags == 1);
    assert(sAnchor.header.gfx.activeAreaIndex == 1 && sAnchor.header.gfx.areaIndex == 1);
    assert(sAnchor.oFloor == NULL && sAnchor.resetCount == 1);
    for (s32 i = 0; i < 3; i++) assert(sAnchor.position[i] == sWarps[2].displacement[i]);
    check_cross_area();

    // Unowned objects remain source-area objects and are released normally.
    setup(sOther, 1);
    sOwned.header.gfx.activeAreaIndex = -1;
    check_instant_warp();
    assert(sOwned.position[0] == 100 && sOwned.home[0] == 200);
    assert(sOwned.header.gfx.activeAreaIndex == -1 && sFreed == 2 && sReused == 1);
    assert(sMario.heldObj == NULL && sMario.riddenObj == NULL);
    puts("PASS instant warp ownership, teardown, reuse and preservation");
}
'''


class InstantWarpOwnershipTests(unittest.TestCase):
    def test_owned_objects_survive_native_area_teardown_and_slot_reuse(self):
        with tempfile.TemporaryDirectory(prefix="instant-warp-ownership-") as directory:
            folder = Path(directory)
            functions = []
            for path, names in (
                ("src/game/spawn_object.c", ("deallocate_object", "try_allocate_object", "unload_object")),
                ("src/game/object_list_processor.c", ("unload_objects_from_area",)),
                ("src/game/area.c", ("unload_area", "change_area")),
                ("src/game/level_update.c", ("move_owned_object_through_instant_warp",
                                           "move_owned_objects_through_instant_warp")),
            ):
                functions.extend(Source(ROOT / path).function(name) for name in names)
            # This function's nested configuration branches have alternative opening braces.
            functions.append(Source(ROOT / "src/game/level_update.c").region(
                r"^void check_instant_warp", r"^s16 music_unchanged_through_warp"))
            (folder / "instant_warp_sources.inc").write_text("\n".join(functions))
            driver = folder / "driver.c"
            driver.write_text(DRIVER)
            executable = folder / "check"
            result = subprocess.run(
                ["cc", "-std=c11", "-Wall", "-Wextra", "-Werror", "-fsanitize=undefined",
                 str(driver), "-I", str(folder), "-o", str(executable)],
                capture_output=True, text=True, timeout=30)
            self.assertEqual(result.returncode, 0, result.stdout + result.stderr)
            result = subprocess.run([str(executable)], capture_output=True, text=True, timeout=10)
            self.assertEqual(result.returncode, 0, result.stdout + result.stderr)


if __name__ == "__main__":
    unittest.main()
