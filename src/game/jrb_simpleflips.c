#include "game_options_runtime.h"
#include "sm64.h"
#include "jrb_simpleflips.h"
#include "area.h"
#include "audio/external.h"
#include "seq_ids.h"
#include "sound_init.h"
#include "memory.h"
#include "behavior_actions.h"
#include "debugger/assert.h"
#include "behavior_data.h"
#include "camera.h"
#include "dialog_ids.h"
#include "engine/graph_node.h"
#include "engine/math_util.h"
#include "engine/surface_load.h"
#include "interaction.h"
#include "level_table.h"
#include "level_update.h"
#include "mario.h"
#include "mario_actions_cutscene.h"
#include "model_ids.h"
#include "object_helpers.h"
#include "object_list_processor.h"
#include "spawn_sound.h"
#include "save_file.h"
#include "player_stats.h"
#include "levels/jrb/header.h"

enum SimpleFlipsActions {
    SIMPLEFLIPS_SIGN,
    SIMPLEFLIPS_TOAD,
    SIMPLEFLIPS_DIALOG,
    SIMPLEFLIPS_PUNCH,
    SIMPLEFLIPS_FINISHED,
    SIMPLEFLIPS_SIGN_HIDDEN,
    SIMPLEFLIPS_SIGN_FADING
};

#define SIMPLEFLIPS_PUNCH_WINDUP 12
#define SIMPLEFLIPS_PUNCH_CONTACT 14
#define SIMPLEFLIPS_PUNCH_DISTANCE 100.0f
#define SIMPLEFLIPS_TOAD_IDLE_YAW_OFFSET 0x1555
#if defined(VERSION_EU)
#define SIMPLEFLIPS_FRAMES_PER_SECOND 25
#else
#define SIMPLEFLIPS_FRAMES_PER_SECOND 30
#endif
#define SIMPLEFLIPS_FINAL_READ 128
#define SIMPLEFLIPS_ONE_UP_TWIRL_FRAMES 38

// Shared encounter/title latch; boot can restore it from the one-shot saved game over.
static u8 sSimpleFlipsGameOver;
static u8 sSimpleFlipsReadCount;
// Set when reading commits; cleared when Mario leaves this sign's reading action.
static u8 sSimpleFlipsReadEffectsPending;
static u8 sSimpleFlipsRoomRed;
static u8 sSimpleFlipsRoomMusicAllowed;
static u8 sSimpleFlipsFinalReadComplete;
// Borrowed object pointer: mark for deletion on room cleanup; only forget it on reset.
static struct Object *sSimpleFlipsMips;

static char sSimpleFlipsEnding[] =
    "Nathanial scuttle bug paragoomba two-legged black haired singularity three-legged master race "
    "koopa the quick everytime sometimes once in a while untitled artist one mahjong Titans was "
    "held available on PC Windows flash computer personal computer Android mobile phone samsung "
    "galaxy Microsoft Windows 10 Windows 2000 Desktop Linux PC Mac html5 Mario platformer super "
    "Windows 8.1 Windows Microsoft 7 Chrome OS mobile android Kit kat Macintosh Apple computer "
    "windows VirtualBox VM Ware Wii U browser Wii U Nintendo browser original Nintendo low-low on "
    "official 3ds Linux super Ubuntu spongebob stupid and Mario simulator portable device Goomba "
    "Koopa Troopa para bupa bullet bill my aunt Jill pair on a plant obama's ramp big boo little "
    "boo medium blue id miss you both womp YZ Winfrey involve impeaching chomp chain womp "
    "teeny-weeny peeny beanie lakitu brackets lickety-splickety spiny whiny two-tole lining BOM BA "
    "Bom boombah boombah pokey whoa key lucky dokey boom boom boom banzai banzai billy harrington "
    "and shocky klepto fly you Nagi ill sushi mushi golden tiger roll and jolly roger bay home it's "
    "time for dinner bill blaster when bob ross ah space micro bloomerang medication beach grindle "
    "spindle skeeter bubba you're in trouble bully her amamiya super cooper jamboree oh mama "
    "heave-ho mister blizzard holy golden lizard mad paino who always scared me as a kid in Super "
    "Mario 64 bub foosh don't blow me off the mountain Nathan money bag bunny slag bombs snuff - "
    "Chuck yeah lickiest Uschi shark mister i booking bigger mystery Jayant eyeball killer chair "
    "flips in all of you from one room to be another bandi Namco entertainment Japanese video game "
    "development bandy";

