// white_puff_explode.inc.c

void bhv_white_puff_exploding_loop(void) {
    f32 stepStartY = o->oPosY;

    if (o->oTimer == 0) {
        cur_obj_compute_vel_xz();
        o->oWhitePuffScaleX = o->header.gfx.scale[0];
        switch (o->oBehParams2ndByte) {
            case WHITE_PUFF_EXPLODE_BP_FAST_FADE:
                o->oOpacity = 254;
                o->oWhitePuffOpacityDiff = -21;
                o->oWhitePuffGrowWhileFading = 0;
                break;
            case WHITE_PUFF_EXPLODE_BP_VANISH_RING:
            case WHITE_PUFF_EXPLODE_BP_WATER_DOWN_IMPULSE:
                o->oOpacity = 254;
                o->oWhitePuffOpacityDiff =
                    o->oBehParams2ndByte == WHITE_PUFF_EXPLODE_BP_WATER_DOWN_IMPULSE ? -14 : -10;
                o->oWhitePuffGrowWhileFading = 1;
                break;
            case WHITE_PUFF_EXPLODE_BP_SLOW_FADE:
                o->oOpacity = 254;
                o->oWhitePuffOpacityDiff = -13;
                o->oWhitePuffGrowWhileFading = 1;
                break;
            case WHITE_PUFF_EXPLODE_BP_GROUND_POUND_IMPACT:
                o->oOpacity = 254;
                o->oWhitePuffOpacityDiff = -16;
                o->oWhitePuffGrowWhileFading = 1;
                break;
        }
    }

    if ((o->oBehParams2ndByte == WHITE_PUFF_EXPLODE_BP_GROUND_POUND_IMPACT && o->oTimer >= 15)
        || (o->oBehParams2ndByte == WHITE_PUFF_EXPLODE_BP_WATER_DOWN_IMPULSE && o->oTimer >= 18)) {
        obj_mark_for_deletion(o);
        return;
    }

    cur_obj_move_using_vel_and_gravity();
    cur_obj_apply_drag_xz(o->oDragStrength);

    if (o->oBehParams2ndByte == WHITE_PUFF_EXPLODE_BP_WATER_DOWN_IMPULSE) {
        struct Surface *floor;
        // Query from pre-step height so a falling puff can detect the floor it crossed.
        f32 floorHeight = find_floor(o->oPosX, stepStartY, o->oPosZ, &floor);
        if (floor != NULL && o->oPosY <= floorHeight) {
            obj_mark_for_deletion(o);
            return;
        }
    }

    if (o->oVelY > 100.0f) {
        o->oVelY = 100.0f;
    }

    if (o->oTimer > (o->oBehParams2ndByte == WHITE_PUFF_EXPLODE_BP_VANISH_RING ? 26 : 20)) {
        obj_mark_for_deletion(o);
    }

    if (o->oOpacity) {
        f32 scale;
        o->oOpacity += o->oWhitePuffOpacityDiff;
        if (o->oOpacity < 2) {
            obj_mark_for_deletion(o);
        }
        if (o->oWhitePuffGrowWhileFading) {
            scale = o->oWhitePuffScaleX * ((254 - o->oOpacity) / 254.0f);
        } else {
            scale = o->oWhitePuffScaleX * (o->oOpacity / 254.0f);
        }
        cur_obj_scale(scale);
    }
}
