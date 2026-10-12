/* Real object loops and twirl-stun wrapper; object behavior and classification
 * are controlled doubles. No rendering, enemy AI or collision simulation. */
#include <stdint.h>
#include <math.h>
#include "test_support.h"
typedef int32_t s32;
typedef uint32_t u32;
typedef uint8_t u8;
typedef float f32;
typedef uintptr_t BehaviorScript;
#define TRUE 1
#define FALSE 0
#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define MIN(a, b) ((a) < (b) ? (a) : (b))
#include "animation_definitions.inc"

struct ObjectNode {
    struct ObjectNode *next;
};
struct Object {
    struct ObjectNode node; // First so the actual list loop can cast to Object.
    struct {
        struct {
            struct {
                u32 flags;
            } node;
            f32 pos[3];
            f32 scale[3];
            int16_t angle[3];
        } gfx;
    } header;
    u32 metalContactPendingDefeat, metalContactCooldown;
    s32 oGroundTwirlStunTimer, oGroundTwirlContactRebound, oGroundTwirlStunDuration;
    s32 oMoveAngleYaw, oFaceAngleYaw;
    u32 oInteractStatus, oMoveFlags, oInteractType, activeFlags, oFlags;
    f32 oGravity, oWallHitboxRadius, hitboxRadius, oGraphYOffset, oPosY, oPosVec[3];
    f32 oForwardVel, oVelX, oVelY, oVelZ;
    f32 oFloorHeight;
    s32 oFloorType;
    void *oFloor, *collisionData;
    const BehaviorScript *behavior;
    int isEnemy, updates, animationOnUpdate;
};
static struct Object *gCurrentObject, *gMarioObject;
static u32 gTimeStopState;
static const BehaviorScript bhvHeaveHo[] = { 1 }, bhvMontyMole[] = { 2 }, bhvSnufit[] = { 3 },
                            bhvSwoop[] = { 4 };
static f32 absf(f32 value) {
    return fabsf(value);
}
static int classificationCalls;
static s32 mario_ground_twirl_is_enemy(struct Object *obj) {
    classificationCalls++;
    return obj->isEnemy;
}
static s32 obj_has_behavior(struct Object *obj, const BehaviorScript *behavior) {
    return obj->behavior == behavior;
}
static int defeatCalls;
static void cur_obj_defeat_metal_contact_enemy(void) {
    defeatCalls++;
}
static void cur_obj_update(void) {
    gCurrentObject->updates++;
    gCurrentObject->animationOnUpdate =
        !!(gCurrentObject->header.gfx.node.flags & GRAPH_RENDER_HAS_ANIMATION);
}
static void unsupported(const char *service) {
    fprintf(stderr, "HARNESS ERROR: animation test entered unmodeled %s\n", service);
    exit(2);
}
static void load_object_collision_model(void) {
    unsupported("sleeping surface collision");
}
// Landing is a supplied collision result for the wrapper/timer test. Actual
// sweep geometry is exercised separately by swimming.stunned_*.
static u32 landingFlags;
static int movementCalls;
static void cur_obj_update_floor_and_walls(
    void) { /* Supplied floor state: grounded unless a test requests movement. */
}
static void cur_obj_move_stunned(void) {
    if (!landingFlags) {
        unsupported("airborne movement");
    }
    movementCalls++;
    gCurrentObject->oMoveFlags = landingFlags;
    gCurrentObject->oVelY = 0;
}
static void vec3_copy_y_off(f32 *dst, f32 (*src)[3], f32 offset) {

    memcpy(dst, *src, sizeof(*src));
    dst[1] += offset;
}
static s32 ground_twirl_enemy_needs_water_update(struct Object *obj) {
    return FALSE;
}

// These objects are not Fight 3 arena enemies. Recovery and phase-in stay
// inactive; their real behavior is covered by the Bowser fixture.
static void bowser_arena_recover_enemy(struct Object *obj) {
}
static s32 bowser_arena_phase_in(struct Object *obj) {
    return FALSE;
}

#include "animation_sources.inc"

static struct ObjectNode sHead;
static struct Object sObjects[3];
static void begin(int count) {
    memset(sObjects, 0, sizeof(sObjects));
    sHead.next = &sObjects[0].node;
    for (int i = 0; i < count; i++) {
        sObjects[i].node.next = i + 1 < count ? &sObjects[i + 1].node : &sHead;
        sObjects[i].header.gfx.node.flags = GRAPH_RENDER_HAS_ANIMATION;
        sObjects[i].activeFlags = ACTIVE_FLAG_ACTIVE;
    }
    gMarioObject = NULL;
    gCurrentObject = NULL;
    gTimeStopState = TIME_STOP_ALL_OBJECTS;
    landingFlags = 0;
    movementCalls = 0;
    classificationCalls = 0;
    defeatCalls = 0;
}
static int animated(int i) {
    return !!(sObjects[i].header.gfx.node.flags & GRAPH_RENDER_HAS_ANIMATION);
}
static int step(void) {
    return update_objects_starting_at(&sHead, sHead.next);
}
static int paused_step(void) {
    return update_objects_during_time_stop(&sHead, sHead.next);
}

