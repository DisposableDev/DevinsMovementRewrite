#ifndef SPAWN_SOUND_H
#define SPAWN_SOUND_H

#include <PR/ultratypes.h>

struct SoundState {
    s16 playSound; // Nonzero enables this table entry.
    s8 animFrame1; // Independent animation-frame triggers; a negative value disables one.
    s8 animFrame2;
    s32 soundMagic; // Packed SOUND_* value, including bank and priority.
};

// Request playback at gCurrentObject's camera-relative position, subject to its
// render-active flag and selected sound range/room gates. Continuous sounds need refresh.
void cur_obj_play_sound_1(s32 soundMagic);
// Same gates and source as _1, with rumble for selected impact sounds.
void cur_obj_play_sound_2(s32 soundMagic);
// Copy gCurrentObject's position into a separate source. bhvSoundSpawner owns the
// delayed playback request and later deactivation; the parent can be deleted meanwhile.
void create_sound_spawner(s32 soundMagic);
// Caller supplies a table covering gCurrentObject->oSoundStateID; there is no bounds check.
// Enabled triggers independently request _2 playback on exact animation-frame matches.
void exec_anim_sound_state(struct SoundState *soundStates);

#endif // SPAWN_SOUND_H
