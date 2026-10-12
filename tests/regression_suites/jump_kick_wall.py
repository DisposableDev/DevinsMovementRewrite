"""Production extraction and deliberate defects for the jump_kick_wall suite."""
from .common import prepare_common_constants
from .mutation_support import MutationSource

def prepare_jump_kick_wall(root, source, definitions, functions):
    prepare_common_constants(source, definitions)
    definitions += [source("include/sm64.h").macros("ACT_GROUND_POUND")]
    definitions += [source("src/game/mario_actions_airborne.h").macros("JUMP_KICK_(CANCEL|BUFFER)_FRAMES")]
    functions += [source("src/game/mario_actions_airborne.c").function("mario_jump_kick_can_cancel")]
    functions += [
        "void mario_he_thicc_reset(void) {}",
        "s32 mario_he_thicc_try_belly_landing(struct MarioState *m) { return FALSE; }",
        "s32 mario_he_thicc_try_pound_bounce(struct MarioState *m) { return FALSE; }",
        "void mario_he_thicc_wall_kick(struct MarioState *m) {}",
        "void mario_he_thicc_slide_kick_bounce(struct MarioState *m) {}",
    ]
    definitions += [source("src/game/mario_he_thicc.h").macros(
        "HE_THICC_.*|ACT_ARG_WALL_KICK_HE_THICC|ACT_ARG_FORWARD_ROLLOUT_HE_THICC_BOUNCE")]
    definitions += [source("src/game/mario_actions_airborne.h").macros(
        "ACT_ARG_JUMP_BACKWARD_BUTT_SLIDE.*|ACT_ARG_JUMP_KICK_FOLLOWUP_QUEUED|ACT_ARG_JUMP_KICK_BACKWARD_BUTT_SLIDE.*|ACT_ARG_STEEP_JUMP_BACKWARD_BUTT_SLIDE_NO_UPHILL|ACT_ARG_DIVE_FROM_ROLLOUT_CONTACT")]
    definitions += [source("src/game/mario_actions_stationary.h").macros(
        "ACT_ARG_COUGHING_SHALLOW_WATER")]
    definitions += [source("include/config/config_movement.h").macros("BACKWARD_BUTT_SLIDE_JUMP_AIR_DRAG|BACKWARD_JUMP_KICK_UPHILL_DECEL")]
    definitions += [source("src/game/interaction.h").declaration("enum", "Interactions")]
    air = source("src/game/mario_actions_airborne.c")
    interaction = source("src/game/interaction.c")
    definitions += [source("src/game/mario_actions_airborne.h").declaration("enum", "ActionStatesJumpKick"),
                    air.macros("JUMP_KICK_(ACTIVE|FOLLOWUP|BUFFER)_FRAMES")]
    functions += [interaction.region(r"^#define MAX_PUNCH_HIT_OBJECTS\b",
                                     r"^static struct Object \*sKnockbackSourceObj;")]
    functions += [interaction.function(name) for name in (
        "mario_set_punch_hit_phase", "mario_reset_attack_hit_record",
        "same_punch_wall", "remember_punch_hit_wall", "mario_forget_attack_object",
        "punch_hit_object", "remember_punch_hit_object", "punch_hit_wall", "he_thicc_attack_contact", "check_kick_or_punch_wall")]
    functions += [air.function("act_jump_kick")]

def mutations_jump_kick_wall(root):
    path = "src/game/interaction.c"
    yield "jump_kick_wall", "missing_kick_wall_blast", "jump_kick_wall.contact_blast", {
        path: MutationSource(root / path).replace_in_function("check_kick_or_punch_wall",
            "he_thicc_attack_contact(m, INT_KICK);", "(void) he_thicc_attack_contact;")}
    yield "jump_kick_wall", "repeat_same_wall_hit", "jump_kick_wall.same_wall", {
        path: MutationSource(root / path).replace_in_function("check_kick_or_punch_wall",
            "|| ((m->flags & MARIO_KICKING) && m->action == ACT_JUMP_KICK)", "")}
    path = "src/game/mario_actions_airborne.c"
    yield "jump_kick_wall", "reuse_previous_kick_record", "jump_kick_wall.followup", {
        path: MutationSource(root / path).replace_in_function("act_jump_kick",
            "mario_reset_attack_hit_record();", "(void)0;")}

