#include "game_options_input.h"

#include <ultra64.h>
#include <string.h>
#include "game_init.h"
#include "game_options.h"

struct OptionsInputState {
    s32 port;
    u16 blocked;
    u16 accepted;
    u16 ready;
    u16 carryHeld;
    u8 held[16];
    u8 released[16];
};

// These preferences form the input context; changing one clears pending holds.
struct OptionsInputSettings {
    u8 controller; // Saved selection: 0 is Automatic, otherwise port + 1.
    u8 debounceFrames;
    u8 minimumHoldFrames;
};

static struct OptionsInputState sInputs[MAX_NUM_PLAYERS];
static struct OptionsInputState sRecovery;
static struct Controller sRecoveryController;
static s32 sFile;
static s32 sMenu;
static s32 sDemo;
static s32 sPrimary;
static s32 sConnected;
static s32 sDisconnect;
static struct OptionsInputSettings sSettings;

void game_options_input_init(void) {
    memset(sInputs, 0, sizeof(sInputs));
    memset(&sRecovery, 0, sizeof(sRecovery));
    memset(&sRecoveryController, 0, sizeof(sRecoveryController));
    for (s32 i = 0; i < MAX_NUM_PLAYERS; i++) {
        sInputs[i].port = -2;
    }
    sRecovery.port = -2;
    sFile = -2;
    sMenu = sDemo = -1;
    sPrimary = -1;
    sConnected = sDisconnect = 0;
    memset(&sSettings, 0, sizeof(sSettings));
}

static s32 first_connected(u8 bits) {
    for (s32 port = 0; port < MAXCONTROLLERS; port++) {
        if (bits & (1 << port)) {
            return port;
        }
    }
    return -1;
}

static void link_controller(struct Controller *controller, s32 port) {
    controller->port = port;
    controller->controllerData = port >= 0 ? &gControllerPads[port] : NULL;
    controller->statusData = port >= 0 ? &gControllerStatuses[port] : NULL;
}

void game_options_input_select(s32 menu, s32 demo) {
    u8 bits = 0;
    struct OptionsInputSettings settings = {
        .controller = game_options_get(GO_CONTROLLER),
        .debounceFrames = game_options_get(GO_DEBOUNCE),
        .minimumHoldFrames = game_options_get(GO_MIN_HOLD),
    };
    s32 file = game_options_active_file();
    s32 contextChanged = file != sFile || demo != sDemo || settings.controller != sSettings.controller
                         || settings.debounceFrames != sSettings.debounceFrames
                         || settings.minimumHoldFrames != sSettings.minimumHoldFrames;
    s32 changed = contextChanged || menu != sMenu;
    for (s32 port = 0; port < MAXCONTROLLERS; port++) {
        if (gControllerPads[port].errno == 0) {
            bits |= 1 << port;
        }
    }
    gControllerBits = bits;
    game_options_set_connected_controllers(bits);
    s32 first = first_connected(bits);
    if (!demo && settings.controller && !(bits & (1 << (settings.controller - 1)))) {
        // Follow a connected pad instead of retaining an unavailable explicit port.
        // The physical port change still quarantines held input below.
        settings.controller = first + 1;
        game_options_set(GO_CONTROLLER, settings.controller);
    }
    s32 primary = settings.controller ? settings.controller - 1 : first;
    s32 connected = primary >= 0 && (bits & (1 << primary));
    // A held before a dialogue appears must reach its held-input handler. Keep
    // the hold distinct from a fresh confirmation and from attachment changes.
    u16 carryHeld = !contextChanged && !demo && connected && (menu & GAME_OPTIONS_INPUT_DIALOGUE)
                            && sInputs[0].port == primary && !(sInputs[0].blocked & A_BUTTON)
                            && ((gControllers[0].buttonDown & A_BUTTON) || sInputs[0].held[15] != 0)
                        ? A_BUTTON
                        : 0;
    if (!changed && sConnected && !(bits & (1 << sPrimary)) && !menu && !demo && file >= 0
        && game_options_get(GO_PAUSE_DISCONNECT)) {
        sDisconnect = 1;
    }
    if (changed) {
        for (s32 i = 0; i < MAX_NUM_PLAYERS; i++) {
            sInputs[i].port = -2;
            sInputs[i].carryHeld = 0;
        }
        sInputs[0].carryHeld = carryHeld;
        sRecovery.port = -2;
        sDisconnect = 0;
    }
    sSettings = settings;
    sFile = file;
    sMenu = menu;
    sDemo = demo;
    sPrimary = primary;
    sConnected = connected;
    // Attract demos own a virtual pad even if no physical controller is attached.
    s32 port = demo ? 0 : connected ? primary : menu ? first : -1;
    link_controller(&gControllers[0], port);
    link_controller(&sRecoveryController, first);
    bits &= port >= 0 ? ~(1 << port) : 0xFF;
    for (s32 i = 1; i < MAX_NUM_PLAYERS; i++) {
        port = first_connected(bits);
        link_controller(&gControllers[i], port);
        if (port >= 0) {
            bits &= ~(1 << port);
        }
    }
}

