#include "actors/group15.h"

#define CASTLE_LOBBY_FIRST_GUEST_DELAY (15 * 30)
#define CASTLE_LOBBY_FADE_FRAMES 32
#define CASTLE_LOBBY_SUN_FADE_FRAMES 30

STATIC_ASSERT(DIALOG_CASTLE_LOBBY_SIGN <= 255, "Finale sign ID must fit the parameter byte");

enum CastleLobbyGuest {
    CASTLE_LOBBY_PEACH,
    CASTLE_LOBBY_BOBOMB,
    CASTLE_LOBBY_KOOPA,
    CASTLE_LOBBY_SIGN,
    CASTLE_LOBBY_BOWSER,
    CASTLE_LOBBY_PIRANHA,
    CASTLE_LOBBY_MOTOS,
    CASTLE_LOBBY_TOAD,
    CASTLE_LOBBY_PEACH_FINALE,
    CASTLE_LOBBY_MIPS,
    CASTLE_LOBBY_FINISHED,
};

enum CastleLobbyGuestAction {
    CASTLE_LOBBY_GUEST_ARRIVING,
    CASTLE_LOBBY_GUEST_READY,
    CASTLE_LOBBY_GUEST_TALKING,
    CASTLE_LOBBY_GUEST_DONE,
};

struct CastleLobbyGuestInfo {
    s16 model;
    s16 x, z;
    s16 dialog, animation;
    f32 scale;
    f32 focusHeight;
    s16 hitboxRadius, hitboxHeight;
    const struct Animation *const *animations;
};

// Clockwise from the carpet's forward point; MIPS arrives in the center afterward.
static const struct CastleLobbyGuestInfo sCastleLobbyGuests[] = {
    { MODEL_CASTLE_LOBBY_PEACH, 0, -1064, DIALOG_CASTLE_LOBBY_PEACH, PEACH_ANIM_0, 1.0f, 220.0f, 60,
      240, castle_lobby_peach_seg5_anims_0501C41C },
    { MODEL_CASTLE_LOBBY_BOBOMB, 752, -752, DIALOG_CASTLE_LOBBY_BOBOMB, 0, 1.0f, 90.0f, 50, 100,
      castle_lobby_bobomb_seg8_anims_0802396C },
    { MODEL_CASTLE_LOBBY_KOOPA, 1064, 0, DIALOG_CASTLE_LOBBY_KOOPA, KOOPA_ANIM_STOPPED, 3.0f, 300.0f,
      120, 360, castle_lobby_koopa_seg6_anims_06011364 },
    { MODEL_YOSHI_CREDIT_SIGN, 752, 752, DIALOG_CASTLE_LOBBY_SIGN, 0, 1.0f, 100.0f, 60, 150, NULL },
    { MODEL_CASTLE_LOBBY_BOWSER, 0, 1064, DIALOG_CASTLE_LOBBY_BOWSER, BOWSER_ANIM_IDLE, 1.0f, 400.0f,
      240, 500, castle_lobby_bowser_seg6_anims_06057690 },
    { MODEL_CASTLE_LOBBY_PIRANHA, -752, 752, DIALOG_CASTLE_LOBBY_PIRANHA, PIRANHA_PLANT_ANIM_SLEEPING,
      1.0f, 180.0f, 100, 240, castle_lobby_piranha_plant_seg6_anims_0601C31C },
    { MODEL_CASTLE_LOBBY_MOTOS, -1064, 0, DIALOG_CASTLE_LOBBY_MOTOS_NEW, 8, 2.0f, 280.0f, 120, 340,
      castle_lobby_motos_anime },
    { MODEL_TOAD, -752, -752, DIALOG_CASTLE_LOBBY_TOAD, 6, 1.0f, 90.0f, 60, 120,
      toad_seg6_anims_0600FB58 },
    { MODEL_CASTLE_LOBBY_PEACH, 0, -1064, DIALOG_CASTLE_LOBBY_PEACH_FINALE, PEACH_ANIM_0, 1.0f, 220.0f,
      60, 240, castle_lobby_peach_seg5_anims_0501C41C },
    { MODEL_CASTLE_LOBBY_MIPS, 0, 0, DIALOG_CASTLE_LOBBY_MIPS, 0, 1.0f, 60.0f, 50, 100,
      mips_seg6_anims_06015634 },
};

static struct Object *sCastleLobbyFinale;
static struct Object *sCastleLobbyPeach;
static s32 sCastleLobbySunReady;
static s32 sCastleLobbySunFadeFrames;
static s32 sCastleLobbyRestoreIndex;
static s32 sCastleLobbyRestoreEnd;

