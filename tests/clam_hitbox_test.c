/* Production clam loop, animation initialization and tangibility helpers.
 * Animation frames/render visibility are supplied; no collision engine or renderer. */
#include <stdint.h>
#include <math.h>
#include "test_support.h"
typedef int16_t s16;
typedef int32_t s32;
typedef uint32_t u32;
typedef float f32;
#define TRUE 1
#define FALSE 0
#define BIT(x) (1u << (x))
#define INTERACT_CLAM_OR_BUBBA 1
#define SOUND_GENERAL_CLAM_SHELL_CLOSE 1
#define SOUND_GENERAL_CLAM_SHELL_OPEN 2
#define MODEL_BUBBLE 1
static const int bhvBubbleMaybe = 1;
struct Animation {
    s16 flags, startFrame, loopStart, loopEnd;
};
#include "clam_definitions.inc"
struct AnimInfo {
    struct Animation *curAnim;
    s16 animFrame, animYTrans;
    s32 animAccel;
};
struct GraphNodeObject {
    struct AnimInfo animInfo;
    f32 scale[3];
};
struct Object {
    struct {
        struct GraphNodeObject gfx;
    } header;
    struct Animation **oAnimations;
    s32 oAction, oTimer, oClamShakeTimer, oIntangibleTimer, oSoundStateID;
    f32 oDistanceToMario;
    s32 oFlags, oInteractType, oDamageOrCoinValue, oHealth, oNumLootCoins;
    f32 hitboxRadius, hitboxHeight, hurtboxRadius, hurtboxHeight, hitboxDownOffset;
};
struct ObjectHitbox {
    s32 interactType, downOffset, damageOrCoinValue, health, numLootCoins;
    s32 radius, height, hurtboxRadius, hurtboxHeight;
};
static struct Object clam, *o = &clam;
static struct Animation *animations[] = { &sAnimations[0], &sAnimations[1] };
static s32 rendering, closeSounds, openSounds, mist, bubbles, shakes, attackChecks;
static void *segmented_to_virtual(void *p) {
    return p;
}
static void cur_obj_play_sound_2(s32 sound) {
    if (sound == SOUND_GENERAL_CLAM_SHELL_CLOSE) {
        closeSounds++;
    } else if (sound == SOUND_GENERAL_CLAM_SHELL_OPEN) {
        openSounds++;
    } else {
        fprintf(stderr, "HARNESS ERROR: unexpected clam sound\n");
        exit(2);
    }
}
static void spawn_mist_from_global(void) {
    mist++;
}
static void cur_obj_shake_y(f32 amplitude) {
    (void) amplitude;
    shakes++;
}
static s32 obj_is_rendering_enabled(void) {
    return rendering;
}
static void spawn_object_relative(s32 param, s16 x, s16 y, s16 z, struct Object *parent, s32 model,
                                  int behavior) {
    (void) param;
    (void) x;
    (void) y;
    (void) z;
    (void) parent;
    (void) model;
    (void) behavior;
    bubbles++;
}
static f32 sins(s16 angle) {
    return sinf(angle * 6.28318530718f / 65536.0f);
}
static f32 coss(s16 angle) {
    return cosf(angle * 6.28318530718f / 65536.0f);
}
void obj_set_hitbox(struct Object *obj, struct ObjectHitbox *hitbox);
static s32 obj_check_attacks(struct ObjectHitbox *hitbox, s32 action) {
    (void) action;
    if (hitbox->damageOrCoinValue != 2 || hitbox->radius != 150 || hitbox->height != 80) {
        fprintf(stderr, "HARNESS ERROR: clam damage/shape unexpectedly changed\n");
        exit(2);
    }
    obj_set_hitbox(o, hitbox);
    attackChecks++;
    return FALSE;
}
#include "clam_sources.inc"
static void begin(s32 action, s16 frame, s32 visible) {
    memset(&clam, 0, sizeof(clam));
    clam.oAnimations = animations;
    clam.oAction = action;
    clam.header.gfx.animInfo.curAnim = animations[action];
    clam.header.gfx.animInfo.animFrame = frame;
    clam.oDistanceToMario = 100;
    clam.oTimer = 40;
    rendering = visible;
    clam.header.gfx.scale[0] = clam.header.gfx.scale[2] = 1;
    closeSounds = openSounds = mist = bubbles = shakes = attackChecks = 0;
}
static int damage_boundaries(void) {
    for (int visible = 0; visible < 2; visible++) {
        for (int wrongState = 0; wrongState < 2; wrongState++) {
            for (int frame = -1; frame < 60; frame++) {
                begin(CLAM_ACT_OPENING, frame, visible);
                clam.oIntangibleTimer = wrongState ? 0 : -1;
                bhv_clam_loop();
                EXPECT_EQ(clam.oIntangibleTimer, frame < 8 ? 0 : -1);
                EXPECT_EQ(attackChecks, 1);
                EXPECT_EQ(bubbles, visible && frame == 8 ? 13 : 0);
                EXPECT_EQ(clam.oTimer, 40);
                EXPECT_EQ(clam.oAction, CLAM_ACT_OPENING);
            }
            for (int frame = -1; frame < 30; frame++) {
                begin(CLAM_ACT_CLOSING, frame, visible);
                clam.oIntangibleTimer = wrongState ? 0 : -1;
                bhv_clam_loop();
                EXPECT_EQ(clam.oIntangibleTimer, frame < 25 ? -1 : 0);
                EXPECT_EQ(attackChecks, 1);
                EXPECT_EQ(closeSounds, frame == 25 ? 1 : 0);
                EXPECT_EQ(mist, frame == 25 ? 1 : 0);
                EXPECT_EQ(clam.oTimer, frame == 25 ? 0 : 40);
            }
        }
    }
    return 1;
}
static int skipped_and_frozen_frames(void) {
    begin(CLAM_ACT_OPENING, 7, 0);
    bhv_clam_loop();
    EXPECT_EQ(clam.oIntangibleTimer, 0);
    clam.header.gfx.animInfo.animFrame = 9;
    bhv_clam_loop();
    EXPECT_EQ(clam.oIntangibleTimer, -1);
    for (int frame = 0; frame < 10; frame++) {
        bhv_clam_loop();
        EXPECT_EQ(clam.oIntangibleTimer, -1);
    }
    EXPECT_EQ(bubbles, 0);
    begin(CLAM_ACT_CLOSING, 24, 0);
    bhv_clam_loop();
    EXPECT_EQ(clam.oIntangibleTimer, -1);
    clam.header.gfx.animInfo.animFrame = 29;
    bhv_clam_loop();
    EXPECT_EQ(clam.oIntangibleTimer, 0);
    for (int frame = 0; frame < 10; frame++) {
        bhv_clam_loop();
        EXPECT_EQ(clam.oIntangibleTimer, 0);
    }
    EXPECT_EQ(closeSounds, 0);
    EXPECT_EQ(mist, 0);
    return 1;
}
static int repeated_phase_changes(void) {
    begin(CLAM_ACT_CLOSING, 29, 1);
    clam.oTimer = 150;
    bhv_clam_loop();
    EXPECT_EQ(clam.oAction, CLAM_ACT_CLOSING);
    EXPECT_EQ(clam.oIntangibleTimer, 0);
    clam.oTimer = 151;
    clam.oDistanceToMario = 500;
    bhv_clam_loop();
    EXPECT_EQ(openSounds, 0);
    clam.oDistanceToMario = 499;
    for (int cycle = 0; cycle < 3; cycle++) {
        bhv_clam_loop();
        EXPECT_EQ(clam.oAction, CLAM_ACT_OPENING);
        EXPECT_EQ(openSounds, cycle + 1);
        // Native animation initialization resets the old closing pose on entry.
        clam.oTimer = 0;
        bhv_clam_loop();
        EXPECT_EQ(clam.header.gfx.animInfo.animFrame, -1);
        EXPECT_EQ(clam.oIntangibleTimer, 0);
        for (int frame = 0; frame < 60; frame++) {
            clam.header.gfx.animInfo.animFrame = frame;
            clam.oTimer = frame;
            bhv_clam_loop();
            EXPECT_EQ(clam.oIntangibleTimer, frame < 8 ? 0 : -1);
        }
        clam.oTimer = 150;
        bhv_clam_loop();
        EXPECT_EQ(clam.oAction, CLAM_ACT_OPENING);
        clam.oTimer = 151;
        bhv_clam_loop();
        EXPECT_EQ(clam.oAction, CLAM_ACT_CLOSING);
        EXPECT_EQ(clam.oIntangibleTimer, -1);
        clam.oTimer = 0;
        bhv_clam_loop();
        EXPECT_EQ(clam.header.gfx.animInfo.animFrame, -1);
        EXPECT_EQ(clam.oIntangibleTimer, -1);
        for (int frame = 0; frame < 30; frame++) {
            clam.header.gfx.animInfo.animFrame = frame;
            clam.oTimer = frame;
            bhv_clam_loop();
            EXPECT_EQ(clam.oIntangibleTimer, frame < 25 ? -1 : 0);
            if (frame == 25) {
                EXPECT_EQ(clam.oTimer, 0);
                EXPECT_EQ(clam.oClamShakeTimer, 10);
            }
        }
        EXPECT_EQ(closeSounds, cycle + 1);
        EXPECT_EQ(mist, cycle + 1);
        EXPECT_EQ(bubbles, (cycle + 1) * 13);
        EXPECT_EQ(clam.oClamShakeTimer, 6);
        clam.oTimer = 151;
    }
    return 1;
}
int main(int argc, char **argv) {
    test_init(argc, argv);
    if (test_selected("clam.damage_boundaries")) {
        test_report("clam.damage_boundaries", damage_boundaries());
    }
    if (test_selected("clam.skipped_and_frozen_frames")) {
        test_report("clam.skipped_and_frozen_frames", skipped_and_frozen_frames());
    }
    if (test_selected("clam.repeated_phase_changes")) {
        test_report("clam.repeated_phase_changes", repeated_phase_changes());
    }
    return test_finish();
}
