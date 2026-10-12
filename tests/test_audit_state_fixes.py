#!/usr/bin/env python3
"""Focused fork-audit save/statistics regressions using production units and entry points.

Host adapters control storage, dialogue completion and movement contacts. These tests
do not exercise ROM timing, rendering, audio or emulator save hardware.
"""

from pathlib import Path
import re
import subprocess
import tempfile
import unittest

from regression_support import Source
from run_regressions import prepare
from test_player_stats import prepare_fixture

ROOT = Path(__file__).resolve().parents[1]
STATS_DRIVER = """#define main inherited_stats_main
#include "tests/player_stats_test.c"
#undef main
"""


def compile_and_run(folder, code, stats=True):
    driver = folder / "audit.c"
    driver.write_text(code)
    command = ["cc", "-std=c11", "-Wall", "-Wextra", "-Werror",
               "-Wno-unused-parameter", "-fsanitize=undefined", "-I", str(folder),
               "-I", str(ROOT), "-I", str(ROOT / "tests")]
    if stats:
        command += [str(folder / name) for name in
                    ("player_stats.c", "player_stats_tracking.c", "behaviors.c")]
    command += [str(driver), "-lm", "-o", str(folder / "audit")]
    result = subprocess.run(command, capture_output=True, text=True, timeout=30)
    if result.returncode:
        raise AssertionError(result.stdout + result.stderr)
    result = subprocess.run([str(folder / "audit")], capture_output=True, text=True, timeout=10)
    if result.returncode:
        raise AssertionError(result.stdout + result.stderr)


