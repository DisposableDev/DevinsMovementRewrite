"""Check interruption of scripted camera effects by debug flight."""
from pathlib import Path
import re
import subprocess
import tempfile
import unittest

from regression_support import Source

ROOT = Path(__file__).resolve().parents[1]


class DebugFlyCameraTests(unittest.TestCase):
    def test_interrupts_effects_without_resetting_view(self):
        source = Source(ROOT / "src/game/camera.c")
        header = (ROOT / "src/game/camera.h").read_text()
        flags = "\n".join(re.search(r"enum " + name + r" \{.*?\};", header, re.S)[0]
            for name in ("CameraMovementFlags", "CameraFlags"))
        fixture = r'''
#include <assert.h>
#include <stddef.h>
#include <string.h>
typedef int s32;
#define ACT_DEBUG_FREE_MOVE 99
#define CUTSCENE_NONE 0
#define CUTSCENE_OBJ_NONE 0
#define CAM_EVENT_NONE 0
#define CAM_FOV_DEFAULT 1
struct Camera {int cutscene,mode; float pos[3];};
static struct {int action;} mario, *gMarioState=&mario;
static struct {int cameraEvent;} marioCam, *sMarioCamState=&marioCam;
static struct {int framesLeft;} sModeTransition;
static struct {int shakeMagnitude[3],roll,keyDanceRoll;} gLakituState;
static struct {float fov,fovOffset,shakeAmplitude; int shakePhase;} sFOVState;
static int sObjectCutscene,gRecentCutscene,gCutsceneTimer,sCutsceneShot,gCutsceneObjSpawn;
static int gCameraMovementFlags,sStatusFlags,sHandheldShakeMag;
static int sHandheldShakePitch,sHandheldShakeYaw,sHandheldShakeRoll,fovFunction;
static void *gCutsceneFocus,*gSecondCameraFocus;
static void set_fov_function(int function) {fovFunction=function;}
'''
        checks = r'''
int main(void) {
    struct Camera c={5,14,{100,200,300}};
    sObjectCutscene=gRecentCutscene=5; gCutsceneTimer=90; sCutsceneShot=2;
    gCutsceneObjSpawn=3; gCutsceneFocus=gSecondCameraFocus=&c;
    marioCam.cameraEvent=6; sModeTransition.framesLeft=60;
    gCameraMovementFlags=CAM_MOVE_FIX_IN_PLACE|CAM_MOVE_ZOOMED_OUT;
    sStatusFlags=CAM_FLAG_BLOCK_SMOOTH_MOVEMENT|CAM_FLAG_BLOCK_AREA_PROCESSING
                 |CAM_FLAG_UNUSED_CUTSCENE_ACTIVE|CAM_FLAG_SMOOTH_MOVEMENT;
    gLakituState.shakeMagnitude[0]=500; gLakituState.shakeMagnitude[1]=400;
    gLakituState.shakeMagnitude[2]=300; gLakituState.roll=gLakituState.keyDanceRoll=1000;
    sHandheldShakeMag=200; sHandheldShakePitch=sHandheldShakeYaw=sHandheldShakeRoll=300;
    fovFunction=3; sFOVState.fov=20; sFOVState.fovOffset=8;
    sFOVState.shakeAmplitude=5; sFOVState.shakePhase=100;
    clear_debug_fly_camera_effects(&c);
    assert(c.cutscene==5 && gCutsceneTimer==90 && sFOVState.fov==20);
    mario.action=ACT_DEBUG_FREE_MOVE;
    clear_debug_fly_camera_effects(&c);
    assert(!c.cutscene && !sObjectCutscene && !gRecentCutscene && !gCutsceneTimer);
    assert(!sCutsceneShot && !gCutsceneObjSpawn && !gCutsceneFocus && !gSecondCameraFocus);
    assert(!marioCam.cameraEvent && !sModeTransition.framesLeft);
    assert(gCameraMovementFlags==CAM_MOVE_ZOOMED_OUT && sStatusFlags==CAM_FLAG_SMOOTH_MOVEMENT);
    for(int i=0;i<3;i++) assert(!gLakituState.shakeMagnitude[i]);
    assert(!gLakituState.roll && !gLakituState.keyDanceRoll && !sHandheldShakeMag);
    assert(!sHandheldShakePitch && !sHandheldShakeYaw && !sHandheldShakeRoll);
    assert(fovFunction==CAM_FOV_DEFAULT && sFOVState.fov==45 && !sFOVState.fovOffset);
    assert(!sFOVState.shakeAmplitude && !sFOVState.shakePhase);
    assert(c.mode==14 && c.pos[0]==100 && c.pos[1]==200 && c.pos[2]==300);
    // A queued effect during flight must also be discarded on the next update.
    c.cutscene=7; sObjectCutscene=7; sFOVState.fov=30;
    clear_debug_fly_camera_effects(&c);
    assert(!c.cutscene && !sObjectCutscene && sFOVState.fov==45);
    return 0;
}
'''
        with tempfile.TemporaryDirectory(prefix="debug-fly-camera-") as directory:
            path = Path(directory)
            (path / "test.c").write_text(fixture + flags + "\n"
                + source.function("clear_debug_fly_camera_effects") + checks)
            subprocess.run(["cc", "-std=gnu99", "-Wall", "-Wextra", "-Werror",
                "-fsanitize=undefined", "-fno-sanitize-recover=all", str(path / "test.c"),
                "-o", str(path / "test")], check=True)
            subprocess.run([str(path / "test")], check=True)
