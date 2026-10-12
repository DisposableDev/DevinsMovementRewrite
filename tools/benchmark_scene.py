"""GDB-side scene setup. Executed by benchmark.py inside GDB, not directly.

Only the controller and optional fixed camera need frame callbacks. Timing comes
from the existing guest exporter, rather than separate remote reads per timer.
"""

import hashlib
import json
from pathlib import Path
import struct
import subprocess

import gdb


scene = BENCHMARK_SCENE
work = Path(BENCHMARK_WORK)
inferior = gdb.selected_inferior()
age = 0
last_frame = None
warped = False
ready = False
observations = []
events_done = []
sequence_start = None
controller = None
previous_buttons = 0


def value(expression):
    return int(gdb.parse_and_eval(str(expression))) & 0xffffffff


def address(expression):
    return value("&( " + expression + " )")


def read_u32(location):
    return struct.unpack(">I", bytes(inferior.read_memory(location, 4)))[0]


def assign(expression, number):
    if isinstance(number, str):
        number = value(number)
    gdb.execute("set " + expression + " = " + str(number), to_string=True)


def vector(expression, numbers):
    inferior.write_memory(address(expression), struct.pack(">3f", *numbers))


def virtual(location):
    if location >= 0x80000000:
        return location
    base = read_u32(segment_table + 4 * (location >> 24))
    return (base + (location & 0xffffff)) | 0x80000000


level = value(scene["level"])
timer_address = address("gGlobalTimer")
sequence_address = address("sCaptureSequence")
segment_table = address("sSegmentTable")
camera_transform = address("gCameraTransform")
seed_address = address("gRandomSeed16")
object_size = value("sizeof(struct Object)")
symbols = subprocess.check_output(["/opt/n64-gcc-16.2/bin/mips64-elf-nm", "-S",
                                   gdb.current_progspace().filename], text=True)
pool_sizes = [int(fields[1], 16) for line in symbols.splitlines()
              if len(fields := line.split()) == 4 and fields[3] == "gObjectPool"]
if len(pool_sizes) != 1 or pool_sizes[0] % object_size:
    raise RuntimeError("Cannot determine the ELF's object pool capacity")
object_count = pool_sizes[0] // object_size
object_base = address("gObjectPool")
active_offset = value("(unsigned int)&((struct Object *)0)->activeFlags")
behavior_offset = value("(unsigned int)&((struct Object *)0)->behavior")
assert value("(unsigned int)&((struct Controller *)0)->buttonReleased") == 20


def objects_with_behavior(names):
    wanted = {virtual(value("&" + name)): name for name in names}
    matches = {name: [] for name in names}
    data = bytes(inferior.read_memory(object_base, object_size * object_count))
    for index in range(object_count):
        offset = index * object_size
        if not struct.unpack_from(">H", data, offset + active_offset)[0]:
            continue
        behavior = struct.unpack_from(">I", data, offset + behavior_offset)[0]
        if behavior in wanted:
            matches[wanted[behavior]].append(index)
    return matches


def place_mario():
    mario = scene.get("mario", {})
    if "position" in mario:
        vector("gMarioState->pos", mario["position"])
        vector("gMarioObject->rawData.asF32[6]", mario["position"])
        vector("gMarioObject->header.gfx.pos", mario["position"])
    vector("gMarioState->vel", mario.get("velocity", [0, 0, 0]))
    assign("gMarioState->forwardVel", mario.get("forward_speed", 0))
    for field, number in mario.get("set", {}).items():
        assign("gMarioState->" + field, number)


def check_scene():
    if value("gCurrLevelNum") != level or value("gCurrentArea->index") != scene["area"]:
        raise RuntimeError("Scene left the requested level/area")
    if value("sCurrPlayMode") != 0 or value("gDialogID") != 0xffffffff:
        raise RuntimeError("Scene entered a transition, pause or dialog")
    if value("gMarioState->health") < 0x100:
        raise RuntimeError("Mario died during the scene")
    for check in scene.get("checks", []):
        if check.get("frame", age) != age:
            continue
        actual = float(gdb.parse_and_eval(check["expression"]))
        if actual < check.get("min", actual) or actual > check.get("max", actual):
            raise RuntimeError("Scene check failed: " + check["expression"] + " = " + str(actual))


def observe():
    position = struct.unpack(">3f", bytes(inferior.read_memory(address("gMarioState->pos"), 12)))
    camera = struct.unpack(">3f", bytes(inferior.read_memory(address("gLakituState.pos"), 12)))
    counts = {}
    if scene.get("observe_behaviors"):
        counts = {name: len(indices) for name, indices in
                  objects_with_behavior(scene["observe_behaviors"]).items()}
    state = {"age": age, "position": position, "camera": camera,
             "action": value("gMarioState->action"), "flags": value("gMarioState->flags"),
             "health": value("gMarioState->health"), "objects": value("gPrevFrameObjectCount"),
             "rng": struct.unpack(">H", bytes(inferior.read_memory(seed_address, 2)))[0],
             "behaviors": counts}
    observations.append(state)


class SceneBreakpoint(gdb.Breakpoint):
    def __init__(self, symbol):
        super().__init__("*" + symbol, type=gdb.BP_HARDWARE_BREAKPOINT, internal=True)

    def stop(self):
        try:
            return self.step()
        except Exception as error:
            (work / "scene-error.json").write_text(json.dumps(
                {"error": str(error), "age": age, "pc": value("$pc")}, indent=2) + "\n")
            return True