STATIC_ASSERT(CASTLE_LOBBY_FINISHED == CASTLE_LOBBY_CONVERSATION_COUNT,
              "Saved finale progress must cover every conversation");

// World-space face height; ordinary NPC camera and prompt placement stay unchanged.
f32 castle_lobby_guest_focus_height(struct Object *guest) {
    s32 index = GET_BPARAM1(guest->oBehParams);
    if (!obj_has_behavior(guest, bhvCastleLobbyGuest) || index >= CASTLE_LOBBY_FINISHED) {
        return -1.0f;
    }
    return sCastleLobbyGuests[index].focusHeight;
}

s32 castle_lobby_finale_sun_ready(void) {
    return castle_secret_lobby_active() && castle_secret_lobby_rejection_dialog() < 0
           && sCastleLobbySunReady;
}

s32 castle_lobby_finale_sun_alpha(void) {
    return castle_lobby_finale_sun_ready()
               ? 255 * sCastleLobbySunFadeFrames / CASTLE_LOBBY_SUN_FADE_FRAMES
               : 0;
}

void bhv_castle_lobby_finale_init(void) {
    sCastleLobbyFinale = NULL;
    sCastleLobbyPeach = NULL;
    sCastleLobbySunReady = FALSE;
    sCastleLobbySunFadeFrames = 0;
    sCastleLobbyRestoreIndex = 0;
    sCastleLobbyRestoreEnd = 0;
    if (castle_secret_lobby_rejection_dialog() >= 0) {
        obj_mark_for_deletion(o);
    } else {
        sCastleLobbyFinale = o;
        o->oAction = save_file_get_castle_lobby_progress(gCurrSaveFileNum - 1);
        sCastleLobbyRestoreEnd = o->oAction;
        sCastleLobbySunReady = o->oAction == CASTLE_LOBBY_FINISHED;
    }
}

static s32 castle_lobby_spawn_guest(s32 guestIndex, s32 restored) {
    const struct CastleLobbyGuestInfo *info = &sCastleLobbyGuests[guestIndex];
    struct Object *guest = try_to_spawn_object(0, info->scale, o, info->model, bhvCastleLobbyGuest);
    if (guest == NULL) {
        return FALSE; // Retry without skipping a guest or playing a phantom sound.
    }
    guest->oBehParams = (guestIndex << 24) | (restored << 8);
    guest->oPosX = -1023 + info->x;
    guest->oPosY = -200;
    guest->oPosZ = 717 + info->z;
    guest->oFaceAngleYaw = atan2s(-info->z, -info->x);
    guest->oMoveAngleYaw = guest->oFaceAngleYaw;
    if (guestIndex == CASTLE_LOBBY_MIPS) {
        // Face Peach, rather than Mario.
        guest->oMoveAngleYaw = 0x8000;
        guest->oFaceAngleYaw = guest->oMoveAngleYaw;
    }
    if (guestIndex == CASTLE_LOBBY_PEACH) {
        sCastleLobbyPeach = guest;
    }
    return TRUE;
}

void bhv_castle_lobby_finale_loop(void) {
    if (sCastleLobbySunReady && sCastleLobbySunFadeFrames < CASTLE_LOBBY_SUN_FADE_FRAMES) {
        sCastleLobbySunFadeFrames++;
    }
    // Restore completed guests silently before resuming the next unread conversation.
    if (sCastleLobbyRestoreIndex == CASTLE_LOBBY_PEACH_FINALE) {
        sCastleLobbyRestoreIndex++; // Peach's second speech reuses her first object.
    }
    if (sCastleLobbyRestoreIndex < sCastleLobbyRestoreEnd) {
        if (castle_lobby_spawn_guest(sCastleLobbyRestoreIndex, TRUE)) {
            sCastleLobbyRestoreIndex++;
        }
        return;
    }
    s32 guestIndex = o->oAction;
    if (guestIndex >= CASTLE_LOBBY_FINISHED || o->oSubAction != 0
        || (guestIndex == CASTLE_LOBBY_PEACH && o->oTimer < CASTLE_LOBBY_FIRST_GUEST_DELAY)) {
        return;
    }
    if (guestIndex == CASTLE_LOBBY_PEACH_FINALE) {
        // Peach alone has a second conversation after all eight guests have spoken.
        sCastleLobbyPeach->oBehParams = CASTLE_LOBBY_PEACH_FINALE << 24;
        sCastleLobbyPeach->oAction = CASTLE_LOBBY_GUEST_READY;
    } else if (!castle_lobby_spawn_guest(guestIndex, FALSE)) {
        return;
    }
    o->oSubAction = 1;
}

