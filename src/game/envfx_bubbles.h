#ifndef ENVFX_BUBBLES_H
#define ENVFX_BUBBLES_H

#include <PR/ultratypes.h>
#include <PR/gbi.h>

// Jet particles are drawn in groups of five.
#define JRB_JETSTREAM_BUBBLE_COUNT 30
// Timing limits in gameplay updates.
#define JRB_JETSTREAM_BUBBLE_LIFETIME 24
#define JRB_JETSTREAM_BUBBLE_LIFETIME_JITTER 5
#define JRB_JETSTREAM_BUBBLE_STARTUP_DELAY 6
#define JRB_JETSTREAM_BUBBLE_RESPAWN_DELAY 3

enum EnvfxBubblesState {
    ENVFX_STATE_UNUSED,
    // Whirlpool / jet-stream bubble source position
    ENVFX_STATE_SRC_X,
    ENVFX_STATE_SRC_Y,
    ENVFX_STATE_SRC_Z,
    // only for whirlpool, where bubbles get sucked in
    ENVFX_STATE_DEST_X,
    ENVFX_STATE_DEST_Y,
    ENVFX_STATE_DEST_Z,
    ENVFX_STATE_PARTICLECOUNT,
    // Whirlpool orientation around DEST: placed pitch and roll (legacy YAW slot).
    ENVFX_STATE_PITCH,
    ENVFX_STATE_YAW
};

// Shared whirlpool/jet-stream emitter configuration; bubble initialization clears it.
extern s16 gEnvFxBubbleConfig[10];
Gfx *envfx_update_bubbles(s32 mode, Vec3s marioPos, Vec3s camTo, Vec3s camFrom);

#endif // ENVFX_BUBBLES_H
