#include <ultra64.h>

#include "config.h"
#include "input_history.h"

#ifdef INPUT_DISPLAY

const u16 gInputHistoryButtons[INPUT_HISTORY_CHANNELS] = {
    [INPUT_STICK] = 0,           [INPUT_D_UP] = U_JPAD,        [INPUT_D_RIGHT] = R_JPAD,
    [INPUT_D_DOWN] = D_JPAD,     [INPUT_D_LEFT] = L_JPAD,      [INPUT_B] = B_BUTTON,
    [INPUT_L] = L_TRIG,          [INPUT_START] = START_BUTTON, [INPUT_A] = A_BUTTON,
    [INPUT_C_UP] = U_CBUTTONS,   [INPUT_C_RIGHT] = R_CBUTTONS, [INPUT_C_DOWN] = D_CBUTTONS,
    [INPUT_C_LEFT] = L_CBUTTONS, [INPUT_Z] = Z_TRIG,           [INPUT_R] = R_TRIG,

};

static u32 input_history_increment(u32 frames) {
    return frames == 0xFFFFFFFFu ? frames : frames + 1;
}

void input_history_reset(struct InputHistory *history) {
    *history = (struct InputHistory){ 0 };
    history->entries[0].sector = INPUT_STICK_NEUTRAL;
}

void input_history_update(struct InputHistory *history, u16 buttons, u8 sector) {
    struct InputHistoryEntry next = history->entries[0];
    u32 i;
    s32 changed;

    buttons &= A_BUTTON | B_BUTTON | Z_TRIG | START_BUTTON | U_JPAD | D_JPAD | L_JPAD | R_JPAD | L_TRIG
               | R_TRIG | U_CBUTTONS | R_CBUTTONS | D_CBUTTONS | L_CBUTTONS;
    if (sector > INPUT_STICK_NEUTRAL) {
        sector = INPUT_STICK_NEUTRAL;
    }
    changed = history->length && (next.buttons != buttons || next.sector != sector);

    for (i = 0; i < INPUT_HISTORY_CHANNELS; i++) {
        s32 held =
            i == INPUT_STICK ? sector != INPUT_STICK_NEUTRAL : (buttons & gInputHistoryButtons[i]) != 0;
        if (!held) {
            next.held[i] = 0;
        } else if (i == INPUT_STICK && next.sector != sector) {
            next.held[i] = 1;
        } else {
            next.held[i] = input_history_increment(next.held[i]);
        }
    }
    next.neutralFrames =
        buttons == 0 && sector == INPUT_STICK_NEUTRAL ? input_history_increment(next.neutralFrames) : 0;
    next.buttons = buttons;
    next.sector = sector;

    // Freeze the pre-change sample; the updated sample becomes the new head.
    if (changed) {
        for (i = INPUT_HISTORY_LENGTH - 1; i > 0; i--) {
            history->entries[i] = history->entries[i - 1];
        }
        if (history->length < INPUT_HISTORY_LENGTH) {
            history->length++;
        }
    } else if (history->length == 0) {
        history->length = 1;
    }
    history->entries[0] = next;
}

u8 input_history_channels(const struct InputHistoryEntry *entry, u8 *channels) {
    u8 channel;
    u8 count = 0;

    for (channel = 0; channel < INPUT_HISTORY_CHANNELS; channel++) {
        if (entry->held[channel] != 0) {
            channels[count++] = channel;
        }
    }
    return count;
}

void input_history_format_count(u32 frames, char *text) {
    u8 length = 0;
    char reversed[10];

    do {
        reversed[length++] = '0' + frames % 10;
        frames /= 10;
    } while (frames != 0);
    while (length != 0) {
        *text++ = reversed[--length];
    }
    *text = '\0';
}

#endif
