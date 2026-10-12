#include "game_options_runtime.h"
#include "sm64.h"
#include "mario_character.h"
#include "mario_actions_moving.h"
#include "mario_actions_cutscene.h"
#include "area.h"
#include "level_table.h"
#include "level_update.h"
#include "game_init.h"
#include "ingame_menu.h"
#include "game_options.h"
#include "save_file.h"
#include "behavior_data.h"
#include "object_helpers.h"
#include "object_list_processor.h"

const struct MarioMovementProfile gMarioMovementProfiles[1] = {
    { WALK_TARGET_SPEED, 1.375f, JUMP_HEIGHT_VELOCITY_SCALE, JUMP_FORWARD_SPEED_RETENTION, 0.20f },
};

static const struct {
    u8 movement;
    enum MarioTextStyle text;
} sPalettes[] = {
    { 0, MARIO_TEXT_STANDARD },
    { 0, MARIO_TEXT_HE_THICC },
};

// Order also defines the children of geo_switch_mario_character.
static const struct MarioCharacterSelection sSelections[MARIO_CHARACTER_COUNT] = {
    { MARIO_APPEARANCE_NORMAL, MARIO_PALETTE_STANDARD },
    { MARIO_APPEARANCE_NORMAL, MARIO_PALETTE_HE_THICC },
    { MARIO_APPEARANCE_SIMPLIFIED, MARIO_PALETTE_STANDARD },
    { MARIO_APPEARANCE_SIMPLIFIED, MARIO_PALETTE_HE_THICC },
};
static const char *const sNames[MARIO_CHARACTER_COUNT] = {
    "Mario / Standard",
    "Mario / HE THICC",
    "Simple Mario / Standard",
    "Simple Mario / HE THICC",
};
// Indices into sSelections; demos/credits can use index 0 without losing the request.
static u8 sRequested;
static u8 sActive;
// Keep the last gameplay selection so temporary defaults do not reset fast walk.
static u8 sLastOrdinary;

void mario_character_init(void) {
    sLastOrdinary = 0;
    sActive = 0;
    sRequested = 0;
}

s32 mario_character_request(s32 index) {
    if (index < 0 || index >= MARIO_CHARACTER_COUNT) {
        return FALSE;
    }
    if (sSelections[index].palette
        != (save_file_is_he_thicc(gCurrSaveFileNum - 1) ? MARIO_PALETTE_HE_THICC
                                                        : MARIO_PALETTE_STANDARD)) {
        return FALSE;
    }
    sRequested = index;
    return TRUE;
}

void mario_character_load_file(s32 fileIndex) {
    sRequested = save_file_is_he_thicc(fileIndex) ? 1 : 0;
}

// The courtyard's sole water box is the L is Real fountain. Check the shipped
// bounds and water height, rather than accepting any low pose elsewhere.
s32 mario_character_can_unlock(struct MarioState *m) {
    struct Object *head;
    struct Object *obj;
    s32 courtyardBigBooPresent = FALSE;

    if (game_options_secrets_blocked()
        || !(gCurrDemoInput == NULL && gCurrCreditsEntry == NULL
             && gCurrLevelNum == LEVEL_CASTLE_COURTYARD && gCurrAreaIndex == 1 && m->action == ACT_DAB
             && m->waterLevel == 51
             // Check before drowning's damage/sink handlers can remove the cap.
             && !(m->flags & (MARIO_CAP_ON_HEAD | MARIO_CAP_IN_HAND)) && m->pos[0] > -656.0f
             && m->pos[0] < 674.0f && m->pos[2] > -2405.0f && m->pos[2] < -1074.0f)) {
        return FALSE;
    }

    // Count living ordinary Boos even when invisible, stunned or out of range.
    // The courtyard cage carrier must survive; other Big Boo behaviours are exempt.
    head = (struct Object *) &gObjectLists[OBJ_LIST_GENACTOR];
    for (obj = (struct Object *) head->header.next; obj != head;
         obj = (struct Object *) obj->header.next) {
        if (obj->activeFlags != ACTIVE_FLAG_DEACTIVATED
            && obj_has_behavior(obj, bhvBooWithCage)
            // Match Boo's visible/appearing threshold, excluding a fade back to invisibility.
            && obj->oOpacity > 180 && obj->oBooTargetOpacity == 255
            && obj->oBooDeathStatus == BOO_DEATH_STATUS_ALIVE && obj->oAction != 3) {
            courtyardBigBooPresent = TRUE;
        }
        if (obj->activeFlags != ACTIVE_FLAG_DEACTIVATED
            && (obj_has_behavior(obj, bhvGhostHuntBoo) || obj_has_behavior(obj, bhvBoo)
                || obj_has_behavior(obj, bhvMerryGoRoundBoo))
            && obj->oBooDeathStatus == BOO_DEATH_STATUS_ALIVE
            && obj->oAction != 3) { // Fatal hit already accepted; death animation may not have run yet.
            return FALSE;
        }
    }
    return courtyardBigBooPresent;
}

s32 mario_character_is_unlock_death(struct MarioState *m) {
    return m->action == ACT_QUICKSAND_DEATH && m->actionArg == ACT_ARG_QUICKSAND_DEATH_HE_THICC_UNLOCK;
}

s32 mario_character_update_tas_code(u8 *progress, u32 buttons) {
    u32 first = L_CBUTTONS | R_CBUTTONS;
    u32 second = U_CBUTTONS | D_CBUTTONS;
    u32 expected = (*progress & 1) ? second : first;

    if (buttons != expected) {
        *progress = buttons == first ? 1 : 0;
        return FALSE;
    }
    if (++*progress == MARIO_TAS_CODE_FRAMES) {
        *progress = 0;
        return TRUE;
    }
    return FALSE;
}

void mario_character_commit(struct MarioState *m, s32 useDefault) {
    if (useDefault) {
        sActive = 0;
    } else {
        if (sRequested != sLastOrdinary) {
            gDebugFastWalk = FALSE;
        }
        sActive = sRequested;
        sLastOrdinary = sActive;
    }
    m->movementProfile = mario_character_movement();
}

// Compatible model branches share bones, palette and physics, so appearance
// changes can be displayed immediately without committing a different character.
void mario_character_apply_appearance(s32 useDefault) {
    if (useDefault || sSelections[sRequested].palette != sSelections[sActive].palette) {
        return;
    }
    sActive = sRequested;
    sLastOrdinary = sActive;
}

s32 mario_character_active_index(void) {
    return sActive;
}

s32 mario_character_requested_index(void) {
    return sRequested;
}

u8 mario_character_palette(void) {
    return sSelections[sActive].palette;
}

enum MarioTextStyle mario_character_text_style(void) {
    if (gMenuMode == MENU_MODE_RENDER_PAUSE_SCREEN || game_options_get(GO_DISABLE_THICC_TEXT)) {
        return MARIO_TEXT_STANDARD;
    }
    return sPalettes[mario_character_palette()].text;
}

const struct MarioMovementProfile *mario_character_movement(void) {
    return &gMarioMovementProfiles[sPalettes[mario_character_palette()].movement];
}

const char *mario_character_name(s32 index) {
    return sNames[(index >= 0 && index < MARIO_CHARACTER_COUNT) ? index : 0];
}

const char *mario_character_movement_name(void) {
    return "Current hack";
}

s32 mario_character_is_simplified(void) {
    return sSelections[sActive].appearance == MARIO_APPEARANCE_SIMPLIFIED;
}
