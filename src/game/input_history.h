#ifndef INPUT_HISTORY_H
#define INPUT_HISTORY_H

#include <PR/ultratypes.h>

#define INPUT_HISTORY_COUNT_TEXT_SIZE 11
#define INPUT_HISTORY_LENGTH 15
#define INPUT_HISTORY_CHANNELS 15
#define INPUT_STICK_NEUTRAL 8

enum InputHistoryChannel {
    INPUT_STICK,
    INPUT_D_UP,
    INPUT_D_RIGHT,
    INPUT_D_DOWN,
    INPUT_D_LEFT,
    INPUT_B,
    INPUT_L,
    INPUT_START,
    INPUT_A,
    INPUT_C_UP,
    INPUT_C_RIGHT,
    INPUT_C_DOWN,
    INPUT_C_LEFT,
    INPUT_Z,
    INPUT_R,
};

struct InputHistoryEntry {
    // Consecutive sampled frames per channel, carried across changes in other inputs.
    u32 held[INPUT_HISTORY_CHANNELS];
    u32 neutralFrames;
    u16 buttons;
    u8 sector;
};

struct InputHistory {
    // Populated entries are newest first; entry zero is the live sample.
    struct InputHistoryEntry entries[INPUT_HISTORY_LENGTH];
    u8 length;
};

extern const u16 gInputHistoryButtons[INPUT_HISTORY_CHANNELS];

void input_history_reset(struct InputHistory *history);
// One sample advances held/neutral counts by one frame; counters saturate at u32 max.
void input_history_update(struct InputHistory *history, u16 buttons, u8 sector);
// channels must hold INPUT_HISTORY_CHANNELS entries; returns the populated count.
u8 input_history_channels(const struct InputHistoryEntry *entry, u8 *channels);
// Writes the exact unsigned count and terminator into INPUT_HISTORY_COUNT_TEXT_SIZE bytes.
void input_history_format_count(u32 frames, char *text);

#endif
