#!/usr/bin/env python3
"""Production camera effects: output reduction preserves phase, decay and RNG."""
from pathlib import Path
import os
import subprocess
import tempfile
import unittest

from regression_support import Source

ROOT = Path(__file__).resolve().parents[1]

PRELUDE = r'''
#include <assert.h>
#include <math.h>
#include <stdint.h>
#include <string.h>
typedef int8_t s8;
typedef uint8_t u8;
typedef int16_t s16;
typedef uint16_t u16;
typedef int32_t s32;
typedef uint32_t u32;
typedef float f32;
typedef f32 Vec3f[3];
typedef s16 Vec3s[3];
#define TRUE 1
#define FALSE 0
#define sqr(a) ((a) * (a))
#define vec3_zero(v) memset((v), 0, sizeof(Vec3f))
#define vec3s_copy(a, b) memcpy((a), (b), sizeof(Vec3s))
#define vec3f_copy(a, b) memcpy((a), (b), sizeof(Vec3f))
#define CLAMP(x, lo, hi) ((x)<(lo) ? (lo) : (x)>(hi) ? (hi) : (x))
#define CAM_MOVE_PAUSE_SCREEN 1
enum { GO_CAMERA_SHAKE, GO_PAUSED_CAMERA };
static s32 shakeMode, pausedCamera;
static s32 game_options_get(s32 id) {
    return id==GO_CAMERA_SHAKE ? shakeMode : pausedCamera;
}
static s32 controlsAllowed=1;
static s32 pause_camera_controls_allowed(void) { return controlsAllowed; }
static u32 gGlobalTimer;
static u16 gCameraMovementFlags;
#define CUTSCENE_NONE 0
#define CAM_EVENT_NONE 0
static s32 sPausedCameraUpdating, sPausedCameraUsesNormalView;
struct Camera { s32 updates, cutscene; };
static struct { struct Camera *camera; } area, *gCurrentArea=&area;
static struct { s32 cameraEvent; } marioCam, *sMarioCamState=&marioCam;
static s32 sObjectCutscene;
struct GraphNodeCamera { struct { struct Camera *camera; } config; Vec3f pos, focus; };
static struct { u16 buttonDown, buttonPressed; } controller, *gPlayer1Controller=&controller;
static struct {
    Vec3s shakeMagnitude;
    s16 shakePitchPhase, shakePitchVel, shakePitchDecay;
    s16 shakeYawPhase, shakeYawVel, shakeYawDecay;
    s16 shakeRollPhase, shakeRollVel, shakeRollDecay;
} gLakituState;
static s16 sHandheldShakeMag, sHandheldShakePitch, sHandheldShakeYaw, sHandheldShakeRoll;
static f32 sHandheldShakeTimer, sHandheldShakeInc;
static s32 randomCalls, geometryCalls;
static f32 random_float(void) { return ((++randomCalls * 7) % 17) / 17.0f; }
static f32 sins(s16 angle) { return sinf(angle * (6.283185307179586f / 65536.0f)); }
static f32 coss(s16 angle) { return cosf(angle * (6.283185307179586f / 65536.0f)); }
// Supplied polar-coordinate services observe exact deltas and detect Off round-trips.
static void vec3f_get_dist_and_angle(Vec3f pos, Vec3f focus, f32 *dist, s16 *pitch, s16 *yaw) {
    (void) pos; geometryCalls++; *dist=focus[0]; *pitch=focus[1]; *yaw=focus[2];
}
static void vec3f_set_dist_and_angle(Vec3f pos, Vec3f focus, f32 dist, s16 pitch, s16 yaw) {
    (void) pos; geometryCalls++; focus[0]=dist; focus[1]=pitch; focus[2]=yaw;
}
struct GraphNodePerspective { f32 fov; };
struct GraphNode { s32 unused; };
struct MarioState { s32 unused; };
typedef s32 Gfx;
#define UNUSED __attribute__((unused))
#define GEO_CONTEXT_RENDER 1
static struct MarioState gMarioStates[1];
static s32 fovUpdates;
'''

