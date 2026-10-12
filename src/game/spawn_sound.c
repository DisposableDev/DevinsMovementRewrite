#include <PR/ultratypes.h>

#include "audio/external.h"
#include "area.h"
#include "behavior_data.h"
#include "camera.h"
#include "engine/behavior_script.h"
#include "engine/graph_node.h"
#include "engine/surface_collision.h"
#include "object_helpers.h"
#include "object_constants.h"
#include "object_list_processor.h"
#include "sm64.h"
#include "spawn_sound.h"
#include "rumble_init.h"
#include "config.h"

// The caller's table is indexed by the current object's animation sound state.
// The two frame triggers are checked independently, first then second.
void exec_anim_sound_state(struct SoundState *soundStates) {
    s32 stateIdx = gCurrentObject->oSoundStateID;

    // Animations without sound retain a disabled entry in the table.
    if (soundStates[stateIdx].playSound) {
        s32 animFrame;

        // A negative frame disables that trigger; tables conventionally use -1.
        if ((animFrame = soundStates[stateIdx].animFrame1) >= 0) {
            if (cur_obj_check_anim_frame(animFrame)) {
                cur_obj_play_sound_2(soundStates[stateIdx].soundMagic);
            }
        }

        if ((animFrame = soundStates[stateIdx].animFrame2) >= 0) {
            if (cur_obj_check_anim_frame(animFrame)) {
                cur_obj_play_sound_2(soundStates[stateIdx].soundMagic);
            }
        }
    }
}

// Audio retains a position pointer, so dying objects use a separate sound source.
// bhvSoundSpawner delays one playback request and later deactivates the source.
void create_sound_spawner(s32 soundMagic) {
    struct Object *obj = spawn_object(gCurrentObject, MODEL_NONE, bhvSoundSpawner);

    obj->oSoundEffectBits = soundMagic;
}

// BBH ambience may cross a doorway, but should not refresh from unrelated rooms.
static s32 cur_obj_bbh_sound_is_in_room(s32 soundMagic) {
    if (gCurrLevelNum != LEVEL_BBH) {
        return TRUE;
    }
    s32 sourceRoom;
    switch (soundMagic) {
        case SOUND_ENV_MERRY_GO_ROUND_CREAKING:
        case SOUND_AIR_BLOW_FIRE:
            sourceRoom = BBH_NEAR_MERRY_GO_ROUND_ROOM;
            break;
        case SOUND_ENV_ELEVATOR1:
        case SOUND_ENV_ELEVATOR2:
        case SOUND_ENV_ELEVATOR3:
        case SOUND_ENV_ELEVATOR4:
            sourceRoom = gCurrentObject->oRoom;
            break;
        default:
            return TRUE;
    }
    s32 marioRoom = gMarioCurrentRoom;
    if (marioRoom == BBH_DYNAMIC_SURFACE_ROOM) {
        marioRoom = get_room_at_pos(gMarioObject->oPosX, gMarioObject->oPosY, gMarioObject->oPosZ);
    }
    if (sourceRoom <= 0 || marioRoom <= 0) {
        return TRUE;
    }
    return sourceRoom == marioRoom || sourceRoom == gDoorAdjacentRooms[marioRoom].forwardRoom
           || sourceRoom == gDoorAdjacentRooms[marioRoom].backwardRoom;
}