static const struct {
    u8 read;
    u8 dialog;
} sSimpleFlipsMessages[] = {
#define SIMPLEFLIPS_MESSAGE(read, text) { read, DIALOG_SIMPLEFLIPS_READ_##read },
#include "simpleflips_messages.inc.h"
#undef SIMPLEFLIPS_MESSAGE
};
STATIC_ASSERT(DIALOG_SIMPLEFLIPS_MOTOS <= 255, "SimpleFlips sign IDs must fit the parameter byte");

s32 jrb_simpleflips_room_active(void) {
    return gCurrLevelNum == LEVEL_JRB && gCurrentArea != NULL && gCurrAreaIndex == JRB_SIMPLEFLIPS_AREA;
}

s32 jrb_simpleflips_is_game_over(void) {
    return sSimpleFlipsGameOver;
}

s32 jrb_simpleflips_exit_locked(void) {
    // A blocked debug visit has no active NPCs to finish the encounter.
    // Once Toad makes contact, preserve the forced game-over route.
    return jrb_simpleflips_room_active() && (!game_options_secrets_blocked() || sSimpleFlipsGameOver);
}

s32 lvl_simpleflips_boot(UNUSED s16 arg, UNUSED s32 unusedArg) {
    s32 fileNum = save_file_consume_simpleflips_game_over();

    if (fileNum == 0) {
        return FALSE;
    }
    gCurrSaveFileNum = fileNum;
    sSimpleFlipsGameOver = TRUE;
    return TRUE;
}

void jrb_simpleflips_confine_mario(struct MarioState *m) {
    f32 x;
    f32 y;
    f32 z;

    if (!jrb_simpleflips_room_active()) {
        return;
    }
    // Keep collision failures and debug movement from stranding Mario outside the box.
    // The walls are at +/-900 and the ceiling at 1000; retain body clearance.
    x = CLAMP(m->pos[0], -850.0f, 850.0f);
    y = CLAMP(m->pos[1], 0.0f, 840.0f);
    z = CLAMP(m->pos[2], -850.0f, 850.0f);
    if (m->pos[0] != x || m->pos[2] != z) {
        mario_set_forward_vel(m, 0.0f);
    }
    if (m->pos[1] != y) {
        m->vel[1] = 0.0f;
    }
    if (m->pos[0] != x || m->pos[1] != y || m->pos[2] != z) {
        vec3f_set(m->pos, x, y, z);
        vec3f_copy(m->marioObj->header.gfx.pos, m->pos);
    }
    if (!sSimpleFlipsGameOver) {
        if (m->health < 0x100) {
            set_mario_action(m, ACT_IDLE, 0);
        } else if (m->action == ACT_DEBUG_FREE_MOVE) {
            set_mario_action(m, ACT_IDLE, 0);
        }
        // Only Toad can take away health in this encounter.
        m->health = 0x880;
        m->hurtCounter = 0;
        m->healCounter = 0;
    }
}

void jrb_simpleflips_reset(void) {
    sSimpleFlipsGameOver = FALSE;
    sSimpleFlipsReadCount = 0;
    sSimpleFlipsReadEffectsPending = FALSE;
    sSimpleFlipsRoomRed = FALSE;
    sSimpleFlipsRoomMusicAllowed = FALSE;
    sSimpleFlipsFinalReadComplete = FALSE;
    // Area unloading owns object deletion; never dereference the old pointer here.
    sSimpleFlipsMips = NULL;
}

void jrb_simpleflips_restore_cap(struct MarioState *m) {
    m->flags &= ~MARIO_CAP_IN_HAND;
    m->flags |= MARIO_NORMAL_CAP | MARIO_CAP_ON_HEAD;
    m->capPickupLockFrames = 0;
    m->capPickupLockStartFrame = 0;
    // The visit marker is saved immediately afterward, including these cap flags.
    save_file_clear_flags(SAVE_FLAG_CAP_ON_GROUND | SAVE_FLAG_CAP_ON_KLEPTO | SAVE_FLAG_CAP_ON_UKIKI
                          | SAVE_FLAG_CAP_ON_MR_BLIZZARD);
}

s32 jrb_simpleflips_music_allowed(u8 player, u8 seqId) {
    return !jrb_simpleflips_room_active() || player == SEQ_PLAYER_SFX
           || (sSimpleFlipsRoomMusicAllowed && player == SEQ_PLAYER_LEVEL
               && (seqId & SEQ_BASE_ID) == SEQ_LEVEL_PHYSICS_LAB);
}