CHECKS = r'''
static void periodic(void) {
    for (s32 mode = 0; mode < 2; mode++) {
        shakeMode=mode;
        f32 scale=mode==0 ? 1.0f : 0.0f;
        assert(game_options_shake_scale()==scale);
        memset(&gLakituState,0,sizeof(gLakituState));
        gLakituState.shakeMagnitude[0]=1024;
        gLakituState.shakeMagnitude[1]=512;
        gLakituState.shakeMagnitude[2]=256;
        gLakituState.shakePitchPhase=gLakituState.shakeYawPhase=0x4000;
        gLakituState.shakePitchVel=gLakituState.shakeYawVel=0x100;
        gLakituState.shakePitchDecay=64; gLakituState.shakeYawDecay=32;
        gLakituState.shakeRollVel=0x4000; gLakituState.shakeRollDecay=16;
        Vec3f pos={0}, focus={1200,100,-200};
        geometryCalls=0;
        shake_camera_pitch(pos,focus); shake_camera_yaw(pos,focus);
        s16 roll=123; shake_camera_roll(&roll);
        assert(focus[0]==1200 && focus[1]==100+1024*scale && focus[2]==-200+512*scale);
        assert(roll==123+256*scale && geometryCalls==(mode==1 ? 0 : 4));
        assert(gLakituState.shakePitchPhase==0x4100 && gLakituState.shakeYawPhase==0x4100);
        assert(gLakituState.shakeMagnitude[0]==960 && gLakituState.shakeMagnitude[1]==480);
        assert(gLakituState.shakeRollPhase==0x4000 && gLakituState.shakeMagnitude[2]==240);
        struct GraphNodePerspective perspective={45};
        memset(&sFOVState,0,sizeof(sFOVState));
        sFOVState.shakeAmplitude=1024; sFOVState.shakeSpeed=0x100; sFOVState.decay=64;
        shake_camera_fov(&perspective);
        assert(perspective.fov==45+4*scale && sFOVState.fovOffset==4);
        assert(sFOVState.shakePhase==0x100 && sFOVState.shakeAmplitude==960);
        for (s32 frame=0; frame<40; frame++) {
            shake_camera_pitch(pos,focus); shake_camera_yaw(pos,focus);
            shake_camera_roll(&roll); shake_camera_fov(&perspective);
        }
        assert(!gLakituState.shakeMagnitude[0] && !gLakituState.shakeMagnitude[1]);
        assert(!gLakituState.shakeMagnitude[2] && !gLakituState.shakeRollPhase);
        assert(!sFOVState.shakeAmplitude && !sFOVState.shakePhase);
    }
}
static void handheld(void) {
    Vec3s offsets[3][32];
    struct HandheldShakePoint splines[3][4];
    f32 timers[3]; s32 calls[3];
    for (s32 mode=0; mode<2; mode++) {
        shakeMode=mode; randomCalls=geometryCalls=0;
        memset(sHandheldShakeSpline,0,sizeof(sHandheldShakeSpline));
        sHandheldShakeTimer=0.75f;
        sHandheldShakePitch=128; sHandheldShakeYaw=-256; sHandheldShakeRoll=64;
        for (s32 frame=0; frame<32; frame++) {
            Vec3f pos={0}, focus={1200,100,-200};
            sHandheldShakeMag=300; sHandheldShakeInc=0.5f;
            shake_camera_handheld(pos,focus);
            offsets[mode][frame][0]=sHandheldShakePitch;
            offsets[mode][frame][1]=sHandheldShakeYaw;
            offsets[mode][frame][2]=sHandheldShakeRoll;
            assert(!sHandheldShakeMag && sHandheldShakeInc==0);
            f32 scale=game_options_shake_scale();
            assert(focus[0]==1200 && focus[1]==(s16)(100+sHandheldShakePitch*scale));
            assert(focus[2]==(s16)(-200+sHandheldShakeYaw*scale));
        }
        assert(mode!=1 || !geometryCalls);
        memcpy(splines[mode],sHandheldShakeSpline,sizeof(sHandheldShakeSpline));
        timers[mode]=sHandheldShakeTimer; calls[mode]=randomCalls;
    }
    assert(calls[0]>0);
    for (s32 mode=1; mode<2; mode++) {
        assert(calls[mode]==calls[0] && timers[mode]==timers[0]);
        assert(!memcmp(offsets[0],offsets[mode],sizeof(offsets[0])));
        assert(!memcmp(splines[0],splines[mode],sizeof(splines[0])));
    }
}
static void paused_fov(void) {
    for (s32 mode=0; mode<2; mode++) {
        shakeMode=mode;
        gCameraMovementFlags=0;
        memset(&sFOVState,0,sizeof(sFOVState));
        sFOVState.fov=31;
        sFOVState.fovFunc=CAM_FOV_APP_30;
        sFOVState.shakeAmplitude=1024;
        sFOVState.shakeSpeed=0x100;
        sFOVState.decay=64;
        struct GraphNodePerspective perspective={0};
        fovUpdates=0;
        geo_camera_fov(GEO_CONTEXT_RENDER,(struct GraphNode *)&perspective,NULL);
        assert(fovUpdates==1 && sFOVState.shakePhase==0x100);
        f32 frozenFov=perspective.fov;
        struct CameraFOVStatus frozenState=sFOVState;
        gCameraMovementFlags=CAM_MOVE_PAUSE_SCREEN;
        for (s32 adjusted=0; adjusted<2; adjusted++) {
            pausedCamera=adjusted;
            for (s32 frame=0; frame<20; frame++) {
                geo_camera_fov(GEO_CONTEXT_RENDER,(struct GraphNode *)&perspective,NULL);
                assert(perspective.fov==frozenFov && fovUpdates==1);
                assert(!memcmp(&sFOVState,&frozenState,sizeof(sFOVState)));
            }
        }
        gCameraMovementFlags=0;
        geo_camera_fov(GEO_CONTEXT_RENDER,(struct GraphNode *)&perspective,NULL);
        assert(fovUpdates==2 && sFOVState.shakePhase==0x200);
        // An exhausted shake must not reappear when pausing later.
        sFOVState.shakeAmplitude=0;
        geo_camera_fov(GEO_CONTEXT_RENDER,(struct GraphNode *)&perspective,NULL);
        frozenFov=perspective.fov;
        gCameraMovementFlags=CAM_MOVE_PAUSE_SCREEN;
        geo_camera_fov(GEO_CONTEXT_RENDER,(struct GraphNode *)&perspective,NULL);
        assert(perspective.fov==frozenFov);
    }
    gCameraMovementFlags=0;
}
static void paused_view(void) {
    struct Camera owner={0};
    area.camera=&owner;
    s32 rngBefore=randomCalls;
    gCameraMovementFlags=CAM_MOVE_PAUSE_SCREEN;
    pausedCamera=0; controller.buttonDown=R_CBUTTONS;
    update_paused_camera();
    assert(!owner.updates && !sPausedCameraView.active && !sPausedCameraUsesNormalView);
    pausedCamera=1;
    update_paused_camera();
    assert(!owner.updates && sPausedCameraUsesNormalView);
    gGlobalTimer++; update_paused_camera();
    assert(owner.updates==1 && !cameraDown); // Opening hold is quarantined.
    controller.buttonDown=controller.buttonPressed=0;
    gGlobalTimer++; update_paused_camera();
    controller.buttonDown=R_CBUTTONS|R_TRIG|CONT_A|CONT_START;
    controller.buttonPressed=R_TRIG|CONT_A;
    gGlobalTimer++; update_paused_camera();
    assert(cameraDown==(R_CBUTTONS|R_TRIG) && cameraPressed==R_TRIG);
    assert(controller.buttonDown==(R_CBUTTONS|R_TRIG|CONT_A|CONT_START));
    assert(controller.buttonPressed==(R_TRIG|CONT_A));
    assert(gCameraMovementFlags==(CAM_MOVE_PAUSE_SCREEN|0x10) && !sPausedCameraUpdating);
    s32 updates=owner.updates;
    update_paused_camera(); assert(owner.updates==updates);
    controller.buttonDown=Z_TRIG|U_CBUTTONS; controller.buttonPressed=U_CBUTTONS;
    gGlobalTimer++; update_paused_camera(); assert(cameraDown==U_CBUTTONS && cameraPressed==U_CBUTTONS);
    controller.buttonDown=U_CBUTTONS;
    gGlobalTimer++; update_paused_camera(); assert(cameraDown==U_CBUTTONS && cameraPressed==U_CBUTTONS);
    controller.buttonDown=controller.buttonPressed=0;
    gGlobalTimer++; update_paused_camera();
    controller.buttonDown=controller.buttonPressed=U_CBUTTONS;
    gGlobalTimer++; update_paused_camera(); assert(cameraDown==U_CBUTTONS);
    updates=owner.updates;
    controlsAllowed=0; gGlobalTimer++; update_paused_camera();
    assert(owner.updates==updates && !sPausedCameraView.active);
    controlsAllowed=1;
    for (s32 blocked=0; blocked<4; blocked++) {
        owner.cutscene=blocked==0; sObjectCutscene=blocked==1;
        marioCam.cameraEvent=blocked==2; area.camera=blocked==3 ? NULL : &owner;
        gGlobalTimer++; update_paused_camera();
        assert(owner.updates==updates && !sPausedCameraView.active);
    }
    owner.cutscene=sObjectCutscene=marioCam.cameraEvent=0; area.camera=&owner;
    gCameraMovementFlags=0;
    gGlobalTimer++; update_paused_camera();
    assert(owner.updates==updates && !sPausedCameraUsesNormalView);
    assert(randomCalls==rngBefore);
}
int main(void) { periodic(); handheld(); paused_fov(); paused_view(); return 0; }
'''