// Reject selected distant requests before they enter the audio thread's queue.
// Measure height in world space so camera pitch cannot change the vertical gate.
static s32 cur_obj_sound_is_in_range(s32 soundMagic) {
    if (gCurrLevelNum == LEVEL_CASTLE_GROUNDS && soundMagic == SOUND_ENV_WATERFALL2) {
        return TRUE;
    }
    if (!cur_obj_bbh_sound_is_in_room(soundMagic)) {
        return FALSE;
    }
    f32 range;
    switch (soundMagic) {
        case SOUND_ENV_ELEVATOR1:
        case SOUND_ENV_ELEVATOR2:
        case SOUND_ENV_ELEVATOR3:
        case SOUND_ENV_ELEVATOR4:
            range = OBJECT_MOTOR_SOUND_DISTANCE;
            break;
        case SOUND_ENV_WATERFALL1:
        case SOUND_ENV_WATERFALL2:
        case SOUND_ENV_MOVINGSAND:
        case SOUND_ENV_DRONING1:
        case SOUND_ENV_DRONING2:
        case SOUND_ENV_MERRY_GO_ROUND_CREAKING:
            range = OBJECT_AMBIENT_SOUND_DISTANCE;
            break;
        case SOUND_AIR_AMP_BUZZ:
        case SOUND_AIR_BLOW_FIRE:
        case SOUND_ENV_WIND1:
            range = ENEMY_LOOP_SOUND_DISTANCE;
            break;
        case SOUND_OBJ_GOOMBA_WALK:
        case SOUND_OBJ_BOBOMB_WALK:
        case SOUND_OBJ_KOOPA_WALK:
        case SOUND_OBJ_KOOPA_THE_QUICK_WALK:
        case SOUND_OBJ_BULLY_WALK_SMALL:
        case SOUND_OBJ_BULLY_WALK_LARGE:
        case SOUND_OBJ_SKEETER_WALK:
            range = ENEMY_FOOTSTEP_SOUND_DISTANCE;
            break;
        default:
            return TRUE;
    }
    f32 *pos = gCurrentObject->header.gfx.pos;
    f32 dx = pos[0] - gLakituState.pos[0];
    f32 dy = pos[1] - gLakituState.pos[1];
    f32 dz = pos[2] - gLakituState.pos[2];
    f32 verticalRange = range * OBJECT_SOUND_VERTICAL_DISTANCE_SCALE;
    return dx * dx + dz * dz <= range * range && dy >= -verticalRange && dy <= verticalRange;
}

// Both wrappers use the current object's camera-relative position after shared
// render/range gates. The second also queues rumble for selected impact sounds.
void cur_obj_play_sound_1(s32 soundMagic) {
    if ((gCurrentObject->header.gfx.node.flags & GRAPH_RENDER_ACTIVE)
        && cur_obj_sound_is_in_range(soundMagic)) {
        play_sound(soundMagic, gCurrentObject->header.gfx.cameraToObject);
    }
}

void cur_obj_play_sound_2(s32 soundMagic) {
    if ((gCurrentObject->header.gfx.node.flags & GRAPH_RENDER_ACTIVE)
        && cur_obj_sound_is_in_range(soundMagic)) {
        play_sound(soundMagic, gCurrentObject->header.gfx.cameraToObject);
        if (soundMagic == SOUND_OBJ_BOWSER_WALK) {
            queue_rumble_data(3, 60);
        }
        if (soundMagic == SOUND_OBJ_POUNDING_LOUD) {
            queue_rumble_data(3, 60);
        }
        if (soundMagic == SOUND_OBJ_WHOMP) {
            queue_rumble_data(5, 80);
        }
    }
}

/*
 * These 2 functions below are completely unreferenced in all versions
 * of Super Mario 64. They are likely functions which facilitated
 * calculation of distance of an object to volume, since these are
 * common implementations of such a concept, and the fact they are
 * adjacent to other sound functions. The fact there are 2 functions
 * might show that the developers were testing several ranges, or certain
 * objects had different ranges, or had these for other unknown purposes.
 * Technically, these functions are only educated guesses. Trust these
 * interpretations at your own discretion.
 */
s32 calc_dist_to_volume_range_1(f32 distance) { // range from 60-124
    s32 volume;

    if (distance < 500.0f) {
        volume = 127;
    } else if (1500.0f < distance) {
        volume = 0;
    } else {
        volume = (((distance - 500.0f) / 1000.0f) * 64.0f) + 60.0f;
    }

    return volume;
}

s32 calc_dist_to_volume_range_2(f32 distance) { // range from 79.2-143.2
    s32 volume;

    if (distance < 1300.0f) {
        volume = 127;
    } else if (2300.0f < distance) {
        volume = 0;
    } else {
        volume = (((distance - 1000.0f) / 1000.0f) * 64.0f) + 60.0f;
    }

    return volume;
}
