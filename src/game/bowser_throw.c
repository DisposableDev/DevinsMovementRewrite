#include "sm64.h"
#include "bowser_throw.h"
#include "behavior_data.h"
#include "game_init.h"
#include "game_options.h"
#include "object_helpers.h"
#include "object_list_processor.h"
#include "engine/math_util.h"
#include "engine/surface_collision.h"

#define BOWSER_THROW_PREDICTION_FRAMES 120

struct BowserThrowRelease {
    s16 yaw, pitch, spin;
};

static struct {
    struct Object *heldObj;
    u32 frame;
    struct BowserThrowRelease release;
} sPreviousBowserThrow;

void bowser_throw_velocity(s16 pitch, s16 spin, f32 *forwardVel, f32 *velY) {
    f32 swingSpeed = spin / 3000.0f * 70.0f;
    if (swingSpeed < 0.0f) {
        swingSpeed = -swingSpeed;
    }
    if (swingSpeed > 90.0f) {
        swingSpeed *= 2.5f;
    }
    *forwardVel = coss(pitch) * swingSpeed;
    *velY = -sins(pitch) * swingSpeed;
}

struct Object *bowser_throw_nearest_mine(struct Object *bowser) {
    struct ObjectNode *head = &gObjectLists[OBJ_LIST_GENACTOR];
    struct Object *nearest = NULL;
    f32 nearestDistance = game_options_get(GO_BOWSER_FIGHTS) == 1 ? 800.0f : 600.0f;
    for (struct Object *mine = (struct Object *) head->next; mine != (struct Object *) head;
         mine = (struct Object *) mine->header.next) {
        if (obj_has_behavior(mine, bhvBowserBomb) && mine->activeFlags != ACTIVE_FLAG_DEACTIVATED
            && mine->oAction == BOWSER_BOMB_ACT_ARMED && mine->oIntangibleTimer == 0) {
            f32 distance = dist_between_objects(bowser, mine);
            if (distance < nearestDistance) {
                nearest = mine;
                nearestDistance = distance;
            }
        }
    }
    return nearest;
}

void bowser_update_spin(struct MarioState *m) {
    if (m->intendedMag > 20.0f) {
        if (game_options_cheat(GO_BOWSER_HOLD_SPIN)) {
            // Latch a direction for this hold; changing stick angle does not
            // cancel acceleration. Keep an existing spin's direction on entry.
            if (m->actionArg < 2) {
                s16 direction =
                    m->angleVel[1] != 0 ? m->angleVel[1] : (s16) (m->intendedYaw - m->faceAngle[1]);
                m->actionArg = direction < 0 ? 3 : 2;
            }
            m->angleVel[1] =
                CLAMP(m->angleVel[1] + (m->actionArg == 3 ? -0x80 : 0x80), -0x1000, 0x1000);
        } else if (m->actionArg == 0 || m->actionArg >= 2) {
            m->actionArg = 1;
            m->twirlYaw = m->intendedYaw;
        } else {
            s16 spin = (s16) (m->intendedYaw - m->twirlYaw) / 0x80;
            spin = CLAMP(spin, -0x80, 0x80);
            m->twirlYaw = m->intendedYaw;
            m->angleVel[1] += spin;
            m->angleVel[1] = CLAMP(m->angleVel[1], -0x1000, 0x1000);
        }
    } else {
        m->actionArg = 0;
        m->angleVel[1] = approach_s32(m->angleVel[1], 0, 0x40, 0x40);
    }
}

void bowser_throw_reset_history(void) {
    sPreviousBowserThrow.heldObj = NULL;
}

void bowser_throw_record(struct MarioState *m) {
    if (m->heldObj == NULL) {
        bowser_throw_reset_history();
        return;
    }
    sPreviousBowserThrow.heldObj = m->heldObj;
    sPreviousBowserThrow.frame = gGlobalTimer;
    sPreviousBowserThrow.release.yaw = m->faceAngle[1];
    sPreviousBowserThrow.release.pitch = m->heldObj->oBowserHeldAnglePitch;
    sPreviousBowserThrow.release.spin = m->heldObj->oBowserHeldAngleVelYaw;
}

// Run only movement on private objects: no actions, mine consumption or statistics.
static s32 bowser_throw_predict(struct MarioState *m, struct BowserThrowRelease *release) {
    f32 forwardVel, velY;
    bowser_throw_velocity(release->pitch, release->spin, &forwardVel, &velY);
    if (game_options_get(GO_BOWSER_FIGHTS) == 0 && ABS(forwardVel) < BOWSER_MINE_MIN_THROW_SPEED) {
        return FALSE;
    }
    struct Object flight = *m->heldObj;
    struct Object parent = *m->marioObj;
    parent.oPosX = m->pos[0];
    parent.oPosY = m->pos[1];
    parent.oPosZ = m->pos[2];
    parent.oMoveAngleYaw = release->yaw;
    flight.parentObj = &parent;
    struct Object *savedObject = gCurrentObject;
    s16 savedCollisionFlags = gCollisionFlags;
    gCurrentObject = &flight;
    cur_obj_get_thrown_or_placed(1.0f, 1.0f, BOWSER_ACT_THROWN);
    flight.oDragStrength = 10.0f;
    flight.oForwardVel = forwardVel;
    flight.oVelY = velY;
    cur_obj_update_floor_and_walls();
    cur_obj_move_standard(-78);
    s32 hit = FALSE;
    for (s32 frame = 0; frame < BOWSER_THROW_PREDICTION_FRAMES; frame++) {
        cur_obj_update_floor_and_walls();
        if (bowser_throw_nearest_mine(&flight) != NULL) {
            hit = TRUE;
            break;
        }
        // The real action checks mines before stopping on its first ground contact.
        if (flight.oMoveFlags & OBJ_MOVE_ON_GROUND) {
            break;
        }
        cur_obj_move_standard(-78);
        if (bowser_check_fallen_off_stage()) {
            break;
        }
    }
    gCurrentObject = savedObject;
    gCollisionFlags = savedCollisionFlags;
    return hit;
}

void bowser_throw_assist(struct MarioState *m) {
    if (!game_options_get(GO_BOWSER_THROW) || m->heldObj == NULL) {
        return;
    }
    struct BowserThrowRelease release = { m->faceAngle[1], m->heldObj->oBowserHeldAnglePitch,
                                          m->heldObj->oBowserHeldAngleVelYaw };
    if (bowser_throw_predict(m, &release)) {
        return;
    }
    s32 hit = FALSE;
    if (sPreviousBowserThrow.heldObj == m->heldObj
        && (u32) (gGlobalTimer - sPreviousBowserThrow.frame) == 1) {
        release = sPreviousBowserThrow.release;
        hit = bowser_throw_predict(m, &release);
    }
    if (!hit) {
        struct MarioState next = *m;
        bowser_update_spin(&next);
        release.yaw = next.faceAngle[1] + next.angleVel[1];
        release.pitch = -ABS(next.angleVel[1]);
        release.spin = next.angleVel[1];
        hit = bowser_throw_predict(m, &release);
    }
    if (hit) {
        m->faceAngle[1] = release.yaw;
        m->angleVel[1] = release.spin;
        m->heldObj->oBowserHeldAnglePitch = release.pitch;
        m->heldObj->oBowserHeldAngleVelYaw = release.spin;
    }
}