void jrb_simpleflips_begin_sign_read(struct Object *sign) {
    u32 i;

    if (!jrb_simpleflips_room_active() || !obj_has_behavior(sign, bhvJrbSimpleFlipsSign)
        || sSimpleFlipsFinalReadComplete || sSimpleFlipsReadEffectsPending) {
        return;
    }
    if (sSimpleFlipsReadCount < SIMPLEFLIPS_FINAL_READ) {
        sSimpleFlipsReadCount++;
    }
    sign->oBehParams2ndByte = DIALOG_SIMPLEFLIPS_SIGN;
    for (i = 0; i < ARRAY_COUNT(sSimpleFlipsMessages); i++) {
        if (sSimpleFlipsMessages[i].read == sSimpleFlipsReadCount) {
            sign->oBehParams2ndByte = sSimpleFlipsMessages[i].dialog;
            break;
        }
    }
    sSimpleFlipsReadEffectsPending = TRUE;
}

Gfx *geo_simpleflips_room(s32 callContext, struct GraphNode *node, UNUSED void *context) {
    Gfx *displayList;
    Gfx *head;

    if (callContext != GEO_CONTEXT_RENDER || !jrb_simpleflips_room_active()) {
        return NULL;
    }
    displayList = alloc_display_list(4 * sizeof(Gfx));
    if (displayList == NULL) {
        return NULL;
    }
    SET_GRAPH_NODE_LAYER(node->flags, LAYER_OPAQUE);
    head = displayList;
    gDPPipeSync(head++);
    gDPSetPrimColor(head++, 0, 0, sSimpleFlipsRoomRed ? 36 : 0, 0, 0, 255);
    gSPDisplayList(head++, jrb_simpleflips_room_dl);
    gSPEndDisplayList(head++);
    return displayList;
}

static void simpleflips_restore_black_room(void) {
    sSimpleFlipsRoomRed = FALSE;
    sSimpleFlipsRoomMusicAllowed = FALSE;
    stop_background_music(SEQUENCE_ARGS(4, SEQ_LEVEL_PHYSICS_LAB));
    fadeout_music(0);
    if (sSimpleFlipsMips != NULL) {
        obj_mark_for_deletion(sSimpleFlipsMips);
        sSimpleFlipsMips = NULL;
    }
}

// Effects start after Mario finishes reading, so the milestone remains visible first.
static void simpleflips_finish_sign_read(void) {
    switch (sSimpleFlipsReadCount) {
        case 26:
            sSimpleFlipsRoomRed = TRUE;
            break;
        case 55:
            sSimpleFlipsRoomMusicAllowed = TRUE;
            play_music(SEQ_PLAYER_LEVEL, SEQUENCE_ARGS(4, SEQ_LEVEL_PHYSICS_LAB), 0);
            break;
        case 70:
            sSimpleFlipsMips = spawn_object(o, MODEL_JRB_SIMPLEFLIPS_MIPS, bhvJrbSimpleFlipsMips);
            sSimpleFlipsMips->oPosX += 250.0f;
            break;
        case 95:
            spawn_object(o, MODEL_EXPLOSION, bhvJrbSimpleFlipsExplosion);
            o->oAction = SIMPLEFLIPS_SIGN_HIDDEN;
            // This state runs below in the same update; discard the sign's old timer.
            o->oTimer = 0;
            o->oInteractType = INTERACT_NONE;
            cur_obj_hide();
            cur_obj_become_intangible();
            break;
        case 105:
            spawn_object_relative(0, 0, 200, 0, o, MODEL_1UP, bhvJrbSimpleFlipsOneUp);
            break;
        case SIMPLEFLIPS_FINAL_READ:
            sSimpleFlipsFinalReadComplete = TRUE;
            simpleflips_restore_black_room();
            o->oInteractType = INTERACT_NONE;
            break;
    }
}

void bhv_jrb_simpleflips_mips_loop(void) {
    cur_obj_init_animation(0);
    if (o->oAction == 0 && (o->oInteractStatus & INT_STATUS_INTERACTED)) {
        o->oAction = 1;
    }
    if (o->oAction == 1
        && cur_obj_update_dialog(MARIO_DIALOG_LOOK_DOWN, DIALOG_FLAG_TEXT_DEFAULT,
                                 DIALOG_SIMPLEFLIPS_MIPS, 0)) {
        o->oAction = 2;
        o->oInteractType = INTERACT_NONE;
        cur_obj_become_intangible();
    }
    o->oInteractStatus = INT_STATUS_NONE;
}

