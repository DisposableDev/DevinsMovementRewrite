#include "game_options_runtime.h"
#include "player_stats.h"
#include <PR/ultratypes.h>

#include "types.h"
#include "actors/common1.h"
#include "actors/group12.h"
#include "actors/group13.h"
#include "area.h"
#include "castle_lobby_finale.h"
#include "audio/external.h"
#include "behavior_actions.h"
#include "behavior_data.h"
#include "camera.h"
#include "debug.h"
#include "dialog_ids.h"
#include "engine/behavior_script.h"
#include "engine/graph_node.h"
#include "engine/math_util.h"
#include "engine/surface_collision.h"
#include "engine/surface_load.h"
#include "game_init.h"
#include "game_options.h"
#include "ingame_menu.h"
#include "interaction.h"
#include "level_misc_macros.h"
#include "level_table.h"
#include "level_update.h"
#include "liquid_particles.h"
#include "physics_lab.h"
#include "levels/bob/header.h"
#include "levels/bowser_3/header.h"
#include "levels/castle_inside/header.h"
#include "levels/castle_grounds/header.h"
#include "levels/hmc/header.h"
#include "levels/physics_lab/header.h"
#include "main.h"
#include "mario.h"
#include "mario_actions_cutscene.h"
#include "mario_step.h"
#include "obj_behaviors.h"
#include "obj_behaviors_2.h"
#include "object_constants.h"
#include "object_helpers.h"
#include "object_list_processor.h"
#include "paintings.h"
#include "platform_displacement.h"
#include "rendering_graph_node.h"
#include "save_file.h"
#include "seq_ids.h"
#include "sm64.h"
#include "spawn_object.h"
#include "spawn_sound.h"
#include "rumble_init.h"

#include "behaviors/star_door.inc.c"
#include "behaviors/mr_i.inc.c"
#include "behaviors/pole.inc.c"
#include "behaviors/thi_top.inc.c"
#include "behaviors/capswitch.inc.c"
#include "behaviors/king_bobomb.inc.c"
#include "behaviors/beta_chest.inc.c"
#include "behaviors/water_objs.inc.c"
#include "behaviors/cannon.inc.c"
#include "behaviors/chuckya.inc.c"
#include "behaviors/breakable_wall.inc.c"
#include "behaviors/kickable_board.inc.c"
#include "behaviors/tower_door.inc.c"
#include "behaviors/rotating_platform.inc.c"
#include "behaviors/koopa_shell_underwater.inc.c"
#include "behaviors/warp.inc.c"
#include "behaviors/white_puff_explode.inc.c"

// Scratch recipe: each call rewrites its configurable fields before synchronous emission.
static struct SpawnParticlesInfo sVariableMistParticles = {
    /* behParam:        */ WHITE_PUFF_EXPLODE_BP_FAST_FADE,
    /* count:           */ 20,
    /* model:           */ MODEL_MIST,
    /* offsetY:         */ 0,
    /* forwardVelBase:  */ 40,
    /* forwardVelRange: */ 5,
    /* velYBase:        */ 30,
    /* velYRange:       */ 20,
    /* gravity:         */ 252,
    /* dragStrength:    */ 30,
    /* sizeBase:        */ 330.0f,
    /* sizeRange:       */ 10.0f,
};

void spawn_mist_particles_variable(s32 count, s32 offsetY, f32 sizeTenths) {
    sVariableMistParticles.sizeBase = sizeTenths;
    sVariableMistParticles.sizeRange = sizeTenths / 20.0f;
    sVariableMistParticles.offsetY = offsetY;

    if (count == 0) {
        sVariableMistParticles.count = 20;
    } else if (count > 0) {
        sVariableMistParticles.count = count;
    } else {
        sVariableMistParticles.count = 4;
    }

    cur_obj_spawn_particles_with_priority(&sVariableMistParticles, PARTICLE_PRIORITY_MIST);
}

#include "behaviors/sparkle_spawn_star.inc.c"
#include "behaviors/coin.inc.c"
#include "behaviors/collide_particles.inc.c"
#include "behaviors/door.inc.c"
#include "behaviors/thwomp.inc.c"
#include "behaviors/tumbling_bridge.inc.c"
#include "behaviors/elevator.inc.c"
#include "behaviors/water_mist_particle.inc.c"
#include "behaviors/break_particles.inc.c"
#include "behaviors/water_mist.inc.c"
#include "behaviors/ground_particles.inc.c"
#include "behaviors/wind.inc.c"
#include "behaviors/unused_particle_spawn.inc.c"
#include "behaviors/ukiki_cage.inc.c"
#include "behaviors/falling_rising_platform.inc.c"
#include "behaviors/fishing_boo.inc.c"
#include "behaviors/flamethrower.inc.c"
#include "behaviors/bouncing_fireball.inc.c"
#include "behaviors/shock_wave.inc.c"
#include "behaviors/flame_mario.inc.c"
#include "behaviors/beta_fish_splash_spawner.inc.c"
#include "behaviors/spindrift.inc.c"
#include "behaviors/tower_platform.inc.c"
#include "behaviors/tree_particles.inc.c"
#include "behaviors/square_platform_cycle.inc.c"
#include "behaviors/piranha_bubbles.inc.c"
#include "behaviors/message_panel.inc.c"
#include "behaviors/purple_switch.inc.c"
#include "behaviors/metal_box.inc.c"
#include "behaviors/breakable_box.inc.c"

