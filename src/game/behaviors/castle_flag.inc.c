// castle_flag.inc.c

static u32 sCastleFlagSecrets;
static s32 sCastleGlassWarpContact;

static s32 castle_top_flag_available(void) {
    return !game_options_secrets_blocked()
           && save_file_get_total_star_count(gCurrSaveFileNum - 1, COURSE_NUM_TO_INDEX(COURSE_MIN),
                                             COURSE_NUM_TO_INDEX(COURSE_MAX))
                  >= CASTLE_ROOF_COMPLETION_STARS;
}

void bhv_castle_flag_init(void) {
    o->header.gfx.animInfo.animFrame = random_float() * 28.0f;
    if (o->oBehParams2ndByte == 5) {
        // The top mast reaches Y=8120; the 360-unit flag starts at Y=7608.
        o->oGraphYOffset = 152.0f;
        o->hitboxHeight = 512.0f;
    }
    if (o->oBehParams2ndByte == 1) {
        sCastleFlagSecrets = 0;
        sCastleGlassWarpContact = FALSE;
    }
}

void bhv_castle_flag_loop(void) {
    s32 flag = o->oBehParams2ndByte;
    if (flag == 5 && !castle_top_flag_available()) {
        cur_obj_hide();
        cur_obj_become_intangible();
        return;
    }
    cur_obj_unhide();
    cur_obj_become_tangible();
    if (!game_options_secrets_blocked() && flag >= 1 && flag <= 5 && gMarioState->usedObj == o
        && (gMarioState->action == ACT_TOP_OF_POLE || gMarioState->action == ACT_TOP_OF_POLE_JUMP)
        && !(sCastleFlagSecrets & (1U << (flag - 1)))) {
        s32 count = 0;
        sCastleFlagSecrets |= 1U << (flag - 1);
        for (s32 i = 0; i < 5; i++) {
            count += (sCastleFlagSecrets >> i) & 1U;
        }
        spawn_orange_number(count, 0, 400, 0);
        cur_obj_play_sound_2(SOUND_MENU_COLLECT_SECRET);
        if (count == 5) {
            play_toads_jingle();
        }
    }
    bhv_pole_base_loop();
}

// Sample contact before the flight action can turn it into wall-bonk recovery.
void castle_secret_glass_check_air(struct MarioState *m) {
    sCastleGlassWarpContact = FALSE;
    if (!game_options_secrets_blocked() && gCurrLevelNum == LEVEL_CASTLE_GROUNDS && gCurrAreaIndex == 1
        && sCastleFlagSecrets == 0x1F && (m->action == ACT_SHOT_FROM_CANNON || m->action == ACT_FLYING)
        && absf(m->pos[0]) <= 402.0f && m->pos[1] >= 2201.0f && m->pos[1] <= 3006.0f
        && absf(m->pos[2] + 2037.0f) <= 100.0f) {
        sCastleGlassWarpContact = TRUE;
    }
}

s32 castle_secret_glass_warp_ready(void) {
    return !game_options_secrets_blocked() && gCurrLevelNum == LEVEL_CASTLE_GROUNDS
           && gCurrAreaIndex == 1 && sCastleGlassWarpContact;
}
