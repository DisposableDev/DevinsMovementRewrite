"""Native pending-pickup lifetime and valid/interrupted pickup preservation."""
from pathlib import Path
import os
import subprocess
import tempfile
import unittest

from regression_support import Source

ROOT = Path(__file__).resolve().parents[1]

PRELUDE = r'''
#include <assert.h>
#include <stddef.h>
#include <string.h>
typedef int s32;
typedef unsigned int u32;
typedef int BehaviorScript;
#define TRUE 1
#define FALSE 0
#define ACTIVE_FLAG_DEACTIVATED 0
#define ACTIVE_FLAG_ACTIVE 1
#define GRAPH_RENDER_BILLBOARD 4
#define GRAPH_RENDER_ACTIVE 1
#define OBJ_FLAG_THROWN_WATER_SPLASH_PENDING 1
#define OBJ_FLAG_THROWN_ABOVE_WATER 2
#define OBJ_FLAG_HOLDABLE (1 << 10)
#define HELD_HELD 1
#define HELD_THROWN 2
#define HELD_DROPPED 3
#define INPUT_OFF_FLOOR (1 << 9)
#define INPUT_STOMPED (1 << 10)
#define INT_SUBTYPE_GRABS_MARIO (1 << 2)
#define ACT_IDLE 0x0C400201
#define ACT_PICKING_UP 0x00000383
#define ACT_FREEFALL 0x0100088C
#define ACT_SHOCKWAVE_BOUNCE 0x00020226
#define ACT_HOLD_IDLE 0x08000207
#define ACT_HOLD_HEAVY_IDLE 0x08000208
#define ACT_STATE_PICKING_UP_GRAB 0
#define ACT_STATE_PICKING_UP_HOLDING 1
#define GRAB_POS_LIGHT_OBJ 1
#define GRAB_POS_HEAVY_OBJ 2
#define MARIO_ANIM_GRAB_HEAVY_OBJECT 1
#define MARIO_ANIM_PICK_UP_LIGHT_OBJ 2
#define SOUND_MARIO_HRMM 1
#define MARIO_MARIO_SOUND_PLAYED 1
struct GraphNode { int flags; };
struct ObjectNode {
    struct ObjectNode *next, *prev;
    struct {
        struct GraphNode node;
        void *throwMatrix;
        float cameraToObject[3];
    } gfx;
};
struct Object {
    struct ObjectNode header;
    int activeFlags, metalContactCooldown, metalContactPendingDefeat;
    int bowserArenaBooTimer, bowserArenaSpinyAge, bowserThrowAirFrames;
    int bowserArenaCoinRolled, bowserArenaFade, bowserArenaCollapse;
    int bowserArenaFireMask, bowserArenaStage, bowserArenaFallAllowed;
    int oFlags, oHeldState, oGroundTwirlStunTimer, oGroundTwirlContactRebound;
    int bhvStackIndex, oInteractionSubtype;
    struct Object *prevObj, *bowserArenaOwner, *parentObj;
    const BehaviorScript *behavior;
    void *oFloor, *curBhvCommand;
};
struct MarioBodyState { int grabPos; };
struct MarioState {
    u32 action, input;
    int actionState;
    struct Object *heldObj, *usedObj;
    struct MarioBodyState *marioBodyState;
};
static struct MarioState *gMarioState;
static struct ObjectNode gFreeObjectList;
static struct GraphNode gObjParentGraphNode;
static struct Object *o;
static const BehaviorScript bhvCarrySomethingHeld[] = { 1 };
static const BehaviorScript bhvCarrySomethingThrown[] = { 2 };
static const BehaviorScript bhvCarrySomethingDropped[] = { 3 };
static int animationEnded, selectedAnimation, grabbed;
static void *segmented_to_virtual(const void *p) { return (void *)p; }
#define OBJSERVICE(name) static void name(struct Object *obj) { (void)obj; }
OBJSERVICE(particle_budget_allocated)
OBJSERVICE(object_platform_motion_allocated)
OBJSERVICE(particle_budget_freed)
OBJSERVICE(persistent_surfaces_forget)
OBJSERVICE(object_platform_motion_freed)
OBJSERVICE(mario_forget_attack_object)
OBJSERVICE(object_platform_motion_reset)
OBJSERVICE(player_stats_object_reset)
static void player_stats_grab(struct Object *obj) { (void)obj; grabbed++; }
static void geo_remove_child(struct GraphNode *node) { (void)node; }
static void geo_add_child(struct GraphNode *a, struct GraphNode *b) { (void)a; (void)b; }
static void stop_sounds_from_source(float *p) { (void)p; }
static int set_mario_action(struct MarioState *m, u32 action, u32 arg) {
    (void)arg; m->action = action; m->actionState = 0; return TRUE;
}
static int drop_and_set_mario_action(struct MarioState *m, u32 action, u32 arg) {
    m->heldObj = NULL; return set_mario_action(m, action, arg);
}
static int is_anim_at_end(struct MarioState *m) { (void)m; return animationEnded; }
static void play_sound_if_no_flag(struct MarioState *m, int sound, int flag) {
    (void)m; (void)sound; (void)flag;
}
static void set_mario_animation(struct MarioState *m, int animation) {
    (void)m; selectedAnimation = animation;
}
static void stationary_ground_step(struct MarioState *m) { (void)m; }
'''