// not sure what this is doing here. not in a behavior file.
Gfx *geo_move_mario_part_from_parent(s32 callContext, UNUSED struct GraphNode *node, Mat4 mtx) {

    if (callContext == GEO_CONTEXT_RENDER) {
        struct Object *obj = (struct Object *) gCurGraphNodeObject;
        if (obj == gMarioObject && obj->prevObj != NULL) {
            obj_update_pos_from_parent_transformation(mtx, obj->prevObj);
            obj_set_gfx_pos_from_pos(obj->prevObj);
        }
    }

    return NULL;
}

#include "behaviors/heave_ho.inc.c"
#include "behaviors/spawn_star_exit.inc.c"
#include "behaviors/unused_poundable_platform.inc.c"
#include "behaviors/beta_trampoline.inc.c"
#include "behaviors/jumping_box.inc.c"
#include "behaviors/yoshi_credit_sign.inc.c"
#include "behaviors/castle_secret_room.inc.c"
#include "behaviors/boo_cage.inc.c"

// Positive spawnerCount is required; each spawner attempts three visible sparkle children.
// radius/verticalSpacing are parent-local offsets in world units, starting one spacing above it.
// phaseStep is in 1/256 turns per call. The phase is shared across all keys/stars using this helper.
void spawn_sparkle_particles(s32 spawnerCount, s32 radius, s32 verticalSpacing, s32 phaseStep) {
    static s16 sSparkleSpawnerPhase = 0x0;
    s32 i;
    s16 angularSpacing = 0x10000 / spawnerCount; // Evenly spread around a circle

    for (i = 0; i < spawnerCount; i++) {
        try_spawn_particle_relative(
            OBJ_BP_NONE, sins(sSparkleSpawnerPhase + i * angularSpacing) * radius,
            (i + 1) * verticalSpacing, coss(sSparkleSpawnerPhase + i * angularSpacing) * radius, 1.0f,
            o, MODEL_NONE, bhvSparkleSpawn, PARTICLE_PRIORITY_FEEDBACK);
    }

    // Preserve phase progression even when some or all allocations fail.
    sSparkleSpawnerPhase += phaseStep * 0x100;
}

#include "behaviors/beta_boo_key.inc.c"
#include "behaviors/grand_star.inc.c"
#include "behaviors/bowser_key.inc.c"
#include "behaviors/bullet_bill.inc.c"
#include "behaviors/bowser.inc.c"
#include "behaviors/bowser_falling_platform.inc.c"
#include "behaviors/bowser_flame.inc.c"
#include "behaviors/blue_fish.inc.c"
#include "behaviors/checkerboard_platform.inc.c"
#include "behaviors/ddd_warp.inc.c"
#include "behaviors/water_pillar.inc.c"
#include "behaviors/moat_drainer.inc.c"
#include "behaviors/bowser_key_cutscene.inc.c"
#include "behaviors/moat_grill.inc.c"
#include "behaviors/clock_arm.inc.c"
#include "behaviors/ukiki.inc.c"
#include "behaviors/lll_octagonal_rotating_mesh.inc.c"
#include "behaviors/lll_sinking_rock_block.inc.c"
#include "behaviors/lll_rotating_hex_flame.inc.c"
#include "behaviors/lll_floating_wood_piece.inc.c"
#include "behaviors/lll_volcano_flames.inc.c"
#include "behaviors/lll_hexagonal_ring.inc.c"
#include "behaviors/lll_sinking_rectangle.inc.c"
#include "behaviors/tilting_inverted_pyramid.inc.c"
#include "behaviors/physics_lab.inc.c"
#include "behaviors/tox_box.inc.c"
#include "behaviors/piranha_plant.inc.c"
#include "behaviors/castle_lobby_finale.inc.c"
#include "behaviors/bowser_puzzle_piece.inc.c"

void set_obj_anim_with_accel_and_sound(s16 frame1, s16 frame2, s32 sound) {
    s32 range = o->header.gfx.animInfo.animAccel / 0x10000;
    if (range == 0) {
        range = 1;
    }

    if (cur_obj_check_anim_frame_in_range(frame1, range) || cur_obj_check_anim_frame_in_range(frame2, range)) {
        cur_obj_play_sound_2(sound);
    }
}

