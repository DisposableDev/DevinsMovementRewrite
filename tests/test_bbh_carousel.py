"""Enclosure boundaries and suspension ownership; host checks, not N64 timings."""
from pathlib import Path
import unittest
import re
from collections import Counter
from regression_support import Source
from test_bbh_audio import compile_run

try:
    from .source_asset import read_text as read_source
except ImportError:
    from source_asset import read_text as read_source

ROOT = Path(__file__).resolve().parents[1]


class CarouselTests(unittest.TestCase):
    def test_exterior_geometry_preserves_shell(self):
        source = read_source(ROOT / 'levels/bbh/areas/1/31/model.inc.c')
        vertices = {}
        for name, body in re.findall(r'static const Vtx (\w+)\[\] = \{(.*?)\n\};', source, re.S):
            vertices[name] = [tuple(map(int, v)) for v in re.findall(
                r'\{\{\{\s*(-?\d+),\s*(-?\d+),\s*(-?\d+)', body)]
        display_lists = dict(re.findall(
            r'(?:static )?const Gfx (\w+)\[\] = \{(.*?)\n\};', source, re.S
        ))

        def display_list_triangles(name):
            result = []
            current_vertices = None
            for command, arguments in re.findall(r'(gs\w+)\((.*?)\),', display_lists[name], re.S):
                if command == 'gsSPDisplayList':
                    result.extend(display_list_triangles(arguments.strip()))
                elif command == 'gsSPVertex':
                    current_vertices = vertices[arguments.split(',')[0].strip()]
                elif command in ('gsSP1Triangle', 'gsSP2Triangles'):
                    values = [int(v.strip(), 0) for v in arguments.split(',')]
                    result.extend(tuple(current_vertices[j] for j in values[i:i+3])
                                  for i in range(0, len(values), 4))
            return result

        full_triangles = display_list_triangles('bbh_seg7_dl_07019EF8')
        exterior_triangles = display_list_triangles('bbh_carousel_exterior')
        removed_triangles = Counter(full_triangles) - Counter(exterior_triangles)
        self.assertFalse(Counter(exterior_triangles) - Counter(full_triangles))
        self.assertEqual(sum(removed_triangles.values()), 42)

        def inside_carousel_bounds(v):
            x, y, z = v
            return -1433 <= x <= 1024 and -2764 <= y <= -1433 and -1023 <= z <= 1434

        # Every removed face belongs to the interior; every shell face remains in order.
        self.assertTrue(all(
            all(inside_carousel_bounds(v) for v in triangle) for triangle in removed_triangles
        ))
        self.assertEqual(exterior_triangles, [
            t for t in full_triangles if not all(inside_carousel_bounds(v) for v in t)
        ])

    def test_boundaries_and_resume(self):
        source = Source(ROOT / 'src/game/object_helpers.c')
        production = '\n'.join(source.function(name) for name in (
            'bbh_merry_go_round_is_active',
            'bbh_merry_go_round_object_is_dormant',
            'bbh_merry_go_round_update_dormancy',
        ))
        collision = Source(ROOT / 'src/game/object_collision.c')
        production += '\n' + collision.function('clear_object_collision')
        production += '\n' + collision.function('check_collision_in_list')
        code = r'''
#include <assert.h>
#include <stddef.h>
#include <string.h>
#include "object_constants.h"
#include "level_table.h"
typedef int s32;
typedef float f32;
#define TRUE 1
#define FALSE 0
#define GRAPH_RENDER_ACTIVE 1
#define GRAPH_RENDER_HAS_ANIMATION 2
struct Object {
    struct {
        struct Object *next;
        struct {
            struct {
                int flags;
            } node;
        } gfx;
    } header;
    int oIntangibleTimer, numCollidedObjs, collidedObjInteractTypes;
    float hurtboxRadius;
    unsigned activeFlags;
    int behavior;
    struct Object *parentObj;
    int timer, action, progress, oAction;
};
static struct {
    float pos[3];
} mario, *gMarioState = &mario;
static int gCurrLevelNum = LEVEL_BBH;
enum {
    bhvMerryGoRoundBooManager = 1,
    bhvMerryGoRoundBoo,
    bhvMerryGoRoundBigBoo,
    bhvFlamethrower,
    bhvFlamethrowerFlame,
    bhvCoinInsideBoo,
    other
};
static int obj_has_behavior(struct Object *obj, int behavior) {
    return obj->behavior == behavior;
}
// Observe overlap-check eligibility without modeling hitbox geometry.
static int hitboxChecks;
static int detect_object_hitbox_overlap(struct Object *a, struct Object *b) {
    (void) a;
    (void) b;
    hitboxChecks++;
    return 0;
}
static void detect_object_hurtbox_overlap(struct Object *a, struct Object *b) {
    (void) a;
    (void) b;
    assert(0);
}
/* PRODUCTION */
// Set the shared Mario position and check the enclosure predicate at that point.
static void check_enclosure_at(float x, float y, float z, int expectedActive) {
    mario.pos[0] = x;
    mario.pos[1] = y;
    mario.pos[2] = z;
    assert(bbh_merry_go_round_is_active() == expectedActive);
}
int main(void) {
    // Check vertical, bevel and doorway thresholds.
    check_enclosure_at(2260, -2626, 514, 0); // Reported water position.
    check_enclosure_at(-205, -2560, 205, 1);
    check_enclosure_at(-205, -1433, 205, 1);   // Falling/jumping above the platform.
    check_enclosure_at(-205, -1432, 205, 0);   // Upstairs must stay separate.
    check_enclosure_at(-1433, -2457, 1434, 0); // Outside the octagon's bevel.
    check_enclosure_at(-1945, -2457, 0, 1);    // Left doorway before the rotating floor.
    check_enclosure_at(1536, -2457, 410, 1);   // Right doorway.
    check_enclosure_at(1537, -2457, 410, 0);
    check_enclosure_at(-205, -2765, 205, 0);
    // Suspension preserves state and restores each initial render-flag combination.
    for (int behavior = bhvMerryGoRoundBooManager; behavior <= bhvFlamethrower; behavior++) {
        for (int flags = 0; flags < 4; flags++) {
            struct Object obj = { 0 };
            obj.behavior = behavior;
            obj.header.gfx.node.flags = flags;
            obj.activeFlags = ACTIVE_FLAG_ACTIVE;
            obj.timer = 57;
            obj.action = 2;
            obj.progress = 3;
            check_enclosure_at(2260, -2626, 514, 0);
            assert(bbh_merry_go_round_update_dormancy(&obj));
            assert(obj.header.gfx.node.flags == 0);
            assert(bbh_merry_go_round_update_dormancy(&obj));
            assert(obj.timer == 57 && obj.action == 2 && obj.progress == 3);
            check_enclosure_at(-205, -2560, 205, 1);
            assert(!bbh_merry_go_round_update_dormancy(&obj));
            assert(obj.header.gfx.node.flags == flags);
            assert(obj.activeFlags == ACTIVE_FLAG_ACTIVE);
        }
    }
    // Verify owner-dependent children, release transitions and level isolation.
    struct Object parent = { 0 }, child = { 0 };
    check_enclosure_at(2260, -2626, 514, 0);
    parent.behavior = bhvMerryGoRoundBoo;
    child.behavior = bhvCoinInsideBoo;
    child.parentObj = &parent;
    assert(bbh_merry_go_round_object_is_dormant(&child));
    child.oAction = COIN_INSIDE_BOO_ACT_DROPPED;
    child.header.gfx.node.flags = GRAPH_RENDER_ACTIVE;
    // Release changes the action one update before changing the behavior.
    assert(!bbh_merry_go_round_object_is_dormant(&child));
    assert(!bbh_merry_go_round_update_dormancy(&child));
    assert(child.header.gfx.node.flags == GRAPH_RENDER_ACTIVE);
    child.behavior = other; // Released coins retain their normal lifetime.
    assert(!bbh_merry_go_round_object_is_dormant(&child));
    parent.behavior = bhvFlamethrower;
    child.behavior = bhvFlamethrowerFlame;
    assert(bbh_merry_go_round_object_is_dormant(&child));
    child.parentObj = NULL;
    assert(!bbh_merry_go_round_object_is_dormant(&child));
    gCurrLevelNum = LEVEL_SSL;
    child.parentObj = &parent;
    assert(!bbh_merry_go_round_object_is_dormant(&child));
    gCurrLevelNum = LEVEL_BBH;
    // Dormant members still clear contacts; ticking and overlap checks resume on entry.
    struct Object sentinel = { 0 }, target = { 0 }, player = { 0 };
    target.behavior = bhvMerryGoRoundBoo;
    sentinel.header.next = &target;
    target.header.next = &sentinel;
    target.oIntangibleTimer = 57;
    target.numCollidedObjs = 2;
    clear_object_collision(&sentinel);
    assert(target.oIntangibleTimer == 57 && target.numCollidedObjs == 0);
    target.oIntangibleTimer = 0;
    check_collision_in_list(&player, &target, &sentinel);
    assert(hitboxChecks == 0);
    check_enclosure_at(-205, -2560, 205, 1);
    check_collision_in_list(&player, &target, &sentinel);
    assert(hitboxChecks == 1); // Entry does not inherit the previous frame's dormancy.
    target.oIntangibleTimer = 57;
    clear_object_collision(&sentinel);
    assert(target.oIntangibleTimer == 56);
    return 0;
}
'''
        compile_run(code.replace('/* PRODUCTION */', production))


if __name__ == '__main__':
    unittest.main()