void bhv_castle_lobby_guest_init(void) {
    s32 guestIndex = GET_BPARAM1(o->oBehParams);
    s32 restored = GET_BPARAM3(o->oBehParams);
    const struct CastleLobbyGuestInfo *info = &sCastleLobbyGuests[guestIndex];
    o->oOpacity = 0;
    o->oInteractionSubtype = INT_SUBTYPE_NPC;
    if (info->animations != NULL) {
        o->oAnimations = segmented_to_virtual(info->animations);
    }
    cur_obj_scale(info->scale);
    // These bounds are already in world units, including each guest's model scale.
    o->hitboxRadius = info->hitboxRadius;
    o->hitboxHeight = info->hitboxHeight;
    if (guestIndex == CASTLE_LOBBY_TOAD) {
        // The native wave pose turns his face 30 degrees right of the object yaw.
        o->oFaceAngleYaw = o->oMoveAngleYaw - 0x1555;
    }
    if (guestIndex == CASTLE_LOBBY_SIGN) {
        o->collisionData = segmented_to_virtual(wooden_signpost_seg3_collision_0302DD80);
        o->oInteractionSubtype = INT_SUBTYPE_SIGN;
    } else if (guestIndex == CASTLE_LOBBY_BOWSER && !restored) {
        o->oPosY += 4000.0f;
        o->oOpacity = 255;
    }
    if (restored) {
        o->oAction = CASTLE_LOBBY_GUEST_DONE;
        o->oOpacity = 255;
        if (guestIndex == CASTLE_LOBBY_PIRANHA) {
            o->oPiranhaPlantScale = 1.0f;
        }
    } else if (guestIndex != CASTLE_LOBBY_BOWSER && guestIndex != CASTLE_LOBBY_PIRANHA) {
        cur_obj_play_sound_2(SOUND_ACTION_TELEPORT);
    }
}

static void castle_lobby_guest_finished(struct Object *guest) {
    guest->oAction = CASTLE_LOBBY_GUEST_DONE;
    guest->oInteractType = INTERACT_TEXT;
    guest->oInteractionSubtype = 0; // Solid text hitbox, without sign/NPC dialogue eligibility.
    guest->oIntangibleTimer = 0;
    if (GET_BPARAM1(guest->oBehParams) == CASTLE_LOBBY_BOWSER) {
        play_sound(SOUND_OBJ_BOWSER_LAUGH, gGlobalSoundSource);
    }
    if (sCastleLobbyFinale != NULL
        && sCastleLobbyFinale->oAction == (s32) GET_BPARAM1(guest->oBehParams)) {
        if (GET_BPARAM1(guest->oBehParams) == CASTLE_LOBBY_TOAD) {
            player_stats_add(PS_TOAD_CONVERSATIONS, 1);
        }
        sCastleLobbyFinale->oAction++;
        sCastleLobbyFinale->oSubAction = 0;
        save_file_set_castle_lobby_progress(gCurrSaveFileNum - 1, sCastleLobbyFinale->oAction);
        if (GET_BPARAM1(guest->oBehParams) == CASTLE_LOBBY_MIPS) {
            sCastleLobbySunReady = TRUE;
        }
    }
}

void castle_lobby_finale_sign_read(struct Object *sign) {
    if (obj_has_behavior(sign, bhvCastleLobbyGuest)
        && GET_BPARAM1(sign->oBehParams) == CASTLE_LOBBY_SIGN) {
        castle_lobby_guest_finished(sign);
    }
}