static u16 physical_buttons(struct Controller *controller) {
    OSContPadEx *pad = controller->controllerData;
    u16 buttons = pad->button;
    if ((controller->statusData->type & CONT_CONSOLE_MASK) == CONT_CONSOLE_GCN) {
        buttons &= ~(Z_TRIG | L_TRIG);
        if (pad->button & Z_TRIG) {
            buttons |= L_TRIG;
        }
        if (pad->l_trig > 85) {
            buttons |= Z_TRIG;
        }
    }
    return buttons;
}

static u16 filter_buttons(struct OptionsInputState *state, u16 raw, s32 menu) {
    u16 result = raw & START_BUTTON;
    s32 debounce = !menu && sSettings.debounceFrames ? sSettings.debounceFrames : 1;
    s32 minimumHold = !menu && sSettings.minimumHoldFrames ? sSettings.minimumHoldFrames : 1;
    for (s32 bit = 0; bit < 16; bit++) {
        u16 mask = 1 << bit;
        if (mask == START_BUTTON) {
            continue;
        }
        if (raw & mask) {
            state->released[bit] = 0;
            if (state->ready & mask) {
                if (state->held[bit] < minimumHold) {
                    state->held[bit]++;
                }
                if (state->held[bit] >= minimumHold) {
                    state->accepted |= mask;
                    state->ready &= ~mask;
                }
            }
            result |= state->accepted & mask;
        } else {
            state->held[bit] = 0;
            state->accepted &= ~mask;
            if (state->released[bit] < debounce) {
                state->released[bit]++;
            }
            if (state->released[bit] >= debounce) {
                state->ready |= mask;
            }
        }
    }
    return result;
}

static void read_controller(struct Controller *controller, struct OptionsInputState *state, s32 menu,
                            s32 demo) {
    OSContPadEx *pad = controller->controllerData;
    u16 buttons = 0;
    if (pad != NULL && (pad->errno == 0 || demo)) {
        buttons = demo ? pad->button : physical_buttons(controller);
        controller->rawStickX = pad->stick_x;
        controller->rawStickY = pad->stick_y;
    } else {
        controller->rawStickX = controller->rawStickY = 0;
    }
    if (state->port != controller->port) {
        u16 carryHeld = state->carryHeld & buttons;
        memset(state, 0, sizeof(*state));
        state->port = controller->port;
        state->blocked = demo ? 0 : buttons & ~carryHeld;
        state->ready = 0xFFFF;
        controller->buttonDown = carryHeld;
    }
    state->blocked &= buttons;
    buttons &= ~state->blocked;
    buttons = demo ? buttons : filter_buttons(state, buttons, menu);
    controller->buttonPressed = buttons & ~controller->buttonDown;
    controller->buttonReleased = controller->buttonDown & ~buttons;
    controller->buttonDown = buttons;
    adjust_analog_stick(controller);
}

void game_options_input_read(s32 menu, s32 demo) {
    for (s32 i = 0; i < MAX_NUM_PLAYERS; i++) {
        read_controller(&gControllers[i], &sInputs[i], menu, demo && i == 0);
    }
    read_controller(&sRecoveryController, &sRecovery, TRUE, FALSE);
    if (!menu && !demo && !sConnected) {
        // A missing explicit port can still reach pause/settings. No action input
        // from the recovery controller reaches gameplay.
        gControllers[0].buttonDown |= sRecoveryController.buttonDown & START_BUTTON;
        gControllers[0].buttonPressed |= sRecoveryController.buttonPressed & START_BUTTON;
    }
    if (game_options_input_take_disconnect()) {
        // Use the ordinary pause gate so dialogue, warps and special-room locks
        // reject this request in exactly the same way as a physical Start press.
        gControllers[0].buttonPressed |= START_BUTTON;
    }
}

s32 game_options_input_take_disconnect(void) {
    s32 pending = sDisconnect && game_options_get(GO_PAUSE_DISCONNECT);
    sDisconnect = 0;
    return pending;
}