static void simpleflips_motos_disappear(void) {
    struct WaterDropletParams droplets = {
        .flags = WATER_DROPLET_FLAG_RAND_ANGLE,
        .model = MODEL_WHITE_PARTICLE_SMALL,
        .behavior = bhvJrbSimpleFlipsMotosDroplet,
        .randForwardVelOffset = 5.0f,
        .randForwardVelScale = 3.0f,
        .randYVelOffset = 30.0f,
        .randYVelScale = 20.0f,
        .randSizeOffset = 0.5f,
        .randSizeScale = 1.0f,
    };
    s32 count = particle_budget_burst_count(PARTICLE_PRIORITY_DROPLET, 6);

    // This dry room has no water plane; keep the splash at Motos's position.
    try_spawn_particle(o, MODEL_WATER_SPLASH, bhvJrbSimpleFlipsMotosSplash, PARTICLE_PRIORITY_FEEDBACK);
    for (s32 i = 0; i < count; i++) {
        if (spawn_water_droplet(o, &droplets) == NULL) {
            break;
        }
    }
    create_sound_spawner(SOUND_OBJ_DIVING_IN_WATER);
    cur_obj_hide();
    cur_obj_become_intangible();
    o->oInteractType = INTERACT_NONE;
    obj_mark_for_deletion(o);
}

void bhv_jrb_simpleflips_motos_loop(void) {
    o->activeFlags |= ACTIVE_FLAG_DITHERED_ALPHA;
    cur_obj_scale(2.0f);
    cur_obj_init_animation(8); // The imported Motos idle animation.
    if (o->oAction == 0) {
        if (game_options_secrets_blocked() || !jrb_simpleflips_room_active() || sSimpleFlipsGameOver
            || (gMarioState->flags & MARIO_CAPS)) {
            cur_obj_hide();
            cur_obj_become_intangible();
            o->oInteractType = INTERACT_NONE;
            o->oInteractStatus = INT_STATUS_NONE;
            return;
        }
        cur_obj_unhide();
        cur_obj_become_tangible();
        o->oInteractType = INTERACT_TEXT;
        if (o->oInteractStatus & INT_STATUS_INTERACTED) {
            o->oAction = 1;
        }
    }
    // Finish normal dialogue cleanup even if Mario regains his cap while talking.
    if (o->oAction == 1
        && cur_obj_update_dialog(MARIO_DIALOG_LOOK_UP, DIALOG_FLAG_TEXT_DEFAULT,
                                 DIALOG_SIMPLEFLIPS_MOTOS, 0)) {
        save_file_set_castle_secret(gCurrSaveFileNum - 1, CASTLE_SECRET_MOTOS_TALKED);
        save_file_do_save(gCurrSaveFileNum - 1);
        o->oAction = 2;
        simpleflips_motos_disappear();
    }
    o->oInteractStatus = INT_STATUS_NONE;
}

void bhv_jrb_simpleflips_one_up_loop(void) {
    if (o->oTimer == 0) {
        play_sound(SOUND_GENERAL2_1UP_APPEAR, gGlobalSoundSource);
    }
    if (o->oTimer > 17) {
        one_up_spawn_sparkles();
    }
    if (o->oTimer < SIMPLEFLIPS_ONE_UP_TWIRL_FRAMES) {
        one_up_loop_in_air();
        if (o->oTimer == SIMPLEFLIPS_ONE_UP_TWIRL_FRAMES - 1) {
            cur_obj_become_tangible();
        }
    } else {
        pole_1up_move_towards_mario();
    }
    cur_obj_move_xz_using_fvel_and_yaw();
    o->oPosY += o->oVelY;
}