static s32 castle_lobby_guest_arrive(s32 guestIndex) {
    if (guestIndex == CASTLE_LOBBY_BOWSER) {
        if (o->oSubAction == 0) {
            cur_obj_init_animation(BOWSER_ANIM_JUMP);
            o->oVelY = MAX(-60.0f, o->oVelY - 2.0f);
            o->oPosY += o->oVelY;
            if (o->oPosY > -200.0f) {
                return FALSE;
            }
            o->oPosY = -200.0f;
            o->oVelY = 0.0f;
            o->oSubAction = 1;
            cur_obj_init_animation(BOWSER_ANIM_JUMP_STOP);
            spawn_mist_particles_variable(0, 0, 60.0f);
            set_environmental_camera_shake(SHAKE_ENV_BOWSER_JUMP);
            cur_obj_play_sound_2(SOUND_OBJ_BOWSER_WALK);
            cur_obj_play_sound_2(SOUND_OBJ2_BOWSER_ROAR);
            // Presentation only: no shockwave, damage or Mario impact reaction.
            return FALSE;
        }
        cur_obj_init_animation(BOWSER_ANIM_JUMP_STOP);
        if (!cur_obj_check_if_near_animation_end()) {
            return FALSE;
        }
    } else if (guestIndex == CASTLE_LOBBY_PIRANHA) {
        // Use the plant's native grow-from-the-ground state, without enemy attacks or lullaby music.
        piranha_plant_act_respawn();
        o->oOpacity = 255;
        if (o->oPiranhaPlantScale < 1.0f) {
            return FALSE;
        }
        o->oPiranhaPlantScale = 1.0f;
        cur_obj_scale(1.0f);
    } else {
        o->oOpacity = MIN(255, 255 * (o->oTimer + 1) / CASTLE_LOBBY_FADE_FRAMES);
        if (o->oOpacity < 255) {
            return FALSE;
        }
    }
    o->oAction = CASTLE_LOBBY_GUEST_READY;
    return TRUE;
}

void bhv_castle_lobby_guest_loop(void) {
    s32 guestIndex = GET_BPARAM1(o->oBehParams);
    const struct CastleLobbyGuestInfo *info = &sCastleLobbyGuests[guestIndex];
    if (info->animations != NULL
        && !(guestIndex == CASTLE_LOBBY_BOWSER && o->oAction == CASTLE_LOBBY_GUEST_ARRIVING)) {
        cur_obj_init_animation(info->animation);
    }
    if (o->oAction == CASTLE_LOBBY_GUEST_ARRIVING && !castle_lobby_guest_arrive(guestIndex)) {
        o->oInteractType = INTERACT_NONE;
        cur_obj_become_intangible();
        return;
    }
    if (guestIndex == CASTLE_LOBBY_SIGN) {
        bhv_message_panel_loop();
        // Preserve the guest-sized reading volume while the native post sinks.
        o->hitboxHeight = info->hitboxHeight + o->oSignpostSinkDepth;
    }
    s32 tangible = o->oAction == CASTLE_LOBBY_GUEST_READY || o->oAction == CASTLE_LOBBY_GUEST_DONE;
    o->oInteractType = tangible ? INTERACT_TEXT : INTERACT_NONE;
    o->oIntangibleTimer = tangible ? 0 : -1;
    o->oInteractionSubtype = 0;
    if (o->oAction == CASTLE_LOBBY_GUEST_READY) {
        o->oInteractionSubtype = guestIndex == CASTLE_LOBBY_SIGN ? INT_SUBTYPE_SIGN : INT_SUBTYPE_NPC;
    }
    if (guestIndex != CASTLE_LOBBY_SIGN && o->oAction == CASTLE_LOBBY_GUEST_READY
        && (o->oInteractStatus & INT_STATUS_INTERACTED)) {
        o->oAction = CASTLE_LOBBY_GUEST_TALKING;
        cur_obj_play_sound_2(SOUND_ACTION_READ_SIGN);
    }
    s32 dialog = info->dialog;
    if (guestIndex == CASTLE_LOBBY_MOTOS
        && (save_file_get_castle_secret(gCurrSaveFileNum - 1) & CASTLE_SECRET_MOTOS_TALKED)) {
        dialog = DIALOG_CASTLE_LOBBY_MOTOS_KNOWN;
    }
    s32 gaze = info->focusHeight < 100.0f ? MARIO_DIALOG_LOOK_DOWN : MARIO_DIALOG_LOOK_FRONT;
    if (info->focusHeight >= 220.0f) {
        gaze = MARIO_DIALOG_LOOK_UP;
    }
    if (o->oAction == CASTLE_LOBBY_GUEST_TALKING
        && cur_obj_update_dialog_with_cutscene(gaze, DIALOG_FLAG_TURN_TO_MARIO, CUTSCENE_DIALOG,
                                               dialog)) {
        castle_lobby_guest_finished(o);
    }
    // A sign stores a dialog ID directly for Mario's native reading action.
    if (guestIndex == CASTLE_LOBBY_SIGN) {
        o->oBehParams2ndByte = info->dialog;
    }
    if (guestIndex != CASTLE_LOBBY_SIGN) {
        // Dialogue turns move yaw; keep the visible model aligned with that direction.
        o->oFaceAngleYaw = o->oMoveAngleYaw;
        if (guestIndex == CASTLE_LOBBY_TOAD) {
            o->oFaceAngleYaw -= 0x1555; // Compensate for the native wave pose.
        }
    }
    o->oInteractStatus = INT_STATUS_NONE;
}