class AccessibilityEffects(unittest.TestCase):
    def test_shake_outputs_and_lifecycle(self):
        camera = Source(ROOT / "src/game/camera.c")
        header = Source(ROOT / "src/game/camera.h")
        actual = Source(ROOT / "src/engine/math_util.h").macros("DEGREES")
        actual += Source(ROOT / "include/n64/PR/os_cont.h").macros(
            "CONT_.*|R_TRIG|Z_TRIG|U_CBUTTONS|D_CBUTTONS|L_CBUTTONS|R_CBUTTONS")
        actual += "#define CBUTTON_MASK (U_CBUTTONS|D_CBUTTONS|L_CBUTTONS|R_CBUTTONS)\n"
        actual += header.declaration("struct", "HandheldShakePoint")
        actual += "static struct HandheldShakePoint sHandheldShakeSpline[4];\n"
        actual += header.declaration("struct", "CameraFOVStatus")
        actual += header.declaration("enum", "CameraFov")
        actual += "static struct CameraFOVStatus sFOVState;\n"
        actual += Source(ROOT / "src/game/game_options.c").function("game_options_shake_scale")
        actual += Source(ROOT / "src/engine/math_util.c").function("approach_s16_asymptotic_bool")
        for name in ("evaluate_cubic_spline", "random_vec3s", "camera_approach_s16_symmetric_bool",
                     "camera_approach_f32_symmetric_bool", "increment_shake_offset", "shake_camera_pitch",
                     "shake_camera_yaw", "shake_camera_roll", "shake_camera_handheld", "shake_camera_fov"):
            actual += camera.function(name)
        # Observe base-FOV callback dispatch; their individual curves are separate behavior.
        for name in ("set_fov_45", "set_fov_29", "zoom_fov_30", "fov_default", "set_fov_bbh",
                     "approach_fov_45", "set_fov_30", "approach_fov_20", "approach_fov_80",
                     "approach_fov_30", "approach_fov_60"):
            actual += (f"static void {name}(UNUSED struct MarioState *m) {{ "
                       "fovUpdates++; sFOVState.fov += 1; }\n")
        actual += camera.function("geo_camera_fov")
        actual += r"""
static u16 cameraDown, cameraPressed;
static void update_camera(struct Camera *c) {
    assert(!(gCameraMovementFlags & CAM_MOVE_PAUSE_SCREEN) && sPausedCameraUpdating);
    cameraDown=controller.buttonDown; cameraPressed=controller.buttonPressed;
    c->updates++; gCameraMovementFlags |= 0x10;
}
"""
        actual += camera.region(r"^struct PausedCameraView", r"^void create_camera\(")
        with tempfile.TemporaryDirectory(prefix="accessibility-effects-") as temporary:
            folder = Path(temporary)
            path = folder / "fixture.c"
            path.write_text(PRELUDE + actual + CHECKS)
            executable = folder / "test"
            subprocess.run(["cc", "-std=c99", "-Wall", "-Wextra", "-Werror",
                            "-fsanitize=address,undefined", "-fno-sanitize-recover=all", "-fno-omit-frame-pointer", "-g",
                            str(path), "-lm", "-o", str(executable)], check=True, timeout=30)
            subprocess.run([str(executable)], check=True,
                           env={**os.environ, "ASAN_OPTIONS": "detect_leaks=0"}, timeout=10)


if __name__ == "__main__":
    unittest.main()
