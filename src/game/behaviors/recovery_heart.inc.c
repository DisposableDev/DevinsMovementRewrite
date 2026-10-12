// recovery_heart.inc.c

struct ObjectHitbox sRecoveryHeartHitbox = {
    /* interactType:      */ 0,
    /* downOffset:        */ 0,
    /* damageOrCoinValue: */ 0,
    /* health:            */ 0,
    /* numLootCoins:      */ 0,
    /* radius:            */ 50,
    /* height:            */ 50,
    /* hurtboxRadius:     */ 50,
    /* hurtboxHeight:     */ 50,
};

#define RECOVERY_HEART_MAX_SPIN_SPEED 11000 // Angle units per update.

void bhv_recovery_heart_loop(void) {
    obj_set_hitbox(o, &sRecoveryHeartHitbox);
    if (obj_check_if_collided_with_object(o, gMarioObject)) {
        if (o->oSpinningHeartPlayedSound == 0) {
            cur_obj_play_sound_2(SOUND_GENERAL_HEART_SPIN);
            o->oSpinningHeartPlayedSound++;
        }

        s32 contactSpinSpeed = (s32) (200.0f * absf(gMarioState->forwardVel)) + 1000;
        if (mario_ground_twirl_is_active(gMarioState)) {
            contactSpinSpeed = RECOVERY_HEART_MAX_SPIN_SPEED;
        }
        o->oAngleVelYaw = MAX(o->oAngleVelYaw, contactSpinSpeed);
    } else {
        o->oSpinningHeartPlayedSound = 0;

        if ((o->oAngleVelYaw -= 50) < 400) {
            o->oAngleVelYaw = 400;
            o->oSpinningHeartTotalSpin = 0;
        }
    }

    o->oAngleVelYaw = MIN(o->oAngleVelYaw, RECOVERY_HEART_MAX_SPIN_SPEED);
    if ((o->oSpinningHeartTotalSpin += o->oAngleVelYaw) >= 0x10000) {
        if (gMarioState->health >= 0x100) {
            gMarioState->healCounter += 4;
#ifdef BREATH_METER
            gMarioState->breathCounter += 4;
#endif
        }
        o->oSpinningHeartTotalSpin -= 0x10000;
    }

    o->oFaceAngleYaw += o->oAngleVelYaw;
}
