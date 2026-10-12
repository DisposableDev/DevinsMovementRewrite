#ifndef BEHAVIOR_SCRIPT_H
#define BEHAVIOR_SCRIPT_H

#include <PR/ultratypes.h>

#define ENEMY_DRAW_DISTANCE_EXIT_SCALE 1.25f

enum BhvProc {
    BHV_PROC_CONTINUE,
    BHV_PROC_BREAK
};

#define cur_obj_get_int(offset) gCurrentObject->OBJECT_FIELD_S32(offset)
#define cur_obj_get_float(offset) gCurrentObject->OBJECT_FIELD_F32(offset)

#define cur_obj_add_float(offset, value) gCurrentObject->OBJECT_FIELD_F32(offset) += (f32)(value)
#define cur_obj_set_float(offset, value) gCurrentObject->OBJECT_FIELD_F32(offset) = (f32)(value)
#define cur_obj_add_int(offset, value) gCurrentObject->OBJECT_FIELD_S32(offset) += (s32)(value)
#define cur_obj_set_int(offset, value) gCurrentObject->OBJECT_FIELD_S32(offset) = (s32)(value)
#define cur_obj_or_int(offset, value)  gCurrentObject->OBJECT_FIELD_S32(offset) |= (s32)(value)
#define cur_obj_and_int(offset, value) gCurrentObject->OBJECT_FIELD_S32(offset) &= (s32)(value)
#define cur_obj_set_vptr(offset, value) gCurrentObject->OBJECT_FIELD_VPTR(offset) = (void *)(value)

#define obj_and_int(object, offset, value) object->OBJECT_FIELD_S32(offset) &= (s32)(value)

// Mario distance and the resulting drawing limit are in world units.
f32 cur_obj_drawing_distance(f32 distanceFromMario);
// Refresh drawing-distance/room visibility without advancing behavior or animation.
void cur_obj_update_sleeping_visibility(void);
void cur_obj_update(void);

#endif // BEHAVIOR_SCRIPT_H