CHECKS = r'''
static struct ObjectNode live;
static struct Object target;
static struct MarioBodyState body;
static struct MarioState mario;
static void begin(void) {
    memset(&target, 0, sizeof(target));
    memset(&mario, 0, sizeof(mario));
    memset(&gFreeObjectList, 0, sizeof(gFreeObjectList));
    target.activeFlags = ACTIVE_FLAG_ACTIVE;
    target.oFlags = OBJ_FLAG_HOLDABLE;
    live.next = live.prev = &target.header;
    target.header.next = target.header.prev = &live;
    mario.action = ACT_PICKING_UP;
    mario.usedObj = &target;
    mario.marioBodyState = &body;
    gMarioState = &mario;
    animationEnded = grabbed = selectedAnimation = 0;
}
int main(void) {
    for (int kind = 0; kind < 3; kind++) {
        begin();
        if (kind == 0) {
            target.activeFlags = ACTIVE_FLAG_DEACTIVATED;
        } else {
            unload_object(&target);
            assert(mario.usedObj == NULL && gFreeObjectList.next == &target.header);
            if (kind == 2) {
                struct Object *replacement = try_allocate_object(&live, &gFreeObjectList);
                assert(replacement == &target);
                replacement->activeFlags = ACTIVE_FLAG_ACTIVE;
                replacement->oHeldState = 0;
            }
        }
        animationEnded = TRUE;
        assert(act_picking_up(&mario) == TRUE);
        assert(mario.action == ACT_IDLE && mario.usedObj == NULL && mario.heldObj == NULL);
        assert(grabbed == 0 && target.oHeldState == 0);
    }
    for (int heavy = 0; heavy < 2; heavy++) {
        begin();
        target.oInteractionSubtype = heavy ? INT_SUBTYPE_GRABS_MARIO : 0;
        assert(act_picking_up(&mario) == FALSE);
        assert(mario.action == ACT_PICKING_UP && mario.heldObj == NULL && grabbed == 0);
        animationEnded = TRUE;
        assert(act_picking_up(&mario) == FALSE);
        assert(mario.heldObj == &target && target.oHeldState == HELD_HELD && grabbed == 1);
        assert(mario.action == (heavy ? ACT_HOLD_HEAVY_IDLE : ACT_HOLD_IDLE));
        assert(body.grabPos == (heavy ? GRAB_POS_HEAVY_OBJ : GRAB_POS_LIGHT_OBJ));
        assert(selectedAnimation == (heavy ? MARIO_ANIM_GRAB_HEAVY_OBJECT : MARIO_ANIM_PICK_UP_LIGHT_OBJ));
    }
    for (int stomp = 0; stomp < 2; stomp++) {
        begin();
        mario.input = stomp ? INPUT_STOMPED : INPUT_OFF_FLOOR;
        animationEnded = TRUE;
        assert(act_picking_up(&mario) == TRUE);
        assert(mario.action == (stomp ? ACT_SHOCKWAVE_BOUNCE : ACT_FREEFALL));
        assert(mario.heldObj == NULL && target.oHeldState == 0 && grabbed == 0);
    }
    begin();
    mario.heldObj = &target; // Dive pickup can acquire before entering this action.
    mario.usedObj = NULL;
    target.oHeldState = HELD_HELD;
    animationEnded = TRUE;
    assert(act_picking_up(&mario) == FALSE);
    assert(mario.action == ACT_HOLD_IDLE && mario.heldObj == &target && grabbed == 0);
    begin();
    mario.heldObj = &target;
    mario.actionState = ACT_STATE_PICKING_UP_HOLDING;
    unload_object(&target);
    assert(mario.usedObj == &target); // Only a pending, empty-handed pickup is invalidated.
    begin();
    mario.action = ACT_FREEFALL;
    unload_object(&target);
    assert(mario.usedObj == &target);
    return 0;
}
'''


def production_functions():
    spawn = Source(ROOT / "src/game/spawn_object.c")
    interaction = Source(ROOT / "src/game/interaction.c")
    helpers = Source(ROOT / "src/game/object_helpers.c")
    actions = Source(ROOT / "src/game/mario_actions_object.c")
    return "\n".join([
        interaction.function("mario_forget_pickup_object"),
        spawn.function("try_allocate_object"), spawn.function("deallocate_object"),
        spawn.function("unload_object"), helpers.function("obj_set_held_state"),
        interaction.function("mario_grab_used_object"), actions.function("act_picking_up"),
    ])


class PendingPickupTests(unittest.TestCase):
    def test_native_lifetime_and_pickup_preservation(self):
        with tempfile.TemporaryDirectory(prefix="sm64-pending-pickup-") as directory:
            folder = Path(directory)
            source = folder / "fixture.c"
            source.write_text(PRELUDE + production_functions()
                              + '\n#line 1 "pending_pickup_checks"\n' + CHECKS)
            executable = folder / "test"
            subprocess.run([
                "cc", "-std=c99", "-Wall", "-Wextra", "-Werror",
                "-fsanitize=address,undefined", "-fno-sanitize-recover=all",
                "-fno-omit-frame-pointer", str(source), "-o", str(executable),
            ], check=True, capture_output=True, text=True, timeout=30)
            result = subprocess.run(
                [str(executable)], capture_output=True, text=True, timeout=10,
                env={**os.environ, "ASAN_OPTIONS": "detect_leaks=0"},
            )
            self.assertEqual(result.returncode, 0, result.stderr)


if __name__ == "__main__":
    unittest.main()