class Route(SceneBreakpoint):
    def step(self):
        global warped
        if not warped and value("$a0") == 1 and value("gCurrentArea"):
            inferior.write_memory(seed_address, struct.pack(">H", scene.get("seed", 0)))
            for expression, number in {"sWarpDest.type": 2, "sWarpDest.levelNum": level,
                                       "sWarpDest.areaIdx": scene["area"],
                                       "sWarpDest.nodeId": scene["node"],
                                       "gCurrActNum": scene.get("act", 1), "sCurrPlayMode": 4,
                                       "sTransitionTimer": 0}.items():
                assign(expression, number)
            warped = True
            self.enabled = False
        return False


class SkipActSelect(SceneBreakpoint):
    def step(self):
        assign("$v0", 1)
        assign("$pc", value("$ra"))
        return False


class Frame(SceneBreakpoint):
    def step(self):
        global age, last_frame, ready, controller, previous_buttons, sequence_start
        if not ready:
            if not warped or value("gCurrLevelNum") != level or not value("gMarioObject"):
                return False
            if value("gMarioState->area") != value("gCurrentArea") or value("sWarpDest.type"):
                return False
            if value("gCurrentArea->index") != scene["area"]:
                return False
            if value("gMarioObject->behavior") != virtual(value("&bhvMario")):
                return False
            controller = value("gMarioState->controller")
            assign("gCurrentArea->dialog[0]", 255)
            assign("gCurrentArea->dialog[1]", 255)
            assign("gDialogID", -1)
            assign("gTimeStopState", 0)
            if gdb.lookup_global_symbol("fDebug") is not None:
                assign("fDebug", int(scene.get("debug_overlay", False)))
            ready = True
        frame = read_u32(timer_address)
        if frame == last_frame:
            return False
        last_frame = frame
        age += 1
        buttons = 0
        stick = [0, 0]
        for entry in scene.get("inputs", []):
            if entry["from"] <= age < entry["until"]:
                stick = entry.get("stick", stick)
                buttons |= entry.get("buttons", 0)
        magnitude = min(64, (stick[0] ** 2 + stick[1] ** 2) ** 0.5)
        payload = struct.pack(">hhfffHHH", *stick, *stick, magnitude, buttons,
                              buttons & ~previous_buttons, previous_buttons & ~buttons)
        inferior.write_memory(controller, payload)
        previous_buttons = buttons
        if age == scene["setup_frame"]:
            for expression, number in scene.get("setup", {}).items():
                assign(expression, number)
        if age == scene["setup_frame"] or (age > scene["setup_frame"] and scene.get("anchor")):
            place_mario()
        for index, event in enumerate(scene.get("events", [])):
            if event["frame"] != age:
                continue
            targets = objects_with_behavior([event["behavior"]])[event["behavior"]]
            if len(targets) != event.get("count", 1):
                raise RuntimeError("Event target count differs: " + event["behavior"])
            for target in targets:
                for field, number in event["set"].items():
                    assign("gObjectPool[" + str(target) + "]." + field, number)
            events_done.append(index)
        if age == scene["sample_start"]:
            # Discard the first export, whose window can straddle warm-up.
            sequence_start = read_u32(sequence_address) + 1
        if age >= scene["setup_frame"] and (age % 15 == 0 or
                any(check.get("frame") == age for check in scene.get("checks", []))):
            check_scene()
        if age >= scene["sample_start"] and age % 15 == 0:
            observe()
        if age % 60 == 0:
            print("BENCHMARK scene=" + scene["name"] + " frame=" + str(age), flush=True)
        if age >= scene["sample_start"] + scene["sample_frames"] + 15:
            finish.enabled = True
        return False


class Camera(SceneBreakpoint):
    def step(self):
        if age >= scene["setup_frame"] and value("$a0") == camera_transform:
            camera = scene["camera"]
            inferior.write_memory(value("$a1"), struct.pack(">3f", *camera["position"]))
            inferior.write_memory(value("$a2"), struct.pack(">3f", *camera["focus"]))
        return False


class Finish(SceneBreakpoint):
    def step(self):
        if age < scene["sample_start"] + scene["sample_frames"] + 15:
            return False
        check_scene()
        if len(events_done) != len(scene.get("events", [])):
            raise RuntimeError("Not all scene events executed")
        index = (value("sRenderingFramebuffer") + 1) % 3
        framebuffer = value("gPhysicalFramebuffers[" + str(index) + "]") | 0x80000000
        pixels = bytes(inferior.read_memory(framebuffer, 320 * 240 * 2))
        (work / "completed-frame.rgba16").write_bytes(pixels)
        (work / "scene-result.json").write_text(json.dumps(
            {"sequence_start": sequence_start, "sequence_end": read_u32(sequence_address),
             "level": level, "observations": observations, "events_done": events_done,
             "completed_buffer_index": index,
             "image_sha256": hashlib.sha256(pixels).hexdigest()}, indent=2) + "\n")
        return True


Route("lvl_init_or_update")
SkipActSelect("lvl_update_obj_and_load_act_button_actions")
Frame("update_objects")
finish = Finish("exec_display_list")
finish.enabled = False
if "camera" in scene:
    Camera("mtxf_lookat")
