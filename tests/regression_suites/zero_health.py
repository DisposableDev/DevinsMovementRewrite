"""Production extraction and deliberate defects for the zero_health suite."""
from .common import prepare_common_constants
from .mutation_support import MutationSource

def prepare_zero_health(root, source, definitions, functions):
    prepare_common_constants(source, definitions)
    # Isolate zero-health policy with debug entry permitted and controlled health
    # restoration. Dedicated options fixtures execute the cheat implementations.
    functions += [
        "static s32 game_options_free_move_active(void) { return TRUE; }",
        "static s32 mario_update_infinite_health(struct MarioState *m) {\n"
        "    if (!sRestoreHealth) {\n"
        "        return FALSE;\n"
        "    }\n"
        "    m->health = 0x880;\n"
        "    return TRUE;\n"
        "}",
        "static void mario_update_forced_caps(struct MarioState *m) { (void) m; }",
        "static void mario_update_environment_cheats(struct MarioState *m) { (void) m; }",
        "static void mario_update_gameplay_options(struct MarioState *m) { (void) m; }",
    ]
    definitions += [source("src/game/mario_he_thicc.h").macros(
        "HE_THICC_.*|ACT_ARG_WALL_KICK_HE_THICC|ACT_ARG_FORWARD_ROLLOUT_HE_THICC_BOUNCE")]
    definitions += [source("src/game/mario_actions_airborne.h").macros(
        "ACT_ARG_JUMP_BACKWARD_BUTT_SLIDE.*|ACT_ARG_JUMP_KICK_FOLLOWUP_QUEUED|ACT_ARG_JUMP_KICK_BACKWARD_BUTT_SLIDE.*|ACT_ARG_STEEP_JUMP_BACKWARD_BUTT_SLIDE_NO_UPHILL|ACT_ARG_DIVE_FROM_ROLLOUT_CONTACT")]
    definitions += [source("src/game/mario_actions_stationary.h").macros(
        "ACT_ARG_COUGHING_SHALLOW_WATER")]
    definitions += [source("include/config/config_movement.h").macros("BACKWARD_BUTT_SLIDE_JUMP_AIR_DRAG|BACKWARD_JUMP_KICK_UPHILL_DECEL")]
    header = source("include/sm64.h")
    definitions += [header.declaration("enum", name) for name in
                    ("MarioInput", "MarioActionGroups", "MarioActionFlags")]
    definitions += [header.macros("ACT_.*")]
    definitions += [source("src/game/level_update.h").declaration("enum", "WarpOperation")]
    definitions += [source("include/types.h").region(r"^typedef struct Controller", r"^typedef u8\s+Bool8;")]
    functions += [source("src/game/interaction.c").function(name) for name in
                  ("mario_stop_riding_object", "mario_stop_riding_and_holding")]
    functions += [source("src/game/mario.c").function(name) for name in
                  ("drop_and_set_mario_action", "mario_block_actions_at_zero_health",
                   "execute_mario_action")]
    functions += [source("src/game/level_update.c").function(name) for name in
                  ("level_is_ending", "pressed_pause")]

def mutations_zero_health(root):
    for name, case, path, function, old, new in (
        ("dead_literal_zero_only", "dead_controls", "src/game/mario.c",
         "mario_block_actions_at_zero_health", "m->health >= 0x100", "m->health > 0"),
        ("dead_keep_jump_buffer", "dead_controls", "src/game/mario.c",
         "mario_block_actions_at_zero_health", "m->landingJumpInputFrames = 0;",
         "m->landingJumpInputFrames = m->landingJumpInputFrames;"),
        ("dead_allow_debug_entry", "dead_controls", "src/game/mario.c",
         "execute_mario_action", "mario_block_actions_at_zero_health(gMarioState, &neutralController);\n"
         "#ifdef ENABLE_DEBUG_FREE_MOVE", "#ifdef ENABLE_DEBUG_FREE_MOVE"),
        ("dead_allow_redispatch", "fatal_redispatch", "src/game/mario.c",
         "execute_mario_action", "while (inLoop) {\n"
         "            mario_block_actions_at_zero_health(gMarioState, &neutralController);",
         "while (inLoop) {"),
        ("dead_allow_contact_shortcut", "fatal_redispatch", "src/game/mario.c",
         "execute_mario_action", "mario_process_interactions(gMarioState);\n"
         "        mario_block_actions_at_zero_health(gMarioState, &neutralController);",
         "mario_process_interactions(gMarioState);"),
        ("dead_leak_controller", "dead_controls", "src/game/mario.c",
         "execute_mario_action", "gMarioState->controller = controller;\n"
         "        return gMarioState->particleFlags;", "return gMarioState->particleFlags;"),
        ("dead_leak_oob_controller", "fatal_redispatch", "src/game/mario.c",
         "execute_mario_action", "gMarioState->controller = controller;\n"
         "            return ACTIVE_PARTICLE_NONE;", "return ACTIVE_PARTICLE_NONE;"),
        ("dead_forbid_pause", "death_pause", "src/game/level_update.c",
         "pressed_pause", "(!intangible || zeroHealth || level_is_ending())", "!intangible"),
    ):
        yield "zero_health", name, "zero_health." + case, {
            path: MutationSource(root / path).replace_in_function(function, old, new)}

