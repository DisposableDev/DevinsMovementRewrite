"""Production extraction and deliberate defects for the rumble suite."""
from .common import prepare_common_constants
from .mutation_support import MutationSource

def prepare_rumble(root, source, definitions, functions):
    prepare_common_constants(source, definitions)
    definitions += [source("src/game/game_options.h").region(r"^#define GAME_OPTIONS_FILES", r"^#endif")]
    functions += [source("src/game/game_options.c").region(r"^#define OPTIONS_MAGIC", r"\Z")]
    definitions += [source("src/game/mario_he_thicc.h").macros(
        "HE_THICC_.*|ACT_ARG_WALL_KICK_HE_THICC|ACT_ARG_FORWARD_ROLLOUT_HE_THICC_BOUNCE")]
    definitions += [source("src/game/mario_actions_airborne.h").macros(
        "ACT_ARG_JUMP_BACKWARD_BUTT_SLIDE.*|ACT_ARG_JUMP_KICK_FOLLOWUP_QUEUED|ACT_ARG_JUMP_KICK_BACKWARD_BUTT_SLIDE.*|ACT_ARG_STEEP_JUMP_BACKWARD_BUTT_SLIDE_NO_UPHILL|ACT_ARG_DIVE_FROM_ROLLOUT_CONTACT")]
    definitions += [source("src/game/mario_actions_stationary.h").macros(
        "ACT_ARG_COUGHING_SHALLOW_WATER")]
    definitions += [source("include/config/config_movement.h").macros("BACKWARD_BUTT_SLIDE_JUMP_AIR_DRAG|BACKWARD_JUMP_KICK_UPHILL_DECEL")]
    definitions += [source("include/sm64.h").macros("ACT_.*")]
    definitions += [source("include/sm64.h").declaration("enum", name) for name in ("MarioActionGroups", "MarioActionFlags")]
    definitions += [source("src/game/mario_character.h").declaration("enum", "MarioPalette")]
    definitions += [source("src/game/rumble_init.h").declaration("enum", "CharacterRumbleLevel")]
    definitions += [source("src/game/main.h").declaration("struct", name) for name in ("RumbleData", "RumbleSettings")]
    rumble = source("src/game/rumble_init.c")
    functions += [rumble.function(name) for name in (
        "set_character_movement_rumble", "stop_rumble_for_reset", "update_rumble_pak", "cancel_rumble",
        "thread6_rumble_loop",
        "reset_rumble_timers_slip", "queue_rumble_submerged")]
    functions += [source("src/game/mario_step.c").function("mario_is_on_floor")]
    functions += [source("src/game/mario.c").function("mario_has_rumble_contact")]
    functions += [source("src/game/mario.c").function("mario_update_character_rumble")]

def mutations_rumble(root):
    for name, case, path, function, old, new in (
        ("heavy_walk", "rumble.actions", "src/game/mario.c", "mario_update_character_rumble", "? CHARACTER_RUMBLE_MEDIUM_LIGHT", "? CHARACTER_RUMBLE_HEAVY"),
        ("rumble_standard", "rumble.actions", "src/game/mario.c", "mario_update_character_rumble", "mario_character_palette() == MARIO_PALETTE_HE_THICC", "mario_character_palette() == MARIO_PALETTE_STANDARD"),
        ("no_heavy_motor", "rumble.motor", "src/game/rumble_init.c", "update_rumble_pak", "(gNumVblanks % 8) < sCharacterMovementRumbleLevel", "(gNumVblanks % 8) < 0"),
        ("rumble_never_expires", "rumble.lifecycle", "src/game/rumble_init.c", "update_rumble_pak", "sCharacterMovementRumbleVblanks--;", "(void)0;"),
        ("rumble_free_air", "rumble.ground_air_contacts", "src/game/mario.c", "mario_update_character_rumble", "&& (m->action & ACT_GROUP_MASK) != ACT_GROUP_CUTSCENE;", "&& (m->action & ACT_GROUP_MASK) != ACT_GROUP_CUTSCENE && mario_has_rumble_contact(m);"),
        ("rumble_twirl_air", "rumble.ground_air_contacts", "src/game/rumble_init.c", "reset_rumble_timers_slip", "if (gCurrDemoInput != NULL)", "if (gCurrDemoInput != NULL || !mario_has_rumble_contact(gMarioState))"),
        ("rumble_free_swim", "rumble.ground_air_contacts", "src/game/rumble_init.c", "queue_rumble_submerged", "if (gCurrDemoInput != NULL)", "if (gCurrDemoInput != NULL || !mario_has_rumble_contact(gMarioState))"),
    ):
        yield "rumble", name, case, {path: MutationSource(root / path).replace_in_function(function, old, new)}