static int resume(void) {
    begin(2);
    sObjects[1].isEnemy = TRUE; // Both ordinary non-enemies and unstunned enemies.
    for (int cycle = 0; cycle < 3; cycle++) {
        EXPECT_EQ(paused_step(), 2);
        for (int i = 0; i < 2; i++) {
            EXPECT_EQ(animated(i), FALSE);
            EXPECT_EQ(sObjects[i].updates, cycle);
        }
        EXPECT_EQ(step(), 2);
        for (int i = 0; i < 2; i++) {
            EXPECT_EQ(animated(i), TRUE);
            EXPECT_EQ(sObjects[i].animationOnUpdate, TRUE);
            EXPECT_EQ(sObjects[i].updates, cycle + 1);
        }
    }
    return TRUE;
}

static int idle_fast_path(void) {
    begin(3);
    sObjects[1].isEnemy = TRUE;
    for (int frame = 0; frame < 70; frame++) {
        step();
        EXPECT_EQ(classificationCalls, 0);
        for (int i = 0; i < 3; i++) {
            EXPECT_EQ(sObjects[i].updates, frame + 1);
        }
    }
    // Non-enemies may use the aliased stun fields for unrelated behavior.
    sObjects[2].oGroundTwirlStunTimer = 7;
    sObjects[1].metalContactCooldown = 15;
    step();
    EXPECT_EQ(classificationCalls, 2);
    EXPECT_EQ(sObjects[1].metalContactCooldown, 14);
    EXPECT_EQ(sObjects[2].oGroundTwirlStunTimer, 7);
    EXPECT_EQ(sObjects[2].updates, 71);
    return TRUE;
}

static int selective_time_stop(void) {
    begin(3);
    gMarioObject = &sObjects[0];
    sObjects[1].oInteractType = INTERACT_DOOR;
    paused_step(); // First clear all animation flags.
    gTimeStopState = TIME_STOP_ENABLED;
    EXPECT_EQ(paused_step(), 3);
    EXPECT_EQ(sObjects[0].animationOnUpdate, TRUE);
    EXPECT_EQ(sObjects[1].animationOnUpdate, TRUE);
    EXPECT_EQ(sObjects[2].updates, 0);
    EXPECT_EQ(animated(2), FALSE);
    gTimeStopState |= TIME_STOP_MARIO_AND_DOORS;
    sObjects[2].activeFlags |= ACTIVE_FLAG_UNIMPORTANT;
    paused_step();
    EXPECT_EQ(animated(0), FALSE);
    EXPECT_EQ(animated(1), FALSE);
    EXPECT_EQ(sObjects[2].animationOnUpdate, TRUE);
    gTimeStopState = TIME_STOP_ALL_OBJECTS;
    paused_step();
    EXPECT_EQ(animated(2), FALSE);
    return TRUE;
}

static int stunned(void) {
    begin(1);
    sObjects[0].isEnemy = TRUE;
    sObjects[0].oGroundTwirlStunTimer = 2;
    sObjects[0].oGroundTwirlStunDuration = 2;
    sObjects[0].oForwardVel = sObjects[0].oVelX = 10;
    paused_step();
    EXPECT_EQ(sObjects[0].oGroundTwirlStunTimer, 2);
    for (int i = 0; i < 2; i++) {
        step();
        EXPECT_EQ(animated(0), FALSE);
        EXPECT_EQ(sObjects[0].updates, 0);
        EXPECT_EQ(sObjects[0].oGroundTwirlStunTimer, 1 - i);
        EXPECT_EQ(sObjects[0].oForwardVel, 0);
        EXPECT_EQ(sObjects[0].oVelX, 0);
    }
    step();
    EXPECT_EQ(sObjects[0].animationOnUpdate, TRUE);
    EXPECT_EQ(sObjects[0].updates, 1);
    return TRUE;
}

static int shared_fields(void) {
    begin(1);
    sObjects[0].oGroundTwirlStunTimer = 99; // These slots can mean something else for non-targets.
    sObjects[0].oGroundTwirlStunDuration = 7;
    paused_step();
    step();
    EXPECT_EQ(sObjects[0].oGroundTwirlStunTimer, 99);
    EXPECT_EQ(sObjects[0].animationOnUpdate, TRUE);
    EXPECT_EQ(sObjects[0].updates, 1);
    return TRUE;
}

