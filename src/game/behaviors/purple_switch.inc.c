
/**
 * Behavior for bhvFloorSwitchHardcodedModel, bhvFloorSwitchGrills, and
 * bhvFloorSwitchAnimatesObject.
 *
 * This controls the purple switches that Mario can step on to affect parts of
 * the environment.
 */

void bhv_purple_switch_loop(void) {
    s32 poundImpact = game_options_get(GO_SWITCHES) == 0 && cur_obj_is_mario_ground_pounding_platform();
    s32 onPlatform = cur_obj_is_mario_on_platform() || poundImpact;
    s32 canPress = onPlatform && (poundImpact || !(gMarioStates[0].action & MARIO_NO_PURPLE_SWITCH))
                   && lateral_dist_between_objects(o, gMarioObject) < 127.5f;

    if (o->oAction == PURPLE_SWITCH_ACT_IDLE && o->oTimer == 0
        && o->oPurpleSwitchActivationSerial == 0) {
        cur_obj_set_model(MODEL_PURPLE_SWITCH);
        cur_obj_scale(1.5f);
    }

    if (!onPlatform) {
        o->oPurpleSwitchPressedLatch = FALSE;
    } else if (canPress && !o->oPurpleSwitchPressedLatch) {
        // Publish one persistent press event. The effect runs after Mario steps
        // away, and another press can refresh it as soon as he returns.
        o->oPurpleSwitchPressedLatch = TRUE;
        o->oPurpleSwitchActivationSerial++;
        if (o->oBehParams2ndByte == PURPLE_SWITCH_BP_REVEAL_HIDDEN) {
            o->oPurpleSwitchEffectTimer = PURPLE_SWITCH_HIDDEN_ACTIVE_FRAMES;
        }
        cur_obj_play_sound_2(SOUND_GENERAL2_PURPLE_SWITCH);
        cur_obj_shake_screen(SHAKE_POS_SMALL);
        queue_rumble_data(5, 80);
    }

    if (o->oPurpleSwitchEffectTimer > 0) {
        o->oPurpleSwitchEffectTimer--;
        play_sound(o->oPurpleSwitchEffectTimer < PURPLE_SWITCH_HIDDEN_BLINK_FRAMES
                       ? SOUND_GENERAL2_SWITCH_TICK_SLOW
                       : SOUND_GENERAL2_SWITCH_TICK_FAST,
                   gGlobalSoundSource);
    }

    // Physical height follows contact; it is independent of the triggered
    // stair, box, or grill lifetime and can reverse before fully rising.
    if (onPlatform && o->oPurpleSwitchPressedLatch) {
        o->header.gfx.scale[1] = approach_f32(o->header.gfx.scale[1], 0.2f, 0.43333334f, 0.43333334f);
        o->oAction =
            o->header.gfx.scale[1] > 0.2f ? PURPLE_SWITCH_ACT_PRESSED : PURPLE_SWITCH_ACT_TICKING;
    } else {
        o->header.gfx.scale[1] = approach_f32(o->header.gfx.scale[1], 1.5f, 0.43333334f, 0.43333334f);
        o->oAction =
            o->header.gfx.scale[1] < 1.5f ? PURPLE_SWITCH_ACT_UNPRESSED : PURPLE_SWITCH_ACT_IDLE;
    }
}