#include "behaviors/tuxie.inc.c"
#include "behaviors/fish.inc.c"
#include "behaviors/express_elevator.inc.c"
#include "behaviors/bub.inc.c"
#include "behaviors/exclamation_box.inc.c"
#include "behaviors/sound_spawner.inc.c"
#include "behaviors/ddd_sub.inc.c"
#include "behaviors/sushi.inc.c"
#include "behaviors/jrb_ship.inc.c"
#include "behaviors/white_puff.inc.c"
#include "behaviors/blue_coin.inc.c"
#include "behaviors/grill_door.inc.c"
#include "behaviors/wdw_water_level.inc.c"
#include "behaviors/tweester.inc.c"
#include "behaviors/boo.inc.c"
#include "behaviors/bbh_tilting_trap.inc.c"
#include "behaviors/bbh_haunted_bookshelf.inc.c"
#include "behaviors/bbh_merry_go_round.inc.c"
#include "behaviors/static_checkered_platform.inc.c"
#include "behaviors/beta_bowser_anchor.inc.c"
#include "behaviors/music_touch.inc.c"
#include "behaviors/castle_floor_trap.inc.c"
#include "behaviors/pole_base.inc.c"
#include "behaviors/sparkle_spawn.inc.c"
#include "behaviors/scuttlebug.inc.c" // :scuttleeyes:
#include "behaviors/whomp.inc.c"
#include "behaviors/water_splashes_and_waves.inc.c"
#include "behaviors/strong_wind_particle.inc.c"
#include "behaviors/sl_snowman_wind.inc.c"
#include "behaviors/sl_walking_penguin.inc.c"

// These legacy movers do not use obj_behaviors_2's attack tables. Keep their
// native rewards and respawn actions, and retire any grab/throw ownership.
s32 cur_obj_defeat_metal_contact_enemy_legacy(void) {
    if (obj_has_behavior(o, bhvBobomb)) {
        // The committed follow-up knocks it away before its native landing explosion.
        o->oAction = BOBOMB_ACT_LAUNCHED;
        o->oMoveAngleYaw = obj_angle_to_object(gMarioObject, o);
        o->oForwardVel = 25.0f;
        o->oVelY = 30.0f;
        o->oFlags &= ~OBJ_FLAG_SET_FACE_YAW_TO_MOVE_YAW;
        o->oBobombFuseTimer = 0;
        o->oBobombFuseLit = TRUE;
        o->oTimer = 0;
        o->oHeldState = HELD_FREE;
        cur_obj_become_intangible();
    } else if (obj_has_behavior(o, bhvMoneybag)) {
        o->oAction = MONEYBAG_ACT_DEATH;
        o->oTimer = 0;
        cur_obj_become_intangible();
    } else if (obj_has_behavior(o, bhvScuttlebug)) {
        cur_obj_set_hitbox_and_die_if_attacked(&sScuttlebugHitbox, SOUND_OBJ_DYING_ENEMY1,
                                               o->oScuttlebugHasNoLootCoins);
        if (o->parentObj != o) {
            o->parentObj->oScuttlebugSpawnerIsDeactivated = TRUE;
        }
    } else if (obj_has_behavior(o, bhvSpindrift)) {
        cur_obj_set_hitbox_and_die_if_attacked(&sSpindriftHitbox, SOUND_OBJ_DYING_ENEMY1, FALSE);
    } else if (obj_has_behavior(o, bhvChuckya) || obj_has_behavior(o, bhvHeaveHo)) {
        if (gMarioState->usedObj == o && gMarioState->action == ACT_GRABBED) {
            gMarioObject->oInteractStatus |=
                INT_STATUS_MARIO_THROWN_BY_OBJ | INT_STATUS_MARIO_DROPPED_BY_OBJ;
        }
        o->oCommonAnchorAction = 0; // Shared with Heave-Ho throw state.
        o->oHeldState = HELD_FREE;
        o->oInteractionSubtype &= ~INT_SUBTYPE_GRABS_MARIO;
        if (obj_has_behavior(o, bhvChuckya)) {
            chuckya_die();
        } else {
            player_stats_enemy_defeated(o);
            // Heave-Ho has no native attack death or coin reward.
            obj_spawn_loot_yellow_coins(o, o->oNumLootCoins, 20.0f);
            spawn_mist_particles_with_sound(SOUND_OBJ_DEFAULT_DEATH);
            obj_mark_for_deletion(o);
        }
    } else {
        o->oInteractStatus = INT_STATUS_NONE;
        return FALSE;
    }
    o->oInteractStatus = INT_STATUS_NONE;
    return TRUE;
}