static int attack_breaks_stun(void) {
    begin(1);
    sObjects[0].isEnemy = TRUE;
    sObjects[0].oGroundTwirlStunTimer = sObjects[0].oGroundTwirlStunDuration = 3;
    sObjects[0].oInteractStatus = INT_STATUS_WAS_ATTACKED;
    step();
    EXPECT_EQ(sObjects[0].oGroundTwirlStunTimer, 0);
    EXPECT_EQ(sObjects[0].updates, 1);
    EXPECT_EQ(sObjects[0].animationOnUpdate, TRUE);
    return TRUE;
}

static int metal_timeout(void) {
    for (int cycle = 0; cycle < 3; cycle++) {
        begin(2);
        sObjects[0].isEnemy = TRUE;
        sObjects[0].metalContactCooldown = sObjects[1].metalContactCooldown = 15;
        for (int update = 0; update < 15; update++) {
            paused_step();
            EXPECT_EQ(sObjects[0].metalContactCooldown, 15 - update);
            step();
            EXPECT_EQ(sObjects[0].metalContactCooldown, 14 - update);
            EXPECT_EQ(sObjects[1].metalContactCooldown, 15); // Field belongs to enemies only.
        }
        step();
        EXPECT_EQ(sObjects[0].metalContactCooldown, 0);
        // Short native ground stun must outlive the remaining contact grace.
        begin(1);
        sObjects[0].isEnemy = TRUE;
        sObjects[0].metalContactCooldown = 15;
        sObjects[0].oGroundTwirlStunTimer = -60;
        sObjects[0].oGroundTwirlStunDuration = 2;
        landingFlags = cycle % 2 ? OBJ_MOVE_UNDERWATER_ON_GROUND : OBJ_MOVE_LANDED;
        step();
        EXPECT_EQ(movementCalls, 1);
        EXPECT_EQ(sObjects[0].metalContactCooldown, 14);
        EXPECT_EQ(sObjects[0].oGroundTwirlStunTimer, 15);
        for (int update = 0; update < 14; update++) {
            paused_step();
            step();
            EXPECT_EQ(sObjects[0].updates, 0);
            EXPECT_EQ(sObjects[0].oGroundTwirlStunTimer, 14 - update);
        }
        EXPECT_EQ(sObjects[0].metalContactCooldown, 0);
        EXPECT_EQ(sObjects[0].oGroundTwirlStunTimer, 1);
        sObjects[0].oInteractStatus = INT_STATUS_WAS_ATTACKED;
        step();
        EXPECT_EQ(sObjects[0].updates, 1);
        EXPECT_EQ(sObjects[0].oGroundTwirlStunTimer, 0);
    }
    return TRUE;
}

static int metal_defeat_time_stop(void) {
    begin(1);
    sObjects[0].isEnemy = TRUE;
    sObjects[0].metalContactPendingDefeat = TRUE;
    sObjects[0].metalContactCooldown = 15;
    sObjects[0].oGroundTwirlStunTimer = -30;
    sObjects[0].oGroundTwirlStunDuration = 20;
    paused_step();
    EXPECT_EQ(defeatCalls, 0);
    EXPECT_EQ(sObjects[0].metalContactPendingDefeat, TRUE);
    EXPECT_EQ(sObjects[0].metalContactCooldown, 15);
    step();
    EXPECT_EQ(defeatCalls, 1);
    EXPECT_EQ(sObjects[0].metalContactPendingDefeat, FALSE);
    EXPECT_EQ(sObjects[0].metalContactCooldown, 0);
    EXPECT_EQ(sObjects[0].oGroundTwirlStunTimer, 0);
    EXPECT_EQ(sObjects[0].oGroundTwirlStunDuration, 0);
    EXPECT_EQ(animated(0), TRUE);
    EXPECT_EQ(sObjects[0].updates, 0);
    for (int update = 0; update < 3; update++) {
        step();
        EXPECT_EQ(defeatCalls, 1);
    }
    EXPECT_EQ(sObjects[0].updates, 3);
    return TRUE;
}

int main(int argc, char **argv) {
    const struct {
        const char *name;
        int (*run)(void);
    } tests[] = {
        { "animation.resume", resume },
        { "animation.selective_time_stop", selective_time_stop },
        { "animation.stunned", stunned },
        { "animation.shared_fields", shared_fields },
        { "animation.attack_breaks_stun", attack_breaks_stun },
        { "animation.metal_timeout", metal_timeout },
        { "animation.metal_defeat_time_stop", metal_defeat_time_stop },
        { "animation.idle_fast_path", idle_fast_path },
    };
    test_init(argc, argv);
    for (unsigned int i = 0; i < sizeof(tests) / sizeof(tests[0]); i++) {
        if (test_selected(tests[i].name)) {
            test_report(tests[i].name, tests[i].run());
        }
    }
    return test_finish();
}