class AuditStateFixesTests(unittest.TestCase):
    def test_star_interaction_preserves_pending_damage_classification(self):
        with tempfile.TemporaryDirectory(prefix="audit-star-pending-damage-") as directory:
            folder = Path(directory)
            prepare_fixture(folder)
            world = folder / "stats_test_world.h"
            text = world.read_text().replace("struct Object {", """struct Object {
struct { struct { Vec3f cameraToObject; } gfx; } header;
u32 oBehParams, oInteractStatus;
""")
            text = text.replace("u8 hurtCounter, healCounter;", """u8 hurtCounter, healCounter, respawnHealCounter;
u16 capTimer;
s16 numCoins, numStars;
struct Object *marioObj, *interactObj;
""")
            world.write_text(text)
            services = r'''
#define UNUSED __attribute__((unused))
#define POWER_STARS_HEAL
#define INT_STATUS_INTERACTED 1
#define SOUND_MENU_STAR_SOUND 0
const u32 bhvStarKeyCollectionPuffSpawner[]={0};
u32 gStarModelLastCollected;
static void mario_stop_riding_and_holding(struct MarioState *m) {(void)m;}
static void queue_rumble_data(s32 x,s32 y) {(void)x;(void)y;}
static s32 mario_is_head_underwater(struct MarioState *m) {(void)m;return 0;}
static struct Object *spawn_object(struct Object *obj,s32 model,const u32 *behavior) {
    (void)model;(void)behavior;return obj;
}
static s32 obj_get_model_id(struct Object *obj) {return obj->model;}
static void game_timing_runtime_star(struct Object *obj,u32 star) {(void)obj;(void)star;}
static void save_file_collect_star_or_key(s16 coins,s16 star) {(void)coins;(void)star;}
static s32 save_file_get_total_star_count(s32 file,s32 low,s32 high) {
    (void)file;(void)low;(void)high;return 1;
}
static void drop_queued_background_music(void) {}
static void fadeout_level_music(s32 frames) {(void)frames;}
static void play_sound(s32 sound,const f32 *pos) {(void)sound;(void)pos;}
static void update_mario_sound_and_camera(struct MarioState *m) {(void)m;}
static void save_file_mark_game_completed(void) {}
static s32 set_mario_action(struct MarioState *m,u32 action,u32 arg) {
    m->action=action;m->actionArg=arg;return 1;
}
'''
            interaction = Source(ROOT / "src/game/interaction.c").region(
                r"^u32 interact_star_or_key\(", r"^u32 interact_bbh_entrance\(")
            checks = r'''
static void fresh_visit(void) {
    setup();
    player_stats_warp(LEVEL_WF,TRUE);
    player_stats_level_entered();
    player_stats_game_update();
    memset(&gObjectPool[0],0,sizeof(gObjectPool[0]));
    gObjectPool[0].model=MODEL_STAR;
    sMario.marioObj=&gObjectPool[1];
}
int main(void) {
    const struct {u8 hurt,heal,respawn;u32 subtype;u32 star;u64 expected;} cases[]={
        {0,0,0,0,0,1},{1,0,0,0,0,0},{1,31,0,0,0,0},{0,31,0,0,0,1},
        {0,31,31,0,0,1},{1,31,31,0,0,0},
        {1,0,0,INT_SUBTYPE_NO_EXIT,STAR_BP_ACT_100_COINS,0},
    };
    for(u32 i=0;i<sizeof(cases)/sizeof(cases[0]);i++) {
        fresh_visit();
        sMario.hurtCounter=cases[i].hurt;sMario.healCounter=cases[i].heal;
        sMario.respawnHealCounter=cases[i].respawn;
        gObjectPool[0].oInteractionSubtype=cases[i].subtype;
        gObjectPool[0].oBehParams=cases[i].star<<24;
        assert(interact_star_or_key(&sMario,0,&gObjectPool[0]));
        assert(value(PS_STARS_COLLECTED_WITHOUT_DAMAGE)==cases[i].expected);
        assert(value(PS_TOTAL_POWER_STARS_COLLECTED)==1);
        assert(value(PS_100_COIN_STARS_COLLECTED)==(cases[i].star==STAR_BP_ACT_100_COINS));
        assert(sMario.hurtCounter==0 && sMario.healCounter==31 && !sMario.respawnHealCounter);
        assert(sMario.health==0x880);
        assert(sMario.action==(cases[i].subtype & INT_SUBTYPE_NO_EXIT
                                  ? ACT_STAR_DANCE_NO_EXIT : ACT_STAR_DANCE_EXIT));
        assert(player_stats_checkpoint(0));
        player_stats_init(read_storage,write_storage);player_stats_select_file(0);
        assert(value(PS_STARS_COLLECTED_WITHOUT_DAMAGE)==cases[i].expected);
    }
    fresh_visit();player_stats_health(0x880,0x840);sMario.healCounter=31;
    assert(interact_star_or_key(&sMario,0,&gObjectPool[0]));
    assert(!value(PS_STARS_COLLECTED_WITHOUT_DAMAGE));
    fresh_visit();gObjectPool[0].model=MODEL_BOWSER_KEY;
    assert(interact_star_or_key(&sMario,0,&gObjectPool[0]));
    assert(!value(PS_TOTAL_POWER_STARS_COLLECTED));
    fresh_visit();gObjectPool[0].oInteractionSubtype=INT_SUBTYPE_GRAND_STAR;
    assert(interact_star_or_key(&sMario,0,&gObjectPool[0]));
    assert(!value(PS_TOTAL_POWER_STARS_COLLECTED));
    fresh_visit();sMario.health=0xFF;
    assert(!interact_star_or_key(&sMario,0,&gObjectPool[0]));
    assert(!value(PS_TOTAL_POWER_STARS_COLLECTED));
    return 0;
}
'''
            compile_and_run(folder, STATS_DRIVER + services + interaction + checks)

    def test_ground_pound_landing_retains_defeat_method(self):
        with tempfile.TemporaryDirectory(prefix="audit-pound-landing-credit-") as directory:
            folder = Path(directory)
            prepare_fixture(folder)
            compile_and_run(folder, STATS_DRIVER + r"""
int main(void) {
    const u32 actions[] = {ACT_GROUND_POUND, ACT_GROUND_POUND_LAND,
                           ACT_TWIRLING, ACT_TWIRL_LAND, ACT_WATER_DOWN_BURST,
                           ACT_FREEFALL_LAND, ACT_METAL_WATER_FALL_LAND};
    for (unsigned i = 0; i < sizeof(actions) / sizeof(actions[0]); i++) {
        setup(); struct Object *enemy = &gObjectPool[0];
        *enemy = (struct Object){.behavior = bhvGoomba, .oHealth = 1};
        player_stats_object_reset(enemy);
        sMario.action = actions[i];
        player_stats_attack(enemy, ATTACK_GROUND_POUND_OR_TWIRL);
        assert(value(PS_TOTAL_ENEMIES_DEFEATED) == 0);
        enemy->oHealth = 0;
        player_stats_enemy_defeated(enemy); player_stats_enemy_defeated(enemy);
        assert(value(PS_TOTAL_ENEMIES_DEFEATED) == 1);
        assert(value(PS_ENEMIES_GROUND_POUNDED) == (i < 2));
        assert(value(PS_ENEMIES_STOMPED) == 0);
    }
    // The attack classification and riding-shell priority still control the method.
    setup(); struct Object *enemy = &gObjectPool[0];
    *enemy = (struct Object){.behavior = bhvGoomba, .oHealth = 1};
    player_stats_object_reset(enemy); sMario.action = ACT_GROUND_POUND_LAND;
    player_stats_attack(enemy, ATTACK_FROM_ABOVE); enemy->oHealth = 0;
    player_stats_enemy_defeated(enemy);
    assert(value(PS_ENEMIES_STOMPED) == 1 && value(PS_ENEMIES_GROUND_POUNDED) == 0);
    setup(); *enemy = (struct Object){.behavior = bhvGoomba, .oHealth = 1};
    player_stats_object_reset(enemy); sMario.action = ACT_GROUND_POUND_LAND;
    sMario.riddenObj = &gObjectPool[1]; player_stats_attack(enemy, ATTACK_GROUND_POUND_OR_TWIRL);
    enemy->oHealth = 0; player_stats_enemy_defeated(enemy);
    assert(value(PS_ENEMIES_DEFEATED_WHILE_RIDING_SHELLS) == 1);
    assert(value(PS_ENEMIES_GROUND_POUNDED) == 0);
    return 0;
}
""")

    def test_bully_collision_preserves_active_attack_credit(self):
        with tempfile.TemporaryDirectory(prefix="audit-bully-credit-") as directory:
            folder = Path(directory)
            prepare_fixture(folder)
            world = folder / "stats_test_world.h"
            text = world.read_text()
            marker = "struct Object {\n"
            self.assertEqual(text.count(marker), 1)
            world.write_text(text.replace(marker, marker + """
u32 oInteractStatus, oFlags;
s32 oBehParams2ndByte, oMoveAngleYaw, oBullyMarioCollisionAngle;
""", 1))
            code = STATS_DRIVER
            code += Source(ROOT / "src/game/interaction.h").declaration("enum", "InteractStatus")
            code += r"""
#define OBJ_FLAG_SET_FACE_YAW_TO_MOVE_YAW 1
#define SOUND_OBJ2_SMALL_BULLY_ATTACKED 1
#define SOUND_OBJ2_LARGE_BULLY_ATTACKED 2
static struct Object *o;
static void cur_obj_play_sound_2(s32 sound) { assert(sound == 1 || sound == 2); }
static void cur_obj_init_animation(s32 animation) { assert(animation == 3); }
"""
            code += Source(ROOT / "src/game/behaviors/bully.inc.c").function(
                "bully_check_mario_collision")
            code += r"""
int main(void) {
    const void *variants[] = {bhvSmallBully, bhvBigBully, bhvBigBullyWithMinions,
                             bhvSmallChillBully, bhvBigChillBully};
    for (int variant = 0; variant < 5; variant++) {
        for (int method = 0; method < 4; method++) {
            for (int passive = 0; passive < 2; passive++) {
                setup(); o = &gObjectPool[0]; *o = (struct Object){0}; player_stats_object_reset(o);
                o->behavior = variants[variant]; o->oHealth = 1;
                o->oBehParams2ndByte = variant == 0 || variant == 3
                    ? BULLY_BP_SIZE_SMALL : BULLY_BP_SIZE_BIG;
                sMario.action = method == 0 ? ACT_PUNCHING : method == 1 ? ACT_GROUND_POUND
                               : method == 2 ? ACT_PUNCHING : ACT_RIDING_SHELL_GROUND;
                sMario.actionArg = method == 2 ? ACT_ARG_PUNCH_SEQUENCE_BREAKDANCE : 0;
                s32 attack = method == 0 ? ATTACK_PUNCH : method == 1 ? ATTACK_GROUND_POUND_OR_TWIRL
                             : method == 2 ? ATTACK_KICK_OR_TRIP : ATTACK_FAST_ATTACK;
                player_stats_attack(o, attack);
                o->oInteractStatus = INT_STATUS_INTERACTED | INT_STATUS_WAS_ATTACKED | attack;
                bully_check_mario_collision(); assert(o->oAction == BULLY_ACT_KNOCKBACK);
                assert(value(PS_TOTAL_ENEMIES_DEFEATED) == 0);
                if (passive) {
                    // A later ordinary body push has no active attack, even after an earlier hit.
                    sMario.action = ACT_IDLE; sMario.actionArg = 0;
                    o->oInteractStatus |= INT_STATUS_INTERACTED; bully_check_mario_collision();
                }
                o->oAction = OBJ_ACT_LAVA_DEATH; o->oHealth = 0;
                player_stats_enemy_defeated(o); player_stats_enemy_defeated(o);
                assert(value(PS_TOTAL_ENEMIES_DEFEATED) == 1);
                assert(value(PS_BULLIES_KNOCKED_INTO_LAVA) == 1);
                assert(value(PS_ENEMIES_PUNCHED) == (!passive && method == 0));
                assert(value(PS_ENEMIES_GROUND_POUNDED) == (!passive && method == 1));
                assert(value(PS_BREAKDANCE_DEFEATS) == (!passive && method == 2));
                assert(value(PS_ENEMIES_DEFEATED_WHILE_RIDING_SHELLS) == (!passive && method == 3));
            }
        }
    }
    return 0;
}
"""
            compile_and_run(folder, code)

    def test_wiggler_final_dialogue_preserves_attack_credit(self):
        with tempfile.TemporaryDirectory(prefix="audit-wiggler-credit-") as directory:
            folder = Path(directory)
            prepare_fixture(folder)
            world = folder / "stats_test_world.h"
            text = world.read_text()
            marker = "struct Object {\n"
            self.assertEqual(text.count(marker), 1)
            text = text.replace(marker, marker + """
s32 oTimer, oFaceAngleYaw, oMoveAngleYaw;
f32 oWigglerSquishSpeed, oForwardVel, oVelY;
struct { struct { f32 scale[3]; } gfx; } header;
""", 1)
            world.write_text(text)
            driver = """#define mario_ground_twirl_is_immune_enemy inherited_immune_enemy
""" + STATS_DRIVER + r"""
#undef mario_ground_twirl_is_immune_enemy
static const u32 bhvWigglerHead[] = {999};
s32 mario_ground_twirl_is_immune_enemy(struct Object *obj) {
    return inherited_immune_enemy(obj) || obj_has_behavior(obj, bhvWigglerHead);
}
#define MARIO_DIALOG_LOOK_UP 1
#define DIALOG_FLAG_NONE 0
#define CUTSCENE_DIALOG 1
#define SOUND_OBJ_WIGGLER_JUMP 1
static struct Object *o;
static int sWigglerHitbox, dialogueFinished;
static s32 approach_f32_ptr(f32 *value, f32 target, f32 step) {
    (void) step; *value = target; return TRUE;
}
static s32 cur_obj_update_dialog_with_cutscene(s32 gaze, s32 flags, s32 scene, s32 dialog) {
    assert(gaze == MARIO_DIALOG_LOOK_UP && flags == 0 && scene == CUTSCENE_DIALOG);
    assert(dialog == DIALOG_152); return dialogueFinished;
}
static void cur_obj_become_intangible(void) {}
static void cur_obj_play_sound_2(s32 sound) { (void) sound; }
static void obj_check_attacks(void *hitbox, s32 action) { (void) hitbox; (void) action; }
"""
            driver += Source(ROOT / "src/game/behaviors/wiggler.inc.c").function(
                "wiggler_act_jumped_on")
            driver += r"""
int main(void) {
    for (int method = 0; method < 3; method++) {
        for (int slow = 0; slow < 2; slow++) {
            setup(); o = &gObjectPool[0]; player_stats_object_reset(o);
            o->behavior = bhvWigglerHead; o->oHealth = 2;
            o->oAction = WIGGLER_ACT_JUMPED_ON; o->oTimer = 31;
            o->header.gfx.scale[1] = 4;
            sMario.action = method == 0 ? ACT_GROUND_POUND
                            : method == 1 ? ACT_FREEFALL : ACT_GROUND_TWIRL;
            player_stats_attack(o, method == 1 ? ATTACK_FROM_ABOVE
                                               : ATTACK_GROUND_POUND_OR_TWIRL);
            sMario.action = ACT_READING_NPC_DIALOG; sDialog = DIALOG_000;
            dialogueFinished = FALSE;
            for (int frame = 0; frame < (slow ? 601 : 1); frame++) {
                player_stats_game_update(); wiggler_act_jumped_on();
                assert(o->oHealth == 2 && value(PS_TOTAL_ENEMIES_DEFEATED) == 0);
            }
            dialogueFinished = TRUE; wiggler_act_jumped_on();
            assert(o->oHealth == 1 && o->oAction == WIGGLER_ACT_SHRINK);
            assert(value(PS_TOTAL_ENEMIES_DEFEATED) == 1);
            assert(value(PS_ENEMIES_GROUND_POUNDED) == (method == 0));
            assert(value(PS_ENEMIES_STOMPED) == (method == 1));
            player_stats_enemy_defeated(o); assert(value(PS_TOTAL_ENEMIES_DEFEATED) == 1);
        }
    }
    // The scripted follow-up cannot manufacture credit, including after cheating.
    for (int cheated = 0; cheated < 2; cheated++) {
        setup(); o = &gObjectPool[0]; player_stats_object_reset(o);
        o->behavior = bhvWigglerHead; o->oHealth = 2;
        o->oAction = WIGGLER_ACT_JUMPED_ON; o->oTimer = 31;
        o->header.gfx.scale[1] = 4;
        if (cheated) {
            sMario.action = ACT_GROUND_POUND; player_stats_attack(o, ATTACK_GROUND_POUND_OR_TWIRL);
            sCheating = TRUE; player_stats_game_update(); sCheating = FALSE;
            player_stats_game_update();
        }
        sMario.action = ACT_READING_NPC_DIALOG; dialogueFinished = TRUE;
        wiggler_act_jumped_on(); assert(value(PS_TOTAL_ENEMIES_DEFEATED) == 0);
    }
    // Ordinary expired environmental credit still does not count.
    setup(); o = &gObjectPool[0]; player_stats_object_reset(o);
    o->behavior = bhvGoomba; o->oHealth = 1; player_stats_attack(o, ATTACK_PUNCH);
    for (int frame = 0; frame < 601; frame++) player_stats_game_update();
    o->oHealth = 0; player_stats_enemy_defeated(o);
    assert(value(PS_TOTAL_ENEMIES_DEFEATED) == 0);
    return 0;
}
"""
            compile_and_run(folder, driver)

    def test_pending_healing_can_rescue_a_survived_fall(self):
        with tempfile.TemporaryDirectory(prefix="audit-fall-healing-") as directory:
            folder = Path(directory)
            prepare_fixture(folder)
            compile_and_run(folder, STATS_DRIVER + r"""
int main(void) {
    setup();
    sMario.pos[1] = 500;
    action(ACT_FREEFALL, 0);
    sMario.pos[1] = -1000;
    sMario.health = 0x180;
    sMario.hurtCounter = 3;
    sMario.healCounter = 3;
    player_stats_fall_survived(&sMario);
    assert(value(PS_GREATEST_FALL_SURVIVED) == 1500 * 256);
    return 0;
}
""")

    def test_erase_and_copy_replace_both_histories_after_retry(self):
        with tempfile.TemporaryDirectory(prefix="audit-history-") as directory:
            folder = Path(directory)
            prepare_fixture(folder)
            compile_and_run(folder, STATS_DRIVER + r"""
static s32 writesUntilFailure, failReadback;
static s32 fault_write(u32 offset, const void *data, u32 size) {
    if (writesUntilFailure == 0) {
        writesUntilFailure = -1;
        return 1;
    }
    if (writesUntilFailure > 0) writesUntilFailure--;
    return write_storage(offset, data, size);
}
static s32 fault_read(u32 offset, void *data, u32 size) {
    if (failReadback) { failReadback = 0; return 1; }
    return read_storage(offset, data, size);
}
static void seed(void) {
    setup();
    player_stats_add(PS_LONG_JUMPS, 8); assert(player_stats_checkpoint(0));
    player_stats_add(PS_LONG_JUMPS, 1); assert(player_stats_checkpoint(0));
    player_stats_select_file(1);
    player_stats_add(PS_LONG_JUMPS, 16); assert(player_stats_checkpoint(1));
    player_stats_add(PS_LONG_JUMPS, 1); assert(player_stats_checkpoint(1));
    writesUntilFailure = -1; failReadback = 0;
    player_stats_init(fault_read, fault_write);
}
int main(void) {
    // First write, second write, and verification failures each leave a retry obligation.
    for (s32 operation = 0; operation < 2; operation++) {
        for (s32 fault = 0; fault < 3; fault++) {
            for (s32 damagedCopy = 0; damagedCopy < 2; damagedCopy++) {
                seed();
                s32 file = operation ? 1 : 0;
                player_stats_select_file(file);
                writesUntilFailure = fault < 2 ? fault : -1;
                failReadback = fault == 2;
                if (operation) player_stats_copy(0, file);
                else player_stats_erase(file);
                assert(player_stats_is_dirty(file));
                // Legitimate play between the failed operation and retry is retained in both copies.
                player_stats_add(PS_LONG_JUMPS, 2);
                assert(player_stats_checkpoint(file));
                assert(!player_stats_is_dirty(file));
                u64 expected = operation ? 11 : 2;
                assert(gPlayerStats[file].values[PS_LONG_JUMPS] == expected);
                assert(gPlayerStats[1 - file].values[PS_LONG_JUMPS] == (operation ? 9 : 17));
                sStorage[PLAYER_STATS_SRAM_OFFSET
                         + (file * 2 + damagedCopy) * PLAYER_STATS_SAVE_STRIDE + 50] ^= 1;
                player_stats_init(read_storage, write_storage);
                // Losing the newest journal may lose later gameplay, but never restore old history.
                u64 surviving = gPlayerStats[file].values[PS_LONG_JUMPS];
                assert(surviving == expected || surviving == (operation ? 9u : 0u));
                assert(gPlayerStats[1 - file].values[PS_LONG_JUMPS] == (operation ? 9 : 17));
            }
        }
    }
    // Ordinary checkpoints still alternate one write rather than replacing both histories.
    seed(); player_stats_select_file(0); player_stats_add(PS_LONG_JUMPS, 1);
    s32 before = sWrites;
    assert(player_stats_checkpoint(0)); assert(sWrites == before + 1);
    return 0;
}
""")

    def test_unlock_drowning_and_actual_held_swimming_strokes(self):
        with tempfile.TemporaryDirectory(prefix="audit-held-swim-") as directory:
            folder = Path(directory)
            prepare_fixture(folder)
            world = (folder / "stats_test_world.h").read_text().replace(
                "s32 oDamageOrCoinValue;", "u32 oInteractStatus; struct {struct {f32 cameraToObject[3];}gfx;}header;\ns32 oDamageOrCoinValue;").replace(
                "u32 action;", "u32 action,actionState,actionTimer,input,particleFlags; f32 forwardVel; struct Object *marioObj;")
            extra = Source(ROOT / "include/sm64.h").declaration("enum", "MarioInput")
            extra += Source(ROOT / "src/game/mario_actions_submerged.h").declaration(
                "enum", "ActionStatesBreaststroke")
            extra += """#define SWIM_DRAG 0.5f
#define SOUND_ACTION_SWIM 1
#define MARIO_ANIM_SWIM_WITH_OBJ_PART1 1
#define PARTICLE_VANISH_WATER_RING 1
#define INT_STATUS_MARIO_DROP_OBJECT 1
"""
            (folder / "stats_test_world.h").write_text(world.replace("#endif\n", extra + "#endif\n"))
            code = STATS_DRIVER + r"""
static s32 set_mario_action(struct MarioState *m, u32 a, u32 arg) {
    player_stats_action(m, a, arg);
    m->action = a; m->actionArg = arg; m->actionTimer = m->actionState = 0; return TRUE;
}
static s32 drop_and_set_mario_action(struct MarioState *m,u32 a,u32 arg) {return set_mario_action(m,a,arg);}
static s32 check_water_jump(struct MarioState *m) {(void)m;return FALSE;}
static void set_anim_to_frame(struct MarioState *m,s16 f) {(void)m;(void)f;}
static void play_sound(s32 snd,f32 *p) {(void)snd;(void)p;}
static void reset_bob_variables(struct MarioState *m) {(void)m;}
static void set_mario_animation(struct MarioState *m,s32 a) {(void)m;(void)a;}
static void common_swimming_step(struct MarioState *m,s32 s,f32 d) {(void)m;(void)s;(void)d;}
"""
            he = Source(ROOT / "src/game/mario_he_thicc.c")
            code += he.function("he_thicc_is_active") + he.function("mario_he_thicc_breaststroke")
            code += Source(ROOT / "src/game/mario_actions_submerged.c").function("act_hold_breaststroke")
            code += r"""
int main(void) {
    setup();
    action(ACT_DAB, 0);
    action(ACT_QUICKSAND_DEATH, ACT_ARG_QUICKSAND_DEATH_HE_THICC_UNLOCK);
    assert(value(PS_DROWNING_DEATHS) == 1 && value(PS_QUICKSAND_DEATHS) == 0);
    action(ACT_IDLE, 0);
    action(ACT_QUICKSAND_DEATH, ACT_ARG_QUICKSAND_DEATH_QUICKSAND);
    assert(value(PS_DROWNING_DEATHS) == 1 && value(PS_QUICKSAND_DEATHS) == 1);
    sMario.marioObj = &gObjectPool[0];
    for (s32 palette = MARIO_PALETTE_STANDARD; palette <= MARIO_PALETTE_HE_THICC; palette++) {
        sPalette = palette;
        u64 initialGeneric = value(PS_SWIMMING_STROKES);
        u64 initialHE = value(PS_HE_THICC_SWIMMING_STROKES);
        set_mario_action(&sMario, ACT_HOLD_BREASTSTROKE, 0);
        act_hold_breaststroke(&sMario);
        assert(value(PS_SWIMMING_STROKES) == initialGeneric + 1);
        for (s32 i = 0; i < 19; i++) {
            sMario.input = sMario.actionTimer == 2 || sMario.actionTimer == 3 ? INPUT_A_PRESSED : 0;
            act_hold_breaststroke(&sMario);
        }
        assert(value(PS_SWIMMING_STROKES) >= initialGeneric + 3);
        if (palette == MARIO_PALETTE_HE_THICC) {
            assert(value(PS_HE_THICC_SWIMMING_STROKES) - initialHE
                   == value(PS_SWIMMING_STROKES) - initialGeneric);
        } else {
            assert(value(PS_HE_THICC_SWIMMING_STROKES) == initialHE);
        }
    }
    return 0;
}
"""
            compile_and_run(folder, code)

    def test_early_swimming_exits_clear_roll_and_buffered_inputs(self):
        with tempfile.TemporaryDirectory(prefix="audit-swim-early-exit-") as directory:
            folder = Path(directory)
            prepare(ROOT, "swim_animation", folder, {})
            fixture = (ROOT / "tests/swim_animation_test.c").read_text()
            declaration = "    f32 forwardVel, pos[3], vel[3], waterLevel;"
            self.assertIn(declaration, fixture)
            fixture = fixture.replace(declaration, declaration + "\n"
                                      "    f32 quicksandDepth;\n"
                                      "    struct { s16 headAngle[3]; } *marioBodyState;")
            (folder / "swim_exit_fixture.c").write_text(fixture)
            dispatcher = Source(ROOT / "src/game/mario_actions_submerged.c").function(
                "mario_execute_submerged_action")
            # Phase callbacks model successful early exits; the dispatcher and control
            # cleanup/roll lifecycle are production code. No movement is emulated here.
            code = r"""#include <assert.h>
#define main inherited_swim_main
#include "swim_exit_fixture.c"
#undef main
static s32 exitMode, actionCalls;
static s32 audit_exit(struct MarioState *m, s32 mode) {
    if (exitMode != mode) return FALSE;
    m->action = mode == 2 ? ACT_WATER_SHELL_SWIMMING : ACT_FREEFALL;
    if (mode == 2) m->heldObj = &sHeld;
    return TRUE;
}
static s32 audit_action(struct MarioState *m) { (void)m; actionCalls++; return FALSE; }
#define mario_transition_underwater_shell_to_riding(m) audit_exit(m, 2)
#define transition_cap_swimming_to_land(m) audit_exit(m, 1)
#define check_common_submerged_cancels(m) audit_exit(m, 3)
#define try_swimming_surface_launch(m) FALSE
"""
            for name in sorted(set(re.findall(r"\bact_\w+(?=\()", dispatcher))):
                code += f"#define {name}(m) audit_action(m)\n"
            code += dispatcher + r"""
int main(void) {
    for (exitMode = 1; exitMode <= 3; exitMode++) {
        begin(FALSE, 0, FALSE);
        sSwimming.rollFrame = 5;
        sSwimming.aTapPending = TRUE;
        sSwimming.bTapFrames = 3;
        sSwimming.bTapPending = TRUE;
        sSwimming.punchQueued = TRUE;
        sSwimming.strokeBuffered = TRUE;
        sSwimming.burstRecovering = TRUE;
        sObject.header.gfx.angle[2] = 1234;
        sMario.faceAngle[2] = 100;
        assert(mario_execute_submerged_action(&sMario));
        assert(sSwimming.rollFrame == 0 && sSwimming.rollCooldown == 30);
        assert(sObject.header.gfx.angle[2] == 100);
        assert(!sSwimming.aTapPending && !sSwimming.bTapPending && !sSwimming.bTapFrames);
        assert(!sSwimming.punchQueued && !sSwimming.strokeBuffered && !sSwimming.burstRecovering);
        assert(actionCalls == 0);
    }
    // Ordinary swimming retains the active roll and runs exactly one action.
    exitMode = 0;
    begin(FALSE, 0, FALSE);
    sMario.marioBodyState = calloc(1, sizeof(*sMario.marioBodyState));
    assert(sMario.marioBodyState);
    sSwimming.rollFrame = 5;
    assert(!mario_execute_submerged_action(&sMario));
    assert(sSwimming.rollFrame == 5 && sSwimming.rollCooldown == 0 && actionCalls == 1);
    free(sMario.marioBodyState);
    return 0;
}
"""
            compile_and_run(folder, code, stats=False)

    def test_failed_base_read_preserves_persisted_saves(self):
        with tempfile.TemporaryDirectory(prefix="audit-base-read-failure-") as directory:
            folder = Path(directory)
            prepare(ROOT, "character_save", folder, {})
            definitions = folder / "character_save_definitions.inc"
            constants = definitions.read_text().replace(
                "#define player_stats_erase(...) ((void) 0)",
                "static s32 statsErases;\n#define player_stats_erase(...) (++statsErases)",
            )
            self.assertNotEqual(constants, definitions.read_text())
            definitions.write_text(constants)
            original = (ROOT / "tests/character_save_test.c").read_text()
            original = original.replace("static s32 sWrites;", "static s32 sWrites, failBaseRead, timingErases;")
            original, injections = re.subn(
                r"(static s32 read_sram_data\(void \*p,\s*s32 size\)\s*\{)",
                r"\1\nif (failBaseRead) { memset(p, 0xFF, size); return 1; }",
                original, count=1,
            )
            self.assertEqual(injections, 1, "base read failure adapter was not installed")
            original, injections = re.subn(
                r"(static void game_timing_erase\(s32 file\)\s*\{)",
                r"\1\ntimingErases++;", original, count=1,
            )
            self.assertEqual(injections, 1, "timing erase adapter was not installed")
            (folder / "character_save_fault_test.c").write_text(original)
            compile_and_run(folder, r"""#define main inherited_character_main
#include "character_save_fault_test.c"
#undef main
int main(void) {
    begin();
    save_file_set_flags(SAVE_FLAG_FILE_EXISTS | SAVE_FLAG_HAVE_KEY_1);
    save_file_do_save(SAVE_FILE_A);
    struct SaveBuffer persisted = sSaveStorage;
    u8 options[sizeof(sOptionsStorage)];
    memcpy(options, sOptionsStorage, sizeof(options));
    s32 writes = sWrites, stats = statsErases, times = timingErases;
    failBaseRead = 1;
    save_file_load_all();
    assert(sWrites == writes && statsErases == stats && timingErases == times);
    assert(memcmp(&persisted, &sSaveStorage, sizeof(persisted)) == 0);
    assert(memcmp(options, sOptionsStorage, sizeof(options)) == 0);
    // Unknown data stays protected from ordinary progress and an unread copy source.
    save_file_set_flags(SAVE_FLAG_FILE_EXISTS);
    save_file_do_save(SAVE_FILE_A);
    save_file_copy(SAVE_FILE_A, SAVE_FILE_B);
    assert(sWrites == writes && statsErases == stats && timingErases == times);
    assert(memcmp(&persisted, &sSaveStorage, sizeof(persisted)) == 0);
    // An explicit erase replaces only its selected file; the menu stays protected.
    save_file_erase(SAVE_FILE_B);
    assert(sWrites == writes + 1 && statsErases == stats + 1 && timingErases == times + 1);
    assert(memcmp(&persisted.files[SAVE_FILE_A], &sSaveStorage.files[SAVE_FILE_A],
                  sizeof(persisted.files[SAVE_FILE_A])) == 0);
    assert(memcmp(&persisted.menuData, &sSaveStorage.menuData, sizeof(persisted.menuData)) == 0);
    assert(sSaveStorage.files[SAVE_FILE_B][0].flags == 0);
    // An explicitly replaced file can become a readable copy source for another file.
    gCurrSaveFileNum = SAVE_FILE_B + 1;
    save_file_set_flags(SAVE_FLAG_FILE_EXISTS | SAVE_FLAG_HAVE_KEY_2);
    save_file_do_save(SAVE_FILE_B);
    save_file_copy(SAVE_FILE_B, SAVE_FILE_D);
    assert(sSaveStorage.files[SAVE_FILE_D][0].flags & SAVE_FLAG_HAVE_KEY_2);
    assert(memcmp(&persisted.files[SAVE_FILE_A], &sSaveStorage.files[SAVE_FILE_A],
                  sizeof(persisted.files[SAVE_FILE_A])) == 0);
    gCurrSaveFileNum = SAVE_FILE_A + 1;
    // A successful reload recovers the preserved save and re-enables ordinary writes.
    failBaseRead = 0;
    save_file_load_all();
    assert(save_file_get_flags() & SAVE_FLAG_HAVE_KEY_1);
    save_file_set_flags(SAVE_FLAG_HAVE_KEY_2);
    save_file_do_save(SAVE_FILE_A);
    assert(sSaveStorage.files[SAVE_FILE_A][0].flags & SAVE_FLAG_HAVE_KEY_2);
    return 0;
}
""", stats=False)

    def test_cross_file_save_and_reload_keep_dirty_owners(self):
        with tempfile.TemporaryDirectory(prefix="audit-dirty-save-") as directory:
            folder = Path(directory)
            prepare(ROOT, "character_save", folder, {})
            header = Source(ROOT / "src/game/save_file.h")
            definitions = folder / "character_save_definitions.inc"
            definitions.write_text(definitions.read_text() + header.macros("STAR_FLAG_TO_SAVE_FLAG"))
            code = """#include <assert.h>
#define main inherited_character_main
#include "tests/character_save_test.c"
#undef main
"""
            original = (ROOT / "tests/character_save_test.c").read_text()
            original = original.replace("static s32 sWrites;", "static s32 sWrites, failMenuWrite;")
            original, injections = re.subn(
                r"(static s32 write_sram_data\(void \*p,\s*s32 size\)\s*\{)",
                r"\1\nif(p==&gSaveBuffer.menuData && failMenuWrite) {failMenuWrite=0;return 1;}",
                original, count=1,
            )
            self.assertEqual(injections, 1, "menu write failure adapter was not installed")
            (folder / "character_save_fault_test.c").write_text(original)
            code = code.replace('"tests/character_save_test.c"', '"character_save_fault_test.c"')
            code += r"""
int main(void) {
    begin(); gCurrSaveFileNum = SAVE_FILE_A + 1;
    gMainMenuDataModified = TRUE; failMenuWrite=1;
    save_main_menu_data(); assert(gMainMenuDataModified);
    save_file_reload(); assert(gMainMenuDataModified);
    save_main_menu_data(); assert(!gMainMenuDataModified);
    save_file_set_flags(SAVE_FLAG_FILE_EXISTS); save_file_do_save(SAVE_FILE_A);
    save_file_set_star_flags(SAVE_FILE_A, COURSE_NUM_TO_INDEX(COURSE_BOB), 1);
    assert(gSaveFileModified && sSaveStorage.files[SAVE_FILE_A][0].courseStars[0] == 0);
    // Run the real paused shortcut, including the conversion save of C.
    gReonucamState.speed = save_file_get_camera_speed();
    set_play_mode(PLAY_MODE_PAUSED); gMenuMode = MENU_MODE_RENDER_PAUSE_SCREEN;
    for (s32 frame = 0; frame < 64; frame++) {
        sController.buttonDown = tas_chord(frame); play_mode_paused();
    }
    assert(gCurrSaveFileNum == SAVE_FILE_C + 1 && gSaveFileModified);
    assert(sSaveStorage.files[SAVE_FILE_A][0].courseStars[0] == 0);
    // Reloading or erasing another file cannot drop A's pending save.
    save_file_reload(); assert(gSaveFileModified);
    save_file_erase(SAVE_FILE_D); assert(gSaveFileModified);
    save_file_copy(SAVE_FILE_C, SAVE_FILE_B); assert(gSaveFileModified);
    gCurrSaveFileNum = SAVE_FILE_A + 1;
    lvl_init_from_save_file(0, LEVEL_BOB);
    assert(gSaveBuffer.files[SAVE_FILE_A][0].courseStars[0] == 1);
    s32 writes = sWrites; save_file_do_save(SAVE_FILE_A);
    assert(sWrites > writes && !gSaveFileModified);
    save_file_load_all(); assert(gSaveBuffer.files[SAVE_FILE_A][0].courseStars[0] == 1);
    // A Game Over rollback intentionally discards only that file's unsaved change.
    gCurrSaveFileNum = SAVE_FILE_A + 1;
    save_file_set_flags(SAVE_FLAG_HAVE_KEY_1);
    save_file_set_star_flags(SAVE_FILE_B, COURSE_NUM_TO_INDEX(COURSE_BOB), 2);
    save_file_reload();
    assert(!(gSaveBuffer.files[SAVE_FILE_A][0].flags & SAVE_FLAG_HAVE_KEY_1));
    assert(gSaveFileModified);
    save_file_do_save(SAVE_FILE_B); assert(!gSaveFileModified);
    save_file_load_all(); assert(gSaveBuffer.files[SAVE_FILE_B][0].courseStars[0] & 2);
    return 0;
}
"""
            compile_and_run(folder, code, stats=False)

    def test_tas_switch_finalizes_old_file_records_at_origin(self):
        with tempfile.TemporaryDirectory(prefix="audit-tas-record-") as directory:
            folder = Path(directory)
            prepare_fixture(folder)
            code = STATS_DRIVER + """
#define SAVE_FILE_C 2
static s32 sTasThiccStartPending, unlocks;
static void save_file_unlock_tas_thicc(void) {unlocks++;}
"""
            code += Source(ROOT / "src/game/level_update.c").function("lvl_prepare_tas_thicc_start")
            code += r"""
int main(void) {
    setup(); gCurrAreaIndex = 2;
    sMario.action = ACT_BUTT_SLIDE;
    player_stats_mario_begin(&sMario); sMario.pos[0] = 200; player_stats_mario_end(&sMario);
    assert(value(PS_LONGEST_CONTINUOUS_SLIDE) == 0);
    lvl_prepare_tas_thicc_start();
    assert(gCurrSaveFileNum == 3 && unlocks == 1 && sTasThiccStartPending);
    assert(value(PS_LONGEST_CONTINUOUS_SLIDE) == 200 * 256);
    assert(gPlayerStats[0].levels[PS_LONGEST_CONTINUOUS_SLIDE] == LEVEL_BOB);
    assert(gPlayerStats[0].areas[PS_LONGEST_CONTINUOUS_SLIDE] == 2);
    assert(value(PS_COURSE_EXITS_WITHOUT_COLLECTING_A_STAR) == 1);
    assert(!player_stats_is_dirty(0));
    gCurrLevelNum = LEVEL_CASTLE_GROUNDS;
    sMario.action = ACT_UNINITIALIZED;
    player_stats_warp(LEVEL_CASTLE_GROUNDS, TRUE);
    player_stats_game_update();
    assert(gPlayerStats[2].values[PS_LONGEST_CONTINUOUS_SLIDE] == 0);
    assert(value(PS_COURSE_EXITS_WITHOUT_COLLECTING_A_STAR) == 1);
    return 0;
}
"""
            compile_and_run(folder, code)

    def test_debug_pause_exit_closes_visit_and_star_eligibility(self):
        with tempfile.TemporaryDirectory(prefix="audit-debug-visit-") as directory:
            folder = Path(directory)
            prepare_fixture(folder)
            source = Source(ROOT / "src/game/level_update.c")
            code = STATS_DRIVER
            code += Source(ROOT / "src/game/area.h").declaration("enum", "MenuOption")
            code += Source(ROOT / "src/game/ingame_menu.h").declaration("enum", "MenuMode")
            for name in ("SpecialWarpDestinations", "PlayModes", "WarpTypes", "WarpFlags"):
                code += Source(ROOT / "src/game/level_update.h").declaration("enum", name)
            code += Source(ROOT / "include/config/config_menu.h").macros("EXIT_COURSE_.*")
            code += r"""
#define CAM_MOVE_PAUSE_SCREEN (1u << 15)
#define WARP_TRANSITION_FADE_INTO_COLOR 1
static s32 gMenuOptSelectIndex,gCameraMovementFlags,gDebugLevelSelect,gSavedCourseNum;
static u8 sPauseTasCodeProgress;
static s32 sCurrPlayMode,sSpecialWarpDest;
static struct {s32 type;} sWarpDest;
static struct {u32 buttonDown;} controller,*gPlayer1Controller=&controller;
static void set_play_mode(s32 mode) {sCurrPlayMode=mode;}
static s32 jrb_simpleflips_exit_locked(void) {return FALSE;}
static void update_paused_camera(void) {}
static s32 game_options_secrets_blocked(void) {return FALSE;}
static s32 game_options_active_file(void) {return 0;}
static void game_options_save(s32 file) {(void)file;}
static void game_timing_end_course(void) {}
static s32 jrb_simpleflips_is_game_over(void) {return FALSE;}
static void pause_easter_egg_update(void) {}
static s32 mario_character_update_tas_code(u8 *p,u32 b) {(void)p;(void)b;return FALSE;}
#define lvl_prepare_tas_thicc_start(...) ((void)0)
#define lvl_restore_cap_on_file_start(...) ((void)0)
#define lvl_init_from_save_file(...) ((void)0)
#define reset_dialog_render_state(...) ((void)0)
#define set_menu_mode(...) ((void)0)
#define raise_background_noise(...) ((void)0)
#define hud_reveal_after_pause(...) ((void)0)
#define fadeout_music(...) ((void)0)
#define play_transition(...) ((void)0)
#define level_set_transition(...) ((void)0)
static s32 level_is_ending(void) {return FALSE;}
static void level_warp_to_physics_lab(void) {assert(0);}
static void initiate_warp(s16 destination,s16 area,s16 node,s32 flags) {
    (void)area;(void)node;
    player_stats_warp(destination,(flags & WARP_FLAG_EXIT_COURSE) != 0);
    sWarpDest.type=WARP_TYPE_CHANGE_LEVEL;
}
"""
            for name in ("warp_special", "fade_into_special_warp", "play_mode_paused"):
                code += source.function(name)
            code += r"""
int main(void) {
    setup();
    gDebugLevelSelect=TRUE; gMenuOptSelectIndex=MENU_OPT_EXIT_COURSE;
    play_mode_paused();
    assert(sCurrPlayMode==PLAY_MODE_CHANGE_LEVEL && sSpecialWarpDest==WARP_SPECIAL_LEVEL_SELECT);
    assert(value(PS_COURSE_EXITS_WITHOUT_COLLECTING_A_STAR)==1);
    assert(!player_stats_is_dirty(0));
    sMario.action=ACT_WALKING; player_stats_level_entered(); player_stats_game_update();
    assert(value(PS_COURSE_ENTRIES)==2);
    // A prior visit's star must not exempt a later starless visit.
    struct Object star={.model=MODEL_STAR}; player_stats_star(&star,STAR_BP_ACT_100_COINS);
    play_mode_paused();
    assert(value(PS_COURSE_EXITS_WITHOUT_COLLECTING_A_STAR)==1);
    player_stats_level_entered(); player_stats_game_update();
    gDebugLevelSelect=FALSE; play_mode_paused();
    assert(value(PS_COURSE_ENTRIES)==3);
    assert(value(PS_COURSE_EXITS_WITHOUT_COLLECTING_A_STAR)==2);
    return 0;
}
"""
            compile_and_run(folder, code)

    def test_simpleflips_toad_completion_and_direct_punch_damage(self):
        with tempfile.TemporaryDirectory(prefix="audit-toad-dialog-") as directory:
            folder = Path(directory)
            prepare_fixture(folder)
            world = (folder / "stats_test_world.h").read_text().replace("struct Object {", """struct Object {
struct {struct {struct {s32 animFrame,animYTrans;void *curAnim;}animInfo;}gfx;}header;
s32 oTimer,oBehParams2ndByte,oMoveAngleYaw,oInteractType,oInteractStatus,oOpacity;
f32 oPosX,oPosZ;
""").replace("u32 action;", "u32 action; s16 faceAngle[3]; s32 invincTimer; u8 respawnHealCounter; f32 forwardVel,vel[3];")
            (folder / "stats_test_world.h").write_text(world)
            code = STATS_DRIVER + r"""
#define MARIO_DIALOG_LOOK_DOWN 1
#define DIALOG_FLAG_TEXT_DEFAULT 1
#define DIALOG_FLAG_TIME_STOP_ENABLED 2
#define SOUND_ACTION_HIT 1
#define load_object_collision_model(...) ((void)0)
#define __n64Assert(...) ((void)0)
#define cur_obj_play_sound_2(...) ((void)0)
static char __n64Assert_MessageBuf[2];
static struct Object *o;
static s32 sSimpleFlipsReadEffectsPending,sSimpleFlipsFinalReadComplete,sSimpleFlipsGameOver,dialogComplete;
static s32 game_options_secrets_blocked(void) {return FALSE;}
static s32 jrb_simpleflips_room_active(void) {return TRUE;}
static void save_file_mark_simpleflips_game_over(void) {}
static void simpleflips_finish_sign_read(void) {}
static void simpleflips_transform_sign(void) {}
static void cur_obj_hide(void) {}
static void cur_obj_unhide(void) {}
static void cur_obj_become_intangible(void) {}
static void cur_obj_become_tangible(void) {}
static s32 cur_obj_is_mario_ground_pounding_platform(void) {return FALSE;}
static s32 cur_obj_update_dialog(s32 look,s32 flags,s32 dialog,s32 arg) {
    (void)look;(void)flags;(void)dialog;(void)arg;return dialogComplete;
}
static void cur_obj_set_model(s32 model) {o->model=model;}
static void cur_obj_init_animation(s32 anim) {(void)anim;}
static f32 sins(s16 a) {return sinf(a*(6.283185307179586f/65536));}
static f32 coss(s16 a) {return cosf(a*(6.283185307179586f/65536));}
static s16 atan2s(f32 z,f32 x) {return (s16)(atan2f(z,x)*(65536/6.283185307179586f));}
static void drop_and_set_mario_action(struct MarioState *m,u32 next,u32 arg) {
    player_stats_action(m,next,arg);m->action=next;
}
static void mario_set_forward_vel(struct MarioState *m,f32 speed) {m->forwardVel=speed;}
"""
            jrb = Source(ROOT / "src/game/jrb_simpleflips.c")
            code += jrb.declaration("enum", "SimpleFlipsActions")
            code += jrb.macros("SIMPLEFLIPS_PUNCH_.*|SIMPLEFLIPS_TOAD_IDLE_YAW_OFFSET")
            code += "#define SIMPLEFLIPS_FRAMES_PER_SECOND 30\n"
            code += Source(ROOT / "src/game/jrb_simpleflips.h").macros("MODEL_JRB_SIMPLEFLIPS_.*")
            interaction = Source(ROOT / "src/game/interaction.h")
            code += interaction.declaration("enum", "ObjInteractTypes")
            code += interaction.declaration("enum", "InteractStatus")
            for name in ("simpleflips_start_punch", "simpleflips_punch_mario", "bhv_jrb_simpleflips_sign_loop"):
                code += jrb.function(name)
            code += r"""
int main(void) {
    setup();gCurrLevelNum=LEVEL_JRB;gCurrAreaIndex=3;sMario.action=ACT_READING_NPC_DIALOG;
    struct Object toad={.oAction=SIMPLEFLIPS_DIALOG};o=&toad;
    for (s32 frame=0;frame<20;frame++) bhv_jrb_simpleflips_sign_loop();
    assert(value(PS_TOAD_CONVERSATIONS)==0 && toad.oAction==SIMPLEFLIPS_DIALOG);
    dialogComplete=TRUE;bhv_jrb_simpleflips_sign_loop();
    assert(toad.oAction==SIMPLEFLIPS_PUNCH && value(PS_TOAD_CONVERSATIONS)==1);
    sMario.healCounter=20;sMario.hurtCounter=8;
    for (s32 frame=0;frame<=SIMPLEFLIPS_PUNCH_CONTACT;frame++) {
        toad.oTimer=frame;bhv_jrb_simpleflips_sign_loop();
    }
    assert(sMario.health==0xFF && sMario.healCounter==0 && sMario.hurtCounter==0);
    assert(sSimpleFlipsGameOver && value(PS_TOAD_CONVERSATIONS)==1);
    assert(value(PS_HITS_TAKEN)==1 && value(PS_POWER_METER_WEDGES_LOST)==0x781);
    assert(value(PS_LIVES_LOST)==0 && value(PS_GAME_OVERS)==0);
    for (s32 frame=0;frame<20;frame++) bhv_jrb_simpleflips_sign_loop();
    assert(value(PS_TOAD_CONVERSATIONS)==1 && value(PS_HITS_TAKEN)==1);
    return 0;
}
"""
            compile_and_run(folder, code)

    def test_tas_metadata_is_never_a_castle_star(self):
        with tempfile.TemporaryDirectory(prefix="audit-tas-stars-") as directory:
            folder = Path(directory)
            header = Source(ROOT / "src/game/save_file.h")
            save = Source(ROOT / "src/game/save_file.c")
            # Select ordinary-save definitions while retaining production function bodies.
            text = re.sub(r"(?:s32 save_file_get_course_star_count|u32 save_file_get_star_flags)\(UNUSED.*?\n}",
                          lambda m: "\n" * m[0].count("\n"), save.text, flags=re.S)
            save = Source(save.path, text)
            code = """#include <stdint.h>
#include <assert.h>
#include <string.h>
typedef int8_t s8; typedef uint8_t u8; typedef int16_t s16; typedef uint16_t u16;
typedef int32_t s32; typedef uint32_t u32; typedef s16 Vec3s[3];
#define TRUE 1
#include "include/course_table.h"
"""
            code += header.macros("NUM_SAVE_FILES|SAVE_CAKE_.*|CASTLE_LOBBY_CONVERSATION_COUNT|SAVE_FLAG_TO_STAR_FLAG|STAR_FLAG_TO_SAVE_FLAG")
            for kind, names in (("enum", ("SaveFileIndex", "SaveProgressFlags")),
                                ("struct", ("SaveBlockSignature", "SaveFile"))):
                code += "".join(header.declaration(kind, name) for name in names)
            code += """static struct {struct SaveFile files[4][2];} gSaveBuffer;
static s32 secretsBlocked;
static s32 game_options_secrets_blocked(void) {return secretsBlocked;}
static void mark_save_file_modified(s32 file) {(void)file;}
static void save_file_do_save(s32 file) {(void)file;}
"""
            for name in ("save_file_unlock_tas_thicc", "save_file_get_star_flags",
                         "save_file_get_course_star_count", "save_file_get_total_star_count",
                         "save_file_get_saved_star_count", "save_file_set_star_flags"):
                code += save.function(name)
            code += r"""
int main(void) {
    secretsBlocked = 1;
    save_file_unlock_tas_thicc();
    assert(gSaveBuffer.files[SAVE_FILE_C][0].flags == 0);
    secretsBlocked = 0;
    save_file_unlock_tas_thicc();
    assert(save_file_get_total_star_count(SAVE_FILE_C, COURSE_MIN - 1, COURSE_MAX - 1) == 0);
    assert(save_file_get_saved_star_count(SAVE_FILE_C) == 0);
    // Only native castle stars are exposed or set; neighboring metadata stays intact.
    for (u32 bit = 0; bit < 5; bit++) {
        save_file_set_star_flags(SAVE_FILE_C, COURSE_NUM_TO_INDEX(COURSE_NONE), 1u << bit);
        assert(save_file_get_course_star_count(SAVE_FILE_C, COURSE_NUM_TO_INDEX(COURSE_NONE)) == (s32)bit + 1);
    }
    assert(gSaveBuffer.files[SAVE_FILE_C][0].flags & SAVE_FLAG_TAS_THICC);
    save_file_set_star_flags(SAVE_FILE_A, COURSE_NUM_TO_INDEX(COURSE_NONE), 0x7F);
    assert(!(gSaveBuffer.files[SAVE_FILE_A][0].flags & SAVE_FLAG_TAS_THICC));
    for (s32 file = 0; file < 4; file++) {
        for (s32 i = 0; i < 15; i++) gSaveBuffer.files[file][0].courseStars[i] = 0x7F;
        for (s32 i = 15; i < 25; i++) {
            if (i != COURSE_NUM_TO_INDEX(COURSE_CAKE_END)) gSaveBuffer.files[file][0].courseStars[i] = 1;
        }
        gSaveBuffer.files[file][0].courseStars[COURSE_NUM_TO_INDEX(COURSE_PSS)] = 3;
        gSaveBuffer.files[file][0].flags |= 0x1F000000;
        assert(save_file_get_total_star_count(file, COURSE_MIN - 1, COURSE_MAX - 1) == 120);
        assert(save_file_get_saved_star_count(file) == 120);
    }
    return 0;
}
"""
            compile_and_run(folder, code, stats=False)


if __name__ == "__main__":
    unittest.main()
