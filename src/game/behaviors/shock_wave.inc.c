// shock_wave.inc.c

/**
 * Shockwave scale distance hit points
 */

/**
 * Bowser's shockwave attack main loop
 */
void bhv_bowser_shock_wave_loop(void) {
    s16 fadeFrames = 70;
    s32 waveTimer = o->oTimer;
    o->oAnimState = game_options_get(GO_BOWSER_FIGHTS) == 1;
    if (o->oAnimState) {
        o->oBowserShockWaveScale = waveTimer * 10.0f;
        cur_obj_scale(o->oBowserShockWaveScale);
        if (gGlobalTimer % 3 != 0) {
            o->oOpacity--;
        }
        if (waveTimer > fadeFrames) {
            o->oOpacity -= 5;
        }
        if (o->oOpacity <= 0) {
            obj_mark_for_deletion(o);
        }
        if (waveTimer < fadeFrames && !mario_is_in_air_action()) {
            f32 scale = o->oBowserShockWaveScale;
            if ((scale * 1.9f < o->oDistanceToMario && o->oDistanceToMario < scale * 2.4f)
                || (scale * 4.0f < o->oDistanceToMario && o->oDistanceToMario < scale * 4.8f)) {
                gMarioObject->oInteractStatus |= INT_STATUS_MARIO_SHOCKWAVE;
            }
        }
        return;
    }
    if (o->oBehParams2ndByte == BOWSER_SHOCKWAVE_DELAYED
        || o->oBehParams2ndByte == BOWSER_SHOCKWAVE_HEAVY
        || o->oBehParams2ndByte == BOWSER_SHOCKWAVE_LIGHT) {
        waveTimer -= o->oBehParams2ndByte == BOWSER_SHOCKWAVE_DELAYED ? BOWSER_SHOCKWAVE_DELAY_FRAMES
                                                                      : o->oBowserTimer;
        if (waveTimer < 0) {
            o->oOpacity = 0;
            return;
        }
        if (waveTimer == 0) {
            o->oOpacity = 255;
        }
    }

    // Scale shockwave as the timer goes on
    o->oBowserShockWaveScale =
        waveTimer * (BOWSER_SHOCKWAVE_MAX_RADIUS / ((fadeFrames - 1) * BOWSER_SHOCKWAVE_OUTER_RADIUS));
    cur_obj_scale(o->oBowserShockWaveScale);

    // Slightly reduce opacity each 3 frames
    if (gGlobalTimer % 3 != 0) {
        o->oOpacity--;
    }
    // Reduce opacity faster after 70 frames have passed
    if (waveTimer > fadeFrames) {
        o->oOpacity -= 5;
    }
    // Delete object when it's fully transparent
    if (o->oOpacity <= 0) {
        obj_mark_for_deletion(o);
    }
    // If object times is less than 70 frame and Mario is not in the air...
    if (waveTimer < fadeFrames && !mario_is_in_air_action()) {
        // Narrow the damaging band by 5%, centered inside the visible ring.
        f32 inset = (BOWSER_SHOCKWAVE_OUTER_RADIUS - BOWSER_SHOCKWAVE_INNER_RADIUS) * 0.025f;
        f32 inner = o->oBowserShockWaveScale * (BOWSER_SHOCKWAVE_INNER_RADIUS + inset);
        f32 outer = o->oBowserShockWaveScale * (BOWSER_SHOCKWAVE_OUTER_RADIUS - inset);
        if (inner < o->oDistanceToMario && o->oDistanceToMario < outer) {
            s32 heavy = o->oBehParams2ndByte == BOWSER_SHOCKWAVE_HEAVY
                        || o->oBehParams2ndByte == BOWSER_SHOCKWAVE_DELAYED;
            mario_queue_bowser_impact_with_strength(o, heavy);
        }
    }
}
