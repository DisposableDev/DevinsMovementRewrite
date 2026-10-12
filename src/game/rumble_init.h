#ifndef RUMBLE_INIT_H
#define RUMBLE_INIT_H

#include <PR/ultratypes.h>

#include "config.h"

// Motor-on video interrupts per eight-interrupt cycle (the N64 motor is binary).
enum CharacterRumbleLevel {
    CHARACTER_RUMBLE_NONE = 0,
    CHARACTER_RUMBLE_FAINT = 1,
    CHARACTER_RUMBLE_LIGHT = 2,
    CHARACTER_RUMBLE_MEDIUM_LIGHT = 3,
    CHARACTER_RUMBLE_MEDIUM = 4,
    CHARACTER_RUMBLE_STRONG = 5,
    CHARACTER_RUMBLE_VERY_STRONG = 6,
    CHARACTER_RUMBLE_HEAVY = 8
};

#ifdef ENABLE_RUMBLE

extern s32 gRumblePakTimer;

void init_rumble_pak_scheduler_queue(void);
void block_until_rumble_pak_free(void);
s32 try_acquire_rumble_pak(void);
void release_rumble_pak_control(void);
void stop_rumble_for_reset(void);
void queue_rumble_data(s16 time, s16 level);
void set_character_movement_rumble(s32 level);
void queue_rumble_for_sound(s16 time, s16 level, u32 soundBits, f32 *source);
void cancel_sound_rumble(u32 soundBits, f32 *source);
void queue_rumble_decay(s16 decay);
u32  is_rumble_finished_and_queue_empty(void);
void reset_rumble_timers_slip(void);
void reset_rumble_timers_vibrate(s32 level);
void queue_rumble_submerged(void);
void cancel_rumble(void);
void create_thread_6(void);
void rumble_thread_update_vi(void);

#else

static inline void init_rumble_pak_scheduler_queue(void)                {}
static inline void block_until_rumble_pak_free(void)                    {}
static inline s32 try_acquire_rumble_pak(void) {
    return TRUE;
}
static inline void release_rumble_pak_control(void)                     {}
static inline void stop_rumble_for_reset(void) {
}
static inline void queue_rumble_data(UNUSED s16 time, UNUSED s16 level) {}
static inline void set_character_movement_rumble(UNUSED s32 level) {
}
static inline void queue_rumble_for_sound(UNUSED s16 time, UNUSED s16 level, UNUSED u32 soundBits,
                                          UNUSED f32 *source) {
}
static inline void cancel_sound_rumble(UNUSED u32 soundBits, UNUSED f32 *source) {
}
static inline void queue_rumble_decay(UNUSED s16 decay)                 {}
static inline u32  is_rumble_finished_and_queue_empty(void)             { return FALSE; }
static inline void reset_rumble_timers_slip(void)                       {}
static inline void reset_rumble_timers_vibrate(UNUSED s32 level)        {}
static inline void queue_rumble_submerged(void)                         {}
static inline void cancel_rumble(void)                                  {}
static inline void create_thread_6(void)                                {}
static inline void rumble_thread_update_vi(void)                        {}

#endif // ENABLE_RUMBLE

#endif // RUMBLE_INIT_H