Gfx *geo_simpleflips_camera(s32 callContext, struct GraphNode *node, void *context) {
    struct GraphNodeCamera *gc = (struct GraphNodeCamera *) node;

    geo_camera_main(callContext, node, context);
    if (callContext == GEO_CONTEXT_RENDER && jrb_simpleflips_room_active()) {
        struct Camera *c = gc->config.camera;

        // Stay inside the box and show both characters from a diagonal angle.
        vec3f_set(gc->pos, CLAMP(gMarioState->pos[0] + 650.0f, -800.0f, 800.0f),
                  CLAMP(gMarioState->pos[1] + 250.0f, 100.0f, 850.0f),
                  CLAMP(gMarioState->pos[2] + 250.0f, -800.0f, 800.0f));
        // Aim between Mario and the sign, including while approaching from spawn.
        vec3f_set(gc->focus, gMarioState->pos[0] * 0.5f, gMarioState->pos[1] + 80.0f,
                  (gMarioState->pos[2] - 150.0f) * 0.5f);
        if (!game_options_secrets_blocked() && !(gMarioState->flags & MARIO_CAPS)
            && gMarioState->pos[0] < -300.0f && gMarioState->pos[2] > 300.0f) {
            // Keep Mario and the distant Motos in view when approaching that corner.
            vec3f_set(gc->focus, (gMarioState->pos[0] - 600.0f) * 0.5f, gMarioState->pos[1] + 100.0f,
                      (gMarioState->pos[2] + 650.0f) * 0.5f);
        }
        gc->rollScreen = 0;
        vec3f_copy(c->pos, gc->pos);
        vec3f_copy(c->focus, gc->focus);
        vec3f_copy(gLakituState.pos, gc->pos);
        vec3f_copy(gLakituState.focus, gc->focus);
        c->nextYaw = atan2s(gc->pos[2] - gc->focus[2], gc->pos[0] - gc->focus[0]);
        c->yaw = c->nextYaw;
        gLakituState.nextYaw = c->yaw;
        gLakituState.yaw = gLakituState.nextYaw;
    }
    return NULL;
}

Gfx *geo_switch_simpleflips_head(s32 callContext, struct GraphNode *node, UNUSED void *context) {
    if (callContext == GEO_CONTEXT_RENDER) {
        ((struct GraphNodeSwitchCase *) node)->selectedCase = jrb_simpleflips_is_game_over();
    }
    return NULL;
}

void bhv_jrb_simpleflips_window_loop(void) {
    // Ship water starts at 2765 and falls once the chest puzzle begins draining it.
    // The window remains visually unchanged and has no solid object collision.
    if (game_options_secrets_blocked() || gCurrActNum != 1 || gCurrAreaIndex != 2
        || gEnvironmentRegions == NULL || gEnvironmentRegions[6] >= 2765
        || save_file_has_simpleflips_encounter(gCurrSaveFileNum - 1)) {
        cur_obj_become_intangible();
    } else {
        cur_obj_become_tangible();
    }
    o->oInteractStatus = INT_STATUS_NONE;
}

static void simpleflips_transform_sign(void) {
    simpleflips_restore_black_room();
    cur_obj_set_model(MODEL_JRB_SIMPLEFLIPS_TOAD);
    o->oAnimations = (void *) jrb_simpleflips_anims;
    o->oOpacity = 255;
    o->header.gfx.animInfo.curAnim = NULL;
    cur_obj_init_animation(0);
    o->oInteractionSubtype = INT_SUBTYPE_NPC;
    o->hitboxRadius = 80.0f;
    o->hitboxHeight = 120.0f;
    o->oAction = SIMPLEFLIPS_TOAD;
}

static void simpleflips_start_punch(void) {
    struct MarioState *m = gMarioState;

    cur_obj_set_model(MODEL_JRB_SIMPLEFLIPS_PUNCH);
    o->header.gfx.animInfo.curAnim = NULL;
    cur_obj_init_animation(1);
    o->header.gfx.animInfo.animYTrans = 189;
    o->oMoveAngleYaw = atan2s(m->pos[2] - o->oPosZ, m->pos[0] - o->oPosX);
    o->oPosX = m->pos[0] - SIMPLEFLIPS_PUNCH_DISTANCE * sins(o->oMoveAngleYaw);
    o->oPosZ = m->pos[2] - SIMPLEFLIPS_PUNCH_DISTANCE * coss(o->oMoveAngleYaw);
    o->oInteractType = INTERACT_NONE;
    cur_obj_become_intangible();
    // Keep Mario in the NPC action until the contact frame; normal time resumes.
    o->oAction = SIMPLEFLIPS_PUNCH;
}

static void simpleflips_punch_mario(void) {
    struct MarioState *m = gMarioState;

    sSimpleFlipsGameOver = TRUE;
    player_stats_damage();
    player_stats_health(m->health, 0xFF);
    m->health = 0xFF;
    m->hurtCounter = 0;
    m->healCounter = 0;
    m->respawnHealCounter = 0;
    m->invincTimer = 30;
    m->faceAngle[1] = o->oMoveAngleYaw + 0x8000;
    drop_and_set_mario_action(m, ACT_HARD_BACKWARD_AIR_KB, 0);
    mario_set_forward_vel(m, -60.0f);
    m->vel[1] = 40.0f;
    cur_obj_play_sound_2(SOUND_ACTION_HIT);
    o->oAction = SIMPLEFLIPS_FINISHED;
}

