"""GDB-side route for profile_gcc.py; CONFIG is supplied by the host runner."""

import json
from pathlib import Path

import gdb

work = Path(CONFIG["work"])
scene = CONFIG["scene"]
level = {"lab": "LEVEL_PHYSICS_LAB", "jrb-swim": "LEVEL_JRB"}[scene]
age = 0
last_frame = None
warped = False
rows = []


def value(expression):
    return int(gdb.parse_and_eval(expression)) & 0xffffffff


def number(expression):
    return float(gdb.parse_and_eval(expression))


def assign(expression, replacement):
    gdb.execute(f"set variable {expression} = {replacement}", to_string=True)


def virtual(address):
    if address >= 0x80000000:
        return address
    return value(f"sSegmentTable[{address >> 24}]") + (address & 0xffffff) | 0x80000000


class Route(gdb.Breakpoint):
    def stop(self):
        global warped
        if not warped and value("$a0") == 1 and value("gCurrentArea"):
            for expression, replacement in [
                ("sWarpDest.type", 2), ("sWarpDest.levelNum", value(f"(int){level}")),
                ("sWarpDest.areaIdx", 1), ("sWarpDest.nodeId", 10),
                ("gCurrActNum", 1), ("sCurrPlayMode", 4), ("sTransitionTimer", 0),
            ]:
                assign(expression, replacement)
            warped = True
        return False


class ActSelection(gdb.Breakpoint):
    def stop(self):
        # Use act 1 without waiting for the star-selection screen.
        assign("$v0", 1)
        assign("$pc", value("$ra"))
        return False


class Frame(gdb.Breakpoint):
    def stop(self):
        global age, last_frame
        if value("gGccProfileDumpState") == 2:
            (work / "route.json").write_text(json.dumps({"scene": scene, "rows": rows,
                "counter_reset_age": 90, "export_age": 255}, indent=2) + "\n")
            return True
        if value("gCurrLevelNum") != value(f"(int){level}") or not value("gMarioObject"):
            return False
        if (value("gCurrentArea->index") != 1
                or value("gMarioState->area") != value("gCurrentArea")
                or value("sWarpDest.type") != 0):
            return False
        if value("gMarioObject->behavior") != virtual(value("&bhvMario")):
            return False
        frame = value("gGlobalTimer")
        if frame == last_frame:
            return False
        last_frame = frame
        age += 1
        controller = "gMarioState->controller->"
        for field in ["rawStickX", "rawStickY", "stickX", "stickY", "stickMag",
                      "buttonDown", "buttonPressed", "buttonReleased"]:
            assign(controller + field, 0)
        if age == 1:
            assign("gCurrentArea->dialog[0]", 255)
            assign("gCurrentArea->dialog[1]", 255)
            assign("gRandomSeed16", 0)
        if age == 60 and scene == "jrb-swim":
            for axis, position in enumerate([0, 600, 5000]):
                assign(f"gMarioState->pos[{axis}]", position)
                assign(f"gMarioObject->rawData.asF32[{6 + axis}]", position)
                assign(f"gMarioObject->header.gfx.pos[{axis}]", position)
            assign("gMarioState->action", CONFIG["water_idle"])
        if scene == "lab" and 90 <= age < 210:
            for field in ["stickY", "rawStickY", "stickMag"]:
                assign(controller + field, 32)
        if age == 90:
            # Reset only arc counters in this frozen ELF, after route warm-up.
            inferior = gdb.selected_inferior()
            for address, size in CONFIG["counters"]:
                inferior.write_memory(address, bytes(size))
        if 90 <= age <= 255:
            rows.append({"age": age, "frame": frame, "level": value("gCurrLevelNum"),
                         "area": value("gCurrentArea->index"),
                         "action": value("gMarioState->action"),
                         "position": [number(f"gMarioState->pos[{axis}]") for axis in range(3)],
                         "floor_height": number("gMarioState->floorHeight"),
                         "water_level": number("gMarioState->waterLevel"),
                         "forward_velocity": number("gMarioState->forwardVel")})
        if age % 30 == 0:
            (work / "progress.json").write_text(json.dumps({"age": age, "frame": frame}) + "\n")
        if age == 255:
            # The game thread performs serialization on its next loop, with its
            # ordinary GP, stack and allocator context. No debugger function call.
            assign("gGccProfileDumpState", 1)
        return False


Route("*lvl_init_or_update", type=gdb.BP_HARDWARE_BREAKPOINT)
ActSelection("*lvl_update_obj_and_load_act_button_actions", type=gdb.BP_HARDWARE_BREAKPOINT)
Frame("*update_objects", type=gdb.BP_HARDWARE_BREAKPOINT)
