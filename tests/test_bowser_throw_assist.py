"""Host timing and state-preservation boundaries for Bowser throw assistance."""
from pathlib import Path
import os
import subprocess
import tempfile
import unittest
from regression_support import Source

ROOT = Path(__file__).resolve().parents[1]


HARNESS = r"""
/* Production prediction/selection; flat-floor movement is a supplied service. */
#include <assert.h>
#include <math.h>
#include <stdint.h>
#include <string.h>
typedef int16_t s16;
typedef int32_t s32;
typedef uint32_t u32;
typedef float f32;
#define TRUE 1
#define FALSE 0
#define ABS(a) ((a) < 0 ? -(a) : (a))
#define CLAMP(a, lo, hi) ((a) < (lo) ? (lo) : (a) > (hi) ? (hi) : (a))
#define sqr(a) ((a) * (a))
#define OBJ_LIST_GENACTOR 0
#define ACTIVE_FLAG_DEACTIVATED 0
#define BOWSER_BOMB_ACT_ARMED 0
#define BOWSER_ACT_THROWN 1
#define BOWSER_MINE_MIN_THROW_SPEED 25.0f
#define OBJ_MOVE_ON_GROUND 1
#define OBJ_MOVE_LANDED 2
struct Object {
    struct {
        struct Object *next;
    } header;
    struct Object *parentObj;
    const void *behavior;
    u32 activeFlags, oMoveFlags;
    s32 oAction, oIntangibleTimer, oBowserHeldAnglePitch, oBowserHeldAngleVelYaw;
    s16 oMoveAngleYaw;
    f32 oPosX, oPosY, oPosZ, oForwardVel, oVelY, oDragStrength;
};
struct ObjectNode {
    struct Object *next;
};
struct MarioState {
    struct Object *heldObj, *marioObj;
    f32 pos[3], intendedMag;
    s16 faceAngle[3], angleVel[3], twirlYaw, intendedYaw;
    u32 actionArg;
};
static struct ObjectNode gObjectLists[1];
static struct Object *gCurrentObject;
static u32 gGlobalTimer;
static s16 gCollisionFlags;
static const char bhvBowserBomb[1];
enum { GO_BOWSER_HOLD_SPIN, GO_BOWSER_THROW, GO_BOWSER_FIGHTS };
static s32 spinAssist, originalFight, throwAssist = TRUE;
static s32 game_options_get(s32 id) { return id == GO_BOWSER_THROW ? throwAssist : id == GO_BOWSER_FIGHTS ? originalFight : spinAssist; }
static s32 game_options_cheat(s32 id) { assert(id == GO_BOWSER_HOLD_SPIN); return spinAssist; }
static s32 obj_has_behavior(struct Object *obj, const void *behavior) {
    return obj->behavior == behavior;
}
static f32 dist_between_objects(struct Object *a, struct Object *b) {
    return sqrtf(sqr(a->oPosX - b->oPosX) + sqr(a->oPosY - b->oPosY) + sqr(a->oPosZ - b->oPosZ));
}
static f32 sins(s16 angle) {
    return sinf(angle * (6.283185307179586f / 65536.0f));
}
static f32 coss(s16 angle) {
    return cosf(angle * (6.283185307179586f / 65536.0f));
}
static s32 approach_s32(s32 value, s32 target, s32 inc, s32 dec) {
    return value < target ? CLAMP(value + inc, value, target) : CLAMP(value - dec, target, value);
}
static void cur_obj_get_thrown_or_placed(f32 forward, f32 up, s32 action) {
    struct Object *obj = gCurrentObject, *parent = obj->parentObj;
    obj->oPosX =
        parent->oPosX + 321.577f * sins(parent->oMoveAngleYaw) - 41.684f * coss(parent->oMoveAngleYaw);
    obj->oPosZ =
        parent->oPosZ + 321.577f * coss(parent->oMoveAngleYaw) + 41.684f * sins(parent->oMoveAngleYaw);
    obj->oPosY = parent->oPosY + 85.859f - 3.0f;
    obj->oMoveAngleYaw = parent->oMoveAngleYaw;
    obj->oMoveFlags = 0;
    obj->oAction = action;
}
static void cur_obj_update_floor_and_walls(void) {
    gCollisionFlags = 0;
}
static s32 forcedRecovery;
static s32 bowser_check_fallen_off_stage(void) {
    return forcedRecovery;
}
static int movementCalls;
static void cur_obj_move_standard(s16 slope) {
    struct Object *obj = gCurrentObject;
    movementCalls++;
    f32 vx = obj->oForwardVel * sins(obj->oMoveAngleYaw);
    f32 vz = obj->oForwardVel * coss(obj->oMoveAngleYaw);
    vx -= copysignf(vx * vx * (obj->oDragStrength * 0.0001f), vx);
    vz -= copysignf(vz * vz * (obj->oDragStrength * 0.0001f), vz);
    obj->oForwardVel = sqrtf(vx * vx + vz * vz);
    obj->oPosX += vx;
    obj->oPosZ += vz;
    obj->oVelY = fmaxf(-78.0f, obj->oVelY - 4.0f);
    obj->oPosY += obj->oVelY;
    if (obj->oPosY < 0) {
        obj->oPosY = 0;
        obj->oMoveFlags = (obj->oMoveFlags & OBJ_MOVE_LANDED) ? OBJ_MOVE_ON_GROUND : OBJ_MOVE_LANDED;
        obj->oVelY *= -0.7f;
    }
}
#include "bowser_throw_assist_functions.inc"

static struct Object held, parent, mine, sentinel;
static struct MarioState mario;
static void setup(s16 yaw, s16 spin) {
    memset(&held, 0, sizeof(held));
    memset(&parent, 0, sizeof(parent));
    memset(&mine, 0, sizeof(mine));
    memset(&mario, 0, sizeof(mario));
    held.activeFlags = 1;
    held.oBowserHeldAnglePitch = -ABS(spin);
    held.oBowserHeldAngleVelYaw = spin;
    mine.activeFlags = 1;
    mine.behavior = bhvBowserBomb;
    mine.oPosZ = 2500;
    gObjectLists[0].next = &mine;
    mine.header.next = (struct Object *) &gObjectLists[0];
    mario.heldObj = &held;
    mario.marioObj = &parent;
    mario.faceAngle[1] = yaw;
    mario.angleVel[1] = spin;
    mario.intendedMag = 32;
    gCurrentObject = &sentinel;
    gCollisionFlags = 7;
    gGlobalTimer = 100;
    forcedRecovery = FALSE;
    spinAssist = FALSE;
    bowser_throw_reset_history();
}
static void check_unchanged(s16 yaw) {
    struct Object savedHeld = held, savedParent = parent, savedMine = mine;
    bowser_throw_assist(&mario);
    assert(mario.faceAngle[1] == yaw);
    assert(memcmp(&held, &savedHeld, sizeof(held)) == 0);
    assert(memcmp(&parent, &savedParent, sizeof(parent)) == 0);
    assert(memcmp(&mine, &savedMine, sizeof(mine)) == 0);
    assert(gCurrentObject == &sentinel && gCollisionFlags == 7);
}
int main(void) {
    setup(-0x1000, 0x1000);
    throwAssist = FALSE;
    int calls = movementCalls;
    check_unchanged(-0x1000);
    assert(movementCalls == calls); // Disabled assistance performs no prediction or adjustment.
    throwAssist = TRUE;
    bowser_throw_assist(&mario);
    assert(mario.faceAngle[1] == 0); // Re-enabling uses the same valid timing window.
    // Every held direction accelerates, without exceeding ordinary speed.
    const s16 directions[] = { 0, 0x4000, -0x4000, -0x8000 };
    for (u32 i = 0; i < sizeof(directions) / sizeof(*directions); i++) {
        setup(0, 0);
        spinAssist = TRUE;
        mario.intendedYaw = directions[i];
        s32 sign = directions[i] < 0 ? -1 : 1;
        for (s32 frame = 1; frame <= 64; frame++) {
            bowser_update_spin(&mario);
            assert(mario.angleVel[1] == sign * CLAMP(frame * 0x80, 0, 0x1000));
            mario.intendedYaw += 0x2000; // Direction stays latched during the hold.
        }
        mario.intendedMag = 20;
        bowser_update_spin(&mario);
        assert(!mario.actionArg && mario.angleVel[1] == sign * 0xFC0);
        mario.intendedMag = 21;
        bowser_update_spin(&mario);
        assert(mario.angleVel[1] == sign * 0x1000); // Resume retains current spin.
        spinAssist = FALSE;
        bowser_update_spin(&mario);
        assert(mario.actionArg == 1 && mario.angleVel[1] == sign * 0x1000);
        assert(mario.twirlYaw == mario.intendedYaw); // Fresh native stick sample.
    }
    // Prediction accelerates a private copy and snaps one update early.
    setup(-0x1000, 0xF80);
    spinAssist = TRUE;
    bowser_throw_assist(&mario);
    assert(mario.faceAngle[1] == 0 && mario.angleVel[1] == 0x1000);
    assert(mario.actionArg == 0); // Predictor did not mutate the hold latch.
    setup(-0x2000, 0xF80);
    spinAssist = TRUE;
    check_unchanged(-0x2000);
    assert(mario.angleVel[1] == 0xF80 && mario.actionArg == 0);
    for (int direction = -1; direction <= 1; direction += 2) {
        s16 spin = direction * 0x1000;
        setup(0, spin);
        struct BowserThrowRelease release = { 0, -0x1000, spin };
        assert(bowser_throw_predict(&mario, &release));
        check_unchanged(0); // Already valid throws preserve their release.
        setup(-spin, spin);
        bowser_throw_assist(&mario);
        assert(mario.faceAngle[1] == 0); // One frame early snaps forward.
        setup(0, spin);
        bowser_throw_record(&mario);
        mario.faceAngle[1] = spin;
        gGlobalTimer++;
        bowser_throw_assist(&mario);
        assert(mario.faceAngle[1] == 0); // One frame late snaps back.
        setup(0, spin);
        bowser_throw_record(&mario);
        mario.faceAngle[1] = spin * 2;
        gGlobalTimer += 2;
        check_unchanged(spin * 2); // Two frames late is outside the window.
        setup(-spin * 2, spin);
        check_unchanged(-spin * 2); // Two frames early is outside the window.
        setup(0, spin);
        bowser_throw_record(&mario);
        bowser_throw_reset_history();
        mario.faceAngle[1] = spin;
        gGlobalTimer++;
        check_unchanged(spin); // New grabs cannot inherit a previous release.
    }
    // The next candidate follows real stick acceleration/deceleration.
    setup(-0xFC0, 0x1000);
    mario.intendedMag = 0;
    bowser_throw_assist(&mario);
    assert(mario.faceAngle[1] == 0 && mario.angleVel[1] == 0xFC0);
    assert(held.oBowserHeldAngleVelYaw == 0xFC0 && held.oBowserHeldAnglePitch == -0xFC0);
    setup(-0x1000, 0xF80);
    mario.actionArg = 1;
    mario.intendedYaw = 0x4000;
    bowser_throw_assist(&mario);
    assert(mario.faceAngle[1] == 0 && mario.angleVel[1] == 0x1000);
    // A frame-number wrap still permits exactly one late update.
    setup(0, 0x1000);
    gGlobalTimer = UINT32_MAX;
    bowser_throw_record(&mario);
    mario.faceAngle[1] = 0x1000;
    gGlobalTimer = 0;
    bowser_throw_assist(&mario);
    assert(mario.faceAngle[1] == 0);
    setup(-0x1000, 0x1000);
    forcedRecovery = TRUE;
    check_unchanged(-0x1000); // No assistance for a flight interrupted by recovery.
    setup(-0x1000, 500);
    movementCalls = 0;
    check_unchanged(-0x1000);
    assert(movementCalls == 0); // Weak throws cannot qualify.
    setup(-0x1000, 0x1000);
    mine.oIntangibleTimer = -1;
    check_unchanged(-0x1000);
    setup(-0x1000, 0x1000);
    mine.oAction = 1;
    check_unchanged(-0x1000);
    setup(-0x1000, 0x1000);
    mine.activeFlags = 0;
    check_unchanged(-0x1000);
    setup(0, 0x1000);
    held.oPosZ = 1900;
    assert(bowser_throw_nearest_mine(&held) == NULL); // Strict 600 boundary.
    held.oPosZ = 1900.25f;
    assert(bowser_throw_nearest_mine(&held) == &mine);
    mine.oPosY = 600;
    assert(bowser_throw_nearest_mine(&held) == NULL); // Full 3D distance.
    // The Original fight uses its native 800-unit mine range and accepts weak throws.
    setup(0, 0);
    originalFight = TRUE;
    held.oPosZ = 1700;
    assert(bowser_throw_nearest_mine(&held) == NULL);
    held.oPosZ = 1700.25f;
    assert(bowser_throw_nearest_mine(&held) == &mine);
    mine.oPosZ = 500;
    struct BowserThrowRelease stationary = { 0, 0, 0 };
    assert(bowser_throw_predict(&mario, &stationary));
    assert(gCurrentObject == &sentinel && gCollisionFlags == 7);
    return 0;
}
"""


class BowserThrowAssist(unittest.TestCase):
    def test_prediction_and_one_frame_window(self):
        source = Source(ROOT / "src/game/bowser_throw.c")
        with tempfile.TemporaryDirectory() as directory:
            temporary = Path(directory)
            (temporary / "bowser_throw_assist_functions.inc").write_text(
                source.region(r"^#define BOWSER_THROW_PREDICTION_FRAMES", r"\Z"))
            (temporary / "test.c").write_text(HARNESS)
            executable = temporary / "test"
            subprocess.run([
                "cc", "-std=c99", "-Wall", "-Wextra", "-Wno-unused-parameter",
                "-fsanitize=address,undefined", "-fno-omit-frame-pointer", "-g",
                "-I", str(temporary), str(temporary / "test.c"),
                "-lm", "-o", str(executable),
            ], check=True, capture_output=True, text=True)
            subprocess.run([str(executable)], check=True,
                           env={**os.environ, "ASAN_OPTIONS": "detect_leaks=0"})


if __name__ == "__main__":
    unittest.main()