void bhv_jrb_simpleflips_sign_loop(void) {
    if (!jrb_simpleflips_room_active()) {
        cur_obj_hide();
        cur_obj_become_intangible();
        return;
    }

    // Debug warps can reach the room without using its window. Keep an existing
    // read's cleanup running, but block new sign/Toad interactions and pounding.
    if (game_options_secrets_blocked() && !sSimpleFlipsReadEffectsPending
        && (o->oAction == SIMPLEFLIPS_SIGN || o->oAction == SIMPLEFLIPS_TOAD)) {
        cur_obj_hide();
        cur_obj_become_intangible();
        o->oInteractType = INTERACT_NONE;
        o->oInteractStatus = INT_STATUS_NONE;
        return;
    }
    if (o->oAction == SIMPLEFLIPS_SIGN || o->oAction == SIMPLEFLIPS_TOAD) {
        cur_obj_unhide();
        cur_obj_become_tangible();
        o->oInteractType = INTERACT_TEXT;
    }

    if (sSimpleFlipsReadEffectsPending
        && !(gMarioState->action == ACT_READING_SIGN && gMarioState->usedObj == o)) {
        sSimpleFlipsReadEffectsPending = FALSE;
        simpleflips_finish_sign_read();
    }

    switch (o->oAction) {
        case SIMPLEFLIPS_SIGN:
            if (cur_obj_is_mario_ground_pounding_platform()) {
                if (sSimpleFlipsFinalReadComplete) {
                    save_file_mark_simpleflips_game_over();
                    __n64Assert_MessageBuf[0] = 0;
                    __n64Assert(__FILE__, __LINE__, sSimpleFlipsEnding);
                } else {
                    simpleflips_transform_sign();
                }
            } else {
                load_object_collision_model();
            }
            break;

        case SIMPLEFLIPS_TOAD:
            if (o->oInteractStatus & INT_STATUS_INTERACTED) {
                o->oAction = SIMPLEFLIPS_DIALOG;
            }
            break;

        case SIMPLEFLIPS_DIALOG:
            if (cur_obj_update_dialog(MARIO_DIALOG_LOOK_DOWN,
                                      DIALOG_FLAG_TEXT_DEFAULT | DIALOG_FLAG_TIME_STOP_ENABLED,
                                      DIALOG_SIMPLEFLIPS_TOAD, 0)) {
                player_stats_add(PS_TOAD_CONVERSATIONS, 1);
                simpleflips_start_punch();
            }
            break;

        case SIMPLEFLIPS_PUNCH:
            if (o->oTimer < SIMPLEFLIPS_PUNCH_WINDUP) {
                o->header.gfx.animInfo.animFrame = 0;
            }
            if (o->oTimer == SIMPLEFLIPS_PUNCH_CONTACT) {
                simpleflips_punch_mario();
            }
            break;

        case SIMPLEFLIPS_FINISHED:
            if (o->oTimer == SIMPLEFLIPS_FRAMES_PER_SECOND / 2) {
                // Native Toad's wave pose turns his face 30 degrees to the right.
                o->oMoveAngleYaw -= SIMPLEFLIPS_TOAD_IDLE_YAW_OFFSET;
                cur_obj_set_model(MODEL_JRB_SIMPLEFLIPS_TOAD);
                o->header.gfx.animInfo.curAnim = NULL;
                o->header.gfx.animInfo.animYTrans = 0;
                cur_obj_init_animation(0);
            }
            break;

        case SIMPLEFLIPS_SIGN_HIDDEN:
            if (o->oTimer >= SIMPLEFLIPS_FRAMES_PER_SECOND * 5) {
                cur_obj_unhide();
                o->oOpacity = 0;
                o->oAction = SIMPLEFLIPS_SIGN_FADING;
                cur_obj_become_tangible();
            }
            break;

        case SIMPLEFLIPS_SIGN_FADING:
            o->oOpacity = MIN(o->oOpacity + 8, 255);
            load_object_collision_model();
            if (o->oOpacity == 255) {
                o->oInteractType = INTERACT_TEXT;
                o->oAction = SIMPLEFLIPS_SIGN;
            }
            break;
    }

    o->oInteractStatus = INT_STATUS_NONE;
}
